# Default platform target
PLATFORM ?= virt
ARCH ?= arm64

# Toolchain definitions
CROSS_COMPILE ?= aarch64-none-elf-
CC            := $(CROSS_COMPILE)gcc
OBJCOPY       := $(CROSS_COMPILE)objcopy
OBJDUMP       := $(CROSS_COMPILE)objdump
NM		      := $(CROSS_COMPILE)nm
QEMU          := qemu-system-aarch64

ifeq ($(PLATFORM),opi)
    CFLAGS += -DMACHINE_OPI
endif

ifeq ($(ARCH),riscv)
	COMMON_FLAGS  := -Wall -ffreestanding -nostdlib -fno-stack-protector -mcmodel=medany -DARCH_SPEC_RISCV
	LDFLAGS       := -T arch/riscv/link.ld -nostdlib $(CFLAGS)
else ifeq ($(ARCH),arm64)
	COMMON_FLAGS  := -Wall -ffreestanding -nostdlib -mgeneral-regs-only -fno-stack-protector -DARCH_SPEC_ARM64 #-g
	LDFLAGS       := -T arch/arm64/link.ld -nostdlib $(CFLAGS)
endif

CCFLAGS       := $(COMMON_FLAGS) -Iinclude -std=gnu99 $(CPPFLAGS)

# Build Directory
BUILD_DIR     := build

# Source Files
ifeq ($(ARCH),riscv)
	ARCH_ASM_SRCS := $(wildcard arch/riscv/*.S)
	ARCH_C_SRCS := $(wildcard arch/riscv/*.c)
else ifeq ($(ARCH),arm64)
	ARCH_ASM_SRCS := $(wildcard arch/arm64/*.S)
	ARCH_C_SRCS := $(wildcard arch/arm64/*.c)
endif
BASE_SRCS     := $(shell find base -type f -name "*.c")
PLATFORM_SRCS := $(wildcard platform/$(PLATFORM)/*.c)
LIBKERN_SRCS  := $(wildcard libkern/*.c)

# Object Files
ifeq ($(ARCH),riscv)
	ARCH_ASM_OBJS := $(patsubst arch/riscv/%.S, $(BUILD_DIR)/riscv_%.o, $(ARCH_ASM_SRCS))
	ARCH_C_OBJS := $(patsubst arch/riscv/%.c, $(BUILD_DIR)/riscv_%.o, $(ARCH_C_SRCS))
else ifeq ($(ARCH),arm64)
	ARCH_ASM_OBJS := $(patsubst arch/arm64/%.S, $(BUILD_DIR)/arm64_%.o, $(ARCH_ASM_SRCS))
	ARCH_C_OBJS := $(patsubst arch/arm64/%.c, $(BUILD_DIR)/arm64_%.o, $(ARCH_C_SRCS))
endif
BASE_OBJS     := $(patsubst base/%.c, $(BUILD_DIR)/base/%.o, $(BASE_SRCS))
PLATFORM_OBJS := $(patsubst platform/$(PLATFORM)/%.c, $(BUILD_DIR)/plat_%.o, $(PLATFORM_SRCS))
LIBKERN_OBJS  := $(patsubst libkern/%.c, $(BUILD_DIR)/libkern_%.o, $(LIBKERN_SRCS))

ALL_OBJS      := $(ARCH_ASM_OBJS) $(ARCH_C_OBJS) $(BASE_OBJS) $(PLATFORM_OBJS) $(LIBKERN_OBJS)

# Target outputs
TARGET_ELF    := kernel8.elf
TARGET_BIN    := kernel8.bin
SYMBOLS_FILE  := kernel8.sym
DISAS_FILE	  := kernel8.disas

# Default Rule
all: $(TARGET_BIN)

# Link ELF
$(TARGET_ELF): $(ALL_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	$(NM) -n $@ > $(SYMBOLS_FILE)
	$(OBJDUMP) -d -S $@ > $(DISAS_FILE)

# Create Binary Image
$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

# Compile Assembly Sources in arch/arm64/
$(BUILD_DIR)/riscv_%.o: arch/riscv/%.S
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ Sources in arch/arm64/
$(BUILD_DIR)/riscv_%.o: arch/riscv/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/arm64_%.o: arch/arm64/%.S
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ Sources in arch/arm64/
$(BUILD_DIR)/arm64_%.o: arch/arm64/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

# Compile Base C++ Sources
$(BUILD_DIR)/base/%.o: base/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/libkern_%.o: libkern/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

# Compile Platform-Specific C++ Sources
$(BUILD_DIR)/plat_%.o: platform/$(PLATFORM)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

# Run in QEMU
qemu: $(TARGET_ELF)
	$(QEMU) -machine virt -cpu cortex-a55 -m 128M -nographic -kernel $(TARGET_ELF)

debug: $(TARGET_ELF)
	$(QEMU) -machine virt -cpu cortex-a55 -m 128M -nographic -kernel $(TARGET_ELF) -s -S

# Clean Build Artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET_ELF) $(TARGET_BIN) $(SYMBOLS_FILE) $(DISAS_FILE)

.PHONY: all qemu clean
