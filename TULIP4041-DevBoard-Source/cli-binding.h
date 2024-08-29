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

// help text for the system command
#define SYSTEM_HELP_TXT "TULIP4041 system status and control\r\n\
        [no argument] shows system status\r\n\
        status        shows system status\r\n\
        pio           shows pio status\r\n\
        cdc           shows status of CDC interfaces\r\n\
        cdcident      identify all CDC interfaces\r\n\
        REBOOT        restart the TULIP4041 system\r\n\
        BOOTSEL       restarts the TULIP4041 into BOOTSEL mode for firmware update\r\n\
        poweron       drive ISA for 20 usecs to switch HP41 on\r\n\
        calcreset     drive PWO to reset HP41\r\n"

#define SDCARD_HELP_TXT "SD card functions\r\n\
        [no argument] shows the SD card status and mounts the card\r\n\
        status        shows the SD card status and mounts the card\r\n\
        mount         mounts the SD card\r\n\
        unmount       unmounts the SD card\r\n\
        mounted       check if the drive is mounted\r\n\
        connect       connect the SD card as USB drive to the host computer\r\n\
        eject         disconnect the SD card as USB drive from the host computer\r\n"

#define PLUG_HELP_TXT "plug functions\r\n\
        [no argument] shows the current plugged ROMs\r\n\
        status        shows the current plugged ROMs\r\n\
        hpil          plug the HP-IL module in Page 7\r\n\
        ilprinter     plug the HP-IL Printer in Page 6 (unplug HP82143A printer)\r\n\
        printer       plug the HP82143A printer in Page 6 (unplug HP-IL printer)\r\n"        

#define UNPLUG_HELP_TXT "plug functions\r\n\
        [no argument] shows the current plugged ROMs\r\n\
        status        shows the current plugged ROMs\r\n\
        hpil          unplug the HP-IL module from Page 7, unplug the HP-IL Printer from Page 6\r\n\
        ilprinter     unplug the HP-IL Printer from Page 6\r\n\
        printer       unplug the HP82143A printer from Page 6\r\n"    

#define PRINTER_HELP_TXT "printer functions for the HP82143\r\n\
        [no argument] shows the HP82143A status\r\n\
        status        shows the HP82143A status\r\n\
        power         toggle power\r\n\
        trace         enable trace mode\r\n\
        norm          enable norm mode\r\n\
        man           enable man mode\r\n\
        paper         toggle Out Of Paper status\r\n\
        print         push PRINT button\r\n\
        adv           push ADV button\r\n"

#define TRACER_HELP_TXT "tracer functions\r\n\
        [no argument] shows the tracer status\r\n\
        status        shows the tracer status\r\n\
        trace         toggle tracer enable/disable\r\n\
        sysloop       toggle tracing of system loops (RSTKB, RST05, BLINK01 and debounce)\r\n\
        sysrom        toggle system rom tracing (Page 0, 1, 2, 3, 5)\r\n\
        ilrom         toggle tracing of Page 6+7\r\n\
        hpil          toggle HP-IL tracing to ILSCOPE USB serial port\r\n\
        pilbox        toggle PILBox serial tracing to ILSCOPE USB serial port\r\n\
        ilregs        toggle tracing of HP-IL registers\r\n\
        save          save tracer settings\r\n"

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

#define XMEM_HELP_TXT "Extended memory functions\r\n\
        status        shows the Extended Memory status\r\n\
        dump          creates a dump of Extended Memory\r\n\
        pattern       programs a pattern for FRAM test\r\n\
        ERASE         earse all Extended Memory\r\n\
        [n]           n= 0, 1, 2 \r\n\
                      plugs 0, 1 or 2 Extended Memory Modules\r\n\
                      do not use in combination with physical Extended Memory modules\r\n\
                      this software version does NOT have the Extended Function ROM built in\r\n\
                      Extended Functions must be physically plugged or available in an HP41CX"

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

// all dir functions
  extern void uif_dir(const char *dir);                // dir root

// all sdcard functions
  extern void uif_sdcard_status();      // uSD card status
  extern void uif_sdcard_mount();       // mount the uSD card
  extern void uif_sdcard_unmount();     // unmount the uSD card
  extern void uif_sdcard_mounted();     // check if card is mounted
  extern void uif_sdcard_connect();     // connect the SD card as a USB drive
  extern void uif_sdcard_eject();       // eject the SD card as USB drive

// import function, program a file to FLASh or FRAM
  extern void uif_import(int F, char *fname);       // import a file and program in FLASH

// ROM plug and unplug functions
  extern void uif_plug(int i);          // plug the selected ROM  
  extern void uif_unplug(int i);        // unplug the selected ROM

  extern void uif_printer(int i);       // function for the HP82143A printer

  extern void uif_xmem(int i);          // functions for Extended Memory control

  extern void uif_tracer(int i);        // functions for the bus tracer


// extern void uif_trace_mode(int m);    // trace [mode]

// extern void uif_list();               // list
  

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

#ifdef __cplusplus 
} 
#endif 

#endif // INC_CLI_BINDING_H_