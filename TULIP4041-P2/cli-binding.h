/*
 * cli-binding.c
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

#ifndef INC_CLI_BINDING_H_
#define INC_CLI_BINDING_H_

#ifdef __cplusplus 
extern "C" { 
#endif 

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "embedded_cli.h"
#include "cdc_helper.h"
#include "hpinterface_hardware.h"
#include "hardware/gpio.h"

// help text for the system command
#if TULIP_HARDWARE == T_DEVBOARD
#define SYSTEM_HELP_TXT "TULIP4041 system status and control\r\n\
        [no argument] shows system status\r\n\
        status        shows system status\r\n\
        pio           shows pio status\r\n\
        cdc           shows status of CDC interfaces\r\n\
        cdcident      identify all CDC interfaces\r\n\
                      DO NOT USE if a virtual HP-IL device is connected! \r\n\
        REBOOT        restart the TULIP4041 system\r\n\
        BOOTSEL       restarts the TULIP4041 into BOOTSEL mode for firmware update\r\n\
        poweron       drive ISA for 20 usecs to switch HP41 on\r\n\
        calcreset     drive PWO to reset HP41\r\n\
        configinit    re-initialize the FRAM persistent settings configuration\r\n\
        configlist    list all configuration settings\r\n\
        gpio          show the GPIO status\r\n"

#elif TULIP_HARDWARE == T_MODULE
        #define SYSTEM_HELP_TXT "TULIP4041 system status and control\r\n\
        [no argument] shows system status\r\n\
        status        shows system status\r\n\
        pio           shows pio status\r\n\
        cdc           shows status of CDC interfaces\r\n\
        cdcident      identify all CDC interfaces\r\n\
                      DO NOT USE if a virtual HP-IL device is connected! \r\n\
        REBOOT        restart the TULIP4041 system\r\n\
        BOOTSEL       restarts the TULIP4041 into BOOTSEL mode for firmware update\r\n\
        poweron       drive ISA for 20 usecs to switch HP41 on\r\n\
        calcreset     drive PWO to reset HP41\r\n\
        configinit    re-initialize the FRAM persistent settings configuration\r\n\
        configlist    list all configuration settings\r\n\
        gpio          show the GPIO status\r\n\
        serial        show/program the TULIP serial number\r\n\
                      the serial number is programmed in OTP and can be programmed only once\r\n\
                      TULIP module gets its serial number during assembly testing\r\n"
#endif

        #define help_status     1
        #define help_pio        2
        #define help_cdc        3
        #define help_cdcident   4
        #define help_reboot     5
        #define help_bootsel    6
        #define help_poweron    7
        #define help_calcreset  8
        #define help_configinit 9
        #define help_configlist 10
        #define help_gpio       11
        #define help_serial     12

#define SDCARD_HELP_TXT "uSD card functions\r\n\
        [no argument] shows the uSD card status and mounts the card\r\n\
        status        shows the uSD card status and mounts the card\r\n\
        mount         mounts the uSD card\r\n\
        unmount       unmounts the uSD card\r\n\
        mounted       check if the uSD card is mounted\r\n\
        connect       connect the uSD card as USB drive to the host computer\r\n\
        eject         disconnect the SD card as USB drive from the host computer\r\n"

        #define sdcard_status   1
        #define sdcard_mount    2
        #define sdcard_unmount  3
        #define sdcard_mounted  4
        #define sdcard_connect  5
        #define sdcard_eject    6

#define PLUG_HELP_TXT "plug functions\r\n\
        hpil          plugs the embedded HP-IL ROM in Page 7 and enables emulation\r\n\
        ilprinter     plugs the embedded HP-IL Printer ROM in Page 6\r\n\
        printer       plugs the embedded HP82143A Printer ROM in Page 6 and enables emulation\r\n\
                      [filenm] can be a .ROM or .MOD file present in FLASH\r\n\
        [filenm] P    plug the named ROM in Page P (hex) and Bank 1\r\n\
        [filenm] P B  plug the named ROM in Page P (hex) and Bank B (1..4)\r\n\
        [filenm]      ROM file: no Page number will autoplug and find a free Page from 8..F\r\n\
                      MOD file: will attempt to plug according to the MOD file parameters\r\n\
        [filenm] T    Autoplug Test only, will not plug for real\r\n\
                      just to check where the ROM/MOD will be plugged\r\n"

        #define plug_hpil       1
        #define plug_ilprinter  2
        #define plug_printer    3
        #define plug_file_X     4   // file in specific Page
        #define plug_file_A     5   // Autoplug file in first free Page
        #define plug_file_T     6   // Autoplug Test only

#define UNPLUG_HELP_TXT "unplug functions\r\n\
        P             unplug the ROM in Page P (hex), including reserved Pages\r\n\
                      unplug all Banks of that Page if no Bank is given\r\n\
        P B           unplug the ROM in Page P and Bank B\r\n\
        all           unplug all plugged ROMs except reserved Pages\r\n\
        ALL           unplug all plugged ROMs including reserved Pages\r\n"

        #define unplug_all     1   // unplug all plugged ROMs except reserved Pages
        #define unplug_ALL     2   // unplug all plugged ROMs including reserved Pages
        // all other values 4..F are a valid Page number to unplug


#define PRINTER_HELP_TXT "printer functions for the HP82143\r\n\
        [no argument] shows the HP82143A status\r\n\
        status        shows the HP82143A status\r\n\
        power         toggle power\r\n\
        trace         enable trace mode\r\n\
        norm          enable norm mode\r\n\
        man           enable man mode\r\n\
        paper         toggle Out Of Paper status\r\n\
        print         push PRINT button\r\n\
        adv           push ADV button\r\n\
        irtest        test the infrared LED\r\n\
        irtog         toggle infrared LED\r\n"

        #define printer_status  1
        #define printer_power   2
        #define printer_trace   3
        #define printer_norm    4
        #define printer_man     5
        #define printer_paper   6
        #define printer_print   7
        #define printer_adv     8
        #define printer_irtest  9
        #define printer_irtog   10


#define TRACER_HELP_TXT "tracer functions\r\n\
        [no argument] shows the tracer status\r\n\
        status        shows the tracer status\r\n\
        buffer        shows the trace buffer size\r\n\
        buffer <size> set the tracer buffer size in number of samples\r\n\
                      default is 5000, maximum is about 10.000 samples\r\n\
                      requires a REBOOT to take effect!\r\n\
        pretrig      shows the pre-trigger buffer size and status\r\n\
        pretrig <size> set the pre-trigger buffer size in number of samples\r\n\
                      default is 32, maximum is 256 samples\r\n\
        trace         toggle tracer enable/disable\r\n\
        sysloop       toggle tracing of system loops (RSTKB, RST05, BLINK01 and debounce)\r\n\
        sysrom        toggle system rom tracing (Page 0 - 5)\r\n\
        ilrom         toggle tracing of Page 6 + 7\r\n\
        hpil          toggle HP-IL tracing to ILSCOPE USB serial port\r\n\
        pilbox        toggle PILBox serial tracing to ILSCOPE USB serial port\r\n\
        ilregs        toggle tracing of HP-IL registers\r\n"
/*
        trig              toggle the trigger enable\r\n\
                          blocks all samples until a trigger is found\r\n\
        trig list         list all triggers\r\n\
        trig + s XXXX     add trigger start at address XXXX (hex)\r\n\
        trig + e XXXX     add trigger end at address XXXX (hex)\r\n\
        trig - XXXX       remove the trigger at address XXXX (hex)\r\n\
        trig count NNN    set the post trigger counter to NNN (dec) samples\r\n\
        filter block      toggle the filter BLOCK enable\r\n\
        filter pass       toggle the filter PASS enable\r\n\
        filter list       list all filters\r\n\
        pass + Px         add a PASS filter for Page x\r\n\
        pass - Px         remove a PASS filter for Page x\r\n\
        pass + XXXX YYYY  add a PASS filter for the range XXX..YYYY (hex)\r\n\
        pass - XXXX YYYY  remove a PASS filter the range XXXX..YYYY (hex)\r\n\
        pass all          remove all BLOCK and PASS filters and PASS all trace samples\r\n\
        block + Px        add a BLOCK filter for Page x\r\n\
        block - Px        remove a BLOCK filter for Page x\r\n\
        block + XXXX YYYY add a BLOCK filter for the range XXXX..YYY (hex)\r\n\
        block - XXXX YYYY remove a BLOCK filter for the range XXXX..YYY (hex)\r\n\
        block all         remove all BLOCK and PASS filters and BLOCK all trace samples\r\n\
        save              save tracer settings\r\n"
*/
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
        #define trace_trig       12
        #define trace_pass       13
        #define trace_block      14


        /*  functions for later implemntation:
        block [no arg] show block entries\r\n\
        block [a1] [a2] block tracing of range between a1 and a2 (hex 0000-FFFF)\r\n\
        block [n]     toggle tracing of designated block entry, n=0..15\r\n\
        block [Pn]    block Page n (n= hex 0..F)\r\n\
        block del [n] delete block entry [n]\r\n\
        pass [no arg] show pass entries\r\n\
        pass [a1] [a2] pass only tracing of range between a1 and a2 (hex addresses 0000-FFFF)\r\n\
        pass [n]      toggle tracing of designated pass entry, n=0..15\r\n\
        pass [Pn]     pass only tracing of Page n (n=hex 0..F)\r\n\
        pass del [n]  delete pass entry [n]\r\n\
        */

