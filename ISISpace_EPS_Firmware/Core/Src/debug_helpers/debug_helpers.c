#include "main.h"
#include "debug_helpers.h"

#include <stdint.h>
#include <string.h>

void debug_uart_print_str(const char* str) {
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)str, strlen(str), 1000);
}

void debug_uart_print_array_hex(const uint8_t* arr, uint16_t len, const char* end_str) {
	char msg[10];
	for (uint16_t i = 0; i < len; i++) {
		sprintf(msg, "0x%02x ", arr[i]);
		debug_uart_print_str(msg);
	}

	debug_uart_print_str(end_str);
}


void debug_i2c_scan() {
	// Go through all I2C addresses, and print in hex and active ones
	debug_uart_print_str("Doing I2C scan...\n");

	char msg[10];
	for (uint8_t i = 0; i < 128; i++) {
		if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 3, 5) == HAL_OK) {
			sprintf(msg, "0x%02x ", i);
			debug_uart_print_str(msg);
		}
		else {
			debug_uart_print_str("---- ");
		}

		if (i > 0 && (i + 1) % 16 == 0) {
			debug_uart_print_str("\n");
		}
	}
	debug_uart_print_str("\n");
}


