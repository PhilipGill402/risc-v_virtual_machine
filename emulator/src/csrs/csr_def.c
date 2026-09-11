#include "csrs/csr_def.h"

csr_descriptor_t csr_table[4096] = { 0 };

uint64_t set_bit(uint64_t value, uint8_t bit_num, uint8_t bit) {
    if (bit)
        value |= 1ULL << bit_num;
    else
        value &= ~(1ULL << bit_num);

    return value;
}