#define XMEM_HELP_TXT "Extended Memory functions\r\n\
        status        shows the Extended Memory status\r\n\
        dump          creates a dump of Extended Memory\r\n\
        PATTERN       programs a pattern for FRAM test\r\n\
        ERASE         erase all Extended Memory\r\n\
        [n]           n= 0, 1, 2 \r\n\
                      plugs 0, 1 or 2 Extended Memory Modules\r\n\
                      do not use in combination with physical Extended Memory modules\r\n\
                      this software version does NOT have the Extended Function ROM built in\r\n\
                      Extended Functions must be physically plugged or available in an HP41CX\r\n"

        #define xmem_status     1
        #define xmem_dump       2
        #define xmem_pattern    3
        #define xmem_erase      4

#define FLASH_HELP_TXT "FLASH test functions\r\n\
        DANGER: the FLASH functions are for development testing only!!!\r\n\
        [no argument] shows the FLASH status\r\n\
        status        shows the FLASH status\r\n\
        dump [ADDR]   creates a dump of FLASH\r\n\
                      [ADDR] in hex is the start address for the dump\r\n\
                      always lists 4K bytes\r\n\
                      subsequent use of dump without [ADDR] lists the next 4K\r\n\
        INIT          initializes the FLASH file system\r\n\
        NUKEALL       erases all FLASH pages\r\n\
        fram [ADDR]   creates a dump of FRAM\r\n"

        #define flash_status    1
        #define flash_dump      2
        #define flash_init      3
        #define flash_nukeall   4
        #define flash_fram      5

