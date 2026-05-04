/**
 * @file hal_uart_be_u1000.c
 * @brief UART driver for BE-U1000 (DesignWare APB UART)
 *
 * The BE-U1000 uses a Synopsys DesignWare APB UART which is 16550-compatible
 * but with WORD-ALIGNED register access (32-bit at 4-byte offsets) instead
 * of the byte-addressed 16550 used in QEMU virt.
 *
 * Register map (word-addressed):
 *   THR/RBR  = base + 0x00  (Transmit/Receive buffer)
 *   IER      = base + 0x04  (Interrupt Enable)
 *   IIR/FCR  = base + 0x08  (Interrupt ID / FIFO Control)
 *   LCR      = base + 0x0C  (Line Control)
 *   MCR      = base + 0x10  (Modem Control)
 *   LSR      = base + 0x14  (Line Status)
 *   MSR      = base + 0x18  (Modem Status)
 *   SCR      = base + 0x1C  (Scratch)
 *   USR      = base + 0x7C  (UART Status)
 *   DLF      = base + 0xC0  (Divisor Latch Fraction)
 */

#include "hal_uart.h"
#include <stdarg.h>

/* DesignWare UART register offsets (word-addressed) */
#define DW_UART_RBR     0x00    /* Receive Buffer Register (read) */
#define DW_UART_THR     0x00    /* Transmit Holding Register (write) */
#define DW_UART_DLL     0x00    /* Divisor Latch Low (when DLAB=1) */
#define DW_UART_IER     0x04    /* Interrupt Enable Register */
#define DW_UART_DLH     0x04    /* Divisor Latch High (when DLAB=1) */
#define DW_UART_IIR     0x08    /* Interrupt Identity Register (read) */
#define DW_UART_FCR     0x08    /* FIFO Control Register (write) */
#define DW_UART_LCR     0x0C    /* Line Control Register */
#define DW_UART_MCR     0x10    /* Modem Control Register */
#define DW_UART_LSR     0x14    /* Line Status Register */
#define DW_UART_MSR     0x18    /* Modem Status Register */
#define DW_UART_SCR     0x1C    /* Scratch Register */
#define DW_UART_USR     0x7C    /* UART Status Register */
#define DW_UART_DLF     0xC0    /* Divisor Latch Fraction Register */

/* LSR bit definitions */
#define LSR_DR          0x01    /* Data Ready */
#define LSR_THRE        0x20    /* Transmit Holding Register Empty */
#define LSR_TEMT        0x40    /* Transmitter Empty */

/* LCR bit definitions */
#define LCR_WLS_8       0x03    /* 8-bit word length */
#define LCR_STB_1       0x00    /* 1 stop bit */
#define LCR_DLAB        0x80    /* Divisor Latch Access Bit */

/* FCR bit definitions */
#define FCR_FIFO_EN     0x01    /* FIFO Enable */
#define FCR_RCVR_RST    0x02    /* Receiver FIFO Reset */
#define FCR_XMIT_RST    0x04    /* Transmitter FIFO Reset */

/* USR bit definitions */
#define USR_BUSY        0x01    /* UART Busy */
#define USR_TFNF        0x02    /* Transmit FIFO Not Full */
#define USR_RFNE        0x08    /* Receive FIFO Not Empty */

#ifndef BE_U1000_UART_INPUT_CLK_HZ
#define BE_U1000_UART_INPUT_CLK_HZ 25000000UL
#endif

/* UART base address (set during init) */
static volatile uint32_t *g_uart_base = (void *)0;

#if OS_CFG_SMP_EN
static volatile int32_t g_uart_print_lock;

static os_reg_t uart_irq_save(void)
{
    os_reg_t flags;
    __asm__ volatile("csrrc %0, mstatus, %1" : "=r"(flags) : "r"(8UL));
    return flags;
}

static void uart_irq_restore(os_reg_t flags)
{
    __asm__ volatile("csrw mstatus, %0" :: "r"(flags));
}

static os_reg_t uart_print_lock(void)
{
    os_reg_t flags = uart_irq_save();

    while (1) {
        while (g_uart_print_lock != 0) {
            __asm__ volatile("nop");
        }

        int32_t tmp;
        int32_t ret;
        __asm__ volatile(
            "li %1, 1\n"
            "1: lr.w %0, (%2)\n"
            "   bnez %0, 1b\n"
            "   sc.w %0, %1, (%2)\n"
            "   bnez %0, 1b\n"
            "   fence rw, rw\n"
            : "=&r"(ret), "=&r"(tmp)
            : "r"(&g_uart_print_lock)
            : "memory");
        if (ret == 0) {
            break;
        }
    }

    return flags;
}

