/*
 * emulation.c
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

// hardware emulation and core1 time critical code is here

/*  Change log
 *  sep 2024    HP-IL instruction decoding regeister selection changed to SELP instruction
 *              not in read/writ decoding    

*/



// pragme causes the compiler to optimize code in this file regardless of the build mode
#pragma GCC optimize("O3")

#include "emulation.h"


// Definitions for HP41 peripheral mode
bool HP82153A_active = false;               // HP82153A Wand active
bool HP82242A_active = false;               // HP82242A Blinky IR printer module active
bool HP82104A_active = false;               // HP82104A Cardreader active
bool HP82182A_active = false;               // HP82104A Time Module active
bool HP41CLem_active = false;               // HP41CL emulation active (limited function set)
bool HEPAX_active    = false;               // HEPAX module native support
bool TINY_active     = false;               // TINY instructions active, SELP 8

int16_t SLCT_PRPH = -1;                     // contains active selected peripheral
                                            // -1     - none active
                                            // 0 .. 7 - used by HP-IL
                                            // 8      - TINY41 instruction
                                            // 9      - SELP9 mode for the HP82143A 


bool SELP9_status_BUSY = false;             // becomes true when the printbuffer is full
                                            // could be used when a delay for IR printing is active
bool SELP9_status_VALID = true;             // always true

uint16_t SELP9_status = prtstatus_def;      // contains the HP82143A printer status bits, set to default values

// Bank Switching, keep trackof enabled Bank for each page
uint8_t active_bank[16];
uint8_t enabled_bank = 1;               // the enabled bank
uint8_t temp_bank = 1;                  // temporary bank 
bool Banks_dirty = false;            // true when the banks have been changed and need to be updated

uint16_t LocalAdvIgnore = false;            // for ignoring local paper advance  

queue_t WandBuffer;                         // buffer for Wand between cores
uint16_t WandCached = 0xFFFF;               // cached input from Wand, reading from the queue between T53 and T0 is not safe
const int WandBufSize = 100;                // size of Wandbuffer
bool Wand_active = true;

int keycount_print = 0;
int keycount_adv = 0;

bool default_map = false;                   // default is disabled, enable in UI
bool default_map_off = false;               // all ROMs off when true;

enum HP41powermode HP41_powermode;

char  TPrint[200];
int   TPrintLen = 0;


// definitions for HP41 user memory. Testing only supports the HP41CX
// active here are Extended Memory Modules 1 and 2.
uint32_t ramselected = 0;                   // selected user memory register
uint32_t ourselected = 0;                   // selected user memory register,
                                            // 0 means not our or invalid
                                            // any other value is the valid memory address
bool write_pending = false;                 // in case a WRITDATA was detected
bool read_pending  = false;                 // in case READDATA was detected



// definitions for peripheral mode
uint32_t prphselected = 0;                  // selected peripheral

// FI output
uint32_t fi_out1 = 0;                       // for flag output driver
uint32_t fi_out2 = 0;

bool sendflag = false;                      // indicates if any flag is set and if the FI output needs to be driven

                                            // Extended User Memory for testing with HP41CX
// uint32_t usermemHi[0x200];                  // memory range from 0x200 to 0x3FF
// uint32_t usermemLo[0x200];                  // Split in lower (D00..D31) and higher (D32..D55) bits
                                            // to be initialzed with all 0's
                                            // range supported:
                                            // 0x200            NONEXISTENT
                                            // 0x201..0x2EF     Extended Memory Module 1
                                            // 0x2F0..0x300     NONEXISTENT
                                            // 0x301..0x3EF     Extended Memory Module 2
                                            // 0x3F0..0x3FF     NONEXISTENT

// Extended User Memory cache registers, for implementing XMEM in FRAM starting at 0x1E000
uint32_t usermemCacheHi;                    // Extended User Memory cache D32..D55
uint32_t usermemCacheLo;                    // Extended User Memory cache D00..D31
uint64_t usermemCache;                      // for testing with all 56 (64) bits User Register


int fram_offset;                            // offset into the FRAM

const uint16_t *flash_contents = (const uint16_t *) (XIP_BASE + ROM_BASE_OFFSET);

uint16_t fram_buf[0x10];
uint32_t fram_adr;

uint32_t cycle_counter = 0;         // counts cycles since last PWO
struct TLine TraceLine;             // the variable with the TraceLine used in capturing cycles in core1
extern queue_t TraceBuffer;

extern CModules TULIP_Pages;


extern int m_eMode;


absolute_time_t t_start, t_end, t_elapsed;

// HP-IL variables

queue_t HPIL_SendBuffer;
queue_t HPIL_RecvBuffer;

uint8_t HPIL_REG[9];            // HP-IL register stack 
                                // HPIL_REG[0..7] used for read
                                // HPIL_REG[8] is the R1W register (write only)
                                // HPIL_REG[2] is different for read and write!

uint16_t IL_lastframe;          // last frame sent
uint16_t IL_inframe;            // incoming frame
uint16_t IL_reg = 0;            // used for selected HP-IL register
uint16_t nextIL_inst = 0;       // used for detection of IL instructions



// extern void __not_in_flash_func(fram_read)();
// extern void __not_in_flash_func(fram_write)();

// initialize the pio state machines
PIO pio0_pio = pio0;        // pio instantiation for pio0, here we put the main SYNC and ISA input state machine
PIO pio1_pio = pio1;        // pio instantiation for pio1, for most other state machines

int sync_offset     = -1;   // offset in SM instruction memory for the SYNC state machine
int sync_sm         = -1;   // pio sm identifier for the SYNC/ISA state machine

int datain_offset   = -1;   // offset in SM instruction memory for the DATAIN state machine
int datain_sm       = -1;   // identifier for the DATAIN state machine

int debugout_offset = -1;   // offset in SM instruction memory for the DEBUGOUT state machine
int debugout_sm     = -1;   // identifier for the DEBUGOUT state machine

int isaout_offset   = -1;   // offset in SM instruction memory for the ISAOUT state machine
int isaout_sm       = -1;   // identifier for the ISAOUT state machine

int dataout_offset  = -1;   // offset in SM instruction memory for the DATAOUT state machine
int dataout_sm      = -1;   // identifier for the DATAOUT state machine

int fiout_offset    = -1;   // offset in SM instruction memory for the FIOUT state machine
int fiout_sm        = -1;   // identifier for the FIOUT state machine

int irout_offset    = -1;   // offset in SM instruction memory for the IROUT state machine
int irout_sm        = -1;   // identifier for the DATAOUT state machine

int fiin_offset     = -1;   // offset in SM instruction memory for the FIIN state machine
int fiin_sm         = -1;   // identifier for the FIIN state machine




void pio_report()
{
    printf("\n\nResult of PIO state machine inititialization: \n");
    printf("  sync     sm PIO0: %2d, offset: %d\n", sync_sm, sync_offset);
    printf("  datain   sm PIO0: %2d, offset: %d\n", datain_sm, datain_offset);
    printf("  fiin     sm PIO0: %2d, offset: %d\n", fiin_sm, fiin_offset);
    printf("  debugout sm PIO0: %2d, offset: %d\n", debugout_sm, debugout_offset);

    printf("\n");

    printf("  isaout   sm PIO1: %2d, offset: %d\n", isaout_sm, isaout_offset);
    printf("  dataout  sm PIO1: %2d, offset: %d\n", dataout_sm, dataout_offset);
    printf("  irout    sm PIO1: %2d, offset: %d\n", irout_sm, irout_offset);
    printf("  fiout    sm PIO1: %2d, offset: %d\n", fiout_sm, fiout_offset);

    // for testing 
    printf("\n\n");
    printf("  CDC Port 1 connected? %s\n", cdc_connected(ITF_CONSOLE) ? "yes":"no");
    printf("  CDC Port 2 connected? %s\n", cdc_connected(ITF_PRINT) ? "yes":"no");
    printf("  CDC Port 3 connected? %s\n", cdc_connected(ITF_HPIL) ? "yes":"no");
    printf("  CDC Port 4 connected? %s\n", cdc_connected(ITF_TRACE) ? "yes":"no");
    printf("  CDC Port 5 connected? %s\n", cdc_connected(ITF_ILSCOPE) ? "yes":"no");
}

void uif_pio_report()
{
    cli_printf("  Result of PIO state machine inititialization: ");
    cli_printf("  sync     sm PIO0: %2d, offset: %d", sync_sm, sync_offset);
    cli_printf("  datain   sm PIO0: %2d, offset: %d", datain_sm, datain_offset);
    cli_printf("  fiin     sm PIO0: %2d, offset: %d", fiin_sm, fiin_offset);
    cli_printf("  debugout sm PIO0: %2d, offset: %d", debugout_sm, debugout_offset);
    cli_printf("  isaout   sm PIO1: %2d, offset: %d", isaout_sm, isaout_offset);
    cli_printf("  dataout  sm PIO1: %2d, offset: %d", dataout_sm, dataout_offset);
    cli_printf("  irout    sm PIO1: %2d, offset: %d", irout_sm, irout_offset);
    cli_printf("  fiout    sm PIO1: %2d, offset: %d", fiout_sm, fiout_offset);
} 


