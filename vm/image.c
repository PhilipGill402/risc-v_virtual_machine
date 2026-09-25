#include <stdint.h>

#define UART_BASE 0x10000000UL
#define UART_THR  0x00

static inline void uart_putc(char c) {
    volatile uint8_t *uart = (volatile uint8_t *)(UART_BASE + UART_THR);
    *uart = (uint8_t)c;
}


__attribute__((section(".text.start")))
void _start(void) {
    uart_putc('A');

    while (1) {
        __asm__ volatile ("wfi");
    }
}
