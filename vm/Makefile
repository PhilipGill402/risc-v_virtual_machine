CC = clang

CFLAGS = -Wall -Wextra -std=c11 \
	-Iinclude \
	-I../emulator/include

SRC_DIR = src
OBJ_DIR = ../build

TARGET = ../riscv_vm

EMULATOR_DIR = ../emulator
EMULATOR_LIB = $(OBJ_DIR)/libriscv.a

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

all: $(TARGET)

$(EMULATOR_LIB):
	$(MAKE) -C $(EMULATOR_DIR)

$(TARGET): $(OBJ_FILES) $(EMULATOR_LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJ_FILES) $(EMULATOR_LIB)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all run clean
