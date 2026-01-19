#ifndef HAL_UART_H
#define HAL_UART_H

#include "os_types.h"

void hal_uart_init(os_ubase_t base, uint32_t baud);
void hal_uart_putc(char c);
char hal_uart_getc(void);
int hal_uart_poll(void);
void hal_uart_puts(const char *s);

void os_print(const char *fmt, ...);

#endif
