/* Linker Script for ESP32-C3 (RISC-V) */

OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    /* IROM (Instruction ROM) - Mapped to Flash */
    irom (rx) : ORIGIN = 0x42000000, LENGTH = 4M
    
    /* DROM (Data ROM) - Mapped to Flash */
    drom (r)  : ORIGIN = 0x3C000000, LENGTH = 4M
    
    /* IRAM (Instruction RAM) - Internal SRAM 1 */
    iram (rwx) : ORIGIN = 0x40380000, LENGTH = 192K
    
    /* DRAM (Data RAM) - Internal SRAM 1 */
    dram (rwx) : ORIGIN = 0x3FC80000, LENGTH = 320K
}

SECTIONS
{
    /* Code in IROM (Flash) */
    .text : AT(0x42000000)
    {
        . = ALIGN(4);
        KEEP(*(.init))
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.eh_frame)
    } > irom

    /* Data in DRAM (LMA in IROM) */
    /* AT > irom tells linker to place LMA in irom, following .text */
    .data :
    {
        . = ALIGN(4);
        __global_pointer$ = . + 0x800;
        *(.sdata .sdata.*)
        *(.data .data.*)
        *(.got .got.*)
        *(.srodata .srodata.*)
        . = ALIGN(4);
    } > dram AT > irom

    /* BSS in DRAM */
    .bss :
    {
        . = ALIGN(4);
        __bss_start = .;
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    } > dram

    /* Stack */
    .stack (NOLOAD) :
    {
        . = ALIGN(16);
        . += 0x2000; /* 8KB Stack */
        _stack_top = .;
        __stack_top = .; /* Alias */
    } > dram

    /* Heap */
    .heap (NOLOAD) :
    {
        . = ALIGN(4);
        _heap_start = .;
        __heap_start = .;
        . = ORIGIN(dram) + LENGTH(dram);
        _heap_end = .;
        __heap_end = .;
    } > dram

    /* Legacy Symbol Aliases for startup.S */
    _bss_start = __bss_start;
    _bss_end = __bss_end;
    _data_start = ADDR(.data);
    _data_end = ADDR(.data) + SIZEOF(.data);
    _data_lma = LOADADDR(.data);
}
