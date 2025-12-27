/*  
 * userinterface.c
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


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include "userinterface.h"

#define Version_Number VERSION_STRING
#define Comp_Date_string COMP_DATE_TIME


 // Global settings strings for human readable settings
const char* __in_flash("flash_constants")glob_set_[] = {

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
    "Bankswitching enabled",                    // 10   bankswitch instructions enabled  
    "Expanded Memory enabled",                  // 11   Expandend Memory enabled (MAXX emulation)
    "Tiny41 Instructions enabled",              // 12   enable decoding of TT specific instructions for device control
    "",                                         // 13   Write Protect all QROM (same effact as #8 ??)

    "",                                         // 14   placeholder

    "HP-IL module plugged",                     // 15   HP-IL module plugged in Page 7
    "HP-IL Printer plugged",                    // 16   IL-Printer module plugged in Page 6
    "HP82143 Printer plugged",                  // 17   HP82143A Printer module plugged
    "",                                         // 18   placeholder
    "",                                         // 19   placeholder

// control of TULIP4041 outputs to the HP41 system bus
    "DATA drive enabled ",                      // 20   enable driving of DATA output
    "ISA drive enabled ",                       // 21   enable driving of ISA (to switch ROM emulation on/off)
    "PWO drive enabled ",                       // 22   enable driving of PWO line for reset of the HP41
    "FI drive enabled",                         // 23   driving of FI is enabled
    "IR drive enabled ",                        // 24   driving the IR led is enabled

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
    "Tracing IL ROM (Page 6+7)",                // 41   placeholder
    "Main Trace Buffer size (samples)",         // 42   placeholder
    "PreTrig buffer size (samples)",            // 43   placeholder
    "PostTrig Counter",                         // 44   placeholder
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
    "User Memory pages",                        // 61   User Memory pages 0x01    Memory Module #1
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
    "Printer mode",                             // 80   HP82143 status word, matches bits in SMA and SMB field (bit 15 + 16)
                                                //      0 - MAN mode
                                                //      1 - NORM
                                                //      2 - TRACE mode
                                                //      3 - TRACE mode (not used)                                    
    "Printer port delay (0.1 secs)",            // 81   for IR printing/ delay in HP82143 printing
    "Printer Paper loaded",                     // 82   is paper loaded
    "Printer Power on",                         // 83   printer power
    "Printer serial output",                    // 84   use serial printer output for output in terminal emulator

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

    "",                                         // 94
    "",                                         // 95
    "",                                         // 96
    "",                                         // 97
    "",                                         // 98

    "Global settings last item",                // 99   to identify the last item
    "",                                         // 100  guard
};

// Global settings strings for human readable settings
const char* __in_flash("flash_constants")gpio_names_[] = {
    "UART TX",                    //  GPIO 0   
    "UART RX",                    //  GPIO 1   
    #if (TULIP_HARDWARE == T_DEVBOARD)
      "FI input",                   //  GPIO 2   // for I2C, OLED, RTC, FI input
      "IR OUT/PWO OUT",             //  GPIO 3   // for IR output and PWO output enable
    #elif (TULIP_HARDWARE == T_MODULE)
      "I2C1 SDA - RTC",             //  GPIO 2   // RTC SDA
      "I2C1 SCL - RTC",             //  GPIO 3   // RTC SCL
    #endif
    "SPI0 SO - FRAM",             //  GPIO 4   // SPI0 SO - FRAM
    "SPI0 CS - FRAM",             //  GPIO 5   // SPI0 CS - FRAM
    "SPI0 CK - FRAM",             //  GPIO 6   // SPI0 CK - FRAM
    "SPI0 SI - FRAM",             //  GPIO 7   // SPI0 SI - FRAM
    "SPI1 DO - uSD",              //  GPIO 8   // SPI1 DO - uSD
    "SPI1 CS - uSD",              //  GPIO 9   // SPI1 CS - uSD
    "SPI1 SI - uSD",              //  GPIO 10  // SPI1 SI - uSD  
    "SPI1 DO - uSD",              //  GPIO 11  // SPI1 DO - uSD
    "HP41 CLK1",                  //  GPIO 12  // HP41 CLK1
    "HP41 CLK2",                  //  GPIO 13  // HP41 CLK2
    "HP41 ISA",                   //  GPIO 14  // HP41 ISA
    "HP41 SYNC",                  //  GPIO 15  // HP41 SYNC
    "HP41 DATA",                  //  GPIO 16  // HP41 DATA
    "HP41 PWO",                   //  GPIO 17  // HP41 PWO
    "HP41 ISA OUT",               //  GPIO 18  // HP41 ISA OUT
    "HP41 ISA OE",                //  GPIO 19  // HP41 ISA OE
    "HP41 DATA OUT",              //  GPIO 20  // HP41 DATA OUT
    "HP41 DATA OE",               //  GPIO 21  // HP41 DATA OE
    "HP41 FI OE",                 //  GPIO 22  // HP41 FI OE
    #if (TULIP_HARDWARE == T_DEVBOARD)
      "not used",                   //  GPIO 23  // not used
    #elif (TULIP_HARDWARE == T_MODULE)
      "SPARE1",                     //  GPIO 23  // Onboard LED pin number
    #endif
    "VBUS present",               //  GPIO 24  // VBUS pin number
    "Onboard LED",                //  GPIO 25  // Onboard LED pin number
    "T0_TIME",                    //  GPIO 26  // T0 time, also
    "SYNC_TIME",                  //  GPIO 27  // SYNC time, during SYNC and ISA instruction (always)
    "P_DEBUG",                    //  GPIO 28  // P_DEBUG pin, for debugging
    #if (TULIP_HARDWARE == T_DEVBOARD)
      "not used",                   //  GPIO 29  // not used
    #elif (TULIP_HARDWARE == T_MODULE)
      "IR LED",                     //  GPIO 29  IR LED
    #endif
};

typedef struct
{
  char command[10];
  uint8_t hex1;
  uint8_t hex2;
  uint8_t type;
} Wand_Commands;


typedef struct 
{
  uint8_t hex1;
  uint8_t hex2;
} Wand_Alpha_Keys;

const Wand_Commands __in_flash("wand_constants")wand_cmd_[] = {
  {"0", 0x00, 0x00, 6},               // type 6, one-byte paper keyboard digit entry
  {"1", 0x81, 0x00, 6},
  {"2", 0x42, 0x00, 6},
  {"3", 0xC3, 0x00, 6},
  {"4", 0x24, 0x00, 6},
  {"5", 0xA5, 0x00, 6},
  {"6", 0x66, 0x00, 6},
  {"7", 0xE7, 0x00, 6},
  {"8", 0x18, 0x00, 6},
  {"9", 0x99, 0x00, 6},
  {".", 0x5A, 0x00, 6},
  {"EEX", 0xDB, 0x00, 6},
  {"CHS", 0x3C, 0x00, 6},
  {"<-", 0xBD, 0x00, 6},
  {"<", 0xBD, 0x00, 6},

  {"CAT", 0x44, 0x00, 4},             // type 4, two-byte non-programmables
  {"GTOL", 0x54, 0x01, 4},
  {"DEL", 0x64, 0x02, 4},
  {"COPY", 0x74, 0x03, 4},
  {"CLP", 0x84, 0x04, 4},
  {"R/S", 0x94, 0x05, 4},
  {"RS", 0x94, 0x05, 4},
  {"SIZE", 0xA4, 0x06, 4},
  {"BST", 0xB4, 0x07, 4},
  {"SST", 0xC4, 0x08, 4},
  {"STAYON", 0xD4, 0x09, 4},
  {"PACK", 0xE4, 0x0A, 4},
  {"DELETE", 0xF4, 0x0B, 4},
  {"ALPHA", 0x14, 0x0C, 4},
  {"PRGM", 0x24, 0x0D, 4},
  {"USER", 0x34, 0x0E, 4},
  {"ASN", 0x44, 0x0F, 4},

  {"IND", 0xA2, 0x80, 3},           // type 3, indirect

  // single APLHA character keys (for testing only)
  // {"X"  , 0xA8, 0x00, 2},           // 'X' = 0x58 = 0b0101.1000, 4 msb's doubled = 0b1010.1000 = 0xA8
                                    // type 5, one-byte ALPHA character

  // type 1, one-byte functions

  {"+", 0x40, 0x40, 1},
  {"-", 0x50, 0x41, 1},
  {"*", 0x60, 0x42, 1},
  {"/", 0x70, 0x43, 1},
  {"X<Y?", 0x80, 0x44, 1},
  {"X>Y?", 0x90, 0x45, 1},
  {"X<=Y?", 0xA0, 0x46, 1},
  {"S+", 0xB0, 0x47, 1},
  {"S-", 0xC0, 0x48, 1},
  {"HMS+", 0xD0, 0x49, 1},
  {"HMS-", 0xE0, 0x4A, 1},
  {"MOD", 0xF0, 0x4B, 1},
  {"%", 0x10, 0x4C, 1},
  {"%CH", 0x20, 0x4D, 1},
  {"P-R", 0x30, 0x4E, 1},
  {"R-P", 0x40, 0x4F, 1},
  {"LN", 0x50, 0x50, 1},
  {"X^2", 0x60, 0x51, 1},
  {"SQRT", 0x70, 0x52, 1},
  {"Y^X", 0x80, 0x53, 1},
  {"CHS", 0x90, 0x54, 1},
  {"E^X", 0xA0, 0x55, 1},
  {"LOG", 0xB0, 0x56, 1},
  {"10^X", 0xC0, 0x57, 1},
  {"E^X-1", 0xD0, 0x58, 1},
  {"SIN", 0xE0, 0x59, 1},
  {"COS", 0xF0, 0x5A, 1},
  {"TAN", 0x10, 0x5B, 1},
  {"ASIN", 0x20, 0x5C, 1},
  {"ACOS", 0x30, 0x5D, 1},
  {"ATAN", 0x40, 0x5E, 1},
  {"DEC", 0x50, 0x5F, 1},
  {"1/X", 0x60, 0x60, 1},
  {"ABS", 0x70, 0x61, 1},
  {"FACT", 0x80, 0x62, 1},
  {"X#0?", 0x90, 0x63, 1},
  {"X>0?", 0xA0, 0x64, 1},
  {"LN1+X", 0xB0, 0x65, 1},
  {"X<0?", 0xC0, 0x66, 1},
  {"X=0?", 0xD0, 0x67, 1},
  {"INT", 0xE0, 0x68, 1},
  {"FRC", 0xF0, 0x69, 1},
  {"D-R", 0x10, 0x6A, 1},
  {"R-D", 0x20, 0x6B, 1},
  {"-HMS", 0x30, 0x6C, 1},
  {"-HR", 0x40, 0x6D, 1},
  {"RND", 0x50, 0x6E, 1},
  {"OCT", 0x60, 0x6F, 1},
  {"CLS", 0x70, 0x70, 1},
  {"X<>Y", 0x80, 0x71, 1},
  {"PI", 0x90, 0x72, 1},
  {"CLST", 0xA0, 0x73, 1},
  {"R^", 0xB0, 0x74, 1},
  {"RDN", 0xC0, 0x75, 1},
  {"LASTX", 0xD0, 0x76, 1},
  {"CLX", 0xE0, 0x77, 1},
  {"X=Y?", 0xF0, 0x78, 1},
  {"X#Y?", 0x10, 0x79, 1},
  {"SIGN", 0x20, 0x7A, 1},
  {"X?0?", 0x30, 0x7B, 1},
  {"MEAN", 0x40, 0x7C, 1},
  {"SDEV", 0x50, 0x7D, 1},
  {"AVIEW", 0x60, 0x7E, 1},
  {"CLD", 0x70, 0x7F, 1},
  {"DEG", 0x80, 0x80, 1},
  {"RAD", 0x90, 0x81, 1},
  {"GRAD", 0xA0, 0x82, 1},
  {"ENTER^", 0xB0, 0x83, 1},

  {"ENTER", 0xB0, 0x83, 1},     // alias of ENTER^

  {"STOP", 0xC0, 0x84, 1},
  {"RTN", 0xD0, 0x85, 1},
  {"BEEP", 0xE0, 0x86, 1},
  {"CLA", 0xF0, 0x87, 1},
  {"ASHF", 0x10, 0x88, 1},
  {"PSE", 0x20, 0x89, 1},
  {"CLRG", 0x30, 0x8A, 1},
  {"AOFF", 0x40, 0x8B, 1},
  {"AON", 0x50, 0x8C, 1},
  {"OFF", 0x60, 0x8D, 1},
  {"PROMPT", 0x70, 0x8E, 1},
  {"ADV", 0x80, 0x8F, 1},
  {"RCL", 0x90, 0x90, 1},
  {"STO", 0xA0, 0x91, 1},
  {"ST+", 0xB0, 0x92, 1},
  {"ST-", 0xC0, 0x93, 1},
  {"ST*", 0xD0, 0x94, 1},
  {"ST/", 0xE0, 0x95, 1},
  {"ISG", 0xF0, 0x96, 1},
  {"DSE", 0x10, 0x97, 1},
  {"VIEW", 0x20, 0x98, 1},
  {"SREG", 0x30, 0x99, 1},
  {"ASTO", 0x40, 0x9A, 1},
  {"ARCL", 0x50, 0x9B, 1},
  {"FIX", 0x60, 0x9C, 1},
  {"SCI", 0x70, 0x9D, 1},
  {"ENG", 0x80, 0x9E, 1},
  {"TONE", 0x90, 0x9F, 1},

  {"SF", 0x30, 0xA8, 1},
  {"CF", 0x40, 0xA9, 1},
  {"FS?C", 0x50, 0xAA, 1},
  {"FC?C", 0x60, 0xAB, 1},
  {"FS?", 0x70, 0xAC, 1},
  {"FC?", 0x80, 0xAD, 1},

  // {"GO/XQ_IND", 0x90, 0xAE, 1},
  // {"LBL\"", 0xC0, 0xC0, 1},          // LBL" 

  {"X<>", 0xB0, 0xCE, 1},
  {"LBL", 0xC0, 0xCF, 1},
  {"GTO", 0xD0, 0xD0, 1},
  {"XEQ", 0xE0, 0xE0, 1},

};

const Wand_Alpha_Keys __in_flash("wand_constants")wand_alpha_[] = {
  {0x11, 0x00},
  {0x21, 0x01},
  {0x31, 0x02},
  {0x41, 0x03},
  {0x51, 0x04},
  {0x61, 0x05},
  {0x71, 0x06},
  {0x81, 0x07},
  {0x91, 0x08},
  {0xA1, 0x09},
  {0xB1, 0x0A},
  {0xC1, 0x0B},
  {0xD1, 0x0C},
  {0xE1, 0x0D},
  {0xF1, 0x0E},
  {0x11, 0x0F},
  {0x31, 0x20},
  {0x41, 0x21},
  {0x51, 0x22},
  {0x61, 0x23},
  {0x71, 0x24},
  {0x81, 0x25},
  {0x91, 0x26},
  {0xA1, 0x27},
  {0xB1, 0x28},
  {0xC1, 0x29},
  {0xD1, 0x2A},
  {0xE1, 0x2B},
  {0xF1, 0x2C},
  {0x11, 0x2D},
  {0x21, 0x2E},
  {0x31, 0x2F},
  {0x51, 0x40},
  {0x61, 0x41},
  {0x71, 0x42},
  {0x81, 0x43},
  {0x91, 0x44},
  {0xA1, 0x45},
  {0xB1, 0x46},
  {0xC1, 0x47},
  {0xD1, 0x48},
  {0xE1, 0x49},
  {0xF1, 0x4A},
  {0x11, 0x4B},
  {0x21, 0x4C},
  {0x31, 0x4D},
  {0x41, 0x4E},
  {0x51, 0x4F},
  {0x71, 0x60},
  {0x81, 0x61},
  {0x91, 0x62},
  {0xA1, 0x63},
  {0xB1, 0x64},
  {0xC1, 0x65},
  {0xD1, 0x66},
  {0xE1, 0x67},
  {0xF1, 0x68},
  {0x11, 0x69},
  {0x21, 0x6A},
  {0x31, 0x6B},
  {0x41, 0x6C},
  {0x51, 0x6D},
  {0x61, 0x6E},
  {0x71, 0x6F},
  {0x91, 0x80},
  {0xA1, 0x81},
  {0xB1, 0x82},
  {0xC1, 0x83},
  {0xD1, 0x84},
  {0xE1, 0x85},
  {0xF1, 0x86},
  {0x11, 0x87},
  {0x21, 0x88},
  {0x31, 0x89},
  {0x41, 0x8A},
  {0x51, 0x8B},
  {0x61, 0x8C},
  {0x71, 0x8D},
  {0x81, 0x8E},
  {0x91, 0x8F},
  {0xB1, 0xA0},
  {0xC1, 0xA1},
  {0xD1, 0xA2},
  {0xE1, 0xA3},
  {0xF1, 0xA4},
  {0x11, 0xA5},
  {0x21, 0xA6},
  {0x31, 0xA7},
  {0x41, 0xA8},
  {0x51, 0xA9},
  {0x61, 0xAA},
  {0x71, 0xAB},
  {0x81, 0xAC},
  {0x91, 0xAD},
  {0xA1, 0xAE},
  {0xB1, 0xAF},
  {0xD1, 0xC0},
  {0xE1, 0xC1},
  {0xF1, 0xC2},
  {0x11, 0xC3},
  {0x21, 0xC4},
  {0x31, 0xC5},
  {0x41, 0xC6},
  {0x51, 0xC7},
  {0x61, 0xC8},
  {0x71, 0xC9},
  {0x81, 0xCA},
  {0x91, 0xCB},
  {0xA1, 0xCC},
  {0xB1, 0xCD},
  {0xC1, 0xCE},
  {0xD1, 0xCF},
  {0xF1, 0xE0},
  {0x11, 0xE1},
  {0x21, 0xE2},
  {0x31, 0xE3},
  {0x41, 0xE4},
  {0x51, 0xE5},
  {0x61, 0xE6},
  {0x71, 0xE7},
  {0x81, 0xE8},
  {0x91, 0xE9},
  {0xA1, 0xEA},
  {0xB1, 0xEB},
  {0xC1, 0xEC},
  {0xD1, 0xED},
  {0xE1, 0xEE},
  {0xF1, 0xEF},

};

// emulation control, this will be in GlobalSettings soon
int trace_enabled          = 1;     // real-time tracing is enable
int trace_outside          = 1;     // trace only addresses > 0x3000
int trace_int              = 1;     // only tracing of internal emulation actions
int trace_disassembler     = 1;     // disassembler is enabled

int emu_rom                = 1;     // rom emulation enabled
int emu_mldl               = 1;     // MLDL/QRAM emulation enabled
int emu_hepax              = 0;     // HEPAX emulation enabled
int emu_HP82143            = 0;     // HP82143 emulation enabled
int emu_HPIL               = 0;     // HP-IL emulation enabled
int emu_display            = 0;     // HP41 display emulation enabled
int emu_wand               = 0;     // HP41 WAND emulation enabled
int emu_blinky             = 0;     // HP41 IR Printer emulation enabled

int sample_break           = 0;     // start trace sampling after nn traces, for very long traces

 
char  UPrint[250];        // used for building cli messages
int   UPrintLen = 0;

uint32_t flash_dump_addr = 0;
uint32_t fram_dump_addr = 0;

int rom_target = -1;                // target ROM page for FLASH or FRAM programming
typedef enum {                      // memory type for ROM programming
  P_NONE,                           // no programming possible
  P_FLASH,                          // program in FLASH
  P_FRAM                            // program in FRAM
} prog_target;

prog_target prog_t = P_NONE;

extern struct TLine TraceLine;      // the variable with the TraceLine

extern CModules TULIP_Pages;
extern Filter TraceFilter;

bool enable_programming = false;    // enable FLASH/FRAM programming

char ROMname[16]; // 16 chars will do normally
char ROMrev[6];
const char __in_flash()HPchar[] = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ |\"#$%&`()*+{-}/0123456789†,<=>?"; 


void testglob()
{
  globsetting.save();
}

// memory statistics functions
uint32_t getTotalHeap() {
   extern char __StackLimit, __bss_end__;
   return &__StackLimit  - &__bss_end__;
}

uint32_t getFreeHeap() {
   struct mallinfo m = mallinfo();
   return getTotalHeap() - m.uordblks;
}

// wake up the HP41 by driving ISA high
void power_on()
{
  wakemeup_41();
}

int pcount = 0;

// Serial loop command structure
typedef struct
{
  char key;
  FPTR fn;
  const char *desc;
} SERIAL_COMMAND;

void serial_help();
void print_help();
void rom_help();
//void pio_status();

// Serial loop command structure for the printer sub-functions
typedef struct
{
  char key;
  FPTR fn;
  const char *desc;
} PRINT_COMMAND;

// Serial loop command structure for the ROM sub-functions
typedef struct
{
  char key;
  FPTR fn;
  const char *desc;
} ROM_COMMAND;

// Serial loop command structure for the TRACE sub-functions
typedef struct
{
  char key;
  FPTR fn;
  const char *desc;
} TRACE_COMMAND;

typedef enum {
  // used for parameter entry
  ST_NONE,        // no state
  ST_QRAM,        // entry of QRAM parameter (not yet used)
  ST_PLUG,        // entry of plug-in module (not yet used)
  ST_BRK,         // entry of breakpoint (not yet used)
  ST_PRT,         // entry of printer function
  ST_XMEM,        // entry of active Extended Memory Modules 
  ST_FILE,        // entry of file submenu
  ST_ROM,         // entry of ROM submenu
  ST_TRACE,       // entry of TRACE subcommands
  ST_WAND,        // entry of Wand subcommands (0..9)
} State_e;    

State_e state = ST_NONE; 

// functions called from the CLI



// program a delay as a busy wait while updating the USB CDC interface
// returns true if the delay was cancelled by a keypress
bool uif_delay_cancel(int ms)
{
  int sleepcount = ms / 2;  // 2 ms per loop, so divide by 2

  while(true) {
    tud_task();                       // to process IO until the watchdog triggers
    sleep_ms(2);
    if (cdc_available(ITF_CONSOLE)) {
      cdc_read_flush(ITF_CONSOLE);
      cli_printf("  key pressed, delay cancelled");
      return true;
    }
    sleepcount--;
    if (sleepcount < 0) {  
      return false;  // delay expired  
    }
  }
  return true; // delay was cancelled
}


void measure_freqs(void) {
    uint32_t f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint32_t f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint32_t f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    uint32_t f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint32_t f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint32_t f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint32_t f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
#ifdef CLOCKS_FC0_SRC_VALUE_CLK_RTC
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);
#endif

    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
#ifdef CLOCKS_FC0_SRC_VALUE_CLK_RTC
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);
#endif

    // Can't measure clk_ref / xosc as it is the ref
}


// CLI Welcome message and status
void uif_status()
{

    measure_freqs();  // prints to serial console

    uint32_t f_pll_sys  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint32_t f_pll_usb  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint32_t f_rosc     = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    uint32_t f_clk_sys  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint32_t f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint32_t f_clk_usb  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint32_t f_clk_adc  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);

    // adc_select_input(4);                    // select the temperature sensor input
    const float conversionFactor = 3.3f / (1 << 12); // 3.3V reference voltage, 12-bit ADC
    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    float speed = clock_get_hz(clk_sys)/MHZ;

    float totalheap = getTotalHeap() / 1024;
    float freeheap = getFreeHeap() / 1024;

    // show the current tracer status
    int TraceSize = globsetting.get(tracer_mainbuffer);
    float tracebytes = (sizeof(TraceLine) * TraceSize) / 1024;

    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);

    char board_id_str[sizeof(board_id.id) * 2 + 1];
    for (int i = 0; i < sizeof(board_id.id); i++) {
        sprintf(board_id_str + i * 2, "%02X", board_id.id[i]);
    }

    cli_printf("****************************************************************************");
    cli_printf("*    __   ___    __");
    cli_printf("*   /  | |   |  /  | /|  Welcome to TULIP4041");   
    cli_printf("*  /___| |   | /___|  |  The ULtimate Intelligent Peripheral for the HP41");
    cli_printf("*      | |___|     |  |  (C) 2025 Meindert Kuipers, Netherlands");
    if (TULIP_HARDWARE == T_DEVBOARD) {
      cli_printf("*                        Firmware for the DevBoard version!");
    } else {
      cli_printf("*                        Firmware for the Module version!");
    }
    cli_printf("*  Firmware: %s -- Compiled %s %s", Version_Number, __DATE__, __TIME__);

    #ifdef DEBUG
      cli_printf("*  *** DEBUG build, not for production use! ***");
    #endif

    char ser_string[32] = {0}; // buffer for serial string
    if (otp_read_serial(ser_string)) {
      // reading OK, check if strin is empty
      if (strlen(ser_string) == 0) {
        cli_printf("*  no serial string programmed");
      } else {
        // print the serial number
        cli_printf("*  Hardware: %s", ser_string);
      }
    } else {
      cli_printf("*  error reading serial string");
    }

    // now read the owner string, if any
    // map the string at FF_OWNER_BASE to a char
    char *owner_string = (char *)FF_OWNER_BASE;  // buffer for the owner string, fixed address in FLASH
    // check if the owner string is already programmed
    if (owner_string[0] < 0x20 || owner_string[0] > 0x7E) {
      // the first character is not printable, so no owner string programmed
      cli_printf("*  Owner   : no owner string programmed");
    } else {
      cli_printf("*  Owner   : %s", owner_string);
    }

    cli_printf("*");
    cli_printf("*  Clock overview");
    cli_printf("*    TULIP freq : %7.2lf MHz", speed);
    cli_printf("*      pll_sys  : %7d kHz", f_pll_sys);
    cli_printf("*      pll_usb  : %7d kHz", f_pll_usb);
    cli_printf("*      rosc     : %7d kHz", f_rosc);
    cli_printf("*      clk_sys  : %7d kHz", f_clk_sys);
    cli_printf("*      clk_peri : %7d kHz", f_clk_peri);
    cli_printf("*      clk_usb  : %7d kHz", f_clk_usb);
    cli_printf("*      clk_adc  : %7d kHz", f_clk_adc);
    cli_printf("*      cpu temp :   %.02f C, %.02f F", tempC, tempC*9/5+32);
    cli_printf("*      board id :  %s (unique RP2350 CPU identifier)", board_id_str);
    cli_printf("*"); 
    cli_printf("*  Memory usage");
    float totalram = 520; // total RAM in the RP2350 is 520 KBytes
    cli_printf("*    Total RAM  : %7.2lf KBytes", totalram);
    cli_printf("*    Total heap : %7.2lf KBytes", totalheap);
    cli_printf("*    Free heap  : %7.2lf KBytes", freeheap);
    cli_printf("*    Tracebuffer: %7.2lf KBytes, %d samples = %d bytes/traceline", tracebytes, TraceSize, sizeof(TraceLine));
    cli_printf("*");    
    cli_printf("****************************************************************************");
}

// PIO status
void uif_pio_status()
{
    uif_pio_report();
} 


// CDC connection status
void uif_cdc_status()
{
  cli_printf("  CDC Port 1 [console]  connected? %s", cdc_connected(ITF_CONSOLE) ? "yes":"no");
  cli_printf("  CDC Port 2 [tracer]   connected? %s", cdc_connected(ITF_TRACE)   ? "yes":"no");
  cli_printf("  CDC Port 3 [HP-IL]    connected? %s", cdc_connected(ITF_HPIL)    ? "yes":"no");
  cli_printf("  CDC Port 4 [IL scope] connected? %s", cdc_connected(ITF_ILSCOPE) ? "yes":"no");
  cli_printf("  CDC Port 5 [printer]  connected? %s", cdc_connected(ITF_PRINT)   ? "yes":"no");
}

// identify all CDC ports
void uif_cdc_ident()
{
  // for testing 
  int i;
  int PrintLen;
  char  PrintStr[200];

  cli_printf("\n");

  for (i = 0; i < 5; i++)
  {
    if (cdc_connected(i))
    {
      cli_printf("  CDC Port %d OK  connected: %s", i, ITF_str[i]);

      PrintLen = 0;
      PrintLen += sprintf(PrintStr + PrintLen,"\n  *** testing CDC Port %i: %s ***\n\r", i, ITF_str[i]);
      cdc_send_string(i, PrintStr, PrintLen);
      cdc_flush(i);
    }
    else
    {
      cli_printf("  CDC Port %d NOT connected: %s", i, ITF_str[i]);
    }
  }
}

// Reboot the RP2350, with a 2 second delay to allow a cancel
void uif_reboot()
{
  cli_printf("  RESETTING THE TULIP4041 in 2 seconds !! press any key to cancel");
  watchdog_enable(2000, 1);   // set the watchdog to 2 seconds
  while(true) {
    tud_task();                 // to process IO until the watchdog triggers

    // check for any input to disable the timer
    if (cdc_available(ITF_CONSOLE)) {
      // stop the watchdog
      hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
      cdc_read_flush(ITF_CONSOLE);
      cli_printf("  reset cancelled");
      return;
    }
  }
}


void uif_bootsel()
{
  uint sleepcount = 500;

  cli_printf("  RESETTING THE TULIP4041 to BOOTSEL mode in 2 seconds!! press any key to cancel");
  cli_printf("  will unplug all embedded modules before the REBOOT\n\n");

  while(true) {
    tud_task();                 // to process IO until the watchdog triggers
    sleep_ms(2);
    if (cdc_available(ITF_CONSOLE)) {
      cdc_read_flush(ITF_CONSOLE);
      cli_printf("  reset cancelled");
      return;
    }
    sleepcount--;
    if (sleepcount == 0) {  
      // unplug any embedded modules here
      for (int page = 4; page < 16; page++) {
        if (TULIP_Pages.isPlugged(page, 1) && TULIP_Pages.isEmbeddedROM(page, 1)) {
          // page is plugged and embedded, so unplug it and all banks
          TULIP_Pages.unplug(page, 1);
          TULIP_Pages.unplug(page, 2);
          TULIP_Pages.unplug(page, 3);
          TULIP_Pages.unplug(page, 4);
          if (page == 6) {
            // Printer Page
            globsetting.set(HP82143A_enabled, 0); // disable the Printer ROM in the settings
          } else if (page == 7) {
            // HPIL Page
            globsetting.set(HP82160A_enabled, 0); // disable the HP82160A ROM in the settings
          }
          globsetting.save();
          TULIP_Pages.save();
        }
      }

      // reboots the RP2350 when the counter expires, uses the standard LED for activity monitoring
      reset_usb_boot(1<<PICO_DEFAULT_LED_PIN, 0);      
    }
  }
}

// blink the LED b times, just for testing and fun
void uif_blink(int b)
{
  if (b == 0) {
    // toggle the LED once, to show that the command was received
    cli_printf("toggle LED status");
    gpio_toggle(PICO_DEFAULT_LED_PIN);
    return;
  }
  while ((b > 0) && (b < 10)) {
    // blink the LED b times, just for testing and fun
    cli_printfn("%d ", b);
    gpio_toggle(PICO_DEFAULT_LED_PIN);
    sleep_ms(250);
    gpio_toggle(PICO_DEFAULT_LED_PIN);
    sleep_ms(250);
    b--;
  }
  cli_printf("");
}

// helper function, returns true if PWO is low
// returns false and sends a message to the CLI if PWO is high
bool uif_pwo_low()
{
    if (gpio_get(P_PWO) == 0) {
      return true;
    }
    cli_printf("  PWO is high, function only permitted when HP41 is OFF or STANDBY");
    return false;
}

void uif_calcreset() {
  // drive PWO low, 
  cli_printf("  forcing PWO low to reset the HP41");
  shutdown_41();
}

void uif_poweron() {
  // can only be done if PWO is low!
  if (uif_pwo_low()) {
    cli_printf("  driving ISA to power on the HP41");
    wakemeup_41();
  }
}

void uif_configinit() {
  // can only be done if PWO is low!
    if (uif_pwo_low()) {
      cli_printf("  re-initializing all peristent settings");
      globsetting.set_default();
    }
}

void uif_configlist() {
  // can only be done if PWO is low!
    if (uif_pwo_low()) {
      for (int i = 0; i <= rommap + 1; i++) {
        cli_printf("  item# %2d  value %04X - %s", i, globsetting.get(i), glob_set_[i]);
      }
    }
}

void uif_owner(const char *str)
{
  // show/program the owner of the device
  // the owner string is stored in FLASH just under the FLASH File System
  // Address is fixed at FF_OWNER_BASE

  // only when calculator is not running
  if (!uif_pwo_low()) return;

  // map the string at FF_OWNER_BASE to a char
  char *owner_string = (char *)FF_OWNER_BASE;  // buffer for the owner string, fixed address in FLASH

  if (str == NULL) {
    // read the owner string

    #ifdef DEBUG
    cli_printf("  reading TULIP4041 owner string from FLASH memory");
    #endif

    // check if the owner string is already programmed
    if (owner_string[0] < 0x20 || owner_string[0] > 0x7E) {
      // the first character is not printable, so no owner string programmed
      cli_printf("  no proper string programmed at address: 0x%08X", FF_OWNER_BASE);
      // print the first 32 characters as hex
      for (int i = 0; i < 32; i++) {
        cli_printfn("  %02X ", owner_string[i]);
      }
      cli_printf("");
      return;
    }

    #ifdef DEBUG
    cli_printf("  owner string address: 0x%08X", owner_string);
    #endif

    // print the owner string
    cli_printf("  owner: \"%s\"", owner_string);

  } else {
    // program string as new owner string to FLASH

    // check if the first 256 bytes of FLASH at address FF_OWNER_OFFSET is erased
    if (ff_erased_block(FF_OWNER_OFFSET, 256, 1) != NOTFOUND) {
      // first erase the page
      cli_printf("  erasing FLASH at address: 0x%08X", FF_OWNER_OFFSET);
      ff_erase_block(FF_OWNER_OFFSET);
    }

    // now ready to program the new string
    // copy the string in a 256 byte buffer
    char buffer[256];
    int len = strlen(str);
    memset(buffer, 0xFF, sizeof(buffer));       // ensure that the buffer contains all FF's
    strncpy(buffer, str, len);          // copy the new owner string
    buffer[len] = '\0';                 // ensure null termination

    // program the new owner string to FLASH
    cli_printf("  new owner string: \"%s\" to \"%s\"", str, buffer);
    cli_printf("  programming new owner string to FLASH at address: 0x%08X", FF_OWNER_OFFSET);
    ff_program_block(FF_OWNER_OFFSET, (uint8_t *)buffer, sizeof(buffer));

    // and show the new owner string
    cli_printf("  owner string address: 0x%08X", owner_string);
    cli_printf("  owner string value  : \"%s\"", owner_string);
  }
}


void uif_serial(const char *str)
{
  // program/read the TULIp serial number
  // program if str has the correct format, otherwise just read the serial number
  // the string programmed is "TULIP4041 HW V0.9 serial #xxxx", 31 chars
  // the string must be 4 characters with the serial, the start of the string is fixed
  char ser_string[32] = "TULIP4041 HW V1.0 serial #xxxx";   // this string is 30 characters long
                                                            // xxx is the placeholder for the serial number

  if (str == NULL) {
    // read the serial number
    cli_printf("  reading TULIP4041 serial string from OTP memory");
    if (otp_read_serial(ser_string)) {
      // reading OK, check if strin is empty
      if (strlen(ser_string) == 0) {
        cli_printf("  serial string is empty, no serial string programmed");
      } else {
        // print the serial number
        cli_printf("  serial string: %s", ser_string);
      }
    } else {
      cli_printf("  error reading TULIP4041 serial string");
    }
    return;
  }

  // there was an argument passed, the intention is to program a serial number

  // check the string length
  if (strlen(str) != 4) {
    cli_printf("  error: serial number must be 4 characters long");
    return;
  }

  // string must be 4 numbers, so check if all characters are digits
  for (int i = 0; i < 4; i++) {
    if ((str[i] < '0') || (str[i] > '9')) {
      cli_printf("  error: serial number must be 4 digits");
      return;
    }
  }

  // now happy with the string, program it
  // append the number to the fixed string replacing the xxxx in the string

  // find the xxxx in the string and replace it with the serial number

  char *pos = strstr(ser_string, "xxxx");
  if (pos == NULL) {
    // this should never happen, but just in case
    cli_printf("  error: serial number string does not contain 'xxxx' placeholder");
    return;
  }
  // replace the xxxx with the serial number
  strncpy(pos, str, 4);  // copy the serial number to the string
  pos[4] = '\0';         // terminate the string after the serial number

  cli_printf("  programming serial string: %s", ser_string);

  // give the user 4 seconds to cancel the operation
  cli_printf("  press any key within 5 seconds to cancel the operation");
  if (uif_delay_cancel(5000)) {
    cli_printf("  serial string programming cancelled");
    return;
  }

  if (otp_write_serial(ser_string)) {
    // writing OK
    cli_printf("  serial string programmed: %s", ser_string);
  } else {
    cli_printf("  error programming TULIP4041 serial number");
  }

  // read the serial number again to verify
  if (otp_read_serial(ser_string)) {
    // reading OKp
    cli_printf("  serial string: %s", ser_string);
  } else {
    cli_printf("  error reading serial string");
  }
}


// show the status of all GPIO pins
void uif_gpio_status()
{
  cli_printf("  GPIO status:");
  for (int i = 0; i < 30; i++) {
      cli_printf("  GPIO %2d: %s  -  %s", i, gpio_get(i) ? "HI" : "LO", gpio_names_[i]);
  }
}


void uif_dir(const char *dir)
{
  sd_dir(dir);
}


// all sdcard functions
void uif_sdcard_status()      // uSD card status
{
  sd_status();
}

void uif_sdcard_mount()       // mount the uSD card
{
  sd_mount();
}

void uif_sdcard_unmount()     // unmount the uSD card
{
  sd_unmount();
}

void uif_sdcard_mounted()     // check if the SD card is mounted
{
  sd_mounted();
}

void uif_sdcard_eject()
{
  usb_sd_eject();
}

void uif_sdcard_connect()
{
  usb_sd_connect();
}

const char* file_types[] =
// list of file types for the import command
{
    "ROM",
    "MOD",
    "ilprinter",
    "printer",
};

// copmare the current open file with the one in FLASH at offset offs
// the file is already open, and the file in FLASH is found at offs
// returns the following values:
// 0 - file is not in FLASH or file read error
// 1 - file is in FLASH and the same
// 2 - file is in FLASH but different and can be updated without erasing
// 3 - file is in FLASH but different and requires erasing before updating
// 4 - file is in FLASH but with different size or type and cannot be updated

#define COMPARE_NOT_FOUND 0
#define COMPARE_SAME 1
#define COMPARE_DIFFERENT 2
#define COMPARE_DIFF_ERASE 3
#define COMPARE_DIFF_SIZE 4

int compare_openfile(FIL* fp, uint32_t offs)  
{
  uint8_t buf[0x1000];          // 4K buffer

  uint filesize = f_size(fp);   // get the filesize

  #ifdef DEBUG
    cli_printf("  comparing open file size %d bytes with FLASH file at %08X", filesize, offs);
  #endif

  // no check is done of the type of file, this is done in the import function

  // now map the header of the file in FLASH
  ModuleMetaHeader_t *MetaH;          // pointer to meta header for getting info of next file
  MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct

  #ifdef DEBUG
  cli_printf("  filename                         type  size      address     next file");
  cli_printf("  -------------------------------  ----  --------  ----------  ----------");
  cli_printf("  %-31s  0x%02X  %8d  0x%08X  0x%08X", 
                MetaH->FileName, MetaH->FileType, MetaH->FileSize, offs, MetaH->NextFile);
  #endif

  // verify size infomation
  if (MetaH->FileSize != filesize) {
    // cli_printf("  file size in FLASH is %d, different from file on uSD card, cannot update", MetaH->FileSize);
    return COMPARE_DIFF_SIZE;
  }

  // now read the file from the uSD card in chunks of 4K compare with the file in FLASH
  uint32_t addr = 0;

  // map the contents of the file in FLASH
  uint8_t *flash_buf = (uint8_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t));

  UINT read = 0;
  uint32_t toread = filesize;
  uint32_t readsize = 0x1000;
  bool result = true;
  FRESULT fr = f_lseek(fp, 0);    // set the file pointer to the beginning of the file
  bool is_writable = true;
  bool is_different = false;
  uint8_t flash_val = 0;  // byte read from FLASH

  while ((toread > 0) && result) {
    if (toread < readsize) readsize = toread;
    fr = f_read(fp, buf, readsize, &read);
    if (FR_OK != fr) {
      cli_printf("  file read error: %s (%d)", FRESULT_str(fr), fr);
      return COMPARE_NOT_FOUND;
    }

    // compare the buffer with the FLASH
    // result = (memcmp((void*)addr, buf, read) == 0);   // memcmp returns 0 if the buffers are equal

    // do a byte-by-byte compare to check if the byte is different
    // if it is different check if is can be overwritten in FLASH without prior erasing
    for (int i = 0; i < read; i++) {

      if (buf[i] != flash_buf[addr + i]) {
        // bytes are different the same, now check if programming is possible
        is_different = true;          // at least one byte is different

        flash_val = flash_buf[addr + i] & buf[i];    // this is the AND
        flash_val ^= buf[i];                  // XOR to check if any bits need to change from 0 to 1, which is not allowed in FLASH
        if (flash_val != 0) {
        // bits need to change from 0 to 1, which is not allowed in FLASH
          is_writable = false;
          #ifdef DEBUG
          cli_printf("  bytes at %08X differ: FLASH: %02X - File: %02X", addr + i, flash_buf[addr + i], buf[i]);
          #endif
          return COMPARE_DIFF_ERASE;
        }
      }
    }

    //prepare for next 4K
    toread -= read;
    addr += read;
  }

  // now check the result of the comparison
  // if the files are equal, return COMPARE_SAME
  if (is_different) {
        return COMPARE_DIFFERENT;
      } else {
        return COMPARE_SAME;
      }
}

// compare a file with the one in FLASH
// returns the following values:
// 0 - file is not in FLASH or file read error
// 1 - file is in FLASH and the same
// 2 - file is in FLASH but different and can be updated without erasing
// 3 - file is in FLASH but different and requires erasing before updating
// 4 - file is in FLASH but with different size or type and cannot be updated


int compare_file(const char *fname)
{
  uint8_t buf[0x1000];          // 4K buffer
  char ffname[32];

  FIL fil;
  FRESULT fr = f_open(&fil, fname, FA_READ);
  if (FR_OK != fr) {
      cli_printf("  cannot open file: %s, %s (%d)", fname, FRESULT_str(fr), fr);
      return COMPARE_NOT_FOUND;
  }

  // make a copy of fname in ffname
  strcpy(ffname, fname);

  // convert the filename to upper case
  for (int i = 0; i < strlen(ffname); i++) {
    ffname[i] = toupper(ffname[i]);
  }

  // get the file extension from the filename
  char *ext = strrchr(ffname, '.');

  // file is open now for reading, first get the file size
  uint filesize = f_size(&fil);
  cli_printf("  file %s opened, size %d bytes, type is %s", fname, filesize, ext);

  // remove the directory indicator from the filename
  char *p = strrchr(fname, '/');
  if (p != NULL) {
    fname = p + 1;
  }

  // determine file type from the extension
  int type = 0;
  if (strcmp(ext, ".MOD") == 0) type = 1;     // can be MOD1 or MOD2
  if (strcmp(ext, ".ROM") == 0) type = 3;     // ROM file
  if (strcmp(ext, ".UMM") == 0) type = 4;     // User Memory
  if (strcmp(ext, ".EXT") == 0) type = 5;     // Extended Memory
  if (strcmp(ext, ".EXP") == 0) type = 6;     // Expanded Memory
  if (strcmp(ext, ".TRM") == 0) type = 7;     // ROM mapping
  if (strcmp(ext, ".TGL") == 0) type = 8;     // Global settings
  if (strcmp(ext, ".TTF") == 0) type = 9;     // Tracer settings

  // check if the file type is supported
  if (type == 0) {
    cli_printf("  file type not supported");
    f_close(&fil);
    return COMPARE_NOT_FOUND;
  }

  // check if the file exists in FLASH
  uint32_t offs = ff_findfile(fname); 
  if (offs == NOTFOUND) {
    cli_printf("  file not in FLASH");
    f_close(&fil);
    return COMPARE_NOT_FOUND;
  }

  cli_printf("  file in FLASH at 0x%08X", offs);

  // now map the header of the file in FLASH
  ModuleMetaHeader_t *MetaH;          // pointer to meta header for getting info of next file
  MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct
  cli_printf("  filename                         type  size      address     next file");
  cli_printf("  -------------------------------  ----  --------  ----------  ----------");
  cli_printf("  %-31s  0x%02X  %8d  0x%08X  0x%08X", 
                MetaH->FileName, MetaH->FileType, MetaH->FileSize, offs, MetaH->NextFile);
  
  // verify size infomation
  if (MetaH->FileSize != filesize) {
    cli_printf("  file size in FLASH is different, cannot update");
    f_close(&fil);
    return COMPARE_DIFF_SIZE;
  }

  // now read the file from FLASH and compare with the file on the uSD card
  uint32_t addr = FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t);
  UINT read = 0;
  uint32_t toread = filesize;
  uint32_t readsize = 0x1000;
  bool result = true;

  while ((toread > 0) && result) {
    if (toread < readsize) readsize = toread;
    fr = f_read(&fil, buf, readsize, &read);
    if (FR_OK != fr) {
      cli_printf("  file read error: %s (%d)", FRESULT_str(fr), fr);
      f_close(&fil);
      return COMPARE_NOT_FOUND;
    }

    // compare the buffer with the FLASH
    result = (memcmp((void*)addr, buf, read) == 0);

    toread -= read;
    addr += read;
  }

  f_close(&fil);        // compare done, close file

  cli_printf("  file %s %s", fname, result ? "is the same" : "is different");

  if (result) return COMPARE_SAME;
  return COMPARE_DIFFERENT;
  
}

// write a function to compare strings with wildcards
// returns true if the strings match, false otherwise
// park this code for now, not used
bool wildcard_match(const char *str, const char *pattern) {
  while (*pattern) {
    if (*pattern == '*') {
      pattern++;
      if (!*pattern) return true; // Trailing '*' matches everything
      while (*str) {
        if (wildcard_match(str, pattern)) return true;
        str++;
      }
      return false;
    } else if (*pattern == '?') {
      if (!*str) return false; // '?' must match a character
      str++;
      pattern++;
    } else {
      if (*str != *pattern) return false; // Exact match required
      str++;
      pattern++;
    }
  }
  return !*str && !*pattern; // Both strings must be at the end
}

// import a file with a given filename
// import a file and program in FLASH
// also called by the import_all function
// a2 (option) and a3 are passed with the following values:
// a2              a3
// import_no_arg   import_no_arg            import [file]
// import_update   import_no_arg            import [file] UPDATE
// import_compare  import_no_arg            import [file] compare
//

// the following values are returned:
#define import_nofile             0             // no file found or cannot open file
#define import_nosupport          1             // file type not supported
#define import_file_same          2             // files are identical, no update needed
#define import_file_notinflash    3             // file not found in FLASH
#define import_file_sizediff      4             // file size does not match, cannot compare or update
#define import_file_diff_erase    5             // file is different and requires erasing before updating
#define import_file_diff          6             // file is different and can be updated without erasing
#define import_file_exists        7             // file exists and cannot be imported again
#define import_file_imported      8             // file imported successfully
#define import_filesys_full       9             // import not OK, file system full
#define import_nospace           10            // import not OK, not enough space in FLASH


int import_file(const char *fname, int option)
{
  uint8_t buf[0x1000];          // 4K buffer
  char ffname[32];
  int result = 0;
  int rslt = 0;

  // first sort out the file to be imported
  FIL fil;
  FRESULT fr = f_open(&fil, fname, FA_READ);
  if (FR_OK != fr) {
      cli_printf("  cannot open file: %s, %s (%d)", fname, FRESULT_str(fr), fr);
      return import_nofile;
  }

  // make a copy of fname in ffname
  strcpy(ffname, fname);

  // convert the filename to upper case
  for (int i = 0; i < strlen(ffname); i++) {
    ffname[i] = toupper(ffname[i]);
  }

  // get the file extension from the filename
  char *ext = strrchr(ffname, '.');
  
  // file is open now for reading, first get the file size
  uint filesize = f_size(&fil);
  cli_printfn("  file %-31s opened, size %8d bytes, type is %-20s  ", fname, filesize, ext);

  // remove the directory indicator from the filename
  char *p = strrchr(fname, '/');
  if (p != NULL) {
    fname = p + 1;
  }

  // determine file type from the extension
  int type = 0;  
  if (strcmp(ext, ".MOD") == 0) type = 1;     // can be MOD1 or MOD2
  if (strcmp(ext, ".ROM") == 0) type = 3;     // ROM file
  if (strcmp(ext, ".UMM") == 0) type = 4;     // User Memory
  if (strcmp(ext, ".EXT") == 0) type = 5;     // Extended Memory
  if (strcmp(ext, ".EXP") == 0) type = 6;     // Expanded Memory
  if (strcmp(ext, ".TRM") == 0) type = 7;     // ROM mapping
  if (strcmp(ext, ".TGL") == 0) type = 8;     // Global settings  
  if (strcmp(ext, ".TTF") == 0) type = 9;     // Tracer settings

  // check if the file type is supported  
  if (type == 0) {
    cli_printf("  file type not supported"); 
    f_close(&fil);
    return import_nosupport;
  }

  // check if the file already exists in FLASH
  // if it exists maybe the UPDATE or compare option is used
  uint32_t offs = ff_findfile(fname);
  
  if (offs != NOTFOUND) {
    cli_printf("  file found in FLASH at 0x%08X", offs);
    // file exists, is this UPDATE or compare?
    if ((option == import_update) || (option == import_compare)) {
      // file UPDATE or compare, first compare the file
      result = compare_openfile(&fil, offs);
      if (result == COMPARE_SAME) {
        cli_printf("  file in FLASH is identical, no update needed");
        f_close(&fil);
        return import_file_same;
      }
      if (result == COMPARE_NOT_FOUND) {
        // in case the file is not found in FLASH, should never get here
        cli_printf("  file not found in FLASH");
        f_close(&fil);
        return import_file_notinflash;
      }
      if (result == COMPARE_DIFF_SIZE) {
        cli_printf("  file size in FLASH is different, cannot compare or update");
        f_close(&fil);
        return import_file_sizediff;
      }
      if (result == COMPARE_DIFF_ERASE) {
        cli_printf("  file in FLASH is different, requires erasing before updating");
        rslt = import_file_diff_erase;
      }
      if (result == COMPARE_DIFFERENT) {
        cli_printf("  file in FLASH is different, can update without erasing");
        rslt = import_file_diff;
      }
    } else {
      // file exists in FLASH, but no UPDATE or compare requested
      cli_printf("  file already in FLASH, cannot import again");     
      f_close(&fil);
      return import_file_exists;
    }

    // we get here if the file was found in FLASH but different
    // if this was compare only we get out here
    // if this was an update whe can write the file with or without prior erasing
    if (option == import_compare) {
      f_close(&fil);
      return rslt;   // return the result of the comparison
    } 
  }

  // when we get here the file was not found and it is a new file
  // if we only do compare report it and get out, no programming then
  if (option == import_compare) {
    cli_printf("  file not found in FLASH, can be imported");
    f_close(&fil);
    return import_file_notinflash;
  }

  // offs now contains the address where to start programming
  // first construct the header 
  ModuleMetaHeader_t header;

  // the UPDATE programming is handled below
  // but only if the file already existed in FLASH
  // if the file did not exist in FLASH it is a new file
  if ((option == import_update) && (offs != NOTFOUND)) {
    // prepare the update by saving the header
    // first get a pointer to the current header
    ModuleMetaHeader_t *MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map FLASH header to struct
    memcpy(&header, MetaH, sizeof(ModuleMetaHeader_t));                             // copy the header to save it

    // here we do the erasing, if necessary
    if (result == COMPARE_DIFF_ERASE) {
      cli_printf("  erasing FLASH at 0x%08X until 0x%08X", offs, header.NextFile);
      ff_erase(offs, header.NextFile);
    }

    // file is now erased or does not need to be erased, the header is saved

  } else {
    // this is a regular import of a new file, prepare for programming
    // file not found in FLASH, prepare to import
    // first find the end of the file chain
    offs = ff_lastfree(0);
    if (offs == NOTFOUND ) {
      cli_printf("  no free space in FLASH");
      f_close(&fil);
      return import_filesys_full;
    }

    // check if the file will fit in FLASH  
    #ifdef DEBUG
      cli_printf("  last free space in FLASH at 0x%08X", offs);
      cli_printf("  now checking for any holes that are large enough for the file");
    #endif

    offs = ff_findfree(0, filesize); // find the next free space in FLASH

    // if offs is NOTFOUND, then there is no free space in FLASH
    #ifdef DEBUG
      cli_printf("  found free space in FLASH at 0x%08X", offs);
    #endif

    if (offs == NOTFOUND) {
      cli_printf("  no free space in FLASH for this file");
      f_close(&fil);
      return import_nospace;
    }

    if ((FF_SYSTEM_SIZE - offs) < (filesize + 256)) {
      cli_printf("  not enough space in FLASH for this file");
      f_close(&fil);
      return import_nospace;
    }

    // offs now contains the address where to start programming
    // first construct the header
    header.FileType = type;
    strcpy(header.FileName, fname);
    header.FileSize = filesize;

    // next file points to the next available file in the filesystem
    // unless we are at the end of the filesystem
    // first check the header of the current (*deleted?) file at offs
    ModuleMetaHeader_t *MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);    // map header to struct
    uint32_t nextoffs = MetaH->NextFile;  // get the next file offset from the header
    if (MetaH->FileType == FILETYPE_END) {
      // this is the end of the filesystem, header can be set to the file size
      header.NextFile = offs + ((filesize + sizeof(header) + 255) & ~255);
    } else {
      // this is not the end of the filesystem, so we can set the next file offset
      // to the next available file in the filesystem
      header.NextFile = nextoffs;
    }

    // check if flash can be programmed at all with ff_erased 
    uint32_t addr = ff_erased(offs, filesize + sizeof(header), 1);
    if (addr != NOTFOUND) {
      cli_printf("  FLASH not erased at 0x%08X", addr);
      cli_printf("  erasing FLASH at 0x%08X until 0x%08X", offs, header.NextFile);
      
      // now erase the FLASH at the offset offs until the next entry in the filesystem

      // disable all interrupts, flash and restore interrupts
      ff_erase(offs, header.NextFile);
    }
  }

  // when we get here the FLASH is ready for programming
  // the space is erased or does not need to be erased
  // the header is prepared
  
  // show the programming details in the CLI
  cli_printf("  flashing %-31s, type %04X, size %8d bytes at 0x%08X", header.FileName, header.FileType, header.FileSize, offs);

  // ensure that any unused buffer is all 0xFF
  memset(buf, 0xFF, 0x1000);

  // copy to header to the buffer
  memcpy(buf, &header, sizeof(ModuleMetaHeader_t));

  // and add the first part of the file
  UINT read;

  // in case this is an UPDATE, the filepointer must be reset
  f_lseek(&fil, 0);

  // create a pointer inside our buffer to the start of the file data
  uint8_t* pp = buf + sizeof(header);
  fr = f_read(&fil, pp, 0x1000 - sizeof(header), &read);
  if (FR_OK != fr) {
      cli_printf("  file read error: %s (%d)", FRESULT_str(fr), fr);
      f_close(&fil);
      return import_nofile;
  }  
  
  // pause for a moment to allow any pending USB transfers to complete
  ff_delay500();

  // ensure that a multiple of 256 bytes is programmed including the header
  // normally this is always 4K, but for small files it can be less
  int bytestoprogram = read + sizeof(header);
  bytestoprogram = (bytestoprogram + 0xFF) & ~0xFF;

  // disable all interrupts, flash and restore interrupts
  uint32_t ints = save_and_disable_interrupts();
  flash_range_program(FF_OFFSET + offs, buf, bytestoprogram);
  restore_interrupts (ints);

  // program the remaining part of the file
  int left = filesize - read;                 // bytes left to read from the file
  while (left > 0) {
    memset(buf, 0xFF, 0x1000);                // ensure that any unused buffer is all 0xFF
    fr = f_read(&fil, buf, 0x1000, &read);    // read up tp 4K bytes from the open file
    offs = offs + bytestoprogram;             // increase the offset from the previous flashing

    if (read != 0) {
    // work out number of bytes to program and ensure that a multiple of 256 bytes is programmed
      bytestoprogram = read;
      bytestoprogram = (bytestoprogram + 0xFF) & ~0xFF;

      // and flash the data
      ints = save_and_disable_interrupts();
      flash_range_program(FF_OFFSET + offs, buf, bytestoprogram);
      restore_interrupts (ints);
      left -= read;
    } else {
      break;
    }
  }

  // close the file
  f_close(&fil);
  return import_file_imported;
}


// import all files in the directory
// argument i is passed as follows:
// #define import_no_arg   0
// #define import_all      1
// #define import_update   2
// #define import_compare  3

void uif_import_all(const char *dir, int arg)
{
  int result[11] = {0};   // array to count the results of the import
  int res;

  if (!ff_isinited()) {
    cli_printf("  FLASH File system not initialized, please run INIT first");
    cli_printf("  Total space in FLASH appr %d Kbytes free", FF_SYSTEM_SIZE/1024);
    
    return;
  }

  // first open the directory
  char cwdbuf[FF_LFN_BUF] = {0};
  FRESULT fr; /* Return value */
  char const *p_dir;
  if (dir[0]) {
      p_dir = dir;
  } else {
      fr = f_getcwd(cwdbuf, sizeof cwdbuf);
      if (FR_OK != fr) {
          cli_printf("  cannot open directory: %s (%d)", FRESULT_str(fr), fr);
          return;
      }
      p_dir = cwdbuf;
  }

  cli_printf("  Import directory in FLASH: %s", p_dir);
  DIR dj = {};      /* Directory object */
  FILINFO fno = {}; /* File information */
  assert(p_dir);
  fr = f_findfirst(&dj, &fno, p_dir, "*");
  if (FR_OK != fr) {
    cli_printf("  cannot find first file: %s (%d)", FRESULT_str(fr), fr);
    return;
  }

  while (fr == FR_OK && fno.fname[0]) { /* Repeat while an item is found */
    tud_task();  // must keep the USB port updated

    // create string with full path and filename
    char fname[80];
    strcpy(fname, p_dir);
    strcat(fname, "/");
    strcat(fname, fno.fname);

    // filename is fno.fname	
    // first check if this is a real file or a directory
    // if the file is a directory then skip it
    if (fno.fattrib & AM_DIR) {
      cli_printf("  skipping subdirectory %s", fno.fname);
    } else {
      // no directory, import the file and pass the argument
      // to handle compare or UPDATE
      res = import_file(fname, arg);
      result[res]++;
      cli_printf("");
    }
    fr = f_findnext(&dj, &fno);     // Search for next item
  }

  // all done, close directory and finish
  f_closedir(&dj);

  // show results of mass importy/compare
  cli_printf("  import/compare done, results:");
  cli_printf("    files imported/updated successfully:    %3d", result[import_file_imported]);
  cli_printf("    files exist in FLASH, no re-import:     %3d", result[import_file_exists]);
  cli_printf("    files identical, no update needed:      %3d", result[import_file_same]);
  cli_printf("    files different, update no erase:       %3d", result[import_file_diff]);
  cli_printf("    files different, update with erase:     %3d", result[import_file_diff_erase]);
  cli_printf("    files size different, cannot update:    %3d", result[import_file_sizediff]);
  cli_printf("    files not found in FLASH:               %3d", result[import_file_notinflash]);
  cli_printf("    files type not supported:               %3d", result[import_nosupport]);
  cli_printf("    files could not be opened:              %3d", result[import_nofile]);
  cli_printf("    files could not be imported, no space:  %3d", result[import_nospace]);
  cli_printf("    files could not be imported, FS full:   %3d", result[import_filesys_full]);
}


