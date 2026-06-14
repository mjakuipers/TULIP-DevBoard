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


// TODO: use spare bits in xq_carry as a status to indicate if the TULIP is driving the output:
// useful for ISA, DATA, FI, carry
// there are spare bits in DATA, FI, ISA instruction and Bank

// TLine  is for the maximum possible trace line structure with FI and HP-IL
struct __attribute__((packed))TLine {                                       //      bytes     total   bits used  spare bits
    uint32_t    cycle_number;       // to count the cycles since the last PWO       4 bytes      4      32          0
    uint32_t    data1;              // DATA D31..D00                                4 bytes      8      32          0
    uint32_t    data2;              // DATA D55..D32                                4 bytes     12      24+2        6
                                    // bit 29: HPILRegs contains a time-tag, not actual HPIL register values
                                    // bit 30: instruction decoded by TULIP
                                    // bit 31: DATA driven by TULIP
    uint16_t    isa_address;        // ISA address                                  2 bytes     14      16          0
    uint16_t    isa_instruction;    // ISA instruction with SYNC status             2 bytes     16      12+4        0
                                    // bit 12-13: active bank
                                    // bit 14: TULIP driving ISA (address is in TULIP)
                                    // bit 15: TULIP driving carry, carry is not actually traced!
    uint16_t    ramslct;            // selected RAM chip                            2 bytes     18      10          6   
    uint16_t    fi;                 // compressed FI                                2 bytes     20      14          2
    uint16_t    prphslct;           // selected peripheral                          2 bytes     22      12          4
    uint16_t    frame_in;           // HP-IL frame input                            2 bytes     24      16          0
    uint16_t    frame_out;          // HP-IL frame output                           2 bytes     26      16          0
    uint8_t     HPILregs[9];        // HP-IL registers                              9 bytes     35      36          0

    // in case the frame is indicated as a time-tage only, the absolute time in usecs is stored in the HPILregs, 64 bits = 8 bytes
    //   typedef uint64_t absolute_time_t;
    //   this will be done with a cast
    //   if there is a time tage, the cycle number will be valid but likely zero
    //   The RAMSLCT will have  a value showing the type of event related to the time tag:
    //     0x00 - unknown event
    //     0x01 - deep sleep  -> light sleep transition
    //     0x02 - light sleep -> deep sleep transition
    //     0x03 - deep sleep  -> running transition
    //     0x04 - light sleep -> running transition
    //     0x05 - running     -> light sleep transition
    //     0x06 - running     -> deep sleep transition
    //     0x10 - external trigger
    //     0x11 - wake-up by ISA trigger
    //                                total size is 35 bytes
}; 


// add prphslct, 2 bytes. Actually only 10 bits. Use new status word of 32 bits:
//  bit 0..9    prphslct, actually only 8 bits?
//  bit 10..19  ramslct
//  bit 20..21  current bank
//  bit 22      TULIP driving DATA, could be in DATA
//


struct __attribute__((packed))TTLine {                                       //      bytes     total   bits used spare bits
    uint32_t    cycle_number;       // to count the cycles since the last PWO       4 bytes      4    32        0
    uint32_t    data1;              // DATA D31..D00                                4 bytes      8    32        0
    uint32_t    data2;              // DATA D55..D32                                4 bytes     12    24        8
    uint32_t    status;             // see bit assignment above                     4 bytes     16    32        0

    uint16_t    isa_address;        // ISA address                                  2 bytes     14    16        0
    uint16_t    isa_instruction;    // ISA instruction with SYNC status             2 bytes     16    12        4   

    uint16_t    fi;                 // compressed FI                                2 bytes     20    14        2
    uint16_t    frame_in;           // HP-IL frame input                            2 bytes     24    16        0
    uint16_t    frame_out;          // HP-IL frame output                           2 bytes     26    16        0
    uint8_t     HPILregs[9];        // HP-IL registers                              9 bytes     35    35        0
                                    //                               total size is 35 bytes
}; 


