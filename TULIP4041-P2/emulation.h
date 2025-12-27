/*
 * emulation.h
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

#ifndef __EMULATION_H__
#define __EMULATION_H__

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
#include <sys/time.h>

#include "pico/stdlib.h"
#include "pico/platform.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "pico/util/queue.h"                    // used for safe FIFO management
#include "hardware/structs/systick.h"
#include "hardware/uart.h"                      // used for UART0 Printer port
#include "hp41_defs.h"
#include "hpinterface_hardware.h"
#include "hp41_pio.pio.h"
// #include "userinterface.h"
#include "fram.h"
#include "cdc_helper.h"
#include "peripherals.h"
#include "globalsettings.h"
#include "module.h"
#include "embed_roms.h"

// Definitions for HP41 peripheral mode

extern class GSettings globsetting;


extern int16_t SLCT_PRPH ;                 // contains active selected peripheral
                                            // 0 - none active
                                            // 9 - SELP9 mode for the HP82143A

extern bool SELP9_status_BUSY;              // 
extern bool SELP9_status_VALID;             // always true

extern uint16_t SELP9_status;               // contains the HP82143A printer status bits, set to default values

extern uint16_t LocalAdvIgnore;             // for ignoring local paper advance

extern queue_t PrintBuffer;                 // printbuffer between cores
extern const int PrintBufSize;              // size of PrintBuffer

extern queue_t WandBuffer;                  // buffer for simulated WAND scans between cores
extern const int WandBufSize;               // size of WandBuffer

extern queue_t HPIL_SendBuffer;             // buffer for HP-IL send and receive
extern queue_t HPIL_RecvBuffer;             // buffer for HP-IL send and receive

extern int xmem_mods;                       // number of Extended Memory modules active (0, 1 or 2)

uint32_t cycles();

void send_ir_frame(uint32_t frame);
void setflag_FI0();
void pio_init();                         
void  __not_in_flash_func(core1_pio)();  
void pwo_callback(uint gpio, uint32_t events);
void shutdown_41();
enum HP41powermode {eAwake, eLightSleep, eDeepSleep};       // sleep modes  
void uif_pio_report();

extern bool USB_powered;                   // true when USB power is present

extern uint16_t SELP9_status;      // contains the HP82143A printer status bits, set to default values
extern uint8_t HPIL_REG[9];

extern int trace_enabled;
extern uint32_t fi_out1;                    // for flag output driver

extern enum HP41powermode HP41_powermode;

void InitEmulation();
bool IR_toggle();
void PowerMode_task();

#define ROM_BASE_OFFSET  0x00080000

// const uint16_t *flash_contents = (const uint16_t *) (XIP_BASE + ROM_BASE_OFFSET);
#define FLASH0_OFFSET   0x0000            // FLASH Page 0
#define FLASH1_OFFSET   0x1000            // FLASH Page 1
#define FLASH2_OFFSET   0x2000            // FLASH Page 2
#define FLASH3_OFFSET   0x3000            // FLASH Page 3
#define FLASH4_OFFSET   0x4000            // FLASH Page 4
#define FLASH5_OFFSET   0x5000            // FLASH Page 5
#define FLASH6_OFFSET   0x6000            // FLASH Page 6
#define FLASH7_OFFSET   0x7000            // FLASH Page 7
#define FLASH8_OFFSET   0x8000            // FLASH Page 8
#define FLASH9_OFFSET   0x9000            // FLASH Page 9

#define FRAM0_OFFSET    0x00000             //FRAM Page 0
#define FRAM1_OFFSET    0x02000             //FRAM Page 1
#define FRAM2_OFFSET    0x04000             //FRAM Page 2
#define FRAM3_OFFSET    0x06000             //FRAM Page 3
#define FRAM4_OFFSET    0x08000             //FRAM Page 4
#define FRAM5_OFFSET    0x0A000             //FRAM Page 5
#define FRAM6_OFFSET    0x0C000             //FRAM Page 6
#define FRAM7_OFFSET    0x0E000             //FRAM Page 7
#define FRAM8_OFFSET    0x10000             //FRAM Page 8
#define FRAM9_OFFSET    0x12000             //FRAM Page 9

#ifdef __cplusplus
}
#endif

#endif