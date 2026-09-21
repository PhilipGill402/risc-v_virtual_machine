#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "cpu.h"
#include "memory.h"
#include "csrs/csr_def.h"
#include "paging.h"

#define ROOT_TABLE   0x80001000ULL
#define LEVEL1_TABLE 0x80002000ULL
#define LEVEL0_TABLE 0x80003000ULL
#define TARGET_PAGE  0x80004000ULL

#define TEST_VA      0x40000000ULL

#define PTE_V (1ULL << 0)
#define PTE_R (1ULL << 1)
#define PTE_W (1ULL << 2)
#define PTE_X (1ULL << 3)
#define PTE_U (1ULL << 4)
#define PTE_A (1ULL << 6)
#define PTE_D (1ULL << 7)


static uint64_t make_table_pte(uint64_t next_table)
{
    uint64_t ppn = next_table >> 12;

    return (ppn << 10) | PTE_V;
}


static uint64_t make_leaf_pte(uint64_t physical_page, uint64_t flags)
{
    uint64_t ppn = physical_page >> 12;

    return (ppn << 10) | flags;
}


static void clear_test_tables(memory_t *mem)
{
    /*
     * If you already zero all RAM during memory initialization,
     * you technically don't need this.
     *
     * Otherwise clear the 3 page-table pages.
     */
    for (uint64_t offset = 0; offset < 0x1000; offset += 8) {
        mem_write64(mem, ROOT_TABLE + offset, 0);
        mem_write64(mem, LEVEL1_TABLE + offset, 0);
        mem_write64(mem, LEVEL0_TABLE + offset, 0);
    }
}


static void setup_basic_mapping(
    cpu_t *cpu,
    memory_t *mem,
    uint64_t leaf_flags
)
{
    clear_test_tables(mem);

    /*
     * TEST_VA = 0x40000000
     *
     * VPN[2] = 1
     * VPN[1] = 0
     * VPN[0] = 0
     */

    mem_write64(
        mem,
        ROOT_TABLE + 1 * 8,
        make_table_pte(LEVEL1_TABLE)
    );

    mem_write64(
        mem,
        LEVEL1_TABLE + 0 * 8,
        make_table_pte(LEVEL0_TABLE)
    );

    mem_write64(
        mem,
        LEVEL0_TABLE + 0 * 8,
        make_leaf_pte(TARGET_PAGE, leaf_flags)
    );

    /*
     * satp:
     *
     * MODE = 8 (Sv39)
     * PPN = ROOT_TABLE >> 12
     */
    cpu->csrs[CSR_SATP] =
        (8ULL << 60) |
        (ROOT_TABLE >> 12);
}


static void test_bare_mode(cpu_t *cpu, memory_t *mem)
{
    printf("test_bare_mode...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_SATP] = 0;

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            0x81234567,
            ACCESS_LOAD
        );

    assert(result.result == TRANSLATION_SUCCESS);
    assert(result.physical_address == 0x81234567);
}


static void test_m_mode_bypasses_translation(cpu_t *cpu, memory_t *mem)
{
    printf("test_m_mode_bypasses_translation...\n");

    cpu->priviledge = M_MODE;

    /*
     * Even though Sv39 is supposedly enabled,
     * normal M-mode accesses bypass it.
     */
    cpu->csrs[CSR_SATP] =
        (8ULL << 60) |
        (ROOT_TABLE >> 12);

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    assert(result.result == TRANSLATION_SUCCESS);
    assert(result.physical_address == TEST_VA);
}


