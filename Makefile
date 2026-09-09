EMULATOR_DIR = emulator
VM_DIR = vm
DEBUG_DIR = debug
TEST_DIR = tests
UTILS_DIR = utils

VM = riscv_vm

all: vm

utils:
	$(MAKE) -C $(UTILS_DIR)

emulator: utils
	$(MAKE) -C $(EMULATOR_DIR)

vm: emulator
	$(MAKE) -C $(VM_DIR)

debug: emulator
	$(MAKE) -C $(DEBUG_DIR)

test: emulator
	$(MAKE) -C $(TEST_DIR) test

asm:
	$(MAKE) -C $(TEST_DIR) asm

disasm:
	$(MAKE) -C $(TEST_DIR) disasm

run: vm
	$(MAKE) -C $(VM_DIR)
	./$(VM)

clean:
	$(MAKE) -C $(EMULATOR_DIR) clean
	$(MAKE) -C $(VM_DIR) clean
	$(MAKE) -C $(DEBUG_DIR) clean
	$(MAKE) -C $(TEST_DIR) clean

.PHONY: all emulator vm debug test asm disasm run clean
