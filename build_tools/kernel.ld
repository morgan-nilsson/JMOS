OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kernel_main)

PHDRS {
    text    PT_LOAD;
    rodata  PT_LOAD;
    data    PT_LOAD;
}

SECTIONS {
    /* Canonical higher-half base for x86-64 kernels. */
    . = 0xffffffff80000000;

    .text : {
        *(.text .text.*)
    } :text

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    .limine_requests : {
        KEEP(*(.limine_requests_start))
        KEEP(*(.limine_requests))
        KEEP(*(.limine_requests_end))
    } :rodata

    /* Test-case registry (ktest). Empty in normal builds; populated only when
       tests/kernel/*.c is compiled in (the `test-kernel` target). */
    .ktests : {
        . = ALIGN(8);
        __start_ktests = .;
        KEEP(*(.ktests))
        __stop_ktests = .;
    } :rodata

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .data : {
        *(.data .data.*)
    } :data

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data

    /* Host-toolchain noise */
    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
        *(.comment)
    }
}
