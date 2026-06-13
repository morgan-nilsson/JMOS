LLVM_PREFIX   := $(shell brew --prefix llvm)
# lld ships as its own keg
LLD_PREFIX    := $(shell brew --prefix lld@20 2>/dev/null || brew --prefix lld)
QEMU_PREFIX   := $(shell brew --prefix qemu)
LIMINE_PREFIX := $(shell brew --prefix limine)
MTOOLS_PREFIX := $(shell brew --prefix mtools)
NASM_PREFIX   := $(shell brew --prefix nasm)

# ===== Toolchain
CC      := $(LLVM_PREFIX)/bin/clang
LD      := $(LLD_PREFIX)/bin/ld.lld
ASM     := $(NASM_PREFIX)/bin/nasm
MFORMAT := $(MTOOLS_PREFIX)/bin/mformat
MMD     := $(MTOOLS_PREFIX)/bin/mmd
MCOPY   := $(MTOOLS_PREFIX)/bin/mcopy
QEMU    := $(QEMU_PREFIX)/bin/qemu-system-x86_64

# ===== Layout
SRC_DIR    = src
BUILD_DIR  = build
OBJ_DIR    = $(BUILD_DIR)/obj
TOOLS_DIR  = build_tools
# external binaries/objects embedded into the kernel image (via incbin)
ASSETS_DIR = assets

KERNEL_LD  = $(TOOLS_DIR)/kernel.ld
LIMINE_CFG = limine.conf

# Compile every .c under src/ except the vendored Limine header dir.
SRCS := $(shell find $(SRC_DIR) -name '*.c' -not -path '$(SRC_DIR)/limine/*')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Auto-generated header dependency files (-MMD), pulled in at the bottom.
DEPS := $(OBJS:.o=.d)

# Assemble every .asm under src/ (NASM, Intel syntax).
ASM_SRCS := $(shell find $(SRC_DIR) -name '*.asm' -not -path '$(SRC_DIR)/limine/*')
ASM_OBJS := $(patsubst $(SRC_DIR)/%.asm,$(OBJ_DIR)/%.o,$(ASM_SRCS))

# ===== Kernel compile flags
# -target x86_64-elf      : freestanding bare-metal ELF, not macOS Mach-O
# -ffreestanding -nostdlib: no host libc / startup files
# -mcmodel=kernel         : code+data live in the top 2GB (the higher-half base)
# -fno-pic -fno-pie       : static image, no runtime self-relocation
# -mno-red-zone           : the 128B red zone is unsafe once we take interrupts
# -mno-sse -mno-sse2 -mno-mmx -mno-80387 : don't emit FP/SIMD; not enabled this early
# -fno-stack-protector    : no __stack_chk_guard symbol exists in freestanding land
CFLAGS := -target x86_64-elf -ffreestanding -nostdlib -std=c11 \
        -masm=intel \
        -mcmodel=kernel -fno-pic -fno-pie \
        -mno-red-zone -mno-sse -mno-sse2 -mno-mmx -mno-80387 \
        -fno-stack-protector -fno-stack-check \
        -I$(SRC_DIR)/limine \
        -Wall -Wextra -O2 -g \
        -MMD -MP \
		-Iinclude

# Logging verbosity (see include/libs/log.h). Override per build, e.g.
#   make run LOG_LEVEL=LOG_TRACE
# Defaults to LOG_INFO inside the header when unset.
ifdef LOG_LEVEL
CFLAGS += -DLOG_LEVEL=$(LOG_LEVEL)
endif
# ===== Kernel link flags
# -static / --no-dynamic-linker : fully static, no PT_INTERP
# -z max-page-size=0x1000       : 4K page alignment so the image stays compact
LDFLAGS := -nostdlib -static --no-dynamic-linker \
           -z max-page-size=0x1000 -z noexecstack -T $(KERNEL_LD)

# ===== Kernel assemble flags
# -f elf64 : 64-bit ELF objects to match the C output and linker target
# -g -F dwarf : DWARF debug info so asm symbols show up under lldb/gdb
# -I assets/ : let incbin reference embedded files by bare name
ASMFLAGS := -f elf64 -g -F dwarf -I$(ASSETS_DIR)/

# ===== Outputs
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
ESP_IMG    = $(BUILD_DIR)/jmos.img

# ===== Limine prebuilt artifacts
LIMINE_EFI = $(LIMINE_PREFIX)/share/limine/BOOTX64.EFI

# ===== OVMF firmware
OVMF_CODE     = $(QEMU_PREFIX)/share/qemu/edk2-x86_64-code.fd
OVMF_VARS_SRC = $(QEMU_PREFIX)/share/qemu/edk2-i386-vars.fd
OVMF_VARS     = $(BUILD_DIR)/ovmf-vars.fd

QEMU_FLAGS = -machine q35 -m 256M \
    -drive if=pflash,format=raw,unit=0,readonly=on,file=$(OVMF_CODE) \
    -drive if=pflash,format=raw,unit=1,file=$(OVMF_VARS) \
    -drive if=none,id=esp,format=raw,file=$(ESP_IMG) \
    -device ahci,id=ahci -device ide-hd,drive=esp,bus=ahci.0 \
    -serial stdio

.PHONY: all clean run debug test
all: $(ESP_IMG)

# ===== Kernel
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(@D)
	$(ASM) $(ASMFLAGS) $< -o $@

# The embedded font stub also depends on the file it incbin's.
$(OBJ_DIR)/drivers/font/font_data.o: $(ASSETS_DIR)/spleen-16x32.psfu

$(KERNEL_ELF): $(OBJS) $(ASM_OBJS) $(KERNEL_LD) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(ASM_OBJS)

# ===== EFI System Partition image
$(ESP_IMG): $(KERNEL_ELF) $(LIMINE_CFG) $(LIMINE_EFI) | $(BUILD_DIR)
	rm -f $@
	dd if=/dev/zero of=$@ bs=1m count=64
	$(MFORMAT) -i $@ -F ::
	$(MMD) -i $@ ::/EFI ::/EFI/BOOT ::/boot
	$(MCOPY) -i $@ $(LIMINE_EFI)  ::/EFI/BOOT/BOOTX64.EFI
	$(MCOPY) -i $@ $(KERNEL_ELF)  ::/boot/kernel.elf
	$(MCOPY) -i $@ $(LIMINE_CFG)  ::/boot/limine.conf

$(OVMF_VARS): $(OVMF_VARS_SRC) | $(BUILD_DIR)
	cp $(OVMF_VARS_SRC) $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(ESP_IMG) $(OVMF_VARS)
	$(QEMU) $(QEMU_FLAGS)

# Connect with: lldb -o "gdb-remote 1234" or gdb -ex "target remote :1234"
debug: $(ESP_IMG) $(OVMF_VARS)
	$(QEMU) $(QEMU_FLAGS) -s -S

test: $(ESP_IMG) $(OVMF_VARS)
	echo 'done'

clean:
	rm -rf $(BUILD_DIR)

# Pull in auto-generated header dependencies so editing a .h recompiles every
# .c that includes it. The '-' silences the first-build case where they're absent.
-include $(DEPS)
