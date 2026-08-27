ENTRY(_start)

SECTIONS
{
    . = 0x00200000;

    .text : {
        KEEP(*(.text.boot))
        *(.text*)
    }

    .rodata : { *(.rodata*) }
    .data : { *(.data*) }

    .bss ALIGN(8) : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    /DISCARD/ : { *(.comment) *(.gnu*) }
}