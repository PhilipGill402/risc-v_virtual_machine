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

uint32_t timer_read32(timer_t* timer, uint64_t offset) {
    switch(offset) {
        case TIMER_MSIP: return timer->msip & 0x1;
        case TIMER_MTIMECMP: return (uint32_t)(timer->mtimecmp & 0xFFFFFFFFULL);
        case TIMER_MTIMECMP + 4: return (uint32_t)(timer->mtimecmp >> 32);
        case TIMER_MTIME: return (uint32_t)(timer->mtime & 0xFFFFFFFFULL);
        case TIMER_MTIME +  4: return (uint32_t)(timer->mtime >> 32);
        default: log_error("read from unimplemented timer offset 0x%llx\n", offset); return 0;
    }
}

uint64_t timer_read64(timer_t* timer, uint64_t offset) {
    switch (offset) {
        case TIMER_MSIP: return timer->msip & 0x1;
        case TIMER_MTIMECMP: return timer->mtimecmp;
        case TIMER_MTIME: return timer->mtime;
        default: log_error("read from unimplemented timer offset 0x%llx\n", offset); return 0;
    }
}

void timer_write32(timer_t* timer, uint64_t offset, uint32_t value) {
    switch(offset) {
        case TIMER_MSIP: timer->msip = value & 0x1; break;
        case TIMER_MTIMECMP: (timer->mtimecmp & 0xFFFFFFFF00000000ULL) | (uint64_t)value; break;
        case TIMER_MTIMECMP + 4: (timer->mtimecmp & 0x00000000FFFFFFFFULL) | ((uint64_t)value << 32); break;
        case TIMER_MTIME: (timer->mtime & 0xFFFFFFFF00000000ULL) | (uint64_t)value; break;
        case TIMER_MTIME +  4: (timer->mtime & 0x00000000FFFFFFFFULL) | ((uint64_t)value << 32); break;
        default: log_error("read from unimplemented timer offset 0x%llx\n", offset);
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
