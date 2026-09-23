#include "timer.h"
#include "log.h"

void timer_reset(timer_t* timer) {
    timer->msip = 0; 
    timer->mtime = 0;
    timer->mtimecmp = UINT64_MAX;
}

void timer_tick(timer_t* timer) {
    timer->mtime++;
}

uint64_t timer_read64(timer_t* timer, uint64_t offset) {
    switch (offset) {
        case TIMER_MSIP: return timer->msip & 0x1;
        case TIMER_MTIMECMP: return timer->mtimecmp;
        case TIMER_MTIME: return timer->mtime;
        default: log_error("read from unimplemented timer offset 0x%llx\n", offset); return 0;
    }
}

void timer_write64(timer_t* timer, uint64_t offset, uint64_t value) {
    switch (offset) {
        case TIMER_MSIP: timer->msip = value & 0x1;
        case TIMER_MTIMECMP: timer->mtimecmp = value; return;
        case TIMER_MTIME: timer->mtime = value; return;
        default: log_error("write to unimplemented timer offset 0x%llx\n", offset); return; 
    }
}