// import a file and program in FLASH
// a2 and a3 are passed with the following values:
// a2              a3
// import_no_arg   import_no_arg            import [file]
// import_update   import_no_arg            import [file] UPDATE
// import_compare  import_no_arg            import [file] compare
// import_all      import_no_arg            import [directory] ALL
// import_all      import_update            import [directory] ALL UPDATE
// import_all      import_compare           import [directory] ALL compare
void uif_import(const char *fname, int a2, int a3)       
{
  int i = 0;

  if (!uif_pwo_low()) {
    cli_printf("  function only permitted when HP41 is OFF or STANDBY");  
    return;    // only do this when calc is not running
  }

  #ifdef DEBUG
  // show parameters
    cli_printf("  command: import %s %d %d", fname, a2, a3);
  #endif

  if (a2 == import_all) {
    // import all files in the directory
    // a3 can be import_no_arg, import_update or import_compare
    uif_import_all(fname, a3);
    return; 
  }

  // we get here for a single file import
  // a2 and a3 are passed with the following values:
  // a2              a3
  // import_no_arg   import_no_arg            import [file]
  // import_update   import_no_arg            import [file] UPDATE
  // import_compare  import_no_arg            import [file] compare
  import_file(fname, a2);   // does all the work for a single file
}


void ShowROMDetails(uint16_t *ROMoffset)
{
  // given an offset in FLASH to the ROM contents, show the details of the ROM file
  uint16_t *myROMImage;               // buffer pointer for the ROM image

  myROMImage = ROMoffset;

  int NumFunctions = swap16(myROMImage[1]);
  cli_printf("    XROM      %3d", swap16(myROMImage[0]));
  cli_printf("    # Funcs   %3d", NumFunctions);

  // get a pointer to the ROM name (first function)
  uint16_t ROMName_offs = ((swap16(myROMImage[3]) & 0xFF)) + ((swap16(myROMImage[2]) & 0xFF) * 256);
  if (swap16(myROMImage[2]) > 0xFF) 
  {
    // usercode ROM, do not decode, just mention it
    cli_printf("    ROMName    <USERCODE>");
  }
  else if ((ROMName_offs > 0x0FFF) && (NumFunctions !=0)){
    // ROM name in other ROM, do not decode, just mention it
    cli_printf("    ROMName    <in other Page>");
  }
  else if (NumFunctions == 0) {
    // No functions thus no ROM name
    cli_printf("    ROMName    <none> (no functions)");
  }
  else
  {
    char c = swap16(myROMImage[ROMName_offs - 1]);
    int i = 0;
    while ( ((c & 0xFF) < 0x40) && (i < 16)) {
      // only allowed chars
      ROMname[i] = HPchar[c & 0x3F];
      i++;
      c = swap16(myROMImage[ROMName_offs - 1 - i]);
    }
    // and add the last character of the ROM name plus a NULL
    ROMname[i] = HPchar[c & 0x3F];
    ROMname[i + 1] = 0;
    cli_printf("    ROMName    \"%s\" - first function entry at 0x%04X", ROMname, ROMName_offs);
  }

  // now the ROM Revision and checksum
  cli_printf("    Rev        %c%c-%c%c", HPchar[swap16(myROMImage[0xFFE]) & 0x3F], 
                                         HPchar[swap16(myROMImage[0xFFD]) & 0x3F], 
                                         HPchar[swap16(myROMImage[0xFFC]) & 0x3F], 
                                         HPchar[swap16(myROMImage[0xFFB]) & 0x3F]);
  cli_printf("    Checksum   0x%03X", swap16(myROMImage[4095]));      

}

