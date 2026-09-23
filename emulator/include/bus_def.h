#ifndef EMULATOR_INCLUDE_BUS_DEF_H_
#define EMULATOR_INCLUDE_BUS_DEF_H_

typedef struct bus {
    void *ctx;

    uint8_t (*read8)(void* ctx, uint64_t addr);
    uint16_t (*read16)(void* ctx, uint64_t addr);
    uint32_t (*read32)(void* ctx, uint64_t addr);
    uint64_t (*read64)(void* ctx, uint64_t addr);

    void (*write8)(void* ctx, uint64_t addr, uint8_t value);
    void (*write16)(void* ctx, uint64_t addr, uint16_t value);
    void (*write32)(void* ctx, uint64_t addr, uint32_t value);
    void (*write64)(void* ctx, uint64_t addr, uint64_t value);
} bus_t;

#endif