#define FRAM_HELP_TXT "FRAM test functions\r\n\
        DANGER: the FRAM functions are for development testing only!!!\r\n\
        [no argument] shows the FRAM status\r\n\
        status        shows the FRAM status\r\n\
        dump [ADDR]   creates a dump of FRAM\r\n\
                      [ADDR] in hex is the start address for the dump\r\n\
                      always lists 4K bytes\r\n\
                      subsequent use of dump without [ADDR] lists the next 4K\r\n\
        INIT          initializes the FRAM file system\r\n\
                      limited to the ROM MAP portion of the FRAM\r\n\
        NUKEALL       erases all FRAM to zero\r\n"

        #define fram_status    1
        #define fram_dump      2
        #define fram_init      3
        #define fram_nukeall   4


#define IMPORT_HELP_TXT "import functions\r\n\
        [filename]                   import a single file to FLASH\r\n\
        [directory] [ALL]            import all files in a directory to FLASH\r\n"

        /*
        [filename]  [compare]        compare a single file with FLASH\r\n\
        [filename]  [UPDATE]         update a single file in FLASH\r\n\

        [directory] [compare]        compare all files in a directory with FLASH\r\n\
        [directory] [UPDATE]         update all files in a directory in FLASH\r\n\"

        /*
        [filename]  [FRAM]           import a single file in FRAM\r\n\
        [filename]  [compare] <FRAM> compare a single file with the one in FLASH (or FRAM)\r\n\
        [directory] [ALL] [compare]  compare all files in a directory with the ones in FLASH\r\n\
        [directory] [ALL] [UPDATE]   update all files in a directory in FLASH\r\n"
        */

