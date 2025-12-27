/*
 * peripherals.c
 *
 * This file is part of the TULIP4041 project.
 * Copyright (C) 2024 Meindert Kuipers
 * 
 * This file contains the higher level emulation of peripherals:
 *  - IR printing
 *  - HP-IL communication and PILBox emulation
 *  - IO definitions and initialization
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

#include "peripherals.h"

uint16_t PrintChar;     // character received from printbuffer
char* PrintLine[200];   // printbuffer to print single lines

uint16_t ir_code;
uint32_t ir_frame;

queue_t PrintBuffer;            // printbuffer between cores
const int PrintBufSize = 100;   // size of printbuffer

bool HPIL_firstconnect = false;
bool ILScope_firstconnect = false;
bool Print_firstconnect = false;
bool ilscope_enabled = true;


// HP-IL variables
const int HPIL_BufSize = 10;    // size of printbuffer for both sending and receiving

bool HPIL_closed    = false;        // indicates if the HP-IL loop is closed at the HP82160A
                                    // this is like the cables on the HP-IL module are connected together without any device
bool PILBox_closed  = false;        // indicates if the HP-IL loop is closed at the HP82160A
                                    // loop is closed by default on the real PILBox if it was not initialized
bool m_bLoopClosed  = true;         // loop is closed  
bool enable_AUTOIDY = false;        // if system is in AUTO IDY mode, default when power is applied

uint16_t PIL_rx_lo;                 // PILBox lo byte previously received   
uint16_t PIL_rx_hi;                 // PILBox hi byte previously received
uint16_t PIL_tx_lo;                 // PILBox lo byte previously sent   
uint16_t PIL_tx_hi;                 // PILBox hi byte previously sent
uint16_t PIL_rx_frame;              // PILBox frame just received
uint16_t PIL_rx_prevframe;          // PILBox previous frame received
uint16_t PIL_tx_prevframe;          // PILBox previous frame sent

uint16_t PIL_CMD_frame = 0;         // PILBox CMD frame last sent
uint16_t PIL_FRC_frame = 0;         // PILBox RFC frame last sent

bool PIL_rx_pending = false;        // true is a lo byte is read but not yet the hi byte
bool PIL_tx_pending = false;        // true if the hi byte still has to be sent
bool PILmode8 = true;               // PILBox transfer mode, true when in 8-bit mode, false when in 7-bit mode
uint16_t PILBox_mode = TDIS;        // PILBOx mode (TDIS, CON, COFF, COFI)
uint16_t PILBox_prevmode = 0;       // PILBOx mode to detect a change (TDIS, CON, COFF, COFI)

// enum MODE {eNone = 0, eController = 1 , eDevice = 2};
// enum MODE m_eMode = eNone;          // controller/device mode

#define m_eNone 0
#define m_eController 1
#define m_eDevice 2

int m_eMode = 0;


uint16_t m_wLastFrame;              // last sent frame
uint16_t m_wLastCmd;                // last CMD frame
uint16_t loopbackFrame = 0xFFFF;    // used for loopback frames for PILBox emulation      

volatile bool HPIL_timeout = false;
absolute_time_t t_IDY_timer;                // 10 ms timer for sending AUTO IDY frames

// forward declaration
void HPIL_SendFrame(uint16_t wFrame);

// for the HP-IL Scope
char  ILScopePrint[200];
int   ILScopePrintLen = 0;

struct ILScope_struct {
    int16_t ILcode;
    int16_t ILmask;
    const char* ILmnemonic;
} ; 


// const ILScope_struct __in_flash() IL_mnemonics[] = 
const ILScope_struct IL_mnemonics[] = 
// list of HP-IL mnemonics
// forced in FLASH to maximize SRAM
{   // opcode, mask, mnemonic
    {0x000, 0x700, "DAB"},      // DATA frame        // element #0
    {0x100, 0x700, "DSR"},
    {0x200, 0x700, "END"},      // End Byte
    {0x300, 0x700, "ESR"},
    {0x400, 0x7FF, "NUL"},      // NULL
    {0x401, 0x7FF, "GTL"},      // Go To Local
    {0x404, 0x7FF, "SDC"},      // Selected Device Clear
    {0x405, 0x7FF, "PPD"},      // Parellel Poll Disable
    {0x408, 0x7FF, "GET"},      // Group Execute Trigger
    {0x40F, 0x7FF, "ELN"},      // Enable Listener Not Readey For Data
    {0x410, 0x7FF, "NOP"},      // No Operation        // element #10
    {0x411, 0x7FF, "LLO"},      // Local Lock Out
    {0x414, 0x7FF, "DCL"},      // Device Clear
    {0x415, 0x7FF, "PPU"},      // Parallel Poll Unconfigure
    {0x418, 0x7FF, "EAR"},      // Enable Asynchronous Request
    {0x43F, 0x7FF, "UNL"},      // Unlisten
    {0x420, 0x7E0, "LAD"},      // Listener Address
    {0x45F, 0x7FF, "UNT"},      // Untalk
    {0x440, 0x7E0, "TAD"},      // Talker Address
    {0x460, 0x7E0, "SAD"},      // Seconday Address
    {0x480, 0x7F0, "PPE"},      // Parellel Poll Enable        // element #20
    {0x490, 0x7FF, "IFC"},      // Interface Clear
    {0x492, 0x7FF, "REN"},      // Remote Enable
    {0x493, 0x7FF, "NRE"},      // Not Remote Enable
    {0x494, 0x7FF, "*TDIS"},    // PILBox Translator Disable
    {0x495, 0x7FF, "*COFI"},    // PILBox Controller
    {0x496, 0x7FF, "*CON"},     // PILBox Controller On
    {0x497, 0x7FF, "*COFF"},    // PILBox Controller Off with IDY
    {0x49A, 0x7FF, "AAU"},      // Auto Address Unconfigure
    {0x49B, 0x7FF, "LPD"},      // Loop Power Down
    {0x4A0, 0x7E0, "DDL"},      // Device Dependent Listener Command         // element #30
    {0x4C0, 0x7E0, "DDT"},      // Device Dependent Talker Command
    {0x400, 0x700, "CMD"},      // All commands filter 0x400-0x4C0
    {0x500, 0x7FF, "RFC"},      // Ready For Command
    {0x540, 0x7FF, "ETO"},      // End Of Transmission, OK
    {0x541, 0x7FF, "ETE"},      // End Of Transmission, Error
    {0x542, 0x7FF, "NRD"},      // Not Ready For Data
    {0x560, 0x7FF, "SDA"},      // Send Data
    {0x561, 0x7FF, "SST"},      // SSP, Send Serial Poll ??
    {0x562, 0x7FF, "SDI"},      // Send Device ID
    {0x563, 0x7FF, "SAI"},      // Send Accessory ID        // element #40
    {0x564, 0x7FF, "TCT"},      // Take Control
    {0x580, 0x7E0, "AAD"},      // Auto Address 0-30
    {0x5A0, 0x7E0, "AEP"},      // Auto Extended Primary
    {0x5C0, 0x7E0, "AES"},      // Auto Extended Secondary
    {0x5E0, 0x7E0, "AMP"},      // Auto Multiple Primary
    {0x500, 0x700, "RDY"},      // Ready
    {0x600, 0x700, "IDY"},      // Identify
    {0x700, 0x700, "ISR"},      //                          //  element #48
    };



// General functions for GPIO control

// function to toggle a GPIO signal
void gpio_toggle(uint signal)
{
    gpio_put(signal, !gpio_get(signal));
}

// function to toggle a GPIO signal multiple times, mainly for debugging
void gpio_pulse(uint signal, uint numtimes)
{
    uint i;
    for (i = 0 ; i < numtimes ; i ++)
    {
       gpio_toggle(signal);
       gpio_toggle(signal);
    }    
} 


// initialize the UART0 on pins 0 and 1
void serialport_init()
{
    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);
    uart_set_hw_flow(UART_ID, false, false);

    // Set the TX and RX pins by using the function select on the GPIO
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Send out a string, with CR/LF conversions
    uart_puts(UART_ID, " UART initialized!\n");
    // Send out a character without any conversions
    // uart_putc_raw(UART_ID, 'A');

    // Send out a character but do CR/LF conversions
    // uart_putc(UART_ID, 'B');

}

void bus_init()
{

    gpio_init(ONBOARD_LED);

    gpio_set_dir(ONBOARD_LED, GPIO_OUT);
    gpio_put(ONBOARD_LED, 1);               // light the led

    //  HP41 inputs
    gpio_init(P_CLK1);
    gpio_set_dir(P_CLK1, GPIO_IN);
    gpio_init(P_CLK2);
    gpio_set_dir(P_CLK2, GPIO_IN);
    gpio_init(P_SYNC);
    gpio_set_dir(P_SYNC, GPIO_IN);
    gpio_init(P_ISA);
    gpio_set_dir(P_ISA, GPIO_IN);
    gpio_init(P_DATA);
    gpio_set_dir(P_DATA, GPIO_IN);
    gpio_init(P_PWO);
    gpio_set_dir(P_PWO, GPIO_IN);

    gpio_init(P_FI);                    // FI input on GPIO2, on DevBoard only!
    gpio_set_dir(P_FI, GPIO_IN);

    // helper outputs
    gpio_init(P_T0_TIME);
    gpio_set_dir(P_T0_TIME, GPIO_OUT);
    gpio_init(P_DEBUG);
    gpio_set_dir(P_DEBUG, GPIO_OUT);
    gpio_init(P_SYNC_TIME);
    gpio_set_dir(P_SYNC_TIME, GPIO_OUT);

    gpio_init(P_DATA_OE);
    gpio_set_dir(P_DATA_OE, GPIO_OUT);
    gpio_put(P_DATA_OE, 0);                 // set to low to disable output driver

    gpio_init(P_FI_OE);    
    gpio_set_dir(P_FI_OE, GPIO_OUT);
    // gpio_set_pulls(P_FI_OE, false, true);   // enable pullup, but not needed
    gpio_put(P_FI_OE, 0);                   // set to low to disable output driver

    gpio_init(P_ISA_OE);  
    gpio_set_dir(P_ISA_OE, GPIO_OUT);
    gpio_put(P_ISA, 0);                     // set to low to disable output driver

    gpio_init(P_IR_LED);  
    gpio_set_dir(P_IR_LED, GPIO_OUT);       // IR led and PWO_OE output driver
    gpio_set_drive_strength(P_IR_LED, GPIO_DRIVE_STRENGTH_12MA);
    gpio_put(P_IR_LED, 0);                  // set low

    gpio_init(PICO_VBUS_PIN);
    gpio_set_dir(PICO_VBUS_PIN, GPIO_IN);
    
    #if (TULIP_HARDWARE == T_MODULE)
        // PWO_OE is shared with SPARE1 on the module version
        gpio_init(P_PWO_OE);  
        gpio_set_dir(P_PWO_OE, GPIO_OUT);
        gpio_put(P_PWO_OE, 0);               // set to low to disable output driver
    #endif


    //prepare the IRQ for the PWO rising and falling edge
    gpio_set_irq_enabled_with_callback(P_PWO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &pwo_callback);
}

void WandBuffer_init()
{
    queue_init(&WandBuffer, sizeof(uint16_t), WandBufSize);
}


// functions for IR output

// Calculate Frame Payload with Parity
// from https://github.com/vogelchr/avr-redeye/blob/master/avr-redeye.c
   
   /*   And one frame consists of <start> 12*(<one>|<zero>), the order for
    the 12 payload bits in a frame is:

          d c b a 7 6 5 4 3 2 1 0

   7..0 are data, a,b,c,d are parity bits, so that, if the frame payload
   is written as a 12-bit integer the following subsets of bits must have
   a even numbers of bits set:

   +---+---+---+---++---+---+---+---++---+---+---+---++--------+
   : d : c : b : a :: 7 : 6 : 5 : 4 :: 3 : 2 : 1 : 0 ::   Hex  :
   +---+---+---+---++---+---+---+---++---+---+---+---++--------+
   : X :   :   :   ::   : X : X : X :: X :   :   :   :: 0x0878 :
   +---+---+---+---++---+---+---+---++---+---+---+---++--------+
   :   : X :   :   :: X : X : X :   ::   : X : X :   :: 0x04e6 :
   +---+---+---+---++---+---+---+---++---+---+---+---++--------+
   :   :   : X :   :: X : X :   : X ::   : X :   : X :: 0x02d5 :
   +---+---+---+---++---+---+---+---++---+---+---+---++--------+
   :   :   :   : X :: X :   :   :   :: X :   : X : X :: 0x018b :
   +---+---+---+---++---+---+---+---++---+---+---+---++--------+

   */

