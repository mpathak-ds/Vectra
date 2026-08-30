# Default platform target
PLATFORM ?= virt
ARCH ?= arm64

# Toolchain definitions
CROSS_COMPILE ?= aarch64-none-elf-
CC            := $(CROSS_COMPILE)gcc
CXX           := $(CROSS_COMPILE)g++
OBJCOPY       := $(CROSS_COMPILE)objcopy
OBJDUMP       := $(CROSS_COMPILE)objdump
NM		      := $(CROSS_COMPILE)nm
QEMU          := qemu-system-aarch64

# Flags
ifeq ($(ARCH),riscv)
	COMMON_FLAGS  := -Wall -ffreestanding -nostdlib -fno-stack-protector -fno-threadsafe-statics -mcmodel=medany -DARCH_SPEC_RISCV
	LDFLAGS       := -T arch/riscv/link.ld -nostdlib $(CPPFLAGS)
else ifeq ($(ARCH),arm64)
	COMMON_FLAGS  := -Wall -ffreestanding -nostdlib -mgeneral-regs-only -fno-stack-protector -fno-threadsafe-statics -fno-use-cxa-atexit #-g
	LDFLAGS       := -T arch/arm64/link.ld -nostdlib $(CPPFLAGS) -DARCH_SPEC_ARM64
endif
CFLAGS        := $(COMMON_FLAGS) $(CPPFLAGS)
CXXFLAGS      := $(COMMON_FLAGS) -fno-exceptions -fno-rtti -Iinclude $(CPPFLAGS)

ifeq ($(PLATFORM),opi)
    CPPFLAGS += -DMACHINE_OPI
endif

# Build Directory
BUILD_DIR     := build

# Source Files
ifeq ($(ARCH),riscv)
	ARCH_ASM_SRCS := $(wildcard arch/riscv/*.S)
	ARCH_CPP_SRCS := $(wildcard arch/riscv/*.cpp)
else ifeq ($(ARCH),arm64)
	ARCH_ASM_SRCS := $(wildcard arch/arm64/*.S)
	ARCH_CPP_SRCS := $(wildcard arch/arm64/*.cpp)
endif
BASE_SRCS     := $(shell find base -type f -name "*.cpp")
PLATFORM_SRCS := $(wildcard platform/$(PLATFORM)/*.cpp)
LIBKERN_SRCS  := $(wildcard libkern/*.cpp)

# Object Files
ifeq ($(ARCH),riscv)
	ARCH_ASM_OBJS := $(patsubst arch/riscv/%.S, $(BUILD_DIR)/arch_%.o, $(ARCH_ASM_SRCS))
	ARCH_CPP_OBJS := $(patsubst arch/riscv/%.cpp, $(BUILD_DIR)/arch_%.o, $(ARCH_CPP_SRCS))
else ifeq ($(ARCH),arm64)
	ARCH_ASM_OBJS := $(patsubst arch/arm64/%.S, $(BUILD_DIR)/arch_%.o, $(ARCH_ASM_SRCS))
	ARCH_CPP_OBJS := $(patsubst arch/arm64/%.cpp, $(BUILD_DIR)/arch_%.o, $(ARCH_CPP_SRCS))
endif
BASE_OBJS     := $(patsubst base/%.cpp, $(BUILD_DIR)/base/%.o, $(BASE_SRCS))
PLATFORM_OBJS := $(patsubst platform/$(PLATFORM)/%.cpp, $(BUILD_DIR)/plat_%.o, $(PLATFORM_SRCS))
LIBKERN_OBJS  := $(patsubst libkern/%.cpp, $(BUILD_DIR)/libkern_%.o, $(LIBKERN_SRCS))

ALL_OBJS      := $(ARCH_ASM_OBJS) $(ARCH_CPP_OBJS) $(BASE_OBJS) $(PLATFORM_OBJS) $(LIBKERN_OBJS)

# Target outputs
TARGET_ELF    := kernel8.elf
TARGET_BIN    := kernel8.bin
SYMBOLS_FILE  := kernel8.sym
DISAS_FILE	  := kernel8.disas

# Default Rule
all: $(TARGET_BIN)

# Link ELF
$(TARGET_ELF): $(ALL_OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@
	$(NM) -n $@ > $(SYMBOLS_FILE)
	$(OBJDUMP) -d -S $@ > $(DISAS_FILE)

# Create Binary Image
$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

# Compile Assembly Sources in arch/arm64/
$(BUILD_DIR)/arch_%.o: arch/riscv/%.S
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ Sources in arch/arm64/
$(BUILD_DIR)/arch_%.o: arch/riscv/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Base C++ Sources
$(BUILD_DIR)/base/%.o: base/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/libkern_%.o: libkern/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Platform-Specific C++ Sources
$(BUILD_DIR)/plat_%.o: platform/$(PLATFORM)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run in QEMU
qemu: $(TARGET_ELF)
	$(QEMU) -machine virt -cpu cortex-a55 -m 128M -nographic -kernel $(TARGET_ELF)

debug: $(TARGET_ELF)
	$(QEMU) -machine virt -cpu cortex-a55 -m 128M -nographic -kernel $(TARGET_ELF) -s -S

# Clean Build Artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET_ELF) $(TARGET_BIN) $(SYMBOLS_FILE) $(DISAS_FILE)

.PHONY: all qemu clean