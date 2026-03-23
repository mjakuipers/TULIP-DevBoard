**A number of prepared MOD files are provided here for use with the TULIP4041**

**HEPAX-4HT**
- Modified HEPAX-4H for TULIP, plugs in any Page 8-F, contains only the 4-0banks of the HEPAX ROM. Written by Steen Petersen, portions A. Martin. This HEPAX version was built for use together with the LIBRARY4 ROM

**LIBRARY4-CV.MOD**
- Library4 MOD file for the HP41C or CV, 2 banks loaded in Page 4

**LIBRARY4-CX.MOD**
- Library4 MOD file for the HP41CX, single bank loaded in Page 4

**HEPRAM-4K.MOD, HEPRAM-8K.MOD. HEPRAM-16K.MOD**
- HEPAX RAM files with repectively 4, 8 an 16K RAM. Must be loaded in QROM to be useful. Before use the Pages must be released and the HEPRAM chain must be initialized (see instructions later in the README.MD)

**OSX_BS4X-CX.MOD**
- If you have an HP41, the OSX+ ROM is a must. Based upon the original CCD-ROM Ángel Martin built this 4-bank ROM to extend the HP41 system. Uses LIBRARY4. This version is for the HP41CX

**OSX_BS4X.MOD**
- OSX+ version for the HP41C/CV

**OSX_BS4X-P6.MOD and OSX_BS4X-P7.MOD**
- OSX+ versions that plug in PAge 6 or 7

**OSX_BS4X-LIB4.MOD**
- OSX+ version that includes LIBRARY4

**QROM-4K.MOD and QROM-8K.MOD**
- Empty QROM files. These MUST be imported in QROM to make any sense


**Using HEPAX and HEPAX RAM with the TULIP**, text in bold and italic are TULIP CLI commands
- Step 1: plug the HEPAX4H and LIBRARY4 ROM images (after importing of course)
- Step 2: import the HEPRAM files of choice in QROM with the command ***import hepram-8k.mod qrom***
- Step 3: you can now plug the MOD file: ***plug hepram-8k.mod***, and check with ***cat*** in which Pages the HEPRAM is plugged
- Step 4: mark the HEPRAM Pages to be included in the HEPRAM chain: ***hepram release P*** for both HEPRAM pages
- Step 5: initilaize the HEPRAM chain: ***hepram INIT***, this will initialize the HEPRAM chain
- Step 6: on the HP41, type HEPROOM and HEPDIR to check the amount of avaialble registers

Any changes in a file that is plugged from QROM/FRAM are __immediately__ available in the file in FRAM. This file can be exported to the uSD card and then used in other HP41 systems (emulated or real). Be careful when moving a HEPRAM chained MOD file to another system. The Page numbers are hardcoded in the HEPRAM itself and when this ends up in another Page the effects may be undesirable. Either edit the chain manually or use one of Ángels tools (in the HPX-Plus ROM for example). Best is to enforce the HEPRAM in exactly the same Page configuration, but this may not always be easy.

It is perfectly possible to plug a file a second time. With a QROM Page this has the effect that changes in one Page will immediately be visible in the other Page. This may not always be your intention, but if that is not your case, create a copy of the (empty) QROM file, import it in QROM/FRAM and then plug it after the earlier file. 

With a MOD file it is not always possible to force it in a certain Page/Bank combination. Instead, use a ROM file for this purpose. Alternatively, create or edit a MOD file so it has exactly the features you want.