char parity(uint8_t byte)
{
	byte = byte ^ (byte >> 4); /* merge 7..4 -> 3..0 */
	byte = byte ^ (byte >> 2); /* merge 3..2 -> 1..0 */
	byte = byte ^ (byte >> 1); /* merge    1 ->    0 */
	return byte & 0x01;
}

uint16_t calculate_frame_payload(uint8_t data)
{
    // this payload has the msb as the first bit to be sent!
	int16_t frame = data;
	if(parity(data & 0x78)) frame |= 0x800;
	if(parity(data & 0xe6)) frame |= 0x400;
	if(parity(data & 0xd5)) frame |= 0x200;
	if(parity(data & 0x8b)) frame |= 0x100;
	return frame;
}


int32_t construct_frame(uint16_t data)
{
    // data contains a 12 bit value:
    //  d c b a 7 6 5 4 3 2 1 0          (dcba= checksum, 76543210 is the data payload)

    uint32_t frame = 0b111;                 // start bits           
                                            // frame = 0000.0111
    for (int i = 0; i < 12 ; i++)
    {
        // go through all 12 bits and construct new frame
        // a 0 bit expands to 10
        // a 1 bit expands to 01
        frame = frame << 2;                 
        if ((data & 0x800) == 0x800) {           
            // msb is set
            frame = frame | 0x2;              // OR with 0b10
        }                  
        else
        {
            // msb is not set
            frame = frame | 0x01;             // OR with 0b01
        }
        data = data << 1;
    }

    // frame now contains the following for character A for example:
    // 0000.0111.1010.0110.0110.0101.0101.0110
    //       ^^^ start bits
    //           ^^^........................^^  payload 24 bits
    // this must now be left aligned for proper sending, else the first 0's are sent
    // ensure in the state machine that only 24+3=27 bits are transitted !
    frame = frame << 5;
    return frame;
}