void ShowROMDump(uint16_t *ROMoffset)
{
  // given an offset in FLASH to the ROM contents, dumps the ROM file

  char  ShowPrint[250];
  int   ShowPrintLen = 0;
  uint16_t addr = 0;                        // address counter
  uint16_t endaddr = addr + 0x1000;         // end of the dump
  int i = 0;                                // counter for the number of bytes in a line
  char c;
  uint16_t romword;
  int m;

  uint16_t *myROMImage;               // buffer pointer for the ROM image

  myROMImage = ROMoffset;

  // now generate the hex dump
  cli_printf("  dump of ROM contents");
  cli_printf("  address  00   01   02   03   04   05   06   07   08   09   0A   0B   0C   0D   0E   0F        ASCII");
  cli_printf("  ----    ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----  ----------------");

  do {
    tud_task();  // keep the USB port updated
    ShowPrintLen = 0;
    ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "  %04X    ", addr);       // print the address

    // print 16 bytes
    for (m = 0; m < 16; m++) {
      romword = swap16(myROMImage[addr]);
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%04X ", romword);
      addr++;
    }
    addr = addr - 16;
    // print byte values as characters
    ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "  ");
    for (m = 0; m < 16; m++) {
      c = swap16(myROMImage[addr]) & 0xFF;
      if ((c < 0x20) || (c > 0x7E)) {
        c = '.';
      }
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%c", c);
      addr++;
    } 
    cli_printf("%s", ShowPrint);
        
  } while (addr < endaddr);   // list 4K bytes by default
  i++;
}

void unpack_image(word *ROM, const byte *BIN)
  {
  int i;
  word *ptr=ROM;
  if ((ROM==NULL)||(BIN==NULL))
    return;
  for (i=0;i<5120;i+=5)
    {
    *ptr++=((BIN[i+1]&0x03)<<8) | BIN[i];
    *ptr++=((BIN[i+2]&0x0F)<<6) | ((BIN[i+1]&0xFC)>>2);
    *ptr++=((BIN[i+3]&0x3F)<<4) | ((BIN[i+2]&0xF0)>>4);
    *ptr++=(BIN[i+4]<<2) | ((BIN[i+3]&0xC0)>>6);
    }
  }


/*
  The format of a packed ROM file (.BIN format) is as follows:
  BIN - This format is used by Emu41 (J-F Garnier) and HP41EPC (HrastProgrammer).
      Note: HP41EPC uses BIN format but names them .ROM files.
      All bits are packed into 5120 bytes, 4 machine words are packed into 5 bytes:
        Byte0=Word0[7-0]
        Byte1=Word1[5-0]<<2 | Word0[9-8]
        Byte2=Word2[3-0]<<4 | Word1[9-6]
        Byte3=Word3[1-0]<<6 | Word2[9-4]
        Byte4=Word3[9-2]
*/

inline uint16_t getfrombin(void* binarray, uint16_t address) {
  uint8_t *bin = (uint8_t*)binarray;
  uint16_t res1, res2, result;
  uint16_t offset = (address * 5) / 4;            // offset in the packed ROM file of the first byte
  int shift1 = (address & 0x0003) * 2;
  int shift2 = 8 - shift1;
  uint16_t mask1 = 0xFF << shift1;
  uint16_t mask2 = 0xFF >> (shift2 -2);
  res1 = (bin[offset] & mask1) >> shift1;    // get the first part of the word
  res2 = (bin[offset + 1] & mask2);
  res2 = res2 << shift2; // get the second part of the word

  result = res1 + res2;
  return result;
}


void ShowMODDetails(ModuleFileHeader_t *MODoffset)
{
  // given an offset in FLASH to the ROM contents, show the details of the ROM file
  ModuleFileHeader_t *myMODImage;               // buffer pointer to the MOD file in FLASH
  ModuleHeader_t *myPageHeader;                 // pointer to the individual pages header
  uint8_t *myImage;                             // pointer to the image data

  int NumFunctions;

  myMODImage = MODoffset;
  int ModNumPages = myMODImage->NumPages;
  cli_printf("    MOD Format %s", myMODImage->FileFormat);
  cli_printf("    MOD Title  %s", myMODImage->Title);
  cli_printf("    MOD Pages  %d", myMODImage->NumPages);

  int i = 0;
  while (i < ModNumPages) {
    // show details of the individual pages
    // only works for MOD1 type MOD files
    myPageHeader = (ModuleHeader_t*)((uint32_t)MODoffset + sizeof(ModuleFileHeader_t)
                                                         + i * sizeof(ModuleHeader_t));

    cli_printf("   *Page %2d    %s", i, myPageHeader->Name);

    myImage = myPageHeader->Image;

    NumFunctions = getfrombin(myImage, 1);
    
    cli_printf("    XROM      %3d", getfrombin(myImage, 0));
    cli_printf("    # Funcs   %3d", NumFunctions);

    // get a pointer to the ROM name (first function)
    uint16_t ROMName_offs = ((getfrombin(myImage, 3) & 0xFF)) + ((getfrombin(myImage, 2) & 0xFF) * 256);
    if (getfrombin(myImage, 2) > 0xFF) 
    {
      // usercode ROM, do not decode, just mention it
      cli_printf("    ROMName    <USERCODE>");
    }
    else if ((ROMName_offs > 0x0FFF) && (NumFunctions != 0)){
      // ROM name in other ROM, do not decode, just mention it
      cli_printf("    ROMName    <in other Page>");
    }
    else if (NumFunctions == 0) {
      // no functions in this ROM
      cli_printf("    ROMName    <none> (no functions)");
    } else
    {
      char c = getfrombin(myImage, ROMName_offs - 1);
      int i = 0;
      while ( ((c & 0xFF) < 0x40) && (i < 16)) {
        // only allowed chars
        ROMname[i] = HPchar[c & 0x3F];
        i++;
        c = getfrombin(myImage, ROMName_offs - 1 - i);
      }
      // and add the last character of the ROM name plus a NULL
      ROMname[i] = HPchar[c & 0x3F];
      ROMname[i + 1] = 0;
      cli_printf("    ROMName    \"%s\" - first function entry at 0x%04x", ROMname, ROMName_offs);
    }
    // now the ROM Revision and checksum
    cli_printf("    Rev        %c%c-%c%c", HPchar[getfrombin(myImage, 0xFFE) & 0x3F], 
                                           HPchar[getfrombin(myImage, 0xFFD) & 0x3F], 
                                           HPchar[getfrombin(myImage, 0xFFC) & 0x3F], 
                                           HPchar[getfrombin(myImage, 0xFFB) & 0x3F]);
    cli_printf("    Checksum   0x%03X", getfrombin(myImage, 0xFFF));  
    i++;
  }

}


void ShowMODDump(ModuleFileHeader_t *MODoffset)
{
  // given an offset in FLASH to the ROM contents, dumps the first page of the MOD file
  ModuleFileHeader_t *myMODImage;               // buffer pointer to the MOD file in FLASH
  ModuleHeader_t *myPageHeader;                 // pointer to the individual pages header
  uint8_t *myImage;                             // pointer to the image data

  char  ShowPrint[250];
  int   ShowPrintLen = 0;
  uint16_t addr = 0;                        // address counter
  uint16_t endaddr = addr + 0x1000;         // end of the dump
  int i = 0;                                // counter for the number of bytes in a line
  char c;
  uint16_t romword;
  int m;

  myMODImage = MODoffset;
  int ModNumPages = myMODImage->NumPages;

  // we do not yet support MOD2 files
  if (strcmp(myMODImage->FileFormat, "MOD2") == 0) {
    cli_printf("  MOD2 files not yet supported");
    return;
  }


  ModNumPages = 1; // only show the first page
  while (i < ModNumPages) {
    // show details of the individual pages
    myPageHeader = (ModuleHeader_t*)((uint32_t)MODoffset + sizeof(ModuleFileHeader_t)
                                                         + i * sizeof(ModuleHeader_t));

    myImage = myPageHeader->Image;

    // now generate the hex dump
    cli_printf("  dump of ROM contents");
    cli_printf("  address  00   01   02   03   04   05   06   07   08   09   0A   0B   0C   0D   0E   0F        ASCII");
    cli_printf("  ----    ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----  ----------------");

    do {
      tud_task();  // keep the USB port updated
      ShowPrintLen = 0;
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "  %04X    ", addr);       // print the address

      // print 16 bytes
      for (m = 0; m < 16; m++) {
        romword = getfrombin(myImage, addr);
        ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%04X ", romword);
        addr++;
      }
      addr = addr - 16;
      // print byte values as characters
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "  ");
      for (m = 0; m < 16; m++) {
        c = getfrombin(myImage, addr) & 0xFF;
        if ((c < 0x20) || (c > 0x7E)) {
          c = '.';
        }
        ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%c", c);
        addr++;
      } 
      cli_printf("%s", ShowPrint);
        
    } while (addr < endaddr);   // list 4K bytes by default
    i++;
  }

}

void uif_rtc(int i, const char *args)    // RTC test functions
{

  // #define rtc_status      1
  // #define rtc_set         2
  // #define rtc_get         3    
  // #define rtc_reset       4
  // #define rtc_dump        5
  // #define display_test    6

  switch (i) {
    uint8_t buf[20];          // buffer for RTC data
    case 1:     // RTC status
      i2c_scan();
      return;
    case 2:     // RTC set, time string in args
      // args = "YYMMDDhhmmss" 
      // YY = year, MM = month, DD = day, hh = hour, mm = minute, ss = second
      // example: 230101120000 = 1 Jan 2023, 12:00:00
      // set RTC to this time
      if (strlen(args) != 12) {
        cli_printf("RTC set function requires 12 digits in YYMMDDhhmmss format");
        return;
      }

      buf[0] = 3;                                        // we start with register 3
      // convert the string in values for the RTC registers in BCD coded decimal
      buf[7] = ((args[0] - '0') << 4) + (args[1] - '0');   // year
      buf[6] = ((args[2] - '0') << 4) + (args[3] - '0');   // month
      buf[4] = ((args[4] - '0') <<4 ) + (args[5] - '0');   // day
      buf[5] = 0;                                        // day of week, not used
      buf[3] = ((args[6] - '0') << 4) + (args[7] - '0');   // hour
      buf[2] = ((args[8] - '0') << 4) + (args[9] - '0');   // minute
      buf[1] = ((args[10] - '0') << 4) + (args[11] - '0'); // second

      i2c_write_blocking(i2c1, PCF8523_ADDRESS, buf, 8, false);

      cli_printf(" RTC set");
      // no break here, to read the time back
      // fall through to case 3
    case 3:     // RTC get
      pcf8520_read_all(buf);
      cli_printf("  Date: %02x/%02x/%02x [YY/MM/DD]", buf[9], buf[8], buf[6]);
      cli_printf("  Time: %02x:%02x:%02x [HH:MM:SS]", buf[5], buf[4], buf[3] & 0x7F);

      // report power management status, should be 0b0010.0000
      cli_printf(" Reg[2] %02X ", buf[2]);

      if ((buf[2] & 0x04) == 0x00) {
        //BLF bit in control register 3 is 0, battery 
        cli_printf("  Battery OK, >2.5V");
      } else {
        cli_printf("  Battery LOW, <2.5V");
      }

      // report clock valid status
      if ((buf[3] & 0x80) == 0x00) {
        // bit 7 in the seconds register is 0, so clock is valid
        cli_printf("  Oscillator OK, RTC status is valid");
      } else {
        cli_printf("  Oscillator has been interrupted, RTC clock may not be valid");
      }
      return;
    case rtc_reset:     // RTC reset
      pcf8520_reset();
      return;
    case 5:     // RTC dump
      pcf8520_read_all(buf);
      
      for (int i = 0; i < 20; i++) {
        cli_printf("  Reg %02d - %02X ", i, buf[i]);
      }

      cli_printf("  Date: %02x/%02x/%02x [YY/MM/DD]", buf[9], buf[8], buf[6]);
      cli_printf("  Time: %02x:%02x:%02x [HH:MM:SS]", buf[5], buf[4], buf[3] & 0x7F);
      
      return;
    case 6:     // display test
      cli_printf("  Display test not implemented yet");
      // ssd1306_test();
      return;
    default:
      break;
  }


}


// list the pluggable ROM and MOD files in FLASH and FRAM
void uif_list(int i, const char *fname)
{
  // list the pluggable ROMs in FLASH and FRAM
  // format
  // [name] [type] [size] [location]
  // 1  all             // list all files, including erased and dummy files
  // 2  ext             // extended listing with more details per file
  // 3  flash           // list all files in FLASH
  // 4  fram            // list all files in FRAM (ignored for now)
  // -1 no valid argument, assume only filename given

  // first list the ROMs in FLASH (no FRAM support for now)
  uint32_t offs = 0;
  uint32_t end = FF_SYSTEM_SIZE;
  ModuleMetaHeader_t *MetaH;          // pointer to meta header for getting info of next file
  ModuleFileHeader_t *ModH;           // pointer to MOD header for getting MOD details
  ModuleHeader_t *myPageHeader;       // pointer to the individual pages header
  uint32_t ROMoffset;                 // offset to the ROM image in FLASH
  uint16_t *myROMImage;               // buffer pointer for the ROM image
  V1_t *myV1;                         // pointer to the module contents in BIN format (5120 bytes)
  ModuleFileHeader_t *MODoffset;
  int filecounter = 0;                // counter for the number of files found in the directory
  
  if (!ff_isinited()) {
    cli_printf("  FLASH File system not initialized, please run INIT first");
    cli_printf("  Total space in FLASH appr %d Kbytes free", FF_SYSTEM_SIZE/1024);
    
    return;
  }

  // locate the file with the name fname
  if (fname != NULL) {
    // a filename is given so we only show the details of this file
    offs = ff_findfile(fname);
    if (offs == NOTFOUND) {
      cli_printf("  file \"%s\" not found", fname);
      return;
    }
    // show the file details here
    MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct
    cli_printf("  filename                         type      size  address     next file");
    cli_printf("  -------------------------------  ----  --------  ----------  ----------");
    cli_printfn("  %-31s  0x%02X  %8d  0x%08X  0x%08X", 
              MetaH->FileName, MetaH->FileType, MetaH->FileSize, offs, MetaH->NextFile);
    if (MetaH->FileType == FILETYPE_DELETED) {
      // deleted file
      cli_printf("  ** DELETED FILE **");
      return;
    } else if (MetaH->FileType == FILETYPE_DUMMY) {
      // dummy file
      cli_printf("  ** DUMMY FILE **");
      return;
    }

    cli_printf(" ");
    // and prepare to print more details from the MOD/ROM content below
    if (MetaH->FileType == FILETYPE_ROM) {
      // ROM file, show details
      ShowROMDetails((uint16_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t)));
      
      ShowROMDump((uint16_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t)));
     return;
    } else if (MetaH->FileType == FILETYPE_MOD1 || MetaH->FileType == FILETYPE_MOD2) {
      // show MOD file details
      ShowMODDetails((ModuleFileHeader_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t)));

      ShowMODDump((ModuleFileHeader_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t)));
      return;
    } else {
      cli_printf("  file type not supported");
      return;
    }
  }

  // no filename given, so we list all files
  // with i = 1 - all we list all files, including deleted and dummy files
  //      i = 2 - ext we list all files with more details
  //      i = 3 - flash we list all files in FLASH only
  //      i = 4 - fram we list all files in FRAM (not yet supported)

  char line[80];

  // layout for listing
  //  filename                        type   size    address     next
  // "...............................  ..  ........  ........  ........"
  cli_printf("  filename                         type      size  address     next file");
  cli_printf("  -------------------------------  ----  --------  ----------  ----------");

  while (offs < end) {
    tud_task();  // must keep the USB port updated

    MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct
    if (MetaH->FileType == FILETYPE_FFFF) {
      // end of chain reached
      uint32_t free = end - offs;
      cli_printf("  ** Total files found:                            %d", filecounter);
      cli_printf("  ** END OF FILE SYSTEM **                         0x%08X", offs);
      cli_printf("  ** UNUSED SPACE UNTIL **                         0x%08X - appr %d Kbytes free", end, free/1024);
      return;
    }

    // build the string for showing the listing
    sprintf(line, "  %-31s  0x%02X  %8d  0x%08X  0x%08X", 
              MetaH->FileName, MetaH->FileType, MetaH->FileSize, offs, MetaH->NextFile);
    cli_printfn(line);
    filecounter++;  // count the number of files found
    if (MetaH->FileType == FILETYPE_DELETED) {
      // deleted file
      cli_printfn("  ** DELETED FILE **");
      filecounter--;  // count the number of files found
    } else if (MetaH->FileType == FILETYPE_DUMMY) {
      // dummy file
      cli_printfn("  ** DUMMY FILE **");
      filecounter--;  // count the number of files found
    }
    cli_printf(" ");
    if (i == 2) {
      // extended listing, show more details
      // show more details for the MOD/ROM content
      // first check if the file is a ROM or MOD file
      if ((MetaH->FileType == FILETYPE_MOD1) || (MetaH->FileType == FILETYPE_MOD2)) {
        // MOD file, show the MOD header
        ModH = (ModuleFileHeader_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t));
        cli_printf("    MOD Format %s", ModH->FileFormat);
        cli_printf("    MOD Title  %s", ModH->Title);
        cli_printf("    MOD Pages  %d", ModH->NumPages);

        // also add details of the ROM contents later
      } else if (MetaH->FileType == FILETYPE_ROM) {
        // ROM file, show the ROM header
        ShowROMDetails((uint16_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t)));
      } 
        // allother filetypes, just show the info
        // to be added later
    }

    offs = MetaH->NextFile;                                     // go to the next file
  }    
}


