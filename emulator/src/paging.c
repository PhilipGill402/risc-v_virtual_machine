#include "paging.h"
#include "cpu.h"
#include "log.h"
#include "csrs/csr_def.h"

static translation_error_t access_type_to_error(access_type_t access_type) {
    switch (access_type) {
        case ACCESS_STORE: return STORE_PAGE_FAULT;
        case ACCESS_LOAD: return LOAD_PAGE_FAULT;
        case ACCESS_FETCH: return FETCH_PAGE_FAULT;
    } 
}

static uint8_t check_upper_bits(uint64_t virtual_address) {
    uint64_t sign = (virtual_address >> 38) & 0x1;
    uint64_t upper = virtual_address >> 39;
    
    // VA bits 63:39 must equal VA bit 38
    if ((!sign && upper != 0) || (sign && upper != 0x1FFFFFFULL))
        return 0;

    return 1;
}

static uint8_t check_priviledge(cpu_t* cpu, uint8_t u_bit, access_type_t access_type) {
    uint8_t sum = (uint8_t)(cpu->csrs[CSR_MSTATUS] >> 18) & 0x1;
    uint8_t mxr = (uint8_t)(cpu->csrs[CSR_MSTATUS] >> 19) & 0x1;
    if (cpu->priviledge == U_MODE && !u_bit) { // user process can't access this page
        return 0; 
    } else if (cpu->priviledge == S_MODE && u_bit) {
        // supervisor code can't execute user code 
        if (access_type == ACCESS_FETCH)
            return 0; 
        
        // sstatus.SUM must be set
        if (!sum)
            return 0;
    }

    return 1;
}

static uint64_t get_physical_address(uint64_t* pte_ppn, uint64_t* vpn, uint64_t offset, uint8_t level) {
    uint64_t pa_ppn0;
    uint64_t pa_ppn1;
    uint64_t pa_ppn2;
    if (level == 2) {
        pa_ppn2 = pte_ppn[2];
        pa_ppn1 = vpn[1];
        pa_ppn0 = vpn[0];
    } else if (level == 1) {
        pa_ppn2 = pte_ppn[2];
        pa_ppn1 = pte_ppn[1];
        pa_ppn0 = vpn[0];
    } else {
        pa_ppn2 = pte_ppn[2];
        pa_ppn1 = pte_ppn[1];
        pa_ppn0 = pte_ppn[0];
    }

    return (pa_ppn2 << 30) | (pa_ppn1 << 21) | (pa_ppn0 << 12) | offset; 
}

static translation_result_t sv39(cpu_t* cpu, memory_t* mem, uint64_t virtual_address, access_type_t access_type) {
    translation_result_t result; 
        
    if (!check_upper_bits(virtual_address))             
        goto page_fault;

    uint64_t satp = cpu->csrs[CSR_SATP];
    uint64_t ppn = satp & ((1ULL << 44) - 1);
    uint64_t vpn[3];
    vpn[0] = (virtual_address >> 12) & 0x1FF;
    vpn[1] = (virtual_address >> 21) & 0x1FF;
    vpn[2] = (virtual_address >> 30) & 0x1FF;

    uint64_t offset = virtual_address & 0xFFF;
    uint64_t table = ppn << 12;

    for (int8_t i = 2; i >= 0; --i) {
        uint64_t pte_addr = table + vpn[i] * 8;
        uint64_t pte = mem_read64(mem, pte_addr);
        
        // bits 63:54 must be zero unles Svnapot or Svpbmt is implemented
        if ((pte >> 54) != 0)
            goto page_fault;

        uint64_t pte_ppn[3];
        pte_ppn[0] = (pte >> 10) & 0x1FF;
        pte_ppn[1] = (pte >> 19) & 0x1FF;
        pte_ppn[2] = (pte >> 28) & 0x3FFFFFF;
                
        uint8_t d_bit = (uint8_t)(pte >> 7) & 0x1;
        uint8_t a_bit = (uint8_t)(pte >> 6) & 0x1; 
        uint8_t u_bit = (uint8_t)(pte >> 4) & 0x1;
        uint8_t x_bit = (uint8_t)(pte >> 3) & 0x1;
        uint8_t w_bit = (uint8_t)(pte >> 2) & 0x1;
        uint8_t r_bit = (uint8_t)(pte >> 1) & 0x1;
        uint8_t v_bit = (uint8_t)(pte) & 0x1;
        
        // these configurations are reserved
        if ((w_bit && !r_bit) || !v_bit)
            goto page_fault; 

        // this is not a leaf entry
        if (!r_bit && !x_bit) {
            if (i == 0)
                goto page_fault;

            uint64_t pte_ppn_full = (pte_ppn[2] << 18) | (pte_ppn[1] << 9) | pte_ppn[0];
            table = pte_ppn_full << 12;
            continue;
        }

        // TODO: check for misaligned superpage
        if (i == 2 && (pte_ppn[1] != 0 || pte_ppn[0] != 0))
            goto page_fault;
        else if (i == 1 && pte_ppn[0] != 0)
            goto page_fault;

        if (!check_priviledge(cpu, u_bit, access_type))
            goto page_fault;
        
        // correct access bits are not set
        uint8_t mxr = (uint8_t)(cpu->csrs[CSR_MSTATUS] >> 19) & 0x1;
        if (access_type == ACCESS_STORE && !w_bit)
            goto page_fault; 
        else if (access_type == ACCESS_LOAD && !r_bit && !(mxr && x_bit))
            goto page_fault; 
        else if (access_type == ACCESS_FETCH && !x_bit)
            goto page_fault;

        if (!a_bit || (access_type == ACCESS_STORE && !d_bit)) {
            // Svade throws an exception here
            goto page_fault;
        }

        result.physical_address = get_physical_address(pte_ppn, vpn, offset, i);
        result.result = TRANSLATION_SUCCESS;
        return result;
    }

page_fault:
    result.physical_address = 0;
    result.result = access_type_to_error(access_type);
    return result;
}

translation_result_t translate_address(cpu_t* cpu, memory_t* mem, uint64_t virtual_address, access_type_t access_type) {
    uint64_t satp = cpu->csrs[CSR_SATP];
    uint8_t mode = (satp >> 60) & 0xF;
    
    if (mode == 0 || cpu->priviledge == M_MODE) {    // direct mode
        translation_result_t result = {
            .physical_address = virtual_address,
            .result = TRANSLATION_SUCCESS,
        };

        return result;
    } else if (mode == 8) { // Sv39
        return sv39(cpu, mem, virtual_address, access_type); 
    } else {
        log_error("Unrecognized mode in SATP (%d)\n", mode);

        translation_result_t result = {
            .physical_address = 0,
            .result = access_type_to_error(access_type),
        };

        return result;
    }
}
