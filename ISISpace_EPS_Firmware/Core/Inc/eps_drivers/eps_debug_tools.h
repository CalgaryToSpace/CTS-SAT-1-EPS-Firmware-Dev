
#ifndef __INCLUDE_GUARD__EPS_DEBUG_TOOLS_H__
#define __INCLUDE_GUARD__EPS_DEBUG_TOOLS_H__

#include "eps_drivers/eps_types.h"

uint8_t eps_debug_get_and_print_channel_stats(EPS_CHANNEL_enum_t eps_channel);

void eps_debug_uart_print_system_status(eps_result_system_status_t* system_status);

#endif /* __INCLUDE_GUARD__EPS_DEBUG_TOOLS_H__ */
