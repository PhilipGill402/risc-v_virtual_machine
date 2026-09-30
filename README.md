# RISC-V Virtual Machine

A 64-bit RISC-V virtual machine and emulator written in C, built from scratch to explore CPU emulation, privilege modes, virtual memory, devices, interrupts, and Linux boot.

The emulator currently boots through OpenSBI into a RISC-V Linux kernel and reaches an interactive BusyBox userspace shell.

## Features

- RV64 instruction emulation
- RV64I base integer instruction set
- M extension for integer multiplication and division
- A extension for atomic instructions
- Zicsr CSR instructions
- Zifencei instruction support
- Machine, Supervisor, and User privilege modes
- RISC-V trap and exception handling
- CSR implementation
- Sv39 virtual memory and page-table translation
- OpenSBI boot support
- Linux kernel boot support
- CLINT timer emulation
- Supervisor and machine timer interrupts
- 16550-style UART emulation
- Device Tree based hardware description
- Initramfs / BusyBox userspace
- Interactive Linux shell

## Current Guest Architecture

The virtual machine currently models a single-hart RV64 system using:

```text
ISA:        rv64ima_zicsr_zifencei
MMU:        Sv39
RAM:        64 MiB
RAM base:   0x80000000
UART:       0x10000000
CLINT:      0x02000000
Timebase:   10 MHz
```

The current boot chain is:

```text
Host
  |
  v
RISC-V Virtual Machine
  |
  v
OpenSBI
  |
  v
Linux Kernel
  |
  v
Initramfs
  |
  v
BusyBox
  |
  v
Interactive Shell
```

## Project Goals

This project is primarily an educational systems project intended to better understand how a CPU, firmware, operating system, and hardware devices interact.

Rather than relying on an existing CPU emulation library, the major components of the virtual machine are implemented directly, including instruction execution, privilege transitions, page-table walking, interrupts, timers, and memory-mapped devices.

The project has progressed from executing individual RISC-V instructions to booting a complete Linux userspace.

## Implemented CPU Functionality

### Base ISA

The emulator supports the RV64I base instruction set, including:

- Integer arithmetic
- Logical operations
- Loads and stores
- Branches
- Jumps
- Immediate operations
- 32-bit RV64 word operations
- Environment calls
- Privileged returns

### Extensions

Currently supported extensions include:

```text
M        Integer multiplication and division
A        Atomic memory operations
Zicsr    Control and Status Register instructions
Zifencei Instruction-fetch fence
```

Compressed, floating-point, and vector instructions are intentionally not required by the current guest environment.

## Privilege Modes

The emulator supports all three privilege modes used by the current system:

```text
M-mode   Machine mode
S-mode   Supervisor mode
U-mode   User mode
```

This includes privilege transitions during OpenSBI startup, Linux kernel entry, exceptions, interrupts, system calls, `MRET`, and `SRET`.

## Virtual Memory

Linux runs using Sv39 virtual memory.

The emulator performs page-table translation for guest virtual addresses and supports the page-table behavior required by Linux, including permission checks and page faults.

```text
Virtual Address
      |
      v
Sv39 Page Table Walk
      |
      v
Guest Physical Address
      |
      v
Emulated RAM / MMIO
```

## Interrupts and Timers

The virtual machine includes CLINT-style timer support.

The timer path used by Linux is approximately:

```text
Linux
  |
  | SBI set_timer
  v
OpenSBI
  |
  v
mtimecmp
  |
  v
Machine Timer Interrupt
  |
  v
OpenSBI
  |
  v
Supervisor Timer Interrupt
  |
  v
Linux
```

## UART

A minimal 16550-compatible UART is implemented at:

```text
0x10000000
```

The UART currently supports Linux console output and interactive terminal input.

Implemented behavior includes:

- THR transmit writes
- Receive FIFO
- Interrupt Enable Register
- Interrupt Identification Register
- FIFO Control Register
- Line Control Register
- Line Status Register
- Modem Control Register
- Scratch Register
- DLAB behavior
- TX-empty state
- RX data-ready state

The host terminal is used as the guest serial console.

## Linux

The emulator is capable of booting a RISC-V Linux kernel through OpenSBI.

From the shell, standard BusyBox commands and Linux interfaces such as the following can be used:

## Userspace

The current userspace is generated with Buildroot and uses musl and BusyBox.

It is built specifically for:

```text
rv64ima_zicsr_zifencei
```

without requiring compressed or floating-point RISC-V extensions.

## Memory Map

Current major regions include:

| Address | Device |
|---|---|
| `0x02000000` | CLINT |
| `0x10000000` | UART |
| `0x80000000` | Guest RAM / OpenSBI |
| `0x80200000` | Linux kernel |
| `0x82200000` | Device Tree Blob |

The exact layout may change as the emulator develops.

## Building

The emulator itself is written in C and is intended to be built on a Unix-like host.

Linux, OpenSBI, Buildroot, and the device tree are built separately and loaded as guest images.

The VM loads them into guest physical memory before beginning execution in machine mode.

## Boot Configuration

The VM currently starts execution approximately as follows:

```text
PC          = 0x80000000
a0          = hart ID
a1          = DTB address
privilege   = Machine mode
```

OpenSBI then performs the transition into the Linux kernel.

## Development Status

This project is under active development.

- [x] RV64 instruction execution
- [x] Machine mode
- [x] Supervisor mode
- [x] User mode
- [x] CSRs
- [x] Exceptions and traps
- [x] Sv39 paging
- [x] OpenSBI
- [x] Linux boot
- [x] Timer interrupts
- [x] UART output
- [x] UART input
- [x] Initramfs
- [x] BusyBox shell
- [ ] More complete 16550 behavior
- [ ] Additional hardware devices
- [ ] Improved performance
- [ ] Broader ISA extension support
- [ ] Multi-hart support

## Why This Project?

A RISC-V emulator sits at the intersection of several areas of systems programming:

- CPU architecture
- Assembly
- Operating systems
- Virtual memory
- Interrupts
- Firmware
- Device drivers
- Binary formats
- Hardware/software interfaces

The goal of this project is to understand those systems by implementing them rather than treating them as black boxes.

## References

Useful specifications and projects for understanding the system include:

- RISC-V Instruction Set Manual
- RISC-V Privileged Architecture Specification
- OpenSBI
- Linux RISC-V port
- Buildroot
- BusyBox

## Author

**Philip Gill**

Built as a systems programming and computer architecture project focused on understanding the full path from CPU instruction execution to a bootable Linux userspace.
