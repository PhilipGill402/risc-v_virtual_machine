CC      ?= clang
AR      ?= ar
DTC     ?= dtc

CFLAGS  ?= -Wall -Wextra -Wpedantic -std=c11 -g
LDFLAGS ?=

# ============================================================
# Directories
# ============================================================

SRC_DIR      := src
INC_DIR      := include
APP_DIR      := apps
BUILD_DIR    := build

UTIL_DIR     := utils
UTIL_INC     := $(UTIL_DIR)/include
UTIL_LIB     := $(BUILD_DIR)/libutils.a

DEBUGGER_DIR := tools/debugger
DEBUGGER_INC := $(DEBUGGER_DIR)/include
DEBUGGER_SRC := $(DEBUGGER_DIR)/src

# ============================================================
# Device Tree
# ============================================================

VM_DTS := $(SRC_DIR)/vm/vm.dts
VM_DTB := $(BUILD_DIR)/vm.dtb

# ============================================================
# Output files
# ============================================================

VM_BIN       := $(BUILD_DIR)/vm
DEBUGGER_BIN := $(BUILD_DIR)/debugger
VM_LIB       := $(BUILD_DIR)/libvm.a

# ============================================================
# VM sources
# ============================================================

VM_SRCS := $(shell find $(SRC_DIR) -name '*.c')

VM_OBJS := $(patsubst \
	$(SRC_DIR)/%.c, \
	$(BUILD_DIR)/obj/vm/%.o, \
	$(VM_SRCS))

VM_MAIN     := $(APP_DIR)/vm_main.c
VM_MAIN_OBJ := $(BUILD_DIR)/obj/apps/vm_main.o

# ============================================================
# Debugger sources
# ============================================================

DEBUGGER_SRCS := $(shell find $(DEBUGGER_SRC) -name '*.c')

DEBUGGER_OBJS := $(patsubst \
	$(DEBUGGER_SRC)/%.c, \
	$(BUILD_DIR)/obj/debugger/%.o, \
	$(DEBUGGER_SRCS))

# ============================================================
# Includes
# ============================================================

CPPFLAGS := \
	-I$(INC_DIR) \
	-I$(UTIL_INC) \
	-I$(DEBUGGER_INC)

# ============================================================
# Public targets
# ============================================================

.PHONY: all vm libvm debugger utils clean

all: vm debugger

# ============================================================
# VM executable
# ============================================================

vm: $(VM_BIN)

$(VM_BIN): $(VM_MAIN_OBJ) $(VM_LIB) $(UTIL_LIB) $(VM_DTB)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) \
		$(VM_MAIN_OBJ) \
		$(VM_LIB) \
		$(UTIL_LIB) \
		-o $@

# ============================================================
# Device Tree
# ============================================================

$(VM_DTB): $(VM_DTS)
	@mkdir -p $(dir $@)
	$(DTC) -I dts -O dtb -o $@ $<

# ============================================================
# libvm static library
# ============================================================

libvm: $(VM_LIB)

$(VM_LIB): $(VM_OBJS) $(UTIL_LIB)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $(VM_OBJS)

# ============================================================
# Debugger executable
# ============================================================

debugger: $(DEBUGGER_BIN)

$(DEBUGGER_BIN): $(DEBUGGER_OBJS) $(VM_LIB) $(UTIL_LIB)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) \
		$(DEBUGGER_OBJS) \
		$(VM_LIB) \
		$(UTIL_LIB) \
		-o $@

# ============================================================
# VM source compilation
# ============================================================

$(BUILD_DIR)/obj/vm/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ============================================================
# VM main compilation
# ============================================================

$(VM_MAIN_OBJ): $(VM_MAIN)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ============================================================
# Debugger source compilation
# ============================================================

$(BUILD_DIR)/obj/debugger/%.o: $(DEBUGGER_SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ============================================================
# Utils
# ============================================================

utils:
	$(MAKE) -C $(UTIL_DIR)

$(UTIL_LIB):
	$(MAKE) -C $(UTIL_DIR)

# ============================================================
# Cleanup
# ============================================================

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C $(UTIL_DIR) clean
