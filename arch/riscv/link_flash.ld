OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x20000000, LENGTH = 4M
    RAM (rwx)   : ORIGIN = 0x80000000, LENGTH = 16M
}

STACK_SIZE = 16K;

SECTIONS
{
    .text :
    {
        KEEP(*(.text.entry))
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .data :
    {
        _data_start = .;
        *(.data .data.*)
        *(.sdata .sdata.*)
        *(.got .got.*)
        *(.got.plt)
        . = ALIGN(4);
        _data_end = .;
    } > RAM AT > FLASH

    _data_lma = LOADADDR(.data);

    .bss (NOLOAD) :
    {
        _bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > RAM

    . = ALIGN(8);
    _heap_start = .;
    _heap_end = ORIGIN(RAM) + LENGTH(RAM) - STACK_SIZE;

    .stack (NOLOAD) :
    {
        . = ALIGN(16);
        _stack_bottom = .;
        . += STACK_SIZE;
        _stack_top = .;
    } > RAM
}