// The Plug function relies on the LoadMOD function, based on LoadMOD in the V41 sources in HP41File.cpp
// Copyright (c) 1989-2002  Warren Furlow   

// the function isPageUsed is replaced by the isUsed function
/****************************/
// return if page is used, W&W RAMBOX RAM is not marked as used!
/****************************/
/* bool isPageUsed(uint page, uint bank)
{
  const ModulePage *pPage = PageMatrix[page][bank-1];
  return pPage!=NULL && !(pPage->fWWRAMBOX && pPage->fRAM);
}
*/


// Return values
#define LOADMOD_SUCCESS                 0
#define LOADMOD_FAIL                    100
#define LOADMOD_OPEN_FAIL               101
#define LOADMOD_READ_FAIL               102
#define LOADMOD_INVALID_FILE            103
#define LOADMOD_LOAD_CONFLICT           104   // load conflict or no space
#define LOADMOD_UNSUPPORTED_HARDWARE    105
#define LOADMOD_UNSUPPORTED_PAGE        106

// in case a supported hardware MOD file is loaded, the retun value is the following:
/*
#define HARDWARE_NONE               0  // no additional hardware specified 
#define HARDWARE_PRINTER            1  // 82143A Printer 
#define HARDWARE_CARDREADER         2  // 82104A Card Reader 
#define HARDWARE_TIMER              3  // 82182A Time Module or HP-41CX built in timer 
#define HARDWARE_WAND               4  // 82153A Barcode Wand 
#define HARDWARE_HPIL               5  // 82160A HP-IL Module 
#define HARDWARE_INFRARED           6  // 82242A Infrared Printer Module 
#define HARDWARE_HEPAX              7  // HEPAX Module - has special hardware features (write protect, relocation) 
#define HARDWARE_WWRAMBOX           8  // W&W RAMBOX - has special hardware features (RAM block swap instructions) 
#define HARDWARE_MLDL2000           9  // MLDL2000 
#define HARDWARE_CLONIX             10 // CLONIX-41 Module 
#define HARDWARE_MAX                10 // maximum HARDWARE_ define value 
*/

int LoadMOD(ModuleMetaHeader_t *MetaH)
{

  ModuleFileHeader_t *pMFH;       // pointer to the Module structure
  uint page,hep_page=0;
  uint FileSize;
  bool fLoad = false;
  int hw_return = LOADMOD_SUCCESS;  // return value, assume success

  pMFH = (ModuleFileHeader_t *)((uint8_t *)MetaH + sizeof(ModuleMetaHeader_t));

  #ifdef DEBUG
  cli_printf("  LoadMOD: called with MetaH      @ 0x%08X", (uint8_t *)MetaH);
  cli_printf("  LoadMOD: ModuleFileHeader  pMFH @ 0x%08X", (uint8_t *)pMFH);
  #endif

  // get file format and the most important MOD file parameters
  // there is no check on the file size etc

  // nFileFormat is 1 for MOD1 and 2 for MOD2, 0 for an unsupported type
  const int nFileFormat = MetaH->FileType;
  if (MetaH->FileType != FILETYPE_MOD1) {
    #ifdef DEBUG
    cli_printf("  LoadMOD: only MOD1 files supported, this is a %d file", MetaH->FileType);
    #endif
    return(LOADMOD_INVALID_FILE);  // not a MOD file, cannot do anything with it
  }

  FileSize = MetaH->FileSize;  // get the file size from the meta header, just to check if we are not reading past the end
  
//  #define HARDWARE_NONE               0  /* no additional hardware specified */
//  #define HARDWARE_PRINTER            1  /* 82143A Printer */
//  #define HARDWARE_CARDREADER         2  /* 82104A Card Reader */
//  #define HARDWARE_TIMER              3  /* 82182A Time Module or HP-41CX built in timer */
//  #define HARDWARE_WAND               4  /* 82153A Barcode Wand */
//  #define HARDWARE_HPIL               5  /* 82160A HP-IL Module */
//  #define HARDWARE_INFRARED           6  /* 82242A Infrared Printer Module */
//  #define HARDWARE_HEPAX              7  /* HEPAX Module - has special hardware features (write protect, relocation) */
//  #define HARDWARE_WWRAMBOX           8  /* W&W RAMBOX - has special hardware features (RAM block swap instructions) */
//  #define HARDWARE_MLDL2000           9  /* MLDL2000 */
//  #define HARDWARE_CLONIX             10 /* CLONIX-41 Module */
//  #define HARDWARE_MAX                10 /* maximum HARDWARE_ define value */

  
// first check if the hardware is supported by the TULIP firmware
  if (
      // (pMFH->Hardware == HARDWARE_PRINTER) || 
      (pMFH->Hardware == HARDWARE_CARDREADER) ||
      (pMFH->Hardware == HARDWARE_TIMER) ||
      // (pMFH->Hardware == HARDWARE_WAND) ||
      // (pMFH->Hardware == HARDWARE_HPIL) ||
      (pMFH->Hardware == HARDWARE_INFRARED) ||
      // (pMFH->Hardware == HARDWARE_HEPAX) ||   // there is no HEPAX hardware support, but HEPAX works with the modified 4H version
      (pMFH->Hardware == HARDWARE_WWRAMBOX) ||
      (pMFH->Hardware == HARDWARE_MLDL2000) ||
      (pMFH->Hardware == HARDWARE_CLONIX)) {
    return (LOADMOD_UNSUPPORTED_HARDWARE);  // unsupported hardware type, cannot load this MOD file
  } else{
    // supported hardware or no specific hardware
    hw_return = pMFH->Hardware;  // remember the hardware type for later use if succesful
  }

  byte ImageNo = 0;                         // image no. in MOD file

  // these are arrays indexed on the page group number (1-8) (unique only within each mod file)
  // dual use: values are either a count stored as a negative number or a (positive) page number 1-f
  int LowerGroup[8]   = {0,0,0,0,0,0,0,0};      // <0, or =page # if lower page(s) go in group
  int UpperGroup[8]   = {0,0,0,0,0,0,0,0};      // <0, or =page # if upper page(s) go in group
  int OddGroup[8]     = {0,0,0,0,0,0,0,0};      // <0, or =page # if odd page(s) go in group
  int EvenGroup[8]    = {0,0,0,0,0,0,0,0};      // <0, or =page # if even page(s) go in group
  int OrderedGroup[8] = {0,0,0,0,0,0,0,0};      // <0, or =page # if ordered page(s) go in group

  #ifdef DEBUG
  cli_printf("  LoadMOD: FileFormat %s, Title \"%s\", NumPages %d, Hardware %d", 
            pMFH->FileFormat, pMFH->Title, pMFH->NumPages, pMFH->Hardware);
  #endif

  // load ROM pages with three pass process
  for (int pass = 1; pass <= 3; pass++) {
    for (int pageIndex = 0; pageIndex < pMFH->NumPages; pageIndex++) {
      // show debug messages

      ModuleHeader_t *pMFP;                 // pointer to the page header in the MOD file 
      // assign the pointer to the page header

      // pMFH = (ModuleFileHeader_t *)((uint8_t *)MetaH + sizeof(ModuleMetaHeader_t));

      pMFP = (ModuleHeader_t*)((uint8_t *)pMFH + sizeof(ModuleFileHeader_t) + pageIndex * sizeof(ModuleHeader_t));

      #ifdef DEBUG
      cli_printf("  Pass %d - PageIndex %d - checking page %01X (group %d) @ 0x%08X", pass, pageIndex, pMFP->Page, pMFP->PageGroup, (uint8_t *)pMFP);
      #endif

      // Skip this (is wrong anyway) - we always use MOD1 format for now
      // if (1 == nFileFormat) {               // MOD1
      //    pMFP = &((ModuleHeader_t *)&pMFH[1])[pageIndex];
      // } else {                              // MOD2
      //   pMFP = (ModuleHeader_t *)(&((ModuleHeaderV2_t *)&pMFH[1])[pageIndex]);
      // }

      fLoad = false;
      switch(pass) {
        case 1:                             // pass 1: validate page variables, flag grouped pages
          if ((pMFP->Page>0x0f && pMFP->Page<POSITION_MIN) || pMFP->Page>POSITION_MAX || pMFP->PageGroup>8 ||
            pMFP->Bank==0 || pMFP->Bank>4 || pMFP->BankGroup>8 || pMFP->RAM>1 || pMFP->WriteProtect>1 || pMFP->FAT>1 ||  //out of range values
            (pMFP->PageGroup && pMFP->Page<=POSITION_ANY) ||    // group pages cannot use non-grouped position codes 
            (!pMFP->PageGroup && pMFP->Page>POSITION_ANY))      // non-grouped pages cannot use grouped position codes
            {
              return(LOADMOD_INVALID_FILE);
            }
          if (pMFP->PageGroup == 0)             // if not grouped do nothing in this pass
            break;
          if (pMFP->Page == POSITION_LOWER)
            LowerGroup[pMFP->PageGroup-1] -= 1; // save the count of pages with each attribute as a negative number
          else if (pMFP->Page == POSITION_UPPER)
            UpperGroup[pMFP->PageGroup-1] -= 1;
          else if (pMFP->Page == POSITION_ODD)
            OddGroup[pMFP->PageGroup-1] -= 1;
          else if (pMFP->Page == POSITION_EVEN)
            EvenGroup[pMFP->PageGroup-1] -= 1;
          else if (pMFP->Page == POSITION_ORDERED)
            OrderedGroup[pMFP->PageGroup-1] -= 1;
          break;

        case 2:                             // pass 2: find free location for grouped pages
          if (pMFP->PageGroup == 0)             // if not grouped do nothing in this pass
            break;
          // a matching page has already been loaded
          if (pMFP->Page == POSITION_LOWER && UpperGroup[pMFP->PageGroup-1]>0)       // this is the lower page and the upper page has already been loaded
            page = UpperGroup[pMFP->PageGroup-1]-1;
          else if (pMFP->Page == POSITION_LOWER && LowerGroup[pMFP->PageGroup-1]>0)  // this is another lower page
            page = LowerGroup[pMFP->PageGroup-1];
          else if (pMFP->Page == POSITION_UPPER && LowerGroup[pMFP->PageGroup-1]>0)  // this is the upper page and the lower page has already been loaded
            page = LowerGroup[pMFP->PageGroup-1]+1;
          else if (pMFP->Page == POSITION_UPPER && UpperGroup[pMFP->PageGroup-1]>0)  // this is another upper page
            page = UpperGroup[pMFP->PageGroup-1];
          else if (pMFP->Page == POSITION_ODD && EvenGroup[pMFP->PageGroup-1]>0)
            page = EvenGroup[pMFP->PageGroup-1]+1;
          else if (pMFP->Page == POSITION_ODD && OddGroup[pMFP->PageGroup-1]>0)
            page = OddGroup[pMFP->PageGroup-1];
          else if (pMFP->Page == POSITION_EVEN && OddGroup[pMFP->PageGroup-1]>0)
            page = OddGroup[pMFP->PageGroup-1]-1;
          else if (pMFP->Page == POSITION_EVEN && EvenGroup[pMFP->PageGroup-1]>0)
            page = EvenGroup[pMFP->PageGroup-1];
          else if (pMFP->Page == POSITION_ORDERED && OrderedGroup[pMFP->PageGroup-1]>0)
            page = ++OrderedGroup[pMFP->PageGroup-1];
          // find first page in group
          else {     // find free space depending on which combination of positions are specified
            if (LowerGroup[pMFP->PageGroup-1] != 0 && UpperGroup[pMFP->PageGroup-1] != 0) {        // lower and upper
              page = 8;
              while (page <= 0xe && (TULIP_Pages.isUsed(page, pMFP->Bank) || TULIP_Pages.isUsed(page+1, pMFP->Bank)))
                page++;
            } else if (LowerGroup[pMFP->PageGroup-1] != 0) {                                       // lower but no upper
              page = 8;
              while (page <= 0xf && TULIP_Pages.isUsed(page, pMFP->Bank))
                page++;
            } else if (UpperGroup[pMFP->PageGroup-1] != 0) {                                       // upper but no lower
              page = 8;
              while (page <= 0xf && TULIP_Pages.isUsed(page, pMFP->Bank))
                page++;
            } else if (EvenGroup[pMFP->PageGroup-1] != 0 && OddGroup[pMFP->PageGroup-1] != 0) {      // even and odd
              page = 8;
              while (page <= 0xe && (TULIP_Pages.isUsed(page, pMFP->Bank) || TULIP_Pages.isUsed(page+1, pMFP->Bank)))
                page += 2;
            } else if (EvenGroup[pMFP->PageGroup-1] !=0 ) {                                        // even only
            page=8;
            while (page <= 0xe && TULIP_Pages.isUsed(page, pMFP->Bank))
              page += 2;
            } else if (OddGroup[pMFP->PageGroup-1] != 0) {                                        // odd only
              page = 8; // return even page number, page no. is incremented when setting OddGroup
              while (page <= 0xe && TULIP_Pages.isUsed(page + 1, pMFP->Bank))
                page += 2;
            } else if (OrderedGroup[pMFP->PageGroup-1] != 0) {                                     // a block
              uint count = -OrderedGroup[pMFP->PageGroup-1];
              for (page = 8; page <= 0x10 - count; page++) {
                uint nFree = 0;
                uint page2;
                for (page2 = page; page2 <= 0x0f; page2++) { // count up free spaces
                  if (!TULIP_Pages.isUsed(page2, pMFP->Bank))
                    nFree++;
                  else
                    break;
                }
                if (count <= nFree)            // found a space
                  break;
              }
            } else {
              page = 8;
              while (page <= 0xf && TULIP_Pages.isUsed(page, pMFP->Bank))
                page++;
            }

            // save the position that was found in the appropriate array
            if (pMFP->Page == POSITION_LOWER)
              LowerGroup[pMFP->PageGroup-1] = page;
            else if (pMFP->Page == POSITION_UPPER) {
              ++page;                                 // found two positions - take the upper one
              UpperGroup[pMFP->PageGroup-1] = page;
            } else if (pMFP->Page == POSITION_EVEN)
              EvenGroup[pMFP->PageGroup-1] = page;
            else if (pMFP->Page == POSITION_ODD) {
              ++page;                                 // found two positions - take the odd one
              OddGroup[pMFP->PageGroup-1] = page;
            } else if (pMFP->Page == POSITION_ORDERED)
              OrderedGroup[pMFP->PageGroup-1] = page;
          }
          fLoad = true;
          break;

        case 3:      // pass 3 - find location for non-grouped pages
          if (pMFP->PageGroup)
            break;
          if (pMFP->Page == POSITION_ANY) {          // a single page that can be loaded anywhere 8-F
            page=8;
            // look for an empty page or a page with W&W RAMBOX RAM
            while (page <= 0xf && TULIP_Pages.isUsed(page,pMFP->Bank))
              page++;
          } else                                    // page number is hardcoded
            page = pMFP->Page;
          fLoad = true;
          break;
      }   // end of switch(pass)

      if (fLoad) {   
        // load the image
        // the page is ready to be plugged, but we can skip this since the Page is in FLASH
        // we do need to get the parameters in the CPage and CBank structures where needed
        // but remember that the contents of the MOD file are always available in FLASH
        // only we cannot write to FLASH

        // The MOD file image is now ready to be plugged with the following parameters
        //     page       : target Page
        //     pMFP->Bank : target Bank
        //     pageIndex  : target PageIndex
        //     ImageNo    : Image number in MOD file
        // show debug messages
        #ifdef DEBUG
        cli_printf("  About to Plug MOD to Page %d - Bank %d - PageIndex %d - ImageNumber %d", page, pMFP->Bank, pageIndex, ImageNo);
        #endif

        if (page <= 3) {
          // not supported, exit
          // free(pMFH);
          return(LOADMOD_UNSUPPORTED_PAGE);
        }

        // the loading of the file is skipped here, the file is already in FLASH with direct access
        size_t nPageCustomOffset;

        // define a single Bank in a Page
        //  struct CBank
        // {
        //   uint32_t b_img_rom;   // Offset to current image in FLASH or FRAM
        //   uint16_t *b_img_data; // hard address of the image in FLASH
        //   uint16_t b_img_flags; // Flags, type and status of the Bank
        //   char b_img_name[32];  // Filename of the plugged module (in case of a ROM file usually)
        //   uint32_t b_img_file;  // Offset to the MOD or ROM file in FLASH or FRAM
        //                         // in case more info is needed
        // };

        // prepare the flags for the new Page
        uint16_t Bank_Flags = 0;

        // BANK_none      = 0x0000,       // nothing plugged here
        // BANK_ACTIVE    = 0x0001,       // 1: bank is active (has a valid content)
        // BANK_FLASH     = 0x0002,       // 1: bank is in FLASH, 0: bank is in FRAM
        // BANK_ROM       = 0x0004,       // 1: bank is ROM, 0: not ROM (but MOD1 or MOD2)
        // BANK_MOD       = 0x0008,       // 1: bank is MOD1, 0: bank is MOD2
        // BANK_ENABLED   = 0x0010,       // 1: bank is enabled for reading, 0: not enabled for reading
        // BANK_DIRTY     = 0x0020,       // 1: bank is dirty (written to but not saved), 0: bank is OK
                                          // remnant from Tiny41, not used in TULIP for now
                                          // may be needed if Read-Modify-Write for QROm MOD files is too slow on FRAM
        // BANK_WRITEABLE = 0x0040,       // 1: bank is write enabled, 0: no writing possible
        // BANK_RESERVED  = 0x0080,       // 1: bank reserved by a physical module, 0: pluggable for TULIP
        // BANK_EMBEDDED  = 0x0100,       // page is embedded in the firmware
                                          // used for the HP-IL and Printer ROMs
                                          // other bits are reserved for future use


        // set the flags for the new Bank
        Bank_Flags = BANK_ACTIVE | BANK_FLASH | BANK_ENABLED;  // always active, enabled and in FLASH

        if (pMFP->RAM)          Bank_Flags &= !BANK_ROM;
        if (pMFP->WriteProtect) Bank_Flags &= !BANK_WRITEABLE;

        if (MetaH->FileType == FILETYPE_MOD1) Bank_Flags |= BANK_MOD;  // MOD1
        // for MOD2 the flag is 0

        // First ensure that the page/bank is free indeed
        if ((page > 0xF) || TULIP_Pages.isUsed(page, pMFP->Bank)) {
          // free(pMFH);
          return(LOADMOD_LOAD_CONFLICT);  // no space or load conflict
        }

        // and plug the image
        TULIP_Pages.Pages[page].m_banks[pMFP->Bank].b_img_data = (uint16_t *)pMFP->Image;      // hard address of the image in FLASH
        TULIP_Pages.Pages[page].m_banks[pMFP->Bank].b_img_rom = (uint32_t)(MetaH) - FF_SYSTEM_BASE; // offset to the MOD file in FLASH
        TULIP_Pages.Pages[page].m_banks[pMFP->Bank].b_img_flags = Bank_Flags;  // set the flags for the bank
        TULIP_Pages.Pages[page].m_banks[pMFP->Bank].b_img_name[0] = 0; // clear the name, it will be set below
        TULIP_Pages.Pages[page].m_banks[pMFP->Bank].b_img_file = (uint32_t)(MetaH) - FF_SYSTEM_BASE; // offset to the MOD file in FLASH

        // and set the name
        strcpy(TULIP_Pages.Pages[page].m_banks[pMFP->Bank].b_img_name, MetaH->FileName);

        #ifdef DEBUG
        cli_printf("  Flags: %04X", Bank_Flags);
        cli_printf("  Image: %04X", TULIP_Pages.Pages[page].m_banks[pMFP->Bank].b_img_data);
        cli_printf("  ROM:   %04X", TULIP_Pages.Pages[page].m_banks[pMFP->Bank].b_img_rom);
        cli_printf("  File:  %04X", TULIP_Pages.Pages[page].m_banks[pMFP->Bank].b_img_file);
        cli_printf("  Name:  %s", TULIP_Pages.Pages[page].m_banks[pMFP->Bank].b_img_name);
        #endif


        /* we do not use the followoing from V41
        ModulePage *pNewPage=new ModulePage;
        pNewPage->pModule = pModuleNew;
        pNewPage->pAltPage = NULL;
        strcpy(pNewPage->szName, pMFP->Name);
        strcpy(pNewPage->szID, pMFP->ID);
        pNewPage->ImageNo = ImageNo++;            // save module position in MOD file
        pNewPage->Page = pMFP->Page;
        pNewPage->ActualPage = page;
        pNewPage->Bank = pMFP->Bank;
        pNewPage->PageGroup = pMFP->PageGroup;
        pNewPage->BankGroup = pMFP->BankGroup;
        if (pMFP->BankGroup)
          pNewPage->ActualBankGroup = pMFP->BankGroup + NextActualBankGroup * 8;  // ensures each bank group has a number that is unique to the entire simulator
        else
          pNewPage->ActualBankGroup = 0;
        pNewPage->fRAM = pMFP->RAM;
        pNewPage->fWriteProtect = pMFP->WriteProtect;
        pNewPage->fFAT = pMFP->FAT;
        pNewPage->fHEPAX = (pMFH->Hardware == HARDWARE_HEPAX);
        pNewPage->fWWRAMBOX = (pMFH->Hardware == HARDWARE_WWRAMBOX);
        */

        // unpacking a compressed file is not needed
        // maybe later when the ROM needs to be in FRAM
        /*
        if (1 == nFileFormat)                   // MOD1
          {
          unpack_image(pNewPage->Image,pMFP->Image);
          nPageCustomOffset=offsetof(ModuleFilePage,PageCustom);
          }
        else                                  // MOD2
          {
          word *pwImage=(word *)pMFP->Image;
          for (size_t i = 0; i < sizeof(pNewPage->Image)/sizeof(pNewPage->Image[0]); ++i)
            {
            // swap bytes
            pNewPage->Image[i]=(pwImage[i] >> 8)|(pwImage[i] << 8);
            }
          nPageCustomOffset=offsetof(ModuleFilePageV2,PageCustom);
          }
          memcpy(pNewPage->PageCustom,(byte *)pMFP+nPageCustomOffset,sizeof(pNewPage->PageCustom));
        */

        // here we check for some situations that the TULIP does not support



        /* Page 0 not supported by TULIP
        // patch the NULL timeout value to be longer - not known if this works for all revisions
        if (page==0 && (0==strcmpi(pNewPage->szName,"NUT0-D") || 0==strcmpi(pNewPage->szName,"NUT0-G") || 0==strcmpi(pNewPage->szName,"NUT0-N")))
          {
          pNewPage->Image[0x0ec7]=0x3ff;                               // original value =0x240
          pNewPage->Image[0x0fff]=compute_checksum(pNewPage->Image);   // fix the checksum so service rom wont report error
          }
          */

        // HEPAX special case, this is skipped for now
        /*
        if (hep_page && pNewPage->fHEPAX && pNewPage->fRAM)            // hepax was just loaded previously and this is the first RAM page after it
          {
          pNewPage->ActualPage=hep_page;
          pNewPage->pAltPage=PageMatrix[hep_page][pMFP->Bank-1]->pAltPage;
          PageMatrix[hep_page][pMFP->Bank-1]->pAltPage=pNewPage;       // load this RAM into alternate page
          }
          */


      }
      
    }   // this is the end of the loop:  for (int pageIndex = 0; pageIndex < pMFH->NumPages; pageIndex++)


    // free(pMFH);
    // return 0;
  }          // this is the end of the 3 pass loop   for (int pass = 1; pass <= 3; pass++)                                      

  return hw_return;  // return the hardware type if succesful
}

// */

/*
#define PLUG_HELP_TXT "plug functions\r\n\
        hpil          plugs the embedded HP-IL ROM in Page 7 and enables emulation\r\n\
        ilprinter     plugs the embedded HP-IL Printer ROM in Page 6\r\n\
        printer       plugs the embedded HP82143A Printer ROM in Page 6 and enables emulation\r\n\
        [filename] X  plug the ROM in Page X (hex)\r\n\
          [filename]   is the name of the file in FLASH with extension\r\n\
        [filename]    no Page number will autoplug and find a free Page from 8..F\r\n\
        [filename] T  Autoplug Test only, will not plug for real\r\n"

        #define plug_hpil       1
        #define plug_ilprinter  2
        #define plug_printer    3
        #define plug_file_X     4   // file in specific Page
        #define plug_file_A     5   // Autoplug file in first free Page
        #define plug_file_T     6   // Autoplug Test only
*/  

