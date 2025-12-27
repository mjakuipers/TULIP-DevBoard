/*
 * peripherals.h
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

#ifndef __PERIPHERALS_H__
#define __PERIPHERALS_H__

#ifdef __cplusplus 
extern "C" { 
#endif 

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "tulip.h"

#include "pico/stdlib.h"

#include "cdc_helper.h"
#include "emulation.h"
#include "tracer.h"
// #include "module.h"

// includes for the SD Card driver
#include "ff.h"
#include "f_util.h"
#include "hw_config.h"

// defines for HP-IL emulation
#define RFC         0x500                       // RFC frame 
#define IDY_C0      0x6C0                       // IDY frame C0
#define IDY_23      0x623                       // IDY frame 23


// PILBox initialization frames
#define TDIS   0x494  // TDIS
#define COFI   0x495  // COFF with IDY, firmware >= v1.6
#define CON    0x496  // Controller ON, not used for HP41
#define COFF   0x497  // Controller OFF
#define SSRQ   0x49C  // Set Service Request, not used for HP41
#define CSRQ   0x49D  // Clear Service Request, not used for HP41

#define NO_PIL 0xFFFF  // indicate no PILBox connected


// Array for Wand Paper Keyboard

// function definitions in peripherals.cpp
void gpio_toggle(uint signal);
void gpio_pulse(uint signal, uint numtimes);
void serialport_init();
void bus_init();
void WandBuffer_init();
void Print_task();
void PrintBuffer_init();
void getIL_mnemonic(uint16_t wFrame, char *mnem);
void HPIL_init();
void HPIL_task();
void Wand_task();

extern int m_eMode;          // HP-IL controller/device mode
extern uint32_t cycles();




// prepare for dynamic buffers for user memory

#ifdef __cplusplus
}
#endif

#endif