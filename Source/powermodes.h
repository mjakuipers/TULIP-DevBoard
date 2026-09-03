/*
 * powermodes.h
 *
 *
 * This file is part of the TULIP4041 project.
 * 
 * USE AT YOUR OWN RISK
 *
 *  
 * use of powermodes.h is commented out, no power management in this version!
 */

/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __POWERMODES_H__
#define __POWERMODES_H__

// #include "tulip.h"
#include "pico.h"

#include "pico/stdlib.h"

#include "hardware/pll.h"
#include "hardware/regs/clocks.h"
#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "hardware/xosc.h"
#include "hardware/regs/rosc.h"
#include "hardware/regs/io_bank0.h"
// For __wfi
#include "hardware/sync.h"
#include "pico/runtime_init.h"

#include "pico/aon_timer.h"

#include "hardware/structs/scb.h"
#include "rosc.h"
#include "hardware/regs/powman.h"
#include "hardware/powman.h" 

#include "emulation.h"                      // for the PWO callback
#include "hardware/uart.h"


#ifdef __cplusplus
extern "C" {
#endif

void powman_example_init(uint64_t abs_time_ms);
int powman_example_off_until_gpio_high(int gpio);
int powman_example_off_until_gpio_low(int gpio);
int powman_example_off_until_time(uint64_t abs_time_ms);
int powman_example_off_for_ms(uint64_t duration_ms);
int rp2350_dormant_until_gpio_high(int gpio);


#ifdef __cplusplus
}
#endif


#endif