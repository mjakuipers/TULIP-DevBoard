#ifndef PIO_UART_H
#define PIO_UART_H

#include "pico/types.h"
#include "hardware/pio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    PIO pio;
    uint sm;
    uint offset;
    uint pin;
    uint baud;
} pio_uart_tx_t;

typedef struct {
    PIO pio;
    uint sm;
    uint offset;
    uint pin;
    uint baud;
} pio_uart_rx_t;

void pio_uart_tx_init(pio_uart_tx_t *uart, PIO pio, uint sm, uint tx_pin, uint baud);
void pio_uart_rx_init(pio_uart_rx_t *uart, PIO pio, uint sm, uint rx_pin, uint baud);

void pio_uart_putc(const pio_uart_tx_t *uart, uint8_t ch);
void pio_uart_write(const pio_uart_tx_t *uart, const uint8_t *data, size_t len);

bool pio_uart_rx_available(const pio_uart_rx_t *uart);
uint8_t pio_uart_getc_blocking(const pio_uart_rx_t *uart);
bool pio_uart_getc_timeout_us(const pio_uart_rx_t *uart, uint8_t *ch, uint32_t timeout_us);

#ifdef __cplusplus
}
#endif

#endif
