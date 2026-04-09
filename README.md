# TULIP-DevBoard and TULIP-Module
README for the TULIP4041 DevBoard and Module.

![TULIP_small](https://github.com/user-attachments/assets/48ce1872-c028-45d0-808b-75ce184649b9)



To order, contact me directly with your email address to receive an order form.

The firmware binaries are now in the Firmware directory. The firmware files are:
   - tulip4041_module.uf2      For the TULIP Module
   - tulip4041_devboard.uf2    For the TULIP DevBoard
DO NOT mix up the firmware files, some hardware I/O and memory sized are different. The previous firmware files are available as well in order to downgrade in case of issues

The MOD files are now in their own subdirectory with explanation and instructions for using HEPAX RAM.

My favourite community for HP calculators is https://www.hpmuseum.org/forum/index.php, there are some TULIP related threads.

***This is a BIG firmware change. Most important is to initialize the FRAM according to the instructions below*** (text in bold and italic are CLI commands)
- Be aware that you will lose all settings and the rom map, sorry
- Step 1: unplug all everything, ensure that cat is empty: ***unplug ALL***
- Step 2: use the command ***fram NUKEALL*** to erase all FRAM
- Step 3: use the command ***fram INIT*** to initialize the FRAM file system
- Step 4: use the ***list*** command to verify that the file system is created with some standard files
- Step 6: enjoy the new firmware. To use the QROM features, use ***import [filename] qrom*** to import files from the uSD card in the QROM (FRAM) area, and plug these files in a Page. Filenames must be unique, you cannot have files with the same name in both FLASH and FRAM
- The User Manual and Background document are both updated

VERSION 0.97 BETA, March 2026
-   Changed handling of FI in the tracer. Instead of tracing the full FI line (64 bits) the flags
    are now compressed in a single 16-bit word, reducing a traceline to 40 bytes instead of 44
-   Added m_emu_flags to the CPage struct to assist in disabling emulation when unplugging a Page
    (but not implemented yet)
-   Removed the CPU temperature from the system overview, the value is inaccurate without calibration 
-   Implemented User Memory (1..4 Memory Modules) and the umem command
-   Re-implemented Extended Memory including the XFunctions Memory and the xmem command
-   Added an asterisk to the tracer selected RAM register if it is inside the TULIP emulation
-   Added the trace filter size to the system overview (trace filters not yet implemented)
-   Adjusted the IR clock rate to the updated clock speed of 150MHz (IR clock was actually too fast)
    irout state machine clock divider is now a parameter
-   Adjusted the IR clock speed with the CPU clock speed change when removing USB power (IR clock was too slow and printing did not work with USB power removed)
-   Implemented the FRAM file system
-   Implemented QROM support and the qrom command
-   Implemented HEPAX and HEPRAM support and the hepram command
-   Implemented the export command to save files (MOD and ROM) from FRAM to the uSD card
-   Added the cd command to change or show the current working directory on the uSD card
-   CLI now supports ASCI characters >0x7F, useful when entering names in the Owner String
    the terminal emulation must be set accordingly (usually to UTF-* support)
-   Added HP41 cycle frequency from the last RUN state to the system output

VERSION 0.96 BETA, December 2025
-   Added rudimentary power management. The RP2350 CPU clock is now 150MHz when USB power is connected 
    and switches to 75MHz when powered by the HP41. This reduces the TULIP power consumption from ~20mA to ~14mA
    Switchover is fully automatic and keeps the UART and SPI clocks at the same speed
    The TULIP will still be running at all times, further power management to be implemented
    Modified the PIO state machines running at 12.5MHz to have a shorter delay to support 2* speedup calculators
-   Peripheral clock speed is increased to 150 MHz, this allows the FRAM SPI speed to be increased to 40MHZ
    This speed helps implementing handling of the READ instructions as 16 registers can now be cached with an HP41 SYNC-SYNC cycle
-   Fixed an issue when plugging an embedded ROM fails to save the emulation status to FRAM
-   Partial implemented the FRAM File System, this is not yet used
-   Implemented Wand emulation and the commands wand and w, updated documentation
    Wand emulation is enabled when plugging the Wand MOD file

VERSION 0.95 BETA 3, November 2025
-   Added Page 4 bankswitching to support Library4 in HP41C/CV/MAXX machines
    Is somewhat experimental, please report issues
-   Reset into BOOTSELmode now unplugs all embedded modules
-   Changed reading uSD card number of sectors, now with ioctl call to fix issue with exFAT on MacOS
-   Fixed issue with enabling hpil emulation upon reboot/powercycle causing a real HP-IL module to malfunction
-   Fixed unplug ALL command which did not clear reservations
-   Fixed flash INIT command, would sometimes hang
-   Fixed driving ISA to powerup the calculator
-   Fixed tracer output which showed all instructions in column 7, it now correctly shows only TULIP decoded instructions

VERSION 0.94 BETA 2, September 2025
-   Fixed issue with Page 8 bank switching

VERSION 0.93 BETA 2, September 2025
-   Fixed issue with reserving Page 5, 6, 7
-   Increased GPIO drive strength for IR LED to 12 mA
-   Added support for MOD file plugging
-   Added import UPDATE and compare functions for mass update from a directory on the uSD card into FLASH
-   Cosmetic changes to the cat listing

VERSION 0.92 BETA 2, July 2025, Functional changes
-   Fixed issue with autoplug in the plug function
-   Added command 'printer irtog' to toggle IR LED on or off constanty to facilitate current measurement
-   Added command 'system gpio' to list all GPIO's
Other changes:
-   Updated to Pico SDK V2.2.0

VERSION 0.91 BETA 2, July 2025, Functional changes:
-   Support for Bank Switching and loading .ROM files in individual Banks
-   Support for ZEPROM Sticky Bank Switching (use the CLI emulate command to enable)
-   Flexible sizing of Trace Buffer (change requires restart)
-   A ROM can now be plugged with a Page nmber. This works only for Bank 1 and the first available Page is used
-   The reserve command is added to reserve a Page for a physical Module, plugging without a Page number respects this reservation

**Remember to unplug your embedded ROMs (HP-IL, ILPrinter) when doing a firmware upgrade (this is automatic from Firmware V0.95)**

**ONLY USE THE .UF2 VERSION FOR THE DEVBOARD!**

Some users with a Mac as their host computer have reported issues with the uSD card. It works fine on the TULIP itself, but refuse to connect to the host PC as a drive while in the TULIP. The solution is to format the to the ExFAT format using the official SD CArd Formatting software available at https://www.sdcard.org/downloads/formatter/

**WARNING for Wand users:**
When experimenting with the Wand please be aware that sending strings with characters < 0x10 can lead to
a crash of the calculator that can be recoverd from only by completely removing power (and batteries) and 
the TULIP. The firmware replaces these characters with a space. With the wand send command it is still 
possible to construct these strings and send this.

**TULIP4041 Quick Start**

The instructions below will guide you through the steps to get up and running with the new firmware TULIP4041 quickly.

0.  Please read the documentation. If you have time read it all, if not then start with Chapter 10 and then 11. Follow the instructions in Chapter 12 for the firmware update process
1.	Format a micro SD card (minimum 2 GByte) with the exFAT (recommended) or FAT file system
2.	Create a repository in a subdirectory on the card of ROM and MOD files that you wish to use in your TULIP (can also be done after step 3). Let’s call this myROMrepo
3.	Plug the microSD card in your TULIP and connect it with your host PC, verify if the card is visible as a removable drive in your host computer
4.	Connect a Terminal Emulator with the TULIP Command Line Interface. Try the newly discovered serial ports until you see the TULIP welcome message. Use the sdcard connect command in the CLI to connect with the host if the USB drive was not visible. When using the FAT file system the detection by the host can take some time, even up to several minutes when a large capacity SC card is used.
5.	Update to the latest firmware (see the instructions in the documentation) if needed
6.	Type the command dir myromrepo (do not care about the case of the subdirectory name) and verify if all your files are visible
7.	Type the ***list*** command to check which ROMs are already in FLASH. If this is the first time the TULIP is used it will contain the ROM files used for production testing. Go to step 12 in case the list is garbled or if no files are visible at all. If the system is newly initialized it will show only the “TULIP4041 FLASH HEADER” file of type 41.
8.	Type the command import myromrepo ALL (ALL must be uppercase!). This will copy your ROM repository to FLASH memory which is needed for the ROMs to be pluggable. 
9.	Use list again to verify if all your roms are imported
10.	Connect the TULIP with your calculator. While doing that check which ports are physically occupied. You can now use the plug command. Say that you have imported the set of PPC roms, PPCL.rom and PPCU.rom. Assuming that you have Port 4 free (this is Page E and F) you can type (the case of the ROM name and the Page do not matter)

    TULIP> plug PPCL.rom E

    TULIP> plug PPCU.rom F 

12.	Use cat in the TULIP CLI to confirm that the ROMs are plugged and enter CAT 2 on the HP41 to confirm that the PPC ROM is now plugged in the calculator. The configuration is saved in FRAM and will be available again after a power cycle
13.	In case the file system is corrupted or not yet initialized you need to fully initialize the FLASH File System with the following steps:


    TULIP> flash NUKEALL

    TULIP> flash INIT

    and/or
 
    TULIP> fram NUKEALL

    TULIP> fram INIT

And go back to step 7

Any issues? Best is to report these in the issue tracker here on GitHub.

Here are links to some videos I made:
- Soldering the HP41 connector: https://www.youtube.com/watch?v=pW8rN5y5UB0
- Production testing: https://www.youtube.com/watch?v=3AXWcQL1dGo
- Harvest a connector from an HP41 module: https://www.youtube.com/watch?v=I3a0NojwHeg
- Preview of the pre-production units, firmware and housing: https://www.youtube.com/watch?v=8PRu2yhQjck
- Overview of the pre-production units: https://www.youtube.com/watch?v=zgKlAMlz9vY
- Demonstration of the Wand emulation: https://youtu.be/-Lm2CUnrXPQ
- Assembly and soldering of the new battery board: https://youtu.be/wtu5E9yf-MI

Robert Prosperi gave a talk about the TULIP on the 2025 HHC confernce in Orlando, with a Q+A session where I particpated online: https://www.youtube.com/watch?v=4R3ebXHAJd4, here is the presentation PDF: https://hhuc.us/2025/Presentations/TULIP4041%20-%20The%20Next%20Chapter.pdf

If you wish to use the infrared printer output, you can use theRedEye IR Receiver here: https://github.com/mjakuipers/HP-RedEye-Receiver, base on Martin Hepperle's project https://www.mh-aerotools.de/hp/red-eye/index.htm 

The housing design is now also available here: https://github.com/blackjetrock/hp41c-tulip-module-case. Thanks Andrew for the design!

![IMG_20240925_110722543](https://github.com/user-attachments/assets/33102d0d-1736-4b8a-81de-e93384321606)

[TUP-Devboard Schematics V1.1.pdf](https://github.com/user-attachments/files/16324529/TUP-Devboard.Schematics.V1.1.pdf)

[TUP-Module Adapter Schematics V1.0.pdf](https://github.com/user-attachments/files/16324675/TUP-Module.Adapter.Schematics.V1.0.pdf)

Version history

- jul 21 2024   - initial version, schematics updated, not yet verified in hardware!
- sep 25 2024   - DevBoard PCB's are verified and ready to ship
                - Will change to the RP2350 and Pico2 after evaluating the Pico2
                - BOM file updated with the RP2350 Pico2
                - Documentation uploaded: TULIP description, usage, assembly and testing of DevBoard
- sep 26 2024   - The directory TULIP4041-RP2040-Final contains the archived 2040 sources and the latest binary (.uf2)
- sep 26 2024   - The direcory TULIP-P2 contains the sources and build for the RP2350 version
- oct 27 2024   - refreshed sources and firmware to version 00.01.05
- jun 27 2025   - update to version 0.9 BETA 1. See release notes for details
- jul 26 2025   - update to version 0.91 BETA 2. See release notes for details

All files are open source. Use of the hardware and software AT YOUR OWN RISK, no warranty

User Karel designed a tray for the mainboard and a module-like housing for the connectorboard. Thanks Karel for sharing! Details and download here: https://www.printables.com/model/1053554-tulip4041-devboard-case
