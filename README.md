# RISC-V Virtual Machine

A 64-bit RISC-V virtual machine and emulator written in C.

This project started as a low-level CPU emulator and has grown into a small virtual machine capable of booting OpenSBI, the Linux kernel, and a BusyBox userspace shell. The goal is to understand computer architecture, operating systems, virtual memory, privilege modes, traps, interrupts, and device emulation by implementing the full stack from scratch.

## Current Capabilities

The VM currently supports:

- RV64I base integer instruction set
- M extension for integer multiplication and division
- A extension for atomic instructions
- Zicsr CSR instructions
- Zifencei
- Machine, Supervisor, and User privilege modes
- RISC-V traps and exceptions
- CSR handling
- Sv39 virtual memory
- Page-table walking
- Accessed and Dirty PTE handling
- Machine and Supervisor timer interrupts
- OpenSBI boot
- Linux boot
- Initramfs support
- Buildroot / BusyBox userspace
- 16550-style UART emulation
- UART transmit support
- UART receive FIFO
- Interactive terminal input
- Device Tree support
- Physical bus abstraction
- Static VM library
- Debugger frontend

The emulator currently boots to an interactive Linux shell.

## Architecture

The emulator is structured around a central VM containing the CPU, physical memory, and emulated devices.

The bus handles physical address routing to RAM and MMIO devices.

Virtual accesses first pass through the CPU's address translation logic:

```text
Virtual Address
      |
      v
Sv39 Translation
      |
      v
Physical Address
      |
      v
Bus
      |
      +--> RAM
      +--> UART
      +--> Timer
```

Page-table walks themselves perform physical bus reads and writes.

## Supported ISA

The current guest ISA is:

```text
rv64ima_zicsr_zifencei
```

Floating-point, compressed, and vector extensions are not currently required by the guest environment.

## Privilege Modes

The emulator supports all three major RISC-V privilege levels:

```text
M-mode   Machine mode
S-mode   Supervisor mode
U-mode   User mode
```

This includes privilege transitions during:

- OpenSBI startup
- Linux boot
- Exceptions
- Interrupts
- System calls
- `MRET`
- `SRET`

## Virtual Memory

Linux runs using Sv39 paging.

The emulator implements:

- Three-level Sv39 page-table walks
- Virtual-to-physical translation
- Permission checks
- Page faults
- Accessed bit handling
- Dirty bit handling
- Physical PTE updates
- Instruction, load, and store access types

## Traps and Interrupts

The CPU supports synchronous exceptions and asynchronous interrupts.

Implemented functionality includes:

- Environment calls
- Illegal instruction traps
- Page faults
- Machine timer interrupts
- Supervisor timer interrupts
- Trap delegation
- `mtvec`
- `stvec`
- `mepc`
- `sepc`
- `mcause`
- `scause`
- `medeleg`
- `mideleg`

## UART

The VM includes a minimal 16550-compatible UART.

The UART currently supports:

- Transmit Holding Register
- Receive Buffer Register
- Interrupt Enable Register
- Interrupt Identification Register
- FIFO Control Register
- Line Control Register
- Line Status Register
- Modem Control Register
- Scratch Register
- DLAB handling
- THRE interrupt behavior
- Receive FIFO
- Host terminal input
- Linux console output

The host terminal acts as the guest serial console.

## Linux Boot

The VM boots through the following chain:

```text
RISC-V VM
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

A successful boot eventually reaches an interactive prompt:

```text
===============================
 My RISC-V Linux system booted!
===============================

/ #
```

## Guest Memory Map

The current VM configuration uses a memory map similar to:

| Address | Device |
|---|---|
| `0x02000000` | Timer / CLINT |
| `0x10000000` | UART |
| `0x80000000` | Guest RAM / OpenSBI |
| `0x80200000` | Linux kernel |
| `0x82200000` | Device Tree Blob |

The exact layout may change as the project develops.

## Project Structure

The project is organized so that the VM core can be built as a reusable static library.

The VM core is compiled into:

```text
build/libvm.a
```

The normal VM and debugger both link against the same VM implementation.

## Building

### Build the VM

```sh
make vm
```

This builds the VM executable and Device Tree Blob.

Typical outputs:

```text
build/vm
build/vm.dtb
build/libvm.a
```

### Build the VM Library

```sh
make libvm
```

This builds:

```text
build/libvm.a
```

### Build the Debugger

```sh
make debugger
```

This builds:

```text
build/debugger
```

### Build Everything

```sh
make
```

### Clean

```sh
make clean
```

## Debugger

The debugger is implemented as a separate frontend that links against the same VM library.

This allows the debugger to inspect and control the same machine implementation used by the normal emulator.

Debugger functionality can include:

- Single-step execution
- Register inspection
- CSR inspection
- Virtual memory inspection
- Physical memory inspection
- Instruction fetching
- Breakpoints
- Trap inspection
- Address translation debugging

## Design Goals

The main goal of this project is educational.

Instead of using an existing emulation framework, the VM implements the major components directly in order to better understand how they interact.

Areas explored by the project include:

- Instruction decoding
- CPU execution
- RISC-V privilege architecture
- Virtual memory
- Page tables
- Traps
- Interrupts
- Memory-mapped I/O
- Device emulation
- Firmware
- Linux boot
- System calls
- Userspace
- Terminal I/O
- Debugging infrastructure

## Development Status

Working:

- [x] RV64I
- [x] M extension
- [x] A extension
- [x] Zicsr
- [x] Zifencei
- [x] M-mode
- [x] S-mode
- [x] U-mode
- [x] CSRs
- [x] Traps
- [x] Interrupt delegation
- [x] Sv39
- [x] OpenSBI
- [x] Linux
- [x] Timer interrupts
- [x] UART output
- [x] UART input
- [x] BusyBox userspace
- [x] Interactive shell
- [x] VM static library
- [x] Debugger frontend

Possible future work:

- [ ] More complete 16550 behavior
- [ ] Multi-hart support
- [ ] Additional MMIO devices
- [ ] Improved debugger functionality
- [ ] Better performance
- [ ] More ISA extensions
- [ ] More complete atomic-memory semantics
- [ ] Additional guest operating systems

## References

Useful references for the project include:

- RISC-V Unprivileged ISA Specification
- RISC-V Privileged Architecture Specification
- OpenSBI
- Buildroot
- BusyBox

## Author

Philip Gill

Built as a systems programming and computer architecture project focused on understanding the full path from instruction execution to a bootable Linux userspace.