// plug and enable the selected ROM
void uif_plug(int func, int Page, int Bank, const char *fname)          // plug the selected ROM 
{
  char *ffname;
  uint16_t *myROMImage;               // buffer pointer for the ROM image
  uint32_t ROMImage_offs;
  uint32_t ROMName_offs;
  int NumFunctions;
  uint32_t offs;
  int i;
  ModuleMetaHeader_t *MetaH;
  uint16_t rom_flags;
  char *ext;
  uint16_t *ptr_image;

  char  ShowPrint[250];
  int   ShowPrintLen = 0;
  char c;
  int Pg = 0; // Page number 
  bool PgFound = false; // Page found flag

  if (!uif_pwo_low()) return;    // only do this when calc is not running

  // check if Page is already plugged or reserved
  if ((func != plug_file_A) && (func != plug_file_T)) {
    // only check if the Page is occupied when we are not autoplugging
    if (TULIP_Pages.isPlugged(Page, Bank)) {
      cli_printf("  Page %X Bank %d is already occupied, please unplug first", Page, Bank);
      return;
    } else {
      if (TULIP_Pages.isReserved(Page)) {
        cli_printf("  Page %X is reserved, please cancel reservation first", Page);
        return;
      } 
    } 
  }

  switch (func)
  {
    case plug_hpil: 
            // plug the Embedded HPIL ROM in Page 6
            // get a pointer to the ROM image in FLASH
            // const uint16_t  __in_flash()embed_HPIL_rom[]

            // check if Page is occupied
            if (TULIP_Pages.isPlugged(Page, 1)) {
              cli_printf("  Page %X is already occupied, please unplug first", Page);
              return;
            }

            rom_flags = BANK_ACTIVE | BANK_FLASH | BANK_ROM | BANK_ENABLED | BANK_EMBEDDED; // prepare the flags for the ROM
            cli_printf("  plugging Embedded HP-IL ROM in Page 7");        
            TULIP_Pages.plug_embedded(7, 1, rom_flags, embed_HPIL_rom); // plug the ROM in the given page
            TULIP_Pages.save(); // save the page settings in FRAM
            globsetting.set(HP82160A_enabled, 1); // enable the HP82160A ROM in the settings
            globsetting.save();
            Page = 7; // set the page to 7, as this is the page we plugged the ROM in
            break;

    case plug_ilprinter:
            // plug the Embedded HPIL Printer ROM in Page 6
            // get a pointer to the ROM image in FLASH
            // const uint16_t  __in_flash()embed_HPILPRINTER_rom[]

            // check if Page is occupied
            if (TULIP_Pages.isPlugged(Page, 1)) {
              cli_printf("  Page %X is already occupied, please unplug first", Page);
              return;
            }

            rom_flags = PAGE_ACTIVE | PAGE_FLASH | PAGE_ROM | PAGE_ENABLED | PAGE_EMBEDDED; // prepare the flags for the ROM
            cli_printf("  plugging Embedded HP-IL Printer ROM in Page 6");      
            TULIP_Pages.plug_embedded(6, 1, rom_flags, embed_ILPRINTER_rom); // plug the ROM in the given page      
            TULIP_Pages.save(); // save the page settings in FRAM
            globsetting.set(HP82143A_enabled, 0); // disable the HP82143A ROM in the settings, in case it was plugged
            globsetting.save();
            Page = 6; // set the page to 6, as this is the page we plugged the ROM in
            break;

    case plug_printer:
            // plug the Embedded Printer ROM in Page 6
            // get a pointer to the ROM image in FLASH
            // const uint16_t  __in_flash()embed_printer_rom[]

            // check if Page is occupied
            if (TULIP_Pages.isPlugged(Page, 1)) {
              cli_printf("  Page %X is already occupied, please unplug first", Page);
              return;
            }

            rom_flags = PAGE_ACTIVE | PAGE_FLASH | PAGE_ROM | PAGE_ENABLED | PAGE_EMBEDDED; // prepare the flags for the ROM
            cli_printf("  plugging Embedded HP82143A Printer ROM in Page 6");   
            TULIP_Pages.plug_embedded(6, 1, rom_flags, embed_PRINTER_rom); // plug the ROM in the given page         
            TULIP_Pages.save(); // save the page settings in FRAM
            globsetting.set(HP82143A_enabled, 1);
            globsetting.save();
            Page = 6;
            break;

    case plug_file_X: // plug the give filename in the given Page
            // check if the file exists in flash

            // check if Page is occupied
            if (TULIP_Pages.isPlugged(Page, Bank)) {
              cli_printf("  Page %X Bank %d is already occupied, please unplug first", Page, Bank);
              return;
            }

            offs = ff_findfile(fname);
            if (offs == NOTFOUND) {
              cli_printf("  file \"%s\" not found", fname);
              return;
            }

            cli_printf("  plugging file %s in Page %X Bank %d", fname, Page, Bank);
            // file exists and has the correct extension
            // check if the file is a MOD or ROM file

        
            MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct
            cli_printf("  filename                         type      size  address     next file");
            cli_printf("  -------------------------------  ----  --------  ----------  ----------");
            cli_printf("  %-31s  0x%02X  %8d  0x%08X  0x%08X", 
                      MetaH->FileName, MetaH->FileType, MetaH->FileSize, offs, MetaH->NextFile);


            if (MetaH->FileType == FILETYPE_ROM) {
              // ROM file, show details
              // get a pointer to the start of the ROM
              // and prepare to print more details from the MOD/ROM content below
              myROMImage = (uint16_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t));
              ShowROMDetails(myROMImage);

              ROMImage_offs = (uint32_t)myROMImage - (uint32_t)FF_SYSTEM_BASE; // get the offset to the ROM image in FLASH

              // get the offset to the ROM image in FLASH
              cli_printf("    ROM Image offset 0x%08X, at address 0x%08X", ROMImage_offs, myROMImage);

              // now prepare the flags and do the actual plugging
              /* enum {
                PAGE_ACTIVE    = 0x01,       // page is active (has a valid content)
                PAGE_FLASH     = 0x02,       // page is in FLASH or FRAM
                PAGE_ROM       = 0x04,       // page is ROM or not
                PAGE_MOD       = 0x08,       // page is MOD1 or MOD2
                PAGE_ENABLED   = 0x10,       // page is enabled for reading
                PAGE_DIRTY     = 0x20,       // page is dirty
                PAGE_WRITEABLE = 0x40,       // page is write enabled
                PAGE_SPARE1    = 0x80,       // for future use
              */ 
              rom_flags = BANK_ACTIVE | BANK_FLASH | BANK_ROM | BANK_ENABLED;
              
              TULIP_Pages.plug(Page, Bank, rom_flags, offs); // plug the ROM in the given page
              TULIP_Pages.save(); // save the page settings in FRAM

            }

            if (MetaH->FileType == FILETYPE_MOD1 || MetaH->FileType == FILETYPE_MOD2) {
              cli_printf("  only ROM files currently supported in a given Page, please use Autoplug for MOD files");
              return;
            }
            break;

    case plug_file_A: // autoplug the given filename in the first free Page
    case plug_file_T: // autoplug the given filename in the first free Page, Test option
            // check if the file exists in flash
            offs = ff_findfile(fname);
            if (offs == NOTFOUND) {
              cli_printf("  file \"%s\" not found", fname);
              return;
            }
            // file exists and has the correct extension
            MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct
            cli_printf("  filename                         type      size  address     next file");
            cli_printf("  -------------------------------  ----  --------  ----------  ----------");
            cli_printf("  %-31s  0x%02X  %8d  0x%08X  0x%08X", 
                      MetaH->FileName, MetaH->FileType, MetaH->FileSize, offs, MetaH->NextFile);
            if (MetaH->FileType == FILETYPE_ROM) {
              // ROM file, show details
              // get a pointer to the start of the ROM
              // and prepare to print more details from the MOD/ROM content below
              myROMImage = (uint16_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t));
              ShowROMDetails(myROMImage);
              ROMImage_offs = (uint32_t)myROMImage - (uint32_t)FF_SYSTEM_BASE; // get the offset to the ROM image in FLASH
              // get the offset to the ROM image in FLASH
              cli_printf("    ROM Image offset 0x%08X, at address 0x%08X, now AutoPlugging", ROMImage_offs, myROMImage);
              // now prepare the flags and do the actual plugging
              /* enum {
                BANK_ACTIVE    = 0x01,       // bank is active (has a valid content)
                BANK_FLASH     = 0x02,       // bank is in FLASH or FRAM
                BANK_ROM       = 0x04,       // bank is ROM or not
                BANK_MOD       = 0x08,       // bank is MOD1 or MOD2
                BANK_ENABLED   = 0x10,       // bank is enabled for reading
                BANK_DIRTY     = 0x20,       // bank is dirty
                BANK_WRITEABLE = 0x40,       // bank is write enabled
                BANK_EMBEDDED  = 0x80,       // bank contains an embedded ROM
              */
              rom_flags = BANK_ACTIVE | BANK_FLASH | BANK_ROM | BANK_ENABLED; // prepare the flags for the ROM

              // now find the first free Page from 8..F
              Pg = 8;
              PgFound = false;

              while ((Pg < 16) && !PgFound) {
                // check if the Page is free
                if (TULIP_Pages.isPlugged(Pg, 1) || TULIP_Pages.isReserved(Pg)) {
                  // Page is occupied, so try the next one
                  Pg++;
                } else {
                  // Page is free, so use this one
                  PgFound = true;
                }
              }

              if (!PgFound) {
                // no free Page found
                cli_printf("  No free pages available, please unplug a page first");
                return;
              } 

              cli_printf("  Free Page %X found for %s", Pg, fname);

              if (func == plug_file_T) {
                // Test option, so do not plug the ROM, just show the details
                cli_printf("  Autoplug Test only, not plugged for real");
                return;
              }

              // Page is found, so use this Page
              Page = Pg; // set the Page to the found Page

              // do the actual plugging
              TULIP_Pages.plug(Page, 1, rom_flags, offs); // plug the ROM in the given page
              TULIP_Pages.save(); // save the page settings in FRAM
            } else if (MetaH->FileType == FILETYPE_MOD1) {
              // MOD2 is not supported yet, but no error message is given
              cli_printf("  Plugging MOD file");

              // call the MOD file loader

              int res = LoadMOD(MetaH);
              if (res >= LOADMOD_FAIL) {
                // show the error
                switch (res) {
                  case LOADMOD_OPEN_FAIL:
                    cli_printf("  Error: failed to open MOD file");
                    break;
                  case LOADMOD_READ_FAIL:
                    cli_printf("  Error: failed to read MOD file");
                    break;
                  case LOADMOD_INVALID_FILE:
                    cli_printf("  Error: invalid MOD file");
                    break;
                  case LOADMOD_LOAD_CONFLICT:
                    cli_printf("  Error: load conflict or no space available");
                    break;
                  case LOADMOD_UNSUPPORTED_HARDWARE:
                    cli_printf("  Error: unsupported hardware");
                    break;
                  case LOADMOD_UNSUPPORTED_PAGE:
                    cli_printf("  Error: unsupported Page requested in MOD file");
                    break;
                  default:
                    cli_printf("  Error: unknown error %d", res);
                    break;
                }
              } else {
                // OK
                cli_printf("  MOD file loaded successfully");
                TULIP_Pages.save(); // save the page settings in FRAM

                // and we may have hardware dependent MOD files plugged
                //  #define HARDWARE_NONE               0  /* no additional hardware specified */
                //  #define HARDWARE_PRINTER            1  /* 82143A Printer */
                //  #define HARDWARE_CARDREADER         2  /* 82104A Card Reader */
                //  #define HARDWARE_TIMER              3  /* 82182A Time Module or HP-41CX built in timer */
                //  #define HARDWARE_WAND               4  /* 82153A Barcode Wand */
                //  #define HARDWARE_HPIL               5  /* 82160A HP-IL Module */
                //  #define HARDWARE_INFRARED           6  /* 82242A Infrared Printer Module */
                //  #define HARDWARE_HEPAX              7  /* HEPAX Module - has special hardware features (write protect, relocation) */
                //  #define HARDWARE_WWRAMBOX           8  /* W&W RAMBOX - has special hardware features (RAM block swap instructions) */
                //  #define HARDWARE_MLDL2000           9  /* MLDL2000 */
                //  #define HARDWARE_CLONIX             10 /* CLONIX-41 Module */
                //  #define HARDWARE_MAX                10 /* maximum HARDWARE_ define value */
                switch (res) {
                  case HARDWARE_PRINTER:
                    cli_printf("  HP82143A Printer hardware emulation enabled");
                    globsetting.set(HP82143A_enabled, 1); // enable the Printer hardware in the settings
                    globsetting.save();
                    break;

                  case HARDWARE_HEPAX:
                    cli_printf("  HEPAX Hardware detected, module plugged but no HEPAX instruction support");
                    break;

                  case HARDWARE_HPIL:
                    cli_printf("  HP-IL Hardware detected, HP-IL emulation enabled");
                    globsetting.set(HP82160A_enabled, 1); // enable the HP82160A HP-IL hardware in the settings
                    globsetting.save();
                    break;

                    case HARDWARE_WAND:
                    cli_printf("  Barcode Wand Hardware detected, Wand emulation enabled");
                    globsetting.set(HP82153A_enabled, 1); // enable the HP82153A WAND hardware in the settings
                    globsetting.save();
                    break;

                  case HARDWARE_NONE:
                    // no action required
                    break;
                
                  default:
                    // other hardware types are not supported yet
                    cli_printf("  Note: hardware type %d detected, but not supported yet", res);
                    break;
                }
              } 
              return;
            }
            else if (MetaH->FileType == FILETYPE_MOD2) {
                // MOD2 is not supported yet, but no error message is given
                cli_printf("  MOD2 files are not supported yet, please use MOD1 files");
                return;
              }
            break;

    default: 
            // no other actions defined here
            break;
  }

  // and update the settings in FRAM
  globsetting.save();


  // after plugging, read the first couple of words for checking the ROM contents

  uint16_t addr = Page * 0x1000; // address of the page in FLASH
  Bank = 1; // always bank 1 for now, as we only support one bank

  #ifdef DEBUG
  // read the first 16 words of the ROM image
  if (TULIP_Pages.isPlugged(Page, Bank)) {
    // page is plugged, so read the first 16 words
    // print the address
    ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%04X  ", addr);

    // print 16 bytes
    for (int m = 0; m < 16; m++) {
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%03X ", TULIP_Pages.getword(addr + m, 1));
    }
    // print byte values as characters
    ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "  ");
    cli_printf("%s", ShowPrint);
  } else {
    // page is not plugged, so show a message
    cli_printf("  Page %d is not plugged", Page);
  }
  #endif
  
}


/*
#define UNPLUG_HELP_TXT "unplug functions\r\n\
        [no argument] shows the current plugged ROMs\r\n\
        X (hex)       unplug the ROM in Page X, including reserved Pages\r\n\
        all           unplug all plugged ROMs except reserved Pages\r\n\
        ALL           unplug all plugged ROMs including reserved Pages\r\n"

        #define unplug_all     1   // unplug all plugged ROMs except reserved Pages
        #define unplug_ALL     2   // unplug all plugged ROMs including reserved Pages
        // all other values 4..F are a valid Page number to unplug

*/

// unplug and disable the selected Page
void uif_unplug(int p, int bk)            // plug the selected ROM / bank
{
  if (!uif_pwo_low()) return;     // only do this when calc is not running

  if (p == unplug_all) {
    // unplug all plugged ROMs and banks except reserved Pages
    for (int i = 4; i < 16; i++) {
      for (int b = 1; b < 5; b++) {
        // check if the Page is plugged
        if (TULIP_Pages.isPlugged(i, b) && !TULIP_Pages.isReserved(i)) {
          // page is plugged and not reserved, so unplug it
          TULIP_Pages.unplug(i, b);
        }
      }
    }
    globsetting.set(HP82143A_enabled, 0); // disable the Printer ROM in the settings
    globsetting.set(HP82160A_enabled, 0); // disable the HP82160A ROM in the settings
    globsetting.save();
    TULIP_Pages.save();             // save the page settings in FRAM
    cli_printf("  Unplugged all plugged ROMs except reserved Pages");
    return;

  } else if (p == unplug_ALL) {
    // unplug all plugged ROMs and Banks including reserved Pages
    // cli_printf("  Unplugging all plugged ROMs including reserved Pages");
    for (int i = 4; i < 16; i++) {
      for (int b = 1; b < 5; b++) {
        // check if the Page is plugged
        if (TULIP_Pages.isPlugged(i, b) || TULIP_Pages.isReserved(i)) {
          // page is plugged, so unplug it
          TULIP_Pages.unplug(i, b);
        }
      }
    }
    globsetting.set(HP82143A_enabled, 0); // disable the Printer ROM in the settings
    globsetting.set(HP82160A_enabled, 0); // disable the HP82160A ROM in the settings
    globsetting.save();
    TULIP_Pages.save();                   // save the page settings in FRAM
    cli_printf("  All ROMs unplugged and all reservations cancelled");
    return;
  }

  if (TULIP_Pages.isReserved(p)) {
    // Page is reserved, so we cannot unplug it
    cli_printf("  Page %X is reserved, please cancel with reserve clear [P]", p);
    return; // exit the function
  }

  // unplug a specific Page or Bank
  if (bk == 0) {
    // unplug the Page and all Banks
    for (int b = 1; b < 5; b++) {
      if (TULIP_Pages.isPlugged(p, b)) {
        TULIP_Pages.unplug(p, b); // unplug the Page p, bank b 
      }
    }
    cli_printf("  unplugged Page %X, all Banks", p);
  } else {
    // unplug the specific Page and Bank
    if (TULIP_Pages.isPlugged(p, bk)) {
      TULIP_Pages.unplug(p, bk); // unplug the Page p, bank bk
      cli_printf("  unplugged Page %X Bank %d", p, bk);
    } else {
      cli_printf("  Page %X Bank %d was not plugged", p, bk);
      return; // exit the function
    }
  }

  // must disable emulation of the page
  if (p == 6) {
    // Printer Page
    globsetting.set(HP82143A_enabled, 0); // disable the Printer ROM in the settings
  } else if (p == 7) {
    // HPIL Page
    globsetting.set(HP82160A_enabled, 0); // disable the HP82160A ROM in the settings
  }
  globsetting.save();
  
  TULIP_Pages.save();             // save the page settings in FRAM
}


// reserve a Page for a physical module
// i is the function number
// p is the Page number to reserve (4..15)
// fname is the comment string to be used
//  #define reserve_cx      1
//  #define reserve_timer   2       
//  #define reserve_printer 3
//  #define reserve_hpil    4
//  #define reserve_clear   5
//  #define reserve_page    6   

void uif_reserve(int i, int p, const char *comment)
{

  const char P3_txt[] = "RESERVED for HP41CX Extended Functions Module";
  const char P5_txt[] = "RESERVED for HP41CX/TIME Module";
  const char P6_txt[] = "RESERVED for Printer (HP82143, IL Printer or Blinky)";
  const char P7_txt[] = "RESERVED for HPIL Module";

  if (!uif_pwo_low()) return;     // only do this when calc is not running

  // Page number should be check already, but who knows ...
  if ((i == reserve_page) && (p < 4 || p > 15)) {
    cli_printf("  Page number must be >=3 and <=15");
    return;
  }

  // check if the Page is already plugged or reserved, we cannot reserve it
  if (TULIP_Pages.isPlugged(p, 1)) {
    cli_printf("  Page %X is already plugged, please unplug first", p);
    return; // exit the function
  }

  switch (i) {
    case reserve_cx:      // reserve Page 3+5 for HP41-CX module, but 3 is always reserved
    case reserve_timer:   // reserve Page 5 for Timer module
      if (TULIP_Pages.isReserved(5)) {
        cli_printf("  Page 5 is already reserved, please clear the reservation first", p);
      }
      TULIP_Pages.plug(5, 1, PAGE_RESERVED, 0); // plug Page 5 for TIME module
      TULIP_Pages.setComment(5, 1, P5_txt); // set the comment 
      p = 5;
      comment = P5_txt;
      break;
    case reserve_printer: // reserve Page 6 for Printer module
      if (TULIP_Pages.isReserved(5)) {
        cli_printf("  Page 6 is already reserved, please clear the reservation first", p);
      }
      TULIP_Pages.plug(6, 1, PAGE_RESERVED, 0); // plug Page 6 for a Printer module
      TULIP_Pages.setComment(6, 1, P6_txt); // set the comment 
      p = 6;
      comment = P6_txt;
      break;
    case reserve_hpil:    // reserve Page 7 for HPIL module
      if (TULIP_Pages.isReserved(7)) {
        cli_printf("  Page 7 is already reserved, please clear the reservation first", p);
      }
      TULIP_Pages.plug(7, 1, PAGE_RESERVED, 0); // plug Page 7 for HP-IL
      TULIP_Pages.setComment(7, 1, P7_txt); // set the comment 
      p = 7;
      comment = P7_txt;
      break;
    case reserve_clear:   // clear the reservation of the given Page
      if (p == 0) {
        // clear all reservations
        // cli_printf("  Clearing all Page reservations");
        for (int i = 4; i < 16; i++) {
          if (TULIP_Pages.isReserved(i)) {
            TULIP_Pages.unplug(i, 1); // unplug the Page
            TULIP_Pages.unplug(i, 0);
          }
        }
        cli_printf("  All Page reservation cancelled");
        return; // exit the function
      }
      if (TULIP_Pages.isReserved(p)) {
        TULIP_Pages.unplug(p, 1); // unplug the Page
        TULIP_Pages.unplug(p, 0);
        cli_printf("  Reservation of Page %X cleared", p);
        return;
      } else {
        cli_printf("  Page %X is not reserved, nothing to clear", p);
        return; // exit the function
      }
      break;
    case reserve_page:    // reserve a specific Page with a comment
      // check if the Page is reserved, we cannot reserve it
      if (TULIP_Pages.isReserved(p)) {
        cli_printf("  Page %X is already reserved, please clear the reservation first", p);
        return; // exit the function
      }
        TULIP_Pages.plug(p, 1, PAGE_RESERVED, 0); // plug the Page for reservation
        TULIP_Pages.setComment(p, 1, comment); // set the comment for the Page
        // cli_printf("  Reserved Page %X for comment: %s", p, comment);
      break;
    default:
      cli_printf("  Invalid reservation function");
      return; // exit the function
  }

  // reserve the page for the given file name
  // TULIP_Pages.reserve(p, fname);
  TULIP_Pages.save();             // save the page settings in FRAM
  cli_printf("  Page %X reserved for %s", p, comment);
}


// the CAT function shows the current ROM map
// p=0 means show the current ROM map
// P= 4..F means show ROM details for that Page and Bank
void uif_cat(int p, int b)
{
  int flags = 0; // flags for the page settings
  char rev[8]; // buffer for the revision string

  char FileNm[32];

  #ifdef DEBUG
  TULIP_Pages.dumpAll(); // dump the current page settings to the console
  #endif

  if (p == 0) {

    cli_printf("  Current ROM map:");
    cli_printf("  Page - Bank - XROM -  Rev  - #Funcs - Flags - File  ");
    cli_printf("  ----   ----   ----   -----   ------   -----   ---------------------");

    for (int pg = 0; pg < 16; pg++) {
      //go through all Pages 0..15
      for (int bk = 1; bk < 5; bk++) {
        // go through all Banks 1..4
        
        // check if the page is reserved
        if (TULIP_Pages.isReserved(pg) && (bk == 1)) {
          // page is reserved, so show a message and print the stored filename
          // only check for Bank 1, other Banks are not relevant for reserved pages
          TULIP_Pages.getFileName(pg, bk, FileNm);           // get the file name of the reserved page
       // cli_printf("  Page - Bank - XROM -  Rev  - #Funcs - Flags   - File  ");
       // cli_printf("  ----   ----   ----   -----   ------   -----   ---------------------");
          cli_printf("  %3X                                   %04X    - reserved : %s", pg, TULIP_Pages.getflags(pg, bk), FileNm);
          break; // break the inner loop, as we only show the first Bank for reserved pages
        } else {

        // Page is not reserved, use the generic loop to get the information. It is empty or something is there
        // check if the page is plugged and has a valid ROM image
          if (!TULIP_Pages.isPlugged(pg, bk)) {        
            TULIP_Pages.getFileName(pg, bk, FileNm);           // get the file name, if any
            // this is an empty page, so show a message

            if (bk ==1) {
              // we only print bank 1 if it is not plugged
              // other unplugged banks are skipped
           // cli_printf("  Page - Bank - XROM -  Rev  - #Funcs - Flags   - File  ");
           // cli_printf("  ----   ----   ----   -----   ------   -----   ---------------------");
              cli_printf("  %3X    %3d                            %04X    - available: %s", pg, bk, TULIP_Pages.getflags(pg, bk), FileNm);
            } else {
              // we do not print empty other banks, as they are not relevant for empty pages
              // cli_printf("          %3d                                      - empty", bk);
            }

          } else if (TULIP_Pages.isPlugged(pg, bk) && !TULIP_Pages.isEmbeddedROM(pg, bk)) {
            // Normal plugged ROM, so show the details
            TULIP_Pages.getRevision(pg, bk, rev);
            int addr = pg * 0x1000; // address of the page
           // cli_printf("  Page - Bank - XROM -  Rev  - #Funcs - Flags   - File  ");
           // cli_printf("  ----   ----   ----   -----   ------   -----   ---------------------");
            if (bk == 1) {
              cli_printf("  %3X    %3d    %4d   %s     %2d     %04X    * %-25s  @ 0x%08X", pg, bk, TULIP_Pages.getword(addr + 0x00, bk), 
                                                                                    rev, 
                                                                                    TULIP_Pages.getword(addr + 0x01, bk),
                                                                                    TULIP_Pages.getflags(pg, bk),
                                                                                    TULIP_Pages.Pages[pg].m_banks[bk].b_img_name,
                                                                                    TULIP_Pages.Pages[pg].m_banks[bk].b_img_data);
            } else {
              // do not print Page number for readability
              cli_printf("         %3d    %4d   %s     %2d     %04X    * %-25s  @ 0x%08X", bk, TULIP_Pages.getword(addr + 0x00, bk), 
                                                                                    rev, 
                                                                                    TULIP_Pages.getword(addr + 0x01, bk),
                                                                                    TULIP_Pages.getflags(pg, bk),
                                                                                    TULIP_Pages.Pages[pg].m_banks[bk].b_img_name,
                                                                                    TULIP_Pages.Pages[pg].m_banks[bk].b_img_data);
            }
          } else if (TULIP_Pages.isPlugged(pg, bk) && TULIP_Pages.isEmbeddedROM(pg, bk)) {
            // Emmbedded ROM
            int addr = pg * 0x1000; // address of the page in FLASH
            TULIP_Pages.getRevision(pg, bk, rev);
           // cli_printf("  Page - Bank - XROM -  Rev  - #Funcs - Flags   - File  ");
           // cli_printf("  ----   ----   ----   -----   ------   -----   ---------------------");
            if (bk ==1) {
              cli_printf("  %3X    %3d    %4d   %s     %2d     %04X    * %-25s  @ 0x%08X ", pg, bk, TULIP_Pages.getword(addr + 0x00, bk), 
                                                                                    rev, 
                                                                                    TULIP_Pages.getword(addr + 0x01, bk),
                                                                                    TULIP_Pages.getflags(pg, bk),
                                                                                    TULIP_Pages.Pages[pg].m_banks[bk].b_img_name,
                                                                                    TULIP_Pages.Pages[pg].m_banks[bk].b_img_data);
            } else {
            // do not print Page number for readability
              cli_printf("         %3d    %4d   %s     %2d     %04X    * %-25s  @ 0x%08X ", bk, TULIP_Pages.getword(addr + 0x00, bk), 
                                                                                    rev, 
                                                                                    TULIP_Pages.getword(addr + 0x01, bk),
                                                                                    TULIP_Pages.getflags(pg, bk),
                                                                                    TULIP_Pages.Pages[pg].m_banks[bk].b_img_name,
                                                                                    TULIP_Pages.Pages[pg].m_banks[bk].b_img_data);
            }
          } else {
            // nothing plugged in this page/bank
            cli_printf("  %3X     %3d                                      - nothing to see here", pg, bk);
         }
        }
      }
    } 

  } else {
    // p = 4..F means show ROM details for that page
    // b is the bank number, 1..4, when 0 only Bank 1 is shown
    if (! TULIP_Pages.isPlugged(p, b)) {
      // page is not plugged, so show a message
      cli_printf("  Page %X - Bank %d is not plugged", p, b);
      return; 
    }

    TULIP_Pages.dumpPage(p);  // dump the current page settings to the console
    cli_printf("");

    // and now create a hex dump of the entire Page
    char  ShowPrint[250];
    int   ShowPrintLen = 0;
    uint16_t addr = p * 0x1000; // address of the page in FLASH
    uint16_t endaddr = addr + 0x1000; // end of the dump
    int i = 0;                                // counter for the number of bytes in a line
    int m;
    char c;
    do {

      ShowPrintLen = 0;
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "  %04X    ", addr);       // print the address
      // print 16 bytes
      for (m = 0; m < 16; m++) {
        ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%03X ", TULIP_Pages.getword(addr + m, b));
      }
    
      // print byte values as characters
      ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "  ");
      for (m = 0; m < 16; m++) {
        c = TULIP_Pages.getword(addr + m, b) & 0xFF;
        if ((c < 0x20) || (c > 0x7E)) {
          c = '.';
        }
        ShowPrintLen += sprintf(ShowPrint + ShowPrintLen, "%c", c);
      } 
      addr += 16; // increment the address by 16 bytes
      // print the line
      cli_printf("%s", ShowPrint);

    } while (addr < endaddr);   // list 4K bytes by default

  }
}


