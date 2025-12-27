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
#include "inttypes.h"
#include "sys/types.h"
#include "tulip.h"
#include "pico/stdlib.h"
#include "pico/unique_id.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/flash.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"
#include "fram.h"
#include "tracer.h"
#include "ffmanager.h"
#include "powermodes.h"

#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include "hardware/adc.h"

#include "emulation.h"
#include "globalsettings.h"
#include "embed_roms.h"
#include "peripherals.h"
#include "i2c_devices.h"
#include "cli-binding.h"
#include "fram.h"
#include "powermodes.h"

#include "module.h"

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
void uif_serial(const char *str);          // show/program the TULIP serial number
void uif_gpio_status();   // show the GPIO status
void uif_owner(const char *str);          // show/program the owner of the device

void measure_freqs(void);

void uif_blink(int b);

void uif_dir(const char *dir);                  // dir

// all sdcard functions
void uif_sdcard_status();                       // uSD card status
void uif_sdcard_mount();                        // mount the uSD card
void uif_sdcard_unmount();                      // unmount the uSD card
void uif_sdcard_mounted();

void uif_import(const char *fname, int a2, int a3);       // import a file and program in FLASH/FRAM

void uif_list(int i, const char *fname);        // list
void uif_delete(const char *fname);             // delete a file from FLASH/FRAM

void uif_plug(int func, int Page, int Bank, const char *fname);          // plug the selected ROM 
void uif_unplug(int i, int bk);                 // unplug the selected ROM / Bank
void uif_cat(int p, int b);                     // show the plugged ROMs
void uif_reserve(int i, int p, const char *comment);     // reserve a Page for a module

void uif_fram(int i, uint32_t addr);            // FRAM functions

void uif_printer(int i);                        // HP82143A printer functions

void uif_xmem(int i);                           // functions for Extended Memory control

void uif_tracer(int i, int bufsize);            // functions for the bus tracer

void uif_rtc(int i, const char *args);          // RTC test functions

void uif_emulate(int i, int p);                 // enable/disable hardware emulation functions

void uif_wand(int i, const char* fname, char* instruction);       // wand functions
void uif_w(const char *instruction);            // WAND Paper Keyboard functions

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
extern void PrintIRchar(uint8_t c);

// extern embed_rom1[];
// extern embed_romP[];
extern const uint16_t __in_flash() embed_romP[];
extern const uint16_t *flash_contents;

#ifdef __cplusplus 
} 
#endif 

 
#endif  // __USERINTERFACE_H__


