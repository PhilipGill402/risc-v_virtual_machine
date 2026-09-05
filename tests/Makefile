CC = clang

CFLAGS = -Wall -Wextra -std=c11 -I../emulator/include

OBJ_DIR = ../build/test

TARGET = ../test_vm

TEST_SRC = test.c
TEST_OBJ = $(OBJ_DIR)/test.o

EMULATOR_DIR = ../emulator
EMULATOR_LIB = ../build/libriscv.a

RISCV_AS      = riscv64-elf-as
RISCV_LD      = riscv64-elf-ld
RISCV_OBJCOPY = riscv64-elf-objcopy
RISCV_OBJDUMP = riscv64-elf-objdump

RISCV_SRC = program.asm
RISCV_OBJ = $(OBJ_DIR)/program.o
RISCV_ELF = $(OBJ_DIR)/program.elf
RISCV_BIN = program.bin

all: $(TARGET)

$(EMULATOR_LIB):
	$(MAKE) -C $(EMULATOR_DIR)

$(TEST_OBJ): $(TEST_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(TEST_OBJ) $(EMULATOR_LIB)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJ) $(EMULATOR_LIB)

test: $(TARGET)
	$(TARGET)

asm: $(RISCV_SRC)
	@mkdir -p $(OBJ_DIR)
	$(RISCV_AS) -march=rv64i -mabi=lp64 $(RISCV_SRC) -o $(RISCV_OBJ)
	$(RISCV_LD) -Ttext=0x80000000 -e _start $(RISCV_OBJ) -o $(RISCV_ELF)
	$(RISCV_OBJCOPY) -O binary $(RISCV_ELF) $(RISCV_BIN)

disasm: asm
	$(RISCV_OBJDUMP) -d $(RISCV_ELF)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(RISCV_BIN)

.PHONY: all test asm disasm clean
