    .section .text
    .globl _start

_start:
    # x5 = 10
    addi x5, x0, 10

    # x6 = 20
    addi x6, x0, 20

    # x7 = 30
    add x7, x5, x6

    # x8 = MEM_BASE + 0x100
    lui x8, 0x80000
    addi x8, x8, 0x100

    # store x7 at memory address
    sd x7, 0(x8)

    # load it back into x9
    ld x9, 0(x8)

    # should be equal, so branch
    beq x7, x9, equal

    # should never execute
    addi x10, x0, 99

equal:
    # signed compare: 10 < 20
    slt x10, x5, x6

    # x10 should now be 1

    # jump over the next instruction
    jal x11, done

    # should never execute
    addi x12, x0, 77

done:
    ebreak
