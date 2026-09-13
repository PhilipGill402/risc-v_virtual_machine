.section .text
    .globl _start

_start:
    la t0, trap_handler
    csrw mtvec, t0

    li t1, 1

    ecall

    li t2, 2

done:
    j done

trap_handler:
    csrr t3, mcause
    csrr t4, mepc
    addi t4, t4, 4
    csrw mepc, t4

    li t5, 3

    mret