static void test_basic_load_translation(cpu_t *cpu, memory_t *mem)
{
    printf("test_basic_load_translation...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_R |
        PTE_W |
        PTE_A |
        PTE_D
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    assert(result.result == TRANSLATION_SUCCESS);
    assert(result.physical_address == TARGET_PAGE);
}


static void test_page_offset_preserved(cpu_t *cpu, memory_t *mem)
{
    printf("test_page_offset_preserved...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_R |
        PTE_A
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA + 0x123,
            ACCESS_LOAD
        );

    assert(result.result == TRANSLATION_SUCCESS);
    assert(result.physical_address == TARGET_PAGE + 0x123);
}


static void test_invalid_pte(cpu_t *cpu, memory_t *mem)
{
    printf("test_invalid_pte...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_R |
        PTE_A
        /* V deliberately missing */
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    assert(result.result == LOAD_PAGE_FAULT);
}


static void test_reserved_write_only_pte(cpu_t *cpu, memory_t *mem)
{
    printf("test_reserved_write_only_pte...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_W |
        PTE_A |
        PTE_D
    );

    /*
     * W=1, R=0 is reserved.
     */

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_STORE
        );

    assert(result.result == STORE_PAGE_FAULT);
}


static void test_load_permission(cpu_t *cpu, memory_t *mem)
{
    printf("test_load_permission...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    /*
     * Executable but not readable.
     * MXR is disabled.
     */
    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_X |
        PTE_A
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    assert(result.result == LOAD_PAGE_FAULT);
}


static void test_mxr(cpu_t *cpu, memory_t *mem)
{
    printf("test_mxr...\n");

    cpu->priviledge = S_MODE;

    /*
     * Enable MXR, bit 19.
     */
    cpu->csrs[CSR_MSTATUS] = (1ULL << 19);

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_X |
        PTE_A
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    /*
     * X=1 with MXR=1 means load is allowed.
     */
    assert(result.result == TRANSLATION_SUCCESS);
    assert(result.physical_address == TARGET_PAGE);
}


static void test_store_requires_w(cpu_t *cpu, memory_t *mem)
{
    printf("test_store_requires_w...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_R |
        PTE_A |
        PTE_D
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_STORE
        );

    assert(result.result == STORE_PAGE_FAULT);
}


static void test_fetch_requires_x(cpu_t *cpu, memory_t *mem)
{
    printf("test_fetch_requires_x...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_R |
        PTE_A
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_FETCH
        );

    assert(result.result == FETCH_PAGE_FAULT);
}


static void test_user_page(cpu_t *cpu, memory_t *mem)
{
    printf("test_user_page...\n");

    cpu->priviledge = U_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_R |
        PTE_U |
        PTE_A
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    assert(result.result == TRANSLATION_SUCCESS);
    assert(result.physical_address == TARGET_PAGE);
}


static void test_user_cannot_access_supervisor_page(
    cpu_t *cpu,
    memory_t *mem
)
{
    printf("test_user_cannot_access_supervisor_page...\n");

    cpu->priviledge = U_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_R |
        PTE_A
        /* U intentionally zero */
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    assert(result.result == LOAD_PAGE_FAULT);
}


static void test_supervisor_sum(cpu_t *cpu, memory_t *mem)
{
    printf("test_supervisor_sum...\n");

    cpu->priviledge = S_MODE;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_R |
        PTE_U |
        PTE_A
    );

    /*
     * SUM disabled.
     */
    cpu->csrs[CSR_MSTATUS] = 0;

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    assert(result.result == LOAD_PAGE_FAULT);

    /*
     * Now enable SUM, bit 18.
     */
    cpu->csrs[CSR_MSTATUS] = (1ULL << 18);

    result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    assert(result.result == TRANSLATION_SUCCESS);
}


static void test_supervisor_cannot_fetch_user_page(
    cpu_t *cpu,
    memory_t *mem
)
{
    printf("test_supervisor_cannot_fetch_user_page...\n");

    cpu->priviledge = S_MODE;

    /*
     * Even SUM=1 must not allow instruction fetch
     * from a U page.
     */
    cpu->csrs[CSR_MSTATUS] = (1ULL << 18);

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_X |
        PTE_U |
        PTE_A
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_FETCH
        );

    assert(result.result == FETCH_PAGE_FAULT);
}


static void test_accessed_bit(cpu_t *cpu, memory_t *mem)
{
    printf("test_accessed_bit...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_R
        /* A deliberately zero */
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    assert(result.result == LOAD_PAGE_FAULT);
}


static void test_dirty_bit(cpu_t *cpu, memory_t *mem)
{
    printf("test_dirty_bit...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_R |
        PTE_W |
        PTE_A
        /* D deliberately zero */
    );

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_STORE
        );

    assert(result.result == STORE_PAGE_FAULT);
}


static void test_noncanonical_address(cpu_t *cpu, memory_t *mem)
{
    printf("test_noncanonical_address...\n");

    cpu->priviledge = S_MODE;

    setup_basic_mapping(
        cpu,
        mem,
        PTE_V |
        PTE_R |
        PTE_A
    );

    /*
     * Bit 39 set while bit 38 is zero.
     * Not a valid Sv39 canonical address.
     */
    uint64_t bad_va = 1ULL << 39;

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            bad_va,
            ACCESS_LOAD
        );

    assert(result.result == LOAD_PAGE_FAULT);
}


static void test_level0_nonleaf_fault(cpu_t *cpu, memory_t *mem)
{
    printf("test_level0_nonleaf_fault...\n");

    cpu->priviledge = S_MODE;
    cpu->csrs[CSR_MSTATUS] = 0;

    clear_test_tables(mem);

    mem_write64(
        mem,
        ROOT_TABLE + 1 * 8,
        make_table_pte(LEVEL1_TABLE)
    );

    mem_write64(
        mem,
        LEVEL1_TABLE,
        make_table_pte(LEVEL0_TABLE)
    );

    /*
     * A V-only PTE is a non-leaf.
     * At level zero that is invalid because there
     * is nowhere else to walk.
     */
    mem_write64(
        mem,
        LEVEL0_TABLE,
        make_table_pte(TARGET_PAGE)
    );

    cpu->csrs[CSR_SATP] =
        (8ULL << 60) |
        (ROOT_TABLE >> 12);

    translation_result_t result =
        translate_address(
            cpu,
            mem,
            TEST_VA,
            ACCESS_LOAD
        );

    assert(result.result == LOAD_PAGE_FAULT);
}


void run_sv39_tests(cpu_t *cpu, memory_t *mem)
{
    test_bare_mode(cpu, mem);
    test_m_mode_bypasses_translation(cpu, mem);

    test_basic_load_translation(cpu, mem);
    test_page_offset_preserved(cpu, mem);

    test_invalid_pte(cpu, mem);
    test_reserved_write_only_pte(cpu, mem);

    test_load_permission(cpu, mem);
    test_mxr(cpu, mem);
    test_store_requires_w(cpu, mem);
    test_fetch_requires_x(cpu, mem);

    test_user_page(cpu, mem);
    test_user_cannot_access_supervisor_page(cpu, mem);
    test_supervisor_sum(cpu, mem);
    test_supervisor_cannot_fetch_user_page(cpu, mem);

    test_accessed_bit(cpu, mem);
    test_dirty_bit(cpu, mem);

    test_noncanonical_address(cpu, mem);
    test_level0_nonleaf_fault(cpu, mem);

    printf("All Sv39 tests passed!\n");
}

int main() {
    cpu_t cpu = cpu_init();
    memory_t mem = memory_init();

    run_sv39_tests(&cpu, &mem);
}
