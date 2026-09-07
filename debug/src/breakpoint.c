#include "breakpoint.h"

breakpoint_t breakpoint_table[MAX_BREAKPOINTS] = { 0 };
uint8_t breakpoint_count = 0;

void add_breakpoint(uint64_t addr) {
    breakpoint_t breakpoint = { 
        .addr = addr 
    };

    breakpoint_table[breakpoint_count++] = breakpoint;
}

uint8_t is_breakpoint(uint64_t pc) {
    for (uint8_t i = 0; i < breakpoint_count; ++i) {
        if (breakpoint_table[i].addr == pc)
            return 1;
    }

    return 0;
}
