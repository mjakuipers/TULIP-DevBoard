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

// definition of PICO GPIO pins for the HP41

//  PICO pins in numerical order
//              proto setup                 final PCB
//      0       UART TX                     UART TX / I2C
//      1       UART RX                     UART RX / I2C
//      2       IR LED output (temp)        I2C
//      3       FI input                    I2C
//
//      4       FRAM RX  spi0               FRAM RX
//      5       FRAM CS  spi0               FRAM CS
//      6       FRAM SCK spi0               FRAM SCK
//      7       FRAM TX  spi0               FRAM TX
//
//      8       uSD RX   spi1               uSD RX
//      9       uSD CS   spi1               uSD CS
//     10       uSD SCK  spi1               uSD SCK
//     11       uSD TX   spi1               uSD TX
//
//     12       CLK01-HP41 input            CLK01
//     13       CLK02-HP41 input            CLK02
//     14       ISA-HP41   input            ISA
//     15       SYNC-HP41  input            SYNC
//     16       DATA-HP41  input            DATA
//     17       PWO-HP41   input            PWO
//
//     18       ISA_out HP41 output         ISA_out
//     19       ISA_OE  HP41 output         ISA_OE
//     20       DATA_out HP41 output        DATA_out
//     21       DATA_OE HP41 output         DATA_OE
//     22       FI_OE HP41 output           FI_OE
//     23       na on PICO
//     24       na on PICO
//
//     25       PICO onboard LED            onboard LED
//
//     26       T0_TIME helper              T0_TIME
//     27       SYNC_TIME helper            SYNC_TIME
//     28       DEBUG_OUT helper            DEBUG_OUT
//
//     29       na on PICO

#ifndef __HPINTERFACE_HARDWARE_H__
#define __HPINTERFACE_HARDWARE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ONBOARD_LED             25          // onboard LED

// HP41 input pins
#define P_CLK1                  12
#define P_CLK2                  13
#define P_DATA                  16
#define P_ISA                   14
#define P_SYNC                  15
#define P_PWO                   17
#define P_FI                    2

// helper pins for synchronizing PIO State Machines
#define P_T0_TIME               26   // phase 00, D0 time, also used for CARRY output on ISA
#define P_SYNC_TIME             27   // phase 46-55, during SYNC and ISA instruction (always)
#define P_ADDR_END              28   // spare, probably not used

// HP41 Output and Output Enable signals
#define P_ISA_OUT               18
#define P_ISA_OE                19
#define P_DATA_OUT              20
#define P_DATA_OE               21
#define P_FI_OE                 22

#define P_IR_LED                3           // IR output to LED, temporary pin
#define P_PWO_OE                P_IR_LED    // PWO output enable, same pin as the IR-LED

// For the OLED display
// #define SDA_PIN                 2
// #define SCL_PIN                 3

#define P_uSD_DO                8           // uSD card DO
#define P_uSD_CS                9           // uSD card CS
#define P_uSD_SCK               10          // uSD card SCK
#define P_uSD_DI                11          // uSD card DI

// For UART 0 printer emulation
// using pins 0 and 1 as connected to the PicoProbe
#define UART_ID                 uart0
#define BAUD_RATE               115200

#define UART_TX_PIN             0
#define UART_RX_PIN             1

// extern void wakemeup_41();
// extern int keycount_print;
// extern int keycount_adv;

#define DBG_OUT0                0b0000000000000001
#define DBG_OUT1                0b0000000000000101
#define DBG_OUT2                0b0000000000010101
#define DBG_OUT3                0b0000000001010101
#define DBG_OUT4                0b0000000101010101
#define DBG_OUT5                0b0000010101010101
#define DBG_OUT6                0b0001010101010101
#define DBG_OUT7                0b0101010101010101


#ifdef __cplusplus
}
#endif

#endif