//  to send a frame, the bits need to be split into half-bits:
//      start frame :  3 hi-lo transitions
//      0-bit       :  1 hi-lo, 1 lo-lo 
//      1-bit       :  1 lo-lo, 1 hi-lo
//  the irout state machine sends the following:
//      input 0-bit :  send lo-lo frame
//      input 1-bit :  send hi-lo frame
//
// for sending a 1 bit: put 01 in the output frame (lsb sent first)
// for sending a 0 bit: put 10 in the output frame (lsb sent first)


void PrintBuffer_init()
{
    queue_init(&PrintBuffer, sizeof(uint16_t), PrintBufSize);
}


void Print_task()
{
    // first process the printbuffer
    // need to add something to distinguish between printer types or preferred output (IR or serial)
    // this routine is mainly for the HP82143 emulation and send the printcodes 
    // to both the serial port and the IR led

    // check for a first connection from the Printer CDC
    if (cdc_connected(ITF_PRINT)) {
        // only if the Print CDC interface is connected
        if (!Print_firstconnect) {
            // HPIL_firstconnect was false, so this is now a new CDC connection
            Print_firstconnect = true;
            cli_printf("  CDC Port 5 [printer] connected");
        }
    }

    // check for disconnection of Printer
    if ((Print_firstconnect) && (!cdc_connected(ITF_PRINT))) {
        // CDC interface is disconnected
        cli_printf("  CDC Port 5 [printer] disconnected");
        Print_firstconnect = false;
    }

    if (!queue_is_empty(&PrintBuffer)) {
        queue_remove_blocking(&PrintBuffer, &PrintChar);

        /*  code below for testing throttling of the printer
        ACA does not lead to a PRINTER ERROR, HP41 just hangs while PBUSY sends the carry
        PRA does lead to a PRINTER ERROR, probably after an EOL
        prev_level = level;
        level = queue_get_level(&PrintBuffer);
        if (level != prev_level) 
        {
            printf("level = %0d, prevlevel = %d\n", level, prev_level);
        }
        */

       /* printer monitoring to the console is disabled here
        if ((PrintChar == 0xE0) || (PrintChar == 0xE8)) {
            printf("\nPRINT: ");       
        }

        if (PrintChar > 0x01F) {
            printf("%c", PrintChar);
        }
            
        uart_putc_raw(UART_ID, PrintChar);             // Send the character to the serial port

        */

        if (cdc_connected(ITF_PRINT))
        {
            // only if connected
            // but this causes problems with the HP82240 simulator
            // the beta version of this works fine with the CDC_connected function
            cdc_send_printport(PrintChar);                 // send the character to the USB printport
        }

        // output to a real serial port if required
        // if ((PrintChar == 0xE0) || (PrintChar == 0xE8)) {
        //    uart_putc_raw(UART_ID, 13);             
        // }
            
        // send the printcharacter to the IR LED
        ir_code = calculate_frame_payload(PrintChar);
        ir_frame = construct_frame(ir_code);
        send_ir_frame(ir_frame);

        // line below for debugging the construction of the IR frame
        // printf("IR char = %02X, code = %04X, frame = %08X\n", PrintChar, ir_code, ir_frame);
    }
}

