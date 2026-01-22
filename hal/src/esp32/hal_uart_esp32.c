#include "hal_uart.h"
#include <stdint.h>
#include <stdarg.h>

/* Using NS16550 for Renode Simulation Compatibility */
#define UART0_BASE          0x60000000
#define UART_RBR_OFFSET     0x00
#define UART_THR_OFFSET     0x00
#define UART_LSR_OFFSET     0x14  /* 0x05 << 2 */

#define UART_LSR_THRE       0x20
#define UART_LSR_DR         0x01

#define REG(base, off)      (*(volatile uint32_t *)((base) + (off)))

/* Status Register Bits */
#define UART_TXFIFO_CNT_MASK 0x00FF0000
#define UART_TXFIFO_CNT_SHIFT 16
#define UART_RXFIFO_CNT_MASK 0x000000FF
#define UART_RXFIFO_CNT_SHIFT 0

static volatile uint32_t g_uart_base = UART0_BASE;

void hal_uart_init(os_ubase_t base, uint32_t baud) {
    if (base != 0) {
        g_uart_base = base;
    }
    
    /* 
     * UART is typically initialized by the ROM bootloader on ESP32-C3.
     * We just need to ensure we can write to the FIFO.
     * Advanced configuration (baud rate, parity) would go here.
     */
}

void hal_uart_putc(char c) {
    /* Wait for THRE (Transmitter Holding Register Empty) */
    while ((REG(g_uart_base, UART_LSR_OFFSET) & UART_LSR_THRE) == 0) {
        /* Busy wait */
    }
    REG(g_uart_base, UART_THR_OFFSET) = c;
}

char hal_uart_getc(void) {
    /* Wait for DR (Data Ready) */
    while ((REG(g_uart_base, UART_LSR_OFFSET) & UART_LSR_DR) == 0) {
        /* Busy wait */
    }
    return (char)(REG(g_uart_base, UART_RBR_OFFSET) & 0xFF);
}

int hal_uart_poll(void) {
    return (REG(g_uart_base, UART_LSR_OFFSET) & UART_LSR_DR) != 0;
}

void hal_uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') {
            hal_uart_putc('\r');
        }
        hal_uart_putc(*s++);
    }
}

/* Re-use generic os_print from hal_uart.c or reimplement if needed. 
 * Since hal_uart.c is not compiled for ESP32 (due to our CMake logic),
 * we need to copy os_print here or move it to a common file.
 * For simplicity, we reimplement minimal print helpers here.
 */

static void print_dec(uint32_t val) {
    char buf[12];
    int i = 0;
    if (val == 0) {
        hal_uart_putc('0');
        return;
    }
    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    while (--i >= 0) {
        hal_uart_putc(buf[i]);
    }
}

static void print_hex(uint32_t val) {
    const char hex[] = "0123456789abcdef";
    hal_uart_putc('0');
    hal_uart_putc('x');
    for (int i = 28; i >= 0; i -= 4) {
        hal_uart_putc(hex[(val >> i) & 0xF]);
    }
}

void os_print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd':
                case 'u':
                    print_dec(va_arg(args, uint32_t));
                    break;
                case 'x':
                case 'X':
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
            if (*fmt == '\n') {
                hal_uart_putc('\r');
            }
            hal_uart_putc(*fmt);
        }
        fmt++;
    }
    
    va_end(args);
}
