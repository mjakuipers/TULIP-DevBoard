# TULIP-DevBoard and TULIP-Module
README for the TULIP4041 DevBoard

This is a major firmware update. The binary is in the build directory.

**ONLY USE THE .UF2 VERSION FOR THE DEVBOARD!**

TULIP4041 Quick Start
The instructions below will guide you through the steps to get up and running with the new firmware TULIP4041 quickly.
0.  Please read the documentation. If you have time read it all, if not then start with Chapter 10 and then 11. Follow the instructions in Chapter 12 for the firmware update process
1.	Format a micro SD card (minimum 2 GByte) with the exFAT (recommended) or FAT file system
2.	Create a repository in a subdirectory on the card of ROM and MOD files that you wish to use in your TULIP (can also be done after step 3). Let’s call this myROMrepo
3.	Plug the microSD card in your TULIP and connect it with your host PC, verify if the card is visible as a removable drive in your host computer
4.	Connect a Terminal Emulator with the TULIP Command Line Interface. Try the newly discovered serial ports until you see the TULIP welcome message. Use the sdcard connect command in the CLI to connect with the host if the USB drive was not visible. When using the FAT file system the detection by the host can take some time, even up to several minutes when a large capacity SC card is used.
5.	Update to the latest firmware (see the instructions in the previous chapter) if needed
6.	Type the command dir myromrepo (do not care about the case of the subdirectory name) and verify if all your files are visible
7.	Type the list command to check which ROMs are already in FLASH. If this is the first time the TULIP is used it will contain the ROM files used for production testing. Go to step 12 in case the list is garbled or if no files are visible at all. If the system is newly initialized it will show only the “TULIP4041 FLASH HEADER” file of type 41.
8.	Type the command import myromrepo ALL (ALL must be uppercase!). This will copy your ROM repository to FLASH memory which is needed for the ROMs to be pluggable. 
9.	Use list again to verify if all your roms are imported
10.	Connect the TULIP with your calculator. While doing that check which ports are physically occupied
You can now use the plug command. Say that you have imported the set of PPC roms, PPCL.rom and PPCU.rom. Assuming that you have Port 4 free (this is Page E and F) you can type (the case of the ROM name and the Page do not matter
TULIP> plug PPCL.rom E
TULIP> plug PPCU.rom F 
11.	Use cat in the TULIP CLI to confirm that the ROMs are plugged and enter CAT 2 on the HP41 to confirm that the PPC ROM is now plugged in the calculator. The configuration is saved in FRAM and will be available again after a power cycle
12.	In case the file system is corrupted you need to fully initialize the FLASH File System with the following steps:
TULIP> flash NUKEALL
TULIP> flash INIT
And go back to step 7

Any issues? Best is to report these in the issue tracker here on GitHub.

Details on the TULIP Module version are coming soon.

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
- jun 27 2025   - update to version 0.9 BETA 1. See relaease notes for details

All files are open source. Use of the hardware and software AT YOUR OWN RISK, no warranty

User Karel designed a tray for the mainboard and a module-like housing for the connectorboard. Thanks Karel for sharing! Details and download here: https://www.printables.com/model/1053554-tulip4041-devboard-case
