/*
 * sdcard.h
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

#ifndef __SDCARD_H__
#define __SDCARD_H__

#ifdef __cplusplus 
extern "C" { 
#endif 

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "tulip.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/flash.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "cli-binding.h"

// includes for FatFS
#include "ff.h"             /* Obtains integer types */
#include "diskio.h"         /* Declarations of disk functions */
#include "f_util.h"
#include "hw_config.h"
#include "my_debug.h"
#include "sd_card.h"
#include "crash.h"

#define DRIVENUM        0       // drive number on TULIP is always 0
#define DEFAULT_DRIVE   ""     // this will be our default drive mount
#define SECT_SIZE       512     // fix the sector size

uint32_t sd_sect_count();

void sdcard_init();                 
void sd_status();
FRESULT sd_dir(const char *dir);
bool sd_mount();
bool sd_mount_s();
void sd_unmount();
void sd_mounted();

void put_out_error_message(const char *s);
void put_out_info_message(const char *s);
void put_out_debug_message(const char *s);

#ifdef __cplusplus 
} 
#endif 
    
#endif  // __SDCARD_H__