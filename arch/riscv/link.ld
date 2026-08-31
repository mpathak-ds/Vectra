ENTRY(_start)

SECTIONS
{
    . = 0x80200000;

    __text_start = .;

    .text : ALIGN(8) {
        KEEP(*(.text.boot))
        *(.text*)
    }

    .rodata : ALIGN(8) {
        *(.rodata*)
    }

    .data : ALIGN(8) {
        *(.data*)
    }

    .bss ALIGN(8) : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    . = ALIGN(16);
    __heap_start = .;
    . += 0x400000; /* Reserve 4 MB for early boot heap */
    __heap_end = .;

    __text_end = .;
    /DISCARD/ : { *(.comment) *(.gnu*) }
}