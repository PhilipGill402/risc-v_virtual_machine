#include "uart.h"
#include "log.h"
#include <stdio.h>

uart_t uart_init() {
    uart_t uart = { 0 };

    return uart;
}

uint8_t uart_read8(uart_t* uart, uint64_t offset) {
    switch (offset) {
        case 0x00: {
            if (uart->lcr & UART_LCR_DLAB)
                return uart->dll;
            else
                return 0; // uart->rbr
        }

        case 0x01: {
            if (uart->lcr & UART_LCR_DLAB)
                return uart->dlm;
            else
                return uart->ier;
        }
        
        case UART_IIR: {
            uint8_t fifo = (uart->fcr & UART_FCR_ENABLE_FIFO) ? UART_IIR_FIFO_ENABLED : 0;
        
            

            if (uart->ier & UART_IER_THR_EMPTY && uart->thre_pending) {
                uart->thre_pending = 0;
                return fifo | UART_IIR_THR_EMPTY;
            }

            return fifo | UART_IIR_NO_INTERRUPT;
        }

        case UART_LCR: return uart->lcr;
        case UART_MCR: return uart->mcr;
        case UART_LSR: return UART_LSR_THR_EMPTY | UART_LSR_TX_EMPTY;
        case UART_MSR: return 0x00;
        case UART_SCR: return uart->scr;
        default: log_error("ignoring attempted read to unimplemented UART register at offset 0x%llx\n", offset); return 0;
    }
}

//uint16_t uart_read16(uart_t* uart, uint64_t offset);
//uint32_t uart_read32(uart_t* uart, uint64_t offset);
//uint64_t uart_read64(uart_t* uart, uint64_t offset);

void uart_write8(uart_t* uart, uint64_t offset, uint8_t value) {
    switch (offset) {
        case 0x00: {
            if (uart->lcr & UART_LCR_DLAB) {
                uart->dll = value;
            } else {
                putchar(value);
                fflush(stdout);
                uart->thre_pending = 1;
            }
            break;
        }

        case 0x01: {
            if (uart->lcr & UART_LCR_DLAB)
                uart->dlm = value;
            else {
                uint8_t old_ier = uart->ier;
                uart->ier = value;

                if (!(uart->ier & UART_IER_THR_EMPTY))
                    uart->thre_pending = 0;

                if (!(old_ier & UART_IER_THR_EMPTY) && (uart->ier & UART_IER_THR_EMPTY) && uart->tx_count == 0)
                    uart->thre_pending = 1;
            }
            break;
        }

        case UART_FCR: { 
            if (value & UART_FCR_CLEAR_RX)
                uart->rx_count = 0;
            
            if (value & UART_FCR_CLEAR_TX)
                uart->tx_count = 0;
            
            uart->fcr = value & ~(UART_FCR_CLEAR_RX | UART_FCR_CLEAR_TX); break;
        }

        case UART_LCR: uart->lcr = value; break;
        case UART_MCR: uart->mcr = value; break;
        case UART_LSR: break; // read only
        case UART_MSR: break; // read only
        case UART_SCR: uart->scr = value; break;
        default: log_error("ignoring attempted write to unimplemented UART register at offset 0x%llx\n", offset); break;
    }
}

//void uart_write16(uart_t* uart, uint64_t offset, uint16_t value);
//void uart_write32(uart_t* uart, uint64_t offset, uint32_t value);
//void uart_write64(uart_t* uart, uint64_t offset, uint64_t value);
