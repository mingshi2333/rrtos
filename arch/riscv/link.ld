OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    RAM (rwx)   : ORIGIN = 0x80000000, LENGTH = 4M
    FLASH (rx)  : ORIGIN = 0x20000000, LENGTH = 2M
}

STACK_SIZE = 16K;

SECTIONS
{
    .text :
    {
        *(.text.entry)
        *(.text .text.*)
        *(.rodata .rodata.*)
        . = ALIGN(4);
    } > RAM

    .data :
    {
        _data_start = .;
        *(.data .data.*)
        . = ALIGN(4);
        _data_end = .;
    } > RAM

    _data_lma = LOADADDR(.data);

    .got :
    {
        __got_start__ = .;
        *(.got)
        __got_end__ = .;
        . = ALIGN(4);
    } > RAM

    .got.plt :
    {
        __got_plt_start__ = .;
        *(.got.plt)
        __got_plt_end__ = .;
        . = ALIGN(4);
    } > RAM

    .bss :
    {
        _bss_start = .;
        *(.bss .bss.*)
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

    /DISCARD/ :
    {
        *(.eh_frame)
        *(.comment)
    }
}