void PrintIRchar(uint8_t c)
{
    // send a character to the IR printer, for testing the IR LED
    ir_code = calculate_frame_payload(c);
    ir_frame = construct_frame(ir_code);
    send_ir_frame(ir_frame);
}

// HP-IL tasks and PIL-box emulation

// function to get the HP-IL menmonic from a given frame
void getIL_mnemonic(uint16_t wFrame, char *mnem)
{
    // char mnem[5];
    // first find the mnemonic
    int i = 0;
    while (i < numILmnemonics) {   
        if ((wFrame & IL_mnemonics[i].ILmask) == IL_mnemonics[i].ILcode) break;
        i++;
    }
    sprintf(mnem, "%s", IL_mnemonics[i].ILmnemonic);
}

void HPIL_scope(uint16_t wFrame, bool out, bool traceIDY)
{
    // HPIL tracer, shoing outgoing and incoming frames
    // output is sent to its own USB CDC port
    //  out         - when true this is outgoing traffic, otherwise it is incoming
    //  traceIDY    - when true will trace IDY frames
    uint16_t framemask;
    int i = 0;
    int stamp = cycles();

    if (ilscope_enabled && cdc_connected(ITF_ILSCOPE) && globsetting.get(ilscope_IL_enabled)) {
        // only do this if something is connected or enabled, otherwise just wasting cycles

        // first find the mnemonic
        i = 0;
        while (i < numILmnemonics) {   
            if ((wFrame & IL_mnemonics[i].ILmask) == IL_mnemonics[i].ILcode) break;
            i++;
        }

        // do nothing if it is an IDY frame and the tracing of these is disabled
        if (((wFrame & 0x700) == 0x600) && !traceIDY) return;   
    
        // build the ILScope string
        ILScopePrintLen = 0;
        ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen," %s %03X %s                           [%8d]\n\r",  out ? ">":"<", wFrame, IL_mnemonics[i].ILmnemonic, stamp);

        cdc_send_string(ITF_ILSCOPE, ILScopePrint, ILScopePrintLen);
        cdc_flush(ITF_ILSCOPE);
    } 
}

void PILBox_scope(uint16_t wFrame, uint8_t bhi, uint8_t blo, bool out)
{
    // PILBox tracer, showing outgoing and incoming frames and bytes
    // output is sent to the USB CDC port that is also showing the HP-IL scope
    //  out         - when true this is outgoing traffic, otherwise it is incoming
    //  traceIDY    - when true will trace IDY frames

    uint16_t framemask;
    int i = 0;
    int stamp = cycles();

    if (ilscope_enabled && cdc_connected(ITF_ILSCOPE) && globsetting.get(ilscope_PIL_enabled)) {
        // only do this if something is connected or enabled, otherwise just wasting cycles

        // first find the mnemonic
        i = 0;
        while (i < numILmnemonics) {   
            if ((wFrame & IL_mnemonics[i].ILmask) == IL_mnemonics[i].ILcode) break;
            i++;
        } 
    
        // build the ILScope string
        ILScopePrintLen = 0;
        ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen,"  PILBox %s %04X %s   ", out ? ">":"<", wFrame, IL_mnemonics[i].ILmnemonic);
        ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen,"hi: %02X lo: %02X  [%8d]\n\r", bhi, blo, stamp);

        cdc_send_string(ITF_ILSCOPE, ILScopePrint, ILScopePrintLen);
        cdc_flush(ITF_ILSCOPE);
    }
}

void HPIL_init()
{
    // initialize all HP-IL registers to their default state
    // initialize the HP-IL buffer to communicate with the core1 function

    // initialize send and receive buffer, HPIL frame is a 16-bit word
    queue_init(&HPIL_SendBuffer, sizeof(uint16_t), HPIL_BufSize);
    queue_init(&HPIL_RecvBuffer, sizeof(uint16_t), HPIL_BufSize);

    // preset HP-IL registers for HP-IL module hot plugging
    // ASSERT(m_pHpil != NULL);
    // memset(m_pHpil->HPIL_REG,0,sizeof(m_pHpil->HPIL_REG));

    HPIL_REG[0] = 0x81;    // SC=1 MCL=1
    HPIL_REG[1] = 0x00;    // SC=1 MCL=1
    HPIL_REG[2] = 0x00;    // SC=1 MCL=1
    HPIL_REG[3] = 0x00;    // SC=1 MCL=1
    HPIL_REG[7] = 0x00;    // SC=1 MCL=1
    HPIL_REG[8] = 0x00;    // SC=1 MCL=1

    // HP-IL scratch pad registers
    HPIL_REG[4] = 0x01;    // selected loop address
    HPIL_REG[5] = 0x01;    // current device number
    HPIL_REG[6] = 0x01;    // starting device number      

}


