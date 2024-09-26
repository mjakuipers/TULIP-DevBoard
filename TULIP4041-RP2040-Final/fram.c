/*
 * fram.c
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


#include "fram.h"

//
// FRAM read/write routines
// 

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
} 


// initialize the SPI port
void init_spi_fram()
{
    spi_init(SPI_PORT_FRAM, SPI_FRAM_SPEED);
    gpio_set_function(PIN_SPI0_RX, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI0_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI0_TX, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_SPI0_CS);
    gpio_put(PIN_SPI0_CS, 1);
    gpio_set_dir(PIN_SPI0_CS, GPIO_OUT);  

    // SPI modes supported by the device: mode 0 (0, 0) and mode 3 (1,1)
}


void __not_in_flash_func(fram_read)(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t *buf, size_t len) 
// base function to read bytes form the fram device
{
    cs_select(cs_pin);
    uint8_t cmdbuf[4] = {
            FRAM_READ_CMD,
            addr >> 16,
            addr >> 8,
            addr
    };
    spi_write_blocking(spi, cmdbuf, 4);             // write READ command
    spi_read_blocking(spi, 0, (uint8_t*)buf, len);            // read result
    cs_deselect(cs_pin);
} 

void __not_in_flash_func(fram_read32)(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint32_t *buf, size_t len) 
// base function to read bytes form the fram device, for 32-bit wide words
{
    cs_select(cs_pin);
    uint8_t cmdbuf[4] = {
            FRAM_READ_CMD,
            addr >> 16,
            addr >> 8,
            addr
    };
    spi_write_blocking(spi, cmdbuf, 4);             // write READ command
    spi_read_blocking(spi, 0, (uint8_t*)buf, len);            // read result
    cs_deselect(cs_pin);
} 


// read FRAM device ID
// this is always 9 bytes!
void __not_in_flash_func(fram_read_id)(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t *buf, size_t len) 
// base function to read bytes form the fram device
{
    cs_select(cs_pin);
    uint8_t cmdbuf[4] = {
            FRAM_RDID_CMD,
            addr >> 16,
            addr >> 8,
            addr
    };
    spi_write_blocking(spi, cmdbuf, 4);             // write READ command
    spi_read_blocking(spi, 0, (uint8_t*)buf, len);            // read result
    cs_deselect(cs_pin);
} 

// === write control ===
void __not_in_flash_func(fram_write_enable)(spi_inst_t *spi, uint cs_pin) {
    CS_on ; 
    uint8_t cmd = FRAM_WREN_CMD ;
    spi_write_blocking(spi, &cmd, 1);
    CS_off ; 
}

void __not_in_flash_func(fram_write_disable)(spi_inst_t *spi, uint cs_pin) {
    CS_on ; 
    uint8_t cmd = FRAM_WREN_CMD ;
    spi_write_blocking(spi, &cmd, 1);
    CS_off ; 
}

// === write commands ===
void __not_in_flash_func(fram_write)(spi_inst_t *spi, uint cs_pin, uint32_t addr, uint8_t * data, size_t len) {
    uint8_t cmdbuf[4] = {
            FRAM_WRITE_CMD,
            addr >> 16,
            addr >> 8,
            addr
    };
    fram_write_enable(spi, cs_pin) ;
    CS_on ;
    spi_write_blocking(spi, cmdbuf, 4) ;
    spi_write_blocking(spi, data, len) ;
    CS_off ;
    fram_write_disable(spi, cs_pin) ;
}


