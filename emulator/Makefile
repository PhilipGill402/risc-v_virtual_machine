CC = clang
AR = ar

CFLAGS = -Wall -Wextra -std=c11 -Iinclude -I../utils/include

SRC_DIR = src
BUILD_DIR = ../build
OBJ_DIR = $(BUILD_DIR)/emulator

TARGET = $(BUILD_DIR)/libriscv.a
UTILS_LIB = $(BUILD_DIR)/libutils.a

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

all: $(TARGET)

# Build the utils library if needed
$(UTILS_LIB):
	$(MAKE) -C ../utils

# Build the emulator static library
$(TARGET): $(OBJ_FILES) $(UTILS_LIB)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
