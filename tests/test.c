#include "vm.h"
#include "cpu.h"
#include "memory.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

static int tests_run = 0;
static int tests_failed = 0;

#define CHECK_U64(name, actual, expected) do { \
    ++tests_run; \
    uint64_t _a = (uint64_t)(actual); \
    uint64_t _e = (uint64_t)(expected); \
    if (_a != _e) { \
        ++tests_failed; \
        fprintf(stderr, "FAIL %-24s got 0x%016" PRIx64 " expected 0x%016" PRIx64 "\n", \
                (name), _a, _e); \
    } else { \
        printf("PASS %-24s\n", (name)); \
    } \
} while (0)

#define CHECK_U32(name, actual, expected) do { \
    ++tests_run; \
    uint32_t _a = (uint32_t)(actual); \
    uint32_t _e = (uint32_t)(expected); \
    if (_a != _e) { \
        ++tests_failed; \
        fprintf(stderr, "FAIL %-24s got 0x%08" PRIx32 " expected 0x%08" PRIx32 "\n", \
                (name), _a, _e); \
    } else { \
        printf("PASS %-24s\n", (name)); \
    } \
} while (0)

#define CHECK_U16(name, actual, expected) do { \
    ++tests_run; \
    uint16_t _a = (uint16_t)(actual); \
    uint16_t _e = (uint16_t)(expected); \
    if (_a != _e) { \
        ++tests_failed; \
        fprintf(stderr, "FAIL %-24s got 0x%04x expected 0x%04x\n", \
                (name), (unsigned)_a, (unsigned)_e); \
    } else { \
        printf("PASS %-24s\n", (name)); \
    } \
} while (0)

#define CHECK_U8(name, actual, expected) do { \
    ++tests_run; \
    uint8_t _a = (uint8_t)(actual); \
    uint8_t _e = (uint8_t)(expected); \
    if (_a != _e) { \
        ++tests_failed; \
        fprintf(stderr, "FAIL %-24s got 0x%02x expected 0x%02x\n", \
                (name), (unsigned)_a, (unsigned)_e); \
    } else { \
        printf("PASS %-24s\n", (name)); \
    } \
} while (0)

/* ---------- RV64I instruction encoders ---------- */

static uint32_t enc_r(uint8_t funct7, uint8_t rs2, uint8_t rs1,
                      uint8_t funct3, uint8_t rd, uint8_t opcode)
{
    return ((uint32_t)(funct7 & 0x7f) << 25) |
           ((uint32_t)(rs2 & 0x1f) << 20) |
           ((uint32_t)(rs1 & 0x1f) << 15) |
           ((uint32_t)(funct3 & 0x07) << 12) |
           ((uint32_t)(rd & 0x1f) << 7) |
           (uint32_t)(opcode & 0x7f);
}

static uint32_t enc_i(int32_t imm, uint8_t rs1, uint8_t funct3,
                      uint8_t rd, uint8_t opcode)
{
    return ((uint32_t)imm & 0xfffU) << 20 |
           ((uint32_t)(rs1 & 0x1f) << 15) |
           ((uint32_t)(funct3 & 0x07) << 12) |
           ((uint32_t)(rd & 0x1f) << 7) |
           (uint32_t)(opcode & 0x7f);
}

static uint32_t enc_s(int32_t imm, uint8_t rs2, uint8_t rs1,
                      uint8_t funct3, uint8_t opcode)
{
    uint32_t uimm = (uint32_t)imm & 0xfffU;
    return ((uimm >> 5) << 25) |
           ((uint32_t)(rs2 & 0x1f) << 20) |
           ((uint32_t)(rs1 & 0x1f) << 15) |
           ((uint32_t)(funct3 & 0x07) << 12) |
           ((uimm & 0x1fU) << 7) |
           (uint32_t)(opcode & 0x7f);
}

