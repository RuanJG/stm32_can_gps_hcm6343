#ifndef _SLOG_H
#define _SLOG_H

#include "stdio.h"
#define slog_printf(...) printf(__VA_ARGS__)
void slog_init(Uart_t *uart);
void slog(char *str, unsigned int num);
void slog_string(char *str);
void slog_num(unsigned int num);

#endif