// tracer individual bits to pass:
// RAMSLCT bit 15 is 1 if our RAM access, 0 if other RAM access or ROM access
// FI is compressed to 16 bits, with 1 bit per digit, so we can

// TLine  is for basic tracing with FI
struct __attribute__((packed))TLine_FI {
    uint32_t    cycle_number;       // to count the cycles since the last PWO       4 bytes      4
    uint32_t    data1;              // DATA D31..D00                                4 bytes      8
    uint32_t    data2;              // DATA D55..D32                                4 bytes     12
    uint16_t    isa_address;        // ISA address                                  2 bytes     14
    uint16_t    isa_instruction;    // ISA instruction with SYNC status             2 bytes     16
    uint16_t    ramslct;            // selected RAM chip                            2 bytes     18
    uint16_t    fi;                 // compressed FI                                2 bytes     20
    
 // uint16_t    xq_instr;           // instruction decoded                          2 bytes     22   
 // uint16_t    frame_in;           // HP-IL frame input                            2 bytes     24
 // uint16_t    frame_out;          // HP-IL frame output                           2 bytes     26    
 // uint8_t     bank;               // current selected bank                        1 byte      27
 // uint8_t     HPILregs[9];        // HP-IL registers                              9 bytes     36 
 // bool        xq_carry;           // when carry is sent                           1 byte      37
} ;


// TLine basic is for the smallest possible trace line structure
struct __attribute__((packed))TLine_basic {
    uint32_t    cycle_number;       // to count the cycles since the last PWO       4 bytes      4
    uint32_t    data1;              // DATA D31..D00                                4 bytes      8
    uint32_t    data2;              // DATA D55..D32                                4 bytes     12
    uint16_t    isa_address;        // ISA address                                  2 bytes     14
    uint16_t    isa_instruction;    // ISA instruction with SYNC status             2 bytes     16    

 // uint16_t    ramslct;            // selected RAM chip                            2 bytes     18
 // uint16_t    fi;                 // compressed FI                                2 bytes     20
 // uint16_t    xq_instr;           // instruction decoded                          2 bytes     22   
 // uint16_t    frame_in;           // HP-IL frame input                            2 bytes     24
 // uint16_t    frame_out;          // HP-IL frame output                           2 bytes     26    
 // uint8_t     bank;               // current selected bank                        1 byte      27
 // uint8_t     HPILregs[9];        // HP-IL registers                              9 bytes     36 
 // bool        xq_carry;           // when carry is sent                           1 byte      37
};

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
#define     BRK_SIZE    ((ADDR_MASK+1)/(32/2)) // 2 bits per brkpt


// #define BRK_MASK(a,w) ((m_brkpt[a>>4]w >> ((a & 0xF)<<1)) & 0b11)
// #define BRK_SHFT(a) ((a & 0xF)<<1)
// #define BRK_WORD(a) (a >> 4)

#define FILTER_PASS             0b00
#define FILTER_BLOCK            0b01
#define FILTER_TRIG             0b10
#define FILTER_TRIGEND          0b11

#define PRE_TRIG_BUFSIZE        256     // fixed size of the pre-trigger buffer
                                        // how much is used is user defined by the global settings in tracer_pretrig

// struct for the filter as stored in RAM and FRAM