static uint32_t enc_b(int32_t imm, uint8_t rs2, uint8_t rs1,
                      uint8_t funct3, uint8_t opcode)
{
    uint32_t uimm = (uint32_t)imm & 0x1fffU;
    return (((uimm >> 12) & 0x1U) << 31) |
           (((uimm >> 5) & 0x3fU) << 25) |
           ((uint32_t)(rs2 & 0x1f) << 20) |
           ((uint32_t)(rs1 & 0x1f) << 15) |
           ((uint32_t)(funct3 & 0x07) << 12) |
           (((uimm >> 1) & 0x0fU) << 8) |
           (((uimm >> 11) & 0x1U) << 7) |
           (uint32_t)(opcode & 0x7f);
}

static uint32_t enc_u(uint32_t imm20, uint8_t rd, uint8_t opcode)
{
    return ((imm20 & 0xfffffU) << 12) |
           ((uint32_t)(rd & 0x1f) << 7) |
           (uint32_t)(opcode & 0x7f);
}

static uint32_t enc_j(int32_t imm, uint8_t rd, uint8_t opcode)
{
    uint32_t uimm = (uint32_t)imm & 0x1fffffU;
    return (((uimm >> 20) & 0x1U) << 31) |
           (((uimm >> 1) & 0x3ffU) << 21) |
           (((uimm >> 11) & 0x1U) << 20) |
           (((uimm >> 12) & 0xffU) << 12) |
           ((uint32_t)(rd & 0x1f) << 7) |
           (uint32_t)(opcode & 0x7f);
}

/* ---------- execution helpers ---------- */

static void clear_regs(cpu_t *cpu)
{
    for (uint8_t i = 1; i < 32; ++i)
        cpu_write_reg(cpu, i, 0);
}

static void prep(vm_t *vm)
{
    clear_regs(&vm->cpu);
    cpu_reset(&vm->cpu);
}

static void run_at_base(vm_t *vm, uint32_t instruction)
{
    mem_write32(&vm->ram, MEM_BASE, instruction);
    vm->cpu.pc = MEM_BASE;
    cpu_step(&vm->cpu, &vm->ram);
}

/* ---------- tests ---------- */

static void test_x0(vm_t *vm)
{
    prep(vm);
    cpu_write_reg(&vm->cpu, 0, 0xdeadbeefULL);
    CHECK_U64("x0 hardwired zero", cpu_read_reg(&vm->cpu, 0), 0);
}

static void test_op_imm(vm_t *vm)
{
    /* ADDI */
    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 100);
    run_at_base(vm, enc_i(-5, 5, 0x0, 6, 0x13));
    CHECK_U64("ADDI", cpu_read_reg(&vm->cpu, 6), 95);

    /* SLTI signed */
    prep(vm);
    cpu_write_reg(&vm->cpu, 5, (uint64_t)-10);
    run_at_base(vm, enc_i(-5, 5, 0x2, 6, 0x13));
    CHECK_U64("SLTI", cpu_read_reg(&vm->cpu, 6), 1);

    /* SLTIU: sign-extended immediate, unsigned comparison */
    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 5);
    run_at_base(vm, enc_i(-1, 5, 0x3, 6, 0x13));
    CHECK_U64("SLTIU", cpu_read_reg(&vm->cpu, 6), 1);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x55aaULL);
    run_at_base(vm, enc_i(0x0f0, 5, 0x4, 6, 0x13));
    CHECK_U64("XORI", cpu_read_reg(&vm->cpu, 6), 0x555aULL);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x5500ULL);
    run_at_base(vm, enc_i(0x0aa, 5, 0x6, 6, 0x13));
    CHECK_U64("ORI", cpu_read_reg(&vm->cpu, 6), 0x55aaULL);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x55aaULL);
    run_at_base(vm, enc_i(0x0f0, 5, 0x7, 6, 0x13));
    CHECK_U64("ANDI", cpu_read_reg(&vm->cpu, 6), 0x00a0ULL);

    /* SLLI shamt=40 tests the 6-bit RV64 shift amount. */
    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 1);
    run_at_base(vm, enc_i(40, 5, 0x1, 6, 0x13));
    CHECK_U64("SLLI", cpu_read_reg(&vm->cpu, 6), 1ULL << 40);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x8000000000000000ULL);
    run_at_base(vm, enc_i(4, 5, 0x5, 6, 0x13));
    CHECK_U64("SRLI", cpu_read_reg(&vm->cpu, 6), 0x0800000000000000ULL);

    /* SRAI: funct6=010000 plus shamt=4. */
    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x8000000000000000ULL);
    run_at_base(vm, enc_i((0x10 << 6) | 4, 5, 0x5, 6, 0x13));
    CHECK_U64("SRAI", cpu_read_reg(&vm->cpu, 6), 0xf800000000000000ULL);
}