#define DELETE_HELP_TXT "delete function, removes a file from FLASH\r\n\
        [filename]    delete the named file from FLASH\r\n\
                      only marks the file for deletion\r\n"

#define LIST_HELP_TXT "list functions\r\n\
        [no argument] lists all files\r\n\
        <filename>    show details of named file\r\n\
        ext           extended listing with more details per file for all files\r\n\
        all           include erased files in the listing\r\n"

#define CAT_HELP_TXT "cat functions\r\n\
        [no argument] shows the status of the plugged ROMs\r\n\
        <P> <B>       shows details of the ROM plugged in the Page (P=4..F hex)\r\n\
                      if B is given, it shows the Bank (1..4) of that Page\r\n\
                      with a dump of that complete ROM\r\n"


#define emulate_status    1
#define emulate_hpil      2
#define emulate_printer   3
#define emulate_zeprom    4
#define emulate_xmem      5
#define emulate_blinky    6
#define emulate_timer     7


#define EMULATE_HELP_TXT "enable/disable hardware emulation functions\r\n\
        toggles the emulation status. Do not enable if the real hardware is used!\r\n\
        [no argument] shows the status of the emulated hardware\r\n\
        status        shows the status of the emulated hardware\r\n\
        hpil          toggle HPIL hardware emulation\r\n\
        printer       toggle HP82143A printer emulation\r\n\
        zeprom P      toggle ZEPROM emulation in Page P (hex) for sticky bankswitching\r\n\
                      P is the Page number in hex (0..F)\r\n"

#define RESERVE_HELP_TXT "reserve a Page for a physical module\r\n\
        [P] <comment> reserve a physical module in Page P\r\n\
                      X is the Page number in hex (0..F)\r\n\
                      comment is a descriptive text for the module\r\n\
                      use no spaces or enclose in quotes\r\n\
        cx            reserve Page 3 and 5 for the HP41CX\r\n\
        timer         reserve Page 5 for the HP41 timer module\r\n\
        printer       reserve Page 6 for a printer\r\n\
        hpil          reserve Page 7 for the HP-IL module\r\n\
        clear [P]     cancel reservation for Page P (in hex)\r\n\
        clear all     cancel all reservations\r\n"

#define reserve_cx      1
#define reserve_timer   2       
#define reserve_printer 3
#define reserve_hpil    4
#define reserve_clear   5
#define reserve_page    6   


#define rtc_status      1
#define rtc_set         2
#define rtc_get         3    
#define rtc_reset       4
#define rtc_dump        5
#define rtc_display     6

