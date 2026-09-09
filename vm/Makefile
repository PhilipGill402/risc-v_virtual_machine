CC = clang

CFLAGS = -Wall -Wextra -std=c11 \
	-Iinclude \
	-I../emulator/include \
	-I../utils/include

SRC_DIR = src
BUILD_DIR = ../build
OBJ_DIR = $(BUILD_DIR)/vm

TARGET = ../riscv_vm

EMULATOR_DIR = ../emulator
EMULATOR_LIB = $(BUILD_DIR)/libriscv.a

UTILS_DIR = ../utils
UTILS_LIB = $(BUILD_DIR)/libutils.a

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

all: $(TARGET)

$(UTILS_LIB):
	$(MAKE) -C $(UTILS_DIR)

$(EMULATOR_LIB): $(UTILS_LIB)
	$(MAKE) -C $(EMULATOR_DIR)

$(TARGET): $(OBJ_FILES) $(EMULATOR_LIB) $(UTILS_LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJ_FILES) $(EMULATOR_LIB) $(UTILS_LIB)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all run clean
