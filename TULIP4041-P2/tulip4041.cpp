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
 *  Sep 2024    modified for migration to RP2350 and SDK2.0
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
#include "tulip.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "pico/util/queue.h"                    // used for safe multi-core FIFO management"
#include "hardware/gpio.h"
#include "hardware/uart.h"                      // used for UART0 Printer port
#include "pico/bootrom.h"

#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
// #include "hardware/adc.h"


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
#include "module.h"


int main() {

    // set the correct main clock frequency to 125 MHz   
    // set_sys_clock_khz(75000, true);
    set_sys_clock_khz(TULIP_CLOCK_FAST, true);   // set the Tulip clock to 125 MHz


    // The previous line automatically detached clk_peri from clk_sys, and
    // attached it to pll_usb, so that clk_peri won't be disturbed by future
    // changes to system clock or system PLL. If we need higher clk_peri
    // frequencies, we can attach clk_peri directly back to system PLL (no
    // divider available) and then use the clk_sys divider to scale clk_sys
    // independently of clk_peri.
    clock_configure(
        clk_peri,                                               // Clock to configure   
        0,                                                      // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,       // System PLL on AUX mux
        TULIP_CLOCK_FAST * 1000,                                 // Input frequency
        TULIP_CLOCK_FAST * 1000                                  // Output (must be same as no divider)
    );
    
    uint32_t actual_freq = clock_get_hz(clk_sys);

    // set the peripheral clock to the same frequency to 80MHz be able to set the SPI speed higher
    // clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, 80 * MHZ, 80 * MHZ);
    // clock_set_reported_hz(clk_peri, 80 * MHZ);


    // initialize the serial devices
    stdio_init_all();           // initialize stdio for the debug UART
    serialport_init();          // debug UART on GPIO 0+1

    measure_freqs();

    // initialize all relevant GPIO
    // enable PWO interrupt to reset SYNC state machine
    bus_init();         // in peripherals.cpp

    // allow some time to stabilize all I/O and USB
    sleep_ms(1000);    
    gpio_toggle(ONBOARD_LED);
    sleep_ms(1000);
    gpio_toggle(ONBOARD_LED);
    
    // tusb_init();
    tud_init(BOARD_TUD_RHPORT); // initialize the TinyUSB stack
    usbd_serial_init();



    // initialize the debug output
    gpio_put(P_DEBUG, true);

    // initialize SPI interface for FRAM
    init_spi_fram();


    // all IO is now initialized, especially UART and SPI as these use the peripheral clocks
    // We can now change the system clock without affecting these peripherals
    USB_powered = gpio_get(PICO_VBUS_PIN);   // check if USB power is present

    if (!USB_powered) {
        // lower the clock speed to save power when no USB power is present
        uint div = TULIP_CLOCK_FAST / TULIP_CLOCK_SLOW;
        clock_configure(
            clk_sys,
            CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
            CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
            TULIP_CLOCK_FAST * 1000,
            TULIP_CLOCK_SLOW * 1000
        );
    }   

    // initialize the I2C port for the RTC
    // only on the module version, on the devboard this is used for the FI input and IR output
    #if (TULIP_HARDWARE == T_MODULE)
        i2c_initialize();           // initialize the I2C port for the RTC
        pcf8520_reset();            // reset the RTC
    #endif

    sdcard_init();     // initialize the FatFS system and uSD card SPI interface


    // prepare the HP-IL loop
    HPIL_init();

    measure_freqs();

    // get the global persistent settings and initialize if needed
    globsetting.retrieve();         // only works if PWO was low, but we do not check here
    if (!globsetting.is_initialized()) {
        globsetting.set_default();
        globsetting.save();
    }

    // initialize trace and printbuffers 
    TraceBuffer_init();     // only if used, do this dynamic in the future
    PrintBuffer_init();     // only if used, do this dynamic in the future
    WandBuffer_init();      // only if used, do this dynamic in the future
   
    initCliBinding();                               // initialization of the embedded-cli

    welcome();                                      // show welcome message on the 'old' user interface
    gpio_put(ONBOARD_LED, false);                   // switch off LED, this will now light when HP41 bus activity is detected

    // initialize Bank registers
    InitEmulation();

    fram_rommap_init();                             // initialize the FRAM ROM map, only if not already done

    multicore_reset_core1();                        // reset core1 to initial state

    
    multicore_launch_core1(core1_pio);       // launching core1 code, must be done before starting the PIO state machines

    // initialize and start the PIO state machines, report the status in the old CLI
    pio_init();                             // initialize the PIO state machines, this must be done before using them                     
    pio_report();

    adc_init();                             // initialize the ADC
    adc_set_temp_sensor_enabled(true);      // enable the temperature sensor
    adc_select_input(4);                    // select the temperature sensor input

    // below is the main loop, it never ends
    
    while (true) {
        
        PowerMode_task();           // Verify the HP41 power mode

        tud_task();                 // process the USB interfaces required by TinyUSB

        runCLI();                   // process the 'new' embedded CLI

        serial_loop();              // process the 'old' user interface
                                    // we keep it here for debugging and the PWO monitoring just in case

        Trace_task();               // process HP41 Trace/disassembler

        Print_task();               // process any print tasks (serial and/or IR, not HP-IL printing)

        HPIL_task();                // process the HP-IL task for transmitting and receiving frames
                                    // only if HP-IL is active

        // Wand_task();               // process the HP41 wand input emulation

        // add a task to check for uSD card removal or insertion ??
        // see workaround on https://github.com/carlk3/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
        // sd_test_com();            
    }

    return 0;   // of course we will never get here
}