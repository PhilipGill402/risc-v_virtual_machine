# Compiler and flags

CC = clang++
CFLAGS = -Wall -Wextra -std=c++23 -Iinclude -Itest

# Directories

SRC_DIR = src
OBJ_DIR = build
TARGET = vm

# Find all .c files recursively

SRC_FILES := $(shell find $(SRC_DIR) -name '*.cpp')

# Convert:
# src/main.c            -> build/src/main.o

SRC_OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/src/%.o,$(SRC_FILES))

OBJ_FILES := $(SRC_OBJ_FILES)

# Default target

all: $(TARGET)

# Link everything

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $(OBJ_FILES)

# Compile src files

$(OBJ_DIR)/src/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Run emulator

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
