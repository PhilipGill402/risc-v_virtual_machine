CC = clang
AR = ar

CFLAGS = -std=c11 -O2 -Wall -Wextra -Iinclude

BUILD_DIR = ../build
OBJ_DIR = $(BUILD_DIR)/utils
TARGET = $(BUILD_DIR)/libutils.a

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