void pio_init()
{    
    // load and init pio programs in the pio instruction memory

    sync_sm = pio_claim_unused_sm(pio0_pio, true);                          // claim a state machine in pio0
    sync_offset = pio_add_program(pio0_pio, &hp41_pio_sync_program);        // in pio 0 !!

    datain_sm = pio_claim_unused_sm(pio0_pio, true);                        // claim a state machine in pio0
    datain_offset = pio_add_program(pio0_pio, &hp41_pio_datain_program);    // in pio 0 !!

    #if (TULIP_HARDWARE == T_DEVBOARD)
        // we only have FI input on the DevBoard, not on the module version
        // for the fiin state machine we use a second instantiation of the datain state machine
        // these are functionally identical
        fiin_sm = pio_claim_unused_sm(pio0_pio, true);                          // claim a state machine in pio0    
        fiin_offset = datain_offset;
    #endif

    debugout_sm = pio_claim_unused_sm(pio0_pio, true);                      // claim a state machine in pio0
    debugout_offset = pio_add_program(pio0_pio, &hp41_pio_debugout_program);// in pio 0 !!

    isaout_sm = pio_claim_unused_sm(pio1_pio, true);                        // claim a state machine in pio1    
    isaout_offset = pio_add_program(pio1_pio, &hp41_pio_isaout_program);    // in pio 1 !!

    dataout_sm = pio_claim_unused_sm(pio1_pio, true);                        // claim a state machine in pio1    
    dataout_offset = pio_add_program(pio1_pio, &hp41_pio_dataout_program);   // in pio 1 !!

    irout_sm = pio_claim_unused_sm(pio1_pio, true);                          // claim a state machine in pio1    
    irout_offset = pio_add_program(pio1_pio, &hp41_pio_irout_program);       // in pio 1 !!

    fiout_sm = pio_claim_unused_sm(pio1_pio, true);                          // claim a state machine in pio1    
    fiout_offset = pio_add_program(pio1_pio, &hp41_pio_fiout_program);       // in pio 1 !!


    // state machines in PIO0    
    hp41_pio_sync_program_init(pio0_pio, sync_sm, sync_offset, P_ISA, P_T0_TIME);
    hp41_pio_datain_program_init(pio0_pio, datain_sm, datain_offset, P_DATA, P_T0_TIME);

    #if (TULIP_HARDWARE == T_DEVBOARD)
        hp41_pio_datain_program_init(pio0_pio, fiin_sm, datain_offset, P_FI, P_T0_TIME);            // new instantiation of datain for fiin
    #endif

    hp41_pio_debugout_program_init(pio0_pio, debugout_sm, debugout_offset, P_DEBUG, 0, 0, 0);

    // state machines in PIO1
    hp41_pio_isaout_program_init(pio1_pio, isaout_sm, isaout_offset, P_ISA_OUT, P_ISA_OE, 0, 0);
    hp41_pio_dataout_program_init(pio1_pio, dataout_sm, dataout_offset, P_DATA_OUT, P_DATA_OE, P_T0_TIME);
    hp41_pio_irout_program_init(pio1_pio, irout_sm, irout_offset, 0, P_IR_LED, 0);    // uses only sideset
    hp41_pio_fiout_program_init(pio1_pio, fiout_sm, fiout_offset, P_FI_OE, 0, P_T0_TIME);    // no sideset used here

    // pio_report();

    // apply setting to emulation variables where needed
    // this is for the HP82143A printer
    SELP9_status = SELP9_status & 0x37FF;             // clear bits 15 and 14 (printer mode) and 11 (OOP)

    // SELP9_status = SELP9_status | (gsettings(PRT_mode) << 14);         // apply mode bits
    // uint16_t temp = globsetting(PRT_mode);
    SELP9_status = SELP9_status | (globsetting.get(PRT_mode) << 14);         // apply mode bits

    SELP9_status = SELP9_status | (globsetting.get(PRT_paper) << 10);        // apply OOP bit
}

// PWO GPIO callback for resetting the SYNC/ISA state machine
// called at rising and falling egde of PWO
void __not_in_flash_func()pwo_callback(uint gpio, uint32_t events) {
    if (gpio_get(P_PWO) == 0) {
        // PWO is now low, HP41 is sleeping, nothing to do
        // the HP41 generates no CLK's and the sync and datain state machines will be stalled waiting for a CLK01 or CLK02
        // and data pending in the ISR should be cleared, this is done by enforcing a push
        // there should be no data in the sync sm, but there will be data in the datain sm

        pio_sm_exec(pio0_pio, datain_sm, pio_encode_push(0, 0) ); 

        #if (TULIP_HARDWARE == T_DEVBOARD)
            pio_sm_exec(pio0_pio, fiin_sm, pio_encode_push(0, 0) );
            pio_sm_clear_fifos(pio0_pio, fiin_sm); 
        #endif

        pio_sm_clear_fifos(pio1_pio, fiout_sm); 

        pio_sm_exec(pio0_pio, sync_sm, pio_encode_push(0, 0) | pio_encode_sideset(2, 1)); 

        // enforce a jump to the start of the SYNC state machine
        // hp41_pio_program_init(sync_pio, sync_sm, sync_offset, P_ISA, P_D0_TIME);
        pio_sm_clear_fifos(pio0_pio, sync_sm);
        pio_sm_exec(pio0_pio, sync_sm, pio_encode_jmp(sync_offset + hp41_pio_sync_offset_sync_start) | pio_encode_sideset(2, 1));   

        pio_sm_clear_fifos(pio0_pio, datain_sm); 
        pio_sm_exec(pio0_pio, datain_sm, pio_encode_jmp(datain_offset + hp41_pio_datain_offset_data_start));  

        #if (TULIP_HARDWARE == T_DEVBOARD)
            pio_sm_exec(pio0_pio, fiin_sm, pio_encode_jmp(datain_offset + hp41_pio_datain_offset_data_start));  
        #endif
  
        // initialize bank registers
        for (int i = 0; i < 16; i++) {
            active_bank[i] = 1;
        }     
        gpio_put(ONBOARD_LED, 0);           // turn LED off
        // FI output
        fi_out1 = 0;                       // for flag output driver
        fi_out2 = 0;
    } 
    else {
        // upon rising edge of PWO
        // PWO is now high so HP41 is running
        // reset state machine, next SYNC is imminent
        gpio_put(ONBOARD_LED, 1);           // turn LED on

        // enforce a jump to the start of the SYNC state machine
        pio_sm_clear_fifos(pio0_pio, sync_sm);
        pio_sm_exec(pio0_pio, sync_sm, pio_encode_jmp(sync_offset + hp41_pio_sync_offset_sync_start)| pio_encode_sideset(2, 1));  

        // code below to add the value 42 in the state machine Y register to be used as a longer counter
        pio_sm_put(pio0_pio, sync_sm, 42);                                                          // value 42 in the FIFO; 
        pio_sm_exec(pio0_pio, sync_sm, pio_encode_pull(false, false) | pio_encode_sideset(2,1));    // pull to OSR
        pio_sm_exec(pio0_pio, sync_sm, pio_encode_mov(pio_y, pio_osr) | pio_encode_sideset(2,1));    // move OSR to Y

        pio_sm_clear_fifos(pio0_pio, datain_sm); 

        #if (TULIP_HARDWARE == T_DEVBOARD)
            pio_sm_clear_fifos(pio0_pio, fiin_sm);
            pio_sm_exec(pio0_pio, fiin_sm, pio_encode_jmp(datain_offset + hp41_pio_datain_offset_data_start));
        #endif
        
        pio_sm_exec(pio0_pio, datain_sm, pio_encode_jmp(datain_offset + hp41_pio_datain_offset_data_start));    
        pio_sm_exec(pio1_pio, fiout_sm, pio_encode_jmp(fiout_offset + hp41_pio_fiout_offset_fiout_start));  
        pio_sm_clear_fifos(pio1_pio, fiout_sm);

        for (int i = 0; i < 16; i++) {
            // must check if there is any sticky bank set
            if (TULIP_Pages.Pages[i].m_bank & bank_sticky) {
                // if the page is sticky then we set the active bank to the sticky bank
                active_bank[i] = TULIP_Pages.Pages[i].m_bank & 0x0F;   // get the active bank from the sticky bank
            }
            else
                // no sticky bank, so set the active bank to 1
                active_bank[i] = 1;
        }  
        cycle_counter = 0;
        ramselected = 0;
    }
}