static void test_op_imm_32(vm_t *vm)
{
    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x000000007fffffffULL);
    run_at_base(vm, enc_i(1, 5, 0x0, 6, 0x1b));
    CHECK_U64("ADDIW", cpu_read_reg(&vm->cpu, 6), 0xffffffff80000000ULL);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 1);
    run_at_base(vm, enc_i(31, 5, 0x1, 6, 0x1b));
    CHECK_U64("SLLIW", cpu_read_reg(&vm->cpu, 6), 0xffffffff80000000ULL);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x0000000080000000ULL);
    run_at_base(vm, enc_i(4, 5, 0x5, 6, 0x1b));
    CHECK_U64("SRLIW", cpu_read_reg(&vm->cpu, 6), 0x0000000008000000ULL);

    /* SRAIW: funct7=0100000 in imm[11:5], shamt=4. */
    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x0000000080000000ULL);
    run_at_base(vm, enc_i((0x20 << 5) | 4, 5, 0x5, 6, 0x1b));
    CHECK_U64("SRAIW", cpu_read_reg(&vm->cpu, 6), 0xfffffffff8000000ULL);
}

static void test_op(vm_t *vm)
{
    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 20);
    cpu_write_reg(&vm->cpu, 6, 7);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x0, 7, 0x33));
    CHECK_U64("ADD", cpu_read_reg(&vm->cpu, 7), 27);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 20);
    cpu_write_reg(&vm->cpu, 6, 7);
    run_at_base(vm, enc_r(0x20, 6, 5, 0x0, 7, 0x33));
    CHECK_U64("SUB", cpu_read_reg(&vm->cpu, 7), 13);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 1);
    cpu_write_reg(&vm->cpu, 6, 40);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x1, 7, 0x33));
    CHECK_U64("SLL", cpu_read_reg(&vm->cpu, 7), 1ULL << 40);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, (uint64_t)-2);
    cpu_write_reg(&vm->cpu, 6, 1);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x2, 7, 0x33));
    CHECK_U64("SLT", cpu_read_reg(&vm->cpu, 7), 1);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 1);
    cpu_write_reg(&vm->cpu, 6, 2);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x3, 7, 0x33));
    CHECK_U64("SLTU", cpu_read_reg(&vm->cpu, 7), 1);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x55aa);
    cpu_write_reg(&vm->cpu, 6, 0x0ff0);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x4, 7, 0x33));
    CHECK_U64("XOR", cpu_read_reg(&vm->cpu, 7), 0x5a5a);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x8000000000000000ULL);
    cpu_write_reg(&vm->cpu, 6, 4);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x5, 7, 0x33));
    CHECK_U64("SRL", cpu_read_reg(&vm->cpu, 7), 0x0800000000000000ULL);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x8000000000000000ULL);
    cpu_write_reg(&vm->cpu, 6, 4);
    run_at_base(vm, enc_r(0x20, 6, 5, 0x5, 7, 0x33));
    CHECK_U64("SRA", cpu_read_reg(&vm->cpu, 7), 0xf800000000000000ULL);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x5500);
    cpu_write_reg(&vm->cpu, 6, 0x00aa);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x6, 7, 0x33));
    CHECK_U64("OR", cpu_read_reg(&vm->cpu, 7), 0x55aa);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x55aa);
    cpu_write_reg(&vm->cpu, 6, 0x0ff0);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x7, 7, 0x33));
    CHECK_U64("AND", cpu_read_reg(&vm->cpu, 7), 0x05a0);
}

