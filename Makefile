# Compiler and flags

CC = clang
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

# Directories

SRC_DIR = src
OBJ_DIR = build
TEST_DIR = tests

# Targets

TARGET = vm
TEST_TARGET = test_vm

# Find all .c files recursively

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')

# Convert:
# src/main.c            -> build/src/main.o

SRC_OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/src/%.o,$(SRC_FILES))
OBJ_FILES := $(SRC_OBJ_FILES)

# Test files

TEST_SRC = $(TEST_DIR)/test.c
TEST_OBJ = $(OBJ_DIR)/tests/test.o

# Do not link the normal main.c into the test executable,
# since tests/test.c should provide the test main().
TEST_SRC_OBJ_FILES := $(filter-out $(OBJ_DIR)/src/main.o,$(SRC_OBJ_FILES))
TEST_OBJ_FILES := $(TEST_SRC_OBJ_FILES) $(TEST_OBJ)

# Default target

all: $(TARGET)

# Link emulator

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $(OBJ_FILES)

# Compile src files

$(OBJ_DIR)/src/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile tests/test.c.

$(TEST_OBJ): $(TEST_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(TEST_DIR) -c $(TEST_SRC) -o $@

# Build and run tests

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ_FILES)
	$(CC) $(CFLAGS) -I$(TEST_DIR) -o $@ $(TEST_OBJ_FILES)

# Clean

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

# Run emulator

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run test