// toggle the emulation of the selected function in the given page
void uif_emulate(int i, int p) {

  bool sticky = false;

  if (!uif_pwo_low()) return;    // only do this when calc is not running

  switch (i)
  {
    case emulate_status:         // list hardware emulation status
            cli_printf("  Emulation status:");
            cli_printf("  - HP82160A HP-IL    %s", globsetting.get(HP82160A_enabled) ? "enabled":"disabled");
            cli_printf("  - HP82143A Printer  %s", globsetting.get(HP82143A_enabled) ? "enabled":"disabled");      
            cli_printf("  - HP82153A Wand     %s", globsetting.get(HP82153A_enabled) ? "enabled":"disabled");
            
            for (int j = 4; j < 16; j++) {
              // go through all Pages 4..F to check the ZEPROM emulation status
              if (TULIP_Pages.isPlugged(j, 1) && (TULIP_Pages.Pages[j].m_bank & bank_sticky)  ) {
                // Found Page with ZEPROM emulation enabled
                cli_printf("  - ZEPROM Page %X     enabled (Sticky Bankswitching)", j);
                sticky = true;
              }
            }
            if (!sticky) {
              cli_printf("  - ZEPROM            disabled (sticky Bankswitching)");
            }
            break;
    case emulate_hpil: // toggle HP-IL emulation
            if (globsetting.get(HP82160A_enabled) == 0) {
              // HP-IL is not enabled, so enable it
              globsetting.set(HP82160A_enabled, 1);
              cli_printf("  HP-IL emulation enabled");
            } else {
              globsetting.set(HP82160A_enabled, 0);
              cli_printf("  HP-IL emulation disabled");
            }        
            break;
    case emulate_printer: // toggle HP82143 Printer emuation
            if (globsetting.get(HP82143A_enabled) == 0) {
              // HP-IL printer is not enabled, so enable it
              globsetting.set(HP82143A_enabled, 1);
              cli_printf("  HP82143 Printer emulation enabled");
            } else {
              globsetting.set(HP82143A_enabled, 0);
              cli_printf("  HP82143 Printer emulation disabled");
            }
            break;  
    case emulate_zeprom: // toggle ZEPROM emulation in the given Page
            // toggle the bank_sticky bit, but only if a module is plugged there
            if (!TULIP_Pages.isPlugged(p, 1)) {
              cli_printf("  Page %X is not plugged, cannot toggle ZEPROM emulation", p);
              return; // exit the function
            }
            if (TULIP_Pages.Pages[p].m_bank & bank_sticky) {
              // bank_sticky bit is set, so clear it
              TULIP_Pages.Pages[(p & 0xFE)    ].m_bank &= ~bank_sticky;
              TULIP_Pages.Pages[(p & 0xFE) + 1].m_bank &= ~bank_sticky;
              cli_printf("  ZEPROM emulation (sticky Bankswitching) disabled in Page %X + %X", p & 0xFE, (p & 0xFE) + 1);
              TULIP_Pages.save(); // save the page settings in FRAM
            } else {
              // bank_sticky bit is clear, so set it
              TULIP_Pages.Pages[(p & 0xFE)    ].m_bank |= bank_sticky;
              TULIP_Pages.Pages[(p & 0xFE) + 1].m_bank |= bank_sticky;
              cli_printf("  ZEPROM emulation (sticky Bankswitching) enabled in Page %X + %X", p & 0xFE, (p & 0xFE) + 1);
              TULIP_Pages.save(); // save the page settings in FRAM
            }
            break;
    case emulate_wand: // toggle HP82153A Wand emulation
            if (globsetting.get(HP82153A_enabled) == 0) {
              // HP-IL Wand is not enabled, so enable it
              globsetting.set(HP82153A_enabled, 1);
              cli_printf("  HP82153A Wand emulation enabled");
            } else {
              globsetting.set(HP82153A_enabled, 0);
              cli_printf("  HP82153A Wand emulation disabled");
            }
            break;
    default: 
            // no other actions defined here
            break;
  }

  globsetting.save(); // save the settings in FRAM

}

void uif_delete(const char *fname)
{
  // delete the file with the given name from FLASH
  // check if the file exists in flash
  uint32_t offs = ff_findfile(fname);
  if (offs == NOTFOUND) {
    cli_printf("  file \"%s\" not found", fname);
    return;
  }

  // only delete if PWO is low
  if (!uif_pwo_low()) {
    cli_printf("  file \"%s\" not deleted, PWO is high", fname);
    return;
  } 

  ModuleMetaHeader_t *MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct  
  // file exists, so delete it
  cli_printf("  deleting file \"%s\" from FLASH", fname);
  cli_printf("  DO NOT FORGET TO UNPLUG THE PAGE IF IT IS PLUGGED");

  // show file offset en address and size
  cli_printf("  filename                         type      size  address     next file"); 
  cli_printf("  -------------------------------  ----  --------  ----------  ----------");
  cli_printf("  %-31s  0x%02X  %8d  0x%08X  0x%08X", 
            MetaH->FileName, MetaH->FileType, MetaH->FileSize, offs, MetaH->NextFile);

  // delete the file
  if (ff_write(offs, FILETYPE_DELETED)) {
    cli_printf("  file \"%s\" marked as deleted", fname);
  } else {
    cli_printf("  file \"%s\": ERROR deleting file", fname);
  };
}


// function for the HP82143A printer
//    1 - status           // get status
//    2 - power            // toggle power
//    3 - trace            // printer mode trace
//    4 - norm             // printer mode normal
//    5 - man              // printer mode manual
//    6 - paper            // toggle Out Of Paper status
//    7 - print            // push PRINT button
//    8 - adv              // push ADV button

void uif_printer(int i) {

  uint16_t pr_mode;

  if (i == printer_irtest) {
    // test the infrared LED
    // simply send a string to the IR led, no throttling
    for (int i = 'A'; i <= 'Z'; i++) {
      // send the string to the IR led
      // this is a test, so no throttling
      PrintIRchar(i);
    }	
    PrintIRchar(00);    // send a carriage return
    PrintIRchar(0xE0);    // send a line feed
    return;
  }

  if (i == printer_irtog) {
    // test the IR LED power consumption by toggling it
    IR_toggle(); // toggle the IR LED
    // wait for 10 ms
    sleep_ms(10); // wait for 10 ms
    cli_printf("  IR LED (GPIO %d) is %s", P_IR_LED, gpio_get(P_IR_LED) ? "ON" : "OFF");
    return;
  }

  if (!globsetting.get(HP82143A_enabled)) {
    cli_printf("  HP82143A Printer not plugged, settings will not be active until the printer is plugged");
  }

  switch (i) {
    case 1: // status
            cli_printf("  printer power: %s", globsetting.get(PRT_power) ? "ON":"OFF");
            pr_mode = globsetting.get(PRT_mode);
            switch (pr_mode) {
              case 0:  cli_printf("  printer mode : MAN"); break;
              case 1:  cli_printf("  printer mode : NORM"); break;
              case 2:  cli_printf("  printer mode : TRACE"); break;
              case 3:  cli_printf("  printer mode : TRACE*"); break;
              default: cli_printf("  printer mode : <invalid>");
            }
            cli_printf("  printer BUSY : %s", SELP9_status_BUSY ? "on":"off");
            cli_printf("  status word  : %04X", SELP9_status);
            cli_printf("    SMA  %01d - bit 15 - SMA, TRACE mode when set", (SELP9_status >> 15) & 0x0001);
            cli_printf("    SMB  %01d - bit 14 - SMB, NORM when set, MAN when bit 14 and 15 are clear", (SELP9_status >> 14) & 0x0001);
            cli_printf("    PRT  %01d - bit 13 - PRT, PRINT key down", (SELP9_status >> 13) & 0x0001);
            cli_printf("    ADV  %01d - bit 12 - ADV, PAPER ADVANCE key down", (SELP9_status >> 12) & 0x0001);
            cli_printf("    OOP  %01d - bit 11 - OOP, Out Of Paper", (SELP9_status >> 11) & 0x0001);
            cli_printf("    LB   %01d - bit 10 - LB , Low Battery", (SELP9_status >> 10) & 0x0001);
            cli_printf("    IDL  %01d - bit  9 - IDL, Idle", (SELP9_status >> 9) & 0x0001);
            cli_printf("    BE   %01d - bit  8 - BE , Buffer Empty", (SELP9_status >> 8) & 0x0001);
            cli_printf("    LCA  %01d - bit  7 - LCA, Lower Case Alpha", (SELP9_status >> 7) & 0x0001);
            cli_printf("    SCO  %01d - bit  6 - SCO, Special Column Output (Graphics mode)", (SELP9_status >> 6) & 0x0001);
            cli_printf("    DWM  %01d - bit  5 - DWM, Double Wide Mode", (SELP9_status >> 5) & 0x0001);
            cli_printf("    TEO  %01d - bit  4 - TEO, Type of End-Of-Line", (SELP9_status >> 4) & 0x0001);
            cli_printf("    EOL  %01d - bit  3 - EOL, Last End-Of-Line", (SELP9_status >> 3) & 0x0001);
            cli_printf("    HLD  %01d - bit  2 - HLD, Hold for Paper", (SELP9_status >> 2) & 0x0001);
            cli_printf("    bt1  %01d - bit  1 - not used, always returns 0", (SELP9_status >> 1) & 0x0001);
            cli_printf("    bt0  %01d - bit  0 - not used, always returns 0", (SELP9_status >> 0) & 0x0001);
            break;
    case 2: // toggle printer power
            if (globsetting.get(PRT_power)) {
              globsetting.set(PRT_power, 0);
            } else {
              globsetting.set(PRT_power, 1);
            }
            cli_printf("  printer power: %s", globsetting.get(PRT_power) ? "ON":"OFF");
            break;
    case 3: // set printer to TRACE status, bit 15 set, bit 14 clear
            SELP9_status = SELP9_status | prt_SMA_mask & ~prt_SMB_mask;
            globsetting.set(PRT_mode, 2);
            cli_printf("  printer set to TRACE mode");
            break;
    case 4: // set printer to NORM mode, bit 15 clear, bit 14 set
            SELP9_status = (SELP9_status & ~prt_SMA_mask) | prt_SMB_mask;
            globsetting.set(PRT_mode, 1);
            cli_printf("  printer set to NORM mode");
            break;    
    case 5: // set printer to MAN status, bit 14 and 15 of printer status are 0
            SELP9_status = SELP9_status & ~prt_SMA_mask & ~prt_SMB_mask;
            globsetting.set(PRT_mode, 0);
            cli_printf("  printer set to MAN mode");
            break;
    case 6: // toggle the Out Of Paper status in the printer status word
            SELP9_status ^= prt_OOP_mask;   // XOR of Out Of Paper status bit
            if (globsetting.get(PRT_paper)) {
              globsetting.set(PRT_paper, 0);
            } else {
              globsetting.set(PRT_paper, 1);
            }
            cli_printf("  printer Out Of Paper status %s", SELP9_status & prt_OOP_mask ? "on":"off");
            break;
    case 7: // push the printer PRINT button, short push only, button status is reset after 1 read
            // this is printer status bit 13
            if (!globsetting.get(HP82143A_enabled)) {
              cli_printf("  HP82143A ROM not plugged, function ignored");
              break;
            } 
            SELP9_status = SELP9_status | prt_PRT_mask;
            keycount_print = 2;
            wakemeup_41();
            cli_printf("  printer PRINT key pushed");
            break;
    case 8: // push the printer ADV button, short push only, button status is reset after 1 read
            // this is printer status bit 12
            if (!globsetting.get(HP82143A_enabled)) {
              cli_printf("  HP82143A ROM not plugged, function ignored");
              break;
            } 
            SELP9_status = SELP9_status | prt_ADV_mask;
            keycount_print = 2;
            wakemeup_41();
            cli_printf("  printer ADV key pushed");
            break;            

    default: 
            // no other actions defined here
            ;
            
  }  // switch (i)

}

// functions for Extended Memory control
// values passed:
//   1  command 1 - status
//   2  command 2 - dump
//   3  command 3 - program FRAM test pattern
//   4  command 4 - clear XMEM and test pattern
//  10  plug 0 XMEM modules
//  11  plug 1 XMEM module
//  12  plug 2 XMEM modules
void uif_xmem(int i)          
{
  int j;
  uint64_t reg_cont;
  int num_mods;

  if (!uif_pwo_low()) return;    // only do this when calc is not running

  switch (i) {
    case 1: // status
            cli_printf("  XMEM modules plugged: %1d", globsetting.get(xmem_pages));
            break;
    case 2: // dump XMEM contents, starting at FRAM 0x1E00
            cli_printf("  XMEM contents from FRAM (non-zero registers only)");
            cli_printf("  XMEM modules plugged: %1d", globsetting.get(xmem_pages));
            for (j = 0; j < 0x200; j++) {
              fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, (XMEMstart + j * 8), (uint8_t*)&reg_cont, 8);

              // printf(" R%03d = 0x%" PRIx64 "\n", j * 8,);
              if (reg_cont != 0) {
                cli_printf("  REG %03X = 0x%" PRIX64 "", j + 0x200, reg_cont);
              }
            }
            break;
    case 3: // program test pattern in XMEM
            reg_cont = 0x4041404140414041;      // set reg_cont to init value
            for (j = 0; j < 0x200; j++) {
              reg_cont = reg_cont + 0x0101010101010101;
              fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, (XMEMstart + j * 8), (uint8_t*)&reg_cont, 8);
            }
            cli_printf("  FRAM test pattern programmed in Extended Memory");
            cli_printf("  use xmem dump to verify");
            break;           
    case 4: // erase all XMEM
            reg_cont = 0;      // set reg_cont to 0
            for (j = 0; j < 0x200; j++) {
              fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, (XMEMstart + j * 8), (uint8_t*)&reg_cont, 8);
            }
            cli_printf("  all Extended Memory registers set to 0");
            break;    
    case 10: // unplug all XMEM modules
            globsetting.set(xmem_pages, 0);
            globsetting.save();
            break;
    case 11: // only 1 XMEM module
            globsetting.set(xmem_pages, 1);
            globsetting.save();
            break;
    case 12: // 2 XMEM modules
            globsetting.set(xmem_pages, 2);
            globsetting.save();
            break;
    default:
            // no other actions defined here
            ;
  }
}

/*
#define TRACER_HELP_TXT "tracer functions\r\n\
        [no argument] shows the tracer status\r\n\
        status        shows the tracer status\r\n\
        buffer        shows the trace buffer size\r\n\
        buffer <size> set the tracer buffer size in number of samples\r\n\
                      default is 5000, maximum is about 10.000 samples\r\n\
                      requires a reboot to take effect!\r\n\
        pretrig      shows the pre-trigger buffer size and status\r\n\
        pretrig <size> set the pre-trigger buffer size in number of samples\r\n\
                      default is 32, maximum is 256 samples\r\n\
        trace         toggle tracer enable/disable\r\n\
        sysloop       toggle tracing of system loops (RSTKB, RST05, BLINK01 and debounce)\r\n\
        sysrom        toggle system rom tracing (Page 0 - 5)\r\n\
        ilrom         toggle tracing of Page 6+7\r\n\
        hpil          toggle HP-IL tracing to ILSCOPE USB serial port\r\n\
        pilbox        toggle PILBox serial tracing to ILSCOPE USB serial port\r\n\
        ilregs        toggle tracing of HP-IL registers\r\n\
        save          save tracer settings\r\n"

        #define trace_status      1
        #define trace_buffer      2
        #define trace_pretrig     3
        #define trace_trace       4
        #define trace_sysloop     5    
        #define trace_sysrom      6
        #define trace_ilrom       7
        #define trace_hpil        8
        #define trace_pilbox      9
        #define trace_ilregs     10
        #define trace_save       11
*/

        // settings for tracer, enable by default
        // gsettings[tracer_enabled]       = 1;
        // gsettings[tracer_ilregs_on]     = 1;
        // gsettings[tracer_sysrom_on]     = 1;
        // gsettings[tracer_sysloop_on]    = 1;
        // gsettings[tracer_ilroms_on]     = 1;

        // HP-IL scope settings
// #define     ilscope_enabled     50          // HP-IL scope enabled
// #define     ilscope_PIL_enabled 51          // PILBox tracing enabled

                // block some known system loops
        //   0x0098 - 0x00A1       RSTKB and RST05
        //   0x0177 - 0x0178       delay for debounce
        //   0x089C - 0x089D       BLINK01

void uif_tracer(int i, int bufsize) {
  uint16_t stat;
  int size;

  switch (i) {
    case trace_status: // status

            // show the current tracer status
            cli_printf("  Main Trace buffer       %5d samples (%d KByte)", globsetting.get(tracer_mainbuffer), 
                       globsetting.get(tracer_mainbuffer) * sizeof(TraceLine) / 1024 );
            cli_printf("  PreTrig buffer          %5d samples (%d KByte)", globsetting.get(tracer_pretrig), 
                       globsetting.get(tracer_pretrig) * sizeof(TraceLine) / 1024 );
            cli_printf("  HP41 tracer             %s", globsetting.get(tracer_enabled) ? "enabled ":"disabled");
            cli_printf("  system loop tracing     %s (RSTKB, RST05, BLINK01 and debounce)", globsetting.get(tracer_sysloop_on) ? "enabled ":"disabled");
            cli_printf("  system ROM tracing      %s (Page 0..5)", globsetting.get(tracer_sysrom_on) ? "enabled ":"disabled");
            cli_printf("  tracing of IL roms      %s (Page 6+7)", globsetting.get(tracer_ilroms_on) ? "enabled ":"disabled");
            cli_printf("  IL scope traffic        %s", globsetting.get(ilscope_IL_enabled) ? "enabled ":"disabled");
            cli_printf("  PILBox traffic          %s", globsetting.get(ilscope_PIL_enabled) ? "enabled ":"disabled");
            cli_printf("  tracing of IL regs      %s", globsetting.get(tracer_ilregs_on) ? "enabled ":"disabled");
            break;
    case trace_buffer: // buffer
            // show and/or set the current buffer size
            if (!uif_pwo_low()) {
              cli_printf("  PreTrigger Buffer size can only be set when PWO is low");
              return; // exit the function
            }
            size = globsetting.get(tracer_mainbuffer);
            cli_printf("  TraceBuffer %d samples using %d KBytes", size, size * sizeof(TraceLine));
            if (bufsize > 0) {
              // set the new buffer size
              if (bufsize < 1000 || bufsize > 10000) {
                cli_printf("  TraceBuffer must be between 1000 and 10000 samples");
                return;
              }
              globsetting.set(tracer_mainbuffer, bufsize);
              cli_printf("  TraceBuffer set to %d samples, requires a reboot to take effect: system REBOOT", bufsize);
              globsetting.save(); // save the settings in FRAM
            }
            break;
    case trace_pretrig: // pre-trigger buffer
            // show and /or set the current pre-trigger buffer size
            if (!uif_pwo_low()) {
              cli_printf("  PreTrigger Buffer size can only be set when PWO is low");
              return; // exit the function
            }
            size = globsetting.get(tracer_pretrig);
            cli_printf("  PreTrigger Buffer %d samples using %d KBytes", size, size * sizeof(TraceLine));
            if (bufsize > 0) {
              // set the new pre-trigger buffer size
              if (bufsize < 32 || bufsize > 256) {
                cli_printf("  PreTrigger Buffer must be between 1 and 256 samples");
                return;
              }
              globsetting.set(tracer_pretrig, bufsize);

              cli_printf("  PreTrigger Buffer set to %d samples", bufsize);
            }
            break;
    case trace_trace: // trace
            globsetting.set(tracer_enabled, !globsetting.get(tracer_enabled));
            trace_enabled != tracer_enabled;
            cli_printf("  HP41 tracer         %s", globsetting.get(tracer_enabled) ? "enabled ":"disabled");
            break;            
    case trace_sysloop: // sysloop
            globsetting.set(tracer_sysloop_on, !globsetting.get(tracer_sysloop_on));
            cli_printf("  system loop tracing %s (RSTKB, RST05, BLINK01 and debounce)", globsetting.get(tracer_sysloop_on) ? "enabled ":"disabled");
            break; 
    case trace_sysrom: // sysrom
            globsetting.set(tracer_sysrom_on, !globsetting.get(tracer_sysrom_on));
            cli_printf("  system ROM tracing  %s (Page 0..5)", globsetting.get(tracer_sysrom_on) ? "enabled ":"disabled");
            break; 
    case trace_ilrom: // ilrom
            globsetting.set(tracer_ilroms_on, !globsetting.get(tracer_ilroms_on));
            cli_printf("  tracing of IL roms  %s (Page 6+7)", globsetting.get(tracer_ilroms_on) ? "enabled ":"disabled");
            break; 
    case trace_hpil: // hpil scope
            globsetting.set(ilscope_IL_enabled, !globsetting.get(ilscope_IL_enabled));
            cli_printf("  IL scope traffic    %s", globsetting.get(ilscope_IL_enabled) ? "enabled ":"disabled");
            break; 
    case trace_pilbox: // PILBox scope
            globsetting.set(ilscope_PIL_enabled, !globsetting.get(ilscope_PIL_enabled));
            cli_printf("  PILBox traffic      %s", globsetting.get(ilscope_PIL_enabled) ? "enabled ":"disabled");
            break;  
    case trace_ilregs: // ilregs
            globsetting.set(tracer_ilregs_on, !globsetting.get(tracer_ilregs_on));
            cli_printf("  tracing of IL regs  %s", globsetting.get(tracer_ilregs_on) ? "enabled ":"disabled");
            break; 
    case trace_save: // save
            // only if PWO is low
            if (!uif_pwo_low()) {
              return;
            }
            globsetting.save();
            cli_printf("  tracer setting saved in FRAM");
            break; 
    default:
            // no other actions defined here
            ;         
  }          
}        


#define STORAGE_CMD_TOTAL_BYTES 100


// functions for the flash command
//  1        status        shows the flash status
//  2        dump          dump the flash contents
//  3        init          initialize the flash file system
//  4        nukeall       erase all flash pages
void uif_flash(int i, uint32_t addr) {
  uint8_t id[8];
  int sleepcount = 1000;
  uint32_t dump_addr = 0; // default dump address
  flash_devinfo_size_t flash_size = flash_devinfo_get_cs_size(0);

  uint8_t txbuf[STORAGE_CMD_TOTAL_BYTES] = {0x9f};
  uint8_t rxbuf[STORAGE_CMD_TOTAL_BYTES] = {0};
  flash_do_cmd(txbuf, rxbuf, STORAGE_CMD_TOTAL_BYTES);
  int capacity = 1 << rxbuf[3];

  // only do this when PWO is low
  if (!uif_pwo_low()) return;

  switch (i) {
    case 1: // status, just show ID from FLASH and reported FLASH size
            flash_get_unique_id(id);
            cli_printf("  FLASH ID       : %02X%02X%02X%02X%02X%02X%02X%02X", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
            cli_printf("  FLASH CS size  : %X size indicator", flash_size);
            cli_printf("  FLASH size     : %d MByte reported by firmware", PICO_FLASH_SIZE_BYTES / (1024 * 1024));
            cli_printf("  FLASH capacity : %d bytes / %d MByte reported by device", capacity, capacity / (1024 * 1024));

            break;
    case 2: // dump
            if (addr == 0x40414243) {
              dump_addr = flash_dump_addr;
              flash_dump_addr = flash_dump_addr + 0x1000;  
            } else {
              dump_addr = addr;
              flash_dump_addr = dump_addr + 0x1000;  
            } 
            cli_printf(" dumping FLASH contents from address 0x%08X [next: 0x%08X]", dump_addr, flash_dump_addr);
            ff_show(dump_addr);
            break;
    case 3: // init
            ff_init();
            break;
    case 4: // nukeall

            cli_printf("  ERASING THE FLASH FILE SYSTEM in 4 seconds!! press any key to cancel");
            while(true) {
              tud_task();                 // to process IO until the watchdog triggers
              sleep_ms(2);
              if (cdc_available(ITF_CONSOLE)) {
                cdc_read_flush(ITF_CONSOLE);
                cli_printf("  NUKE cancelled");
                return;
              }
              sleepcount--;
              if (sleepcount == 0) {  
                cli_printf("  NUKING FILE SYSTEM");
                // Erase the FLASH File System when the counter expires
                ff_nuke();
                cli_printf("  FLASH FILE SYSTEM ERASED");
                cli_printf("  use the flash INIT command to initialize the FLASH File system");
                return;
              }
            }
            break;
    case 5: // dump FRAM for debugging
            // this is a special case, dump the FRAM contents
            if (addr == 0x40414243) {
              dump_addr = fram_dump_addr;
              fram_dump_addr = fram_dump_addr + 0x1000;  
            } else {
              dump_addr = addr;
              fram_dump_addr = dump_addr + 0x1000;  
            } 
            cli_printf(" dumping FRAM contents from address 0x%05X [next: 0x%05X]", dump_addr, fram_dump_addr);
            fram_show(dump_addr);
            break;

    default:
            // no other actions defined here
            ;         
  }          
}


void fr_init_filesystem()
{

    // initialize the FRAM device by creating the FRAM file system  
    cli_printf("  Initializing FRAM File System");

    // create the first files in FRAM:
    // - TULIP4041 FRAM HEADER          FRAM header
    // - TULIP_Global_Settings.TGL      all persistent settings
    // - TULIP_Module_MAP.TRM           the default rom map
    // - TULIP_Tracefilters.TRF         the default trace filters

    //create the FRAM header file
    ModuleMetaHeader_t MetaH;

    // FRAM Header file
    MetaH.FileType = FILETYPE_4041;
    strcpy(MetaH.FileName, "TULIP4041 FRAM HEADER");
    MetaH.FileSize = 31; // for version string

    // create the version string to be programmed in FRAM
    char versionstring[32];
    snprintf(versionstring, sizeof(versionstring), "%s", Version_Number);

    MetaH.NextFile = FRAM_SETTINGS_FILE;    // always 0x100 for the settings file
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_HEADER, (uint8_t*)&MetaH, sizeof(ModuleMetaHeader_t));
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_HEADER + sizeof(ModuleMetaHeader_t), (uint8_t*)versionstring, 31);
    #ifdef DEBUG
      cli_printf("  FRAM Header created at 0x%05X", FRAM_HEADER);
    #endif

    // Global Settings file
    MetaH.FileType = FILETYPE_GLOB;
    MetaH.FileSize = sizeof(globsetting.gsettings); // size of global settings
    strcpy(MetaH.FileName, "TULIP_Config.TCF");
    // MetaH.FileSize = sizeof(GSettings.gsettings); // size of global settings
    MetaH.NextFile = FRAM_ROMMAP_HEADER;
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_SETTINGS_FILE, (uint8_t*)&MetaH, sizeof(ModuleMetaHeader_t));
    // write default global settings
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_SETTINGS_CONTENT, (uint8_t*)globsetting.gsettings, sizeof(globsetting.gsettings));

    #ifdef DEBUG
      cli_printf("  Global Configuration file created at 0x%05X", FRAM_SETTINGS_FILE);
    #endif

    // ROM Map file
    MetaH.FileType = FILETYPE_MMAP;
    strcpy(MetaH.FileName, "TULIP_Module.TMM");
    MetaH.FileSize = sizeof(TULIP_Pages.Pages); // size of rom map
    MetaH.NextFile = FRAM_TRACER_HEADER;
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_ROMMAP_HEADER, (uint8_t*)&MetaH, sizeof(ModuleMetaHeader_t));
    // write default rom map
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_ROMMAP_CONTENT, (uint8_t*)TULIP_Pages.Pages, sizeof(TULIP_Pages.Pages));
    #ifdef DEBUG
      cli_printf("  Module Map Map file created at 0x%05X", FRAM_ROMMAP_HEADER);
    #endif

    // Trace Filters file
    MetaH.FileType = FILETYPE_TRAC;
    strcpy(MetaH.FileName, "TULIP_Tracer.TRF");
    MetaH.FileSize = sizeof(TraceFilter.m_filter) ; // size of trace filters
    MetaH.NextFile = FRAM_FS_CONTENT_START;         // here is where the non-fixed files start
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_TRACER_HEADER, (uint8_t*)&MetaH, sizeof(ModuleMetaHeader_t));
    // write default trace filters
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_TRACER_CONTENT, (uint8_t*)TraceFilter.m_filter, sizeof(TraceFilter.m_filter));
    #ifdef DEBUG
      cli_printf("  Tracer Filters file created at 0x%05X", FRAM_TRACER_HEADER);
    #endif

    // Finally we mark the type of the next file as empty
    MetaH.FileType = FILETYPE_FFFF;
    fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_FS_CONTENT_START, (uint8_t*)&MetaH.FileType, sizeof(uint8_t));

    cli_printf("  FRAM File System initialized"); 

}