static void test_op_32(vm_t *vm)
{
    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x7fffffffULL);
    cpu_write_reg(&vm->cpu, 6, 1);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x0, 7, 0x3b));
    CHECK_U64("ADDW", cpu_read_reg(&vm->cpu, 7), 0xffffffff80000000ULL);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0);
    cpu_write_reg(&vm->cpu, 6, 1);
    run_at_base(vm, enc_r(0x20, 6, 5, 0x0, 7, 0x3b));
    CHECK_U64("SUBW", cpu_read_reg(&vm->cpu, 7), 0xffffffffffffffffULL);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 1);
    cpu_write_reg(&vm->cpu, 6, 31);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x1, 7, 0x3b));
    CHECK_U64("SLLW", cpu_read_reg(&vm->cpu, 7), 0xffffffff80000000ULL);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x80000000ULL);
    cpu_write_reg(&vm->cpu, 6, 4);
    run_at_base(vm, enc_r(0x00, 6, 5, 0x5, 7, 0x3b));
    CHECK_U64("SRLW", cpu_read_reg(&vm->cpu, 7), 0x0000000008000000ULL);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 0x80000000ULL);
    cpu_write_reg(&vm->cpu, 6, 4);
    run_at_base(vm, enc_r(0x20, 6, 5, 0x5, 7, 0x3b));
    CHECK_U64("SRAW", cpu_read_reg(&vm->cpu, 7), 0xfffffffff8000000ULL);
}

static void test_u_type(vm_t *vm)
{
    prep(vm);
    run_at_base(vm, enc_u(0x12345, 5, 0x37));
    CHECK_U64("LUI positive", cpu_read_reg(&vm->cpu, 5), 0x0000000012345000ULL);

    prep(vm);
    run_at_base(vm, enc_u(0x80000, 5, 0x37));
    CHECK_U64("LUI sign extend", cpu_read_reg(&vm->cpu, 5), 0xffffffff80000000ULL);

    prep(vm);
    run_at_base(vm, enc_u(0x00001, 5, 0x17));
    CHECK_U64("AUIPC", cpu_read_reg(&vm->cpu, 5), MEM_BASE + 0x1000ULL);
}

static void test_jumps(vm_t *vm)
{
    prep(vm);
    run_at_base(vm, enc_j(16, 5, 0x6f));
    CHECK_U64("JAL rd", cpu_read_reg(&vm->cpu, 5), MEM_BASE + 4);
    CHECK_U64("JAL pc", vm->cpu.pc, MEM_BASE + 16);

    prep(vm);
    cpu_write_reg(&vm->cpu, 6, MEM_BASE + 0x100);
    run_at_base(vm, enc_i(3, 6, 0x0, 5, 0x67));
    CHECK_U64("JALR rd", cpu_read_reg(&vm->cpu, 5), MEM_BASE + 4);
    CHECK_U64("JALR pc lsb clear", vm->cpu.pc, MEM_BASE + 0x102);
}

static void test_branches(vm_t *vm)
{
    const int32_t off = 16;

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 42);
    cpu_write_reg(&vm->cpu, 6, 42);
    run_at_base(vm, enc_b(off, 6, 5, 0x0, 0x63));
    CHECK_U64("BEQ taken", vm->cpu.pc, MEM_BASE + off);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 1);
    cpu_write_reg(&vm->cpu, 6, 2);
    run_at_base(vm, enc_b(off, 6, 5, 0x1, 0x63));
    CHECK_U64("BNE taken", vm->cpu.pc, MEM_BASE + off);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, (uint64_t)-2);
    cpu_write_reg(&vm->cpu, 6, 1);
    run_at_base(vm, enc_b(off, 6, 5, 0x4, 0x63));
    CHECK_U64("BLT signed", vm->cpu.pc, MEM_BASE + off);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 5);
    cpu_write_reg(&vm->cpu, 6, (uint64_t)-1);
    run_at_base(vm, enc_b(off, 6, 5, 0x5, 0x63));
    CHECK_U64("BGE signed", vm->cpu.pc, MEM_BASE + off);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 1);
    cpu_write_reg(&vm->cpu, 6, UINT64_MAX);
    run_at_base(vm, enc_b(off, 6, 5, 0x6, 0x63));
    CHECK_U64("BLTU unsigned", vm->cpu.pc, MEM_BASE + off);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, UINT64_MAX);
    cpu_write_reg(&vm->cpu, 6, 1);
    run_at_base(vm, enc_b(off, 6, 5, 0x7, 0x63));
    CHECK_U64("BGEU unsigned", vm->cpu.pc, MEM_BASE + off);

    /* One not-taken test catches unconditional-branch bugs. */
    prep(vm);
    cpu_write_reg(&vm->cpu, 5, 1);
    cpu_write_reg(&vm->cpu, 6, 2);
    run_at_base(vm, enc_b(off, 6, 5, 0x0, 0x63));
    CHECK_U64("BEQ not taken", vm->cpu.pc, MEM_BASE + 4);
}

