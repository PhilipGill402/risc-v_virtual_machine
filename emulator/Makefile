CC = clang
AR = ar

CFLAGS = -Wall -Wextra -std=c11 -Iinclude

SRC_DIR = src
BUILD_DIR = ../build
OBJ_DIR = $(BUILD_DIR)/emulator

TARGET = $(BUILD_DIR)/libriscv.a

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
