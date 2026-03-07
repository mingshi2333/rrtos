OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    FLASH  (rx)  : ORIGIN = 0xA0000000, LENGTH = 256K
    TCMA   (rwx) : ORIGIN = 0x40010000, LENGTH = 64K
    TCMB   (rwx) : ORIGIN = 0x40020000, LENGTH = 96K
    SRAM   (rwx) : ORIGIN = 0x70000000, LENGTH = 32K
}

__stack_size = 4K;

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        _text_start = .;

        KEEP(*(.text.init))
        KEEP(*(.text.init.*))

        . = ALIGN(256);
        KEEP(*(.text.vectors))

        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)

        . = ALIGN(4);
        _text_end = .;
    } > FLASH

    .data : AT(_text_end)
    {
        . = ALIGN(4);
        _data_start = .;
        _data_lma = LOADADDR(.data);

        __global_pointer$ = . + 0x800;

        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)

        . = ALIGN(4);
        _data_end = .;
    } > TCMB

    .got :
    {
        *(.got)
        *(.got.plt)
    } > TCMB

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _bss_start = .;

        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)

        . = ALIGN(4);
        _bss_end = .;
    } > TCMB

    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        _heap_start = .;
        . = ORIGIN(TCMB) + LENGTH(TCMB) - __stack_size;
        _heap_end = .;
    } > TCMB

    .stack (NOLOAD) :
    {
        . = ALIGN(16);
        _stack_bottom = .;
        . += __stack_size;
        . = ALIGN(16);
        _stack_top = .;
    } > TCMB

    .shared (NOLOAD) :
    {
        . = ALIGN(4);
        _shared_start = .;
        *(.shared)
        *(.shared.*)
        . = ALIGN(4);
        _shared_end = .;
    } > SRAM

    /DISCARD/ :
    {
        *(.eh_frame)
        *(.eh_frame_hdr)
        *(.comment)
        *(.note.*)
    }
}

PROVIDE(_start = ORIGIN(FLASH));
PROVIDE(_end = _bss_end);
