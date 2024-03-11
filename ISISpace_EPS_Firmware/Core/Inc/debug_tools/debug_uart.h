// debug_uart.h
// Functions for debug printing to the Debug UART interface.

#ifndef __INCLUDE_GUARD__DEBUG_UART_H__
#define __INCLUDE_GUARD__DEBUG_UART_H__

#include "main.h"
#include <stdint.h>

void debug_uart_print_str(const char *str);

void debug_uart_print_array_hex(
		const uint8_t* arr, uint16_t len, const char* end_str);



#endif // __INCLUDE_GUARD__DEBUG_UART_H__
