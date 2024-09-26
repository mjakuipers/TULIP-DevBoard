/*
 * tracer.c
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

#include "tracer.h"



struct TLine TraceSample;                   // Trace Buffer definition, default (maximum info)

struct TLine_FI TraceSample_FI;             // Trace Buffer definition, no HP-IL , with FI
struct TLine_basic Tracesample_basic;       // basic Tracebuffer

uint32_t ISAsample;
uint64_t DATAsample;
uint32_t DATAsample1;
uint32_t DATAsample2;
uint32_t xq1 = 0;
uint32_t xq2 = 0;

static volatile uint32_t fi1 = 0;
static volatile uint32_t fi2 = 0;
static volatile uint64_t fi = 0;
 
uint32_t addr;
uint32_t instr, sinstr;
uint32_t delayed_dis;
uint32_t sync;
uint32_t cycle;
uint32_t cycle_prev;

uint16_t type_GOXQ;

char overflow = '.';
char prev_overflow = ' ';
bool sample_skipped = false;

uint32_t data0;
uint32_t data1;

uint32_t data_x;        // DATA exponent, 2 digits
uint32_t data_xs;       // DATA exponent sign, 1 digit
uint32_t data_m1;       // DATA mantissa, 5 digits, D12..D31
uint32_t data_m2;       // DATA mantissa, 5 digits, D28..D51
uint32_t data_s;        // DATA sign, D52..D55

int16_t activeSELP = -1;    // for disassembling peripheral instructions

bool Tracer_firstconnect = false;   // to detect the first connect to a CDC host
                                    // to display a welcome message

uint16_t ILframe_out = 0;
uint16_t ILframe_in = 0;
uint8_t HPIL_REG_copy[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
bool ILchanged = false;
uint16_t wframe;
char ILmnem[10];

// const int TRACELENGTH = 3000;       //Trace Buffer length

queue_t TraceBuffer;

char  TracePrint[250];
int   TracePrintLen = 0;

volatile int level;
volatile int prev_level;

int ALD_range_lo;
int ALD_range_hi;
bool ALD_block = false;
int test = 0;
bool blocking = false;

bool prev_block = false;
bool block = false;
    
// extern queue_t TraceBuffer;
extern struct TLine TraceLine;             // the variable with the TraceLine

extern int sample_break;


// initialize the trace buffer
void TraceBuffer_init()
{
    queue_init(&TraceBuffer, sizeof(TraceLine), TRACELENGTH);        // define trace buffer 
}

void HPIL_instr(uint16_t instr)
// disassemble HP-IL specific instruction for SELP = 0..7
// activeSELP indicates the selected register
{
    uint16_t i_type = instr & 0x003;         // isolate last two bits

    switch (i_type)
    {
    case 1:         // copy literal to selected HP-IL register
        TracePrintLen += sprintf(TracePrint + TracePrintLen, "  reg %d=%02X", activeSELP, (instr & 0x3FC) >> 2); 
        break;
    case 2:         // copies HP-IL register n to C[0..1]
        TracePrintLen += sprintf(TracePrint + TracePrintLen, "  C[0.1]=reg %d", (instr & 0x1C0) >> 6); 
        break;
    case 3:         // return control to CPU - ?PFSET
        TracePrintLen += sprintf(TracePrint + TracePrintLen, "  ?PFSET"); 
        break;
    default:
        TracePrintLen += sprintf(TracePrint + TracePrintLen, "  oops, unknown?"); 
        // should never get here
        break;
    }
}


// this is the main tracer function, called constantly from the main() loop in core0




void Trace_task()
{

    // check if a CDC port is connected, and if it was the first connection
    // to display a welcome message to identify the port

    if (cdc_connected(ITF_TRACE)) {
        // only if the Tracer CDC interface is connected
        if (!Tracer_firstconnect) {
            // Tracer_firstconnect was false, so this is now a new CDC connection
            Tracer_firstconnect = true;
            TracePrintLen = 0;
            cli_printf("  CDC Port 2 [tracer] connected");
            TracePrintLen += sprintf(TracePrint + TracePrintLen, "TRACER CDC PORT connected, trace is %s\n\r", trace_enabled ? "enabled":"disabled");
            cdc_sendbuf(ITF_TRACE, TracePrint, TracePrintLen);
            cdc_flush(ITF_TRACE);
        } else {
            if (cdc_read_char(ITF_TRACE) != 0) {
                TracePrintLen = 0;
                trace_enabled = !trace_enabled;
                globsetting.set(tracer_enabled, !globsetting.get(tracer_enabled));
                TracePrintLen += sprintf(TracePrint + TracePrintLen, "Trace is %s\n\r", trace_enabled ? "enabled":"disabled");
                cdc_sendbuf(ITF_TRACE, TracePrint, TracePrintLen);
                cdc_flush(ITF_TRACE);
            }
        }
    }
    else {
        // Tracer CDC not connected
        Tracer_firstconnect = false;
    }

    if (!queue_is_empty(&TraceBuffer)) {
        // only do something if there is something in the tracebuffer
        tud_task();             // must keep the USB port updated

        // read from the Trace Buffer
        queue_remove_blocking(&TraceBuffer, &TraceSample);

        if (!globsetting.get(tracer_enabled)) return;        // tracer is disabled
        // if (!trace_enabled) return;

        // we could get out here if there is no CDC port connected for the tracer
        // but we must keep emptying the buffer
        if (!cdc_connected(ITF_TRACE)) return;

        // now start analyzing the trace sample

        sinstr = TraceSample.isa_instruction;       // sync is still in here
        instr = sinstr & 0x03FF;
        sync  = TraceSample.isa_instruction >> 11;

        addr  = TraceSample.isa_address;
        cycle_prev = cycle;
        cycle = TraceSample.cycle_number;
        
        DATAsample1 = TraceSample.data1;        // D31..D00
        DATAsample2 = TraceSample.data2;        // D55..D32, right justified 0xfeffffff

        // handle FI signal
        fi1 = TraceSample.fi1;
        fi2 = TraceSample.fi2;                  // need to shift
        fi = fi2;
        fi = fi << 24;
        fi = fi +fi1;

        data_x  =  DATAsample1 & 0x000000FF;                // DATA exponent, 2 digits
        data_xs = (DATAsample1 & 0x00000F00) >>  8;         // DATA exponent sign, 1 digit
        data_m1 = (DATAsample1 & 0xFFFFF000) >> 12;         // DATA mantissa, 5 digits, D12..D31
        data_m2 =  DATAsample2 & 0x000FFFFF;                // DATA mantissa, 5 digits, D28..D51
        data_s  = (DATAsample2 & 0x00F00000) >> 20;         // DATA sign, D52..D55

        // a traceline starting with O marks a buffer overflow
        if (cycle != (cycle_prev + 1)) {
            overflow = 'O' ;
        } else {
            overflow = ' ';
        }

        // first check for all ranges to be filtered out

        // SYSTEM ROM, pages 0..5
        bool sysrom_trace = (addr >= 0x0000) && (addr < 0x6000);

        // IL ROMs, pages 6, 7
        bool ilrom_trace = (addr >= 0x6000) && (addr < 0x8000);

        // block some known system loops
        //   0x0098 - 0x00A1       RSTKB and RST05
        //   0x0177 - 0x0178       delay for debounce
        //   0x089C - 0x089D       BLINK01
        //   0x0E9A - 0x0E9E       NLT10 wait for key to NULL
        //   0x0EC9 - 0x0ECE       NULTST NULL timer
        bool sysloop_trace = ((addr >= 0x0098) && (addr <= 0x00A1)) || 
                             ((addr >= 0x0177) && (addr <= 0x0178)) || 
                             ((addr >= 0x089C) && (addr <= 0x089D)) ||
                             ((addr >= 0x0E9A) && (addr <= 0x0E9E)) ||
                             ((addr >= 0x0EC9) && (addr <= 0x0ECE));

        // now check if sample should be blocked 
        block = !globsetting.get(tracer_sysloop_on) && sysloop_trace;
        block |= block || !globsetting.get(tracer_sysrom_on) && sysrom_trace;
        block |= block || !globsetting.get(tracer_ilroms_on) && ilrom_trace;

        // if block is true then we have to block this sample, also set marker for previous block
        if (block) prev_block = true;

        if (!block) {
            // sample not to be blocked, now build the trace string 

            if (prev_block) {
                // there was a previous block, indicate skipped lines
                prev_block = false;
                overflow = '=' ;
            } 

            // build the trace/disassembly string
            TracePrintLen = 0;
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"%c  %6d  %04X  %01X  %03X  ", overflow, cycle, addr, sync, instr);
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"%01X.%05X%05X.%01X.%02X  ", data_s, data_m2, data_m1, data_xs, data_x);
            if (TraceSample.xq_instr == 0) {
                // print .... if no instruction was executed by TULIP
                TracePrintLen += sprintf(TracePrint + TracePrintLen,"...  ", TraceSample.xq_instr);  // instruction decoded by TULIP
            } else {
                // decoded instruction, print it
                TracePrintLen += sprintf(TracePrint + TracePrintLen,"%03X  ", TraceSample.xq_instr & 0x3FF);  // instruction decoded by TULIP
            }
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"R%03X  ", TraceSample.ramslct);
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"C%01X  ", TraceSample.xq_carry);    

            // work out the FI bits
            // if one of the bits is low, we count this as a valid flag
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"FI"); 
            for (int i = 0; i < 14; i++) {      // all 14 digits
                if (((fi >> (i * 4)) & 0b1111) != 0b1111 ) {
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"%01X", i); 
                } else {
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"-"); 
                }
            }

            // printf(" %08x", TraceSample.xq_data1) in case emulator internals are needed

            // keep track of the length of the disassembly line for alignment of any text after it
            int dis_len = TracePrintLen + 20;       // allow 20 chars for the disassembly text

            // disassembly of the traceline
            if (sync == 1) {
                // valid instruction
                if ((instr & 0x003) == 0x001) {     // class 1 instruction
                    // Class 1 is 2-word GO/XQ, so handle in the next disassembly line
                    delayed_dis = instr;
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  ..."); 
                } else 
                {
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  %s", mnemonics[instr]); 
                    delayed_dis = 0;
                    // isolate SELPn instruction, this is a CLASS 0 instruction
                    // bit pattern PPPPIIII00,. where I = 1001 (0x9) and P is the peripheral selected
                    // mask is 0b0000111111 (0x03F), test is 0b0000100100 (0x024)
                    if ((instr & 0x03F) == 0x024) 
                    {
                        // SELPF Found
                        activeSELP = (instr & 0x3C0) >> 6;
                    }
                    else activeSELP = -1;           // reset after any instruction with a SYNC
                }
            } else     
            {
                // no SYNC, so the 2nd word of a multi-byte instruction
                // or under peripheral control, or FETCH S&X
                if ((delayed_dis & 0x003) == 0x001)
                {
                    // class 1 instruction XQ/GO
                    type_GOXQ = instr & 0x003;
                    char goxq_str[10];
                    switch (type_GOXQ)
                    {
                        case 0x000: sprintf(goxq_str, "?NC XQ"); break;
                        case 0x001: sprintf(goxq_str, "?C XQ "); break;
                        case 0x002: sprintf(goxq_str, "?NC GO"); break;
                        case 0x003: sprintf(goxq_str, "?C GO ");  break;
                        default: break;
                    }
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  %s", goxq_str); 
                    TracePrintLen += sprintf(TracePrint + TracePrintLen," %02X%02X", (instr & 0x3FC) >>2, (delayed_dis & 0x3FC) >> 2);
                    delayed_dis = 0;
                } else {
                    // not a class 1 instruction so treat as literal
                    // could be from LDI, under peripheral control or FETCH S&X
                    // implement in disassembler (maybe) later
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  %03X", instr);
                    if ((activeSELP >= 0) && (activeSELP <= 7)) 
                    {
                        // there is an active peripheral, decode the literal
                        // for now for HP-IL
                        HPIL_instr(instr);
                    }
                }
            }

            // file the traceline with spaces until the disassembly text is 20 chars long
            // for alligment of the IL frame and registers after this
            //  1BA  C[0.1]=reg 6 is 17 chars   
            while (TracePrintLen < dis_len) {
                TracePrintLen += sprintf(TracePrint + TracePrintLen," ");
            }

            // add tracing for the HP-IL frames and registers if enabled and the HP-IL module is plugged            
            if (globsetting.get(tracer_ilregs_on) && globsetting.get(HPIL_plugged)) {
                // list the HP-IL registers and the frames
                // only list when a frame is sent or received
                wframe = TraceSample.frame_out;

                // find out the frame if any and print if there was an incoming or outgoing frame
                if ( wframe != 0xFFFF) {
                    // 0xFFFF is the value in the tracebuffer is nothing was sent
                    getIL_mnemonic(wframe, ILmnem);
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  IL> %03X %s", wframe, ILmnem); 
                } else {
                    wframe = TraceSample.frame_in;
                    if (ILframe_in != wframe) {
                        // packet received
                        getIL_mnemonic(wframe, ILmnem);
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"  IL< %03X %s", wframe, ILmnem); 
                        ILframe_in = wframe;
                    } else {
                        // no packet received of sent
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"             "); 
                    }
                }
                              
                // now print the HP-IL registers only if there was a change in one of the registers
                ILchanged = false;
                for (int i = 0; i < 9; i++) {
                    if (HPIL_REG_copy[i] != TraceSample.HPILregs[i]) {
                        ILchanged = true;
                    }
                }

                if (ILchanged) {
                     // only print the registers if there was a change
                    TracePrintLen += sprintf(TracePrint + TracePrintLen,"  Reg "); 
                    for (int i = 0; i < 9; i++) {   
                        TracePrintLen += sprintf(TracePrint + TracePrintLen,"%02X", TraceSample.HPILregs[i]);

                        // a changed register is indicated with a *
                        if (HPIL_REG_copy[i] != TraceSample.HPILregs[i]) {
                            TracePrintLen += sprintf(TracePrint + TracePrintLen,"* ");
                        } else {
                            TracePrintLen += sprintf(TracePrint + TracePrintLen,"  ");
                        }
                        HPIL_REG_copy[i] = TraceSample.HPILregs[i];
                    }
                }
            }

            // end of the traceline, finish it
            TracePrintLen += sprintf(TracePrint + TracePrintLen,"\n\r");

            if (cdc_connected(ITF_TRACE))
            // only send if something is connected, otherwise this will stall
            // better move this test to earlier in the tracer task for performance
            {
                cdc_sendbuf(ITF_TRACE, TracePrint, TracePrintLen);
                cdc_flush(ITF_TRACE);
            }
             
        } 
        else
        {
            // sample skipped
            sample_skipped = true;
        }
    }
}
