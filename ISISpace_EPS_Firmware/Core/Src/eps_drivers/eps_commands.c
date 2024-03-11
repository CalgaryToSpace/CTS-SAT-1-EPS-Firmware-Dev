
#include "main.h"

#include "debug_tools/debug_uart.h"
#include "debug_tools/debug_i2c.h"
#include "eps_drivers/eps_internal_drivers.h"
#include "stm_drivers/timing_helpers.h"


uint8_t eps_system_reset() {
	const uint8_t CC = 0xAA;
	const uint8_t arg_reset_key = 0xA6;

	const uint8_t cmd_len = 5;
	uint8_t cmd_buf[cmd_len];
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = arg_reset_key;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}

uint8_t eps_no_operation() {
	// FIXME: it appears that the no_operation command does not return it's own CC+1 in the RC field
	const uint8_t CC = 0x02;
	return eps_run_argumentless_cmd(CC);
}

uint8_t eps_cancel_oper() {
	const uint8_t CC = 0x04;
	return eps_run_argumentless_cmd(CC);
}

uint8_t eps_watchdog() {
	const uint8_t CC = 0x06;
	return eps_run_argumentless_cmd(CC);
}

uint8_t eps_output_bus_group_on(uint16_t CH_BF,  uint16_t CH_EXT_BF) {
	const uint8_t CC = 0x10;
	const uint8_t cmd_len = 8;
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = CH_BF & 0x00FF;
	cmd_buf[5] = CH_BF >> 8;
	cmd_buf[6] = CH_EXT_BF & 0x00FF;
	cmd_buf[7] = CH_EXT_BF >> 8;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}

uint8_t eps_output_bus_group_off(uint16_t CH_BF,  uint16_t CH_EXT_BF) {
	const uint8_t CC = 0x12;
	const uint8_t cmd_len = 8;
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = CH_BF & 0x00FF;
	cmd_buf[5] = CH_BF >> 8;
	cmd_buf[6] = CH_EXT_BF & 0x00FF;
	cmd_buf[7] = CH_EXT_BF >> 8;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}

uint8_t eps_output_bus_group_state(uint16_t CH_BF,  uint16_t CH_EXT_BF) {
	const uint8_t CC = 0x14;
	const uint8_t cmd_len = 8;
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = CH_BF & 0x00FF;
	cmd_buf[5] = CH_BF >> 8;
	cmd_buf[6] = CH_EXT_BF & 0x00FF;
	cmd_buf[7] = CH_EXT_BF >> 8;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}

uint8_t eps_output_bus_channel_on(uint8_t CH_IDX) {
	const uint8_t CC = 0x16;
	const uint8_t cmd_len = 5;
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = CH_IDX;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}

uint8_t eps_output_bus_channel_off(uint8_t CH_IDX) {
	const uint8_t CC = 0x18;
	const uint8_t cmd_len = 5;
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = CH_IDX;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}

uint8_t eps_switch_to_nominal_mode() {
	const uint8_t CC = 0x30;
	return eps_run_argumentless_cmd(CC);
}

uint8_t eps_switch_to_safety_mode() {
	const uint8_t CC = 0x32;
	return eps_run_argumentless_cmd(CC);
}

