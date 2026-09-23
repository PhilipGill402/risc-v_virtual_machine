#include "uart.h"
#include "log.h"
#include <stdio.h>

static uint8_t lcr_dlab(uart_t* uart) {
    return (uart->lcr >> 7) & 0x1;
}

uart_t uart_init() {
    uart_t uart = { 0 };
    uart_reset(&uart);

    return uart;
}

void uart_reset(uart_t* uart) {
    uart->dll = 0x00;
    uart->rbr = 0x00;
    uart->dlm = 0x00;
    uart->ier = 0x00;
    uart->fcr = 0xC0;
    uart->iir = 0xC1;
    uart->lcr = 0x00;
    uart->mcr = 0x00;
    uart->lsr = 0x60;
}

uint8_t uart_read8(uart_t* uart, uint64_t offset) {
    switch (offset) {
        case 0x00: {
            if (lcr_dlab(uart))
                return uart->dll;
            else
                return uart->rbr;
        }

        case 0x01: {
            if (lcr_dlab(uart))
                return uart->dlm;
            else
                return uart->ier;
        }
        
        case UART_IIR: return uart->iir;
        case UART_LCR: return uart->lcr;
        case UART_MCR: return uart->mcr;
        case UART_LSR: return uart->lsr;
        default: log_error("ignoring attempted read to unimplemented UART register at offset 0x%llx", offset); return 0;
    }
}

//uint16_t uart_read16(uart_t* uart, uint64_t offset);
//uint32_t uart_read32(uart_t* uart, uint64_t offset);
//uint64_t uart_read64(uart_t* uart, uint64_t offset);

void uart_write8(uart_t* uart, uint64_t offset, uint8_t value) {
    switch (offset) {
        case 0x00: {
            if (lcr_dlab(uart)) {
                uart->dll = value;
            } else {
                putchar(value);
                fflush(stdout);
            }
            break;
        }

        case 0x01: {
            if (lcr_dlab(uart))
                uart->dlm = value;
            else
                uart->ier = value;
            break;
        }

        case UART_FCR: uart->fcr = value; break;
        case UART_LCR: uart->lcr = value; break;
        case UART_MCR: uart->mcr = value; break;
        case UART_LSR: break; // read only
        default: log_error("ignoring attempted write to unimplemented UART register at offset 0x%llx", offset); break;
    }    
}

//void uart_write16(uart_t* uart, uint64_t offset, uint16_t value);
//void uart_write32(uart_t* uart, uint64_t offset, uint32_t value);
//void uart_write64(uart_t* uart, uint64_t offset, uint64_t value);
