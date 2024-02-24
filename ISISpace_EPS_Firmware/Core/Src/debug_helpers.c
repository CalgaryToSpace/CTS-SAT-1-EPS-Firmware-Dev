#include "main.h"
#include "debug_helpers.h"

#include <stdint.h>

void debug_uart_print_str(const char *str) {
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)str, strlen(str), 1000);
}
