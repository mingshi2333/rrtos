#include "hal_uart.h"
#include <stdarg.h>

#define UART_THR    0x00
#define UART_RBR    0x00
#define UART_IER    0x01
#define UART_FCR    0x02
#define UART_LCR    0x03
#define UART_MCR    0x04
#define UART_LSR    0x05

#define UART_LSR_DR     0x01
#define UART_LSR_THRE   0x20

static volatile uint8_t *g_uart_base;

void hal_uart_init(os_ubase_t base, uint32_t baud) {
    (void)baud;
    g_uart_base = (volatile uint8_t *)base;
}

void hal_uart_putc(char c) {
    while (!(g_uart_base[UART_LSR] & UART_LSR_THRE)) {
    }
    g_uart_base[UART_THR] = c;
}

char hal_uart_getc(void) {
    while (!(g_uart_base[UART_LSR] & UART_LSR_DR)) {
    }
    return g_uart_base[UART_RBR];
}

int hal_uart_poll(void) {
    return (g_uart_base[UART_LSR] & UART_LSR_DR) ? 1 : 0;
}

void hal_uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') {
            hal_uart_putc('\r');
        }
        hal_uart_putc(*s++);
    }
}

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
