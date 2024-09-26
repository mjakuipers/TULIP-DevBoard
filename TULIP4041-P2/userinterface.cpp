// Created: 2018/02/04 12:24:41
// Last modified: 2024/09/17 14:42:46

#define Last_Modified "2024/09/17_19:56:54"
#define Version_String "*   VERSION 00.01.03 BETA - build time: %s"

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

#include "userinterface.h"


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

int8_t ROMBuffer[0x1FFF];           // array for download buffer
uint16_t *rom_buf = (uint16_t *) ROMBuffer;  // same, for 16-bit access

int rom_target = -1;                // target ROM page for FLASH or FRAM programming
typedef enum {                      // memory type for ROM programming
  P_NONE,                           // no programming possible
  P_FLASH,                          // program in FLASH
  P_FRAM                            // program in FRAM
} prog_target;

prog_target prog_t = P_NONE;

extern struct TLine TraceLine;      // the variable with the TraceLine

bool enable_programming = false;    // enable FLASH/FRAM programming

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
    "Bankswitching enabled",                    // 10   bankswitch instructions enabled  
    "Expanded Memory enabled",                  // 11   Expandend Memory enabled (MAXX emulation)
    "Tiny41 Instructions enabled",              // 12   enable decoding of TT specific instructions for device control
    "All QROM write protected",                 // 13   Write Protect all QROM (same effact as #8 ??)

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



