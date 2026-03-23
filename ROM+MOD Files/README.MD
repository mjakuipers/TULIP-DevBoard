A number of prepared MOD files are prvided here for use with the TULIP4041

HEPAX-4HT
  Modified HEPAX-4H for TULIP, plugs in any Page 8-F, contains only the 4-0banks of the HEPAX ROM 
  Written by Steen Petersen, portions A. Martin
  This HEPAX version was built for use together with the LIBRARY4 ROM


# TULIP-DevBoard and TULIP-Module
README for the TULIP4041 DevBoard and Module.


**WARNING for firmware version 0.96:**
When experimenting with the Wand please be aware that sending strings with characters < 0x10 can lead to
a crash of the calculator that can be recoverd from only by completely removing power (and batteries) and 
the TULIP. The firmware replaces these characters with a space. With the wand send command it is still 
possible to construct these strings and send this.

To order, contact me directly with your email address to receive an order form.

Please be aware that the firmware files are in the TULIP4041-P2/build subdirectory. The firmware files are:
   - tulip4041_module.uf2      For the TULIP Module
   - tulip4041_devboard.uf2    For the TULIP DevBoard
DO NOT mix up the firmware files, some hardware I/O and memory sized are different.

The TULIP Hardware V1.0 Schematics are added, following the succesful verification of the production units

There are now a number of MOD files prepared for use in the TULIP:
- HEPAX_4H.MOD       4-Bank modified HEPAX with RAM pages (plugs in any even Page)
- OSX-BS4X.MOD       standard 4-bank CCD OSX (plugs in any lower port Page) for HP41CX
- OSX_BS4X-LIB4.MOD  standard 4-bank CCD OSX (plugs in any lower port Page) with LIBRARY4 (plugs in Page 4) for HP41CX

  For the HP41C/CV Use the following:
- XOSX_BS4X.MOD      4-bank version of CCD OSX for HP41C/CV systems (plugs in any lower port Page)
- LIBRARY4-MAXX.MOD  2-bank switching version of Library4. Must be plugged in Page 4 for HP41C/CV systems
                     the system MUST support bankswitching in Page 4. Is supported in TULIP from firmware 0.95!
Many other MOD files are available at http://www.hp41.org/mod/, and the website http://www.hp41.org/Intro.cfm is a great resource
for manuals, ROM and MOD images and tools.
My favourite community for HP calculators is https://www.hpmuseum.org/forum/index.php, there are some TULIP related threads.
