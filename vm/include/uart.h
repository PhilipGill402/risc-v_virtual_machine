#ifndef VM_INCLUDE_UART_H_
#define VM_INCLUDE_UART_H_

#include <stdint.h>

#define UART_RBR 0x00
#define UART_THR 0x00
#define UART_IER 0x01
#define UART_IIR 0x02
#define UART_FCR 0x02
#define UART_LCR 0x03
#define UART_MCR 0x04
#define UART_LSR 0x05

typedef struct uart {
    uint8_t dll;
    uint8_t rbr;
    uint8_t dlm;
    uint8_t ier;
    uint8_t fcr;
    uint8_t iir;
    uint8_t lcr;
    uint8_t mcr;
    uint8_t lsr;
} uart_t;

uart_t uart_init();
void uart_reset(uart_t* uart);

uint8_t uart_read8(uart_t* uart, uint64_t offset);
//uint16_t uart_read16(uart_t* uart, uint64_t offset);
//uint32_t uart_read32(uart_t* uart, uint64_t offset);
//uint64_t uart_read64(uart_t* uart, uint64_t offset);

void uart_write8(uart_t* uart, uint64_t offset, uint8_t value);
//void uart_write16(uart_t* uart, uint64_t offset, uint16_t value);
//void uart_write32(uart_t* uart, uint64_t offset, uint32_t value);
//void uart_write64(uart_t* uart, uint64_t offset, uint64_t value);

#endif
