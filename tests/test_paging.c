#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "memory.h"
#include "csrs/csr_def.h"

/*
 * Physical memory layout
 *
 * 0x80001000  root page table
 * 0x80002000  level-1 page table
 * 0x80003000  level-0 page table
 *
 * 0x80004000  physical code page
 * 0x80005000  physical data page
 * 0x80006000  physical store page
 *
 * 0x80007000  M-mode trap vector
 */

#define ROOT_TABLE      0x80001000ULL
#define LEVEL1_TABLE    0x80002000ULL
#define LEVEL0_TABLE    0x80003000ULL

#define CODE_PA         0x80004000ULL
#define DATA_PA         0x80005000ULL
#define STORE_PA        0x80006000ULL

#define TRAP_VECTOR     0x80007000ULL

/*
 * Virtual address layout
 *
 * All of these share:
 *
 * VPN[2] = 1
 * VPN[1] = 0
 *
 * but have different VPN[0] values.
 */

#define CODE_VA         0x40000000ULL
#define DATA_VA         0x40001000ULL
#define STORE_VA        0x40002000ULL
#define UNMAPPED_VA     0x40003000ULL


/* PTE bits */
#define PTE_V (1ULL << 0)
#define PTE_R (1ULL << 1)
#define PTE_W (1ULL << 2)
#define PTE_X (1ULL << 3)
#define PTE_U (1ULL << 4)
#define PTE_A (1ULL << 6)
#define PTE_D (1ULL << 7)


/*
 * RISC-V exception causes
 */
#define EXC_FETCH_PAGE_FAULT 12
#define EXC_LOAD_PAGE_FAULT  13
#define EXC_STORE_PAGE_FAULT 15


/*
 * Instructions used by these tests.
 *
 * addi x5, x0, 42
 * ld   x5, 0(x6)
 * sd   x5, 0(x6)
 */
#define INSTR_ADDI_X5_42     0x02A00293U
#define INSTR_LD_X5_X6      0x00033283U
#define INSTR_SD_X5_X6      0x00533023U


static uint64_t make_table_pte(uint64_t next_table)
{
    uint64_t ppn = next_table >> 12;

    return (ppn << 10) | PTE_V;
}


static uint64_t make_leaf_pte(
    uint64_t physical_page,
    uint64_t flags
)
{
    uint64_t ppn = physical_page >> 12;

    return (ppn << 10) | flags;
}


static void clear_page(memory_t *mem, uint64_t address)
{
    for (uint64_t offset = 0;
         offset < 0x1000;
         offset += 8) {

        mem_write64(mem, address + offset, 0);
    }
}


static void clear_test_memory(memory_t *mem)
{
    clear_page(mem, ROOT_TABLE);
    clear_page(mem, LEVEL1_TABLE);
    clear_page(mem, LEVEL0_TABLE);

    clear_page(mem, CODE_PA);
    clear_page(mem, DATA_PA);
    clear_page(mem, STORE_PA);
}


/*
 * Creates:
 *
 * ROOT_TABLE
 *     |
 *     | VPN[2] = 1
 *     v
 * LEVEL1_TABLE
 *     |
 *     | VPN[1] = 0
 *     v
 * LEVEL0_TABLE
 *
 * Level zero then contains:
 *
 * entry 0 -> CODE_PA
 * entry 1 -> DATA_PA
 * entry 2 -> STORE_PA
 * entry 3 -> INVALID / unmapped
 */
