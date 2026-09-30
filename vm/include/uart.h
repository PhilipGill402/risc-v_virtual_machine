#ifndef VM_INCLUDE_UART_H_
#define VM_INCLUDE_UART_H_

#include <stdint.h>

#define UART_RBR 0x00  // Receive Buffer Register (read, DLAB=0)
#define UART_THR 0x00  // Transmit Holding Register (write, DLAB=0)
#define UART_DLL 0x00  // Divisor Latch Low (DLAB=1)

#define UART_IER 0x01  // Interrupt Enable Register (DLAB=0)
#define UART_DLM 0x01  // Divisor Latch High (DLAB=1)

#define UART_IIR 0x02  // Interrupt Identification Register (read)
#define UART_FCR 0x02  // FIFO Control Register (write)

#define UART_LCR 0x03  // Line Control Register
#define UART_MCR 0x04  // Modem Control Register
#define UART_LSR 0x05  // Line Status Register
#define UART_MSR 0x06  // Modem Status Register
#define UART_SCR 0x07  // Scratch Register

/* LCR */
#define UART_LCR_DLAB 0x80

/* IER */
#define UART_IER_RX_AVAILABLE 0x01
#define UART_IER_THR_EMPTY    0x02
#define UART_IER_LINE_STATUS  0x04
#define UART_IER_MODEM_STATUS 0x08

/* FCR */
#define UART_FCR_ENABLE_FIFO  0x01
#define UART_FCR_CLEAR_RX     0x02
#define UART_FCR_CLEAR_TX     0x04

/* LSR */
#define UART_LSR_DATA_READY   0x01
#define UART_LSR_OVERRUN      0x02
#define UART_LSR_PARITY       0x04
#define UART_LSR_FRAMING      0x08
#define UART_LSR_BREAK        0x10
#define UART_LSR_THR_EMPTY    0x20
#define UART_LSR_TX_EMPTY     0x40
#define UART_LSR_FIFO_ERROR   0x80

/* IIR */
#define UART_IIR_NO_INTERRUPT 0x01
#define UART_IIR_THR_EMPTY    0x02
#define UART_IIR_RX_AVAILABLE 0x04
#define UART_IIR_LINE_STATUS  0x06

#define UART_IIR_FIFO_ENABLED 0xC0

#define UART_FIFO_SIZE 16

typedef struct uart {
    uint8_t rx_fifo[UART_FIFO_SIZE];
    uint8_t tx_fifo[UART_FIFO_SIZE];

    uint16_t rx_head;
    uint16_t rx_tail;
    uint16_t rx_count;

    uint16_t tx_head;
    uint16_t tx_tail;
    uint16_t tx_count;

    uint8_t thre_pending;

    uint8_t dll;
    uint8_t rbr;
    uint8_t dlm;
    uint8_t ier;
    uint8_t iir;
    uint8_t fcr;
    uint8_t lcr;
    uint8_t mcr;
    uint8_t lsr;
    uint8_t scr;
} uart_t;

uart_t uart_init();
void uart_handle_host_input(uart_t* uart);

uint8_t uart_read8(uart_t* uart, uint64_t offset);
//uint16_t uart_read16(uart_t* uart, uint64_t offset);
//uint32_t uart_read32(uart_t* uart, uint64_t offset);
//uint64_t uart_read64(uart_t* uart, uint64_t offset);

void uart_write8(uart_t* uart, uint64_t offset, uint8_t value);
//void uart_write16(uart_t* uart, uint64_t offset, uint16_t value);
//void uart_write32(uart_t* uart, uint64_t offset, uint32_t value);
//void uart_write64(uart_t* uart, uint64_t offset, uint64_t value);

#endif
