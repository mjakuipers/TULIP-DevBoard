/*
 * fram.h
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

#ifndef __FRAM_H__
#define __FRAM_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <malloc.h>

#include "pico/stdlib.h"
#include "pico/platform.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "pico/util/queue.h"                    // used for safe FIFO management
#include "hardware/structs/systick.h"
#include "hardware/uart.h"                      // used for UART0 Printer port

#include "hp41_defs.h"
#include "cli-binding.h"
#include "hpinterface_hardware.h"
#include "modfile.h"
// #include "globalsettings.h"
// #include "module.h"

#define SPI_FRAM_SPEED  (40*1000*1000)          // set speed to 40 (?) MHz

// chip select macros
#define CS_on           gpio_put(PIN_SPI0_CS,0) 
#define CS_off          gpio_put(PIN_SPI0_CS,1) 


// the FRAM device is a Cypress FM25V10, 128k*8
// address range is 0x00000 .. 0x1FFFF

// on 2nd prototype the device is a 2 mbit Fujitsu device

// the FRAM device is a Cypress FM25V10, 256k*8
// address range is 0x00000 .. 0x1FFFF
// FRAM address map
//  0x00000 .. 0x01FFF      ROM image #0
//
//  0x1D000                 Global settings start
//  0x1E000                 XMEM start
//   

// definition of FRAM File System Addresses

#define FRAM_gsettings_start    0x1D000                 // start of global peristent settings in FRAM
#define FRAM_tracer_start       0x1D400                 // start of tracer settings
#define XMEMstart               0x1E000                 // start address of XMEM modules in FRAM

#define FRAM_INIT_ADDR          0x00000                 // address to store the FRAM initialization value
#define FRAM_INIT_VALUE         0x4041                  // value to indicate that the FRAM is initialized
#define FRAM_ROMMAP_START       0x00010



// #define FRAM_INIT_ADDR          0x10000                 // address to store the FRAM initialization value
// #define FRAM_INIT_VALUE         0x4041                  // value to indicate that the FRAM is initialized
// #define FRAM_ROMMAP_START       0x10010


#define FRAM_FS_START           0x00000                 // start of FRAM file system
#define FRAM_SIZE               0x40000                 // size of the FRAM device in bytes (256k*8 = 2 Mbit device)
#define FRAM_FS_END             0x3FFFF                 // end of FRAM file system  
#define FRAM_HEADER             0x00000                                             

#define FRAM_SETTINGS_FILE      0x00100
#define FRAM_SETTINGS_CONTENT   FRAM_SETTINGS_FILE + sizeof(ModuleMetaHeader_t)     // start of settings contents
#define FRAM_SETTINGS_CONTENT_SIZE 126                    // size of global settings

#define FRAM_ROMMAP_HEADER      FRAM_SETTINGS_CONTENT + FRAM_SETTINGS_CONTENT_SIZE         // start of ROM map header
                                
#define FRAM_ROMMAP_CONTENT     FRAM_ROMMAP_HEADER + sizeof(ModuleMetaHeader_t)     // start of ROM map contents

#define FRAM_TRACER_HEADER      FRAM_ROMMAP_CONTENT + sizeof(TULIP_Pages.Pages)                 // start of tracer settings header

#define FRAM_TRACER_CONTENT     FRAM_TRACER_HEADER + sizeof(ModuleMetaHeader_t)     // start of tracer settings contents
#define FRAM_FS_CONTENT_START   FRAM_TRACER_CONTENT + sizeof(TraceFilter.m_filter)              // start of file system contents

// The default file system at teh start of FRAM will then be the following:
//  Address         Size                    Description
//  0x00000         0x100                   FRAM Header
//  0x00100         Global Settings File start
//  0x00140         Global Settings contents
//  ...             ...
//  0x...           sizeof(ModuleMetaHeader_t) + sizeof(Pages)
//  ...             ...
//  0x...           sizeof(ModuleMetaHeader_t) + sizeof(m_filter)




// FRAM commands
#define FRAM_WRITE_CMD  0x02    // write memory data            0000 0010b
#define FRAM_READ_CMD   0x03    // read memory data             0000 0011b
#define FRAM_WREN_CMD   0x06    // set write enable latch       0000 0110b
#define FRAM_WRDI_CMD   0x04    // reset write enable latch     0000 0100b
#define FRAM_RDSR_CMD   0x05    // read status register         0000 0101b
#define FRAM_WRSR_CMD   0x01    // write status register        0000 0001b
#define FRAM_FSTRD_CMD  0x0B    // fast read memory data        0000 1011b
#define FRAM_SLEEP_CMD  0xB9    // enter sleep mode             1011 1001b
#define FRAM_RDID_CMD   0x9F    // read device ID               1001 1111b
#define FRAM_SNR_CMD    0xC3    // read serial number           1100 0011b

#define fram_write_cmd (2)
#define fram_read_cmd  (3)
#define fram_we_cmd    (6)
#define fram_wd_cmd    (4)


void init_spi_fram();
void fram_write(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t * data, size_t len);
void fram_write16(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t * data, size_t len);
void fram_write32(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t * data, size_t len);

void fram_read(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t *buf, size_t len);
void fram_read16(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint16_t *buf, size_t len);
void fram_read32(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint32_t *buf, size_t len);


// read from FRAM device, with fixed spi channel and cs pin
void fr_read(uint32_t addr, uint8_t *buf, size_t len);
void fr_readid(uint8_t *buf);

uint16_t fr_read16(uint32_t addr);
uint32_t fr_read32(uint32_t addr);

void fr_write(uint32_t addr, uint8_t *buf, size_t len);
void fr_write16(uint32_t addr, uint16_t word);
void fr_write32(uint32_t addr, uint32_t word);
void fram_show(uint32_t addr);


// for the FRAM File System managament
uint32_t fr_lastfree(uint32_t offs);
uint32_t fr_findfree(uint32_t offs, uint32_t size);
uint32_t fr_findfile(const char *name);

void fr_nukeall();                     // erase all FRAM to zero
bool fr_isinited();                    // check if FRAM is initialized


#ifdef __cplusplus
}
#endif

#endif