#define RTC_HELP_TXT "RTC test functions\r\n\
        [no argument]     shows the RTC status\r\n\
        status            shows the RTC status\r\n\
        set YYMMDDHHMMSS  set the RTC to the given date and time\r\n\
        get               get the current date and time from the RTC\r\n\
        reset             reset the RTC\r\n\
        dump              dump the RTC registers\r\n"


// list all functions in userinterface.cpp called from cli-bindings here:

// all system functions
  extern void uif_status();             // system []/ system status
  extern void uif_pio_status();         // system pio
  extern void uif_cdc_status();         // system cdc
  extern void uif_cdc_ident();          // system ident
  extern void uif_reboot();             // system REBOOT
  extern void uif_bootsel();            // system BOOTSEL
  extern void uif_blink(int b);         // blink [n]
  extern void uif_calcreset();          // drive PWO to reset the HP41
  extern void uif_poweron();            // drive ISA to powerup the HP41
  extern void uif_configinit();         // reinitialize peristent settings
  extern void uif_configlist();         // list all settings
  extern void uif_serial(const char *str);    // show/program the TULIP serial number
  extern void uif_gpio_status();        // show the GPIO status

// all dir functions
  extern void uif_dir(const char *dir);                // dir root

// all sdcard functions
  extern void uif_sdcard_status();      // uSD card status
  extern void uif_sdcard_mount();       // mount the uSD card
  extern void uif_sdcard_unmount();     // unmount the uSD card
  extern void uif_sdcard_mounted();     // check if card is mounted
  extern void uif_sdcard_connect();     // connect the SD card as a USB drive
  extern void uif_sdcard_eject();       // eject the SD card as USB drive

// import function, program a file to FLASH or FRAM
  extern void uif_import(const char *fname, int a2, int a3);       // import a file and program in FLASH
  extern void uif_delete(const char *fname);  // delete a file from FLASH

// ROM plug and unplug functions
  extern void uif_plug(int func, int Page, int Bank, const char *fname);          // plug the selected ROM  
  extern void uif_unplug(int i, int bk);        // unplug the selected ROM
  extern void uif_cat(int p, int b);    // show the plugged ROMs
  extern void uif_reserve(int i, int p, const char *comment);     // reserve a Page for a module

  extern void uif_printer(int i);       // function for the HP82143A printer

  extern void uif_xmem(int i);          // functions for Extended Memory control

  extern void uif_tracer(int i, int bufsize);      // functions for the bus tracer

  extern void uif_flash(int i, uint32_t addr);   // functions for the FLASH test
  extern void uif_fram(int i, uint32_t addr);    // functions for the FRAM test

  extern void uif_list(int i, const char *fname);    // list

  extern void uif_rtc(int i, const char *args);    // RTC test functions

  extern void uif_emulate(int i, int p);        // enable/disable hardware emulation functions

// extern void uif_trace_mode(int m);    // trace [mode]



/**
 * Definition of the cli_printf() buffer size.
 * Can make smaller to decrease RAM usage,
 * make larger to be able to print longer strings.
 */
// #define CLI_PRINT_BUFFER_SIZE 255
#define CLI_PRINT_BUFFER_SIZE 500

// function that send one char to the output
void writeCharToCLI(EmbeddedCli *embeddedCli, char c);

/**
 * Clears the whole terminal.
 */
void onClearCLI(EmbeddedCli *cli, char *args, void *context);

/**
 * Example callback function, that also parses 2 arguments,
 * and has an 'incorrect usage' output.
 */
void onLed(EmbeddedCli *cli, char *args, void *context);

/**
 * Function to bind command bindings you'd like to be added at setup of the CLI.
 */
void initCliBinding();

// called from main core0 loop
void runCLI();

// function to encapsulate the embeddedCliPrint function
int cli_printf(const char *format, ...);

// function to encapsulate the embeddedCliPrintN function, no linefeed
int cli_printfn(const char *format, ...);


#ifdef __cplusplus 
} 
#endif 

#endif // INC_CLI_BINDING_H_