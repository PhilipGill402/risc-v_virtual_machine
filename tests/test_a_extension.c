#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "memory.h"

#define TEST_ADDR    0x80001000ULL
#define TEST_ADDR_2  0x80002000ULL
#define CODE_ADDR    MEM_BASE

/*
 * A-extension funct5 values.
 * Use your own enum/constants if these already exist.
 */
#define AMOADD   0x00
#define AMOSWAP  0x01
#define LR       0x02
#define SC       0x03
#define AMOXOR   0x04
#define AMOOR    0x08
#define AMOAND   0x0C
#define AMOMIN   0x10
#define AMOMAX   0x14
#define AMOMINU  0x18
#define AMOMAXU  0x1C

/*
 * funct3:
 * 010 = .W
 * 011 = .D
 */
#define AMO_W 0x2
#define AMO_D 0x3

static uint32_t encode_amo(
    uint8_t funct5,
    uint8_t aq,
    uint8_t rl,
    uint8_t rs2,
    uint8_t rs1,
    uint8_t funct3,
    uint8_t rd
)
{
    return
        ((uint32_t)funct5 << 27) |
        ((uint32_t)aq     << 26) |
        ((uint32_t)rl     << 25) |
        ((uint32_t)rs2    << 20) |
        ((uint32_t)rs1    << 15) |
        ((uint32_t)funct3 << 12) |
        ((uint32_t)rd     << 7)  |
        0x2F;
}

static void reset_cpu(cpu_t *cpu)
{
    *cpu = cpu_init();

    cpu->pc = CODE_ADDR;
    cpu->priviledge = M_MODE;

    cpu->reservation.valid = 0;
    cpu->reservation.phys_addr = 0;
    cpu->reservation.size = 0;
}

static void write_instruction(
    memory_t *mem,
    uint64_t address,
    uint32_t instruction
)
{
    mem_write32(mem, address, instruction);
}

/* ----------------------------------------------------- */
/* LR.D                                                  */
/* ----------------------------------------------------- */

