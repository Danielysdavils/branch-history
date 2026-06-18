CC := gcc

CFLAGS := -O2 \
	-Wall \
	-Wextra \
	-std=c11 \
	-fno-if-conversion \
	-fno-tree-vectorize \
	-fno-unroll-loops \
	-fno-inline

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

COMMON_SRC := $(SRC_DIR)/common.c
COMMON_HDR := $(SRC_DIR)/common.h
COMMON_OBJ := $(BUILD_DIR)/common.o

EXPERIMENTS := exp1_global_history exp2_global_distance exp3_local_history exp4_effective_size

BINS := $(addprefix $(BIN_DIR)/,$(EXPERIMENTS))

.PHONY: all dirs asm clean

all: dirs $(BINS)

dirs:
	mkdir -p $(BUILD_DIR) $(BIN_DIR)

$(COMMON_OBJ): $(COMMON_SRC) $(COMMON_HDR) | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BINS): $(BIN_DIR)/%: $(SRC_DIR)/%.c $(COMMON_OBJ) $(COMMON_HDR) | dirs
	$(CC) $(CFLAGS) $< $(COMMON_OBJ) -o $@

asm: all
	for bin in $(BINS); do \
		name=$$(basename $$bin); \
		objdump -d $$bin > $(BUILD_DIR)/$$name.asm; \
	done

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)