static void test_loads(vm_t *vm)
{
    const uint64_t data = MEM_BASE + 0x1000;

    /* Byte loads */
    prep(vm);
    mem_write8(&vm->ram, data, 0x80);
    cpu_write_reg(&vm->cpu, 5, data);
    run_at_base(vm, enc_i(0, 5, 0x0, 6, 0x03));
    CHECK_U64("LB sign extend", cpu_read_reg(&vm->cpu, 6), 0xffffffffffffff80ULL);

    prep(vm);
    mem_write8(&vm->ram, data, 0x80);
    cpu_write_reg(&vm->cpu, 5, data);
    run_at_base(vm, enc_i(0, 5, 0x4, 6, 0x03));
    CHECK_U64("LBU zero extend", cpu_read_reg(&vm->cpu, 6), 0x80);

    prep(vm);
    mem_write16(&vm->ram, data, 0x8001);
    cpu_write_reg(&vm->cpu, 5, data);
    run_at_base(vm, enc_i(0, 5, 0x1, 6, 0x03));
    CHECK_U64("LH sign extend", cpu_read_reg(&vm->cpu, 6), 0xffffffffffff8001ULL);

    prep(vm);
    mem_write16(&vm->ram, data, 0x8001);
    cpu_write_reg(&vm->cpu, 5, data);
    run_at_base(vm, enc_i(0, 5, 0x5, 6, 0x03));
    CHECK_U64("LHU zero extend", cpu_read_reg(&vm->cpu, 6), 0x8001);

    prep(vm);
    mem_write32(&vm->ram, data, 0x80000001U);
    cpu_write_reg(&vm->cpu, 5, data);
    run_at_base(vm, enc_i(0, 5, 0x2, 6, 0x03));
    CHECK_U64("LW sign extend", cpu_read_reg(&vm->cpu, 6), 0xffffffff80000001ULL);

    prep(vm);
    mem_write32(&vm->ram, data, 0x80000001U);
    cpu_write_reg(&vm->cpu, 5, data);
    run_at_base(vm, enc_i(0, 5, 0x6, 6, 0x03));
    CHECK_U64("LWU zero extend", cpu_read_reg(&vm->cpu, 6), 0x0000000080000001ULL);

    prep(vm);
    mem_write64(&vm->ram, data, 0x0123456789abcdefULL);
    cpu_write_reg(&vm->cpu, 5, data);
    run_at_base(vm, enc_i(0, 5, 0x3, 6, 0x03));
    CHECK_U64("LD", cpu_read_reg(&vm->cpu, 6), 0x0123456789abcdefULL);

    /* Negative offset */
    prep(vm);
    mem_write8(&vm->ram, data - 4, 0x7f);
    cpu_write_reg(&vm->cpu, 5, data);
    run_at_base(vm, enc_i(-4, 5, 0x0, 6, 0x03));
    CHECK_U64("LB negative offset", cpu_read_reg(&vm->cpu, 6), 0x7f);
}