void fr_list_files()
{

  // list the files in FRAM
  uint32_t offs = FRAM_HEADER;
  uint32_t end = FRAM_SIZE;
  ModuleMetaHeader_t MetaH;          // pointer to meta header for getting info of next file

  ModuleFileHeader_t *ModH;           // pointer to MOD header for getting MOD details
  ModuleHeader_t *myPageHeader;       // pointer to the individual pages header
  uint32_t ROMoffset;                 // offset to the ROM image in FLASH
  uint16_t *myROMImage;               // buffer pointer for the ROM image
  V1_t *myV1;                         // pointer to the module contents in BIN format (5120 bytes)
  ModuleFileHeader_t *MODoffset;
  int filecounter = 0;                // counter for the number of files found in the directory
  
  if (!fr_isinited()) {
    cli_printf("  FRAM File system not initialized, please run INIT first");
    cli_printf("  Total space in FRAM appr %d Kbytes free", FRAM_SIZE/1024);
    return;
  }

  /*
  // locate the file with the name fname
  if (fname != NULL) {
    // a filename is given so we only show the details of this file
    offs = ff_findfile(fname);
    if (offs == NOTFOUND) {
      cli_printf("  file \"%s\" not found", fname);
      return;
    }
    // show the file details here
    MetaH = (ModuleMetaHeader_t*)(FF_SYSTEM_BASE + offs);       // map header to struct
    cli_printf("  filename                         type      size  address     next file");
    cli_printf("  -------------------------------  ----  --------  ----------  ----------");
    cli_printfn("  %-31s  0x%02X  %8d  0x%08X  0x%08X", 
              MetaH->FileName, MetaH->FileType, MetaH->FileSize, offs, MetaH->NextFile);
    if (MetaH->FileType == FILETYPE_DELETED) {
      // deleted file
      cli_printf("  ** DELETED FILE **");
      return;
    } else if (MetaH->FileType == FILETYPE_DUMMY) {
      // dummy file
      cli_printf("  ** DUMMY FILE **");
      return;
    }
  */

  // no filename given, so we list all files
  // with i = 1 - all we list all files, including deleted and dummy files
  //      i = 2 - ext we list all files with more details
  //      i = 3 - flash we list all files in FLASH only
  //      i = 4 - fram we list all files in FRAM (not yet supported)

  char line[80];

  // layout for listing
  //  filename                        type   size    address     next
  // "...............................  ..  ........  ........  ........"
  cli_printf("  filename                         type      size  address     next file");
  cli_printf("  -------------------------------  ----  --------  ----------  ----------");

  while (offs < end) {
    tud_task();  // must keep the USB port updated

    // read header of the file at offs from fram into MetaH
    fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, offs, (uint8_t*)&MetaH, sizeof(ModuleMetaHeader_t));

    if (MetaH.FileType == FILETYPE_FFFF) {
      // end of chain reached
      uint32_t free = end - offs;
      cli_printf("  ** Total files found:                            %d", filecounter);
      cli_printf("  ** END OF FILE SYSTEM **                         0x%08X", offs);
      cli_printf("  ** UNUSED SPACE UNTIL **                         0x%08X - appr %d Kbytes free", end, free/1024);
      return;
    }

    // build the string for showing the listing
    sprintf(line, "  %-31s  0x%02X  %8d  0x%08X  0x%08X", 
              MetaH.FileName, MetaH.FileType, MetaH.FileSize, offs, MetaH.NextFile);
    cli_printfn(line);
    filecounter++;  // count the number of files found
    if (MetaH.FileType == FILETYPE_DELETED) {
      // deleted file
      cli_printfn("  ** DELETED FILE **");
      filecounter--;  // count the number of files found
    } else if (MetaH.FileType == FILETYPE_DUMMY) {
      // dummy file
      cli_printfn("  ** DUMMY FILE **");
      filecounter--;  // count the number of files found
    }
    cli_printf(" ");

    /*
    if (i == 2) {
      // extended listing, show more details
      // show more details for the MOD/ROM content
      // first check if the file is a ROM or MOD file
      if ((MetaH->FileType == FILETYPE_MOD1) || (MetaH->FileType == FILETYPE_MOD2)) {
        // MOD file, show the MOD header
        ModH = (ModuleFileHeader_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t));
        cli_printf("    MOD Format %s", ModH->FileFormat);
        cli_printf("    MOD Title  %s", ModH->Title);
        cli_printf("    MOD Pages  %d", ModH->NumPages);

        // also add details of the ROM contents later
      } else if (MetaH->FileType == FILETYPE_ROM) {
        // ROM file, show the ROM header
        ShowROMDetails((uint16_t*)(FF_SYSTEM_BASE + offs + sizeof(ModuleMetaHeader_t)));
      } 
        // allother filetypes, just show the info
        // to be added later
    }
    */

    offs = MetaH.NextFile;                                     // go to the next file
  }    


} 


// functions for the flash command
// #define fram_status    1
// #define fram_dump      2
// #define fram_init      3
// #define fram_nukeall   4

void uif_fram(int i, uint32_t addr) {
  uint8_t buf[8];
  int sleepcount = 1000;
  uint32_t dump_addr = 0; // default dump address
  int reported_size;
  uint16_t init_val;

  // only do this when PWO is low
  if (!uif_pwo_low()) return;


  switch (i) {
    case fram_status: // status, just show ID from FRAM
            fr_readid(buf);
            cli_printf("  FRAM ID      : %02X %02X %02X %02X", buf[0], buf[1], buf[2], buf[3]);

            // calculated reported size from buf[2]
            reported_size = (buf[2] & 0x1F) / 4; // size in Mbits
            cli_printf("  FRAM size    : %d Mbit, %d KByte", reported_size, reported_size * 128);

            break;
    case fram_dump: // dump
            if (addr == 0x40414243) {
              dump_addr = fram_dump_addr;
              fram_dump_addr = fram_dump_addr + 0x1000;  
            } else {
              dump_addr = addr;
              fram_dump_addr = dump_addr + 0x1000;  
            } 
            cli_printf(" dumping FRAM contents from address 0x%05X [next: 0x%05X]", dump_addr, fram_dump_addr);
            fram_show(dump_addr);
            break;
    case fram_init: // init ROMMAP only and set to zero
            cli_printf("  initializing FRAM ROMMAP to zero");
            init_val = fr_read16(FRAM_INIT_ADDR);
            cli_printf("  FRAM init value: %04X", init_val);
            if (TULIP_Pages.is_rommmap_inited()) {
              cli_printf("  FRAM ROMMAP already initialized, skipping");
              return;
            }
            TULIP_Pages.init_rommap();
            break;
    case fram_nukeall: // nukeall
            cli_printf("  ERASING ALL FRAM in 4 seconds!! press any key to cancel");
            cli_printf("  this will unplug all modules and reset all TULIP4041 settings to default");
            while(true) {
              tud_task();                 // to process IO until the watchdog triggers
              sleep_ms(2);
              if (cdc_available(ITF_CONSOLE)) {
                cdc_read_flush(ITF_CONSOLE);
                cli_printf("  FRAM NUKE cancelled");
                return;
              }
              sleepcount--;
              if (sleepcount == 0) {  
                cli_printf("  NUKING FRAM to ZERO");
                // Erase the FLASH File System when the counter expires
                fr_nukeall();
                cli_printf("  FRAM ERASED");
                return;
              }
            }

            // after nuking we initialize the FRAM file system
            cli_printf("  initializing FRAM contents");
            globsetting.set_default();
            globsetting.save();
            fram_rommap_init(); 
            break;

    case fram_newinit:
            cli_printf("  initializing FRAM FILE SYSTEM");
            fr_init_filesystem();
            break;

    case fram_list:
            cli_printf("  Listing FRAM files:");
            fr_list_files();
            break;

    default:
            // no other actions defined here
            ;         
  }          
}

void scan_file(const char *fname)
{
  TCHAR str[256];  // buffer for reading text from the file
  char ffname[32];
  int result = 0;
  int rslt = 0;

  uint8_t buf[128];
  int buflen = 0;
  char *ptr = (char*)str;

  // first sort out the file to be scanned
  FIL fil;
  FRESULT fr = f_open(&fil, fname, FA_READ);
  if (FR_OK != fr) {
      cli_printf("  cannot open file: %s, %s (%d)", fname, FRESULT_str(fr), fr);
      return;
  }

  // make a copy of fname in ffname
  strcpy(ffname, fname);

  // convert the filename to upper case
  for (int i = 0; i < strlen(ffname); i++) {
    ffname[i] = toupper(ffname[i]);
  }

  // get the file extension from the filename
  char *ext = strrchr(ffname, '.');
  
  // file is open now for reading, first get the file size
  uint filesize = f_size(&fil);
  cli_printfn("  file %s opened, size %d bytes ", fname, filesize);

  // remove the directory indicator from the filename
  char *p = strrchr(fname, '/');
  if (p != NULL) {
    fname = p + 1;
  }

  // read the first line of the file, if it is not empty this is the name of the program
  f_gets(str, sizeof(str), &fil);

  // if the lengthof the strinbg >1 this is valid program name
  if (strlen((char*)str) > 1) {
    cli_printf("  Program name: %s", str);
  } else {
    cli_printf("  No valid program name found in file");
  }

  // now we can read the file line by line
  cli_printf("  starting scanning of file %s, press X to abort ", fname);


  // start reading the file. This is must be a text file but that is not checked here
  while (!f_eof(&fil)) {
    tud_task();  // keep the USB port alive

    // read a block of data from the file
    f_gets(str, sizeof(str), &fil);

    if (f_error(&fil)) {
      cli_printf("  error reading file: %s, %s (%d)", fname, FRESULT_str(fr), fr);
      result = -1;
      f_close(&fil);
      break;
    }

    // we now have a valid string
    #ifdef DEBUG
      cli_printf("  read line: %s", str);
    #endif

    // if the line is empty skip it
    if (strlen((char*)str) < 1) {
      continue;
    } 

    // if the line starts with a 'P' it is a comment line, skip it
    if (str[0] == 'P' || str[0] == 'p') {
      continue;
    }

    // If the line starts with an 'R' is is the Row indicator, print it and skip it
    if (str[0] == 'R' || str[0] == 'r') {
      // remove any CR/LF from the string
      str[strcspn((char*)str, "\r\n")] = 0;
      cli_printfn("  %-25s   ", str);
      continue;
    }

    // If the string starts with a valid hex character is is a scan line
    // get the values from the line 
    // each byte is represented by 2 hex characters
    // store the bytes in the buffer buf  
    // check if the string starts with a valid hex character
    if (!isxdigit(str[0])) {
      // not a valid scan line, skip it
      #ifdef DEBUG
        cli_printf("  skipping invalid scan line: %s", str);
      #endif
      continue;
    }

    buflen = 0;
    ptr = (char*)str;

    while (*ptr != '\0' && *ptr != '\r' && *ptr != '\n') {
      // read 2 hex characters
      char hexstr[3] = {0};
      hexstr[0] = *ptr++;
      if (*ptr == '\0' || *ptr == '\r' || *ptr == '\n') break;
      hexstr[1] = *ptr++;
      // convert to byte
      buf[buflen] = (uint8_t)strtol(hexstr, NULL, 16);
      buflen++;
    }

    // and send the line to the Wand emulation buffer
    for (int i = 0; i < buflen; i++) {
      queue_add_blocking(&WandBuffer, &buf[i]);
    }
    setflag_FI0();
    wakemeup_41();

    // we now have buflen bytes in buf
    cli_printfn("  scanned %d bytes: ", buflen); 
    // and show the bytes
    for (int i = 0; i < buflen; i++) {
      cli_printfn(" %02X", buf[i]);
    }
    cli_printf(""); 

    // wait for a key to be pressed to continue
    // cli_printf("  scanned one line of %s completed, press any key to continue, press X to exit", fname);
    // while (!cdc_available(ITF_CONSOLE)) {
    //   tud_task();  // keep the USB port alive
    //   sleep_ms(10);
    // }
    tud_task();  // keep the USB port alive
    char ch = cdc_read_char(ITF_CONSOLE);
    if (ch == 'X' || ch == 'x') {
      cli_printf("  scanning of file %s aborted by user", fname);
      break;
    }
  }

  // close the file
  f_close(&fil);
  cli_printf("  file %s closed", fname);

}



bool process_wand_instruction(const char *instruction)
{
  uint8_t barcode; 
  uint8_t code_ascii;
  uint16_t checksum;
  char hexstr[3] = {0};
  size_t len = strlen(instruction);
  char alpha_barcode[32]; // buffer for building the alpha barcode string
  char numeric_barcode[16]; // buffer for building the numeric barcode string
  bool result = true;  
  bool CHS_used = false;
  bool digit_decoded = false;

  // here we receive single token to be processed as a WAND instruction
  #ifdef DEBUG
    cli_printf("  processing W command instruction: %s", instruction);
  #endif

  // first check if the instruction is a string starting with a double quote 
  if (instruction[0] == '\"' ) {
    // this is a string to be sent to the wand buffer

    // first we start building the code to be sent in a new array
    // lenghth is maximum 14 characters:
    //   Byte 1    - 8 bit checksum (end-around carry)
    //   Byte 2    - 4-bit Type indicator  7: alpha replace
    //                                     8: alpha append
    //               4-bit number of ASCII characters (1-14)
    //   Byte 3..n - ASCII characters (max 14)

    // the '\' is used as escape character

    int j = 2; // index for alpha_barcode
    alpha_barcode[0] = 0;     // checksum placeholder
    alpha_barcode[1] = 0x70;  // type and length placeholder, set to ALPHA replace
                              // will be set to 0x80 for ALPHA append if needed

    for (size_t i = 1; i < len; i++) {
      char ch = instruction[i];
      if (ch == '\\') {
        // next character is escaped
        // if it is a hex digit we read the next two characters as hex value
        i++;
        if (i >= len) {
          // invalid escape sequence, ignore
          continue;
        } 
        ch = instruction[i];
        if (isxdigit((unsigned char)ch)) {
          // read two hex digits
          code_ascii = ch - '0';
          if (ch >= 'A' && ch <= 'F') code_ascii = ch - 'A' + 10;
          if (ch >= 'a' && ch <= 'f') code_ascii = ch - 'a' + 10;       
          alpha_barcode[j] = code_ascii << 4;
          i++; 

          if (i >= len) {
            // invalid escape sequence, ignore
            continue;
          }

          ch = instruction[i];
          if (isxdigit((unsigned char)ch)) {
            code_ascii = ch - '0';
            if (ch >= 'A' && ch <= 'F') code_ascii = ch - 'A' + 10;
            if (ch >= 'a' && ch <= 'f') code_ascii = ch - 'a' + 10;       
            alpha_barcode[j] |= code_ascii & 0x0F;
            j++;
            if (j >= sizeof(alpha_barcode)) {
              // buffer overflow, stop processing
              break;
            }
            if (alpha_barcode[j-1] < 0x10) {
              // can crash the HP41
              // convert to SPACE character
              alpha_barcode[j-1] = 0x20; // space character instead
            }
            continue;    // next character
          } else {
            // invalid escape sequence, ignore
            continue;
          }
        }   // end of isxdigit

        // other escape characters
        switch (ch)
        {
          case '\\': // send backslash
                     alpha_barcode[j] = (uint8_t)'\\';   
                     j++;
                     continue;       
                     break;
          case 'S':  // send sigma
                     alpha_barcode[j] = 0x7E;      
                     j++;
                     continue;        
                     break;
          case '+':  // this is append
                     alpha_barcode[j] = 0x80; // set to ALPHA append;
                     // i++;
                     continue;
                     break;
          case 't':  // hanging t
                     alpha_barcode[j] = 0x60;
                     j++;
                     continue;
                     break;
          case '#':  // not equal symbol
                     alpha_barcode[j] = 0x1D;
                     j++;
                     continue;
                     break;
          case 's':  // space
                     alpha_barcode[j] = 0x20;
                     j++;
                     continue;
                     break;
          // case 'u':  // greek mu
          //            alpha_barcode[j] = 0x0C; 
          //            j++;
          //            continue;
          //            break;
          // case 'a':  // angle symbol
          //            alpha_barcode[j] = 0x0D;
          //            j++;
          //            continue;
          //            break;
          case '\'': // single quote
                     alpha_barcode[j] = 0x27;
                     j++;
                     continue;
                     break;
          // case '\"': // double quote
          //            alpha_barcode[j] = 0x22;
          //            j++;
          //            continue;
          //            break;  
          default:   // character not supported, ignore
                     continue;
                     break;
        }
      }  // end of handling escape characters

      // here are only the normal ASCII characters
      // check for the end of the string
      if (ch == '\"') {
        // end of string
        break;
      } else {
        // normal character
        alpha_barcode[j] = (uint8_t)ch;
        j++;
        if (j >= sizeof(alpha_barcode)) {
          // buffer overflow, stop processing
          break;
        }
      }
    } // end of for loop over the string

    // the string is processed now, we have j bytes in alpha_barcode
    // set the length in byte 1, but maximum 14 characters
    if (j - 2 > 14) {
      j = 16; // set to maximum length, what is left is truncated
    } 
    alpha_barcode[1] |= (j - 2) & 0x0F; // set length
    // now calculate the checksum
    checksum = 0;
    for (int k = 1; k < j; k++) {
      checksum += alpha_barcode[k];
      if (checksum > 0xFF) {
        checksum = (checksum & 0xFF) + 0x01; // end-around carry
      }

    }
    alpha_barcode[0] = checksum & 0xFF; // set checksum
    // now we have the complete barcode in alpha_barcode with j bytes
    // send the barcode to the wand buffer
    #ifdef DEBUG
      cli_printfn("  WAND ALPHA barcode to be scanned (%d bytes) ", j);
      for (int k = 0; k < j; k++) {
        cli_printfn(" %02X", alpha_barcode[k]);
      }
      cli_printf(""); 
    #endif

    for (int k = 0; k < j; k++) {
      barcode = alpha_barcode[k];
      queue_add_blocking(&WandBuffer, &barcode);  
    }

    setflag_FI0();
    wakemeup_41();
    return true; // exit the function


  }  // end of string handling

  // handle instructions starting with a single quote
  // sends a single ALPHA character as a keystroke
  if (instruction[0] == '\'') {
    if (len == 1) {
      // only a single quote, send the ALPHA key
      alpha_barcode[0] = 0x14; // ALPHA keycode
      alpha_barcode[1] = 0x0C; // ALPHA keycode

      // send the barcode to the wand buffer
      #ifdef DEBUG
        cli_printfn("  WAND ALPHA single ALPHA key barcode to be scanned ");
        cli_printf(" %02X %02X", alpha_barcode[0], alpha_barcode[1]);
      #endif
      // must wait until the previous barcode is processed
      while (gpio_get(P_PWO) == 1) {
        // wand is busy
        tud_task();  // keep the USB port alive
        if (cdc_available(ITF_CONSOLE)) {
          char ch = cdc_read_char(ITF_CONSOLE);
          cli_printf("  WAND instruction processing aborted by user");
          return false;
        }
        sleep_ms(20);
      }
      barcode = alpha_barcode[0];
      queue_add_blocking(&WandBuffer, &barcode);
      barcode = alpha_barcode[1];
      queue_add_blocking(&WandBuffer, &barcode);
      setflag_FI0();
      wakemeup_41();
      return true; // exit the function
    }
    for (size_t i = 1; i < len; i++) {
      char ch = instruction[i];
      if (ch == '\\') {
        // next character is escaped
        // if it is a hex digit we read the next two characters as hex value
        i++;
        if (i >= len) {
          // invalid escape sequence, ignore
          continue;
        }

        int idx = 0;      // index to alpha char array
        
        // handle escape characters
        switch (instruction [i]) {
          case '\\':  // send backslash
                      idx = 0x5C;
                      break;           
          case 'S':   // send sigma
                      idx = 0x7E;      
                      break;        
          case '+':   // this is append
                      idx = 0x7F;
                      break;
          case 't':   // hanging t
                      idx = 0x60;
                      break;
          case '#':   // not equal symbol
                      idx = 0x1D;
                      break;
          case 's':   // space
                      idx = 0x20;
                      break;
          case 'u':   // greek mu
                      idx = 0x0C; 
                      break;
          case 'a':   // angle symbol
                      idx = 0x0D;
                      break;
          case '\'':  // single quote
                      idx = 0x27;
                      break;
          case '\"':  // double quote
                      idx = 0x22;
                      break;  
          default:    // process hex digits
                      // read two hex digits
                      if (isxdigit((unsigned char)instruction[i])) {
                        char ch1 = instruction[i];
                        i++; 
                        if (i >= len) {
                          // invalid escape sequence, ignore
                          break;
                        }
                        char ch2 = instruction[i];
                        if (isxdigit((unsigned char)ch2)) {
                          // convert to byte
                          hexstr[0] = ch1;
                          hexstr[1] = ch2;
                          idx = (uint8_t)strtol(hexstr, NULL, 16);
                        } else {
                          // invalid escape sequence, ignore
                          break;
                        }
                      } else {
                        // invalid escape sequence, ignore
                        break;
                      }                      
        }

        #ifdef DEBUG
          cli_printf("  WAND ALPHA escaped character to be scanned: index %02X, code %02X %02X", idx, wand_alpha_[idx].hex1, wand_alpha_[idx].hex2);
        #endif

        alpha_barcode[0] = wand_alpha_[idx].hex1;
        alpha_barcode[1] = wand_alpha_[idx].hex2;

      } else if (ch == ' ') {
        // end of string reached when there is a space, get out
        break;
      } else {
        // normal character, get the index from the wand_alpha_ array
        #ifdef DEBUG
          cli_printf("  WAND ALPHA normal character to be scanned: character %c, code %02X %02X", ch, wand_alpha_[(uint8_t)ch].hex1, wand_alpha_[(uint8_t)ch].hex2);
        #endif

        // normal character, send as ALPHA keystroke
        // build the barcode for a single ALPHA character

        // catch the single quote at the end of the string
        if (ch == '\'') {
          // send ALPHA key
          alpha_barcode[0] = 0x14; // ALPHA keycode
          alpha_barcode[1] = 0x0C; // ALPHA keycode
        } else {
          // normal character
          alpha_barcode[0] = wand_alpha_[ch].hex1;
          alpha_barcode[1] = wand_alpha_[ch].hex2;
        }
      }

      // send the barcode to the wand buffer
      #ifdef DEBUG
        cli_printfn("  WAND ALPHA single character barcode to be scanned ");
        cli_printf(" %02X %02X", alpha_barcode[0], alpha_barcode[1]);
      #endif

      // must wait until the previous barcode is processed
      while (gpio_get(P_PWO) == 1) {
        // wand is busy
        tud_task();  // keep the USB port alive
        if (cdc_available(ITF_CONSOLE)) {
          char ch = cdc_read_char(ITF_CONSOLE);
          cli_printf("  WAND instruction processing aborted by user");
          return false;
        }
        sleep_ms(20);
      }

      barcode = alpha_barcode[0];
      queue_add_blocking(&WandBuffer, &barcode);  
      barcode = alpha_barcode[1];
      queue_add_blocking(&WandBuffer, &barcode);

      setflag_FI0();
      wakemeup_41();
    }

    return true; // exit the function


  }

  // now check if the instruction is one that is stored in the wand_cmd_ array
  // convert the string to upper case for comparison
  char instr_uc[32];
  strncpy(instr_uc, instruction, sizeof(instr_uc));
  for (int i = 0; instr_uc[i]; i++) {
    instr_uc[i] = toupper((unsigned char)instr_uc[i]);
  }

  // get the number of elements in the wand_cmd_ array
  int wand_cmd_count = sizeof(wand_cmd_) / sizeof(wand_cmd_[0]);

  // find the given instruction in the Wand Command set wand_cmd_
  for (int i = 0; i < wand_cmd_count; i++) {
    if (strcmp(instr_uc, wand_cmd_[i].command) == 0) {
      // found the command, execute it

      #ifdef DEBUG
        cli_printf("  WAND command found: %s, type %d, barcode %02X %02X", wand_cmd_[i].command, wand_cmd_[i].type, wand_cmd_[i].hex1, wand_cmd_[i].hex2);
      #endif

      switch (wand_cmd_[i].type) {
        case 0: // type 0 means not supported
                cli_printf("  Instruction %s not supported", instruction);
                return false;
                break;
        case 1: // type 1 are the 2-byte programmable functions
                queue_add_blocking(&WandBuffer, &wand_cmd_[i].hex1);
                queue_add_blocking(&WandBuffer, &wand_cmd_[i].hex2);
                setflag_FI0();
                wakemeup_41();
                return true;
                break;
        case 2: // type 2 are the ALPHA character keys, not yet supported
                // cli_printf("  Instruction %s not supported", instruction);
                queue_add_blocking(&WandBuffer, &wand_cmd_[i].hex1);
                setflag_FI0();
                wakemeup_41();
                break;           
        case 3: // Indirect function IND, treat as type 1 for now
                queue_add_blocking(&WandBuffer, &wand_cmd_[i].hex1);
                queue_add_blocking(&WandBuffer, &wand_cmd_[i].hex2);
                setflag_FI0();
                wakemeup_41();
                return true;
                break;
        case 4: // type 4 are the 2-byte non-programmable functions
                queue_add_blocking(&WandBuffer, &wand_cmd_[i].hex1);
                queue_add_blocking(&WandBuffer, &wand_cmd_[i].hex2);
                setflag_FI0();
                wakemeup_41();
                return true;
                break;
        case 5: // type 5 are the XROM commands, not yet supported
                cli_printf("  Instruction %s not supported", instruction);
                return false;
                break;         
        case 6: // type 6 are the 1-byte keypad functions
                queue_add_blocking(&WandBuffer, &wand_cmd_[i].hex1);
                setflag_FI0();
                wakemeup_41();
                return true;
                break;
        default:
                // command not found or supported
                cli_printf("  Instruction %s not available", instruction);
                break;
                
      }  // switch  
    }
  }


  // The last option is a number to be sent as barcode
  // the number can be positive

  // we have the following possibilities:
  // instruction is a number that starts with a -, + or a digit -> this is a barcode to be scanned
  // this is a barcode to be scanned as single digit (type 6) instructions
  // the - is executed as CHS after the first digit
  // a + is ignored
  // the E is executed as EEX
  // there can be a - before the E
  // the number can start with an E followed by maximum 2 digits

  if (isdigit((unsigned char)instruction[0]) || instruction[0] == '-'  || instruction[0] == 'E' || instruction[0] == 'e' || instruction[0] == '.') {

    // we now have correct digit entry
    // len is the number of characters to process
    size_t i = 0;
    while (i < len) {
      digit_decoded = false;
      // char ch = instruction[i];
      switch (instruction[i]) {
        case '0':  barcode = 0x00;  digit_decoded = true; break;    // send 0
        case '1':  barcode = 0x81;  digit_decoded = true; break;    // send 1
        case '2':  barcode = 0x42;  digit_decoded = true; break;    // send 2
        case '3':  barcode = 0xC3;  digit_decoded = true; break;    // send 3 
        case '4':  barcode = 0x24;  digit_decoded = true; break;    // send 4
        case '5':  barcode = 0xA5;  digit_decoded = true; break;    // send 5
        case '6':  barcode = 0x66;  digit_decoded = true; break;    // send 6
        case '7':  barcode = 0xE7;  digit_decoded = true; break;    // send 7
        case '8':  barcode = 0x18;  digit_decoded = true; break;    // send 8
        case '9':  barcode = 0x99;  digit_decoded = true; break;    // send 9
        case '.':  barcode = 0x5A;  digit_decoded = true; break;    // send decimal point
        case 'E':  barcode = 0xDB;  digit_decoded = true; break;    // send EEX
        case 'e':  barcode = 0xDB;  digit_decoded = true; break;    // send EEX
        case '-': // CHS, but can be applied only after the next digit is sent
                  CHS_used = true;
                  i++;
                  continue;   // next character
        default:  break;
      }

      #ifdef DEBUG
        cli_printf("  WAND DIGIT barcode to be scanned: %02X", barcode);  
      #endif

      if (digit_decoded) {
        // send digit
        while (gpio_get(P_PWO) == 1) {
          // wand is busy
          tud_task();  // keep the USB port alive
          if (cdc_available(ITF_CONSOLE)) {
            char ch = cdc_read_char(ITF_CONSOLE);
            cli_printf("  WAND instruction processing aborted by user");
            return false;
          }
          sleep_ms(20);
        }
        queue_add_blocking(&WandBuffer, &barcode);
        setflag_FI0();
        wakemeup_41();
      }

      if (digit_decoded && CHS_used) {
        // there was a previous CHS, a dgit has been decoded, so send CHS now
        barcode = 0x3C; // CHS code
        CHS_used = false;

        #ifdef DEBUG
          cli_printf("  WAND CHS barcode to be scanned: %02X", barcode);
        #endif

        while (gpio_get(P_PWO) == 1) {
          tud_task();  // keep the USB port alive
          if (cdc_available(ITF_CONSOLE)) {
            char ch = cdc_read_char(ITF_CONSOLE);
            cli_printf("  WAND instruction processing aborted by user");
            return false;
          }
          sleep_ms(20);
        }
        queue_add_blocking(&WandBuffer, &barcode);
        setflag_FI0();
        wakemeup_41();

      }

      digit_decoded = false;
      i++;
    } // end of while over the characters


  } else {
    // invalid instruction
    // cli_printf("  invalid WAND instruction: %s", instruction);
    return false;
  }

  return true;
}

