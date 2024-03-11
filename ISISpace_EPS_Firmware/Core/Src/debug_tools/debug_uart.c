
#include "main.h"
#include "debug_tools/debug_i2c.h"
#include "debug_tools/debug_uart.h"

#include <stdint.h>
#include <string.h>

void debug_uart_print_str(const char *str) {
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)str, strlen(str), 100);
}

void debug_uart_print_array_hex(const uint8_t* arr, uint16_t len, const char* end_str) {
	char msg[10];
	for (uint16_t i = 0; i < len; i++) {
		sprintf(msg, "0x%02x ", arr[i]);
		debug_uart_print_str(msg);
	}

	debug_uart_print_str(end_str);
}