void PILBox_sendframe(uint16_t frame)
{
    // This is the sender of the PILBox emulation
    // sends the frame to the designated USB CDC port
    uint16_t outframe;

    loopbackFrame = 0xFFFF;

    if (!cdc_connected(ITF_HPIL) || (PILBox_mode == TDIS))
    {
        // PILBox disabled (TDIS) or no serial connection
        // defaults to loopback
        loopbackFrame = frame;
    }
    else
    {
        /* ignore RFC handling for now
        if ((frame & 0x700) == 0x400)
        {
            // this is a CMD frame, save it for later
            // when an RFC comes
            PIL_CMD_frame = frame;       // save it for later
            outframe = frame;
        } 
        // else if (((frame & 0x700) == 0x600) && (PILBox_mode == COFI))
        // {
            // this is an IDY frame, only forward in COFI mode
            // loopbackFrame = frame;  // ignore for testing

        // }
        else if (frame == RFC)
        {
            // RFC frame from HP-IL
            outframe = PIL_CMD_frame;              // send previous CMD frame
        }
        */
       outframe = frame;

        // PILBox emulator has an existing connection
        // send data to the CDC output

        // we send the full frame here
        // normally we can optimize traffic by not sending the hi byte if it is the same as the previous hi byte
        // with the high speed USB connection this is not an issue anymore
        // to be implemented later
        if (PILmode8)
        {
            // 8-bit transfer mode
            PIL_tx_lo = (outframe & 0x007F) | 0x80;             // lower 7 data bits, msb = 1
            PIL_tx_hi = ((outframe >> 6) & 0x1E) | 0x20;        // PILBox hi byte previously sent
            cdc_send_char(ITF_HPIL, PIL_tx_hi);                 // send both chars
            cdc_send_char_flush(ITF_HPIL, PIL_tx_lo);           // flush after the 2nd byte
        }
        else
        {
            // 7-bit transfer mode
            PIL_tx_lo = (outframe & 0x003F) | 0x40;             // lower 6 data bits, msb = 1
            PIL_tx_hi = ((outframe >> 6) & 0x1F) | 0x20;        // higher byte
            cdc_send_char(ITF_HPIL, PIL_tx_hi);                 // send both chars
            cdc_send_char_flush(ITF_HPIL, PIL_tx_lo);           // flush after the 2nd byte
        }     
    }
    PILBox_scope(outframe, PIL_tx_hi, PIL_tx_lo, true);
}

