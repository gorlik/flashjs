# flashjs
USB Flash utility for Freescale/NXP S08JS MCU

flashjs is a command line flash update utility for freescale S08JS
USB microcontroller using the freescale ROM USB bootloader
flashjs has ben tested in linux i386 and windows, but should work on any
platform supported by libusb.

Features
------------------
Not many features, just the bare minumum.
- read S19 files
- mass erase (restore to factory new condition)
- partial erase (preserve the configuration block)
- flash program/upgrade (upgrade will skip any data in the configuration block)
- check the CRC of a flashed image
- reboot the MCU 

Building flashjs
----------------------------
Linux requirements:
libusb-1.0-dev
GNU make
gcc

to build type "make". If you have mingw32 cross development tools installed
you can build the windows version typing "make win"

Building on windows should be fairly simple, but untested. A precompiled win32 binary 
can be found in the bin directory.

libusb-1.0 for windows is still in development stage and there are known issues with 
version pbr301 to pbr311. The binaries are statically linked to version pbr299.
Using flashjs
----------------------------
erasing the flash:
flashjs m
mass erase teh MCU. This will restore the MCU to factory new conditions

flashjs e
partial erase. This will preserve the configuration blocks

programming a firmware:
flashjs p file.s19
program file.s19 into the MCU. Should be used only after a mass erase command or a brand new device

flashjs u file.s19
program file.s19 into the MCU (automatically skips data in the configuration block).

CRC check:
flashjs c
uses the MCU internal CRC generator to check if the image was flashed successfully

rebooting:
flashjs x
Reboot the MCU. If the flashed image is valid the MCU will execute
the firmware instead of the bootloader.


TODO
-------
- write a gui version