void uif_w(const char *instruction) 
{

  uint8_t barcode; 
  char instr_copy[128];
  char current_instr[64];

  // only do this when PWO is low
  // if (!uif_pwo_low()) return;

  #ifdef DEBUG
    cli_printf("  W command invoked: %s", instruction);
  #endif

  if (!globsetting.get(HP82153A_enabled)) {
    cli_printf("  WAND EMULATION not enabled, plug the WAND ROM and enable WAND emulation");
    return;
  }


  // the string may contain various elements as it is not tokenized
  // in the main loop we extract the tokens one by one and process them until the string is empty
  // if there is a token enclosed with quotes the quotes are passed with the token!

  // first copy the instruction to a local buffer
  strncpy(instr_copy, instruction, sizeof(instr_copy));
  instr_copy[sizeof(instr_copy) - 1] = '\0';  // ensure null termination
  
  // now process the string until it is empty
  char *ptr = instr_copy;
  while (*ptr != '\0') {
    // skip leading spaces
    while (*ptr == ' ') ptr++;
    // extract the next token into current_instr
    int i = 0;
    if (*ptr == '\"') {
      // token starts with a quote, copy until the closing quote
      current_instr[i++] = *ptr++; // copy the opening quote
      while (*ptr != '\0' && *ptr != '\"' && i < sizeof(current_instr) - 1) {
        current_instr[i++] = *ptr++;
      }
      if (*ptr == '\"') {
        current_instr[i++] = *ptr++; // copy the closing quote
      }
    } else {
      // token does not start with a quote, copy until the next space
      while (*ptr != '\0' && *ptr != ' ' && i < sizeof(current_instr) - 1) {
        current_instr[i++] = *ptr++;
      }
    }

    current_instr[i] = '\0'; // null terminate the current instruction
    #ifdef DEBUG
      cli_printf("  extracted WAND instruction: %s", current_instr);
    #endif
    
    // process the current instruction
    // wait until the wand buffer is empty, cancel on a key press
    cli_printf("  processing WAND instruction: %s", current_instr);   

    while (gpio_get(P_PWO) == 1) {
      tud_task();  // keep the USB port alive
      if (cdc_available(ITF_CONSOLE)) {
        char ch = cdc_read_char(ITF_CONSOLE);
        cli_printf("  WAND instruction processing aborted by user");
        return;
      }
      sleep_ms(20);
    }

    if (!process_wand_instruction(current_instr)) {
      cli_printf("  error processing WAND instruction: %s", current_instr);
      break;
    }

  } 

}


/*
#define wand_status    1
#define wand_test      2
#define wand_scan      3
#define wand_list      4

#define WAND_HELP_TXT "wand functions\r\n\
        [no argument]     shows the wand status\r\n\
        status            shows the wand status\r\n\
        scan [filename]   scan the named file \r\n\
        list              activates the WAND Paper Keyboard\r\n\
        test              performs a wand test sequence, use with WNDTST\r\n"
*/
void uif_wand(int i, const char* fname, char* instruction)
{
  // only do this when PWO is low
  // if (!uif_pwo_low()) return;

  uint16_t barcode; 

  #ifdef DEBUG
    cli_printf("  WAND command invoked %d", i);
  #endif

  if (!globsetting.get(HP82153A_enabled)) {
    cli_printf("  WAND EMULATION not enabled, plug the WAND ROM and enable WAND emulation");
    return;
  }

  switch (i) {
    case 1: // status
            cli_printfn("  HP82153A Wand emulation %s", globsetting.get(HP82153A_enabled) ? "enabled":"disabled");
            cli_printf(", check with cat if the WAND ROM is plugged");
            break;
    case 2: // test
            cli_printf("  WAND test: send BEEP to the HP41");
            barcode = 0xE0; queue_try_add(&WandBuffer, &barcode);
            barcode = 0x86; queue_try_add(&WandBuffer, &barcode);
            setflag_FI0();
            wakemeup_41();
            break;
    case 3: // scan      
            scan_file(fname);
            break;           
    case 4: // list
            // list the functions supported by the Paper Keyboard
            cli_printf("  WAND Paper Keyboard instructions");
            for (int j = 0; j < sizeof(wand_cmd_) / sizeof(wand_cmd_[0]); j++) {
              cli_printfn("    %-15s  type %2d  code %02X %02X", wand_cmd_[j].command, wand_cmd_[j].type, wand_cmd_[j].hex1, wand_cmd_[j].hex2);
              switch (wand_cmd_[j].type) {
                case 0: // not supported
                        cli_printf("      not supported");
                        break;
                case 1: // 2-byte programmable functions
                        cli_printf("      2-byte programmable function");
                        break;
                case 2: // 2-byte programmable functions
                        cli_printf("      ALPHA character keys");

                        break;
                case 3: // indirect function
                        cli_printf("      indirect function");
                        break;
                case 4: // 2-byte non-programmable functions
                        cli_printf("      2-byte non-programmable function");
                        break;
                case 5: // XROM functions
                        cli_printf("      XROM function, not supported");
                        break;
                case 6: // 1-byte keypad functions
                        cli_printf("      1-byte keypad function");
                        break;
                default:
                        // unknown type ?? should never get here
                        cli_printf("      unknown function type");
                        break;
              }
            }
            break;
    case 5: // send direct WAND instructions from command line
            if (instruction == NULL || *instruction == '\0') {
              cli_printf("  nothing to send ...");
              break;
            }
            while (instruction != NULL && *instruction != '\0') {
              // tokenize the string instruction by spaces
              // the string consists of a series of integers separated by spaces
              char *token = strtok((char*)instruction, " ");
              uint8_t value;
              if (token == NULL) {
                break; // no more tokens
              }
                          
              // convert the token to a number
              value = (uint8_t)strtol(token, NULL, 0);
              cli_printf("  sending WAND instruction byte: 0x%02X   decimal %d", value, value);
              queue_add_blocking(&WandBuffer, &value);

              // move to the next token
              instruction += strlen(token) + 1;
            }
            setflag_FI0();
            wakemeup_41();
            break;
    default: break;
    
  }  // switch (i)

}


//************************************************************ */
// Below are the functions for the 'old' user interface

void welcome()
{
    printf("\n****************************************************************************");
    printf("\n*   Welcome to TULIP4041 - The ULtimate Intelligent Peripheral for the HP41 ");
    printf("\n*   Total heap:  %d bytes", getTotalHeap());
    printf("\n*   Free heap:   %d bytes", getFreeHeap());
    printf("\n*   running at:  %d kHz\n", clock_get_hz(clk_sys)/1000);
    printf("\n****************************************************************************\n");
    measure_freqs();
}

void uif_welcome()
{
  cli_printf("test message");
}


void pio_welcome()
{
    pio_report();
}


void toggle_trace(void)
{
  trace_enabled = !trace_enabled;
  printf("Trace is %s\n", trace_enabled ? "on":"off");
}

void toggle_traceo(void)
{
  trace_outside = !trace_outside;
  printf("system ROM trace is %s\n", trace_outside ? "on":"off");
}

// Toggle disassembler (on -> call disassembler if trace is on)
void toggle_disasm(void)
{
  trace_disassembler = ! trace_disassembler;
  printf("Disassembler is %s\n", trace_disassembler ? "on":"off");
}

void set_break(void)
{
  sample_break = sample_break + 5000;
  if (sample_break > 100000) sample_break = 0;
  printf("Breakpoint set to %d traces\n", sample_break);
}


void print_f()
{
  // initiate the printer sub functions. print p and a prompt
  printf("\np_\b"); 
  state = ST_PRT;
}


void print_busy()
{
  // toggle printer BUSY
  SELP9_status_BUSY = !SELP9_status_BUSY; 
  printf("b - printer BUSY status is %s\n", SELP9_status_BUSY ? "on":"off");
}

void print_idle()
{
  // toggle printer BUSY
  SELP9_status ^= prt_IDL_mask;   // XOR of Idle status bit 
  printf("i - printer IDLE status is %s\n", SELP9_status & prt_IDL_mask ? "on":"off");
}

void print_empty()
{
  // toggle printer Buffer Empty
  SELP9_status ^= prt_BE_mask;   // XOR of Idle status bit
  printf("b - printer Buffer Empty status is %s\n", SELP9_status & prt_BE_mask ? "on":"off");
}

void print_man()
{
  // set printer to MAN status, bit 14 and 15 of printer status are 0
  SELP9_status = SELP9_status & ~prt_SMA_mask & ~prt_SMB_mask;
  printf("m - printer set to MAN mode\n");
}

void print_trace()
{
  // set printer to TRACE status, bit 15 set, bit 14 clear
  SELP9_status = SELP9_status | prt_SMA_mask & ~prt_SMB_mask;
  printf("t - printer set to TRACE mode\n");
}

void print_norm()
{
  // set printer to NORM mode, bit 15 clear, bit 14 set
  SELP9_status = (SELP9_status & ~prt_SMA_mask) | prt_SMB_mask;
  printf("n - printer set to NORM mode\n");
}

void print_adv()
{
  // push the printer ADV button, short push only, button status is reset after 1 read
  // this is printer status but 12
  SELP9_status = SELP9_status | prt_ADV_mask;
  keycount_print = 2;
  wakemeup_41();
  printf("a - printer ADV key pushed\n");
}

void print_print()
{  
  // push the printer PRINT button, short push only, button status is reset after 1 read
  // this is printer status but 13
  SELP9_status = SELP9_status | prt_PRT_mask;
  keycount_print = 2;
  wakemeup_41();
  printf("p - printer PRINT key pushed\n");
}

void print_bat()
{
  // toggle the LOW bat status in the printer status word
  // this does not seem to be used anywhere in the printer ROM
  SELP9_status ^= prt_LB_mask;   // XOR of Low Battery status bit
  printf("l - printer Low Battery status %s\n", SELP9_status & prt_LB_mask ? "on":"off");
}

void print_paper()
{
  // toggle the Out Of Paper status in the printer status word
  SELP9_status ^= prt_OOP_mask;   // XOR of Out Of Paper status bit
  printf("o - printer Out Of Paper status %s\n", SELP9_status & prt_OOP_mask ? "on":"off");
}




void rom_f()
// ROM subcomands prompt
{
  printf("\nr_\b");
  state = ST_ROM;
}



/*

int rom_checksum()
{
  // returns the HP checksum for the current ROM
  int accumulator = 0;

  for (int i = 0; i < 0x0FFF; i++) {
    accumulator = accumulator + rom_buf[i];
    if (accumulator > 0x03FF) {
      accumulator = (accumulator & 0x03FF) + 1;
    }
  }

  accumulator = (-accumulator) & 0x03FF;

  return accumulator;

  /* Accumulator := 0;
  for i := 0 to Rom4KSize - 1 do begin
    Accumulator := Accumulator + Swap(MODArray[i]);
    if Accumulator > $03FF then begin
      Accumulator := (Accumulator and $03FF) + 1;
    end;
  end;
  Accumulator := (-Accumulator) and $03FF;
  FrmROMHandler.LblCHK.Caption := Hex3(Swap(MODArray[CHECKSUM_Addr]));
  FrmROMHandler.LblCALCCHK.Caption := Hex3(Accumulator);
*/



void rom_digit(int c)
{
  printf("%c - digit entered for setting ROM target\n", c);
  if ((c >= '0') && (c <='9')) {
    // valid digit key pressed
    rom_target = c - '0';
  }
  printf("ROM programming target Page = %d\n", rom_target);
}

void rom_mapping()
{
  printf("m - toggle default ROM mapping\n");
  default_map = !default_map;
  printf("ROM programming target Page = %d\n", rom_target);
  printf("ROM mapping default %s\n", default_map ? "on":"off");
}

void rom_off()
{
  printf("o - toggle ROM mapping enable\n");
  default_map_off = !default_map_off;
  printf("ROM programming target Page = %d\n", rom_target);
  printf("ROM mapping %s\n", default_map_off ? "enabled":"disabled");
}


void file_f()
{
  // 
  printf("\nf - press F to start download of ROM file");
  printf("\n pressing <ESC> will abort the download");
  state = ST_FILE;
}

void rp2040_reset()
{
  printf("\n RESETTING THE RP2040-TUP !!");
  sleep_ms(1000);
  watchdog_enable(1, 1);
}

void rp2040_bootsel()
{
  printf("\n RESETTING THE RP2040-TUP to BOOTSEL mode!!");
  sleep_ms(1000);

  // reboots the RP2040, uses the standard LED for activity monitoring
  reset_usb_boot(1<<PICO_DEFAULT_LED_PIN, 0) ;

}


void rp2040_sleep()
{

  printf("NOT IMPLEMENTED\n");
  // sleep_ms(1000);
  // uart_default_tx_wait_blocking();

  // go to sleep, wake up on any key press
  // sleep_run_from_xosc();
  // sleep_goto_dormant_until_pin(P_PWO, true, true);

  // sleep_power_up();
  // printf("Now awake \n");
  // sleep_ms(1000 * 1);

}


void hp41_shutdown()
// command D: HP41 forced shutdown (drive PWO low for 20 us)
// only do this when no IR led is connected
{
  printf("D - forcing HP41 shutdown by driving PWO for 20 us\n");
  shutdown_41();
}

void pico_VBUS()
// command V: show VBUS status (USB power present)
// used VBUS GPIO, on Pico this is GPIO pin 24
{
  printf("V - Pico VBUS status, USB connected: %s\n", gpio_get(PICO_VBUS_PIN) ? "on":"off");
}



void print_status()
{
  // list the current printer status word
  printf("s\n");
  // printf("Printer is %s\n", SELP9_status_ON ? "on":"off");
  printf("Printer BUSY is %s\n", SELP9_status_BUSY ? "on":"off");
  printf("Status word %04X\n", SELP9_status);
  printf(" SMA  %01d - bit 15 - SMA, TRACE mode when set\n", (SELP9_status >> 15) & 0x0001);
  printf(" SMB  %01d - bit 14 - SMB, NORM when set, MAN when bit 14 and 15 are clear\n", (SELP9_status >> 14) & 0x0001);
  printf(" PRT  %01d - bit 13 - PRT, PRINT key down\n", (SELP9_status >> 13) & 0x0001);
  printf(" ADV  %01d - bit 12 - ADV, PAPER ADVANCE key down\n", (SELP9_status >> 12) & 0x0001);
  printf(" OOP  %01d - bit 11 - OOP, Out Of Paper\n", (SELP9_status >> 11) & 0x0001);
  printf(" LB   %01d - bit 10 - LB , Low BAttery\n", (SELP9_status >> 10) & 0x0001);
  printf(" IDL  %01d - bit  9 - IDL, Idle \n", (SELP9_status >> 9) & 0x0001);
  printf(" BE   %01d - bit  8 - BE , Buffer Empty\n", (SELP9_status >> 8) & 0x0001);
  printf(" LCA  %01d - bit  7 - LCA, Lower Case Alpha\n", (SELP9_status >> 7) & 0x0001);
  printf(" SCO  %01d - bit  6 - SCO, Special Column Output (Graphics mode)\n", (SELP9_status >> 6) & 0x0001);
  printf(" DWM  %01d - bit  5 - DWM, Double Wide Mode\n", (SELP9_status >> 5) & 0x0001);
  printf(" TEO  %01d - bit  4 - TEO, Type of End-Of-Line\n", (SELP9_status >> 4) & 0x0001);
  printf(" EOL  %01d - bit  3 - EOL, Last End-Of-Line\n", (SELP9_status >> 3) & 0x0001);
  printf(" HLD  %01d - bit  2 - HLD, Hold for Paper\n", (SELP9_status >> 2) & 0x0001);
  printf(" bt1  %01d - bit  1 - not used, always returns 0\n", (SELP9_status >> 1) & 0x0001);
  printf(" bt0  %01d - bit  0 - not used, always returns 0\n", (SELP9_status >> 0) & 0x0001);

}




void wandcode(int i)
// send Wand barcode for testing
{
    uint16_t barcode;
    printf("\nW_\b");
    state = ST_WAND;

    switch (i)
    {
    case 0:       // send digit entry '0'
      barcode = 0; queue_try_add(&WandBuffer, &barcode);
      barcode = 0; queue_try_add(&WandBuffer, &barcode);
      setflag_FI0();
      wakemeup_41();
      break;

    case 1:       // send BEEP
      barcode = 0xE0; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x86; queue_try_add(&WandBuffer, &barcode);
      setflag_FI0();
      wakemeup_41();
      break;

    case 2:       // enter a small program
      barcode = 0xB6; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x10; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x01; queue_try_add(&WandBuffer, &barcode);
      barcode = 0xC6; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x00; queue_try_add(&WandBuffer, &barcode);
      barcode = 0xF2; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x00; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x5A; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x91; queue_try_add(&WandBuffer, &barcode);
      setflag_FI0();
      wakemeup_41();
      break;

    case 3:
      barcode = 0x3E; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x11; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x10; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x75; queue_try_add(&WandBuffer, &barcode);
      barcode = 0xC0; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x01; queue_try_add(&WandBuffer, &barcode);
      barcode = 0x2F; queue_try_add(&WandBuffer, &barcode);
      setflag_FI0();
      wakemeup_41();
      break;
  
    default:        // just pressing button, nothing else
      setflag_FI0();
      wakemeup_41();
      break;
    }

}

// identify CDC serial ports by sending a string  
void ident_cdc()
{
    // for testing 
    int i;
    int PrintLen;
    char  PrintStr[200];

    printf("\n");

    for (i = 0; i < 5; i++)
    {
        if (cdc_connected(i))
        {
          printf("  CDC Port %d OK  connected: %s\n", i, ITF_str[i]);

            PrintLen = 0;
            PrintLen += sprintf(PrintStr + PrintLen,"\n*** Testing CDC Port %i: %s ***\n\r", i, ITF_str[i]);
            cdc_send_string(i, PrintStr, PrintLen);
            cdc_flush(i);
        }
        else
        {
          printf("  CDC Port %d NOT connected: %s \n", i, ITF_str[i]);
        }
    }
}

void sdcard_test()
{
  sd_status();
}

void test_fram()
{
  uint8_t buf[128];
  printf("FRAM read test\n");

  // read 64 bytes from FRAM address 0
  fr_read(0, buf, 64);

  printf("FRAM data at address 0:\n");
  for (int i = 0; i < 64; i++) {
    printf("%02X ", buf[i]);
    if ((i & 0x0F) == 0x0F) printf("\n");
  }
}

// supported commands for the user interface, main level
SERIAL_COMMAND serial_cmds[] = {
  { 'h', serial_help,     "Serial command help"  },
  { '?', serial_help,     "Serial command help"  },
  { 'P', power_on,        "Power On - drive ISA for 20 us"  },
  { 'p', print_f,         "Printer functions subcommands"},
  { 'f', test_fram,       "FRAM test"},
  { 'w', welcome,         "show welcome message"  },
  { 'i', pio_welcome,     "show PIO status"},
  // { 'W', wandcode,      "send one of the barcode test commands"},
  { 'I', ident_cdc,       "Identify CDC serial ports"},
  { 'R', rp2040_reset,    "CAUTION: full reset of the RP2040-TUP"},
  { 'B', rp2040_bootsel,  "CAUTION: reboots the RP2040 in BOOTSEL mode"},
  { 'S', rp2040_sleep,    "CAUTION: RP2040 goes dormant"},
  { 'D', hp41_shutdown,   "CAUTION: HP41 forced shutdown (drive PWO low for 20 us)"},
  { 'V', pico_VBUS,       "show VBUS status (USB power present)"},

};

TRACE_COMMAND trace_cmds[] = {
  { 'h', serial_help,   "Serial command help"  },

};

// supported commands for the user interface, printer sub-level
PRINT_COMMAND print_commands[] = {
  { 'h', print_help,    "Printer commands help" },
  { '?', print_help,    "Printer commands help" },  

};

// supported subcommands for the user interface, ROM sub-level
ROM_COMMAND rom_commands[] = {

};

const int helpSize = sizeof(serial_cmds) / sizeof(SERIAL_COMMAND);
const int phelpSize = sizeof(print_commands) / sizeof(PRINT_COMMAND);
const int rhelpSize = sizeof(rom_commands) / sizeof(ROM_COMMAND);

void serial_help(void)
{
  printf("h - help\n");
  printf("\nCmd | Description");
  printf("\n----+-----------------");
  for (int i = 0; i < helpSize; i++)
  {
    printf("\n  %c | %s", serial_cmds[i].key, serial_cmds[i].desc);
  }
  printf("\n");
}

void print_help(void)
{
  printf("h - help for print subfunctions\n");
  printf("\nCmd | Description of Printer subcommands");
  printf("\n----+-----------------------------------");
  for (int i = 0; i < phelpSize; i++) {
    printf("\n  %c | %s", print_commands[i].key, print_commands[i].desc);
  }
  printf("\n");
}

void rom_help()
// ROM commands help
{
  printf("h - help for ROM subfunctions\n");
  printf("Cmd | Description of ROM subcommands");
  printf("\n----+-----------------------------------");
  for (int i = 0; i < rhelpSize; i++) {
    printf("\n  %c | %s", rom_commands[i].key, rom_commands[i].desc);
  }
  printf("\n\nIMPORTANT: HP41 MUST BE OFF OR IN LIGHT SLEEP WHEN USING ROM COMMANDS\n");
}


int getHexKey(int ky)
{
  int x = -1;
  if( ky >= '0' && ky <= '9' )
    x = ky - '0';
  else if( ky >= 'a' && ky <= 'f' ) 
    x = ky - ('a'-0xa);
  else if( ky >= 'A' && ky <= 'F' )
    x = ky - ('A'-0xa);
  return x; // Value of hex or -1
}


void serial_loop(void)
{
  int key = getchar_timeout_us(0);

  if( key != PICO_ERROR_TIMEOUT ) {
    if (state != ST_NONE) {
      // some kind of input expected
      if( key == 0x1b ) { // ESC - abort any key sequence ...
        state = ST_NONE;      // reset input state
        printf("Abort\n");
      } else 
      {
        switch(state) {
          case ST_PRT:          // printer functions
            for (int i = 0; i < phelpSize; i++) {
              if (print_commands[i].key == key) {
                (*print_commands[i].fn)();
                state = ST_NONE;
              }   
            }     
            break;
          case ST_WAND:
            if ((key >= '0') && (key < '9')) {
              // and  to list the XMEM contents
              wandcode(key - 48);
              state = ST_NONE;
            }
            break;
          case ST_FILE:
            if (key == 'F') {
              // file_download();
              state = ST_NONE;
            }
          case ST_ROM:      // ROM functions
            if ((key >= '0') && (key <='9')) {
            // digit key pressed
            rom_digit(key);
            state = ST_NONE;
            }
            else
            {
              for (int i = 0; i < rhelpSize; i++) {
                if (rom_commands[i].key == key) {
                  (*rom_commands[i].fn)();
                  state = ST_NONE;
                }
              }
            }
            break;
          default:
            break;
        }
      }
    }
    
    else {
      for (int i = 0; i < helpSize; i++) {
        if (serial_cmds[i].key == key) {
          (*serial_cmds[i].fn)();
          break;
        }
      }
    }
  }
  // else
  // {
    // I have found that I need to send something if the serial USB times out
    // otherwise I get lockups on the serial communications.
    // So, if we get a timeout we send a space and backspace it. And
    // flush the stdio, but that didn't fix the problem but seems like a good idea.
  //   stdio_flush();
  //    printf(" \b");
 //  }
}
