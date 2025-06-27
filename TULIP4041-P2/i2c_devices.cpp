/*
 * i2c_devices.cpp
 *
 * This file is part of the TULIP4041 project.
 * Copyright (C) 2024 Meindert Kuipers
 * 
 * This file contains functions to control the I2C devices on GPIO 2+3:
 *  - generic i@c bus scan
 *  - PCF8523 RTC support
 *  - display support for the SSD1306 OLED display
 *  - 
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

#include "i2c_devices.h"



// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}


// initialize the I2C port on GPIO 2+3
void i2c_initialize() {
    i2c_init(I2C_PORT, I2C_SPEED);
    gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C_SDA);
    gpio_pull_up(PIN_I2C_SCL);
}

// scan the I2C bus for any devices
void i2c_scan() {
    // Sweep through all 7-bit I2C addresses, to see if any slaves are present on
    // the I2C bus. Print out a table that looks like this:
    //
    // I2C Bus Scan
    //    0 1 2 3 4 5 6 7 8 9 A B C D E F
    // 00 . . . . . . . . . . . . . . . .
    // 10 . . @ . . . . . . . . . . . . .
    // 20 . . . . . . . . . . . . . . . .
    // 30 . . . . @ . . . . . . . . . . .
    // 40 . . . . . . . . . . . . . . . .
    // 50 . . . . . . . . . . . . . . . .
    // 60 . . . . . . . . . . . . . . . .
    // 70 . . . . . . . . . . . . . . . .
    // E.g. if addresses 0x12 and 0x34 were acknowledged.
    cli_printf("  I2C Bus Scan");
    cli_printf("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        tud_task();                 // tinyusb task, keep the serial ports working

        if (addr % 16 == 0) {
            cli_printfn("  %02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns -1
        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr)) {
            ret = PICO_ERROR_GENERIC;
            cli_printfn("x  ");
        } else {
            ret = i2c_read_blocking(I2C_PORT, addr, &rxdata, 1, false);

            cli_printfn(ret < 0 ? "." : "#");
            cli_printfn(addr % 16 == 15 ? "\n\r" : "  "); 
        }
    }
    cli_printf("");
}


// below are functions to control the PCF8523 RTC
void pcf8520_reset() {
    // First do a reset, then set default values, especially the XTAL load capacitance
    // There are a load more options to set up the device in different ways that could be added here

    // write pattern 0x58 to the control register 0x00 for a software reset
    uint8_t buf[2] = {0x00, 0x58};
    i2c_write_blocking(i2c1, PCF8523_ADDRESS, buf, 2, false);

    // set Control Register 1 to the initial values
    // 0x58 = 1000 0000b
    // bit 7 = 1 -> oscillator quartz is 12.5 pF
    // bit 6 = 0 -> unused, must be written with 0
    // bit 5 = 0 -> STOP, must be written 0
    // bit 4 = 0 -> no software reset, already done
    // bit 3 = 0 -> 24 hour mode selected
    // bit 2 = 0 -> second interrupt disabled
    // bit 1 = 0 -> qlarm interrupt disabled
    // bit 0 = 0 -> correction interrupt disabled
    buf[0] = 0x00; // control register 1
    buf[1] = 0x80; // write 0x00 to control register 1 to disable the oscillator
    i2c_write_blocking(i2c1, PCF8523_ADDRESS, buf, 2, false);

    // next is to enable the battery switchover in the control register 3
    // bits 7..5 are set to standard switching mode (0b000)
    // all other bits and control interrupts are disabled
    buf[0] = 0x02;
    buf[1] = 0x00;  
    i2c_write_blocking(i2c1, PCF8523_ADDRESS, buf, 2, false);

    // set the Tmnr_CLKOUT regsiter (address 0x0F) to 0x00
    // bit 5..3 COF[2:0] - 110, 1 Hz output
    // all other bits are 0
    buf[0] = 0x0F; // Tmnr_CLKOUT register
    buf[1] = 0x60; // set COF[2:0] to 110 for 1 Hz output
    i2c_write_blocking(i2c1, PCF8523_ADDRESS, buf, 2, false);
}


void pcf8520_write_current_time(uint8_t *RTC_time) {
    //Write values for the current time in the RTC_time array
    //index 0 -> second: bits 4-6 are responsible for the ten's digit and bits 0-3 for the unit's digit
    //index 1 -> minute: bits 4-6 are responsible for the ten's digit and bits 0-3 for the unit's digit
    //index 2 -> hour: bits 4-5 are responsible for the ten's digit and bits 0-3 for the unit's digit
    //index 3 -> day of the month: bits 4-5 are responsible for the ten's digit and bits 0-3 for the unit's digit
    //index 4 -> day of the week: where Sunday = 0x00, Monday = 0x01, Tuesday... ...Saturday = 0x06
    //index 5 -> month: bit 4 is responsible for the ten's digit and bits 0-3 for the unit's digit
    //index 6 -> year: bits 4-7 are responsible for the ten's digit and bits 0-3 for the unit's digit
    //NOTE: if the value in the year register is a multiple for 4, it will be considered a leap year and hence will include the 29th of February

    // PCF8520 registers for the time:
    // 0x03 -> seconds
    // 0x04 -> minutes
    // 0x05 -> hours
    // 0x06 -> day of the month
    // 0x07 -> day of the week
    // 0x08 -> month
    // 0x09 -> year

    uint8_t buf[2];
    for (int i = 3; i < 10; ++i) {
        buf[0] = i;
        buf[1] = RTC_time[i - 3];
        i2c_write_blocking(i2c1, PCF8523_ADDRESS, buf, 2, false);
    }
}

void pcf8520_read_all(uint8_t *buffer) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.
    // make certain that the buffer is at leats 20 bytes long, as we will be reading 20 bytes in total

    // Start reading all registers from 0x00 to 0x13, which is 20 bytes in total
    uint8_t val = 0x00; // start register to read from
    i2c_write_blocking(i2c1, PCF8523_ADDRESS, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c1, PCF8523_ADDRESS, buffer, 20, false);
}

void pcf8520_read_time(uint8_t *buffer) {
    // Read the time and date only
    // Start reading registers from 0x03 to 0x09, which is 7 bytes in total
    uint8_t val = 0x03; // start register to read from
    i2c_write_blocking(i2c1, PCF8523_ADDRESS, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c1, PCF8523_ADDRESS, buffer, 7, false);
}


void pcf8520_convert_time(int conv_time[7], const uint8_t raw_time[7]) {
    // Convert raw data into time
    conv_time[0] = (10 * (int) ((raw_time[0] & 0x70) >> 4)) + ((int) (raw_time[0] & 0x0F));
    conv_time[1] = (10 * (int) ((raw_time[1] & 0x70) >> 4)) + ((int) (raw_time[1] & 0x0F));
    conv_time[2] = (10 * (int) ((raw_time[2] & 0x30) >> 4)) + ((int) (raw_time[2] & 0x0F));
    conv_time[3] = (10 * (int) ((raw_time[3] & 0x30) >> 4)) + ((int) (raw_time[3] & 0x0F));
    conv_time[4] = (int) (raw_time[4] & 0x07);
    conv_time[5] = (10 * (int) ((raw_time[5] & 0x10) >> 4)) + ((int) (raw_time[5] & 0x0F));
    conv_time[6] = (10 * (int) ((raw_time[6] & 0xF0) >> 4)) + ((int) (raw_time[6] & 0x0F));
}


 