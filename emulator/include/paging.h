#ifndef EMULATOR_INCLUDE_PAGING_H_
#define EMULATOR_INCLUDE_PAGING_H_

#include <stdint.h>
#include "memory.h"

typedef struct cpu cpu_t;

typedef enum translation_error {
        TRANSLATION_SUCCESS,
        INSTRUCTION_PAGE_FAULT,
        LOAD_PAGE_FAULT,
        STORE_PAGE_FAULT
} translation_error_t

typedef struct translation_result {
    uint64_t physical_address;
    translation_error_t result; 
} translation_result_t;

typedef enum access_type {
    ACCESS_STORE,
    ACCESS_FETCH,
    ACCESS_LOAD
} access_type_t;

translation_result_t translate_address(cpu_t* cpu, memory_t* mem, uint64_t virtual_address, access_type_t access_type);

#endif