static void setup_page_tables(
    cpu_t *cpu,
    memory_t *mem
)
{
    clear_test_memory(mem);

    /*
     * L2:
     *
     * CODE_VA 0x40000000 has VPN[2] = 1.
     */
    mem_write64(
        mem,
        ROOT_TABLE + (1 * 8),
        make_table_pte(LEVEL1_TABLE)
    );

    /*
     * L1:
     *
     * VPN[1] = 0.
     */
    mem_write64(
        mem,
        LEVEL1_TABLE + (0 * 8),
        make_table_pte(LEVEL0_TABLE)
    );

    /*
     * VPN[0] = 0
     *
     * Supervisor executable page.
     */
    mem_write64(
        mem,
        LEVEL0_TABLE + (0 * 8),
        make_leaf_pte(
            CODE_PA,
            PTE_V |
            PTE_R |
            PTE_X |
            PTE_A
        )
    );

    /*
     * VPN[0] = 1
     *
     * Readable data page.
     */
    mem_write64(
        mem,
        LEVEL0_TABLE + (1 * 8),
        make_leaf_pte(
            DATA_PA,
            PTE_V |
            PTE_R |
            PTE_A
        )
    );

    /*
     * VPN[0] = 2
     *
     * Read/write data page.
     */
    mem_write64(
        mem,
        LEVEL0_TABLE + (2 * 8),
        make_leaf_pte(
            STORE_PA,
            PTE_V |
            PTE_R |
            PTE_W |
            PTE_A |
            PTE_D
        )
    );

    /*
     * VPN[0] = 3 is deliberately left zero.
     *
     * UNMAPPED_VA therefore faults.
     */

    cpu->csrs[CSR_SATP] =
        (8ULL << 60) |
        (ROOT_TABLE >> 12);

    /*
     * Keep page faults in M-mode for these tests.
     *
     * This makes the trap vector a physical address because
     * M-mode bypasses Sv39 in your current implementation.
     */
    cpu->csrs[CSR_MEDELEG] = 0;

    cpu->csrs[CSR_MTVEC] = TRAP_VECTOR;
}


/*
 * ---------------------------------------------------------
 * Test 1
 *
 * Instruction fetch itself goes through Sv39.
 *
 * CODE_VA -> CODE_PA
 *
 * Physical CODE_PA contains:
 *
 *     addi x5, x0, 42
 * ---------------------------------------------------------
 */
static void test_translated_instruction_fetch(memory_t *mem)
{
    printf("test_translated_instruction_fetch...\n");

    cpu_t cpu = cpu_init();

    setup_page_tables(&cpu, mem);

    mem_write32(
        mem,
        CODE_PA,
        INSTR_ADDI_X5_42
    );

    cpu.priviledge = S_MODE;
    cpu.pc = CODE_VA;

    cpu_step(&cpu, mem);

    assert(cpu.regs[5] == 42);
    assert(cpu.pc == CODE_VA + 4);
    assert(cpu.priviledge == S_MODE);

    printf("  PASS\n");
}


/*
 * ---------------------------------------------------------
 * Test 2
 *
 * Load address translation.
 *
 * Code:
 *
 *     ld x5, 0(x6)
 *
 * x6 = DATA_VA
 *
 * DATA_VA -> DATA_PA
 * ---------------------------------------------------------
 */
static void test_translated_load(memory_t *mem)
{
    printf("test_translated_load...\n");

    cpu_t cpu = cpu_init();

    setup_page_tables(&cpu, mem);

    uint64_t expected =
        0x1122334455667788ULL;

    mem_write32(
        mem,
        CODE_PA,
        INSTR_LD_X5_X6
    );

    mem_write64(
        mem,
        DATA_PA,
        expected
    );

    cpu.regs[6] = DATA_VA;

    cpu.priviledge = S_MODE;
    cpu.pc = CODE_VA;

    cpu_step(&cpu, mem);

    assert(cpu.regs[5] == expected);
    assert(cpu.pc == CODE_VA + 4);
    assert(cpu.priviledge == S_MODE);

    printf("  PASS\n");
}


/*
 * ---------------------------------------------------------
 * Test 3
 *
 * Store address translation.
 *
 * Code:
 *
 *     sd x5, 0(x6)
 *
 * x5 = value
 * x6 = STORE_VA
 *
 * STORE_VA -> STORE_PA
 * ---------------------------------------------------------
 */
