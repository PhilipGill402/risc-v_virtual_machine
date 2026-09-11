CC = clang

CFLAGS = -Wall -Wextra -std=c11 \
	-Iinclude \
	-I../emulator/include \
	-I../utils/include

SRC_DIR = src
OBJ_DIR = ../build/debug

TARGET = ../riscv_debug

EMULATOR_DIR = ../emulator
EMULATOR_LIB = ../build/libriscv.a

UTILS_DIR = ../utils
UTILS_LIB = ../build/libutils.a

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

all: $(TARGET)

$(EMULATOR_LIB):
	$(MAKE) -C $(EMULATOR_DIR)

$(UTILS_LIB):
	$(MAKE) -C $(UTILS_DIR)

$(TARGET): $(OBJ_FILES) $(EMULATOR_LIB) $(UTILS_LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJ_FILES) $(EMULATOR_LIB) $(UTILS_LIB)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all run clean
