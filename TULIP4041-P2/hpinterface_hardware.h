/*
 * hpinterface_hardware.h
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

// definition of PICO GPIO pins for the TULIP4041 DEVBOARD - Pico2

//  PICO pins in numerical order
//              DevBoard setup              final PCB module version (preliminary)
//      0       UART TX                     UART TX / I2C / AUX GPIO
//      1       UART RX                     UART RX / I2C / AUX GPIO
//      2       IR LED output (temp)        I2C / AUX GPIO
//      3       FI input                    I2C / AUX GPIO
//
//      4       FRAM RX  spi0               FRAM RX     spi0
//      5       FRAM CS  spi0               FRAM CS     spi0
//      6       FRAM SCK spi0               FRAM SCK    spi0
//      7       FRAM TX  spi0               FRAM TX     spi0
//
//      8       uSD RX   spi1               uSD RX      spi1
//      9       uSD CS   spi1               uSD CS      spi1
//     10       uSD SCK  spi1               uSD SCK     spi1
//     11       uSD TX   spi1               uSD TX      spi1
//
//     12       CLK01-HP41 input            CLK01       HP41 input
//     13       CLK02-HP41 input            CLK02       HP41 input
//     14       ISA-HP41   input            ISA         HP41 input
//     15       SYNC-HP41  input            SYNC        HP41 input
//     16       DATA-HP41  input            DATA        HP41 input
//     17       PWO-HP41   input            PWO         HP41 input
//
//     18       ISA_out HP41 output         ISA_out     HP41 output
//     19       ISA_OE  HP41 output         ISA_OE      HP41 output enable
//     20       DATA_out HP41 output        DATA_out    HP41 output
//     21       DATA_OE HP41 output         DATA_OE     HP41 output enable
//     22       FI_OE HP41 output           FI_OE       HP41 output enable
//     23       na on PICO2
//     24       na on PICO2
//
//     25       PICO onboard LED            onboard LED
//
//     26       T0_TIME helper              T0_TIME
//     27       SYNC_TIME helper            SYNC_TIME
//     28       DEBUG_OUT helper            DEBUG_OUT
//
//     29       na on PICO2

#ifndef __HPINTERFACE_HARDWARE_H__
#define __HPINTERFACE_HARDWARE_H__

#ifdef __cplusplus
extern "C" {
#endif



#define T_DEVBOARD              0                   // TULIP4041 DevBoard relevant code
#define T_MODULE                1                   // TULIP4041 final module relevant code    

// we get the correct define for TULIP_HARDWARE from the makefile
// and then two executables are created, one for the DevBoard and one for the final module


// all generic pin definitions go here

// For UART 0 printer emulation
// using pins 0 and 1 as connected to the PicoProbe
#define UART_ID                 uart0
#define BAUD_RATE               (115200)    
#define UART_TX_PIN             0
#define UART_RX_PIN             1

#define ONBOARD_LED             25          // onboard LED pin number
#define PICO_VBUS_PIN           24          // VBUS pin number GPIO


#define PICO_DEFAULT_LED_PIN    25          // default LED pin number for the Pico
#define USE_LED                 1           // use the onboard LED for SD card activity

//SPI hardware configurations for FRAM and uSD card
#define PIN_SPI0_RX             4
#define PIN_SPI0_CS             5
#define PIN_SPI0_SCK            6
#define PIN_SPI0_TX             7
#define SPI_PORT_FRAM           spi0

// For the RTC, OLED display or other I2C devices
// for the DeVBoard these pins are shared with FI input, IR output and PWO_OE
#define PIN_I2C_SDA             2
#define PIN_I2C_SCL             3
#define I2C_PORT                i2c1
#define I2C_SPEED               100000      // 100 kHz for the I2C bus speed, is the maximum for the PCF8523 RTC in standard mode
#define PCF8523_ADDRESS         0x68        // I2C address of the PCF8523 RTC


// SPI hardware configuration for the uSD card
#define P_uSD_DO                8           // uSD card DO
#define P_uSD_CS                9           // uSD card CS
#define P_uSD_SCK               10          // uSD card SCK
#define P_uSD_DI                11          // uSD card DI
#define SPI_PORT_uSD            spi1

// HP41 input pins
#define P_CLK1                  12
#define P_CLK2                  13
#define P_DATA                  16
#define P_ISA                   14
#define P_SYNC                  15
#define P_PWO                   17

// HP41 Output and Output Enable signals
#define P_ISA_OUT               18
#define P_ISA_OE                19
#define P_DATA_OUT              20
#define P_DATA_OE               21
#define P_FI_OE                 22


// helper pins for synchronizing PIO State Machines
#define P_T0_TIME               26   // phase 00, D0 time, also used for CARRY output on ISA
#define P_SYNC_TIME             27   // phase 46-55, during SYNC and ISA instruction (always)
#define P_DEBUG                 28   // Debug output pulses output to monitor state of Core1 loop

#if (TULIP_HARDWARE == T_DEVBOARD)
    #define P_FI                    2           // FI input from the HP41
    #define P_IR_LED                3           // IR output to LED
    #define P_PWO_OE                3           // PWO output enable, shared with SPARE1
    #define SPARE1                  3           // spare pin for future use
#elif (TULIP_HARDWARE == T_MODULE)
    #define P_FI                    -1          // no FI input on the final module
    #define P_IR_LED                29          // IR output to LED, shared PWO OE on DevBoard
    #define P_PWO_OE                23          // PWO output enable, shared with SPARE1
    #define SPARE1                  23          // spare pin for future use, or PWO-OE
#endif

// for generating debug output messages to the CLI
// #define DEBUG                   1        // generate DEBUG output messages to the CLI

#define DEFAULT_CLOCK           150000   // default clock frequency in kHZ for the Pico
#define TULIP_CLOCK_FAST        150000   // fast clock frequency in kHZ for the TULIP4041
#define TULIP_CLOCK_MID         125000   // clock frequency in kHz for the TULIP4041
#define TULIP_CLOCK_SLOW         75000   // slow clock frequency in kHz for the TULIP4041

#define DBG_OUT0                0b0000000000000001
#define DBG_OUT1                0b0000000000000101
#define DBG_OUT2                0b0000000000010101
#define DBG_OUT3                0b0000000001010101
#define DBG_OUT4                0b0000000101010101
#define DBG_OUT5                0b0000010101010101
#define DBG_OUT6                0b0001010101010101
#define DBG_OUT7                0b0101010101010101


// definitions for storing information
// XIP_BASE is the start of the FLASH memory in the RP2350 memory map
// the functions for FLASH access are all relative to XIP_BASE
#define FF_OFFSET       0x00100000                          // start of file system in FLASH
                                                            // allowing 1 MBYTE for firmware                                                            
                                                            // use for programming FLASH

#define FF_SYSTEM_BASE  (XIP_BASE + FF_OFFSET)              // XIP_BASE       0x10000000, start of FLASH in RP2350 memory map
                                                            // FF_SYSTEM BASE 0x10100000 (1 MByte for firmware)
                                                            // start of FLASH File system, used for reading

#define FF_OWNER_BASE   (FF_SYSTEM_BASE - 0x00001000)       // base address for the owner string, 4KByte before FF_SYSTEM_BASE
#define FF_OWNER_OFFSET (FF_OFFSET - 0x1000)                // offset in FLASH for the owner string
#define FF_OWNER_SIZE   0x1000                              // size of the owner string area, 4KByte
                                                            // this is where we put the TULIP owner string

#undef PICO_FLASH_SIZE_BYTES
#if (TULIP_HARDWARE == T_DEVBOARD)
    #define PICO_FLASH_SIZE_BYTES ( 4 * 1024 * 1024) // size of the FLASH memory in bytes,  4 MByte 
#elif (TULIP_HARDWARE == T_MODULE)
    #define PICO_FLASH_SIZE_BYTES (16 * 1024 * 1024) // size of the FLASH memory in bytes, 16 MByte
#endif

#define FF_SYSTEM_END   (PICO_FLASH_SIZE_BYTES)             // end of FLASH memory for Pico relative to FF_SYSTEM_BASE
#define FF_SYSTEM_SIZE  (PICO_FLASH_SIZE_BYTES - FF_OFFSET) // total bytes in the file system

// #define FRAM_BASE       0x10000                             // space before is used by the system
// #define FRAM_END        0x40000                             // for 256 KByte device

// defines below are relevant for the 256-byte page size of the fLASH memory
// #define FLASH_PAGE_SIZE  0x100                              // FLASH page size in bytes
#define FLASH_PAGE_MASK  (FLASH_PAGE_SIZE - 1)              // FLASH page mask, becomes 0xFF
#define FLASH_PAGE_OFFS ~(FLASH_PAGE_MASK)                  // FLASH page offset, becomes 0xFFFFFF00

// defines below are relevant for the 4KByte sector size of the FLASH memory
// #define FLASH_SECTOR_SIZE 0x1000                           // FLASH sector size in bytes, 4K or 4096 bytes
#define FLASH_SECTOR_MASK (FLASH_SECTOR_SIZE - 1)          // FLASH sector mask, becomes 0xFFF
#define FLASH_SECTOR_OFFS ~(FLASH_SECTOR_MASK)             // FLASH sector offset, becomes 0xFFFFF000

#define NOTFOUND        0xFFFFFFFF

#ifdef __cplusplus
}
#endif

#endif