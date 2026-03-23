**A number of prepared MOD files are prvided here for use with the TULIP4041**

**HEPAX-4HT**
Modified HEPAX-4H for TULIP, plugs in any Page 8-F, contains only the 4-0banks of the HEPAX ROM. Written by Steen Petersen, portions A. Martin. This HEPAX version was built for use together with the LIBRARY4 ROM

**LIBRARY4-CV.MOD**

  Library4 MOD file for the HP41C or CV, 2 banks loaded in Page 4

**LIBRARY4-CX.MOD**

  Library4 MOD file for the HP41CX, signle bank loaded in Page 4

**HEPRAM-4K.MOD, HEPRAM-8K.MOD. HEPRAM-16K.MOD**

  HEPAX RAM files with repectively 4, 8 an 16K RAM. Must be loaded in QROM to be useful

**OSX_BS4X-CX.MOD**

  If you have an HP41, the OSX+ ROM is a must. Based upon the original CCD-ROM Ángel Martin built this 4-bank ROM to extend the HP41 system. Uses LIBRARY4. This version is for the HP41CX

**OSX_BS4X.MOD**

  OSX+ version for the HP41C/CV

**OSX_BS4X-P6.MOD and OSX_BS4X-P7.MOD**

  OSX+ versions that plug in PAge 6 or 7

**OSX_BS4X-LIB4.MOD**

  OSX+ version that includes LIBRARY4

**QROM-4K.MOD and QROM-8K.MOD**
  Empty QROM files. These MUST be imported in QROM to make any sense



**Using HEPAX and HEPAX RAM with the TULIP. Text in bold and italic are TUILIP CLI commands**

Step 1: plug the HEPAX4H and LIBRARY4 ROM images (after importing of course)

Step 2: import the HEPRAM files of choice in QROM with the command ***import hepram-8k.mod qrom***

Step 3: you can now plug the MOD file: ***plug hepram-8k.mod***, and check with ***cat*** in which Pages the HEPRAM is plugged

Step 4: mark the HEPRAM Pages to be included in the HEPRAM chain: ***hepram release P*** for both HEPRAM pages

Step 5: initilaize the HEPRAM chain: ***hepram INIT***, this will initialize the HEPRAM chain

Step 6: on the HP41, type HEPROOM and HEPDIR to check the amount of avaialble registers




  


