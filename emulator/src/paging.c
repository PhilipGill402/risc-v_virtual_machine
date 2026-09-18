#include "paging.h"
#include "cpu.h"
#include "log.h"

static translation_error_t access_type_to_error(access_type_t access_type) {
    switch (access_type) {
        case ACCESS_STORE: return STORE_PAGE_FAULT;
        case ACCESS_LOAD: return LOAD_PAGE_FAULT;
        case ACCESS_FETCH: return INSTRUCTION_PAGE_FAULT;
    } 
}

translation_result_t translate_address(cpu_t* cpu, memory_t* mem, uint64_t virtual_address, access_type_t access_type) {
    uint64_t satp = cpu->csrs[CSR_SATP];
    uint8_t mode = (satp >> 60) & 0xF;
    
    if (mode == 0) {    // direct mode
        translation_result_t result = {
            .physical_address = virtual_address,
            .result = TRANSLATION_SUCCESS,
        };

        return result;
    } else if (mode == 8) { // Sv39
        uint64_t ppn = satp & ((1ULL << 44) - 1);
        uint64_t vpn[3];
        vpn[0] = (virtual_address >> 12) & 0x1FF;
        vpn[1] = (virtual_address >> 21) & 0x1FF;
        vpn[2] = (virtual_address >> 30) & 0x1FF;

        uint64_t offset = virtual_address & 0xFFF;
        uint64_t table = ppn << 12;
        uint8_t level = 2;

        while (1) {
            uint64_t pte_addr = table + vpn[level] * 8;
            uint64_t pte = mem_read64(pte_addr);

            // check flags
        }
        

    } else {
        log_error("Unrecognized mode in SATP (%d)\n", mode);

        translation_result_t result = {
            .physical_address = 0,
            .result = access_type_to_error(access_type),
        };

        return result;
    }
}
