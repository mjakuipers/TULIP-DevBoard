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
#include "pico.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/flash.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"

#include "modfile.h"

// includes for FatFS
#include "ff.h"             /* Obtains integer types */
#include "diskio.h"         /* Declarations of disk functions */
#include "f_util.h"
#include "hw_config.h"
#include "my_debug.h"
#include "rtc.h"
#include "sd_card.h"
#include "crash.h"


// definitions for storing information

#define FF_OFFSET       0x00080000                          // start of file system in FLASH

#define FF_SYSTEM_BASE  FF_OFFSET + XIP_BASE                // base of FF manager is at 0x10000000
                                                            //                     plus 0x00080000
                                                            //                       is 0x10080000

#define FF_SYSTEM_END   XIP_BASE + PICO_FLASH_SIZE_BYTES    // end of FLASH memory for Pico relative to FF_SYSTEM_BASE
#define FF_SYSTEM_SIZE  FF_SYSTEM_END - FF_SYSTEM_BASE      // total bytes in the file system

#define FRAM_BASE       0x10000                             // space before is used by the system
#define FRAM_END        0x40000                             // for 256 KByte device

#define FL_PAGE_MASK    0xFF

#define FL_BOUND        FLASH_SECTOR_SIZE - 1               // FL_BOUND becomes 0x0FFF, FLASH_BLOCK_SIZE is 0x1000
#define BLOCK_MASK      ~FL_BOUND                           // BLOCK_MASK becomes 0xFFFFF000

#define NOTFOUND        0xFFFFFFFF


// definitions for the flash memory

const uint8_t  *flash_contents_bt = (const uint8_t *)  (FF_SYSTEM_BASE);    // pointer to FLASH byte array
const uint16_t *flash_contents_wd = (const uint16_t *) (FF_SYSTEM_BASE);    // pointer to FLASH word array



#ifdef __cplusplus 
} 
#endif 
    
#endif  // __FFMANAGER_H__