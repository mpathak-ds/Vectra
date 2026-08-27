# Default platform target
PLATFORM ?= virt

# Toolchain definitions
CROSS_COMPILE ?= aarch64-none-elf-
CC            := $(CROSS_COMPILE)gcc
CXX           := $(CROSS_COMPILE)g++
OBJCOPY       := $(CROSS_COMPILE)objcopy
QEMU          := qemu-system-aarch64

# Flags
COMMON_FLAGS  := -Wall -ffreestanding -nostdlib -mgeneral-regs-only -fno-stack-protector -fno-threadsafe-statics -fno-use-cxa-atexit
CFLAGS        := $(COMMON_FLAGS)
CXXFLAGS      := $(COMMON_FLAGS) -fno-exceptions -fno-rtti -Iinclude
LDFLAGS       := -T arch/arm64/link.ld -nostdlib

ifeq ($(PLATFORM),opi)
    CPPFLAGS += -DMACHINE_OPI
endif

# Build Directory
BUILD_DIR     := build

# Source Files
ARCH_ASM_SRCS := $(wildcard arch/arm64/*.S)
ARCH_CPP_SRCS := $(wildcard arch/arm64/*.cpp)
BASE_SRCS     := $(wildcard base/*.cpp)
PLATFORM_SRCS := $(wildcard platform/$(PLATFORM)/*.cpp)
LIBKERN_SRCS  := $(wildcard libkern/*.cpp)

# Object Files
ARCH_ASM_OBJS := $(patsubst arch/arm64/%.S, $(BUILD_DIR)/arch_%.o, $(ARCH_ASM_SRCS))
ARCH_CPP_OBJS := $(patsubst arch/arm64/%.cpp, $(BUILD_DIR)/arch_%.o, $(ARCH_CPP_SRCS))
BASE_OBJS     := $(patsubst base/%.cpp, $(BUILD_DIR)/base_%.o, $(BASE_SRCS))
PLATFORM_OBJS := $(patsubst platform/$(PLATFORM)/%.cpp, $(BUILD_DIR)/plat_%.o, $(PLATFORM_SRCS))
LIBKERN_OBJS  := $(patsubst libkern/%.cpp, $(BUILD_DIR)/libkern_%.o, $(LIBKERN_SRCS))

ALL_OBJS      := $(ARCH_ASM_OBJS) $(ARCH_CPP_OBJS) $(BASE_OBJS) $(PLATFORM_OBJS) $(LIBKERN_OBJS)

# Target outputs
TARGET_ELF    := kernel8.elf
TARGET_BIN    := kernel8.bin

# Default Rule
all: $(TARGET_BIN)

# Link ELF
$(TARGET_ELF): $(ALL_OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

# Create Binary Image
$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

# Compile Assembly Sources in arch/arm64/
$(BUILD_DIR)/arch_%.o: arch/arm64/%.S
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ Sources in arch/arm64/
$(BUILD_DIR)/arch_%.o: arch/arm64/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Base C++ Sources
$(BUILD_DIR)/base_%.o: base/%.cpp
	@mkdir -p $(BUILD_DIR)
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
	rm -rf $(BUILD_DIR) $(TARGET_ELF) $(TARGET_BIN)

.PHONY: all qemu clean