static void test_translated_store(memory_t *mem)
{
    printf("test_translated_store...\n");

    cpu_t cpu = cpu_init();

    setup_page_tables(&cpu, mem);

    uint64_t expected =
        0xCAFEBABEDEADBEEFULL;

    mem_write32(
        mem,
        CODE_PA,
        INSTR_SD_X5_X6
    );

    cpu.regs[5] = expected;
    cpu.regs[6] = STORE_VA;

    cpu.priviledge = S_MODE;
    cpu.pc = CODE_VA;

    cpu_step(&cpu, mem);

    assert(
        mem_read64(mem, STORE_PA) ==
        expected
    );

    assert(cpu.pc == CODE_VA + 4);
    assert(cpu.priviledge == S_MODE);

    printf("  PASS\n");
}


/*
 * ---------------------------------------------------------
 * Test 4
 *
 * Instruction page fault.
 *
 * PC itself points to an unmapped virtual page.
 *
 * Expected:
 *
 * mcause = 12
 * mepc   = faulting virtual PC
 * mtval  = faulting virtual PC
 * PC     = mtvec
 * mode   = M
 * ---------------------------------------------------------
 */
static void test_instruction_page_fault(memory_t *mem)
{
    printf("test_instruction_page_fault...\n");

    cpu_t cpu = cpu_init();

    setup_page_tables(&cpu, mem);

    cpu.priviledge = S_MODE;
    cpu.pc = UNMAPPED_VA;

    cpu_step(&cpu, mem);
    
    assert(
        cpu.csrs[CSR_MCAUSE] ==
        EXC_FETCH_PAGE_FAULT
    );

    assert(
        cpu.csrs[CSR_MEPC] ==
        UNMAPPED_VA
    );

    assert(
        cpu.csrs[CSR_MTVAL] ==
        UNMAPPED_VA
    );

    assert(cpu.priviledge == M_MODE);
    assert(cpu.pc == TRAP_VECTOR);

    printf("  PASS\n");
}


/*
 * ---------------------------------------------------------
 * Test 5
 *
 * Load page fault.
 *
 * Instruction fetch succeeds:
 *
 *     ld x5, 0(x6)
 *
 * but x6 points at an unmapped virtual page.
 *
 * Expected:
 *
 * mcause = 13
 * mepc   = address of LD
 * mtval  = data VA that failed
 * ---------------------------------------------------------
 */
static void test_load_page_fault(memory_t *mem)
{
    printf("test_load_page_fault...\n");

    cpu_t cpu = cpu_init();

    setup_page_tables(&cpu, mem);

    mem_write32(
        mem,
        CODE_PA,
        INSTR_LD_X5_X6
    );

    cpu.regs[6] = UNMAPPED_VA;

    cpu.priviledge = S_MODE;
    cpu.pc = CODE_VA;

    cpu_step(&cpu, mem);

    assert(
        cpu.csrs[CSR_MCAUSE] ==
        EXC_LOAD_PAGE_FAULT
    );

    /*
     * mepc should contain the PC of the instruction
     * that caused the load fault.
     */
    assert(
        cpu.csrs[CSR_MEPC] ==
        CODE_VA
    );

    /*
     * mtval should contain the virtual DATA address
     * that failed translation.
     */
    assert(
        cpu.csrs[CSR_MTVAL] ==
        UNMAPPED_VA
    );

    assert(cpu.priviledge == M_MODE);
    assert(cpu.pc == TRAP_VECTOR);

    printf("  PASS\n");
}


/*
 * ---------------------------------------------------------
 * Test 6
 *
 * Store page fault.
 *
 * Instruction:
 *
 *     sd x5, 0(x6)
 *
 * x6 points to an unmapped page.
 * ---------------------------------------------------------
 */
static void test_store_page_fault(memory_t *mem)
{
    printf("test_store_page_fault...\n");

    cpu_t cpu = cpu_init();

    setup_page_tables(&cpu, mem);

    mem_write32(
        mem,
        CODE_PA,
        INSTR_SD_X5_X6
    );

    cpu.regs[5] =
        0x123456789ABCDEF0ULL;

    cpu.regs[6] =
        UNMAPPED_VA;

    cpu.priviledge = S_MODE;
    cpu.pc = CODE_VA;

    cpu_step(&cpu, mem);
    
    assert(
        cpu.csrs[CSR_MCAUSE] ==
        EXC_STORE_PAGE_FAULT
    );

    assert(
        cpu.csrs[CSR_MEPC] ==
        CODE_VA
    );

    assert(
        cpu.csrs[CSR_MTVAL] ==
        UNMAPPED_VA
    );

    assert(cpu.priviledge == M_MODE);
    assert(cpu.pc == TRAP_VECTOR);

    printf("  PASS\n");
}


