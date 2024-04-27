#include <3ds.h>
#include "mcuhid.hpp"
#include <cstring>
Handle mcuHidHandle;

u16 _3SliderVal = 0;
u16 volSliderVal = 0;

// 3dsident shows volume change, but the home menu does not
// ig apps reads volume value from places other than mcuhid (why is there 3 getVolume functions?)
// shamelessly copied from Luma3ds, thanks PabloMK7 & profi200
static Result applyCodecVol(void)
{
    // Credit: profi200
    u8 tmp;
    Result res = cdcChkInit();

    float currVolumeSliderOverride = volSliderVal;
    currVolumeSliderOverride = (currVolumeSliderOverride * 100) / 63;

    if (R_SUCCEEDED(res))
        res = CDCCHK_ReadRegisters2(0, 116, &tmp, 1); // CDC_REG_VOL_MICDET_PIN_SAR_ADC
    if (currVolumeSliderOverride >= 0)
        tmp &= ~0x80;
    else
        tmp |= 0x80;
    if (R_SUCCEEDED(res))
        res = CDCCHK_WriteRegisters2(0, 116, &tmp, 1);

    if (currVolumeSliderOverride >= 0)
    {
        s8 calculated = -128 + (((float)currVolumeSliderOverride / 100.f) * 108);
        if (calculated > -20)
            res = -1;                             // Just in case
        s8 volumes[2] = {calculated, calculated}; // Volume in 0.5 dB steps. -128 (muted) to 48. Do not go above -20 (100%).
        if (R_SUCCEEDED(res))
            res = CDCCHK_WriteRegisters2(0, 65, volumes, 2); // CDC_REG_DAC_L_VOLUME_CTRL, CDC_REG_DAC_R_VOLUME_CTRL
    }

    cdcChkExit();
    return res;
}

// not the ideal way to debounce but eh, it works
uint32_t debounce_cnt_1 = 0;
uint32_t debounce_cnt_2 = 0;
uint32_t debounce_cnt_3 = 0;
uint32_t debounce_cnt_4 = 0;

#define debounce_time 40

void decrementSliderVal()
{
    if (debounce_cnt_1 < debounce_time)
    {
        debounce_cnt_1++;
        return;
    }
    debounce_cnt_1 = 0;
    if (_3SliderVal < 0x20)
    {
        _3SliderVal = 0;
    }
    else
    {
        _3SliderVal -= 0x20;
    }
}

void incrementSliderVal()
{
    if (debounce_cnt_2 < debounce_time)
    {
        debounce_cnt_2++;
        return;
    }
    debounce_cnt_2 = 0;
    if (_3SliderVal + 0x20 > 0xff)
    {
        _3SliderVal = 0xff;
    }
    else
    {
        _3SliderVal += 0x20;
    }
}

void decrementVolSliderVal()
{
    if (debounce_cnt_3 < debounce_time)
    {
        debounce_cnt_3++;
        return;
    }
    debounce_cnt_3 = 0;
    if (volSliderVal < 8)
    {
        volSliderVal = 0;
    }
    else
    {
        volSliderVal -= 8;
    }
    // couldn't handle bad results in any meaningful way anyway, just ignore it
    applyCodecVol();
}

void incrementVolSliderVal()
{
    if (debounce_cnt_4 < debounce_time)
    {
        debounce_cnt_4++;
        return;
    }
    debounce_cnt_4 = 0;
    if (volSliderVal + 8 > 63)
    {
        volSliderVal = 63;
    }
    else
    {
        volSliderVal += 8;
    }
    applyCodecVol();
}

Result mcuHidInit()
{
    Result ret = 0;
    ret = srvGetServiceHandle(&mcuHidHandle, "mcu::HID");
    return ret;
}

void mcuHidExit()
{
    svcCloseHandle(mcuHidHandle);
}

Result mcuHidEnableAccelerometer(u8 enable)
{
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x10040;
    cmdbuf[1] = enable;
    Result ret = svcSendSyncRequest(mcuHidHandle);
    if (R_FAILED(ret))
        return ret;
    return cmdbuf[1];
}

Result mcuHidReadAccelerometerValues(AccelerometerEntry *entry)
{
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x60000;
    Result ret = svcSendSyncRequest(mcuHidHandle);
    if (R_FAILED(ret))
        return ret;
    memcpy(entry, &cmdbuf[2], 6);
    return cmdbuf[1];
}

Result mcuHidGetSliderState(u8 *rawvalue)
{
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x70000;
    Result ret = svcSendSyncRequest(mcuHidHandle);
    if (R_FAILED(ret))
        return ret;
    // *rawvalue = cmdbuf[2] & 0xFF;
    *rawvalue = _3SliderVal & 0xFF;
    return cmdbuf[1];
}

Result mcuHidGetAccelerometerEventHandle(Handle *handle)
{
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0xC0000;
    Result ret = svcSendSyncRequest(mcuHidHandle);
    if (R_FAILED(ret))
        return ret;
    *handle = cmdbuf[3];
    return cmdbuf[1];
}

Result mcuHidGetEventReason(uint32_t *reason)
{
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0xD0000;
    Result ret = svcSendSyncRequest(mcuHidHandle);
    if (R_FAILED(ret))
        return ret;
    *reason = cmdbuf[2];
    return cmdbuf[1];
}

Result mcuHidGetSoundVolume(uint8_t *volume)
{
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0xE0000;
    Result ret = svcSendSyncRequest(mcuHidHandle);
    if (R_FAILED(ret))
        return ret;
    // *volume = cmdbuf[2] & 0xFF;
    *volume = volSliderVal & 0xFF;
    return cmdbuf[1];
}

Result mcuHidEnableAccelerometerInterrupt(u8 enable)
{
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0xF0040;
    cmdbuf[1] = enable;
    Result ret = svcSendSyncRequest(mcuHidHandle);
    if (R_FAILED(ret))
        return ret;
    return cmdbuf[1];
}