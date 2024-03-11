#include "main.h"

#include <stdint.h>

void debug_uart_print_str(const char *str);
void debug_uart_print_array_hex(
		const uint8_t* arr, uint16_t len, const char* end_str);

void debug_i2c_scan();
