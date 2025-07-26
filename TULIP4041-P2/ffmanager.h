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

#ifndef __FFMANAGER_H__
#define __FFMANAGER_H__

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
#include <stdio.h>
#include "pico/bootrom.h"
#include "hardware/structs/otp.h"

#include "cli-binding.h"
#include "cdc_helper.h"
#include "emulation.h"
#include "module.h"

// includes for FatFS
#include "ff.h"             /* Obtains integer types */
#include "diskio.h"         /* Declarations of disk functions */
#include "f_util.h"
#include "hw_config.h"
#include "my_debug.h"
#include "sd_card.h"
#include "crash.h"

// definitions for the flash memory

bool otp_write_serial(char *serial_string);
bool otp_read_serial(char *serial_string);

void ff_delay500();
void ff_show(uint32_t addr);
void ff_init();
void ff_nuke();
uint32_t ff_lastfree(uint32_t offs);
uint32_t ff_findfree(uint32_t offs, uint32_t size);
uint32_t ff_findnextf(uint32_t offs);
uint32_t ff_findfile(const char *name);
void ff_erase(uint32_t fl_start, uint32_t fl_end);
bool ff_flasherased(int num);
int ff_compare(uint32_t offs, uint8_t *buf, int num);
uint32_t ff_erased(uint32_t offs, uint32_t size, int num);
bool ff_write_range(uint32_t offs, uint8_t *buf, int num);
bool ff_write(uint32_t offs, uint8_t data);
bool ff_writeable(uint32_t offs, uint32_t size);
bool ff_writeableb(uint32_t offs, uint8_t data);
bool ff_isinited();



#ifdef __cplusplus 
} 
#endif 
    
#endif  // __FFMANAGER_H__