/*
 * ---------------------------------------------------------
 * Test 7
 *
 * A mapped page exists, but permissions don't allow load.
 *
 * CODE_VA is executable, but we'll deliberately try to
 * use the executable page as a data address with MXR=0.
 *
 * This makes sure CPU load integration preserves the
 * permission failure returned by translate_address().
 * ---------------------------------------------------------
 */
static void test_load_permission_page_fault(memory_t *mem)
{
    printf("test_load_permission_page_fault...\n");

    cpu_t cpu = cpu_init();

    setup_page_tables(&cpu, mem);

    mem_write32(
        mem,
        CODE_PA,
        INSTR_LD_X5_X6
    );

    /*
     * CODE_VA maps to an X/R page in the default setup.
     *
     * Make it X-only.
     */
    mem_write64(
        mem,
        LEVEL0_TABLE,
        make_leaf_pte(
            CODE_PA,
            PTE_V |
            PTE_X |
            PTE_A
        )
    );

    /*
     * We still need to execute the LD instruction.
     *
     * So use a second executable mapping for the code.
     * Restore CODE page after setting the instruction
     * by mapping VPN0 0 as RX.
     *
     * Instead, for this test use DATA_VA as the bad
     * execute-only data page.
     */

    mem_write64(
        mem,
        LEVEL0_TABLE,
        make_leaf_pte(
            CODE_PA,
            PTE_V |
            PTE_R |
            PTE_X |
            PTE_A
        )
    );

    mem_write64(
        mem,
        LEVEL0_TABLE + (1 * 8),
        make_leaf_pte(
            DATA_PA,
            PTE_V |
            PTE_X |
            PTE_A
        )
    );

    cpu.csrs[CSR_MSTATUS] &= ~(1ULL << 19);

    cpu.regs[6] = DATA_VA;

    cpu.priviledge = S_MODE;
    cpu.pc = CODE_VA;

    cpu_step(&cpu, mem);

    assert(
        cpu.csrs[CSR_MCAUSE] ==
        EXC_LOAD_PAGE_FAULT
    );

    assert(
        cpu.csrs[CSR_MEPC] ==
        CODE_VA
    );

    assert(
        cpu.csrs[CSR_MTVAL] ==
        DATA_VA
    );

    printf("  PASS\n");
}


/*
 * ---------------------------------------------------------
 * Test 8
 *
 * M-mode still bypasses Sv39.
 *
 * PC is a PHYSICAL address here.
 * ---------------------------------------------------------
 */
static void test_machine_mode_bypass(memory_t *mem)
{
    printf("test_machine_mode_bypass...\n");

    cpu_t cpu = cpu_init();

    setup_page_tables(&cpu, mem);

    mem_write32(
        mem,
        CODE_PA,
        INSTR_ADDI_X5_42
    );

    cpu.priviledge = M_MODE;
    cpu.pc = CODE_PA;

    cpu_step(&cpu, mem);

    assert(cpu.regs[5] == 42);
    assert(cpu.pc == CODE_PA + 4);

    printf("  PASS\n");
}


int main(void)
{
    memory_t mem = memory_init();

    printf("\n");
    printf("===============================\n");
    printf(" Sv39 CPU Integration Tests\n");
    printf("===============================\n\n");

    test_translated_instruction_fetch(&mem);
    test_translated_load(&mem);
    test_translated_store(&mem);

    test_instruction_page_fault(&mem);
    test_load_page_fault(&mem);
    test_store_page_fault(&mem);

    test_load_permission_page_fault(&mem);

    test_machine_mode_bypass(&mem);

    printf("\n");
    printf("===============================\n");
    printf(" ALL INTEGRATION TESTS PASSED\n");
    printf("===============================\n\n");

    return 0;
}
