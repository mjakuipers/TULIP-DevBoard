/*
 * tracer.h
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

#ifndef __TRACER_H__
#define __TRACER_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "pico.h"
#include "pico/stdlib.h"

// #include "disassembler.h"
#include "cdc_helper.h"
#include "emulation.h"
#include "userinterface.h"
#include "module.h"

// #include "peripherals.h"

// definition of the structure for the analyzer functions
// different variations to support dynamic sizing of the tracebuffer

// TLine  is for the maximum possible trace line structure with FI and HP-IL
// TODO: add line for current bank
struct TLine {
    uint32_t    cycle_number;       // to count the cycles since the last PWO       4 bytes
    uint16_t    isa_address;        // ISA address                                  2 bytes
    uint16_t    isa_instruction;    // ISA instruction with SYNC status             2 bytes
    uint32_t    data1;              // DATA D31..D00                                4 bytes
    uint32_t    data2;              // DATA D55..D32                                4 bytes
    uint32_t    fi1;                // for FI line tracing                          4 bytes
    uint32_t    fi2;                //                                              4 bytes
    uint16_t    xq_instr;           // instruction decoded                          2 bytes
    bool        xq_carry;           // when carry is sent                           1 byte
    uint32_t    ramslct;            // selected RAM chip                            4 bytes
    uint16_t    frame_in;           // HP-IL frame input                            2 bytes
    uint16_t    frame_out;          // HP-IL frame output                           2 bytes
    uint8_t     HPILregs[9];        // HP-IL registers                              9 bytes
} ;

// TLine  is for basic tracing with FI
struct TLine_FI {
    uint32_t    cycle_number;       // to count the cycles since the last PWO
    uint16_t    isa_address;        // ISA address
    uint16_t    isa_instruction;    // ISA instruction with SYNC status
    uint32_t    data1;              // DATA D31..D00
    uint32_t    data2;              // DATA D55..D32
    uint32_t    fi1;                // for FI line tracing
    uint32_t    fi2;     
    // uint16_t    xq_instr;           // instruction decoded              
    // bool        xq_carry;           // when carry is sent
    uint32_t    ramslct;            // selected RAM chip
    // uint16_t    frame_in;           // HP-IL frame input
    // uint16_t    frame_out;          // HP-IL frame output
    // uint8_t     HPILregs[9];        // HP-IL registers
} ;


// TLine basic is for the smallest possible trace line structure
struct TLine_basic {
    uint32_t    cycle_number;       // to count the cycles since the last PWO
    uint16_t    isa_address;        // ISA address
    uint16_t    isa_instruction;    // ISA instruction with SYNC status
    uint32_t    data1;              // DATA D31..D00
    uint32_t    data2;              // DATA D55..D32
    // uint32_t    fi1;                // for FI line tracing
    // uint32_t    fi2;   
    // uint16_t    xq_instr;           // instruction decoded                
    // bool        xq_carry;           // when carry is sent
    // uint32_t    ramslct;            // selected RAM chip
    // uint16_t    frame_in;           // HP-IL frame input
    // uint16_t    frame_out;          // HP-IL frame output
    // uint8_t     HPILregs[9];        // HP-IL registers
};

// TLine basic is for the basic trace with HP-IL registers
struct TLine_IL {
    uint32_t    cycle_number;       // to count the cycles since the last PWO
    uint16_t    isa_address;        // ISA address
    uint16_t    isa_instruction;    // ISA instruction with SYNC status
    uint32_t    data1;              // DATA D31..D00
    uint32_t    data2;              // DATA D55..D32
    // uint32_t    fi1;                // for FI line tracing
    // uint32_t    fi2;                
    // uint16_t    xq_instr;           // instruction decoded   
    // bool        xq_carry;           // when carry is sent
    uint32_t    ramslct;            // selected RAM chip
    uint16_t    frame_in;           // HP-IL frame input
    uint16_t    frame_out;          // HP-IL frame output
    uint8_t     HPILregs[9];        // HP-IL registers
};


extern const char *mnemonics[];

void TraceBuffer_init();
void Trace_task();

const int TRACELENGTH = 3000;       //Trace Buffer length

#define numILmnemonics      49      // number of elements in IL_mnemonics 0.. 48, PILBox commands now included

#define NUMFILTERS          16      // number of entries for filters

struct filter {
    uint16_t start_adr;             // start of the range to be filtered
    uint16_t end_adr;               // end of the range to be filtered
    uint16_t count_adr;             // counter in case of a trigger with sample counter
    uint8_t bank;                   // not yet supported
                                    // 0    - any bank
                                    // 1..4 - specific bank
    uint8_t type;                   // type of filter
};

// filter types below
#define filter_none         0x00    // filter empty or deleted
#define filter_valid        0x01    // filter valid
#define filter_active       0x81    // filter valid and active

#define filter_block        0x01
#define filter_pass         0x02
#define filter_trig_a       0x03    // trigger with start and end address
#define filter_trig_c       0x04    // trigger with start adress and count

#define filter_act_block    0x81
#define filter_act_pass     0x82
#define filter_act_trig_a   0x83    // trigger with start and end address
#define filter_act_trig_c   0x84    // trigger with start adress and count

// bit 7 of type defines if a filter is active or inactive
// when set the filter is active, otherwise inactive

#define BLOCK_THIS 1
#define PASS_THIS  2
#define TRIG_START 3
#define TRIG_END   4

class Tracer {

    public:

    filter block_filter[NUMFILTERS];
    filter pass_filter[NUMFILTERS];
    filter trig_filter[NUMFILTERS];

    filter filters[NUMFILTERS];

    Tracer() {
        // initialize to default settings

        // clear all filter arrays
        for (int i = 0; i < NUMFILTERS; i++) 
        {
            block_filter[i].start_adr = 0;
            block_filter[i].end_adr = 0;
            block_filter[i].count_adr = 0;
            block_filter[i].bank = 0;          // default is any bank, but not supported in BETA
            block_filter[i].type = 0;

            pass_filter[i].start_adr = 0;
            pass_filter[i].end_adr = 0;
            pass_filter[i].count_adr = 0;
            pass_filter[i].bank = 0;          // default is any bank, but not supported in BETA
            pass_filter[i].type = 0;

            trig_filter[i].start_adr = 0;
            trig_filter[i].end_adr = 0;
            trig_filter[i].count_adr = 0;
            trig_filter[i].bank = 0;          // default is any bank, but not supported in BETA
            trig_filter[i].type = 0;                        
        }
    }


    // apply the filter, should we pass or block the current address
    // this is called for every sample by the tracer main loop
    // returns 1 if sample should be blocked
    //         2 if sample can be passed
    //         3 if this is a start trigger address
    //         4 if this is an end trigger address

    int apply(uint16_t adr) {
        bool pass_active = false;
        bool pass_potential = false;        // possible pass address
        bool trig_s_potential = false;      // possible trigger start address
        bool trig_e_potential = false;      // possible trigger end address

        for (int i = 0; i < NUMFILTERS; i++) {

            // test block
            if ((filters[i].type == filter_act_block) &&
                (adr >= filters[i].start_adr) &&
                (adr <= filters[i].end_adr)) {         
                    // block takes priority, so always block this sample and get out   
                    return BLOCK_THIS;         
            }
            // test pass
            if ((filters[i].type == filter_act_pass) &&
                (adr >= filters[i].start_adr) &&
                (adr <= filters[i].end_adr)) {         
                    // this may be a pass, if there is no later block
                    pass_potential = true;
            }

            // now check for start trigger
            if (((filters[i].type == filter_act_trig_a) || (filters[i].type == filter_act_trig_c)) && 
                (adr == filters[i].start_adr)) {
                    // trigger start address found, if there is no later block
                    trig_s_potential = true;
            }

            // now check for end trigger
            if ((filters[i].type == filter_act_trig_a) && (adr == filters[i].end_adr)) {
                    // trigger end address found, if there is no later block
                    trig_e_potential = true;
            }           
        }                  

        // if we get here it is because:
        //      - there was a pass potential, which is not blocked
        //      - there was a trig potential, which is not blocked
        //      - there was no block or potential, which is a pass

        if (pass_potential) return PASS_THIS;
        if (trig_s_potential) return TRIG_START;
        if (trig_e_potential) return TRIG_END;
        return PASS_THIS;
    }

    // add a new filter
    // parameters:
    // tp - filter type, 0-block, 1-pass, 2-trig
    // start_adr, end_adr: start and end addresses (or count)
    // bank (not used yet)
    // a new filter is enabled by default
    // returns the entry number, if -1 then no free entry found
    
    int add_filter(uint8_t tp, uint16_t start_adr, uint16_t end_adr, int bank)
    {
        // first find a free entry in the array
        int entry = 0; 

        while ((entry < NUMFILTERS) && (block_filter[entry].type != filter_none)) {
            entry++;
        }

        if (entry > NUMFILTERS) return -1;              // no free netries found

        // we have a valid entry, add parameters
        filters[entry].type = tp;
        filters[entry].start_adr = start_adr;
        filters[entry].end_adr = end_adr;
        filters[entry].bank = bank;

        return entry;
    }

    // remove a filter by entry number
    // returns -1 if not succesful, otehrwise returns the entry number
    int del_filter(int entry)
    {
        int result = -1;

        if ((entry < 5) || (entry > NUMFILTERS)) return -1;  // invalid entry number

        filters[entry].type = 0;        // invalidate entry
        return entry;
    }

    // set the status of a filter to active or inactive
    // returns the new status of the filter
    uint8_t set_filter(int entry)
    {

        return 0;
    }

    // toggle the status of a filter
    uint8_t toggle_filter(int entry)
    {
        return 0;
    }


    uint8_t get_filter(int entry)
    {
        return 0;
    }


    void clear_filters()
    {
        // clear all filter arrays
        for (int i = 0; i < NUMFILTERS; i++) 
        {
            block_filter[i].start_adr = 0;
            block_filter[i].end_adr = 0;
            block_filter[i].count_adr = 0;
            block_filter[i].bank = 0;          // default is any bank, but not supported in BETA
            block_filter[i].type = 0;

            pass_filter[i].start_adr = 0;
            pass_filter[i].end_adr = 0;
            pass_filter[i].count_adr = 0;
            pass_filter[i].bank = 0;          // default is any bank, but not supported in BETA
            pass_filter[i].type = 0;

            trig_filter[i].start_adr = 0;
            trig_filter[i].end_adr = 0;
            trig_filter[i].count_adr = 0;
            trig_filter[i].bank = 0;          // default is any bank, but not supported in BETA
            trig_filter[i].type = 0;                        
        }
    }

    // set items to the default value and save in fram
    int set_default() {
        // set all addresses to NULL 
        // initialize all filters, then set defaults values

        clear_filters();

        // now initialize block_filters
        block_filter[0].start_adr = 0x0098;         // 0x0098 - 0x00A1       RSTKB and RST05
        block_filter[0].end_adr   = 0x00A1;
        block_filter[0].type      = filter_valid;   // valid but not active

        block_filter[1].start_adr = 0x0177;         // 0x0177 - 0x0178       delay for debounce
        block_filter[1].end_adr   = 0x0178;
        block_filter[1].type      = filter_valid;   // valid but not active

        block_filter[2].start_adr = 0x089C;         // 0x089C - 0x089D       BLINK01
        block_filter[2].end_adr   = 0x089D;                
        block_filter[2].type      = filter_valid;   // valid but not active

        block_filter[3].start_adr = 0x0000;         // 0x0000 - 0x5FFF       system ROMS (inlcuding Page 4!)  
        block_filter[3].end_adr   = 0x4FFF;
        block_filter[3].type      = filter_valid;   // valid but not active

        block_filter[4].start_adr = 0x6000;         // 0x6000 - 0x7FFF       HP-IL ROMs (including the printer!)
        block_filter[4].end_adr   = 0x7FFF;
        block_filter[4].type      = filter_valid;   // valid but not active

        if (gpio_get(P_PWO) == 0)
        {
            // when PWO = low we can write to FRAM
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_tracer_start, (uint8_t*)block_filter, sizeof(block_filter));
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_tracer_start + sizeof(block_filter), (uint8_t*)pass_filter, sizeof(pass_filter));
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_tracer_start + sizeof(block_filter) + sizeof(pass_filter), (uint8_t*)trig_filter, sizeof(trig_filter));
            return 1;
        }
        else
        {
            // PWO was high, calculator is running and cannot write to FRAM
            return 0;
        }
    }


    // save all settings in FRAM, can only be done when PWO is low!!
    // is done automatically when HP41 power goes down
    // returns 1 (true) if succesful
    int save() {
        if (gpio_get(P_PWO) == 0)
        {
            // when PWO = low we can write to FRAM
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_tracer_start, (uint8_t*)block_filter, sizeof(block_filter));
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_tracer_start + sizeof(block_filter), (uint8_t*)pass_filter, sizeof(pass_filter));
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_tracer_start + sizeof(block_filter) + sizeof(pass_filter), (uint8_t*)trig_filter, sizeof(trig_filter));
            return 1;
        }
        else
        {
            // PWO was high, calculator is running and cannot write to FRAM
            return 0;
        }
    }

    // retrieve settings from FRAM in array for use, can only be done when PWO is low!!
    // is done automatically upon device power up
    // returns 1 (true) if succesful
    int retrieve() {
        if (gpio_get(P_PWO) == 0)
        {
            // when PWO = low we can write to FRAM
            // fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_gsettings_start, gsettings, sizeof(gsettings));
            // when PWO = low we can read from FRAM
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_tracer_start, (uint8_t*)block_filter, sizeof(block_filter));
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_tracer_start + sizeof(block_filter), (uint8_t*)pass_filter, sizeof(pass_filter));
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_tracer_start + sizeof(block_filter) + sizeof(pass_filter), (uint8_t*)trig_filter, sizeof(trig_filter));
            return 1;
        }
        else
        {
            // PWO was high, calculator is running and cannot write to FRAM
            return 0;
        }
    }

} ; // end of class GSettings

#ifdef __cplusplus
}
#endif

#endif