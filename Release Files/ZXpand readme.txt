ZXpand Support

In real life this add-on for the ZX81 provides 32K of RAM and SD card reading support, controlled through a
patched ROM. In EightyOne you can enable this board form the hardware menu by checking the ZXpand option.

Files placed in the folder '' are treated as a virtual SD card.

Press Z to get the CAT command in order to list the files on the emulated SD card. CAT "directory name" will
show files within a subdirectory.

The LOAD command is patched so files will be loaded from the virtual SD card rather than the tape system.
On a real ZXpand typing LOAD "" will invoke the tape loader and allow files to be loaded that way. Under
EightyOne emulation however this does not work due to the way that the emulator handles the ROM patching.

For full ZXpand instructions please see the online documentation:
https://github.com/charlierobson/ZXpand-Vitamins/wiki/ZXpand---Online-Manual


<3 SirMorris
