/*
 * powermodes.c
 *
 * This file is part of the TULIP4041 project.
 * 
 * USE AT YOUR OWN RISK
 *
 * use of powermodes.h is commented out, no power management in this version!
 */

/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#include "powermodes.h"
// #include "rosc.h"

static powman_power_state off_state;
static powman_power_state on_state;

// Initialise everything
void powman_example_init(uint64_t abs_time_ms) {
    // start powman and set the time
    powman_timer_start();
    powman_timer_set_ms(abs_time_ms);

    // Allow power down when debugger connected
    powman_set_debug_power_request_ignored(true);

    // Power states
    powman_power_state P1_7 = POWMAN_POWER_STATE_NONE;

    powman_power_state P0_3 = POWMAN_POWER_STATE_NONE;
    P0_3 = powman_power_state_with_domain_on(P0_3, POWMAN_POWER_DOMAIN_SWITCHED_CORE);
    P0_3 = powman_power_state_with_domain_on(P0_3, POWMAN_POWER_DOMAIN_XIP_CACHE);

    off_state = P1_7;
    on_state = P0_3;
}

// Initiate power off
static int powman_example_off(void) {
    // Get ready to power off
    stdio_flush();

    // Set power states
    bool valid_state = powman_configure_wakeup_state(off_state, on_state);
    if (!valid_state) {
        return PICO_ERROR_INVALID_STATE;
    }

    // reboot to main
    powman_hw->boot[0] = 0;
    powman_hw->boot[1] = 0;
    powman_hw->boot[2] = 0;
    powman_hw->boot[3] = 0;

    // Switch to required power state
    int rc = powman_set_power_state(off_state);
    if (rc != PICO_OK) {
        return rc;
    }

    // Power down
    while (true) __wfi();
}

// Power off until a gpio goes high
int powman_example_off_until_gpio_high(int gpio) {
    gpio_init(gpio);
    gpio_set_dir(gpio, false);
    if (gpio_get(gpio)) {
        printf("Waiting for gpio %d to go low\n", gpio);
        while(gpio_get(gpio)) {
            sleep_ms(100);
        }
    }
    printf("Powering off until GPIO %d goes high\n", gpio);
    powman_enable_gpio_wakeup(0, gpio, false, true);
    return powman_example_off();
}

// Power off until a gpio goes low
int powman_example_off_until_gpio_low(int gpio) {
    gpio_init(gpio);
    gpio_set_dir(gpio, false);
    if (!gpio_get(gpio)) {
        printf("Waiting for gpio %d to go high\n", gpio);
        while(!gpio_get(gpio)) {
            sleep_ms(100);
        }
    }
    printf("Powering off until GPIO %d goes low\n", gpio);
    powman_enable_gpio_wakeup(0, gpio, false, false);
    return powman_example_off();
}

// Power off until an absolute time
int powman_example_off_until_time(uint64_t abs_time_ms) {
    // Start powman timer and turn off
    printf("Powering off for %" PRIu64 "ms\n", abs_time_ms - powman_timer_get_ms());
    powman_enable_alarm_wakeup_at_ms(abs_time_ms);
    return powman_example_off();
}

// Power off for a number of milliseconds
int powman_example_off_for_ms(uint64_t duration_ms) {
    uint64_t ms = powman_timer_get_ms();
    return powman_example_off_until_time(ms + duration_ms);
}

// Enter RP2350 dormant mode and resume when the selected GPIO goes high
int rp2350_dormant_until_gpio_high(int gpio) {
    gpio_init(gpio);
    gpio_set_dir(gpio, false);

    // Ensure we sleep waiting for a new low->high transition
    if (gpio_get(gpio)) {
        printf("Waiting for gpio %d to go low before dormant\n", gpio);
        while (gpio_get(gpio)) {
            sleep_ms(10);
        }
    }

    printf("Entering dormant mode until GPIO %d goes high\n", gpio);
    stdio_flush();

    gpio_acknowledge_irq(gpio, GPIO_IRQ_EDGE_RISE);
    gpio_set_dormant_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE, true);

    // Blocks here until dormant wake source triggers and XOSC is stable again
    xosc_dormant();

    gpio_set_dormant_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE, false);
    gpio_acknowledge_irq(gpio, GPIO_IRQ_EDGE_RISE);

    printf("Woke from dormant mode on GPIO %d high\n", gpio);
    return PICO_OK;
}




