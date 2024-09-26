/*
 * userinterface.h
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

#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__

#ifdef __cplusplus 
extern "C" { 
#endif 

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "pico.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/flash.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"
#include "fram.h"
#include "tracer.h"
#include "powermodes.h"

#include "emulation.h"
#include "globalsettings.h"

#include "cli-binding.h"

// includes for FatFS
#include "sdcard.h"


// Serial loop command structure

typedef void (*FPTR)(void);

// functions called from the CLI

// subfunctions for the system command
void uif_status();
void uif_pio_status();
void uif_cdc_status();
void uif_cdc_ident();
void uif_reboot();
void uif_bootsel();
void uif_calcreset();         
void uif_poweron();       
void uif_configinit();    
void uif_configlist();

void uif_blink(int b);

void uif_dir(const char *dir);            // dir

// all sdcard functions
void uif_sdcard_status();      // uSD card status
void uif_sdcard_mount();       // mount the uSD card
void uif_sdcard_unmount();     // unmount the uSD card
void uif_sdcard_mounted();

void uif_import(int F, char *fname);       // import a file and program in FLASH

void uif_plug(int i);          // plug the selected ROM
void uif_unplug(int i);        // unplug the selected ROM

void uif_printer(int i);       // HP82143A printer functions

void uif_xmem(int i);          // functions for Extended Memory control

void uif_tracer(int i);        // functions for the bus tracer

void uif_welcome();
void pio_welcome();

extern void usb_sd_eject();
extern void usb_sd_connect();


void welcome();

void serial_loop();


void toggle_trace(void);
void toggle_disasm(void);

extern void pio_report();
void welcome();

extern int trace_enabled;
extern int trace_outside;

extern int printer_on;
extern int keycount_print;
extern int keycount_adv;
extern bool default_map;
extern bool default_map_off;

extern void wakemeup_41();

// extern embed_rom1[];
// extern embed_romP[];
extern const uint16_t __in_flash() embed_romP[];
extern const uint16_t *flash_contents;

#ifdef __cplusplus 
} 
#endif 


    
#endif  // __USERINTERFACE_H__