static void uart_print_unlock(os_reg_t flags)
{
    __asm__ volatile("fence rw, w" ::: "memory");
    g_uart_print_lock = 0;
    uart_irq_restore(flags);
}
#else
static os_reg_t uart_print_lock(void) { return 0; }
static void uart_print_unlock(os_reg_t flags) { (void)flags; }
#endif

/* ============================================================================
 * Register access helpers (word-addressed)
 * ========================================================================= */

static inline uint32_t uart_read(uint32_t offset)
{
    return *(volatile uint32_t *)((uintptr_t)g_uart_base + offset);
}

static inline void uart_write(uint32_t offset, uint32_t value)
{
    *(volatile uint32_t *)((uintptr_t)g_uart_base + offset) = value;
}

/* ============================================================================
 * HAL UART API Implementation
 * ========================================================================= */

void hal_uart_init(os_ubase_t base, uint32_t baud)
{
    g_uart_base = (volatile uint32_t *)base;

    /* Disable interrupts */
    uart_write(DW_UART_IER, 0x00);

    /*
     * Match the BE-U1000 SDK divisor scheme for this DesignWare UART:
     * div is a 16x fixed-point divisor, with the low 4 bits written to DLF.
     * EVU-BA normally boots from external CLKI, which is 25 MHz.
     */
    if (baud == 0) {
        baud = 115200;
    }

    uint32_t divisor = BE_U1000_UART_INPUT_CLK_HZ / baud;
    if (divisor < 16) {
        divisor = 16;
    }
    uint32_t divisor_int = divisor >> 4;
    uint32_t divisor_fract = divisor & 0x0F;

    uart_write(DW_UART_DLF, divisor_fract);

    /* Enable DLAB to set the integer baud rate divisor */
    uart_write(DW_UART_LCR, LCR_DLAB);

    uart_write(DW_UART_DLL, divisor_int & 0xFF);
    uart_write(DW_UART_DLH, (divisor_int >> 8) & 0xFF);

    /* 8 data bits, 1 stop bit, no parity, disable DLAB */
    uart_write(DW_UART_LCR, LCR_WLS_8 | LCR_STB_1);

    /* Enable and reset FIFOs */
    uart_write(DW_UART_FCR, FCR_FIFO_EN | FCR_RCVR_RST | FCR_XMIT_RST);

    /* No modem control needed for console */
    uart_write(DW_UART_MCR, 0x00);
}

void hal_uart_putc(char c)
{
    /* Wait for Transmit Holding Register Empty */
    while (!(uart_read(DW_UART_LSR) & LSR_THRE))
        ;
    uart_write(DW_UART_THR, (uint32_t)c);
}

char hal_uart_getc(void)
{
    /* Wait for Data Ready */
    while (!(uart_read(DW_UART_LSR) & LSR_DR))
        ;
    return (char)(uart_read(DW_UART_RBR) & 0xFF);
}

int hal_uart_poll(void)
{
    return (uart_read(DW_UART_LSR) & LSR_DR) ? 1 : 0;
}

void hal_uart_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') {
            hal_uart_putc('\r');
        }
        hal_uart_putc(*s++);
    }
}

/* ============================================================================
 * os_print — minimal printf (same as generic, included for completeness)
 * ========================================================================= */

static void print_uint(uint32_t n)
{
    char buf[12];
    int i = 0;
    if (n == 0) { hal_uart_putc('0'); return; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (i > 0) { hal_uart_putc(buf[--i]); }
}

static void print_hex(uint32_t n)
{
    const char hex[] = "0123456789abcdef";
    for (int i = 28; i >= 0; i -= 4)
        hal_uart_putc(hex[(n >> i) & 0xF]);
}

void os_print(const char *fmt, ...)
{
    va_list args;
    os_reg_t flags = uart_print_lock();
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
            case 'd': {
                int32_t val = va_arg(args, int32_t);
                if (val < 0) { hal_uart_putc('-'); val = -val; }
                print_uint((uint32_t)val);
                break;
            }
            case 'u':
                print_uint(va_arg(args, uint32_t));
                break;
            case 'x':
                print_hex(va_arg(args, uint32_t));
                break;
            case 's':
                hal_uart_puts(va_arg(args, const char *));
                break;
            case 'c':
                hal_uart_putc((char)va_arg(args, int));
                break;
            case '%':
                hal_uart_putc('%');
                break;
            default:
                hal_uart_putc('%');
                hal_uart_putc(*fmt);
                break;
            }
        } else {
            if (*fmt == '\n') hal_uart_putc('\r');
            hal_uart_putc(*fmt);
        }
        fmt++;
    }

    va_end(args);
    uart_print_unlock(flags);
}