uint16_t PILBox_revcframe()
{
    // This is the receiving end of the PILBox emulation
    // cheks if a frame is available and returns the frame if it is
    // returns 0xFFFF if no frame is available
    // also handles the PILBox initialization frame

    // info below from Christoph Giesselink:
    // PILBox commands
    //  #define TDIS   0x494  // TDIS
    //  #define COFI   0x495  // COFF with IDY, firmware >= v1.6, not used if no IDY frames are supported
    //  #define CON    0x496  // Controller ON, not used, HP41 can only be controller (check HP-IL DEVELOPMENT ROM Scope function!)
    //  #define COFF   0x497  // Controller OFF
    //  #define SSRQ   0x49C  // Set Service Request, obsolete, not used on HP41
    //  #define CSRQ   0x49D  // Clear Service Request, obsolete, not used on HP41

    //  most we don't need when we limit the HP41 always be the controller (not supporting the Scope in the HP-IL DEVEL ROM).
    //  and we don't need COFI when we don't support IDY frames on the device side on first instance.
    //  Another special is the way to transfer 11 HP-IL bits over a byte oriented interface with an acknowlege when using 9600 baud.
    //  And finally the CMD/RFC frame handling on the controller side, it's important to know that the RFC frame is not transferred to the device side

    // frames are sent/received in the following format (ducument by JF Garnier):
    //  7-bit transfers:        001cccbb    hi byte: higher 5 bits (3 control and data bits 7 and 6)
    //                          01bbbbbb    lo byte: lower 6 data bits
    //  8-bit transfers:        001cccb0    hi byte: higher 4 bits (3 control bits and data bit 7);
    //                          1bbbbbbb    lo byte: lower 7 data bits
    //                          this allows quicker retransmit of ASCII data frames (control bits 000, only 7 data bits)
    //
    // To use the 7/8 bit flexibility:
    //      If a frame is to be transmitted to the serial link, check if the high byte is the same as the previous transmitted frame. 
    //      If so, it is not retransmitted and only the lo byte is sent.
    //      If the receiver gets only the low byte, it will use the last received hi byte to rebuild the frame
    //
    // For example, when sending the PILBox initialization, bytes 0x32 and 0x97 are sent for the frame 0x497 (COFF command):
    //      0x32 = 0b00110010 (hi byte), 0x97 = 0b10010111 (lo byte)
    //               001cccb0                     1bbbbbbb 

    // other considerations:
    //  When driven by a controller on the HP-IL side (which is the case with the HP41) IDY frames are locally retransmitted
    //  Also RFC frames are retransmitted and not sent to the computer
    //  All other frames are sent out and received frames are returned to the HP-IL process

    // CMD/RFC handshake
    // CMD/RFC handshake is managed by the PILBox. In DEVICE mode (COFF, COFI) the handsahe is done as follows:
    //      when PILBox receives a CMD fram from HP-IL it keeps a copy and transmits it to the host PC
    //      when an RFC is then received from HP-IL, the PILBox sends the previous CMD frame to the host
    //      when the CMD frame comes back from the host, the OILBox retransmits the RFC to HP-IL
    //  In CONTROLLER mode (CON) (not applicable to the HP41 for now), the handshake is done as follows:
    //      when the PILBox receives a CMD frame from HP-IL it keeps a copy and transmits RFC to HP-IL
    //      when the RFC comes back from HP-IL the previous CMD is sent to the serial link

    // uint16_t PIL_rx_lo;                 // PILBox lo byte previously received   
    // uint16_t PIL_rx_hi;                 // PILBox hi byte previously received
    // uint16_t PIL_tx_lo;                 // PILBox lo byte previously sent   
    // uint16_t PIL_tx_hi;                 // PILBox hi byte previously sent
    // uint16_t PIL_rx_prevframe;          // PILBox previous frame received
    // uint16_t PIL_tx_prevframe;          // PILBox previous frame sent
    // uint16_t PIL_CMD_frame;             // PILBox CMD frame last sent
    // uint16_t PIL_FRC_frame;             // PILBox RFC frame last sent
    // bool PIL_rx_pending = false;        // true is a lo byte is read but not yet the hi byte
    // bool PIL_tx_pending = false;        // true if the hi byte still has to be sent
    // bool PILmode8 = true;               // PILBox transfer mode, true when in 8-bit mode, false when in 7-bit mode

    uint16_t frame;
    int16_t pil_recv;
    bool returnframe = false;               // true if a frame needs to be returned
                                            // false if no data available or only part of a frame received
    
    if (!cdc_connected(ITF_HPIL))
    {
        // no valid serial link, loopback mode 
        frame = loopbackFrame;
        loopbackFrame = 0xFFFF;             // to indicate no new frame is available
        return frame;                       // and get out
    }
    else if (cdc_available(ITF_HPIL) == 0)
    {
        // no bytes available
        return 0xFFFF;                      // return no data and get out
    }
    else
    {
        // we get here when:
        // - there is a valid serial link
        // - and there is data available in the serial buffer
        // if a frame arrives we must check for a PILBox command first
        pil_recv = cdc_read_byte(ITF_HPIL);

        // PILBox emulation received a byte from the PILBox designated serial port
        // pil_recv contains the returned byte
        if ((pil_recv & 0xE0) == 0x20)
        {
            // this is the higher byte of a transfer
            PIL_tx_hi = pil_recv;       // save until the lower byte arrives
            return 0xFFFF;              // and return with no data
        }
        if ((pil_recv & 0x80) == 0x80)
        {
            // this is the lower byte of an 8-bit transfer
            PILmode8 = true;                    // set the correct mode to 8 bits
            PIL_rx_lo = pil_recv;               

            // this completes the 2-byte transfer, complete the frame
            PIL_rx_frame = (pil_recv & 0x7F) | ((PIL_tx_hi & 0x1E) << 6);
        }
         if ((pil_recv & 0xC0) == 0x40)
        {
            // this is the lower byte of a 7-bit transfer
            PILmode8 = false;            // set the correct mode
            PIL_rx_lo = pil_recv;       
              
            // this completes the 2-byte transfer, complete the frame
            PIL_rx_frame = (pil_recv & 0x3F) | ((PIL_tx_hi & 0x1F) << 6);
        }

        // The frame is now received, first process the PILBox commands
        // send to our scope for debugging
        PILBox_scope(PIL_rx_frame, PIL_tx_hi, pil_recv, false);

        switch (PIL_rx_frame)
        {
            case TDIS:                          // TDI: Translator DIsabled
                PILBox_mode = TDIS;             // set mode to disabled
                                                // frame is not forwarded to the HP-IL emulation
                cdc_send_char_flush(ITF_HPIL, pil_recv);       // return command for confirmation
                // return 0xFFFF;                  // and return with no data
                break;
            case CON:                           // CON: Controller ON
                PILBox_mode = CON;              // set mode to controller ON
                                                // default on the HP41
                                                // frame is not forwarded to the HP-IL emulation
                cdc_send_char_flush(ITF_HPIL, pil_recv);       // return command for confirmation
                return 0xFFFF;                  // and return with no data
                break;
            case COFF:                          // Controller OFF
                PILBox_mode = COFF;             // set mode to controller OFF
                                                // the PILBox is now a device
                                                // not used on the HP41
                                                // frame is not forwarded to the HP-IL emulation
                cdc_send_char_flush(ITF_HPIL, pil_recv);       // return command for confirmation
                return 0xFFFF;               // and return with no data
                break;
            case COFI:                          // Controller OFf with IDY 
                PILBox_mode = COFI;             // set mode to COFI
                                                // device with sending IDY frame
                                                // frame is not forwarded to the HP-IL emulation
                cdc_send_char_flush(ITF_HPIL, pil_recv);       // return command for confirmation
                return 0xFFFF;                  // and return with no data
                break;
            // default:

                // all other frames are sent on to the HP-IL loop
                // only need to check for a CMD frame
                // should do a check if the mode is TDIS, any traffic should be ignored
                // if (PIL_rx_frame == m_wLastFrame)
                // {
                    // this is a previous CMD frame, this is returned as an RFC frame
                //     PIL_rx_frame = RFC;
                // }
                // else if (PIL_rx_frame == RFC)
                // {
                //     PIL_rx_frame = m_wLastFrame;
                // }
        }
        // if we get here the frame is complete
        return PIL_rx_frame;
    }
}

/****************************/
// check for frame in transfer with 10s timeout (10*5780)
/****************************/

// probably do not need this, the time-out is handled in the HP-IL module itself
bool IsFrameInTransfer(uint16_t dwIncr)
{
  bool bInTransfer = (m_eMode != 0);
  if (bInTransfer)
  {
    // m_dwCpuCycles += dwIncr;
    // if (m_dwCpuCycles >= 10*5780)           // check for timeout
    // {
    //   bInTransfer = false;
    //   m_eMode = eNone;
    // }
  }
  return bInTransfer;
} 

