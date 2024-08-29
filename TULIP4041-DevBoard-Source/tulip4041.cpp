/*
 * tulip4041.c      TULIP4041 main program
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

/*  Version history
 *
 *  Jun 2024    Cleaned up for first public BETA
 * 
 * 
 *
 */

// C standard includes
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <malloc.h>
#include <sys/time.h>

//RP2040 specific includes
#include "pico.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "pico/util/queue.h"                    // used for safe multi-core FIFO management"
#include "hardware/gpio.h"
#include "hardware/uart.h"                      // used for UART0 Printer port
#include "rtc.h"                                // used for FatFS

// Embedded CLI 
#include "embedded_cli.h"
#include "cli-binding.h"

// Tiny USB includes
#include <tusb.h>
#include "tusb.h"

#include "hw_config.h"

// emulation specific includes
#include "userinterface.h"                      // CLI for this tool
#include "peripherals.h"                        // tasks for HP peripherals (HP-IL, Wand, Printer )
#include "fram.h"
#include "tracer.h"
#include "emulation.h"             
#include "cdc_helper.h"

#include "testcpp.h"
// #include "globalsettings.h"

extern int testfunction();

int main() {

    // initialize the USB CDC devices
    usbd_serial_init();
    tusb_init();
    stdio_init_all(); 

    serialport_init();

    // stdio_usb_init();

    // initialize all relevant GPIO
    // enable PWO interrupt to reset SYNC state machine

    bus_init();         // in peripherals.cpp

    gpio_put(P_ADDR_END, true);

    sdcard_init();     // initialize the FatFS system

    // initialize SPI interface for FRAM
    init_spi_fram();

    // initialize trace and printbuffers 
    TraceBuffer_init();     // only if used, do this dynamic in the future
    PrintBuffer_init();     // only if used, do this dynamic in the future
    WandBuffer_init();      // only if used, do this dynamic in the future
    
    // prepare the HP-IL loop
    HPIL_init();

    // allow some time to stabilize all I/O and USB
    sleep_ms(1000);    
    gpio_toggle(ONBOARD_LED);
    sleep_ms(1000);
    gpio_toggle(ONBOARD_LED);

    // get the global persistent settings and initialize if needed
    globsetting.retrieve();         // only works if PWO was low, but we do not check here
    if (globsetting.is_initialized()) globsetting.set_default();
    globsetting.save();

    initCliBinding();       // initialization of the embedded-cli

    welcome();      // show welcome message on the CLI
    gpio_put(ONBOARD_LED, false);   // switch off LED, this will now light when HP41 bus activity is detected

    InitEmulation();

    // launching core1 code, must be done before starting the PIO state machines
    multicore_launch_core1(core1_pio);  

    // initialize and start the PIO state machines, report the status in the CLI
    pio_init();                         
    pio_report();

    // below is the main loop, it never ends
    
    while (true) {
        
        PowerMode_task();           // Verify the HP41 power mode

        tud_task();                 // process the USB interfaces required by TinyUSB

        runCLI();                   // process the 'new' embedded CLI

        serial_loop();              // process the 'old' user interface

        Trace_task();               // process HP41 Trace/disassembler

        Print_task();               // process any print tasks (serial and/or IR, not HP-IL printing)

        HPIL_task();                // process the HP-IL task for transmitting and receiving frames
                                    // only if HP-IL is active
    
    }

    return 0;
}