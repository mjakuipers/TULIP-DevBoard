/*
 * globalsettings.h
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


#ifndef __GLOBALSETTINGS_H__
#define __GLOBALSETTINGS_H__

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "pico.h"
#include "pico/stdlib.h"

#include "hpinterface_hardware.h"
#include "fram.h"
#include "emulation.h"

// settings below are used as an index in an array of words (uint16)
//      0       = false/no
//      !0      = true/yes
//      [value] = value
// for easy store and retrieve global settings in FRAM

// the first block of parameters enable or disable decoding of the perphipher instructions
// for that specific device. These must be set together with loading the ROM
#define     HP82143A_enabled     0          // HP82143A printer active and SELP9 decoded
#define     HP82153A_enabled     1          // HP82153A Wand active
#define     HP82160A_enabled     2          // HP82160A HP-IL module active
#define     HP82242A_enabled     3          // HP82242A Blinky IR printer module active
#define     HP82104A_enabled     4          // HP82104A Cardreader active
#define     HP82182A_enabled     5          // HP82104A Time Module active
#define     HP41CLem_enabled     6          // HP41CL emulation active (limited function set)
#define     HEPAX_enabled        7          // HEPAX module native support  
#define     QROM_enabled         8          // WROM instruction decoded
#define     ROM_enabled          9          // ROM reading enabled
#define     BKS_enabled         10          // bankswitch instructions enabled  
#define     Expanded_enabled    11          // Expandend Memory enabled (MAXX emulation)
#define     Tiny41_enabled      12          // enable decoding of TT specific instructions for device control

// used for the embedded HP-Il and ILPrinter ROMS in the BETA version
#define     HPIL_plugged        15          // HP-IL module plugged in Page 7
#define     ILPRINTER_plugged   16          // IL-Printer module plugged in Page 6
#define     PRINTER_plugged     17          // HP82143A Printer module plugged

// control of TULIP4041 outputs to the HP41 system bus
#define     DATA_drive_enabled  20          // enabled driving of DATA output
#define     ISA_drive_enabled   21          // enable driving of ISA (to switch ROM emulation on/off)
#define     PWO_drive_enabled   22          // enable driving of PWO line for reset of the HP41
#define     FI_drive_enabled    23          // driving of FI is enabled
#define     IR_drive_enabled    24          // driving the IR led is enabled

// control of tracer settings
#define     tracer_enabled      30          // tracer enabled
#define     tracer_ilregs_on    31          // tracing of HP-IL registers enabled
#define     tracer_dis_enabled  32          // disassembly enabled
#define     tracer_FI_on        33          // FI tracing enabled
#define     tracer_dis_type     34          // tracer disassembly type (JDA only for now)
                                            // 0 = JDA
                                            // 1 = HP   (not yet supported)
                                            // 2 = ZEN  (not yet supported)
#define     tracer_sysrom_on    35          // tracing of system ROMS pages 0, 1, 2, 3, 5 enabled
#define     tracer_userrom_on   36          // tracing of user ROM pages 8..F enabled
#define     tracer_p4_on        37          // tracing of page 4 enabled
#define     tracer_p6_on        38          // tracing of page 6 enabled
#define     tracer_p7_on        39          // tracing of page 7 enabled
#define     tracer_sysloop_on   40          // tracing of system loops enabled:
                        	                //      0x0098 - 0x00A1       RSTKB and RST05
        	                                //      0x0177 - 0x0178       delay for debounce
        	                                //      0x089C - 0x089D       BLINK01
#define     tracer_ilroms_on    41          // tracing of IL ROMs enabled, Page 6+7

// HP-IL scope settings
#define     ilscope_IL_enabled  51          // PILBox tracing enabled
#define     ilscope_PIL_enabled 52          // PILBox tracing enabled
#define     ilscope_traceIDY    53          // Enabel IDY tracing in the HPIL Scope

// XMEM/User memory settings
#define     xmem_pages          60          // number of XMEM pages (0, 1, 2)
#define     xfun_enabled        61          // XFunction memory enabled
#define     mmod1_enabled       62          // Memory Module #1 enabled
#define     mmod2_enabled       63          // Memory Module #2 enabled
#define     mmod3_enabled       63          // Memory Module #3 enabled
#define     mmod4_enabled       64          // Memory Module #4 enabled
#define     mmod_quad_enabled   65          // Quad Memory Module enabled

// control of various messages
#define     PRT_monitor_enabled 70          // monitor printer characters to console
#define     PWR_monitor_enabled 71          // monitor power mode changes to console

// settings fpr HP82143 printer
#define     PRT_mode            80          // NORM/MAN/TRACE, matches bits in SMA and SMB field (bit 15 + 16)
                                            //      0 - MAN mode
                                            //      1 - NORM
                                            //      2 - TRACE mode
                                            //      3 - TRACE mode (not used)  
#define     PRT_delay           81          // for IR printing
#define     PRT_paper           82          // is paper loaded
#define     PRT_power           83          // printer power
#define     PRT_serial          84          // use serial printer output for output in terminal emulator                   

// control of TULIP4041 device
#define     CLI_on_USB          90          // CLI goes to USB CDC port, otherwise to serial port
#define     PWO_event_show      91          // show PWO events in the CLI
#define     gset_init_ok        92          // are the global settings initialized? 
                                            // used to set the defaults the first time
                                            // settings marked as initialized when this field = 41
#define     gset_num            93          // to keep track of active set of GSettings
#define     gsettings_lastitem  99          // to get the total number of items in the array

#define     init_value          0x4041

class GSettings {

    public:

    uint16_t gsettings[gsettings_lastitem];         // array where items are stored

    GSettings() {
        // initialize to default settings

        // first set all items to 0, all false (disabled)
        for (int i = 0; i < gsettings_lastitem; i++) {
            gsettings[i] = 0;
        }
    }

    // set items to the default value and save in fram
    int set_default() {
        // now set the items that should be enabled by default 
        // first set all items to 0, all false (disabled)
        for (int i = 0; i < gsettings_lastitem; i++) {
            gsettings[i] = 0;
        }

        gsettings[gset_init_ok]         = init_value;         // value when initialized
        gsettings[HPIL_plugged]         = 0;
        gsettings[ILPRINTER_plugged]    = 0;
        gsettings[PRINTER_plugged]      = 0;
        gsettings[HP82143A_enabled]     = 0;
        gsettings[HP82160A_enabled]     = 0;

        // settings for HP82143A printer
        gsettings[PRT_mode]             = 0;        // NORM mode
        gsettings[PRT_delay]            = 0;        // no delay for IR (not used yet)
        gsettings[PRT_paper]            = 0;        // no paper loaded in the shipping config
        gsettings[PRT_power]            = 0;        // power off default
        gsettings[PRT_serial]           = 0;        // no serial translation (not used yet)

        // settings for Extended Memory
        gsettings[xmem_pages]           = 0;        // no XMEM plugged

        //settings for tracer, enable by default
        gsettings[tracer_enabled]       = 1;
        gsettings[tracer_ilregs_on]     = 1;
        gsettings[tracer_sysrom_on]     = 1;
        gsettings[tracer_sysloop_on]    = 1;
        gsettings[tracer_ilroms_on]     = 1;

        if (gpio_get(P_PWO) == 0) {
            // when PWO = low we can write to FRAM
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_gsettings_start, (uint8_t*)gsettings, sizeof(gsettings));
            return 1;
        } else {
            // PWO was high, calculator is running and cannot write to FRAM
            return 0;
        }
    }

    bool is_initialized() {
        return (gsettings[gset_init_ok] == init_value); 
    }

    // change a setting
    // used inline here to enable quick access from core1 loop
    inline void set(int idx, uint16_t value) {
        gsettings[idx] = value;
    }

    // retrieve a current setting
    // used inline here to enable quick access from core1 loop
    inline uint16_t get(int idx) {
        return gsettings[idx];
    }

    // save all settings in FRAM, can only be done when PWO is low!!
    // is done automatically when HP41 power goes down
    // returns 1 (true) if succesful
    int save() {
        if (gpio_get(P_PWO) == 0) {
            // when PWO = low we can write to FRAM
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_gsettings_start, (uint8_t*)gsettings, sizeof(gsettings));
            return 1;
        } else {
            // PWO was high, calculator is running and cannot write to FRAM
            return 0;
        }
    }

    // retrieve settings from FRAM in array for use, can only be done when PWO is low!!
    // is done automatically upon device power up
    // relevant settings will be applied to emulation variables
    // returns 1 (true) if succesful
    int retrieve() {
        if (gpio_get(P_PWO) == 0)  {
            // when PWO = low we can write to FRAM
            // fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_gsettings_start, gsettings, sizeof(gsettings));
            // when PWO = low we can read from FRAM
            fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_gsettings_start, (uint8_t*)gsettings, sizeof(gsettings));  



            return 1;
        } else {
            // PWO was high, calculator is running and cannot write to FRAM
            return 0;
        }
    }
}   

; // end of class GSettings

#endif      // __GLOBALSETTINGS_H__