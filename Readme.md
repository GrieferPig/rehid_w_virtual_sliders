# Rehid - Saves your broken 3DS sliders!

**TL;DR: download the zip file from release and unzip it to your sd card root. Use START+L/R to adjust volume, SELECT+L/R to adjust 3D**

You don't need **rehidhelper** for this, and do not use that to update rehid module, it will overwrite this module with the original one which does not have adjustable virtual sliders

---

**Although Rehid is used for remapping buttons on 3DS, I figured out that it can also use as a macro to react to a key combo**

**This fork added mapping support for both sliders. Add and modify this in your rehid json config file to fit your preference**
```Json
{
    "keys": [
        {
            "get": "VOLUP",
            "press": "START+R"
        },
        {
            "get": "VOLDN",
            "press": "START+L"
        },
        {
            "get": "3DUP",
            "press": "SELECT+R"
        },
        {
            "get": "3DDN",
            "press": "SELECT+L"
        }
    ]
}
```
**Note that because I am lazy and these "virtual function keys" (VOLUP, VOLDN, etc. above) are only applicable to key-to-key mappings. That said, I don't see using circle pad and touch screen to adjust volume is really the best way to do that**

shamelessly copied from Luma3ds, thanks PabloMK7 & profi200

---

HID module rewrite with the aim of easier button remapping and more.

## How To Use

You need to have the latest luma for this to work correctly.

* Download `rehidhelper.cia` from the latest release
* Launch the app from home menu
* Click `A` on the `Download Rehid` option. Your 3DS will automatically reboot after this step.
* Go to https://mikahjc.github.io/3dsRemapBuilder/config and generate your remappings.
* Run rehidhelper again and select the Scan QR code option
* Scan your QR.

## How To Remap Buttons

You first need to create a `rehid.json` file with the remappings you want. For eg:-
```Json
{
    "keys":[
        {"get":"R", "press":"ZR"},
        {"get":"L", "press":"ZL"}
    ]
}
```
With the above, everytime you press `ZR` key, `R` key would be triggered,

and everytime you press `ZL` key, `L` key would be triggered.

It is also possible to do custom key combos, i.e.,
```Json
{
    "keys":[
        {"get":"R", "press":"X+Y"},
        {"get":"L+R", "press":"SELECT"}
    ]
}
```
Now everytime you press `X+Y`, `R` key would be triggered and on pressing `SELECT` button, both `L` and `R` would be triggered.

Possible Keys are:-
`A`, `B`, `X`, `Y`, `SELECT`, `START`, `ZL`, `ZR`, `L`, `R`, `LEFT`, `RIGHT`, `UP`, `DOWN`, `CRIGHT`(CPAD), `CLEFT`(CPAD), `CUP`(CPAD), `CDOWN`(CPAD)

Copy your `rehid.json` file to the `rehid` folder.

### Per Title Button Remapping

It is possible to have different button remapings for different titles:-

Inside the `rehid` folder, create a folder with the titleid as the folder name.

You can use [this](https://hax0kartik.github.io/3dsdb/) to find the titleid for your game.

Copy the `rehid.json` file inside this folder.

## Compilation
Get devkitpro, ctrulib and makerom and then `make -j` to compile.

## Credits

@luigoalma Help, testing and listening to my rants.

Druivensap on my discord server for helping me test out.

Luma3ds devs and contributors
