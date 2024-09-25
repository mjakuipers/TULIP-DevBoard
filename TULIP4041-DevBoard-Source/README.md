TULIP4041 RELEASE NOTES

The TULIP4041 soft- and hardware is released under the MIT copyright license

VERSION 00.01.01    -   First public beta
    Supported functions:    
    -   HP-IL, HP-IL Printer, PILBox 
    -   HP82143 Printer
    -   HP-IL Scope
    -   basic CLI
    -   HP41 Tracer

USB virtual serial port assignment
-   ITF 0		main console, user interface
-   ITF 1		trace output HP41 bus tracer 
-   IFT 2		HP-IL frames
-   ITF 3		HP-IL frame tracing
-   ITF 4		Printer output for HP82143A

The SD Card is exposed as a USB MSC device, not used in the application

The UART on GPIO 0 and 1 is used by the 'old' user interface and can be 
used for simple monitoring.
DO NOT USE THE FLASH FUNCTIONS!

USE AT YOUR OWN RISK
This is a very preliminary public BETA release. 
There is no public documentation yet
The sources contain may functions that are still in development 
and very subject to changes

The .uf2 file in the build directly is ready to be loaded in the Pico.