uint8_t eps_get_system_status(eps_result_system_status_t* result_dest) {
	const uint8_t CC = 0x40;
	const uint8_t cmd_len = 4;
	const uint8_t rx_len = 36;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_system_status(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_pdu_overcurrent_fault_state(eps_result_pdu_overcurrent_fault_state_t* result_dest) {
	const uint8_t CC = 0x42;
	const uint8_t cmd_len = 4;
	const uint8_t rx_len = 78;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_pdu_overcurrent_fault_state(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_pbu_abf_placed_state(eps_result_pbu_abf_placed_state_t* result_dest) {
	const uint8_t CC = 0x44;
	const uint8_t cmd_len = 4;
	const uint8_t rx_len = 8;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	// check that the response bytes are valid
	if (rx_buf[6] != EPS_ABF_PIN_APPLIED && rx_buf[6] != EPS_ABF_PIN_NOT_APPLIED) {
		return 20;
	}
	if (rx_buf[7] != EPS_ABF_PIN_APPLIED && rx_buf[7] != EPS_ABF_PIN_NOT_APPLIED) {
		return 21;
	}

	result_dest->abf_placed_0 = (EPS_ABF_PIN_PLACED_enum_t) (rx_buf[6]);
	result_dest->abf_placed_1 = (EPS_ABF_PIN_PLACED_enum_t) (rx_buf[7]);
	return 0;
}

uint8_t eps_get_pdu_housekeeping_data_raw(eps_result_pdu_housekeeping_data_raw_t* result_dest) {
	const uint8_t CC = 0x50;
	const uint8_t cmd_len = 4;
	const uint16_t rx_len = 258;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_pdu_housekeeping_data_raw(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_pdu_housekeeping_data_eng(eps_result_pdu_housekeeping_data_eng_t* result_dest) {
	const uint8_t CC = 0x52;
	const uint8_t cmd_len = 4;
	const uint16_t rx_len = 258;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_pdu_housekeeping_data_eng(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_pdu_housekeeping_data_running_average(eps_result_pdu_housekeeping_data_eng_t* result_dest) {
	const uint8_t CC = 0x54;
	const uint8_t cmd_len = 4;
	const uint16_t rx_len = 258;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_pdu_housekeeping_data_eng(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_pbu_housekeeping_data_raw(eps_result_pbu_housekeeping_data_raw_t* result_dest) {
	const uint8_t CC = 0x60;
	const uint8_t cmd_len = 4;
	const uint8_t rx_len = 84;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_pbu_housekeeping_data_raw(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_pbu_housekeeping_data_eng(eps_result_pbu_housekeeping_data_eng_t* result_dest) {
	const uint8_t CC = 0x62;
	const uint8_t cmd_len = 4;
	const uint8_t rx_len = 84;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_pbu_housekeeping_data_eng(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_pbu_housekeeping_data_running_average(eps_result_pbu_housekeeping_data_eng_t* result_dest) {
	const uint8_t CC = 0x64;
	const uint8_t cmd_len = 4;
	const uint8_t rx_len = 84;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}
	
	pack_eps_result_pbu_housekeeping_data_eng(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_pcu_housekeeping_data_raw(eps_result_pcu_housekeeping_data_raw_t* result_dest) {
	const uint8_t CC = 0x70;
	const uint8_t cmd_len = 4;
	const uint8_t rx_len = 72;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_pcu_housekeeping_data_raw(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_pcu_housekeeping_data_eng(eps_result_pcu_housekeeping_data_eng_t* result_dest) {
	const uint8_t CC = 0x72;
	const uint8_t cmd_len = 4;
	const uint8_t rx_len = 72;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_pcu_housekeeping_data_eng(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_pcu_housekeeping_data_running_average(eps_result_pcu_housekeeping_data_eng_t* result_dest) {
	const uint8_t CC = 0x74;
	const uint8_t cmd_len = 4;
	const uint8_t rx_len = 72;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_pcu_housekeeping_data_eng(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_configuration_parameter(uint16_t parameter_id, uint8_t parameter_value_dest[]) { 
	// parameter_value_dest must be 8 bytes

	const uint8_t CC = 0x82;
	const uint8_t cmd_len = 6;
	const uint8_t rx_len = 16;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	
	cmd_buf[4] = parameter_id & 0x00FF;
	cmd_buf[5] = parameter_id >> 8;

	// TODO: confirm if infinite reading allowed, or if we need to set rx_len based on the required parameter

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	// TODO: check that the parameter value that came back is the right value

	// const uint32_t parameter_value = rx_buf[5] | (rx_buf[6] << 8) | (rx_buf[7] << 16) | (rx_buf[8] << 24);
	
	// copy it to the result
	for (uint8_t idx = 0; idx < 8; idx++) {
		parameter_value_dest[idx] = rx_buf[idx];
	}

	return 0;
}


uint8_t eps_set_configuration_parameter(uint16_t parameter_id, uint8_t new_parameter_value) {
	const uint8_t CC = 0x84;
	const uint8_t cmd_len = 14;
	const uint8_t rx_len = 16;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	
	cmd_buf[4] = parameter_id & 0x00FF;
	cmd_buf[5] = parameter_id >> 8;
	cmd_buf[6] = new_parameter_value; // TODO: fix the width here; big task
	return 100;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	// TODO: check that the set was successful, based on the response

	return 0;
}

uint8_t eps_reset_configuration_parameter(uint16_t parameter_id) {
	const uint8_t CC = 0x86;
	const uint8_t cmd_len = 6;
	const uint8_t rx_len = 16;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	
	cmd_buf[4] = parameter_id & 0x00FF;
	cmd_buf[5] = parameter_id >> 8;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	// TODO: check that the reset was successful, based on the response (compare to parameter_id)

	return 0;
}

uint8_t eps_reset_configuration() {
	const uint8_t CC = 0x90;
	const uint8_t arg_conf_key = 0x87;
	const uint8_t cmd_len = 5;
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	cmd_buf[4] = arg_conf_key;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}

uint8_t eps_load_configuration() {
	const uint8_t CC = 0x92;
	const uint8_t arg_conf_key = 0xA7;
	const uint8_t cmd_len = 5;
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	cmd_buf[4] = arg_conf_key;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}

uint8_t eps_save_configuration() {
	const uint8_t CC = 0x94;
	const uint8_t arg_conf_key = 0xA7;
	const uint16_t CHECKSUM = 0; // FIXME: implement
	const uint8_t cmd_len = 7;
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	cmd_buf[4] = arg_conf_key;
	cmd_buf[5] = CHECKSUM;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}

uint8_t eps_get_piu_housekeeping_data_raw(eps_result_piu_housekeeping_data_raw_t* result_dest) {
	const uint8_t CC = 0xA0;
	const uint8_t cmd_len = 4;
	const uint16_t rx_len = 274;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_piu_housekeeping_data_raw(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_piu_housekeeping_data_eng(eps_result_piu_housekeeping_data_eng_t* result_dest) {
	const uint8_t CC = 0xA2;
	const uint8_t cmd_len = 4;
	const uint16_t rx_len = 274;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}
	
	pack_eps_result_piu_housekeeping_data_eng(rx_buf, result_dest);
	return 0;
}

uint8_t eps_get_piu_housekeeping_data_running_average(eps_result_piu_housekeeping_data_eng_t* result_dest) {
	const uint8_t CC = 0xA4;
	const uint8_t cmd_len = 4;
	const uint16_t rx_len = 274;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	if (comms_err != 0) {
		return comms_err;
	}

	pack_eps_result_piu_housekeeping_data_eng(rx_buf, result_dest);
	return 0;
}

uint8_t eps_correct_time(int32_t time_correction) {
	// Time correction in unix time (positive numbers added to time, negative values subtracted)
	
	const uint8_t CC = 0xC4;
	const uint8_t cmd_len = 8;
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	// TODO: check the byte order and storage type of the signed numbers
	cmd_buf[4] = time_correction & 0xFF;
	cmd_buf[5] = (time_correction >> 8) & 0xFF;
	cmd_buf[6] = (time_correction >> 16) & 0xFF;
	cmd_buf[7] = (time_correction >> 24) & 0xFF;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}

uint8_t eps_zero_reset_cause_counters() {
	const uint8_t CC = 0xC6;
	const uint8_t arg_zero_key = 0xA7;
	const uint8_t cmd_len = 5;
	const uint8_t rx_len = EPS_DEFAULT_RX_LEN_MIN;

	uint8_t cmd_buf[cmd_len];
	uint8_t rx_buf[rx_len];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	
	cmd_buf[4] = arg_zero_key;

	const uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, cmd_len, rx_buf, rx_len);
	return comms_err;
}
