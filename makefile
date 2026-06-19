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

# ===== Tests, tier 2: on-target (boots a test kernel under QEMU)
# Same kernel sources compiled with -DKTEST_BUILD (kernel_main runs the test
# runner after memory_init), plus the runner and cases under tests/kernel/.
KTEST_DIR        = tests/kernel
KTEST_SRCS      := $(shell find $(KTEST_DIR) -name '*.c')
KTEST_OBJ_DIR    = $(BUILD_DIR)/ktest-obj
KTEST_OBJS      := $(patsubst $(SRC_DIR)/%.c,$(KTEST_OBJ_DIR)/src/%.o,$(SRCS)) \
                   $(patsubst $(KTEST_DIR)/%.c,$(KTEST_OBJ_DIR)/t/%.o,$(KTEST_SRCS))
KTEST_DEPS      := $(KTEST_OBJS:.o=.d)
KTEST_CFLAGS    := $(CFLAGS) -DKTEST_BUILD -I$(KTEST_DIR)
KTEST_KERNEL_ELF = $(BUILD_DIR)/ktest-kernel.elf
KTEST_IMG        = $(BUILD_DIR)/ktest.img

# isa-debug-exit lets the kernel set the QEMU exit code; -no-reboot turns a
# triple fault into an exit (not a boot loop) so a crashing test still fails.
KTEST_QEMU_FLAGS = -machine q35 -m 256M \
    -drive if=pflash,format=raw,unit=0,readonly=on,file=$(OVMF_CODE) \
    -drive if=pflash,format=raw,unit=1,file=$(OVMF_VARS) \
    -drive if=none,id=esp,format=raw,file=$(KTEST_IMG) \
    -device ahci,id=ahci -device ide-hd,drive=esp,bus=ahci.0 \
    -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
    -serial stdio -display none -no-reboot

# ===== Tests, tier 1: host (native unit tests for freestanding-pure libs)
HOST_CC         = clang
HOST_TEST_DIR   = tests/host
HOST_TEST_SRCS := $(wildcard $(HOST_TEST_DIR)/*.c)
HOST_UNIT_SRCS  = $(SRC_DIR)/libs/string.c $(SRC_DIR)/libs/stdlib.c $(SRC_DIR)/libs/stdio.c
HOST_TEST_BIN   = $(BUILD_DIR)/host-tests
# -Wno-format: our printf uses non-standard length specifiers (%l/%L), so libc's
# format checking doesn't apply.
HOST_TEST_CFLAGS = -Iinclude -I$(HOST_TEST_DIR) -fno-builtin -g -O1 -Wall -Wno-format

.PHONY: all clean run debug test test-host test-kernel
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

# ===== Tests
# Run everything (host first — it's fast and catches pure-logic regressions).
test: test-host test-kernel
	@echo "== All test suites passed =="

# --- Tier 1: host unit tests
$(HOST_TEST_BIN): $(HOST_TEST_SRCS) $(HOST_UNIT_SRCS) $(HOST_TEST_DIR)/htest.h | $(BUILD_DIR)
	$(HOST_CC) $(HOST_TEST_CFLAGS) $(HOST_TEST_SRCS) $(HOST_UNIT_SRCS) -o $@

test-host: $(HOST_TEST_BIN)
	@echo "== Running host (unit) tests =="
	@./$(HOST_TEST_BIN)

# --- Tier 2: on-target tests
$(KTEST_OBJ_DIR)/src/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(KTEST_CFLAGS) -c $< -o $@

$(KTEST_OBJ_DIR)/t/%.o: $(KTEST_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(KTEST_CFLAGS) -c $< -o $@

$(KTEST_KERNEL_ELF): $(KTEST_OBJS) $(ASM_OBJS) $(KERNEL_LD) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(KTEST_OBJS) $(ASM_OBJS)

$(KTEST_IMG): $(KTEST_KERNEL_ELF) $(LIMINE_CFG) $(LIMINE_EFI) | $(BUILD_DIR)
	rm -f $@
	dd if=/dev/zero of=$@ bs=1m count=64
	$(MFORMAT) -i $@ -F ::
	$(MMD) -i $@ ::/EFI ::/EFI/BOOT ::/boot
	$(MCOPY) -i $@ $(LIMINE_EFI)        ::/EFI/BOOT/BOOTX64.EFI
	$(MCOPY) -i $@ $(KTEST_KERNEL_ELF)  ::/boot/kernel.elf
	$(MCOPY) -i $@ $(LIMINE_CFG)        ::/boot/limine.conf

# QEMU exits (val<<1)|1: ktest writes 0 on pass (exit 1), 1 on fail (exit 3).
test-kernel: $(KTEST_IMG) $(OVMF_VARS)
	@echo "== Running on-target (QEMU) tests =="
	@$(QEMU) $(KTEST_QEMU_FLAGS); \
	  code=$$?; \
	  if [ $$code -eq 1 ]; then echo "== On-target tests PASSED =="; \
	  else echo "== On-target tests FAILED (qemu exit $$code) =="; exit 1; fi

clean:
	rm -rf $(BUILD_DIR)

# Pull in auto-generated header dependencies so editing a .h recompiles every
# .c that includes it. The '-' silences the first-build case where they're absent.
-include $(DEPS)
-include $(KTEST_DEPS)