// CLI Welcome message and status
void uif_status()
{
    float speed = clock_get_hz(clk_sys)/1000000;
    float totalheap = getTotalHeap() / 1024;
    float freeheap = getFreeHeap() / 1024;
    float tracebytes = (sizeof(TraceLine) * TRACELENGTH) / 1024;

    cli_printf("\n****************************************************************************");
    cli_printf("*");   
    cli_printf("*   Welcome to TULIP4041 - The ULtimate Intelligent Peripheral for the HP41 ");
    cli_printf("*");
    cli_printf(Version_String, Last_Modified);
    cli_printf("*");
    cli_printf("*   Running at : %7.2lf MHz", speed);
    cli_printf("*   Total heap : %7.2lf KBytes", totalheap);
    cli_printf("*   Free heap  : %7.2lf KBytes", freeheap);
    cli_printf("*   Tracebuffer: %7.2lf KBytes, %d samples = %d bytes/traceline", tracebytes, TRACELENGTH, sizeof(TraceLine));
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


void uif_reboot()
{
  cli_printf("  RESETTING THE TULIP4041 in 2 seconds !! press any key to cancel");
  watchdog_enable(2000, 1);   // set the watchdog to 2 seconds
  while(true)
  {
    tud_task();                 // to process IO until the watchdog triggers

    // check for any input to disable the timer
    if (cdc_available(ITF_CONSOLE))
    {
      // stop the watchdog
      hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
      cli_printf("  reset cancelled");
      return;
    }
  }
}


void uif_bootsel()
{
  uint sleepcount = 500;

  cli_printf("  RESETTING THE TULIP4041 RP2350 to BOOTSEL mode in 2 seconds!! press any key to cancel");

  while(true)
  {
    tud_task();                 // to process IO until the watchdog triggers
    sleep_ms(2);
    if (cdc_available(ITF_CONSOLE))
    {
      cli_printf("  reset cancelled");
      return;
    }
    sleepcount--;
    if (sleepcount == 0)
    {  
    // reboots the RP2350 when the counter expires, uses the standard LED for activity monitoring
      reset_usb_boot(1<<PICO_DEFAULT_LED_PIN, 0);      
    }
  }
}

// blink the LED b times, just for testing and fun
void uif_blink(int b)
{
  while (b > 0) 
  {
    gpio_toggle(PICO_DEFAULT_LED_PIN);
    sleep_ms(250);
    gpio_toggle(PICO_DEFAULT_LED_PIN);
    sleep_ms(250);
    b--;
  }
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
  cli_printf("  forcing PWO low to reset HP41");
  shutdown_41();
}

void uif_poweron() {
  // can only be done if PWO is low!
  if (uif_pwo_low()) {
    cli_printf("  driving ISA to power on HP41");
    wakemeup_41();
  }
}

void uif_configinit() {
  // can only be done if PWO is low!
    if (uif_pwo_low()) {
      cli_printf("  re-initialized all peristent settings");
      globsetting.set_default();
    }
}

void uif_configlist() {
  // can only be done if PWO is low!
    if (uif_pwo_low()) {

      for (int i = 0; i <= gsettings_lastitem; i++) {
        cli_printf("  item# %2d  value %04X - %s", i, globsetting.get(i), glob_set_[i]);
      }
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


// import a file and program in FLASH
void uif_import(int F, char *fname)       
{

}

// list the pluggable ROM and MOD files in FLASH and FRAM
void uif_list()
{
  // list the pluggable ROMs in FLASH and FRAM
  // format
  // [name] [ROM name] [XROM] [FLASH/FRAM/EMBED] [page]

  if (globsetting.get(ILPRINTER_plugged)) {
      cli_printf("  HP-IL Printer     Page 6"); 
  } else {
      cli_printf("  HP-IL Printer     Page -");
  }

  if (globsetting.get(HP82143A_enabled)) {
      cli_printf("  HP82143 Printer   Page 6"); 
  } else {
      cli_printf("  HP82143 Printer   Page -");
  }

  if (globsetting.get(HPIL_plugged)) {
      cli_printf("  HP-IL module      Page 7"); 
  } else {
      cli_printf("  HP-IL module      Page -");
  }

}


// plug and enable the selected ROM
//    0 - list current ROMs
//    1 - plug HP-IL
//    2 - plug IL Printer
//    3 - plug HP82143A printer (unplug ilprinter)
void uif_plug(int i)          // plug the selected ROM
{
  if (!uif_pwo_low()) return;    // only do this when calc is not running
  switch (i)
  {
    case 1:         // list ROMs
            uif_list();
            break;

    case 2: // plug HP-IL module
            globsetting.set(HPIL_plugged, 1);
            globsetting.set(HP82160A_enabled, 1);
            cli_printf("  plugged HP-IL module");
            break;
    case 3: // plug HP-IL Printer module
            // also plugs HP-IL module, unplugs printer
            globsetting.set(HPIL_plugged, 1);
            globsetting.set(ILPRINTER_plugged, 1);
            globsetting.set(HP82160A_enabled, 1);
            globsetting.set(HP82143A_enabled, 0);
            cli_printf("  plugged HP-IL and HP-IL Printer module");
            break;
    case 4: // plug HP82143A printer
            // must unplug IL Printer
            globsetting.set(HP82143A_enabled, 1);
            globsetting.set(ILPRINTER_plugged, 0);
            cli_printf("  plugged HP82143A Printer module");
            break;    
    default: 
            // no other actions defined here
            break;
  }

  // and update the settings in FRAM
  globsetting.save();
}

// unplug and disable the selected ROM
//    1 - list current ROMs
//    2 - unplug HP-IL (and IL Printer)
//    3 - unplug IL Printer
//    4 - unplug HP82143A printer 
void uif_unplug(int i)          // plug the selected ROM
{
  if (!uif_pwo_low()) return;    // only do this when calc is not running
  switch (i)
  {
    case 1:         // list ROMs
            uif_list();
            break;

    case 2: // unplug HP-IL module
            // also unplug HP-IL printer and disable HP-IL
            globsetting.set(HPIL_plugged, 0);
            globsetting.set(ILPRINTER_plugged, 0);
            globsetting.set(HP82160A_enabled, 0);
            cli_printf("  unplugged HP-IL and HP-IL Printer module");
            break;
    case 3: // unplug HP-IL Printer module
            // also plugs HP-IL module
            globsetting.set(ILPRINTER_plugged, 0);
            cli_printf("  unplugged HP-IL Printer module");
            break;
    case 4: // unplug HP82143A printer
            globsetting.set(HP82143A_enabled, 0);
            cli_printf("  unplugged HP82143A Printer module");
            break;    
    default: 
            // no other actions defined here
            break;
  }

  // and update the settings in FRAM
  globsetting.save();
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
              case 3:  cli_printf("  printer mode : TRACE *"); break;
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

// functions for the bus tracer
//
//  1        status        shows the tracer status
//  2        trace         toggle tracer enable/disable
//  3        sysloop       toggle tracing of system loops
//  4        sysrom        toggle system rom tracing (Page 0, 1, 2, 3, 5)
//  5        ilrom         toggle tracing of Page 6+7
//  6        hpil          toggle HP-IL tracing to ILSCOPE
//  7        pilbox        toggle PILBox serial tracing to ILSCOPE
//  8        ilregs        toggle tracing of HP-Il registers

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

void uif_tracer(int i) {
  uint16_t stat;

  switch (i) {
    case 1: // status
            cli_printf("  HP41 tracer         %s", globsetting.get(tracer_enabled) ? "enabled ":"disabled");
            cli_printf("  system loop tracing %s (RSTKB, RST05, BLINK01 and debounce)", globsetting.get(tracer_sysloop_on) ? "enabled ":"disabled");
            cli_printf("  system ROM tracing  %s (Page 0..5)", globsetting.get(tracer_sysrom_on) ? "enabled ":"disabled");
            cli_printf("  tracing of IL roms  %s (Page 6+7)", globsetting.get(tracer_ilroms_on) ? "enabled ":"disabled");
            cli_printf("  IL scope traffic    %s", globsetting.get(ilscope_IL_enabled) ? "enabled ":"disabled");
            cli_printf("  PILBox traffic      %s", globsetting.get(ilscope_PIL_enabled) ? "enabled ":"disabled");
            cli_printf("  tracing of IL regs  %s", globsetting.get(tracer_ilregs_on) ? "enabled ":"disabled");
            break;
    case 2: // trace
            globsetting.set(tracer_enabled, !globsetting.get(tracer_enabled));
            trace_enabled != tracer_enabled;
            cli_printf("  HP41 tracer         %s", globsetting.get(tracer_enabled) ? "enabled ":"disabled");
            break;            
    case 3: // sysloop
            globsetting.set(tracer_sysloop_on, !globsetting.get(tracer_sysloop_on));
            cli_printf("  system loop tracing %s (RSTKB, RST05, BLINK01 and debounce)", globsetting.get(tracer_sysloop_on) ? "enabled ":"disabled");
            break; 
    case 4: // sysrom
            globsetting.set(tracer_sysrom_on, !globsetting.get(tracer_sysrom_on));
            cli_printf("  system ROM tracing  %s (Page 0..5)", globsetting.get(tracer_sysrom_on) ? "enabled ":"disabled");
            break; 
    case 5: // ilrom
            globsetting.set(tracer_ilroms_on, !globsetting.get(tracer_ilroms_on));
            cli_printf("  tracing of IL roms  %s (Page 6+7)", globsetting.get(tracer_ilroms_on) ? "enabled ":"disabled");
            break; 
    case 6: // hpil scope
            globsetting.set(ilscope_IL_enabled, !globsetting.get(ilscope_IL_enabled));
            cli_printf("  IL scope traffic    %s", globsetting.get(ilscope_IL_enabled) ? "enabled ":"disabled");
            break; 
    case 7: // PILBox scope
            globsetting.set(ilscope_PIL_enabled, !globsetting.get(ilscope_PIL_enabled));
            cli_printf("  PILBox traffic      %s", globsetting.get(ilscope_PIL_enabled) ? "enabled ":"disabled");
            break;  
    case 8: // ilregs
            globsetting.set(tracer_ilregs_on, !globsetting.get(tracer_ilregs_on));
            cli_printf("  tracing of IL regs  %s", globsetting.get(tracer_ilregs_on) ? "enabled ":"disabled");
            break; 
    case 9: // save
            globsetting.save();
            cli_printf("  tracer setting saved in FRAM");
            break; 
    default:
            // no other actions defined here
            ;         
  }          
}        




//************************************************************ */
// Below are the functions for the 'old' user interface

void welcome()
{
    printf("\n****************************************************************************");
    printf("\n*   Welcome to TULIP4041 - The ULtimate Intelligent Peripheral for the HP41 ");
    printf("\n*");
    // printf("\n*   VERSION 00.01.02 BETA");
    printf("\n*   VERSION 00.01.02 BETA - build time: %s", Last_Modified);
    printf("\n*   Total heap:  %d bytes", getTotalHeap());
    printf("\n*   Free heap:   %d bytes", getFreeHeap());
    printf("\n    Tracebuffer: %d samples / %d bytes", TRACELENGTH, sizeof(TraceLine) * TRACELENGTH);
    printf("\n*   running at:  %d kHz\n", clock_get_hz(clk_sys)/1000);
    printf("\n****************************************************************************\n");

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

inline uint16_t swap16(uint16_t b)
{
  return __builtin_bswap16(b);
}

void file_download()
{
  int key;
  int n = 0;     // character counter
  // start dowload into ROMBuffer

  printf("F - file download - start sending the file now\n");

  while ((n < 0x2000) && (key >= 0)) 
  {
    // loop to read bytes 
    while ((key = getchar_timeout_us(100)) < 0) {}
    ROMBuffer[n] = key;
    n++;
    if ((n % 0x40) == 0) { printf("."); }
  }

  // correct download
  printf("\n download finished %d bytes downloaded\n", n);

  // swap the bytes
  for (int i =0; i < 0x1000; i++)
  {
    rom_buf[i] = swap16(rom_buf[i]);
  }

  // and show the results for the first 8 lines
  for (int i = 0; i < 31; i++) {
    printf("\n ROM %04X - ", i * 16); 
    for (int m = 0; m < 16; m++) {
      printf("%04X ", rom_buf[i * 16 + m]);        
    }
  }
  printf("\n");
}

void rom_f()
// ROM subcomands prompt
{
  printf("\nr_\b");
  state = ST_ROM;
}

void rom_hex()
// hex listing of current ROM, only first and last 8 lines
{
  printf("h - hex dump, short\n");
  for (int i = 0; i < 8; i++) {
    printf("\n ROM %04X - ", i * 16); 
    for (int m = 0; m < 16; m++) {
      printf("%04X ", rom_buf[i * 16 + m]);        
    }
  }

  printf("\n");

  for (int i = 0xF8; i < 0x100 ; i++) {
    printf("\n ROM %04X - ", i * 16); 
    for (int m = 0; m < 16; m++) {
      printf("%04X ", rom_buf[i * 16 + m]);        
    }
  }

  printf("\n");
}

void rom_hexl()
// hex listing of current ROM, all lines
{
  printf("H - hex dump, complete ROM\n");
  for (int i = 0x00; i < 0x100 ; i++) {
    printf("\n ROM %04X - ", i * 16); 
    for (int m = 0; m < 16; m++) {
      printf("%04X ", rom_buf[i * 16 + m]);        
    }
  }
  printf("\n");
}

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
}


 
void rom_check()
// print ROM checksum
{
  printf("c - calculate checksum\n");
  printf(" calculated checksum = %03X\n", rom_checksum);

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
}

// ROM summary structure
typedef struct
{
  int rom_xrom;
  int rom_funcs;
  int rom_firstfunc;
  char *rom_name;
  char *rom_revision;
  int rom_checksum;
  int calc_checksum;
} ROM_SUMMARY;

ROM_SUMMARY r_summary;
char ROMname[16]; // 16 chars will do normally
char ROMrev[6];
const char HPchar[] = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ |\"#$%&`()*+{-}/0123456789†,<=>?"; 

void fill_summary()
{

  char c;

  r_summary.rom_xrom = rom_buf[0];
  r_summary.rom_funcs = rom_buf[1];
  r_summary.rom_checksum = rom_buf[0xFFF];
  r_summary.calc_checksum = rom_checksum();
  r_summary.rom_firstfunc = (rom_buf[3] & 0xFF) + ((rom_buf[2] & 0xFF) << 8);  // pointer to the name]

  if (rom_buf[2] > 0xFF) 
  {
    sprintf(ROMname, "<USERCODE>");
  }
  else
  {
    c = rom_buf[r_summary.rom_firstfunc -1];
    int i = 0;
    while ( ((c & 0xFF) < 0x40) && (i < 16)) {
      // only allowed chars
      ROMname[i] = HPchar[c & 0x3F];
      i++;
      c = rom_buf[r_summary.rom_firstfunc - 1 - i];
     }
     // and add the last character of the ROM name plus a NULL
     ROMname[i] = HPchar[c & 0x3F];
     ROMname[i+1] = 0;
  }
  ROMrev[0] = HPchar[rom_buf[0xFFE] & 0x3F];
  ROMrev[1] = HPchar[rom_buf[0xFFD] & 0x3F];
  ROMrev[2] = '-';
  ROMrev[3] = HPchar[rom_buf[0xFFC] & 0x3F];
  ROMrev[4] = HPchar[rom_buf[0xFFB] & 0x3F];
  ROMrev[5] = 0;

}


void rom_name()
// show ROM name
{

  fill_summary();
  printf("n - show ROM name and other characteristics\n");
  printf("\n XROM        : %d 0x%02X", r_summary.rom_xrom, r_summary.rom_xrom);
  printf("\n # functions : %d 0x%02X", r_summary.rom_funcs, r_summary.rom_funcs);
  printf("\n ROM name    : '%s'   @ 0x%04X", ROMname, r_summary.rom_firstfunc);
  printf("\n ROM rev     : %s", ROMrev);
  printf("\n checksum    : %03X - calculated: %03X", r_summary.rom_checksum, r_summary.calc_checksum);
  printf("\n");
}

void rom_list()
// show ROM's in FLASH and/or FRAM
{
  printf("l - list available ROMs in FLASH and/or FRAM\n");

  printf(" memory | page | XROM | revision | name");

  // first list the FLASH pages
  for (int page = 0; page < 10; page++) {
    printf("\n  FLASH |  %02d  |  ", page);
    if (flash_contents[page * 0x1000] == 0xFFFF) {
      // page is empty
      printf("    |         | EMPTY");
    }
    else
    {
      // valid content, so download and list contents
      for (int i = 0; i < 0x1000; i++)
      {
        rom_buf[i] = flash_contents[page * 0x1000 + i];
      }
      fill_summary();
      printf("%02d  |  %s  | %s", r_summary.rom_xrom, ROMrev, ROMname);
    }
  }

  // list the FRAM pages
  for (int page = 0; page < 10; page++) {
    printf("\n  FRAM  |  %02d  |  ", page);

    fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, page * 0x2000, (uint8_t*)rom_buf, 0x2000);  
    if (rom_buf[0] == 0xFFFF)
    {
     printf("    |         | EMPTY");
    }
    else 
    {
      fill_summary();
      printf("%02d  |  %s  | %s", r_summary.rom_xrom, ROMrev, ROMname);
    }
  }

  printf("\n");

}


// #define ROM_BASE_OFFSET  0x00080000
// const uint16_t *flash_contents = (const uint16_t *) (XIP_BASE + ROM_BASE_OFFSET);


void rom_program()
// program ROM in Flash or SRAM according to settings
{
  printf("P - program ROM in FLASH or FRAM\n");

  if (((prog_t == P_NONE) || (rom_target < 0)) && !enable_programming)
  {
    // no programming enabled
    enable_programming = false;
    printf("ROM programming disabled, choose target FLASH or FRAM first or valid target Page!\n");   
  } 
  else
  {
    printf(" start programming");
    switch(prog_t) {
      case P_FLASH:
        printf(" FLASH - Page %d at FLASH address %08X\n", rom_target, XIP_BASE + ROM_BASE_OFFSET + (rom_target * 0x2000));
        sleep_ms(2000);               // needs some rest to prevent USB going crazy
        // first erase FLASH Page
        flash_range_erase(ROM_BASE_OFFSET + (rom_target * 0x2000), 0x2000);
        flash_range_program(ROM_BASE_OFFSET + (rom_target * 0x2000), (uint8_t *)rom_buf, 0x2000);
        break;
      case P_FRAM:
        printf(" FRAM - Page %d at FRAM address %06X\n", rom_target, rom_target * 0x2000 );
        fram_write(SPI_PORT_FRAM, PIN_SPI0_CS, (rom_target * 0x2000), (uint8_t*)rom_buf, 0x2000);
        break;
    default:
      break;
    }
  }
  printf("\n programming disabled, enable again with <rE> for programming a new ROM!\n");
}


void rom_set_fl()
// set target to FLASH for ROM programming
{
  printf("f - set target for programming ROM to FLASH\n");
  prog_t = P_FLASH;
  printf("ROM programming mode set to FLASH");
  if (rom_target < 0) {
    printf(" - do not forget to set ROM target Page");
  }
  else
  {
    printf(" - ROM target Page = %d\n", rom_target);
  }
}

void rom_set_fr()
// set target to FLASH for ROM programming
{
  printf("r - set target for programming ROM to FRAM\n");
  prog_t = P_FRAM;
  printf("ROM programming mode set to FRAM");
  if (rom_target < 0) {
    printf(" - do not forget to set ROM target Page");
  }
  else
  {
    printf(" - ROM target Page = %d\n", rom_target);
  }
}

void rom_pr_en()
// enable programming into FLASH or FRAM
{
  printf("E - Enable programming ROM in FLASH or FRAM\n");
  printf("enable again after programming to program a new ROM !\n");

  if ((prog_t == P_NONE) || (rom_target < 0))
  {
    // no programming enabled
    enable_programming = false;
    printf("ROM programming NOT enabled, choose target FLASH or FRAM first or valid target Page!");   
  } 
  else
  {
    switch(prog_t) {
      case P_FLASH:
        enable_programming = true;
        printf("ROM programming enabled, use with caution. Programming target is FLASH - Page %d", rom_target);
        break;
      case P_FRAM:
        enable_programming = true;
        printf("ROM programming enabled, use with caution. Programming target is FRAM - Page %d", rom_target);
        break;
    default:
      break;
    }
  }
  printf("\n WARNING: THE HP41 MUST BE OFF OR AT LEAST IN LIGHT SLEEP FOR FLASH and FRAM PROGRAMMING \n");
}

void rom_down()
{

  printf("d - Download ROM from FLASH or FRAM\n");

  if ((prog_t == P_NONE) || (rom_target < 0))
  {
    // no valid target page selected
    printf("Sorry, please choose target FLASH or FRAM first or valid target Page!\n");   
  } 
  else
  {
    switch(prog_t) {
      case P_FLASH:
        printf("downloading from FLASH - Page %d\n", rom_target);
        for (int i = 0; i < 0x1000; i++)
        {
          rom_buf[i] = flash_contents[rom_target * 0x1000 + i];
        }
        break;
      case P_FRAM:
        printf("downloading from FRAM - Page %d\n", rom_target);
        // read the complete ROM in one go
        fram_read(SPI_PORT_FRAM, PIN_SPI0_CS, rom_target * 0x2000, (uint8_t*)rom_buf, 0x2000);            
        break;
    default:
      break;
    }
  }
  printf("\n");

}

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

  printf("TUP4041 will NOT go to sleep\n");
  sleep_ms(1000);
  // sleep_goto_dormant_until_pin(P_PWO, true, true);
  
  // (uint gpio_pin, bool edge, bool high)
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

// supported commands for the user interface, main level
SERIAL_COMMAND serial_cmds[] = {
  { 'h', serial_help,     "Serial command help"  },
  { '?', serial_help,     "Serial command help"  },
  { 'd', toggle_disasm,   "Toggle disassembler"  },
  { 't', toggle_trace,    "Toggle trace"  },
  { 'o', toggle_traceo,   "Toggle system ROM trace"  },
  { 'b', set_break,       "Set breakpoint after n samples (for loooong traces)"  },
  { 'P', power_on,        "Power On - drive ISA for 20 us"  },
  { 'p', print_f,         "Printer functions subcommands"},
  // { 'x', xmem_set,      "Extended Memory Modules (0/1/2), 3 prints XMEM contents"},  //  void xmem_set(int i)
  { 'f', file_f,          "File functions (download ROM file)"},
  { 'r', rom_f,           "ROM functions subcommands"},
  { 'w', welcome,         "show welcome message"  },
  { 'i', pio_welcome,     "show PIO status"},
  // { 'W', wandcode,      "send one of the barcode test commands"},
  { 's', sdcard_test,     "uSD card test (list directory)"},
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
  // { 'O', print_on,      "toggle printer ON/OFF" },
  { 'b', print_busy,    "toggle printer BUSY" },
  { 'i', print_idle,    "toggle printer IDLE" },
  { 'e', print_empty,   "toggle printer Buffer Empty" },
  { 'm', print_man,     "set to MAN mode"},
  { 't', print_trace,   "set to TRACE mode"},        
  { 'n', print_norm,    "set to NORM mode"},
  { 'a', print_adv,     "push ADV button (short push only)"},
  { 'p', print_print,   "push PRINT button (short push only)"},
  { 'l', print_bat,     "toggle printer LOW BAT status"},
  { 'o', print_paper,   "toggle Out Of Paper"},
  { 's', print_status,  "show printer status"},
};

// supported subcommands for the user interface, ROM sub-level
ROM_COMMAND rom_commands[] = {
  { 'h', rom_help,    "ROM commands help" },
  { '?', rom_help,    "ROM commands help" },  
  { 'm', rom_mapping, "toggle default ROM mapping" },  
  { 'o', rom_off,     "toggle ROM emulation" },
  { 'x', rom_hex,     "hex listing of current ROM, only 5 first and 5 last lines" },
  { 'X', rom_hexl,    "hex listing of current ROM, all lines"},
  { 'c', rom_check,   "calculate ROM checksum" },
  { 'n', rom_name,    "show ROM name"},
  { 'l', rom_list,    "show ROM's in FLASH and/or FRAM"},
  { 'd', rom_down,    "download selected ROM to current ROM"},
  { 'P', rom_program, "program ROM in Flash or SRAM according to settings"},
  { 'f', rom_set_fl,  "set target to FLASH for ROM programming"},
  { 'r', rom_set_fr,  "set target to FRAM for ROM programming"},
  { 'E', rom_pr_en,   "enable programming into FLASH or FRAM"},
  { '*', rom_help,    "input any digit 0..9 to set the FLASH/FRAM target page"},
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
  printf("h - help for ROM sunfunctions\n");
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
              file_download();
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
              for (int i = 0; i < phelpSize; i++) {
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
