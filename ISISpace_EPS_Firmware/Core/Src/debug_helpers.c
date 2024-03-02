#include "main.h"
#include "debug_helpers.h"

#include <stdint.h>
#include <string.h>

void debug_uart_print_str(const char *str) {
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)str, strlen(str), 1000);
}


void debug_i2c_scan() {
	char msg[5];
	// Go through all possible i2c addresses
	for (uint8_t i = 0; i < 128; i++) {

		if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 3, 5) == HAL_OK) {
			// We got an ack
			sprintf(msg, "%2x ", i);
			debug_uart_print_str(msg);
		} else {
			debug_uart_print_str("-- ");
		}

	if (i > 0 && (i + 1) % 16 == 0) debug_uart_print_str("\n");

	}

	debug_uart_print_str("\n");
}