// code for HP-IL loop timeout (10 seconds)
// probably do not need this, the time-out is handled in the HP-IL module itself
int64_t HPIL_timeout_callback(alarm_id_t id, void *user_data) {
    HPIL_timeout = true;

    return 0;
}


// code for sending IDY frames every 10ms when the HP41 is in light sleep
// this is not interrupt controller but done with a simple Timer
// routine must be called by the 
void HPIL_AutoIDYTask()
{
    absolute_time_t t_now;
    int32_t ms_elapsed;
    int64_t us_elapsed;
    // task to send AUTO IDY frames
    // this must be done every 10 ms when the HP41 is in light sleep, and if AUTO IDY is enabled
    // routine must be called by the HPIL_task
    // the following table is from Christoph Giesselink V41 source (hpil.cpp)
    //      light sleep, CA, AUTO IDY -> 10ms Timer, IDY frame
    //      0            0   0           0           0
    //      0            0   1           0           0
    //      0            1   0           0           0
    //      0            1   1           0           0
    //      1            0   0           0           0
    //      1            0   1           1           0
    //      1            1   0           0           0
    //      1            1   1           1           1

    // loop is entered on the following conditions:
    //   - powermode is Light Sleep (STANDBY)
    //   - AutoIDY is enabled (bit 6 in HP-IL register 3)
    //   - HP-IL is in Controller mode (CA bit, bit 6 in HP-IL register 0) 
    // loop checks if 10 ms are expired before sending AutoIDY frame 0x6C0

    if (((HPIL_REG[3] & 0x40) != 0) && (HP41_powermode == eLightSleep) && ((HPIL_REG[0] & 0x40) != 0))
    {
        t_now = get_absolute_time();
        us_elapsed = absolute_time_diff_us(t_IDY_timer, t_now);
        if (us_elapsed > (10*1000))
        {
            // 10 ms have passed so send a new IDY frame and reset the timer
            t_IDY_timer = t_now;
            HPIL_SendFrame(IDY_C0);   // frame to send is 0x6C0
        }
    }
}

void HPIL_SendFrame(uint16_t wFrame)
{
    // sends a frame to the HPIL bus
    // in this case the frame will be sent to the PILBox emulator
    HPIL_scope(wFrame, true, true);

    if (wFrame != RFC)              // not a RFC frame
    {
        m_wLastFrame = wFrame;      // remember last send frame
                                    // but not really used ??
    }

    PILBox_sendframe(wFrame);       // send to the PILBox emulation
}

void HPIL_RecvFrame(uint16_t wFrame)
{
    // receives a frame from the HPIL bus
    // in this case the frame is received from the PILBox emulator
    // modelled after functions in V41 (Christoph Giesselink)
    // for the HP-IL Scope
    HPIL_scope(wFrame, false, true);

    // CMD frame and CA (controller active) and adding RFC frame enabled
    // CMD/RFC handshaking is done in the PILBox emulation
    if (((wFrame & 0x700) == 0x400) && (HPIL_REG[0] & 0x40) != 0)
    {
        m_wLastCmd = wFrame;                            // remember last CMD frame
        HPIL_SendFrame(RFC);                            // send the RFC frame
        return;
    }

    // CA (controller active) and RFC frame
    // CMD/RFC handshaking done in the PILBox emulation
    if (((HPIL_REG[0] & 0x40) != 0) && (wFrame == RFC))
    {
        wFrame = m_wLastCmd;                            // use the last CMD frame as answer
    } 
        
    queue_try_add(&HPIL_RecvBuffer, &wFrame);           // and put the frame in the bufffer
}