// toggle the status of the IR LED, and return if it is ON (1) or off (0)
// must control via the state machine 
bool IR_toggle() {
    // be aware that reading the GPIO for the return value may come too soon after the state machine has executed the jump
    if (!gpio_get(P_IR_LED)) {
        // IR LED is off, so turn it on
        // jump to the label ir_high in the state machine
        pio_sm_exec(pio1_pio, irout_sm, pio_encode_jmp(irout_offset + hp41_pio_irout_offset_ir_high) ); // execute jump to ir_high 
        // the state machine will now wait for any data to arrive in the TX FIFO or a jump to the normal start to turn it off
    } else {
        // IR LED is on, so turn it off
        // jump to the normal start of the state machine at the ir_start label
        pio_sm_exec(pio1_pio, irout_sm, pio_encode_jmp(irout_offset + hp41_pio_irout_offset_ir_start) ); // execute jump to ir_start
    }
    return gpio_get(P_IR_LED);  // return actual LED status, but this may be wrong
}


// function to return the cycle counter
uint32_t cycles() {
    return cycle_counter;
}


// function to wake up the HP41 by driving ISA high if the calculator is sleeping (PWO low)
// ISA is driven for 20 usecs
// ISA is under control by the isaout state machine, so we have to force instructions there to drive ISA
void wakemeup_41()
{
    uint32_t isa_out_data = 1;

    if (gpio_get(P_PWO) == 0) {
        // only if PWO is low!

        // isaout state machine will be stalled at the blocking pull from the TX FIFO
        // so we can simply send a 1, and the data is pulled 
        // we then do a forced jump to the out instruction
        // this will drive ISA and ISA_OE, and stall waiting for the end of T0 (T0_TIME is low here)
        // after a wait of 10-20 usecs we will force a jump to the start of isaout and wait for data to arrive.

        // put a value 1 in the TX FIFO
        isa_out_data = 1;
        pio_sm_exec(pio1_pio, isaout_sm, pio_encode_jmp(isaout_offset + hp41_pio_isaout_offset_isa_inst_out) | pio_encode_sideset(1, 0));           
        pio_sm_put_blocking(pio1_pio, isaout_sm, isa_out_data); 

        // jump to the output of data
        pio_sm_exec(pio1_pio, isaout_sm, pio_encode_jmp(isaout_offset + hp41_pio_isaout_offset_isa_out) | pio_encode_sideset(1, 1));         

        busy_wait_us(20);    // wait 20 us

        // now stop driving ISA_OE by going back to the start of the state machine
        pio_sm_exec(pio1_pio, isaout_sm, pio_encode_jmp(isaout_offset + hp41_pio_isaout_offset_handle_carry) | pio_encode_sideset(1, 0)); 
    }
}

// function to shutdown the HP41 by driving PWO low for 10 us
// uses the same pin as the IR output! 
// this is done by driving the PWO Output Enable driver high, its input is always tied high
// since the IR output is controlled by the irout PIO state machine we have to use this state machine
// The irout state machine is stalled at the ir_start label (out instruction), and we do the following:
// - set x to 0, so it will execute the hilo loop only once
// - do a forced jump to the hilo_loop label
// The state machine will now do 1 nop with a sideset of 1, and wrap to end up at the ir_start, stalling again

void shutdown_41()
{
    #if (TULIP_HARDWARE == T_DEVBOARD)
        // drive PWO_OE high, on the DevBoard this is shared with IR Output
        pio_sm_exec(pio1_pio, irout_sm, pio_encode_set(pio_x, 0) | pio_encode_sideset(1, 0));
        pio_sm_exec(pio1_pio, irout_sm, pio_encode_jmp(irout_offset + hp41_pio_irout_offset_hilo_loop) | pio_encode_sideset(1, 0));
    #else
        // on the Module version PWO is shared with SPARE1
        // drive PWO high for 5 usecs
        gpio_put(P_PWO_OE, 1);           // drive PWO high
        busy_wait_us(5);                 // wait 5 us
        gpio_put(P_PWO_OE, 0);           // drive PWO_OE low
    #endif
    
}


void send_ir_frame(uint32_t frame)
{
    // not much to do, just dump the formatted frame in the TX FIFO of the irout state machine
    // note that this is a blocking function, maybe change that?
    pio_sm_put_blocking(pio1_pio, irout_sm, frame);  // send the data
}

// for external equipment to be able to set an FI flag
void setflag_FI0()
{
    fi_out1 = fi_out1 | FI_00;
    sendflag = true;
}


// initialze emulation after powerup
void InitEmulation()
{
    // initialize bank registers
    for (int i = 0; i < 16; i++) {
        active_bank[i] = 0;
    }
}

// check if a usermemory address exists
int __not_in_flash_func()exist_usermem(uint32_t address)
{
    int xmem_mods;
    int rval = false;

    xmem_mods = globsetting.get(xmem_pages);      // get this from global settings

    if (xmem_mods == 0) {
        // no XMEM modules plugged
        return false;
    }
    if (((xmem_mods == 1) || (xmem_mods == 2)) && (address > 0x200) && (address < 0x2F0)) {
        // 1 or 2 XMEM modules
        return true;
    }
    if ((xmem_mods == 2) && (address > 0x300) && (address < 0x3F0)) {
        // 2 XMEM modules
        return true;
    }
    return rval;
}


// write to an HP-IL register
// modeled after hpil_rw in V41 (by Christoph Giesseling)
//  reg - register number
//  n   - value
void __not_in_flash_func()HPIL_writereg(uint8_t reg, uint16_t n)
{
    n &= 0xFF; 
    TraceLine.frame_out = 0xFFFF;               // default if no frame is sent
    switch (reg)
    {
        case 0: // status reg.
            if (n & 1)
            {
                // Master Clear bit
                HPIL_REG[0] |=  0x80;               // SC=1
                HPIL_REG[1] &=  0xE1;               // IFCR=SRQR=FRNS=FRAV=0
                HPIL_REG[1] |=  0x01;               // ORAV=1
                HPIL_REG[8] &= ~0x01;               // FLGENB=0
                // update_flags();                  // flags are updated outside this routine
                m_eMode = 0;                        // not in transfer any more
            }
            if (n & 2)
            {
                // CLIFCR Clear 'Interface Clear Received'
                HPIL_REG[1] &= 0xEF;                // IFCR=0
                // update_flags();                  // flags are updated outside this routine
            }
            HPIL_REG[0] = n & 0xF9;                 // SLRDY & CLIFCR are self clearing
            break;

        case 1: // control reg.
            HPIL_REG[8] = n;                        // write to R1W
            // update_flags();                      // flags are updated outside this routine
            break;

        case 2: // data register, build frame and send it
            n |= (HPIL_REG[8] & 0xE0) << 3;         // get bits 8-10 from R1W
            HPIL_REG[1] &= 0xF8;                    // FRAV=FRNS=ORAV=0
            IL_lastframe = n;                       // remember the last sent frame
            TraceLine.frame_out = n;                // HP-IL frame output
            queue_try_add(&HPIL_SendBuffer, &n);    // send frame (to HPIL_task in core0 for handling)
                                                    // non-blocking but is OK here
            break;

        default:
            // also valid for REG[3], parallel poll register. 
            // case 3 in V41 handles AUTOIDY and creates a thread
            // in TUP4041 the AUTOIDY bit is checked by the core0 HPIL_task, no further action here
            HPIL_REG[reg] = n;
    }
}          


// check the HP41 Power Mode
void PowerMode_task()
{
    // monitor PWO and SYNC to keep the HP41 power mode up to date
    enum HP41powermode prev_mode;

    const char *const mode_str[] = {
        "RUNNING",
        "STANDBY",
        "OFF    " };

    prev_mode = HP41_powermode;
    if (gpio_get(P_PWO))
    {
        // PWO is high, HP41 is running
        HP41_powermode = eAwake;    // HP41 is running
    }
    else
    {
        // PWO is low, HP41 is sleeping (DEEP or LIGHT sleep)
        if (gpio_get(P_SYNC))
        {
            // SYNC is high, PWO is low
            HP41_powermode = eLightSleep;
        }
        else
        {
            // SYNC is low, PWO is low
            HP41_powermode = eDeepSleep;
        }
    }

    if (HP41_powermode != prev_mode)
    {
        // there has been a change, report it

        int64_t us_elapsed;
        int32_t secs_elapsed;
        int32_t ms_elapsed;
        // absolute_time_t t_start, t_end, t_elapsed;

        t_end = get_absolute_time();
        us_elapsed = absolute_time_diff_us(t_start, t_end);
        t_start = t_end;
        ms_elapsed = (us_elapsed % (1000 * 1000)) / 1000 ;
        secs_elapsed = us_elapsed / (1000 * 1000);

        TPrintLen = 0;
        TPrintLen += sprintf(TPrint + TPrintLen,"** HP41 Powermode: %s - previous mode %5d.%03d secs %s", 
            mode_str[HP41_powermode], secs_elapsed, ms_elapsed, mode_str[prev_mode]);

        if (prev_mode == eAwake) {
            TPrintLen += sprintf(TPrint + TPrintLen," %9d bus cycles", cycle_counter);
        }
        else {
            TPrintLen += sprintf(TPrint + TPrintLen, "");
        }
        puts(TPrint);

        // if PWO is now low the calculator has stopped running
        // this means that we need to cleanup some dirty stuff
        if ((HP41_powermode != eAwake) && Banks_dirty) {
            // if the banks are dirty we need to save the ROM map to FRAM
            Banks_dirty = false;            // reset dirty flag
            TULIP_Pages.save();             // save the ROM map to FRAM
        }
    }
}


