#include "pio_uart.h"

#include <stddef.h>

#include "hardware/clocks.h"
#include "pico/time.h"
#include "pio_uart.pio.h"

static float pio_uart_calc_clkdiv(uint baud) {
    return (float)clock_get_hz(clk_sys) / ((float)baud * 8.0f);
}

void pio_uart_tx_init(pio_uart_tx_t *uart, PIO pio, uint sm, uint tx_pin, uint baud) {
    uart->pio = pio;
    uart->sm = sm;
    uart->pin = tx_pin;
    uart->baud = baud;
    uart->offset = pio_add_program(pio, &pio_uart_tx_program);

    pio_uart_tx_program_init(pio, sm, uart->offset, tx_pin, pio_uart_calc_clkdiv(baud));
}

void pio_uart_rx_init(pio_uart_rx_t *uart, PIO pio, uint sm, uint rx_pin, uint baud) {
    uart->pio = pio;
    uart->sm = sm;
    uart->pin = rx_pin;
    uart->baud = baud;
    uart->offset = pio_add_program(pio, &pio_uart_rx_program);

    pio_uart_rx_program_init(pio, sm, uart->offset, rx_pin, pio_uart_calc_clkdiv(baud));
}

void pio_uart_putc(const pio_uart_tx_t *uart, uint8_t ch) {
    pio_sm_put_blocking(uart->pio, uart->sm, ch);
}

void pio_uart_write(const pio_uart_tx_t *uart, const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        pio_uart_putc(uart, data[i]);
    }
}

bool pio_uart_rx_available(const pio_uart_rx_t *uart) {
    return !pio_sm_is_rx_fifo_empty(uart->pio, uart->sm);
}

uint8_t pio_uart_getc_blocking(const pio_uart_rx_t *uart) {
    return (uint8_t)pio_sm_get_blocking(uart->pio, uart->sm);
}

bool pio_uart_getc_timeout_us(const pio_uart_rx_t *uart, uint8_t *ch, uint32_t timeout_us) {
    absolute_time_t deadline = make_timeout_time_us(timeout_us);
    while (!time_reached(deadline)) {
        if (pio_uart_rx_available(uart)) {
            *ch = (uint8_t)pio_sm_get(uart->pio, uart->sm);
            return true;
        }
    }
    return false;
}
