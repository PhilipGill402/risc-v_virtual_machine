#ifndef DEBUG_INCLUDE_CLI_BREAKPOINT_H_
#define DEBUG_INCLUDE_CLI_BREAKPOINT_H_

#include <stdint.h>

#define MAX_BREAKPOINTS 64

typedef struct {
    uint64_t addr;
} breakpoint_t;

void add_breakpoint(uint64_t addr);
uint8_t is_breakpoint(uint64_t pc);

extern breakpoint_t breakpoint_table[MAX_BREAKPOINTS];
extern uint8_t breakpoint_count;

#endif
