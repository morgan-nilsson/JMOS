brew install llvm lld mtools limine qemu
make        # build kernel + FAT EFI System Partition image (build/jmos.img)
make run    # boot in QEMU via the bundled OVMF firmware
make debug  # boot with a gdb/lldb stub on :1234 (-s -S)
make test   # headless smoke test: boot, screendump, assert the kernel drew