static void test_lr_d(cpu_t *cpu, memory_t *mem)
{
    printf("test_lr_d...\n");

    reset_cpu(cpu);

    mem_write64(
        mem,
        TEST_ADDR,
        0x1122334455667788ULL
    );

    cpu->regs[10] = TEST_ADDR;

    uint32_t instruction = encode_amo(
        LR,
        0,
        0,
        0,      // rs2 must be x0
        10,     // rs1
        AMO_D,
        5       // rd
    );

    write_instruction(mem, CODE_ADDR, instruction);

    cpu_step(cpu, mem);

    assert(
        cpu->regs[5] ==
        0x1122334455667788ULL
    );

    assert(cpu->reservation.valid == 1);
    assert(cpu->reservation.phys_addr == TEST_ADDR);
    assert(cpu->reservation.size == 8);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* LR.W sign extension                                   */
/* ----------------------------------------------------- */

static void test_lr_w_sign_extension(cpu_t *cpu, memory_t *mem)
{
    printf("test_lr_w_sign_extension...\n");

    reset_cpu(cpu);

    mem_write32(
        mem,
        TEST_ADDR,
        0xFFFFFFFFU
    );

    cpu->regs[10] = TEST_ADDR;

    uint32_t instruction = encode_amo(
        LR,
        0,
        0,
        0,
        10,
        AMO_W,
        5
    );

    write_instruction(mem, CODE_ADDR, instruction);

    cpu_step(cpu, mem);

    assert(
        cpu->regs[5] ==
        0xFFFFFFFFFFFFFFFFULL
    );

    assert(cpu->reservation.valid == 1);
    assert(cpu->reservation.phys_addr == TEST_ADDR);
    assert(cpu->reservation.size == 4);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* LR replaces previous reservation                      */
/* ----------------------------------------------------- */

static void test_lr_replaces_reservation(
    cpu_t *cpu,
    memory_t *mem
)
{
    printf("test_lr_replaces_reservation...\n");

    reset_cpu(cpu);

    mem_write64(mem, TEST_ADDR, 10);
    mem_write64(mem, TEST_ADDR_2, 20);

    uint32_t lr_d_x5 =
        encode_amo(LR, 0, 0, 0, 10, AMO_D, 5);

    uint32_t lr_d_x6 =
        encode_amo(LR, 0, 0, 0, 10, AMO_D, 6);

    write_instruction(
        mem,
        CODE_ADDR,
        lr_d_x5
    );

    write_instruction(
        mem,
        CODE_ADDR + 4,
        lr_d_x6
    );

    cpu->regs[10] = TEST_ADDR;

    cpu_step(cpu, mem);

    assert(cpu->reservation.phys_addr == TEST_ADDR);

    cpu->regs[10] = TEST_ADDR_2;

    cpu_step(cpu, mem);

    assert(cpu->reservation.valid == 1);
    assert(cpu->reservation.phys_addr == TEST_ADDR_2);
    assert(cpu->reservation.size == 8);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* LR.D -> SC.D success                                  */
/* ----------------------------------------------------- */

static void test_sc_d_success(cpu_t *cpu, memory_t *mem)
{
    printf("test_sc_d_success...\n");

    reset_cpu(cpu);

    mem_write64(mem, TEST_ADDR, 10);

    uint32_t lr =
        encode_amo(
            LR,
            0,
            0,
            0,
            10,
            AMO_D,
            5
        );

    uint32_t sc =
        encode_amo(
            SC,
            0,
            0,
            11,
            10,
            AMO_D,
            6
        );

    write_instruction(mem, CODE_ADDR, lr);
    write_instruction(mem, CODE_ADDR + 4, sc);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 42;

    cpu_step(cpu, mem);

    assert(cpu->regs[5] == 10);
    assert(cpu->reservation.valid == 1);

    cpu_step(cpu, mem);

    assert(cpu->regs[6] == 0);
    assert(mem_read64(mem, TEST_ADDR) == 42);
    assert(cpu->reservation.valid == 0);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* SC.D without prior LR                                 */
/* ----------------------------------------------------- */

static void test_sc_d_without_lr(cpu_t *cpu, memory_t *mem)
{
    printf("test_sc_d_without_lr...\n");

    reset_cpu(cpu);

    mem_write64(mem, TEST_ADDR, 10);

    uint32_t sc =
        encode_amo(
            SC,
            0,
            0,
            11,
            10,
            AMO_D,
            6
        );

    write_instruction(mem, CODE_ADDR, sc);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 99;

    cpu_step(cpu, mem);

    assert(cpu->regs[6] != 0);
    assert(mem_read64(mem, TEST_ADDR) == 10);
    assert(cpu->reservation.valid == 0);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* SC.D wrong address                                    */
/* ----------------------------------------------------- */

static void test_sc_wrong_address(cpu_t *cpu, memory_t *mem)
{
    printf("test_sc_wrong_address...\n");

    reset_cpu(cpu);

    mem_write64(mem, TEST_ADDR, 10);
    mem_write64(mem, TEST_ADDR_2, 20);

    uint32_t lr =
        encode_amo(
            LR,
            0,
            0,
            0,
            10,
            AMO_D,
            5
        );

    uint32_t sc =
        encode_amo(
            SC,
            0,
            0,
            11,
            10,
            AMO_D,
            6
        );

    write_instruction(mem, CODE_ADDR, lr);
    write_instruction(mem, CODE_ADDR + 4, sc);

    cpu->regs[10] = TEST_ADDR;

    cpu_step(cpu, mem);

    assert(cpu->reservation.phys_addr == TEST_ADDR);

    cpu->regs[10] = TEST_ADDR_2;
    cpu->regs[11] = 77;

    cpu_step(cpu, mem);

    assert(cpu->regs[6] != 0);
    assert(mem_read64(mem, TEST_ADDR) == 10);
    assert(mem_read64(mem, TEST_ADDR_2) == 20);
    assert(cpu->reservation.valid == 0);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* LR.W -> SC.D should fail                              */
/* ----------------------------------------------------- */

static void test_sc_wrong_size(cpu_t *cpu, memory_t *mem)
{
    printf("test_sc_wrong_size...\n");

    reset_cpu(cpu);

    mem_write64(mem, TEST_ADDR, 10);

    uint32_t lr_w =
        encode_amo(
            LR,
            0,
            0,
            0,
            10,
            AMO_W,
            5
        );

    uint32_t sc_d =
        encode_amo(
            SC,
            0,
            0,
            11,
            10,
            AMO_D,
            6
        );

    write_instruction(mem, CODE_ADDR, lr_w);
    write_instruction(mem, CODE_ADDR + 4, sc_d);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 99;

    cpu_step(cpu, mem);

    assert(cpu->reservation.size == 4);

    cpu_step(cpu, mem);

    assert(cpu->regs[6] != 0);
    assert(cpu->reservation.valid == 0);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* Normal store invalidates reservation                  */
/* ----------------------------------------------------- */

static void test_store_invalidates_reservation(
    cpu_t *cpu,
    memory_t *mem
)
{
    printf("test_store_invalidates_reservation...\n");

    reset_cpu(cpu);

    mem_write64(mem, TEST_ADDR, 10);

    uint32_t lr =
        encode_amo(
            LR,
            0,
            0,
            0,
            10,
            AMO_D,
            5
        );

    write_instruction(mem, CODE_ADDR, lr);

    cpu->regs[10] = TEST_ADDR;

    cpu_step(cpu, mem);

    assert(cpu->reservation.valid == 1);

    store_result_t store =
        cpu_store64(
            cpu,
            mem,
            TEST_ADDR,
            20
        );

    assert(store.success);
    assert(cpu->reservation.valid == 0);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* Non-overlapping store keeps reservation               */
/* ----------------------------------------------------- */

static void test_nonoverlap_store_keeps_reservation(
    cpu_t *cpu,
    memory_t *mem
)
{
    printf("test_nonoverlap_store_keeps_reservation...\n");

    reset_cpu(cpu);

    mem_write64(mem, TEST_ADDR, 10);

    uint32_t lr =
        encode_amo(
            LR,
            0,
            0,
            0,
            10,
            AMO_D,
            5
        );

    write_instruction(mem, CODE_ADDR, lr);

    cpu->regs[10] = TEST_ADDR;

    cpu_step(cpu, mem);

    assert(cpu->reservation.valid == 1);

    store_result_t store =
        cpu_store64(
            cpu,
            mem,
            TEST_ADDR_2,
            20
        );

    assert(store.success);
    assert(cpu->reservation.valid == 1);
    assert(cpu->reservation.phys_addr == TEST_ADDR);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* AMOSWAP.D                                             */
/* ----------------------------------------------------- */

static void test_amoswap_d(cpu_t *cpu, memory_t *mem)
{
    printf("test_amoswap_d...\n");

    reset_cpu(cpu);

    mem_write64(mem, TEST_ADDR, 10);

    uint32_t instruction =
        encode_amo(
            AMOSWAP,
            0,
            0,
            11,
            10,
            AMO_D,
            5
        );

    write_instruction(mem, CODE_ADDR, instruction);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 99;

    cpu_step(cpu, mem);

    assert(mem_read64(mem, TEST_ADDR) == 99);
    assert(cpu->regs[5] == 10);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* AMOADD.D                                              */
/* ----------------------------------------------------- */

static void test_amoadd_d(cpu_t *cpu, memory_t *mem)
{
    printf("test_amoadd_d...\n");

    reset_cpu(cpu);

    mem_write64(mem, TEST_ADDR, 10);

    uint32_t instruction =
        encode_amo(
            AMOADD,
            0,
            0,
            11,
            10,
            AMO_D,
            5
        );

    write_instruction(mem, CODE_ADDR, instruction);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 7;

    cpu_step(cpu, mem);

    assert(mem_read64(mem, TEST_ADDR) == 17);
    assert(cpu->regs[5] == 10);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* AMOADD.W wraparound + sign extension                  */
/* ----------------------------------------------------- */

static void test_amoadd_w_sign_extension(
    cpu_t *cpu,
    memory_t *mem
)
{
    printf("test_amoadd_w_sign_extension...\n");

    reset_cpu(cpu);

    mem_write32(
        mem,
        TEST_ADDR,
        0xFFFFFFFFU
    );

    uint32_t instruction =
        encode_amo(
            AMOADD,
            0,
            0,
            11,
            10,
            AMO_W,
            5
        );

    write_instruction(mem, CODE_ADDR, instruction);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 1;

    cpu_step(cpu, mem);

    assert(
        mem_read32(mem, TEST_ADDR) ==
        0x00000000U
    );

    assert(
        cpu->regs[5] ==
        0xFFFFFFFFFFFFFFFFULL
    );

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* AMOMIN.D signed                                       */
/* ----------------------------------------------------- */

static void test_amomin_d(cpu_t *cpu, memory_t *mem)
{
    printf("test_amomin_d...\n");

    reset_cpu(cpu);

    mem_write64(
        mem,
        TEST_ADDR,
        0xFFFFFFFFFFFFFFFFULL
    );

    uint32_t instruction =
        encode_amo(
            AMOMIN,
            0,
            0,
            11,
            10,
            AMO_D,
            5
        );

    write_instruction(mem, CODE_ADDR, instruction);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 5;

    cpu_step(cpu, mem);

    assert(
        mem_read64(mem, TEST_ADDR) ==
        0xFFFFFFFFFFFFFFFFULL
    );

    assert(
        cpu->regs[5] ==
        0xFFFFFFFFFFFFFFFFULL
    );

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* AMOMINU.D unsigned                                    */
/* ----------------------------------------------------- */

static void test_amominu_d(cpu_t *cpu, memory_t *mem)
{
    printf("test_amominu_d...\n");

    reset_cpu(cpu);

    mem_write64(
        mem,
        TEST_ADDR,
        0xFFFFFFFFFFFFFFFFULL
    );

    uint32_t instruction =
        encode_amo(
            AMOMINU,
            0,
            0,
            11,
            10,
            AMO_D,
            5
        );

    write_instruction(mem, CODE_ADDR, instruction);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 5;

    cpu_step(cpu, mem);

    assert(mem_read64(mem, TEST_ADDR) == 5);

    assert(
        cpu->regs[5] ==
        0xFFFFFFFFFFFFFFFFULL
    );

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* AMOMAX.D signed                                       */
/* ----------------------------------------------------- */

static void test_amomax_d(cpu_t *cpu, memory_t *mem)
{
    printf("test_amomax_d...\n");

    reset_cpu(cpu);

    mem_write64(
        mem,
        TEST_ADDR,
        0xFFFFFFFFFFFFFFFFULL
    );

    uint32_t instruction =
        encode_amo(
            AMOMAX,
            0,
            0,
            11,
            10,
            AMO_D,
            5
        );

    write_instruction(mem, CODE_ADDR, instruction);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 5;

    cpu_step(cpu, mem);

    assert(mem_read64(mem, TEST_ADDR) == 5);

    assert(
        cpu->regs[5] ==
        0xFFFFFFFFFFFFFFFFULL
    );

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* AMOMAXU.D unsigned                                    */
/* ----------------------------------------------------- */

static void test_amomaxu_d(cpu_t *cpu, memory_t *mem)
{
    printf("test_amomaxu_d...\n");

    reset_cpu(cpu);

    mem_write64(
        mem,
        TEST_ADDR,
        0xFFFFFFFFFFFFFFFFULL
    );

    uint32_t instruction =
        encode_amo(
            AMOMAXU,
            0,
            0,
            11,
            10,
            AMO_D,
            5
        );

    write_instruction(mem, CODE_ADDR, instruction);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 5;

    cpu_step(cpu, mem);

    assert(
        mem_read64(mem, TEST_ADDR) ==
        0xFFFFFFFFFFFFFFFFULL
    );

    assert(
        cpu->regs[5] ==
        0xFFFFFFFFFFFFFFFFULL
    );

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* AMO invalidates reservation                           */
/* ----------------------------------------------------- */

static void test_amo_invalidates_reservation(
    cpu_t *cpu,
    memory_t *mem
)
{
    printf("test_amo_invalidates_reservation...\n");

    reset_cpu(cpu);

    mem_write64(mem, TEST_ADDR, 10);

    uint32_t lr =
        encode_amo(
            LR,
            0,
            0,
            0,
            10,
            AMO_D,
            5
        );

    uint32_t amo =
        encode_amo(
            AMOADD,
            0,
            0,
            11,
            10,
            AMO_D,
            6
        );

    write_instruction(mem, CODE_ADDR, lr);
    write_instruction(mem, CODE_ADDR + 4, amo);

    cpu->regs[10] = TEST_ADDR;
    cpu->regs[11] = 1;

    cpu_step(cpu, mem);

    assert(cpu->reservation.valid == 1);

    cpu_step(cpu, mem);

    assert(cpu->reservation.valid == 0);
    assert(mem_read64(mem, TEST_ADDR) == 11);

    printf("  PASS\n");
}

/* ----------------------------------------------------- */
/* Main                                                  */
/* ----------------------------------------------------- */

int main(void)
{
    cpu_t cpu = cpu_init();
    memory_t mem = memory_init();

    printf("\n");
    printf("===========================\n");
    printf(" RISC-V A Extension Tests\n");
    printf("===========================\n\n");

    test_lr_d(&cpu, &mem);
    test_lr_w_sign_extension(&cpu, &mem);
    test_lr_replaces_reservation(&cpu, &mem);

    test_sc_d_success(&cpu, &mem);
    test_sc_d_without_lr(&cpu, &mem);
    test_sc_wrong_address(&cpu, &mem);
    test_sc_wrong_size(&cpu, &mem);

    test_store_invalidates_reservation(&cpu, &mem);
    test_nonoverlap_store_keeps_reservation(&cpu, &mem);

    test_amoswap_d(&cpu, &mem);
    test_amoadd_d(&cpu, &mem);
    test_amoadd_w_sign_extension(&cpu, &mem);

    test_amomin_d(&cpu, &mem);
    test_amominu_d(&cpu, &mem);
    test_amomax_d(&cpu, &mem);
    test_amomaxu_d(&cpu, &mem);

    test_amo_invalidates_reservation(&cpu, &mem);

    printf("\n");
    printf("===========================\n");
    printf(" ALL A EXTENSION TESTS PASS\n");
    printf("===========================\n\n");

    return 0;
}
