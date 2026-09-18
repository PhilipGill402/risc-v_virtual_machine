    .section .text
    .globl _start

_start:
    # assume CPU starts here in U-mode
    ecall

    # should execute after returning from SRET
    addi x5, x0, 42

loop:
    jal x0, loop

trap_handler:
    csrr x6, scause
    csrr x7, sepc

    addi x7, x7, 4
    csrw sepc, x7

    sret
