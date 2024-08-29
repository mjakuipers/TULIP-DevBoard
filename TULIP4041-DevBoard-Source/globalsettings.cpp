/*
 * globalsettings.c
 *
 * This file is part of the TULIP4041 project.
 * Copyright (C) 2024 Meindert Kuipers
 *
 * This is free software: you are free to change and redistribute it.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * USE AT YOUR OWN RISK
 *
 */

// This class handles all global settings

#include "globalsettings.h"


// Global settings strings for human readable settings
const char* __in_flash()glob_set_[] = {

    "HP82143A Printer enabled",                 //  0   HP82143A printer active and SELP9 decoded
    "HP82153A Wand enabled",                    //  1   HP82153A Wand active
    "HP82160A HP-IL enabled",                   //  2   HP82160A HP-IL module active
    "HP82242A IR Printer enabled ",             //  3   HP82242A Blinky IR printer module active
    "HP82104A Cardreader enabled",              //  4   HP82104A Cardreader active
    "HP82182A Time Module enabled",             //  5   HP82104A Time Module active
    "HP41CL Emulation enabled",                 //  6   HP41CL emulation active (limited function set)
    "HEPAX instructions enabled",               //  7   HEPAX module native support  
    "QROM enabled (WROM instruction decoded)",  //  8   WROM instruction decoded
    "ROM reading enabled",                      //  9   ROM reading enabled
    "Bankswitching enabled ",                   // 10   bankswitch instructions enabled  
    "Expanded Memory enabled ",                 // 11   Expandend Memory enabled (MAXX emulation)
    "Tiny41 Instructions enabled  ",            // 12   enable decoding of TT specific instructions for device control
    "All QROM write protected"                  // 13   Write Protect all QROM (same effact as #8 ??)

    "",                                         // 14   placeholder
    "",                                         // 15   placeholder
    "",                                         // 16   placeholder
    "",                                         // 17   placeholder
    "",                                         // 18   placeholder
    "",                                         // 19   placeholder

// control of TULIP4041 outputs to the HP41 system bus
    "DATA drive enabled ",                      // 20   enable driving of DATA output
    "ISA drive enabled ",                       // 21   enable driving of ISA (to switch ROM emulation on/off)
    "PWO drive enabled ",                       // 22   enable driving of PWO line for reset of the HP41
    "FI drive enabled",                         // 23   driving of FI is enabled
    "IR drive enabled ",                        // 24   driving the IR led is enabled

    "",                                         // 23   placeholder
    "",                                         // 24   placeholder
    "",                                         // 25   placeholder
    "",                                         // 26   placeholder
    "",                                         // 27   placeholder
    "",                                         // 28   placeholder
    "",                                         // 29   placeholder

// control of tracer settings
    "Tracer enabled",                           // 30   tracer enabled
    "Tracing HP-IL Registers",                  // 31   tracing of HP-IL registers enabled
    "Tracer disassembler on",                   // 32   disassembly enabled
    "Tracer FI line on",                        // 33   FI tracing enabled
    "Tracer Mnemonics type",                    // 34   tracer disassembly type (JDA only for now)
                                                //      0 = JDA
                                                //      1 = HP   (not yet supported)
                                                //      2 = ...
    "Tracing System Pages 0..7",                // 35   tracing of system ROMS pages P0..7
                                                //      bit pattern in byte refers to page
    "Tracing User ROM P8..F",                   // 36   tracing of user ROM pages 8..F
                                                //      bit pattern in byte refers to page
    "Tracer selected system loops",             // 37   tracing of system loops enabled:
                        	                    //      0x0098 - 0x00A1       RSTKB and RST05
        	                                    //      0x0177 - 0x0178       delay for debounce
        	                                    //      0x089C - 0x089D       BLINK01
                                                //      others ??

    "",                                         // 38   placeholder
    "",                                         // 39   placeholder
    "",                                         // 40   placeholder
    "",                                         // 41   placeholder
    "",                                         // 42   placeholder
    "",                                         // 43   placeholder
    "",                                         // 44   placeholder
    "",                                         // 45   placeholder
    "",                                         // 46   placeholder
    "",                                         // 47   placeholder
    "",                                         // 48   placeholder
    "",                                         // 49   placeholder

// HP-IL scope settings
    "HP-IL Scope enabled",                      // 50   HP-IL scope enabled
    "HP-IL PILBox scope enabled",               // 51   PILBox serial tracing enabled
    "",                                         // 52
    "",                                         // 53
    "",                                         // 54
    "",                                         // 55
    "",                                         // 56
    "",                                         // 57
    "",                                         // 58
    "",                                         // 59


// XMEM/User memory settings
    "Extended Memory pages",                    // 60   XMEM/XFUN pages   0x01    XFunctions Memory
                                                //                        0x02    XMem #1 active
                                                //                        0x04    XMem #2 active
    "User Memory pages"                         // 61   User Memory pages 0x01    Memory Module #1
                                                //                        0x02    Memory Module #2
                                                //                        0x04    Memory Module #3
                                                //                        0x08    Memory Module #4

    "",                                         // 62   placeholder
    "",                                         // 63   placeholder
    "",                                         // 64   placeholder
    "",                                         // 65   placeholder
    "",                                         // 66   placeholder
    "",                                         // 67   placeholder
    "",                                         // 68   placeholder
    "",                                         // 69   placeholder

// control of various messages
    "Print monitor enabled",                    // 70   monitor printer characters to console
    "Power monitor enabled",                    // 71   monitor power mode changes to console

    "",                                         // 72
    "",                                         // 73
    "",                                         // 74
    "",                                         // 75
    "",                                         // 76
    "",                                         // 77
    "",                                         // 78
    "",                                         // 79

// settings for HP82143 printer
    "Printer mode ",                            // 80   HP82143 status word, matches bits in SMA and SMB field (bit 15 + 16)
                                                //      0 - MAN mode
                                                //      1 - NORM
                                                //      2 - TRACE mode
                                                //      3 - TRACE mode (not used)                                    
    "Printer port delay (0.1 secs)",            // 81   for IR printing/ delay in HP82143 printing
    "Printer Paper loaded",                     // 82   is paper loaded
    "Printer Power on",                         // 83   printer power
    "Printer serial output"                     // 84   use serial printer output for output in terminal emulator

    "",                                         // 85
    "",                                         // 86
    "",                                         // 87
    "",                                         // 88
    "",                                         // 89

// control of TULIP4041 device
    "CLI on USB",                               // 90   CLI goes to USB CDC port, otherwise to serial port
    "PWO show events",                          // 91   show PWO events in the CLI ? or better in the tracer?
    "Global settings initialized",              // 92   are the global settings initialized? 
                                                //      used to set the defaults the first time
    "Global set active number",                 // 93   to keep track of active set of GSettings   

    "",                                         // 95
    "",                                         // 96
    "",                                         // 97
    "",                                         // 98

    "Global settings last item",                // 99   to identify the last item

};

// instantiate class for globals:
GSettings globsetting;