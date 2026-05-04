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

__hart_stack_size = 4K;
__hart_stack_count = 2;
__stack_size = __hart_stack_size * __hart_stack_count;

SECTIONS
{
    .text :
    {
        /*
         * BE-U1000 BootROM image header.
         *
         * The ROM parses this block before running an eFlash image.  The first
         * word is also a real instruction so a direct branch to eFlash skips the
         * header and lands on the reset vector.
         */
        . = ORIGIN(FLASH);
        LONG(__HEADER_JUMP_OVER__)
        . = ORIGIN(FLASH) + __HEADER_OFFSET_MAGIC_NUM__;
        LONG(__HEADER_MAGIC_NUM__)
        . = ORIGIN(FLASH) + __HEADER_OFFSET_COPY_ADDR__;
        LONG(0x00)
        . = ORIGIN(FLASH) + __HEADER_OFFSET_COPY_SIZE__;
        LONG(__HEADER_COPY_SIZE__)
        . = ORIGIN(FLASH) + __HEADER_SIZE__;

        . = ALIGN(256);
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
        *(.got)
        *(.got.plt)

        . = ALIGN(4);
        _data_end = .;
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
        PROVIDE(__heap_start = _heap_start);
        PROVIDE(__heap_end = _heap_end);
    } > TCMB

    .stack (NOLOAD) :
    {
        . = ALIGN(16);
        _stack_bottom = .;
        __be_u1000_stack_bottom = .;
        __hart1_stack_bottom = .;
        . += __hart_stack_size;
        . = ALIGN(16);
        __hart1_stack_top = .;
        __hart0_stack_bottom = .;
        . += __hart_stack_size;
        . = ALIGN(16);
        _stack_top = .;
        __hart0_stack_top = .;
        __be_u1000_stack_top = .;
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

/* Variables for the BE-U1000 BootROM image header. */
__HEADER_OFFSET_MAGIC_NUM__ = 0x20;
__HEADER_OFFSET_COPY_ADDR__ = 0x28;
__HEADER_OFFSET_COPY_SIZE__ = 0x2C;
__HEADER_SIZE__             = 0x30;

__HEADER_ENTRY_OFFSET__     = _text_start - ORIGIN(FLASH);
__HEADER_JUMP_OVER__        = (((__HEADER_ENTRY_OFFSET__ & 0x100000) << 11)
                              | ((__HEADER_ENTRY_OFFSET__ & 0x0007FE) << 20)
                              | ((__HEADER_ENTRY_OFFSET__ & 0x000800) << 9)
                              |  (__HEADER_ENTRY_OFFSET__ & 0x0FF000)
                              | 0x0000006F);
__HEADER_MAGIC_NUM__        = 0x0BEF5262; /* SDK EFLASH_MAGIC_NUMBER */
__HEADER_COPY_SIZE__        = LOADADDR(.data) + SIZEOF(.data) - LOADADDR(.text);

PROVIDE(_start = ORIGIN(FLASH));
PROVIDE(_end = _bss_end);
