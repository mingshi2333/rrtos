/**
 * Linker script for BE-U1000 RISC-V MCU — TCM execution mode
 *
 * Memory layout (Core0/Core1):
 *   EFLASH  : 0xA0000000, 256 KB (code storage, boot ROM)
 *   TCMA    : 0x40010000, 64 KB  (fast code/data, tightly coupled)
 *   TCMB    : 0x40020000, 96 KB  (heap + stack)
 *   SRAM    : 0x70000000, 32 KB  (shared data between cores)
 *
 * Execution model:
 *   Code copied from EFLASH -> TCMA at boot (XIP alternative available)
 *   Stack at top of TCMB, heap below stack
 */

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

/* Per-hart startup stack bank for Core0/Core1. */
__hart_stack_size = 4K;
__hart_stack_count = 2;
__stack_size = __hart_stack_size * __hart_stack_count;

SECTIONS
{
    /* ---------------------------------------------------------------
     * .text — Code section (runs from TCMA, stored in FLASH)
     * --------------------------------------------------------------- */
    .text : AT(ORIGIN(FLASH))
    {
        . = ALIGN(4);
        _text_start = .;

        /* Reset vector and init code first */
        KEEP(*(.text.init))
        KEEP(*(.text.init.*))

        /* CLIC vector table (must be 256-byte aligned) */
        . = ALIGN(256);
        KEEP(*(.text.vectors))

        /* Application code */
        *(.text)
        *(.text.*)

        /* Read-only data embedded in text section */
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)

        . = ALIGN(4);
        _text_end = .;
    } > TCMA

    /* ---------------------------------------------------------------
     * .data — Initialized data (runs from TCMA, loaded from FLASH)
     * --------------------------------------------------------------- */
    .data : AT(_text_end - ORIGIN(TCMA) + ORIGIN(FLASH))
    {
        . = ALIGN(4);
        _data_start = .;
        _data_lma = LOADADDR(.data);

        /* Global pointer (for efficient small-data access) */
        __global_pointer$ = . + 0x800;

        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
        *(.got)
        *(.got.plt)

        . = ALIGN(4);
        _data_end = .;
    } > TCMA

    /* ---------------------------------------------------------------
     * .bss — Uninitialized data (in TCMA, zeroed at startup)
     * --------------------------------------------------------------- */
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
    } > TCMA

    /* ---------------------------------------------------------------
     * Heap — in TCMB (below stack)
     * --------------------------------------------------------------- */
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        _heap_start = .;

        /* Heap grows upward from TCMB base */
        . = ORIGIN(TCMB) + LENGTH(TCMB) - __stack_size;
        _heap_end = .;
        PROVIDE(__heap_start = _heap_start);
        PROVIDE(__heap_end = _heap_end);
    } > TCMB

    /* ---------------------------------------------------------------
     * Stack bank — at top of TCMB, one explicit 4 KB stack per main hart.
     * Hart0 owns the upper stack; experimental SMP hart1 owns the lower stack.
     * --------------------------------------------------------------- */
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

    /* ---------------------------------------------------------------
     * Shared memory — in SRAM (for inter-core communication)
     * --------------------------------------------------------------- */
    .shared (NOLOAD) :
    {
        . = ALIGN(4);
        _shared_start = .;

        *(.shared)
        *(.shared.*)

        . = ALIGN(4);
        _shared_end = .;
    } > SRAM

    /* Discard unwanted sections */
    /DISCARD/ :
    {
        *(.eh_frame)
        *(.eh_frame_hdr)
        *(.comment)
        *(.note.*)
    }
}

/* Provide symbols for startup code */
PROVIDE(_start = ORIGIN(TCMA));
PROVIDE(_end = _bss_end);