// Core 1 code

// main engine to process HP41 bus traffic:
//  -- trace address, instruction, data (triggers, addres range??)
//  -- get address and decide if a response is needed (ROM/QRAM hit)
//  -- track DATA for several parameters
//  -- push instruction if there is an address match needed
//  -- decode instruction and decide if a response is needed
//      -- WROM
//      -- HEPAX
//      -- bankswitching
//      -- peripheral emulation
//      -- display tracking, better to do in the other core with the analyzer?
//      -- carry drive
//      -- FI drive/undrive
//      -- data drive
//
// this code MUST run from RAM. Running from flash will occasionally result in delay
// fetching code from FLASH at critical moments. Found this on READ instructions

void __not_in_flash_func(core1_pio)()
{
    uint32_t rx_addr = 0;
    uint32_t rx_inst = 0;
    uint32_t rx_inst_t = 0;  // for tracing
    uint32_t rx_isa  = 0;
    uint32_t rx_sync = 0;
    uint32_t regdata = 0;

    uint16_t wrom_addr = 0;  // for WROM instruction
    uint16_t wrom_data = 0;
    uint16_t wrom_page = 0;

    uint32_t rx_data1 = 0;
    uint32_t rx_data2 = 0;

    uint32_t fi_data1 = 0;
    uint32_t fi_data2 = 0;

    uint16_t ptr_data = 0;                      // pinter data received to be printed

    uint32_t rom_addr = 0;
    uint32_t rom_pg = 0;

    uint32_t isa_out_data = 0;
    uint8_t  fram_res[15];              // for reading fram results efficiently

    uint8_t traceoverflow = 0;          // to detect TraceBuffer overflow

    uint isaout_sideset = 0;

    uint pagetoswitch = 1;               // bank for ROM paging
    uint banktoswitch = 1;

    int i = 0;
    int var = 0;
    int n = 0;

    uint8_t ctrlReg;  // used for HP-IL FI flags

    bool sendcarry = false;         // set to true if carry at D0_TIME must be sent
    TraceLine.frame_out = 0;


    // core1 code must be running before starting the SYNC/ISA State Machine
    // PIO State Machine is already running
    // get data from the SYNC/ISA State Machine FIFO and put it in the ISA buffer

    printf("\n core1 starting ...\n\n");

    while(1)
    {
        // ==============================================================================
        // T54  T54  T54  T54  T54  T54  T54  T54  T54  T54  T54  T54  T54  T54  T54  T54
        //
        // first read is always the ISA INSTRUCTION, this happens at T54,
        // almost immediately after the last instruction bit was read on the rising edge at CLK01 
        // and the extra read for getting the SYNC status
        // from the state machine these are 12 bits, left justified in the 32-bit ISR FIFO from the sync_pio state machine
        //   the two MSB's are the SYNC status and a copy of the instruction MSB
        //   the 10 LSB's are the instruction
        //     bits in instruction:   |11|10|09|08|07|06|05|04|03|02|01|00|
        //                                  | <-      instruction ->      |
        //                                bit 10: same as bit 09
        //                                bit 11: SYNC status
        // a WROM instruction (0x040) would show as 0x840, with the SYNC bit set
        // this makes for a quick decision if it is a real instruction (SYNC high) or data (SYNC low)
        // to get right justified INSTRUCTION bits simply shift 20 bits, the SYNC bit is in bit 11

        rx_inst = pio_sm_get_blocking(pio0_pio, sync_sm) >> 20;     // first read is INSTRUCTION

        // gpio_put(P_DEBUG, 1);
        pio_sm_put(pio0_pio, debugout_sm, DBG_OUT0);

        TraceLine.cycle_number = cycle_counter;
        cycle_counter++;

        // TraceLine.xq_instr = 0;
        // TraceLine.xq_data = 0;

        // gpio_pulse(P_DEBUG, 2);                                  // for debugging                                      

        // for peripheral emulation this is the place where the instruction must be decoded and handled
        // fast enough to be ready to send a carry at T0_TIME or data starting at T0_TIME
        // other instructions may take more time to process
        // instructions requiring immediate action:
        //  - when carry must be sent at D0
        //  - register data must be presented on DATA this is the case for the instructions:
        //      - READDATA
        //      - READ 1..15
        //  - peripheral data to be presented on DATA

        TraceLine.xq_instr = rx_inst_t;    
        rx_inst_t = 0;

        // TraceLine.xq_data1 = fi_out1;
        // TraceLine.xq_data2 = fi_out2;

        if ((fi_out1 != 0 ) || (fi_out2 != 0))
        {
            // if one of fi_out is not zero we have a flag to be output
            // in case we have to drive FI for any flags present
            // we only drive the FI_OE signal, this is an active high signal
            // The FI_out is tied to GND, and FI itself is normally pulled high by the system
            // of fi_outx is all zero's, nothing needs to be sent
            // we could alssend fi_outx always ...
            pio_sm_put(pio1_pio, fiout_sm, fi_out1);  // send the data            
            pio_sm_put(pio1_pio, fiout_sm, fi_out2);  // send the data  
        }

        if (rx_inst == inst_READDATA)
        {
            // respond to READDATA instruction, return the register pointed to by ramselected
            // or the peripheral register in case of a valid PRPH selected register
            // must handle here
            // since we do not have the full data line complete here we can only handle part of the data
            // especially when there was a WRIT DATA immediately before
            // we do this as follows:
            //   immediate action is to push the first half of the data D0..D31
            //   delayed action to push the 2nd half of data
                
            // TraceLine.xq_instr = rx_inst;    
            rx_inst_t = rx_inst;            
            if (ourselected > 0) {
                // regdata = usermemLo[ourselected - 0x200];
                regdata = usermemCacheLo;
                // TraceLine.xq_data1 = regdata;
                pio_sm_put_blocking(pio1_pio, dataout_sm, regdata);  // send the data
                read_pending = true;
            }              
            else if (HP82153A_active && (prphselected == PRPH_wand))
            {
                // Wand active and selected, so may need to send data
                // ROM has checked if data is actually in the buffer
                // that is already in the cached Wand data due to speed 
                regdata = WandCached;
                WandCached = 0xFFFF;          // mark as consumed
                // TraceLine.xq_data1 = regdata;
                pio_sm_put_blocking(pio1_pio, dataout_sm, regdata);  // send the data
                pio_sm_put_blocking(pio1_pio, dataout_sm, 0);  // send the data
            }
        } 
      
        if (globsetting.get(HP82143A_enabled))
        {

            // handling of HP82143A (Printer) specific instructions
            switch (rx_inst) {

                case inst_SELP9:                // 0x264, SELP 9 NUT instruction, starts SELP9 mode, SYNC bit set!            
                    rx_inst_t = rx_inst;     
                    SLCT_PRPH = 9;              // selected peripheral is 9, the HP82143A printer
                    break;
                case SELP9_BUSY:                // 0x003, set carry if printer busy, no SYNC bit!            
                    if (SLCT_PRPH == 9) { 
                        rx_inst_t = rx_inst; 
                        // we report the printer as busy when the printbuffer is full and the printer is ON
                        if (globsetting.get(PRT_power)) {
                            SELP9_status_BUSY = queue_is_full(&PrintBuffer);
                            // SELP9_status_BUSY = false;      // never BUSY
                            sendcarry = SELP9_status_BUSY;
                        }
                        SLCT_PRPH = -1;          // return control back to the NUT
                    }
                    break;
                case SELP9_POWON:               // 0x083, set carry if printer is ON, no SYNC bit!!
                    if (SLCT_PRPH == 9) { 
                        rx_inst_t = rx_inst;
                        sendcarry = globsetting.get(PRT_power);
                        SLCT_PRPH = -1;          // return control back to the NUT                    
                    }
                    break;
                case SELP9_VALID:               // 0x043, set carry if status valid, no SYNC bit! 
                    if (SLCT_PRPH == 9) { 
                        rx_inst_t = rx_inst; 
                        if (globsetting.get(PRT_power)) {
                            // valid status only when the printer is ON
                            sendcarry = SELP9_status_VALID;
                        }
                        SLCT_PRPH = -1;          // return control back to the NUT                      
                    }
                    break;           
                                                                
                case SELP9_RDPTRN:              // 0x03A, transfer printer status word to C[10..13], no SYNC bit!
                    if (SLCT_PRPH == 9) { 
                        rx_inst_t = rx_inst; 
                        rx_data1 = SELP9_status;
                        // transfer printer status word to C[10..13], peripheral remains selected
                        // start preparing to send out to DATA
                        pio_sm_put_blocking(pio1_pio, dataout_sm, 0);                   // bits D00..D32 are always 0
                        pio_sm_put_blocking(pio1_pio, dataout_sm, SELP9_status << 8);   // bits D33..D55 contain status
                        // after reading status the status bits for the PRINT and ADV key are reset
                        // but the status is read multiple times, so keep track of the number of reads
                        if (keycount_print == 0) {
                            // SELP9_status = SELP9_status & 0xC0FF;
                            SELP9_status = SELP9_status & (~prt_ADV_mask);
                            SELP9_status = SELP9_status & (~prt_PRT_mask);
                        }
                        else
                        {
                            keycount_print--;
                        }                                                       
                    }
                    break;
            }   // switch
        }

        if (globsetting.get(HP82160A_enabled))
        {
            // handling of HP82160A (HP-IL) specific instructions
            // filter out SELP0..SELP9: 0x024 to 0x1E4, 
            // this is a class 0 instruction PPPPIIII00, where IIII = 0b1001, PPPP = 0..7
            // with the SYNC bit set this becomes 0b100PPPIIII00 or 0b100PPP100100, in hex: 0x824, mask: 0b111000111111 or 0xE3F
            // only P = 0..7 is valid
            // mask for selected peripheral is 0b000111000000 or 0x1C0

            // this part needs to be changed, selection of the register is by the SELP instruction, 
            // NOT by the bits in the READ/WRIT instruction, these are ignored (reaearch by Thomas and Mike)

            if ((rx_inst & 0xE3F) == 0x824) {
                // check for SELP0 .. SELPx
                rx_inst_t = rx_inst; 
                SLCT_PRPH = (rx_inst & 0x1C0) >> 6;
                nextIL_inst = (rx_inst & 0b000111000000) | 0b000000111010;

                // after this the next instruction expected is C=HPIL_Cx
                // 0x824 -> 0x03A       0b100PPP100100  -> 0b0PPP111010
                IL_reg = (rx_inst & 0x1C0) >> 6;           // selected HP-IL register

            }
            else if ((SLCT_PRPH >= 0) && (SLCT_PRPH <= 7) && (rx_inst & 0x23A) == 0x03A)
            {   
                // check for C=HPIL_Cx 
                // only after SELP0..7, SYNC not set
                // filter out 0x03A .. 0x1FA, 0b0000111010 .. 0b0111111010
                // format: 0b0ppp111010 or 0x03A as mask, 0b0111000000 or 0x1C0 as filter

                // HP-IL selected and instruction is 0x03A..0x1FA
                // Read HP-IL Register

                // if HP-IL register 2 is read (data input/output) do the following:
                //  - REG2 is filled when data arrives from the queue, checked after T0
                //  - check if FRAV and FRNS
                //  - if so, then new data is available
                //  - clear FRAV and FRNS flags (will also clear interrupt)
                //  - update R1W with C01..C03 from R1R
                rx_inst_t = rx_inst; 
                // IL_reg = (rx_inst & 0x1C0) >> 6;            // this can be commented out ??
                if (IL_reg == 2)
                {
                    // Register 2, data input/output
                    n = HPIL_REG[1] & 0x06;               // FRAV & FRNS
                    HPIL_REG[1] &= 0xF9;                  // FRAV=FRNS=0
                    if (n)      // FRAV or FRNS is set
                    {
                        // FRAV or FRNS is set, so we have a valid frame received
                        HPIL_REG[8] &= ~0xE0;               // clear CO3-CO1
                        HPIL_REG[8] |= HPIL_REG[1] & 0xE0;  // copy CO3-CO1 from R1R to R1W
                    }
                    // update_flags();                      // flags are updated elsewhere
                }

                // now return register value to C[0..1]
                regdata = HPIL_REG[IL_reg];

                // TraceLine.xq_data1 = regdata;
                pio_sm_put_blocking(pio1_pio, dataout_sm, regdata);     // send the data
                pio_sm_put_blocking(pio1_pio, dataout_sm, 0);           // send the data

            }
            else if ((SLCT_PRPH >= 0) && (SLCT_PRPH <= 7) && ((rx_inst & 0x003) == 0x001))
            {
                // check for HPIL_Cx = (literal), 0bcccccccc01, c = value to be placed in register
                // only after SELP0..7, SYNC not set
                // deselects peripheral
                rx_inst_t = rx_inst; 
                HPIL_writereg(SLCT_PRPH, (rx_inst & 0x3FC) >> 2);
                SLCT_PRPH = -1;
            }
            else if ((SLCT_PRPH >= 0) && (SLCT_PRPH <= 7) && ((rx_inst & 0x003) == 0x003))
            {
                // 
                rx_inst_t = rx_inst; 
                // this is the 3rd instruction of a 3-instruction sequence
                // always returns control back to the NUT
                // no further checks, accept all instructions ending with 2 1's
                SLCT_PRPH = -1;
            }
        }
    

        // check for CLASS0 READ instruction
        // read data from selected register and present on the DATA line
        // register must be sent out to DATA now!
        // if ((rx_inst & mask_CLASS0) == mask_READ) 
        // {
            
        //     TraceLine.xq_instr = rx_inst;
        // }

        if (sendcarry != 0 ) {
            // now send the carry, simply dump a '1' bit into the isaout state machine
            // all the rest is handled by the isaout state machine
            isa_out_data = 0x001;
            pio_sm_put_blocking(pio1_pio, isaout_sm, isa_out_data);
            sendcarry = false;
            TraceLine.xq_carry = true;
        } 
        else {
            TraceLine.xq_carry = false;
        }
  
        // put instruction in TraceLine for the ISA instruction
        TraceLine.isa_instruction = rx_inst;

        // gpio_put(P_DEBUG, 0);
        // gpio_pulse(P_DEBUG, 3);                                  // for debugging 
        pio_sm_put(pio0_pio, debugout_sm, DBG_OUT1);
     
        // ========================================================================
        // T0   T0   T0   T0   T0   T0   T0   T0   T0   T0   T0   T0   T0   T0   T0
        //
        // after reading ISA INSTRUCTION next should be the completion of the DATA
        // the first part is autopushed after 32 bits, the 2nd part should be in the ISR at T0
        // prior to D0 the DATA RX FIFO should be empty (this was read already)
        // to keep in sync, ensure this is indeed empty
        // this must be done BEFORE D0_TIME!

        // blocking read for the 2nd part of DATA rx FIFO to get bits D55..D32
        // this happens at T0_TIME
        // consider doing 28 clok cycles in the datain state machine to get this data earlier for WROM handling
        // figure out when to check for PWO otherwise we might stall here

        // must wait until after T0_TIME, need to figure this out

        // PWO check must be done here. If PWO is low (after SYNC) then HP41 is off
        // and we must not stall in getting any data to prevent getting out of SYNC
        // we could also check for the POWOFF instruction, but that does not work if HP41 is switched off
        // for other reasons (power removal etc)
        // PWO goes low a bit after SYNC, but there are not enough clocks to generate a real D0_TIME pulse
        // if there are not enough clocks then there will be no data pushed when reading rx_data2 below
        // if we have nothing else to do then is is probably best to sleep a few useconds (2 clocks = 3..4 us) 
        // and check PWO prior to reading data

        // at this point we have two options: PWO goes low (power down) or we receive new data

        if ((HPIL_REG[8] & 0x01) == 0x01)   // if FLGENB set in R1W
        {
            ctrlReg = HPIL_REG[1];          // flags are in R1R
            // HPIL Register 1 (write) bit 0 is the flag output enable
            // when it is 1 the flags are enabled
            if ((ctrlReg & 0x01) == 0x01) 
            {
                fi_out2 = fi_out2 | FI_10;        // ORAV
            }
            else
            {
                fi_out2 = fi_out2 & FI_10_off;
            }
        }

        while (gpio_get(P_PWO) && (pio_sm_get_rx_fifo_level(pio0_pio, datain_sm) == 0))
        {
                // do nothing, just wait until PWO goes low or data arrives in the FIFO!
        }

        // when we get out of the previous loop we can continue if PWO is still high

        // for debugging       
        pio_sm_put(pio0_pio, debugout_sm, DBG_OUT2);

        if (gpio_get(P_PWO))
        { 
            // PWO is high, so we can read DATA at T0
            rx_data2 = pio_sm_get_blocking(pio0_pio, datain_sm);        //  blocking read from datain state machine

            // when we get here there is also data in the FI input state machine
            // read both words, but at the first start there may be only one word in the RX FIFO

            #if (TULIP_HARDWARE == T_DEVBOARD) 
                if (pio_sm_get_rx_fifo_level(pio0, fiin_sm) > 0) {
                    TraceLine.fi1 = pio_sm_get_blocking(pio0_pio, fiin_sm); 
                }

                if (pio_sm_get_rx_fifo_level(pio0, fiin_sm) > 0){
                    TraceLine.fi2 = pio_sm_get_blocking(pio0_pio, fiin_sm); 
                }
            #elif (TULIP_HARDWARE == T_MODULE)
                TraceLine.fi1 = fi_out1;
                TraceLine.fi2 = fi_out2;
            #endif

            TraceLine.data2 = rx_data2 >> 8;                            // get received data in TraceLine and align

            // package for the TraceLine is now complete, send it to the TraceBuffer
            // this is non-blocking to prevent going out of SYNC on a full trace bufer
            if ((trace_enabled) || (trace_outside == (rom_addr > 0x6000)))
            {
                traceoverflow = queue_try_add(&TraceBuffer, &TraceLine);                    // add to internal trace buffer for handling by core0
                // traceoverflow = 0 (false) if the element was not added, this is an overflow
                // to be added to the next succesfull trace
            }

            // Traceline is sent, so clear HP-IL frameout for default value
            TraceLine.frame_out = 0xFFFF;               // default if no frame is sent

            // DATA is now fully complete after T0
            // ready to check on instructions needing the data from the PREVIOUS cycle
            // this would be a WRITDATA (or any of the other WRIT variations)
            // this would be triggered by a delayed WRITE here

            if (write_pending == true) 
            {
                // there was a pending write, captured data is now bits D32..55
                // to be stored in selected register
                // the lower bits have already been saved
                if (ourselected > 0) {
                    // usermemHi[ourselected - 0x200] = (rx_data2 >> 8);
                    usermemCacheHi = (rx_data2 >> 8);           // now in cache
                    // TraceLine.xq_data2 = (rx_data2 >> 8);
                    write_pending = false;       
                }
            }

            // now that any pending writes have been done, a possible pending read can be handled
            // this is needed in case a READ immediately follows a WRITE

            if (read_pending == true) 
            {
                // handle a pending READ of the higher data register bits D32..D55
                // TODO: check alignment
                if (ourselected > 0) {
                    // regdata = usermemHi[ourselected - 0x200];
                    regdata =usermemCacheHi;
                    // TraceLine.xq_data2 = regdata;
                    pio_sm_put_blocking(pio1_pio, dataout_sm, regdata);  // send the data
                    read_pending = false;
                }
            }

            // this is after the priority handling of DATA and ISA, and after T0
            // and plenty of time to handle some other stuff, so check the ENBANK instructions here
            // this must be done anyway before the next instruction fetch
            // bankswitching currently only valid in Page 8 for the OSX ROM

            if ((rx_inst == inst_RAMSLCT) && (ourselected > 0)) {
                // if there was a RAMSLCT, write the cached data back to FRAM
                // only if there was relevant data in the cache (in our selected memory)
                // which new register is selected is not known here, we write back to the previous selected register

                // XMEM registers start at FRAM 0x1E000, this is HP register 0x200
                // 8 bytes (64 bits) are used to store one register
                // ourselected is between 0x200 - 0x3FF

                fram_offset = XMEMstart + 8 * (ourselected - 0x200);

                // and write to FRAM
                // lower bits first
                fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, fram_offset, (uint8_t*)&usermemCacheLo, 4);
                fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, fram_offset + 4, (uint8_t*)&usermemCacheHi, 4);
            }

            if (HP82153A_active)
            {
                if (queue_is_empty(&WandBuffer) && (WandCached == 0xFFFF))
                {
                    fi_out1 = fi_out1 & FI_00_off;
                    fi_out1 = fi_out1 & FI_02_off;
                }
                else
                {
                    // cached Wand data is valid or data in the queue
                    // now that data is available, set the flags
                    // Wand sets flags 0 and 2 when it has data
                    // Flag 0 is set when it wants attention
                    fi_out1 = fi_out1 | FI_00 | FI_02;

                    if (WandCached == 0xFFFF)
                    {
                        // our cache is available, so read from the buffer
                        // otherwise cache may be filled but not read yet
                        queue_try_remove(&WandBuffer, &WandCached);
                    }
                }
            }

            // check for incoming data from HP-IL
            // if (HP82160A_active && !queue_is_empty(&HPIL_RecvBuffer))
            if (globsetting.get(HP82160A_enabled) && !queue_is_empty(&HPIL_RecvBuffer))
            {
                // HP-IL is active and there is data in the HP-IL receive queue
                // this means that a valid frame has arrived
                // process the frames according to V41 (Christoph Giesselink)
                // RFC frames are handled by core0

                // read the frame from the queue
                queue_remove_blocking(&HPIL_RecvBuffer, &IL_inframe);
                TraceLine.frame_in = IL_inframe;

                // adjust flags according to received frame and state:
                if ((IL_inframe & 0x400) == 0)              // DOE : 0xx
                {
                    if (HPIL_REG[0] & 0x10)                 // LA, Listener Active
                    {
                        HPIL_REG[1] |= 5;                   // FRAV=ORAV=1
                    }
                    else if (HPIL_REG[0] & 0x20)            // TA, Talker Active
                    {
                        if ((IL_lastframe & 0xff) != (IL_inframe & 0xff))
                            // incoming and outgoing do not match
                            // set FRNS
                            HPIL_REG[1] |= 3;               // FRNS=ORAV=1
                        else
                            HPIL_REG[1] |= 1;               // ORAV=1
                    }
                    else
                    {
                        // SendFrame(IL_inframe);           // case LA=TA=0, retransmit DOE frames!
                                                            // send frame (to HPIL_task in cor0 for handling)
                        queue_try_add(&HPIL_SendBuffer, &IL_inframe);    
                    }
                    if ((IL_inframe & 0x100) != 0)
                        HPIL_REG[1] |= 8;                   // SRQR=1
                    else
                        HPIL_REG[1] &= ~8;                  // SRQR=0
                }

                else if ((IL_inframe & 0x200) != 0)         // IDY : 11x
                {
                    if ((HPIL_REG[0] & 0x30) == 0x30)       // TA=LA=1 (scope mode)
                        HPIL_REG[1] |= 5;                   // FRAV=ORAV=1
                    else
                        HPIL_REG[1] |= 1;                   // ORAV=1
                    if ((IL_inframe & 0x100) != 0)
                        HPIL_REG[1] |= 8;                   // SRQR=1
                    else
                        HPIL_REG[1] &= ~8;                  // SRQR=0
                }

                else if ((IL_inframe & 0x100) == 0)         // CMD : 100
                {
                    if ((HPIL_REG[0] & 0x30) == 0x30)       // TA=LA=1 (scope mode)
                        HPIL_REG[1] |= 5;                   // FRAV=ORAV=1
                    else if (IL_lastframe == IL_inframe)
                        HPIL_REG[1] |= 1;                   // ORAV=1
                    else
                        HPIL_REG[1] |= 3;                   // FRNS=ORAV=1
                    if (IL_inframe == 0x490)                // IFC
                        HPIL_REG[1] |= 16;                  // IFCR=1
                }

                else                                        // RDY : 101
                {
                    if ((HPIL_REG[0] & 0x30) == 0x30)       // TA=LA=1 (scope mode)
                        HPIL_REG[1] |= 5;                   // FRAV=ORAV=1
                    else if ((IL_inframe & 0xc0) == 0x40)   // ARG
                        HPIL_REG[1] |= 5;                   // FRAV=ORAV=1
                    else
                    {
                        if (IL_lastframe == IL_inframe)
                            HPIL_REG[1] |= 1;               // ORAV=1
                        else
                            HPIL_REG[1] |= 3;               // FRNS=ORAV=1
                    }
                }

                if ((HPIL_REG[1] & 6) != 0)                 // if FRNS or FRAV, returned frame in R1R & R2R
                {
                    HPIL_REG[2] = (uint8_t)IL_inframe;        // returned frame
                    HPIL_REG[1] &= 0x1f;
                    HPIL_REG[1] |= (IL_inframe & 0x700) >> 3; // save received bits 8-10 to R1R
                }
                // flags are updated later in the cycle
                // AUTO IDY check is done in core0
            }
            // end of incoming HP-IL handling




            pio_sm_put(pio0_pio, debugout_sm, DBG_OUT3);

            // =================================================================================
            // T30   T30   T30   T30   T30   T30   T30   T30   T30   T30   T30   T30   T30   T30
            // PWO is high, HP41 still running, get ISA ADDRESS

            rx_addr = pio_sm_get_blocking(pio0_pio, sync_sm);           // 2nd read from SYNC/ISA state machine is ADDRESS
            rom_addr = rx_addr >> 16;                 
                              // to get the current address
            TraceLine.isa_address = rom_addr;
            // at this point we have a valid address which is left justified in rx_addr and right justified in rom_addr
            // this can be used to check for a ROM hit, read data and present it on the ISA output

            // this is the place to handle the ROM mapping and bankswitching 

            // monitor ENBANK instructions for tracing, not used for reading ROM images
            // must isolate page for correct handling
            // take care of the following:
            //   HP41CX, ENBANK in Page 3 actually switched Page 5 active bank
            //   for Port 8/9, A/B, C/D and E/F both banks are swichted
            //   

            rom_pg = (rom_addr & 0xF000) >> 12;                            // right aligned page number
            banktoswitch = 0;                                               // default no bankswitching

            // Bankswitch instructions are:
            // ENBANK1      0x100   0b0001.0000.0000.0000
            // ENBANK2      0x180   0b0001.1000.0000.0000
            // ENBANK3      0x140   0b0001.0100.0000.0000
            // ENBANK4      0x1C0   0b0001.1100.0000.0000
            switch (rx_inst)
            {
                case inst_ENBANK1:     
                    banktoswitch = 1;
                    break;
                case inst_ENBANK2:
                    banktoswitch = 2;
                    break;
                case inst_ENBANK3:
                    banktoswitch = 3;
                    break;
                case inst_ENBANK4:
                    banktoswitch = 4;
                    break;                  
                default:
                    banktoswitch = 0;       // if there was no ENBANKx instruction, do nothing
                    break;
            }

            if (banktoswitch != 0) {
                // only if there is an ENBANKx instruction
                if (rom_pg == 3)
                {
                    // HP41CX, ENBANK in Page 3 actually switched Page 5 active bank
                    active_bank[5] = banktoswitch;
                }
                else if ((rom_pg > 8))
                {
                    // switching for Port 8/9, A/B, C/D and E/F odd and even Pages
                    active_bank[rom_pg & 0xFFFE]     = banktoswitch;
                    active_bank[(rom_pg & 0xFFFE) + 1] = banktoswitch;
                }

                // the bankswitching is done, but a check needs to be dome if this is sticky or not
                // if the page is not enabled then the bank is not sticky, so no need to
                if ((TULIP_Pages.Pages[rom_pg].m_bank & bank_sticky) && TULIP_Pages.isEnabled(rom_pg, 1)) {
                    // set the active bank for both the odd and even pages
                    Banks_dirty = true;          // mark the banks as dirty, so they need to be updated
                    TULIP_Pages.Pages[(rom_pg & 0xFFFE)].m_bank     = bank_sticky + banktoswitch;     // set the new sticky bank for the even page
                    TULIP_Pages.Pages[(rom_pg & 0xFFFE) + 1].m_bank = bank_sticky + banktoswitch;     // and for the odd page
                }
            }

            // we can now add the Bank information to the Traceline
            enabled_bank = active_bank[rom_pg];          // this is the currently enabled bank for the page
            TraceLine.bank = enabled_bank;

            // gpio_pulse(P_DEBUG, 6);                                  // for debugging 
            pio_sm_put(pio0_pio, debugout_sm, DBG_OUT4);

            // Read the ROM, embedded or from FLASH
            isa_out_data = 0xffff;              // default if ROM is not enabled

            // must do an extra check here
            // if the Page is not enabled and the Bank > 1 a check is needed if there is a Bank 1 plugged
            if ((enabled_bank > 1) && !TULIP_Pages.isEnabled(rom_pg, enabled_bank) && TULIP_Pages.isEnabled(rom_pg, 1)) {
                // get the word from Bank 1
                // special case for the Advantage Module
                isa_out_data = TULIP_Pages.getword(rom_addr, 1);  // get the word from Bank 1
            } else {            
                if (TULIP_Pages.isEnabled(rom_pg, enabled_bank)) {
                    // check if the page is enabled, if not then return empty data
                    // this is for the TULIP pages, which are not always enabled
                    // this is done in the TULIP_Pages class
                    isa_out_data = TULIP_Pages.getword(rom_addr, enabled_bank);  // get the word from Page/Bank
                }
            }

            if (isa_out_data != 0xFFFF)
            // not an empty page so return valid data
            // otherwise nothing will be returned
            {
                // we must force the state machine to jump to the offset of the isa instruction out
                // normally it is stalled waiting for data right before sending the carry out at D0_TIME
                // this is for faster handling of the carry as we have only 2 clock cycles for that
                // pio_encode_sideset(1, 1), to also use the sideset here (1 sideset bit, value 1), otherwise ISA_OE wil be forced low
                // to be OR'ed with the encoded jump instruction
                pio_sm_exec(pio1_pio, isaout_sm, pio_encode_jmp(isaout_offset + hp41_pio_isaout_offset_isa_inst_out) | pio_encode_sideset(1, 0)); 

                // now ready to put the instruction in the TX FIFO, the rest is done by the PIO state machine
                // the state machine will send out the ISA instruction at the right time, no need to worry about that here
                // just make sure that this is done before the start of SYNC_TIME
                // this is blocking, but the FIFO should  always be empty
                pio_sm_put_blocking(pio1_pio, isaout_sm, isa_out_data); 
            }

            // gpio_pulse(P_DEBUG, 7);                                  // for debugging  
            pio_sm_put(pio0_pio, debugout_sm, DBG_OUT5);

            // =================================================================================
            // T32   T32   T32   T32   T32   T32   T32   T32   T32   T32   T32   T32   T32   T32
            // handling of ISA address now complete
            // next up is capture the first part of DATA, D31..D0

            rx_data1 = pio_sm_get_blocking(pio0_pio, datain_sm);                 // blocking read from datain state machine   
            TraceLine.data1 = rx_data1;

            // gpio_pulse(P_DEBUG, 8);                                  // for debugging             
            pio_sm_put(pio0_pio, debugout_sm, DBG_OUT6);

            
            // we now have D31..D0 in rx_data1
            // this is only D31..00. If more data bits are needed wait for the rest.
            switch (rx_inst) {
                case inst_WROM :                // WROM instruction

                    // TraceLine.xq_data1 = rx_data1;
                    // TraceLine.xq_instr = rx_inst;    

                    if (1 == 0)            // WROM disabled in the BETA version
                    {
                        rx_inst_t = rx_inst;                    
                        wrom_addr = (rx_data1 & 0x0FFFF000) >> 12;   // isolate the WROM address from DATA
                        wrom_data = rx_data1 & 0x03FF;               // isolate WROM data from DATA
                        wrom_page = (wrom_addr & 0xF000) >> 12;                        
                
                        switch(wrom_page) {
                            case 0xC:
                                fram_adr = FRAM1_OFFSET + 2 * (wrom_addr & 0x0FFF);      // FRAM page 0
                                fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, fram_adr, (uint8_t*)&wrom_data, 2);
                                break;

                            case 0xD:
                                fram_adr = FRAM3_OFFSET + 2 * (wrom_addr & 0x0FFF);      // FRAM page 1
                                fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, fram_adr, (uint8_t*)&wrom_data, 2);
                                break;

                        }
                    }

                
                case SELP9_PRINTC:              // 0x007, send byte on C[0..1] to the printbuffer, no SYNC bit!
                    // if ((SLCT_PRPH == 9) && HP82143A_active) { 
                    if ((SLCT_PRPH == 9) && globsetting.get(HP82143A_enabled)) {     
                        // send C[0..1] to the printbuffer
                        // can be handled only after C is available
                        rx_inst_t = rx_inst; 
                        ptr_data = rx_data1 & 0xFF;
                        // TraceLine.xq_data1 = ptr_data;
                        SLCT_PRPH = -1;          // return control back to the NUT after this instruction
                        // handle printer emulation for control codes
                        // note that the control codes immediately below are handled by the printer simulator and ignored here
                        //      0xA0..0xB7: skip 0..23 characters, done by printer simulator
                        //      0xB8..0xBF: skip 0..7 colum sin SCO mode, done by printer simulator
                        switch (ptr_data) {
                            // all characters are sent to the printbuffer anyway
                            // some have a special meaning and must be handled here to set the printer status bits
                            case 0xE0:                      // EOLL, left justified EOL
                                                            // if in graphics mode, terminate SCO
                                                            // set EOL status bit
                                                            // clear TEO status bit
                                        SELP9_status = SELP9_status & (~prt_SCO_mask);      // clear SCO bit
                                        SELP9_status = SELP9_status & (~prt_TEO_mask);      // clear TEO bit
                                        SELP9_status = SELP9_status | prt_EOL_mask;         // set EOL bit
                                        break;

                            case 0xE8:                      // EOLR, right justified EOL
                                                            // if in graphics mode, terminate SCO
                                                            // set EOL status bit
                                                            // set TEO status bit
                                        SELP9_status = SELP9_status & (~prt_SCO_mask);      // clear SCO bit
                                        SELP9_status = SELP9_status | prt_TEO_mask;         // set TEO bit
                                        SELP9_status = SELP9_status | prt_EOL_mask;         // set EOL bit
                                        break;
                            case 0xD0 ... 0xD7:             // set/clear DW, CO and LC mode (ptr_data bits 2, 1, 0 respectively)
                                                            // DW, CO and LC bits are bit 7, 6 and 5
                                        SELP9_status = SELP9_status & ~prt_DWM_mask & ~prt_SCO_mask & ~prt_LCA_mask;     // clear out the relevant bits
                                        SELP9_status = SELP9_status | ((ptr_data & 0x07) << 5);                         // and set according to lsb's of received char
                                        break;
                            case 0xFE:                      // clear local paper Advance Ingnore
                                        LocalAdvIgnore = false;
                                        break;
                            case 0xFF:                      // set local Paper Advance Ignore
                                        LocalAdvIgnore = true;
                                        break;
                            default:
                                        break;                        
                        }
                        // to be tested: P_BUSY status change on printbuffer full
                        if (queue_is_full(&PrintBuffer)) {
                            SELP9_status_BUSY = true;
                        }
                        else
                        {
                            // send char to be printed to core0, non-blocking!
                            // queue should not be full here, just in case
                            SELP9_status_BUSY = false;
                            queue_try_add(&PrintBuffer, &ptr_data);    
                        }
                    }
                    break;  // end of case for SELP9_PRINTC

                case SELP9_RTNCPU:              // 0x005, return control to the HP41 CPU, ends SELP mode, no SYNC bit!
                                                // does not need any data
                    // TraceLine.xq_instr = rx_inst;    
                    rx_inst_t = rx_inst; 
                    // if ((SLCT_PRPH == 9) && HP82143A_active) { 
                    if ((SLCT_PRPH == 9) && globsetting.get(HP82143A_enabled)) {  
                        // return control to the NUT
                        SLCT_PRPH = -1;          // return control back to the NUT                        
                    }
                    break;

                case inst_RAMSLCT:              // RAMSLCT, must wait for DATA[2..0] to become available
                    // TraceLine.xq_instr = rx_inst;    
                    rx_inst_t = rx_inst; 
                    ramselected = rx_data1 & 0x03FF;
                    prphselected = 0;           // deselect any peripheral when RAMSLCT appears
                    if (exist_usermem(ramselected)) {
                        // we have a valid existing register address
                        // now read the regsiter from FRAM into cache
                        ourselected = ramselected;

                        // XMEM registers start at FRAM 0x1E000, this is HP register 0x200
                        // 8 bytes (64 bits) are used to store one register
                        // ourselected is between 0x200 - 0x3FF
                        fram_offset = XMEMstart + 8 * (ourselected - 0x200);

                        // and read from FRAM
                        // lower bits first
                        fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, fram_offset, (uint8_t*)&usermemCacheLo, 4);
                        fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, fram_offset + 4, (uint8_t*)&usermemCacheHi, 4);
                    }
                    else {
                        // not an existing register or not our register selected
                        ourselected = 0;
                    }
                    TraceLine.ramslct = ramselected;
                    // TraceLine.xq_data1 = ramselected;
                    break;

                case inst_PRPHSLCT:             // PRPHSLCT, select peripheral from DATA[2..0]
                    rx_inst_t = rx_inst; 
                    prphselected = (rx_data1 & 0x03FF);
                    // should do a check if the selected peripheral is ours and active (plugged)
                    // for now leave it, used only for the Wand in the test setup
                    break;

                case inst_WRITDATA:             // WRITDATA, must wait for DATA to be complete and then store
                                                // in register pointed to by ramselected
                    // TraceLine.xq_instr = rx_inst;    
                    rx_inst_t = rx_inst; 
                    if (ourselected > 0) {
                        // usermemLo[ourselected - 0x200] = rx_data1;
                        usermemCacheLo = rx_data1;   // read from cache
                        // TraceLine.xq_data1 = rx_data1;
                        write_pending = true;       // mark as pending to handle high bits when data arrives
                    }
                    break;
                                                                      
                default:
                    // TraceLine.xq_instr = 0;
                    // TraceLine.xq_data = 0;

                    // catch the HP-IL=C(0..7) instructions, copy C[0..1] to the HP-IL register
                    // instructions 0x200..0x3C0, bit pattern: 0b1nnn000000, with SYNC bit set: 0b111nnn000000 0xE00
                    
                    // if (HP82160A_active && ((rx_inst & 0xE3F) == 0xE00))
                    if (globsetting.get(HP82160A_enabled) && ((rx_inst & 0xE3F) == 0xE00))
                    // instruction to write C[0..1] to the selected IL register
                    {
                        rx_inst_t = rx_inst; 
                        IL_reg = (rx_inst & 0x1C0) >> 6;            // mask register and bring in position
                        HPIL_writereg(IL_reg, rx_data1 & 0xFF);     // takes care of sending if needed
                    }
                    break;
            }

            for (i = 0; i < 9; i++)
            {
                TraceLine.HPILregs[i] = HPIL_REG[i];
            }
           
            // gpio_pulse(P_DEBUG, 9);                                  // for debugging 
            pio_sm_put(pio0_pio, debugout_sm, DBG_OUT7);

            // at this time we can update the HP-IL flags
            // simply set the FI flags according to the HP-IL register
            // HP-IL uses the following flags:
            //  FI_06   0x07000000      // FI_IFCR, checked with ?IFCR, Interface Clear Received (on fi_out1)
            //  FI_07   0x70000000      // FI_SRQR, checked with ?SRQR, Service Request Received (on fi_out1)
            //  FI_08   0x00000007      // FI_FRAV, checked with ?FRAV, Frame Available  (on fi_out2)
            //  FI_09   0x00000070      // FI_FRNS, checked with ?FRNS, Frame Received Not as Sent (on fi_out2)
            //  FI_10   0x00000700      // FI_ORAV, checked with ?ORAV, Output Register Available (on fi_out2)

            if ((HPIL_REG[8] & 0x01) == 0x01)   // if FLGENB set in R1W
            {
                ctrlReg = HPIL_REG[1];          // flags are in R1R
                // HPIL Register 1 (write) bit 0 is the flag output enable
                // when it is 1 the flags are enabled
                if ((ctrlReg & 0x01) == 0x01) 
                {
                    fi_out2 = fi_out2 | FI_10;        // ORAV
                }
                else
                {
                    fi_out2 = fi_out2 & FI_10_off;
                }

                if ((ctrlReg & 0x02) == 0x02)
                {
                    fi_out2 = fi_out2 | FI_09;        // FRNS
                } 
                else
                {
                    fi_out2 = fi_out2 & FI_09_off;
                }
                
                if ((ctrlReg & 0x04) == 0x04)
                {
                    fi_out2 = fi_out2 | FI_08;        // FRAV
                } 
                else
                {
                    fi_out2 = fi_out2 & FI_08_off;
                }
                
                if ((ctrlReg & 0x08) == 0x08)
                {
                    fi_out1 = fi_out1 | FI_07;        // SRQR
                } 
                else
                {
                    fi_out1 = fi_out1 & FI_07_off;
                }
                
                if ((ctrlReg & 0x10) == 0x10)
                {
                    fi_out1 = fi_out1 | FI_06;        // IFCR
                } 
                else
                {
                    fi_out1 = fi_out1 & FI_06_off;
                }
            }
            else
            {
                // HP-IL flags are disabled, so clear the relevant flags
                fi_out1 = fi_out1 & FI_06_off;
                fi_out1 = fi_out1 & FI_07_off;
                fi_out2 = fi_out2 & FI_08_off;
                fi_out2 = fi_out2 & FI_09_off;
                fi_out2 = fi_out2 & FI_10_off;
            }

        } 
        else {
            // PWO is low, so HP41 is now in deep or light sleep
            // try to empty the buffer but do not block

            // also empty datain FIFO, best to reset state machine to also empty ISR
            
            if (!pio_sm_is_rx_fifo_empty(pio0_pio, sync_sm))
            {           
                // read data if RX FIFO is not empty
                rx_addr = pio_sm_get_blocking(pio0_pio, sync_sm);
            }
            else
            {
                rx_addr = 0;
            }
            // in any case, update the Trace buffer with whatever we have
            queue_try_add(&TraceBuffer, &TraceLine);                    // add to internal trace buffer for handling by core0   

            // gpio_pulse(P_DEBUG, 10);                                  // for debugging 
            pio_sm_put(pio0_pio, debugout_sm, DBG_OUT7);
        }        
    }
}                       // end of the core1 loop
