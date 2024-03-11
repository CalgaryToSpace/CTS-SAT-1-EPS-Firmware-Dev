
#include "debug_tools/debug_uart.h"
#include "eps_drivers/eps_debug_tools.h"
#include "eps_drivers/eps_commands.h"

uint8_t eps_debug_get_and_print_channel_stats(EPS_CHANNEL_enum_t eps_channel) {

	debug_uart_print_str("NOT IMPLEMENTED YET\n");
    return 0;

    /*
	const uint8_t comms_err = eps_get_pdu_housekeeping_data_eng(EPS_data_received);
	if (comms_err != 0) {
		return comms_err;
	}

	if (eps_channel == 0) {
		ch_vip_eng.voltage_mV = EPS_data_received->VIP_CH00.vipd_array[0]; //Voltage measurement in 2 bytes
		ch_vip_eng.current_mA = EPS_data_received->VIP_CH00.vipd_array[2]; //Current measurement in 2 bytes
		ch_vip_eng.power_cW = EPS_data_received->VIP_CH00.vipd_array[4]; //Power measurement in 2 bytes
	}
	else if (eps_channel == 1) {
		ch_vip_eng.voltage_mV = EPS_data_received->VIP_CH01.vipd_array[0]; //Voltage measurement in 2 bytes
		ch_vip_eng.current_mA = EPS_data_received->VIP_CH01.vipd_array[2]; //Current measurement in 2 bytes
		ch_vip_eng.power_cW = EPS_data_received->VIP_CH01.vipd_array[4]; //Power measurement in 2 bytes
	}
	else {
		// FIXME: Placeholder. Implement the result in a refactor.
		ch_vip_eng.voltage_mV = ch_vip_eng.current_mA = ch_vip_eng.power_cW = 42;
	}

	char msg1[250];
	sprintf(
		msg1,
		"Channel %d: V=%d mV, I=%d mA, P=%d cW\n",
		eps_channel,
		ch_vip_eng.voltage_mV,
		ch_vip_eng.current_mA,
		ch_vip_eng.power_cW
	);
	debug_uart_print_str(msg1);

	return 0;
    */
}

void eps_debug_uart_print_system_status(eps_result_system_status_t* system_status) {
	char msg1[365];
	sprintf(
	    msg1,
	    "Mode: %d, Configuration Changed?: %d, Reset cause: %d, Uptime: %lu sec, Error Code: %d, rst_cnt_pwron: %u, rst_cnt_wdg: %d, rst_cnt_cmd: %d, rst_cnt_mcu: %d, rst_cnt_emlopo: %d, time_since_prev_cmd: %d sec, Unix time: %lu sec, Unix year: %d, Unix month: %d, Unix day: %d, Unix hour: %d, Unix minute: %d, Unix second: %d\n",
	    system_status->mode, system_status->config_changed_since_boot, system_status->reset_cause,
        system_status->uptime_sec, system_status->error_code,
        system_status->rst_cnt_pwron, system_status->rst_cnt_wdg, system_status->rst_cnt_cmd,
        system_status->rst_cnt_mcu, system_status->rst_cnt_emlopo,
        
        system_status->time_since_prev_cmd_sec, system_status->unix_time_sec,
        (system_status->calendar_years_since_2000 + 2000), system_status->calendar_month, system_status->calendar_day,
        system_status->calendar_hour, system_status->calendar_minute, system_status->calendar_second
	);

	debug_uart_print_str(msg1);
}
