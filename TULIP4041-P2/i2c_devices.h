/*
 * i2c_devices.h.h
 *
 * This file is part of the TULIP4041 project
 * 
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


#ifndef __I2C_DEVICES_H__
#define __I2C_DEVICES_H__
 
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pico.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "hpinterface_hardware.h"
#include "cli-binding.h"



// function prototypes for the I2C devices
void i2c_initialize();
void i2c_scan();

void pcf8520_reset();
void pcf8520_write_current_time(uint8_t *RTC_time);
void pcf8520_read_all(uint8_t *buffer);


#ifdef __cplusplus
}
#endif

#endif    //__I2C_DEVICES_H__