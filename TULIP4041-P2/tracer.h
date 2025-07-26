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
#include "tulip.h"
#include "pico/stdlib.h"
#include "cdc_helper.h"
#include "emulation.h"
#include "userinterface.h"
#include "fram.h"

// definition of the structure for the analyzer functions
// different variations to support dynamic sizing of the tracebuffer

// TLine  is for the maximum possible trace line structure with FI and HP-IL
struct TLine {
    uint32_t    cycle_number;       // to count the cycles since the last PWO       4 bytes      4
    uint32_t    data1;              // DATA D31..D00                                4 bytes      8
    uint32_t    data2;              // DATA D55..D32                                4 bytes     12
    uint32_t    fi1;                // for FI line tracing                          4 bytes     16
    uint32_t    fi2;                //                                              4 bytes     20
    uint16_t    ramslct;            // selected RAM chip                            4 bytes     24
    uint16_t    isa_address;        // ISA address                                  2 bytes     26
    uint16_t    isa_instruction;    // ISA instruction with SYNC status             2 bytes     28
    uint16_t    xq_instr;           // instruction decoded                          2 bytes     30   
    uint16_t    frame_in;           // HP-IL frame input                            2 bytes     32
    uint16_t    frame_out;          // HP-IL frame output                           2 bytes     34    
    uint8_t     bank;               // current selected bank                        1 byte      35
    uint8_t     HPILregs[9];        // HP-IL registers                              9 bytes     44 
    bool        xq_carry;           // when carry is sent                           1 byte      45
} ;

// TLine  is for basic tracing with FI
struct TLine_FI {
    uint32_t    cycle_number;       // to count the cycles since the last PWO       // 4 bytes      4
    uint32_t    data1;              // DATA D31..D00                                // 4 bytes     12  
    uint32_t    data2;              // DATA D55..D32                                // 4 bytes     16
    uint32_t    fi1;                // for FI line tracing                          // 4 bytes     20
    uint32_t    fi2;                //                                              // 4 bytes     24
    uint16_t    isa_address;        // ISA address                                  // 2 bytes      6    
    uint16_t    isa_instruction;    // ISA instruction with SYNC status             // 2 bytes      8
    uint16_t    ramslct;            // selected RAM chip                            // 4 bytes     28
    // uint16_t    xq_instr;           // instruction decoded              
    // bool        xq_carry;           // when carry is sent
    // uint16_t    frame_in;           // HP-IL frame input
    // uint16_t    frame_out;          // HP-IL frame output
    // uint8_t     HPILregs[9];        // HP-IL registers
} ;


// TLine basic is for the smallest possible trace line structure
struct TLine_basic {
    uint32_t    cycle_number;       // to count the cycles since the last PWO       // 4 bytes      4
    uint32_t    data1;              // DATA D31..D00                                // 4 bytes     10   
    uint32_t    data2;              // DATA D55..D32                                // 4 bytes     14
    uint16_t    isa_address;        // ISA address                                  // 2 bytes      6
    uint16_t    isa_instruction;    // ISA instruction with SYNC status             // 2 bytes      8       
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
    uint32_t    cycle_number;       // to count the cycles since the last PWO       // 4 bytes      4
    uint32_t    data1;              // DATA D31..D00                                // 4 bytes     12
    uint32_t    data2;              // DATA D55..D32                                // 4 bytes     16
    uint16_t    isa_address;        // ISA address                                  // 2 bytes      6 
    uint16_t    isa_instruction;    // ISA instruction with SYNC status             // 2 bytes      8
    // uint32_t    fi1;                // for FI line tracing
    // uint32_t    fi2;                
    // uint16_t    xq_instr;           // instruction decoded   
    // bool        xq_carry;           // when carry is sent
    uint16_t    ramslct;            // selected RAM chip                            // 4 bytes     20
    uint16_t    frame_in;           // HP-IL frame input                            // 2 bytes     22
    uint16_t    frame_out;          // HP-IL frame output                           // 2 bytes     24    
    uint8_t     HPILregs[9];        // HP-IL registers                              // 9 bytes     33
};


// TraceLine definition for PWO events to correctly show in the Tracer
struct TLine_PowerEvent 
{
    uint32_t cycle_number;          // to count the cycles since the last PWO
    // HP41powermode prev_mode;   // previous power mode
    // HP41powermode new_mode;    // new power mode
    absolute_time_t t_start;        // start time of this event
    absolute_time_t t_end;          // end time of this event
};



extern const char *mnemonics[];

void TraceBuffer_init();
void Trace_task();

#define numILmnemonics      49      // number of elements in IL_mnemonics 0.. 48, PILBox commands now included


// *******************************************************************
// new setup for the filters/triggers
// using the setup from Thomas Fänge
// one big array with 2 bits for every HP41 address, no bank discrimination
// bit meaning        00 -- pass
//                    01 -- block
//                    10 -- trigger start
//                    11 -- trigger end
// 
// this vastly reduces the load on the CPU for the tracer since the filtering can be very fast in core1

// In addition define a first level filter per Page/Bank to Pass or Block
// Use a dynamic array per Bank of 4K words * 2 bits per word = 1K per Bank

#define     ADDR_MASK   0xFFFF
#define     BRK_SIZE ((ADDR_MASK+1)/(32/2)) // 2 bits per brkpt




// #define BRK_MASK(a,w) ((m_brkpt[a>>4]w >> ((a & 0xF)<<1)) & 0b11)
// #define BRK_SHFT(a) ((a & 0xF)<<1)
// #define BRK_WORD(a) (a >> 4)


#define FILTER_BLOCK_THIS   0b00
#define FILTER_PASS_THIS    0b01
#define FILTER_TRIG_START   0b10
#define FILTER_TRIG_END     0b11


class Filter {

    public:

    uint32_t    m_filter[BRK_SIZE];      // BRK_SIZE expands to 4096, with 32 bits per word
                                    // total size is 16 KByte for the breakpoints

    Filter() {
        // initialize to default settings

        // clear the main filter array
        for (int i = 0; i < BRK_SIZE; i++) {
            m_filter[i] = 0;
        }
    }


    // apply the filter, should we pass or block the current address
    // this is called for every sample by the tracer main loop
    // returns 1 

    uint8_t apply(uint16_t adr) {

        // get the word and bit position
        int word = adr >> 4;          // divide by 16
        int bit = (adr & 0xF) << 1;   // multiply by 2
        uint32_t mask = 0b11 << bit;  // create the mask for the 2 bits
        uint32_t fil;
        fil = m_filter[word] & mask; // get the filter bits for this address
        // and shift the filter bits out
        fil >>= bit;               // shift the bits to the right
        // and return the value
        return fil; // return the filter bits, 0=pass, 1=block, 2=trigger start, 3=trigger end
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


        return entry;
    }

    // remove a filter by entry number
    // returns -1 if not succesful, otehrwise returns the entry number
    int del_filter(int entry)
    {
        int result = -1;

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

    }

    // set items to the default value and save in fram
    int set_default() {
        // set all addresses to NULL 
        // initialize all filters, then set defaults values

        clear_filters();

        return 0;


    }


    // save all settings in FRAM, can only be done when PWO is low!!
    // is done automatically when HP41 power goes down
    // returns 1 (true) if succesful
    int save() {
        if (gpio_get(P_PWO) == 0)
        {
            // when PWO = low we can write to FRAM
            
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
    
            return 1;
        }
        else
        {
            // PWO was high, calculator is running and cannot write to FRAM
            return 0;
        }
    }

} ; // end of class Filter

#ifdef __cplusplus
}
#endif

#endif