static void test_stores(vm_t *vm)
{
    const uint64_t data = MEM_BASE + 0x1800;

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, data);
    cpu_write_reg(&vm->cpu, 6, 0x1122334455667788ULL);
    run_at_base(vm, enc_s(0, 6, 5, 0x0, 0x23));
    CHECK_U8("SB", mem_read8(&vm->ram, data), 0x88);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, data);
    cpu_write_reg(&vm->cpu, 6, 0x1122334455667788ULL);
    run_at_base(vm, enc_s(2, 6, 5, 0x1, 0x23));
    CHECK_U64("SH", mem_read16(&vm->ram, data + 2), 0x7788);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, data);
    cpu_write_reg(&vm->cpu, 6, 0x1122334455667788ULL);
    run_at_base(vm, enc_s(4, 6, 5, 0x2, 0x23));
    CHECK_U32("SW", mem_read32(&vm->ram, data + 4), 0x55667788U);

    prep(vm);
    cpu_write_reg(&vm->cpu, 5, data + 16);
    cpu_write_reg(&vm->cpu, 6, 0x1122334455667788ULL);
    run_at_base(vm, enc_s(-8, 6, 5, 0x3, 0x23));
    CHECK_U64("SD negative offset", mem_read64(&vm->ram, data + 8), 0x1122334455667788ULL);
}

static void test_little_endian_memory(vm_t *vm)
{
    const uint64_t data = MEM_BASE + 0x2000;

    /* 16-bit write/read and byte order */
    mem_write16(&vm->ram, data, 0x1234U);
    CHECK_U8("LE16 byte[0]", mem_read8(&vm->ram, data + 0), 0x34);
    CHECK_U8("LE16 byte[1]", mem_read8(&vm->ram, data + 1), 0x12);
    CHECK_U16("LE16 round trip", mem_read16(&vm->ram, data), 0x1234U);

    /* 32-bit write/read and byte order */
    mem_write32(&vm->ram, data + 8, 0x12345678U);
    CHECK_U8("LE32 byte[0]", mem_read8(&vm->ram, data + 8), 0x78);
    CHECK_U8("LE32 byte[1]", mem_read8(&vm->ram, data + 9), 0x56);
    CHECK_U8("LE32 byte[2]", mem_read8(&vm->ram, data + 10), 0x34);
    CHECK_U8("LE32 byte[3]", mem_read8(&vm->ram, data + 11), 0x12);
    CHECK_U32("LE32 round trip", mem_read32(&vm->ram, data + 8), 0x12345678U);

    /* 64-bit write/read and byte order */
    mem_write64(&vm->ram, data + 16, 0x0123456789abcdefULL);
    CHECK_U8("LE64 byte[0]", mem_read8(&vm->ram, data + 16), 0xef);
    CHECK_U8("LE64 byte[1]", mem_read8(&vm->ram, data + 17), 0xcd);
    CHECK_U8("LE64 byte[2]", mem_read8(&vm->ram, data + 18), 0xab);
    CHECK_U8("LE64 byte[3]", mem_read8(&vm->ram, data + 19), 0x89);
    CHECK_U8("LE64 byte[4]", mem_read8(&vm->ram, data + 20), 0x67);
    CHECK_U8("LE64 byte[5]", mem_read8(&vm->ram, data + 21), 0x45);
    CHECK_U8("LE64 byte[6]", mem_read8(&vm->ram, data + 22), 0x23);
    CHECK_U8("LE64 byte[7]", mem_read8(&vm->ram, data + 23), 0x01);
    CHECK_U64("LE64 round trip", mem_read64(&vm->ram, data + 16),
              0x0123456789abcdefULL);
}

int main(void)
{
    vm_t vm = vm_init();

    test_little_endian_memory(&vm);
    test_x0(&vm);
    test_op_imm(&vm);
    test_op_imm_32(&vm);
    test_op(&vm);
    test_op_32(&vm);
    test_u_type(&vm);
    test_jumps(&vm);
    test_branches(&vm);
    test_loads(&vm);
    test_stores(&vm);

    printf("\n%d tests run: %d passed, %d failed\n",
           tests_run, tests_run - tests_failed, tests_failed);

    return tests_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

