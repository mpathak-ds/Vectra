ENTRY(_start)

SECTIONS
{
    #ifdef MACHINE_OPI
    . = 0x00200000;
    #else
    . = 0x40080000;
    #endif

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

    /DISCARD/ : { *(.comment) *(.gnu*) }
}