# Default platform target
PLATFORM ?= virt

# Toolchain definitions
CROSS_COMPILE ?= aarch64-none-elf-
CC            := $(CROSS_COMPILE)gcc
CXX           := $(CROSS_COMPILE)g++
OBJCOPY       := $(CROSS_COMPILE)objcopy
QEMU          := qemu-system-aarch64

# Flags
COMMON_FLAGS  := -Wall -ffreestanding -nostdlib -mgeneral-regs-only -fno-stack-protector
CFLAGS        := $(COMMON_FLAGS) -Iinclude $(CPPFLAGS)
LDFLAGS       := -T arch/arm64/link.ld -nostdlib $(CPPFLAGS)

ifeq ($(PLATFORM),opi)
    CPPFLAGS += -DMACHINE_OPI
endif

# Build Directory
BUILD_DIR     := build

# Source Files
ARCH_ASM_SRCS := $(wildcard arch/arm64/*.S)
ARCH_C_SRCS   := $(wildcard arch/arm64/*.c)
BASE_SRCS     := $(shell find base -type f -name "*.c")
PLATFORM_SRCS := $(wildcard platform/$(PLATFORM)/*.c)
LIBKERN_SRCS  := $(wildcard libkern/*.c)

# Object Files
ARCH_ASM_OBJS := $(patsubst arch/arm64/%.S, $(BUILD_DIR)/arch_%.o, $(ARCH_ASM_SRCS))
ARCH_C_OBJS   := $(patsubst arch/arm64/%.c, $(BUILD_DIR)/arch_%.o, $(ARCH_C_SRCS))
BASE_OBJS     := $(patsubst base/%.c, $(BUILD_DIR)/base/%.o, $(BASE_SRCS))
PLATFORM_OBJS := $(patsubst platform/$(PLATFORM)/%.c, $(BUILD_DIR)/plat_%.o, $(PLATFORM_SRCS))
LIBKERN_OBJS  := $(patsubst libkern/%.c, $(BUILD_DIR)/libkern_%.o, $(LIBKERN_SRCS))

ALL_OBJS      := $(ARCH_ASM_OBJS) $(ARCH_C_OBJS) $(BASE_OBJS) $(PLATFORM_OBJS) $(LIBKERN_OBJS)

# Target outputs
TARGET_ELF    := kernel8.elf
TARGET_BIN    := kernel8.bin

# Default Rule
all: $(TARGET_BIN)

# Link ELF
$(TARGET_ELF): $(ALL_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# Create Binary Image
$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

# Compile Assembly Sources in arch/arm64/
$(BUILD_DIR)/arch_%.o: arch/arm64/%.S
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C Sources in arch/arm64/
$(BUILD_DIR)/arch_%.o: arch/arm64/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Base C Sources
$(BUILD_DIR)/base/%.o: base/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/libkern_%.o: libkern/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Platform-Specific C Sources
$(BUILD_DIR)/plat_%.o: platform/$(PLATFORM)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run in QEMU
qemu: $(TARGET_ELF)
	$(QEMU) -machine virt -cpu cortex-a55 -m 128M -nographic -kernel $(TARGET_ELF)

debug: $(TARGET_ELF)
	$(QEMU) -machine virt -cpu cortex-a55 -m 128M -nographic -kernel $(TARGET_ELF) -s -S

# Clean Build Artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET_ELF) $(TARGET_BIN)

.PHONY: all qemu clean