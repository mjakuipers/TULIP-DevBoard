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


//SPI hardware  configurations
#define PIN_SPI0_RX     4
#define PIN_SPI0_CS     5
#define PIN_SPI0_SCK    6
#define PIN_SPI0_TX     7
#define SPI_PORT_FRAM   spi0

#define SPI_FRAM_SPEED  30*1000*1000             // set speed to 30 (?) MHz

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

#define FRAM_gsettings_start    0x1D000                 // start of global peristent settings in FRAM
#define FRAM_tracer_start       0x1D400                 // start of tracer settings
#define XMEMstart               0x1E000                 // start address of XMEM modules in FRAM


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
void fram_read(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t *buf, size_t len);
void fram_read32(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint32_t *buf, size_t len);


#ifdef __cplusplus
}
#endif

#endif


