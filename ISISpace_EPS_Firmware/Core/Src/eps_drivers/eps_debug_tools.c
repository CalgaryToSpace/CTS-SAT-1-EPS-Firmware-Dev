
#include "eps_drivers/eps_debug_tools.h"
#include "eps_drivers/eps_driver.h"

uint8_t eps_debug_get_and_print_channel_stats(EPS_CHANNEL_enum_t eps_channel) {
	PDU_HK_D* EPS_data_received;

	VIPD_eng_t ch_vip_eng;

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
}

void eps_debug_uart_print_system_status(eps_result_system_status_t* system_status) {
	char msg1[365];
	sprintf(
	    msg1,
	    "Mode: %d, Configuration: %d, Reset cause: %d, Uptime: %lu sec, Error: %d, rst_cnt_pwron: %u, rst_cnt_wdg: %d, rst_cnt_cmd: %d, rst_cnt_mcu: %d, rst_cnt_emlopo: %d, Prevcmd elapsed: %d, Unix time: %lu, Unix year: %d, Unix month: %d, Unix day: %d, Unix hour: %d, Unix minute: %d, Unix second: %d\n",
	    system_status->mode, system_status->conf, system_status->reset_cause, system_status->uptime_sec, system_status->error, system_status->rst_cnt_pwron, system_status->rst_cnt_wdg, system_status->rst_cnt_cmd, system_status->rst_cnt_mcu, system_status->rst_cnt_emlopo, system_status->prevcmd_elapsed, system_status->unix_time, system_status->unix_year, system_status->unix_month, system_status->unix_day, system_status->unix_hour, system_status->unix_minute, system_status->unix_second
	);

	debug_uart_print_str(msg1);
}