struct m_filter_trigger {          
    uint32_t    filter[BRK_SIZE];         // BRK_SIZE expands to 4096, with 32 bits per word
                                          // total size is 16 KByte for all breakpoints                                
    uint16_t    filter_page[16];          // first level filter per Page, enough bits for bank and part of Page, will probably not use
                                          // exact bit assignment to be defined, but for example bit 0 could be pass/block, bit
                                          // bit 0    : pass/block entire page
                                          // bit 1..2 : bank 0..3
                                          // bit 3    : use bank in filter or not
                                          // bit 4    : use bank with triggers or not
                                          // bit 5    : page has a filter in it
                                          // bit 6    : page has a trigger in it
                                          // bit 7    : reserved for future use  
                                          // bit 8..15: reserved for future use
    uint8_t     filter_active;            // is the filter active or not
    uint64_t    trigger_data;             // data to compare for triggering, can be used for address or data triggering
    uint64_t    trigger_data_mask;        // mask for the data to compare for triggering
    uint16_t    trigger_instr;            // ISA instruction to compare for triggering
    uint16_t    trigger_instr_mask;       // mask for the ISA instruction to compare
    uint8_t     trigger_sync;             // SYNC status to compare for triggering, 0 or 1
    uint8_t     trigger_bank;             // bank to compare for triggering, 0..3
    uint16_t    trigger_FI;               // FI to compare for triggering, 16 bits compressed FI
    uint16_t    trigger_FI_mask;          // mask for the FI to compare for triggering
    uint8_t     trigger_HPILframe;        // HP-IL frame to compare for triggering, 16 bits
    uint16_t    trigger_HPILframe_mask;   // mask for the HP-IL frame to compare for triggering
    uint8_t     trigger_HPILin;           // IL frame in or out
    uint8_t     trigger_carry;            // carry status to compare for triggering, 0 or 1
    uint8_t     trigger_mode;             // trigger mode
                                          // value 00: off
                                          //       01: single trigger, stop after trigger, requires manual re-arm
                                          //       02: multi trigger, re-trigger after new trigger
                                          //       03: auto single, re-arm after PWO
    uint8_t     trigger_runmode;          // run mode after trigger
                                          // value 00: stop on PWO
                                          //       01: run until trigger count
                                          //       02: run until trigger end
                                          //       03: run until trigger count OR end
                                          //       04: run until overflow of trace buffer
    uint8_t    trigger_filter;            // appply filters or not
    uint8_t    trigger_count;             // count for multi trigger mode, counts number of triggers before the trigger
    uint16_t   post_trigger_count;        // number of samples to record after the trigger, user defined in global settings, default 0 meaning that the tracer will not stop until a POWOFF or a TRIGEND trigger is hit
    uint8_t    pre_trigger_count;         // number of pre-trigger samples to store, 0..256
    uint8_t    reserved[944];             // reserve space to make total struct size exactly 17 KByte (17,408 bytes)

    // total size of this struct is 17 KByte (17,408 bytes)
};  

extern const char *mnemonics[];

void TraceBuffer_init();
void Trace_task();

#define numILmnemonics      49      // number of elements in IL_mnemonics 0.. 48, PILBox commands now included


class Filter {

    public: 
        // m_filter_trigger filters;       // the filter settings, to be stored in FRAM
        
        uint32_t    filter[BRK_SIZE];       // BRK_SIZE expands to 4096, with 32 bits per word
                                            // total size is 16 KByte for all breakpoints
        m_filter_trigger filters;           // the filter settings, to be stored in FRAM

    Filter() {
        // initialize to default settings

        // clear the main filter array
        for (int i = 0; i < BRK_SIZE; i++) {
            filter[i] = 0;
        }

        // initialize all other settings to 0
        filters.filter_active = 0;
        filters.trigger_data = 0;
        filters.trigger_data_mask = 0;
        filters.trigger_instr = 0;
        filters.trigger_instr_mask = 0;
        filters.trigger_sync = 0;
        filters.trigger_bank = 0;
        filters.trigger_FI = 0;
        filters.trigger_FI_mask = 0;
        filters.trigger_HPILframe = 0;
        filters.trigger_HPILframe_mask = 0;
        filters.trigger_HPILin = 0;
        filters.trigger_carry = 0;
        filters.trigger_mode = 0;
        filters.trigger_runmode = 0;
        filters.trigger_filter = 0;

    }


