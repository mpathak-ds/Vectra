# Default platform target
PLATFORM ?= virt

# Toolchain definitions
CROSS_COMPILE ?= aarch64-none-elf-
CC            := $(CROSS_COMPILE)gcc
CXX           := $(CROSS_COMPILE)g++
OBJCOPY       := $(CROSS_COMPILE)objcopy
QEMU          := qemu-system-aarch64

# Flags
COMMON_FLAGS  := -Wall -O2 -ffreestanding -nostdlib
CFLAGS        := $(COMMON_FLAGS)
CXXFLAGS      := $(COMMON_FLAGS) -fno-exceptions -fno-rtti -Iinclude
LDFLAGS       := -T arch/arm64/link.ld -nostdlib

# Build Directory
BUILD_DIR     := build

# Source Files
ASM_SRCS      := arch/arm64/entry.S
BASE_SRCS     := base/kernel.cpp
PLATFORM_SRCS := $(wildcard platform/$(PLATFORM)/*.cpp)

# Object Files
ASM_OBJS      := $(patsubst arch/arm64/%.S, $(BUILD_DIR)/%.o, $(ASM_SRCS))
BASE_OBJS     := $(patsubst base/%.cpp, $(BUILD_DIR)/%.o, $(BASE_SRCS))
PLATFORM_OBJS := $(patsubst platform/$(PLATFORM)/%.cpp, $(BUILD_DIR)/%.o, $(PLATFORM_SRCS))

ALL_OBJS      := $(ASM_OBJS) $(BASE_OBJS) $(PLATFORM_OBJS)

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

# Compile Assembly Sources
$(BUILD_DIR)/%.o: arch/arm64/%.S
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Base C++ Sources
$(BUILD_DIR)/%.o: base/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Platform-Specific C++ Sources
$(BUILD_DIR)/%.o: platform/$(PLATFORM)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run in QEMU
qemu: $(TARGET_ELF)
	$(QEMU) -machine virt -cpu cortex-a55 -m 128M -nographic -kernel $(TARGET_ELF)

# Clean Build Artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET_ELF) $(TARGET_BIN)

.PHONY: all qemu clean