void HPIL_task()
{
    uint16_t HPIL_sendframe;
    uint16_t HPIL_recvframe;
    // called constantly from the main loop in core0
    // handle all HP-IL communication and PIL-Box emulation
    //

    // tasks to be implemented:
    //  - check if a frame is ready to be sent by the HP41 (write to DATA OUTPUT register)
    //  - check if a frame was received from the bus (PILBox emulator) and process
    //  - update the flag register and the FI flags depending on the status (done in send/receive routines)
    //  - check for PILBox emulator handling needed (initialization, data received, data to be sent)
    //  - call the HPIL_AutoIDYTask function 

    // first of all check if HP-IL is enabled and active at all
    // if (!globsetting.get(HP82160A_enabled)) return;

    // check the status of the IL SCOPE, and if there is a first connect
    // also check for an input char to pause the scope
    if (cdc_connected(ITF_ILSCOPE)) {
        // only if the Tracer CDC interface is connected
        if (!ILScope_firstconnect) {
            // ILScope_firstconnect was false, so this is now a new CDC connection
            ILScope_firstconnect = true;
            ILScopePrintLen = 0;
            cli_printf("  CDC Port 4 [IL Scope] connected");
            ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen, "IL Scope CDC PORT connected\r\n");
            cdc_sendbuf(ITF_ILSCOPE, ILScopePrint, ILScopePrintLen);
            cdc_flush(ITF_ILSCOPE);
        } else {
            if (cdc_read_char(ITF_ILSCOPE) != 0) {
                ILScopePrintLen = 0;
                ilscope_enabled = !ilscope_enabled;
                ILScopePrintLen += sprintf(ILScopePrint + ILScopePrintLen, "IL Scope is %s\n\r", ilscope_enabled ? "enabled":"paused");
                cdc_sendbuf(ITF_ILSCOPE, ILScopePrint, ILScopePrintLen);
                cdc_flush(ITF_ILSCOPE);
            }
        }
    }

    // check for disconnection of IL Scope
    if ((ILScope_firstconnect) && (!cdc_connected(ITF_ILSCOPE))) {
        // CDC interface is disconnected
        cli_printf("  CDC Port 4 [IL Scope] disconnected");
        ILScope_firstconnect = false;
    }

    // check for a first connection from the HP-IL CDC
    if (cdc_connected(ITF_HPIL)) {
        // only if the HP-IL CDC interface is connected
        if (!HPIL_firstconnect) {
            // HPIL_firstconnect was false, so this is now a new CDC connection
            HPIL_firstconnect = true;
            cli_printf("  CDC Port 3 [HPIL] connected");
            if (PILBox_mode == TDIS || PILBox_mode == 0) {
                // show a warning that there is no HP-IL connection
                // only show a warning if there is no PilBox connection with 1 second?
                cli_printf("  WARNING: No virtual HP-IL device connected, HP-IL loop may be open");
            }
        }
    }

    // check for disconnection of HP-IL
    if ((HPIL_firstconnect) && (!cdc_connected(ITF_HPIL))) {
        // CDC interface is disconnected
        cli_printf("  CDC Port 3 [HPIL] disconnected");
        HPIL_firstconnect = false;
    }

    // check for a change in the PILBox mode and report this
    if (PILBox_mode != PILBox_prevmode) {
        // there is a change in the PILBox mode, report to the console
        switch(PILBox_mode) {
            case TDIS:  cli_printf("  PILBox mode changed to TDIS / disconnected            - HP-IL loop may be open!"); break;
            case CON :  cli_printf("  PILBox mode changed to CON  / Controller ON           - HP-IL loop is closed"); break;
            case COFF:  cli_printf("  PILBox mode changed to COFF / Controller OFF          - HP-IL loop is closed"); break;
            case COFI:  cli_printf("  PILBox mode changed to COFI / Controller OFF with IDY - HP-IL loop is closed"); break;
            default  :  cli_printf("  oops, unknown PILBox mode, try to re-connect or reboot the TULIP"); break;
        }
        PILBox_prevmode = PILBox_mode;
    }


    // HP-IL emulation is active, first check if a frame should be sent
    if (!queue_is_empty(&HPIL_SendBuffer))
    {
        // data in the queue, so send it out
        queue_remove_blocking(&HPIL_SendBuffer, &HPIL_sendframe);   // get frame from the queue
        HPIL_SendFrame(HPIL_sendframe);                             // and send it
    }

    // check if a frame is received by the PILBox

    if ((HPIL_recvframe = PILBox_revcframe()) != 0xFFFF ) {
        // frame is received, now process
        HPIL_RecvFrame(HPIL_recvframe);
    }

    if (HP41_powermode == eLightSleep)
    {
        // HP41 in Light Sleep, may need to send AutoIDY frames every 10ms
        // AUTO IDY bit is checked here
        // enable_AUTOIDY can be used to overrule the AUTOIDY bit
        if (enable_AUTOIDY) HPIL_AutoIDYTask();
    }
}

void Wand_task()
{
    // first process the printbuffer
    // need to add something to distinguish between printer types or preferred output (IR or serial)
    // this routine is mainly for the HP82143 emulation and send the printcodes 
    // to both the serial port and the IR led

    // check for a first connection from the Printer CDC
    if (cdc_connected(ITF_PRINT)) {
        // only if the Print CDC interface is connected
        if (!Print_firstconnect) {
            // HPIL_firstconnect was false, so this is now a new CDC connection
            Print_firstconnect = true;
            cli_printf("  CDC Port 5 [printer] connected");
        }
    }

    // check for disconnection of Printer
    if ((Print_firstconnect) && (!cdc_connected(ITF_PRINT))) {
        // CDC interface is disconnected
        cli_printf("  CDC Port 5 [printer] disconnected");
        Print_firstconnect = false;
    }

    if (!queue_is_empty(&PrintBuffer)) {
        queue_remove_blocking(&PrintBuffer, &PrintChar);

        /*  code below for testing throttling of the printer
        ACA does not lead to a PRINTER ERROR, HP41 just hangs while PBUSY sends the carry
        PRA does lead to a PRINTER ERROR, probably after an EOL
        prev_level = level;
        level = queue_get_level(&PrintBuffer);
        if (level != prev_level) 
        {
            printf("level = %0d, prevlevel = %d\n", level, prev_level);
        }
        */

       /* printer monitoring to the console is disabled here
        if ((PrintChar == 0xE0) || (PrintChar == 0xE8)) {
            printf("\nPRINT: ");       
        }

        if (PrintChar > 0x01F) {
            printf("%c", PrintChar);
        }
            
        uart_putc_raw(UART_ID, PrintChar);             // Send the character to the serial port

        */

        if (cdc_connected(ITF_PRINT))
        {
            // only if connected
            // but this causes problems with the HP82240 simulator
            // the beta version of this works fine with the CDC_connected function
            cdc_send_printport(PrintChar);                 // send the character to the USB printport
        }

        // output to a real serial port if required
        // if ((PrintChar == 0xE0) || (PrintChar == 0xE8)) {
        //    uart_putc_raw(UART_ID, 13);             
        // }
            
        // send the printcharacter to the IR LED
        ir_code = calculate_frame_payload(PrintChar);
        ir_frame = construct_frame(ir_code);
        send_ir_frame(ir_frame);

        // line below for debugging the construction of the IR frame
        // printf("IR char = %02X, code = %04X, frame = %08X\n", PrintChar, ir_code, ir_frame);
    }
}