    // apply the filter, should we pass or block the current address
    // this is called for every sample by the tracer main loop
    // return values:
    // 0b00 -- pass
    // 0b01 -- block
    // 0b10 -- trigger start
    // 0b11 -- trigger end
    inline uint8_t apply(uint16_t adr) {
        // get the word and bit position
        int word = adr >> 4;          // divide by 16
        int bit = (adr & 0xF) << 1;   // multiply by 2
        uint32_t mask = 0b11 << bit;  // create the mask for the 2 bits
        uint32_t fil;
        fil = filter[word] & mask; // get the filter bits for this address
        // and shift the filter bits out
        fil >>= bit;               // shift the bits to the right
        // and return the value
        return fil; // return the filter bits, 0=pass, 1=block, 2=trigger start, 3=trigger end
    }


    // add a new filter
    // parameters:
    // tp - filter type, 0-block, 1-pass, 2-trig start, 3-trig end
    // start_adr, end_adr: start and end addresses (or count)
    // bank (not used yet)
    // returns false if invalid parameters, true if succesfully added
    bool add_filter(uint8_t tp, uint16_t start_adr, uint16_t end_adr, int bank)
    {
        // first find a free entry in the array
        if (tp > 0b11) {
            // invalid filter type
            return false;
        }

        if (end_adr == 0) {
            // if end_adr is 0 then only the start address will have the filter applied, so set end_adr to start_adr
            end_adr = start_adr;
        }

        // use the start and end address to set the filter bits in the array
        for (uint16_t adr = start_adr; adr <= end_adr; adr++) {
            int word = adr >> 4;          // divide by 16
            int bit = (adr & 0xF) << 1;   // multiply by 2
            uint32_t mask = 0b11 << bit;  // create the mask for the 2 bits
            // clear the bits first
            filter[word] &= ~mask;
            // set the new filter type
            filter[word] |= (tp << bit) & mask;
        }
        return true;
    }

    void clear_filters()
    {
        // clear all filter arrays
        for (int i = 0; i < BRK_SIZE; i++) {
            filter[i] = 0;
        }
        filters.filter_active = 0;
        filters.trigger_data = 0;
        filters.trigger_data_mask = 0;
        filters.trigger_instr = 0;
        filters.trigger_instr_mask = 0;
        filters.trigger_sync = 0;
        filters.trigger_bank = 0;
        filters.trigger_FI = 0;
        filters.trigger_FI_mask = 0;
        filters.trigger_HPILframe = 0;
        filters.trigger_HPILframe_mask = 0;
        filters.trigger_HPILin = 0;
        filters.trigger_carry = 0;
        filters.trigger_mode = 0;
        filters.trigger_runmode = 0;
        filters.trigger_filter = 0;
    }

    // set items to the default value and save in fram
    void set_default() {
        // set all addresses to NULL 
        // initialize all filters, then set defaults values
        clear_filters();
    }


    // save filters settings in FRAM, can only be done when PWO is low!!
    // is done automatically when HP41 power goes down
    // returns 1 (true) if succesful
    bool save() {
        if (gpio_get(P_PWO) == 0) {
            // when PWO = low we can write to FRAM
            // only save the filter array for now
            fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_TRACER_start, (uint8_t*)filter, sizeof(filter));
            
            return true;
        } else{
            // PWO was high, calculator is running and cannot write to FRAM
            return false;
        }
    }

    // retrieve settings from FRAM in array for use, can only be done when PWO is low!!
    // is done automatically upon device power up
    // returns 1 (true) if succesful
    bool retrieve() {
        if (gpio_get(P_PWO) == 0) {
            // read the filter array from FRAM when PWO is low
            fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, FRAM_TRACER_start, (uint8_t*)filter, sizeof(filter));            
            return true;

        } else {
            // PWO was high, calculator is running and cannot write to FRAM
            return false;
        }
    }

} ; // end of class Filter



#ifdef __cplusplus
}
#endif

#endif