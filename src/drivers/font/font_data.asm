; Embeds the PSF2 font file directly into the kernel image so the driver has a
; pointer to its raw bytes with no filesystem/bootloader module needed.
; The incbin path is resolved relative to the directory make runs nasm from
; (the project root); embedded assets live under assets/.

bits 64
section .rodata

global spleen_font_start
global spleen_font_end

align 16
spleen_font_start:
    incbin "assets/spleen-16x32.psfu"
spleen_font_end:
