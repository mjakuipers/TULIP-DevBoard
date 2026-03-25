Some ready made MOD files are provided here for convenience. Take care that these configurations may work differently in non-TULIP environments. 
Text in ***bold and italics*** are commands for the TULIP CLI.

**HEPAX-4HT.MOD**
- A variation of the HEPAX-4H+ (original by Steen Petersen, modifications by Ángel Martin) that will plug in any page, odd or even.
This MOD file has 4 banks! This HEPAX version is modified to be used with LIBRARY4!

**HEPRAM-4K.MOD, HPERAM-8K.MOD and HEPRAM 16K.MOD**
- HEPAX RAM files for use with HEPAX. The MOD files are modified such that these will plug in any Page and may end up in non-consecutive Pages, depending on the current configuration en reserved Pages

**To use HEPAX and HEPRAM in your TULIP**
-  Simply plug the HEPAX and HEPRAM files according to your requirements. The HEPAX RAM pages are normally initialized by the HEPAX ROM to form a chain, and this initialization is not done in this modified version.
Therefore the CLI offers the ***hepram INIT P*** command to initialize a single page of HEPRAM. Easiest is to use the ***hepram INITALL*** command to initialize all HEPRAM pages.
Only pages marked as HEPAX RAM will be initialized, other QROM pages will not be part of the chain, but the ***qrom hepram P*** command can be used to mark or unmark a QROM page as HEPRAM.
It is possible to keep a HEPRAM page out of the chain with the ***hepram reserve P*** command. Please refer to the excellent HEPAX manual for details.
- After the initialization execute the HEPDIR function in your HP41 to verify the amount of registers in the HEPAX filesystem.
- When storing anything in HEPRAM, the files will be immediately updated and can be exported to the uSD card. Since the chain information in the HEPRAM is Page dependent, be careful when plugging the
file in another (emulated) HP41 calculator. If this is your goal then use a single 4K HEPRAM page.
- It might be tempting to plug another 4K HEPRAM file. It is possible to plug the same file again, but keep in mind that the contents will be identical, and a change in one Page will also be a change in the other plugged Page.
Best is to create a copy of the file with a new name, and plug that.

**LIBRARY4-CX.MOD and LIBRARY4-CV.MOD**
-  2 versions of Ángels wonderful LIBRARY4 ROMs to be used in a CX or C/CV respecitively. The CV version is a 2-bank ROM, and both will be plugged in Page 4.

**OSX_BS4X-CX.MOD and OSX_BS4X.MOD**
- 2 versions of the famous OSX+ ROM, developed by Ángel Martin based onm the original CDD ROM. This is a 4-bank ROM image, the file with CX in the filename is for the HP41CX, the other for the C or CV.
This ROM is a must for your calculator and uses LIBRARY4. The files with P6 and P7 will be plugged in Pages 6 or 7 in case you prefer that. These are non-CX versions!
The OSX_BS4X-LIB4.MOD is the C/CV version of OSX+ and includes LIBRARY4

**QROM-4K.MOD and QROM-8K.MOD**
- EMPTY QROM (MLDL RAM) images. Although it is possible to plug a file a second time (and this could be useful in advanced configurations) this is the same physical memory.
A change in one Page will also be a change in the other plugged Page. Best is to create a copy of the file with a new name, and plug that. A change in a QROM Page is an immediate change in the file
and the file will not be an empty QROM Page anymore. Tip: rename the empty QROM file to something that reflects your mcode project
- If you want full control over which Page (or Bank) the QROM is plugged in you will have to use ROM files or modify the MOD file header so it contains your desired configuration
- Check out the CLI ***qrom*** command for some tricks
