/*
 * eps_driver.c
 *
 *  Created on: Nov 14, 2023
 *      Author: frank
 */


#include "eps_driver.h"
#include "timing_helpers.h"


// Note: EPS_I2C_ADDR is left-shifted by 1 to be compatible with the HAL_I2C functions
const uint8_t EPS_I2C_ADDR = (0x20 << 1); // EPS I2C address

const uint8_t EPS_COMMAND_STID = 0x1A; // "System Type Identifier (STID)" (Software ICD, page 17)
const uint8_t EPS_COMMAND_IVID = 0x07; // "Interface Version Identifier (IVID)" (Software ICD, page 18)
const uint8_t EPS_COMMAND_BID = 0x01; // "Board Identifier (BID)" (Software ICD, page 20)

const uint8_t EPS_ENABLE_DEBUG_PRINT = 1; // 0 to disable

const uint32_t EPS_MAX_RESPONSE_POLL_TIME_MS = 100;

uint8_t eps_send_cmd_get_response(
		uint8_t* cmd_buf, uint8_t cmd_buf_len, uint8_t* rx_buf, uint8_t rx_buf_len) {

	// ASSERT: rx_buf_len must be >= 5 for all commands. Raise error if it's less.
	if (rx_buf_len < 5) return 1;

	if (EPS_ENABLE_DEBUG_PRINT) {
		debug_uart_print_str("OBC->EPS: ");
		debug_uart_print_array_hex(cmd_buf, cmd_buf_len, "\n");
	}

	HAL_StatusTypeDef tx_status = HAL_I2C_Master_Transmit(
			&hi2c1, EPS_I2C_ADDR, cmd_buf, cmd_buf_len, 1000);
	if (tx_status != HAL_OK) {
		if (EPS_ENABLE_DEBUG_PRINT) {
			char msg[200];
			sprintf(msg, "OBC->EPS ERROR: tx_status != HAL_OK (%d)\n", tx_status);
			debug_uart_print_str(msg);
		}
		return 2;
	}

	// poll for response
	// success when rx_buf[0] != 0xFF)
	uint32_t start_rx_time_ms = get_uptime_ms();
	uint8_t had_successful_rx = 0;
	uint16_t rx_retry_count = 0;
	while (get_uptime_ms() - start_rx_time_ms < EPS_MAX_RESPONSE_POLL_TIME_MS) {
		HAL_StatusTypeDef rx_status = HAL_I2C_Master_Receive(
				&hi2c1, EPS_I2C_ADDR, rx_buf, rx_buf_len, 50);
		if (rx_status != HAL_OK) {
			// this is a bad an unexpected error; return "there's a problem"
			// TODO: consider making this a retry case as well, as it happens randomly sometimes
			if (EPS_ENABLE_DEBUG_PRINT) {
				char msg[200];
				sprintf(msg, "OBC->EPS ERROR: rx_status != HAL_OK (%d)\n", rx_status);
				debug_uart_print_str(msg);
			}
			return 3;
		}

		if (rx_buf[0] == 0xFF) {
			// quintessential "not ready" response; try again
			delay_ms(5);
//			if (EPS_ENABLE_DEBUG_PRINT) { // commented out because we print the retry count
//				debug_uart_print_str("EPS->OBC: not ready, retrying...\n");
//			}
			rx_retry_count++;
			continue;
		}
		else {
			had_successful_rx = 1;
			if (EPS_ENABLE_DEBUG_PRINT) {
				char msg[100];
				sprintf(msg, "EPS->OBC: success after %d rx retries...\n", rx_retry_count);
				debug_uart_print_str(msg);
			}
			break;
		}
	}


	if (had_successful_rx == 0) {
		if (EPS_ENABLE_DEBUG_PRINT) {
			char msg[100];
			sprintf(msg, "EPS->OBC: failed rx after %d rx retries\n", rx_retry_count);
			debug_uart_print_str(msg);
		}
		return 4;
	}

	if (EPS_ENABLE_DEBUG_PRINT) {
		debug_uart_print_str("EPS->OBC: ");
		debug_uart_print_array_hex(rx_buf, rx_buf_len, "\n");
	}

	// Check STAT field (Table 3-11) - 0x00 and 0x80 mean success
	// TODO: consider doing this check in the next level up
	uint8_t eps_stat_field = rx_buf[4];
	if ((eps_stat_field != 0x00) && (eps_stat_field != 0x80)) {
		if (EPS_ENABLE_DEBUG_PRINT) {
			char msg[100];
			sprintf(msg,
					"EPS returned an error in the STAT field: 0x%02x (see ESP_SICD Table 3-11)\n",
					eps_stat_field);
			debug_uart_print_str(msg);
		}
	}

	return 0;
}


void eps_debug_get_and_print_channel_stats(EPS_CHANNEL_t eps_channel) {
	PDU_HK_D* EPS_data_received;

	VIPD_eng_t ch_vip_eng;

	eps_get_pdu_housekeeping_data_eng(EPS_data_received);

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
}

void eps_debug_uart_print_sys_stat(eps_result_sys_stat_t* sys_stat) {
	char msg1[365];
	sprintf(
	    msg1,
	    "Mode: %d, Configuration: %d, Reset cause: %d, Uptime: %lu sec, Error: %d, rst_cnt_pwron: %u, rst_cnt_wdg: %d, rst_cnt_cmd: %d, rst_cnt_mcu: %d, rst_cnt_emlopo: %d, Prevcmd elapsed: %d, Unix time: %lu, Unix year: %d, Unix month: %d, Unix day: %d, Unix hour: %d, Unix minute: %d, Unix second: %d\n",
	    sys_stat->mode, sys_stat->conf, sys_stat->reset_cause, sys_stat->uptime_sec, sys_stat->error, sys_stat->rst_cnt_pwron, sys_stat->rst_cnt_wdg, sys_stat->rst_cnt_cmd, sys_stat->rst_cnt_mcu, sys_stat->rst_cnt_emlopo, sys_stat->prevcmd_elapsed, sys_stat->unix_time, sys_stat->unix_year, sys_stat->unix_month, sys_stat->unix_day, sys_stat->unix_hour, sys_stat->unix_minute, sys_stat->unix_second
	);

	debug_uart_print_str(msg1);
}


//Driver functions

void eps_system_reset() {
	const uint8_t CC = 0xAA;
	uint8_t Reset_key = 0xA6;

	uint8_t cmd_buf[5];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = Reset_key;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 5, 1000);
}


uint8_t eps_no_operation() {
	// FIXME: it appears that the no_operation command does not return it's own CC+1 in the RC field
	const uint8_t CC = 0x02;
    uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	uint8_t rx_buf[5];
	uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, 4, rx_buf, 5);

	uint8_t status = rx_buf[4];

	return status;
}


uint8_t eps_cancel_oper() {
	const uint8_t CC = 0x04;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);


	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


void eps_watchdog() {
	const uint8_t CC = 0x06;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	uint8_t rx_buf[5];
	uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, 4, rx_buf, 5);
}


uint8_t eps_output_bus_group_on(uint16_t CH_BF,  uint16_t CH_EXT_BF) {
	const uint8_t CC = 0x10;
	uint8_t cmd_buf[8];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = CH_BF;
	cmd_buf[6] = CH_EXT_BF;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 8, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


uint8_t eps_output_bus_group_off(uint16_t CH_BF,  uint16_t CH_EXT_BF) {
	const uint8_t CC = 0x12;
	uint8_t cmd_buf[8];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = CH_BF;
	cmd_buf[6] = CH_EXT_BF;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 8, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


uint8_t eps_output_bus_group_state(uint16_t CH_BF,  uint16_t CH_EXT_BF) {
	const uint8_t CC = 0x14;
	uint8_t cmd_buf[8];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = CH_BF;
	cmd_buf[6] = CH_EXT_BF;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 8, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


uint8_t eps_output_bus_channel_on(uint8_t CH_IDX) {
	const uint8_t CC = 0x16;
	uint8_t cmd_buf[5];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = CH_IDX;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 5, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


uint8_t eps_output_bus_channel_off(uint8_t CH_IDX) {
	const uint8_t CC = 0x18;
	uint8_t cmd_buf[5];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;
	cmd_buf[4] = CH_IDX;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 5, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


void eps_switch_to_nominal_mode() {
	const uint8_t CC = 0x30;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);
}


void eps_switch_to_safety_mode() {
	const uint8_t CC = 0x32;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);
}


uint8_t eps_get_sys_status(eps_result_sys_stat_t* result_dest) {
	const uint8_t CC = 0x40;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	uint8_t rx_buf[36];
	uint8_t comms_err = eps_send_cmd_get_response(cmd_buf, 4, rx_buf, 36);

	if (comms_err != 0) {
		return comms_err;
	}

	result_dest->mode = rx_buf[5];
	result_dest->conf = rx_buf[6];
	result_dest->reset_cause = rx_buf[7];
	result_dest->uptime_sec = rx_buf[8] | (rx_buf[9]<<8) | (rx_buf[10]<<16) | (rx_buf[11]<<24);
	result_dest->error = rx_buf[12]; // FIXME: multi-byte values
	result_dest->rst_cnt_pwron = rx_buf[14];
	result_dest->rst_cnt_wdg = rx_buf[16];
	result_dest->rst_cnt_cmd = rx_buf[18];
	result_dest->rst_cnt_mcu = rx_buf[20];
	result_dest->rst_cnt_emlopo = rx_buf[22];
	result_dest->prevcmd_elapsed = rx_buf[24];
	result_dest->unix_time = rx_buf[26];
	result_dest->unix_year = rx_buf[30];
	result_dest->unix_month = rx_buf[31];
	result_dest->unix_day = rx_buf[32];
	result_dest->unix_hour = rx_buf[33];
	result_dest->unix_minute = rx_buf[34];
	result_dest->unix_second = rx_buf[35];
	return 0;
}


void eps_get_pdu_piu_overcurrent_fault_state(PDU_PIU_OFS* result_dest) {
	const uint8_t CC = 0x42;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[78];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 78, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->stat_ch_on = rx_buf[6];
	result_dest->stat_ch_ext_on = rx_buf[8];
	result_dest->stat_ch_ocf = rx_buf[10];
	result_dest->stat_ch_ext_ocf = rx_buf[12];

	result_dest->ocf_cnt_ch00 = rx_buf[14];
	result_dest->ocf_cnt_ch01 = rx_buf[16];
	result_dest->ocf_cnt_ch02 = rx_buf[18];
	result_dest->ocf_cnt_ch03 = rx_buf[20];
	result_dest->ocf_cnt_ch04 = rx_buf[22];
	result_dest->ocf_cnt_ch05 = rx_buf[24];
	result_dest->ocf_cnt_ch06 = rx_buf[26];
	result_dest->ocf_cnt_ch07 = rx_buf[28];
	result_dest->ocf_cnt_ch08 = rx_buf[30];
	result_dest->ocf_cnt_ch09 = rx_buf[32];
	result_dest->ocf_cnt_ch10 = rx_buf[34];
	result_dest->ocf_cnt_ch11 = rx_buf[36];
	result_dest->ocf_cnt_ch12 = rx_buf[38];
	result_dest->ocf_cnt_ch13 = rx_buf[40];
	result_dest->ocf_cnt_ch14 = rx_buf[42];
	result_dest->ocf_cnt_ch15 = rx_buf[44];
	result_dest->ocf_cnt_ch16 = rx_buf[46];
	result_dest->ocf_cnt_ch17 = rx_buf[48];
	result_dest->ocf_cnt_ch18 = rx_buf[50];
	result_dest->ocf_cnt_ch19 = rx_buf[52];
	result_dest->ocf_cnt_ch20 = rx_buf[54];
	result_dest->ocf_cnt_ch21 = rx_buf[56];
	result_dest->ocf_cnt_ch22 = rx_buf[58];
	result_dest->ocf_cnt_ch23 = rx_buf[60];
	result_dest->ocf_cnt_ch24 = rx_buf[62];
	result_dest->ocf_cnt_ch25 = rx_buf[64];
	result_dest->ocf_cnt_ch26 = rx_buf[66];
	result_dest->ocf_cnt_ch27 = rx_buf[68];
	result_dest->ocf_cnt_ch28 = rx_buf[70];
	result_dest->ocf_cnt_ch29 = rx_buf[72];
	result_dest->ocf_cnt_ch30 = rx_buf[74];
	result_dest->ocf_cnt_ch31 = rx_buf[76];

}


//__________________________________________________________________________________________________

void eps_get_pbu_abf_placed_state(PBU_ABF_PS* result_dest) {
	const uint8_t CC = 0x44;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[8];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 8, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->ABF_Placed_0 = rx_buf[6];
	result_dest->ABF_Placed_1 = rx_buf[7];


}

//_________________________________________________________________________________________________



void eps_get_pdu_housekeeping_data_raw(PDU_HK_D* result_dest) {
	const uint8_t CC = 0x50;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[258];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 258, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP_MCU = rx_buf[8];
	result_dest->VIP_INPUT.vipd_array[0] = rx_buf[10];
	result_dest->STAT_CH_ON = rx_buf[16];
	result_dest->STAT_CH_EXT_ON = rx_buf[18];
	result_dest->STAT_CH_OCF = rx_buf[20];
	result_dest->STAT_CH_EXT_OCF = rx_buf[22];

	result_dest->VIP_VD0.vipd_array[0] = rx_buf[24];
	result_dest->VIP_VD1.vipd_array[0] = rx_buf[30];
	result_dest->VIP_VD2.vipd_array[0] = rx_buf[36];
	result_dest->VIP_VD3.vipd_array[0] = rx_buf[42];
	result_dest->VIP_VD4.vipd_array[0] = rx_buf[48];
	result_dest->VIP_VD5.vipd_array[0] = rx_buf[54];
	result_dest->VIP_VD6.vipd_array[0] = rx_buf[60];

	result_dest->VIP_CH00.vipd_array[0] = rx_buf[66];
	result_dest->VIP_CH01.vipd_array[0] = rx_buf[72];
	result_dest->VIP_CH02.vipd_array[0] = rx_buf[78];
	result_dest->VIP_CH03.vipd_array[0] = rx_buf[84];
	result_dest->VIP_CH04.vipd_array[0] = rx_buf[90];
	result_dest->VIP_CH05.vipd_array[0] = rx_buf[96];
	result_dest->VIP_CH06.vipd_array[0] = rx_buf[102];
	result_dest->VIP_CH07.vipd_array[0] = rx_buf[108];
	result_dest->VIP_CH08.vipd_array[0] = rx_buf[114];
	result_dest->VIP_CH09.vipd_array[0] = rx_buf[120];
	result_dest->VIP_CH10.vipd_array[0] = rx_buf[126];
	result_dest->VIP_CH11.vipd_array[0] = rx_buf[132];
	result_dest->VIP_CH12.vipd_array[0] = rx_buf[138];
	result_dest->VIP_CH13.vipd_array[0] = rx_buf[144];
	result_dest->VIP_CH14.vipd_array[0] = rx_buf[150];
	result_dest->VIP_CH15.vipd_array[0] = rx_buf[156];
	result_dest->VIP_CH16.vipd_array[0] = rx_buf[162];
	result_dest->VIP_CH17.vipd_array[0] = rx_buf[168];
	result_dest->VIP_CH18.vipd_array[0] = rx_buf[174];
	result_dest->VIP_CH19.vipd_array[0] = rx_buf[180];
	result_dest->VIP_CH20.vipd_array[0] = rx_buf[186];
	result_dest->VIP_CH21.vipd_array[0] = rx_buf[192];
	result_dest->VIP_CH22.vipd_array[0] = rx_buf[198];
	result_dest->VIP_CH23.vipd_array[0] = rx_buf[204];
	result_dest->VIP_CH24.vipd_array[0] = rx_buf[210];
	result_dest->VIP_CH25.vipd_array[0] = rx_buf[216];
	result_dest->VIP_CH26.vipd_array[0] = rx_buf[222];
	result_dest->VIP_CH27.vipd_array[0] = rx_buf[228];
	result_dest->VIP_CH28.vipd_array[0] = rx_buf[234];
	result_dest->VIP_CH29.vipd_array[0] = rx_buf[240];
	result_dest->VIP_CH30.vipd_array[0] = rx_buf[246];
	result_dest->VIP_CH31.vipd_array[0] = rx_buf[252];

}

//_________________________________________________________________________________________________

void eps_get_pdu_housekeeping_data_eng(PDU_HK_D* result_dest) {
	const uint8_t CC = 0x52;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[258];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 258, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP_MCU = rx_buf[12];
	result_dest->VIP_INPUT.vipd_array[0] = rx_buf[16];
	result_dest->STAT_CH_ON = rx_buf[16];
	result_dest->STAT_CH_EXT_ON = rx_buf[18];
	result_dest->STAT_CH_OCF = rx_buf[20];
	result_dest->STAT_CH_EXT_OCF = rx_buf[22];

	result_dest->VIP_VD0.vipd_array[0] = rx_buf[24];
	result_dest->VIP_VD1.vipd_array[0] = rx_buf[30];
	result_dest->VIP_VD2.vipd_array[0] = rx_buf[36];
	result_dest->VIP_VD3.vipd_array[0] = rx_buf[42];
	result_dest->VIP_VD4.vipd_array[0] = rx_buf[48];
	result_dest->VIP_VD5.vipd_array[0] = rx_buf[54];
	result_dest->VIP_VD6.vipd_array[0] = rx_buf[60];

	result_dest->VIP_CH00.vipd_array[0] = rx_buf[66];
	result_dest->VIP_CH01.vipd_array[0] = rx_buf[72];
	result_dest->VIP_CH02.vipd_array[0] = rx_buf[78];
	result_dest->VIP_CH03.vipd_array[0] = rx_buf[84];
	result_dest->VIP_CH04.vipd_array[0] = rx_buf[90];
	result_dest->VIP_CH05.vipd_array[0] = rx_buf[96];
	result_dest->VIP_CH06.vipd_array[0] = rx_buf[102];
	result_dest->VIP_CH07.vipd_array[0] = rx_buf[108];
	result_dest->VIP_CH08.vipd_array[0] = rx_buf[114];
	result_dest->VIP_CH09.vipd_array[0] = rx_buf[120];
	result_dest->VIP_CH10.vipd_array[0] = rx_buf[126];
	result_dest->VIP_CH11.vipd_array[0] = rx_buf[132];
	result_dest->VIP_CH12.vipd_array[0] = rx_buf[138];
	result_dest->VIP_CH13.vipd_array[0] = rx_buf[144];
	result_dest->VIP_CH14.vipd_array[0] = rx_buf[150];
	result_dest->VIP_CH15.vipd_array[0] = rx_buf[156];
	result_dest->VIP_CH16.vipd_array[0] = rx_buf[162];
	result_dest->VIP_CH17.vipd_array[0] = rx_buf[168];
	result_dest->VIP_CH18.vipd_array[0] = rx_buf[174];
	result_dest->VIP_CH19.vipd_array[0] = rx_buf[180];
	result_dest->VIP_CH20.vipd_array[0] = rx_buf[186];
	result_dest->VIP_CH21.vipd_array[0] = rx_buf[192];
	result_dest->VIP_CH22.vipd_array[0] = rx_buf[198];
	result_dest->VIP_CH23.vipd_array[0] = rx_buf[204];
	result_dest->VIP_CH24.vipd_array[0] = rx_buf[210];
	result_dest->VIP_CH25.vipd_array[0] = rx_buf[216];
	result_dest->VIP_CH26.vipd_array[0] = rx_buf[222];
	result_dest->VIP_CH27.vipd_array[0] = rx_buf[228];
	result_dest->VIP_CH28.vipd_array[0] = rx_buf[234];
	result_dest->VIP_CH29.vipd_array[0] = rx_buf[240];
	result_dest->VIP_CH30.vipd_array[0] = rx_buf[246];
	result_dest->VIP_CH31.vipd_array[0] = rx_buf[252];

}

//__________________________________________________________________________________________________________________


void eps_get_pdu_housekeeping_data_running_average(PDU_HK_D* result_dest) {
	const uint8_t CC = 0x54;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[258];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 258, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP_MCU = rx_buf[12];
	result_dest->VIP_INPUT.vipd_array[0] = rx_buf[16];
	result_dest->STAT_CH_ON = rx_buf[16];
	result_dest->STAT_CH_EXT_ON = rx_buf[18];
	result_dest->STAT_CH_OCF = rx_buf[20];
	result_dest->STAT_CH_EXT_OCF = rx_buf[22];

	result_dest->VIP_VD0.vipd_array[0] = rx_buf[24];
	result_dest->VIP_VD1.vipd_array[0] = rx_buf[30];
	result_dest->VIP_VD2.vipd_array[0] = rx_buf[36];
	result_dest->VIP_VD3.vipd_array[0] = rx_buf[42];
	result_dest->VIP_VD4.vipd_array[0] = rx_buf[48];
	result_dest->VIP_VD5.vipd_array[0] = rx_buf[54];
	result_dest->VIP_VD6.vipd_array[0] = rx_buf[60];

	result_dest->VIP_CH00.vipd_array[0] = rx_buf[66];
	result_dest->VIP_CH01.vipd_array[0] = rx_buf[72];
	result_dest->VIP_CH02.vipd_array[0] = rx_buf[78];
	result_dest->VIP_CH03.vipd_array[0] = rx_buf[84];
	result_dest->VIP_CH04.vipd_array[0] = rx_buf[90];
	result_dest->VIP_CH05.vipd_array[0] = rx_buf[96];
	result_dest->VIP_CH06.vipd_array[0] = rx_buf[102];
	result_dest->VIP_CH07.vipd_array[0] = rx_buf[108];
	result_dest->VIP_CH08.vipd_array[0] = rx_buf[114];
	result_dest->VIP_CH09.vipd_array[0] = rx_buf[120];
	result_dest->VIP_CH10.vipd_array[0] = rx_buf[126];
	result_dest->VIP_CH11.vipd_array[0] = rx_buf[132];
	result_dest->VIP_CH12.vipd_array[0] = rx_buf[138];
	result_dest->VIP_CH13.vipd_array[0] = rx_buf[144];
	result_dest->VIP_CH14.vipd_array[0] = rx_buf[150];
	result_dest->VIP_CH15.vipd_array[0] = rx_buf[156];
	result_dest->VIP_CH16.vipd_array[0] = rx_buf[162];
	result_dest->VIP_CH17.vipd_array[0] = rx_buf[168];
	result_dest->VIP_CH18.vipd_array[0] = rx_buf[174];
	result_dest->VIP_CH19.vipd_array[0] = rx_buf[180];
	result_dest->VIP_CH20.vipd_array[0] = rx_buf[186];
	result_dest->VIP_CH21.vipd_array[0] = rx_buf[192];
	result_dest->VIP_CH22.vipd_array[0] = rx_buf[198];
	result_dest->VIP_CH23.vipd_array[0] = rx_buf[204];
	result_dest->VIP_CH24.vipd_array[0] = rx_buf[210];
	result_dest->VIP_CH25.vipd_array[0] = rx_buf[216];
	result_dest->VIP_CH26.vipd_array[0] = rx_buf[222];
	result_dest->VIP_CH27.vipd_array[0] = rx_buf[228];
	result_dest->VIP_CH28.vipd_array[0] = rx_buf[234];
	result_dest->VIP_CH29.vipd_array[0] = rx_buf[240];
	result_dest->VIP_CH30.vipd_array[0] = rx_buf[246];
	result_dest->VIP_CH31.vipd_array[0] = rx_buf[252];

}

//_____________________________________________________________________________________________________________________________

void eps_get_pbu_housekeeping_data_raw(PBU_HK_D* result_dest) {
	const uint8_t CC = 0x60;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[84];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 84, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP_MCU = rx_buf[8];
	result_dest->VIP_INPUT.vipd_array[0] = rx_buf[10];
	result_dest->STAT_BU = rx_buf[16];

	result_dest->BP1.VIP_BP_Input.vipd_array[0] = rx_buf[18];
	result_dest->BP1.STAT_BP = rx_buf[24];
	result_dest->BP1.VOLT_CELL1 = rx_buf[26];
	result_dest->BP1.VOLT_CELL2 = rx_buf[28];
	result_dest->BP1.VOLT_CELL3 = rx_buf[30];
	result_dest->BP1.VOLT_CELL4 = rx_buf[32];
	result_dest->BP1.BAT_TEMP1 = rx_buf[34];
	result_dest->BP1.BAT_TEMP2 = rx_buf[36];
	result_dest->BP1.BAT_TEMP3 = rx_buf[38];


	result_dest->BP2.VIP_BP_Input.vipd_array[0] = rx_buf[40];
	result_dest->BP2.STAT_BP = rx_buf[46];
	result_dest->BP2.VOLT_CELL1 = rx_buf[48];
	result_dest->BP2.VOLT_CELL2 = rx_buf[50];
	result_dest->BP2.VOLT_CELL3 = rx_buf[52];
	result_dest->BP2.VOLT_CELL4 = rx_buf[54];
	result_dest->BP2.BAT_TEMP1 = rx_buf[56];
	result_dest->BP2.BAT_TEMP2 = rx_buf[58];
	result_dest->BP2.BAT_TEMP3 = rx_buf[60];


	result_dest->BP3.VIP_BP_Input.vipd_array[0] = rx_buf[62];
	result_dest->BP3.STAT_BP = rx_buf[68];
	result_dest->BP3.VOLT_CELL1 = rx_buf[70];
	result_dest->BP3.VOLT_CELL2 = rx_buf[72];
	result_dest->BP3.VOLT_CELL3 = rx_buf[74];
	result_dest->BP3.VOLT_CELL4 = rx_buf[76];
	result_dest->BP3.BAT_TEMP1 = rx_buf[78];
	result_dest->BP3.BAT_TEMP2 = rx_buf[80];
	result_dest->BP3.BAT_TEMP3 = rx_buf[82];


}

//_______________________________________________________________________________________________

void eps_get_pbu_housekeeping_data_eng(PBU_HK_D* result_dest) {
	const uint8_t CC = 0x62;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[84];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 84, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP_MCU = rx_buf[12];
	result_dest->VIP_INPUT.vipd_array[0] = rx_buf[10];
	result_dest->STAT_BU = rx_buf[16];

	result_dest->BP1.VIP_BP_Input.vipd_array[0] = rx_buf[18];
	result_dest->BP1.STAT_BP = rx_buf[24];
	result_dest->BP1.VOLT_CELL1 = rx_buf[26];
	result_dest->BP1.VOLT_CELL2 = rx_buf[28];
	result_dest->BP1.VOLT_CELL3 = rx_buf[30];
	result_dest->BP1.VOLT_CELL4 = rx_buf[32];
	result_dest->BP1.BAT_TEMP1 = rx_buf[34];
	result_dest->BP1.BAT_TEMP2 = rx_buf[36];
	result_dest->BP1.BAT_TEMP3 = rx_buf[38];


	result_dest->BP2.VIP_BP_Input.vipd_array[0] = rx_buf[40];
	result_dest->BP2.STAT_BP = rx_buf[46];
	result_dest->BP2.VOLT_CELL1 = rx_buf[48];
	result_dest->BP2.VOLT_CELL2 = rx_buf[50];
	result_dest->BP2.VOLT_CELL3 = rx_buf[52];
	result_dest->BP2.VOLT_CELL4 = rx_buf[54];
	result_dest->BP2.BAT_TEMP1 = rx_buf[56];
	result_dest->BP2.BAT_TEMP2 = rx_buf[58];
	result_dest->BP2.BAT_TEMP3 = rx_buf[60];


	result_dest->BP3.VIP_BP_Input.vipd_array[0] = rx_buf[62];
	result_dest->BP3.STAT_BP = rx_buf[68];
	result_dest->BP3.VOLT_CELL1 = rx_buf[70];
	result_dest->BP3.VOLT_CELL2 = rx_buf[72];
	result_dest->BP3.VOLT_CELL3 = rx_buf[74];
	result_dest->BP3.VOLT_CELL4 = rx_buf[76];
	result_dest->BP3.BAT_TEMP1 = rx_buf[78];
	result_dest->BP3.BAT_TEMP2 = rx_buf[80];
	result_dest->BP3.BAT_TEMP3 = rx_buf[82];

}

//________________________________________________________________________________________

void eps_get_pbu_housekeeping_data_running_average(PBU_HK_D* result_dest) {
	const uint8_t CC = 0x64;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[84];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 84, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP_MCU = rx_buf[12];
	result_dest->VIP_INPUT.vipd_array[0] = rx_buf[10];
	result_dest->STAT_BU = rx_buf[16];

	result_dest->status = rx_buf[4];
	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP_MCU = rx_buf[12];
	result_dest->VIP_INPUT.vipd_array[0] = rx_buf[10];
	result_dest->STAT_BU = rx_buf[16];

	result_dest->BP1.VIP_BP_Input.vipd_array[0] = rx_buf[18];
	result_dest->BP1.STAT_BP = rx_buf[24];
	result_dest->BP1.VOLT_CELL1 = rx_buf[26];
	result_dest->BP1.VOLT_CELL2 = rx_buf[28];
	result_dest->BP1.VOLT_CELL3 = rx_buf[30];
	result_dest->BP1.VOLT_CELL4 = rx_buf[32];
	result_dest->BP1.BAT_TEMP1 = rx_buf[34];
	result_dest->BP1.BAT_TEMP2 = rx_buf[36];
	result_dest->BP1.BAT_TEMP3 = rx_buf[38];


	result_dest->BP2.VIP_BP_Input.vipd_array[0] = rx_buf[40];
	result_dest->BP2.STAT_BP = rx_buf[46];
	result_dest->BP2.VOLT_CELL1 = rx_buf[48];
	result_dest->BP2.VOLT_CELL2 = rx_buf[50];
	result_dest->BP2.VOLT_CELL3 = rx_buf[52];
	result_dest->BP2.VOLT_CELL4 = rx_buf[54];
	result_dest->BP2.BAT_TEMP1 = rx_buf[56];
	result_dest->BP2.BAT_TEMP2 = rx_buf[58];
	result_dest->BP2.BAT_TEMP3 = rx_buf[60];


	result_dest->BP3.VIP_BP_Input.vipd_array[0] = rx_buf[62];
	result_dest->BP3.STAT_BP = rx_buf[68];
	result_dest->BP3.VOLT_CELL1 = rx_buf[70];
	result_dest->BP3.VOLT_CELL2 = rx_buf[72];
	result_dest->BP3.VOLT_CELL3 = rx_buf[74];
	result_dest->BP3.VOLT_CELL4 = rx_buf[76];
	result_dest->BP3.BAT_TEMP1 = rx_buf[78];
	result_dest->BP3.BAT_TEMP2 = rx_buf[80];
	result_dest->BP3.BAT_TEMP3 = rx_buf[82];


}

//_________________________________________________________________________________________________

void eps_get_pcu_housekeeping_data_raw(PCU_HK_D* result_dest) {
	const uint8_t CC = 0x70;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[72];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 72, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP_MCU = rx_buf[8];
	result_dest->VIP_OUTPUT.vipd_array[0] = rx_buf[10];

	result_dest->CC1.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[16];
	result_dest->CC1.VOLT_IN_MPPT = rx_buf[22];
	result_dest->CC1.CURR_IN_MPPT = rx_buf[24];
	result_dest->CC1.VOLT_OU_MPPT = rx_buf[26];
	result_dest->CC1.CURR_OU_MPPT = rx_buf[28];

	result_dest->CC2.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[30];
	result_dest->CC2.VOLT_IN_MPPT = rx_buf[36];
	result_dest->CC2.CURR_IN_MPPT = rx_buf[38];
	result_dest->CC2.VOLT_OU_MPPT = rx_buf[40];
	result_dest->CC2.CURR_OU_MPPT = rx_buf[42];

	result_dest->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[44];
	result_dest->CC3.VOLT_IN_MPPT = rx_buf[50];
	result_dest->CC3.CURR_IN_MPPT = rx_buf[52];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[54];
	result_dest->CC3.CURR_OU_MPPT = rx_buf[56];

	result_dest->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[58];
	result_dest->CC3.VOLT_IN_MPPT = rx_buf[64];
	result_dest->CC3.CURR_IN_MPPT = rx_buf[66];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[68];
	result_dest->CC3.CURR_OU_MPPT = rx_buf[70];

}

//_______________________________________________________________________________________

void eps_get_pcu_housekeeping_data_eng(PCU_HK_D* result_dest) {
	const uint8_t CC = 0x72;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[72];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 72, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP_MCU = rx_buf[8];
	result_dest->VIP_OUTPUT.vipd_array[0] = rx_buf[10];

	result_dest->CC1.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[16];
	result_dest->CC1.VOLT_IN_MPPT = rx_buf[22];
	result_dest->CC1.CURR_IN_MPPT = rx_buf[24];
	result_dest->CC1.VOLT_OU_MPPT = rx_buf[26];
	result_dest->CC1.CURR_OU_MPPT = rx_buf[28];

	result_dest->CC2.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[30];
	result_dest->CC2.VOLT_IN_MPPT = rx_buf[36];
	result_dest->CC2.CURR_IN_MPPT = rx_buf[38];
	result_dest->CC2.VOLT_OU_MPPT = rx_buf[40];
	result_dest->CC2.CURR_OU_MPPT = rx_buf[42];

	result_dest->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[44];
	result_dest->CC3.VOLT_IN_MPPT = rx_buf[50];
	result_dest->CC3.CURR_IN_MPPT = rx_buf[52];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[54];
	result_dest->CC3.CURR_OU_MPPT = rx_buf[56];

	result_dest->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[58];
	result_dest->CC3.VOLT_IN_MPPT = rx_buf[64];
	result_dest->CC3.CURR_IN_MPPT = rx_buf[66];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[68];
	result_dest->CC3.CURR_OU_MPPT = rx_buf[70];
}

//_______________________________________________________________________________________

void eps_get_pcu_housekeeping_data_running_average(PCU_HK_D* result_dest) {
	const uint8_t CC = 0x74;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[72];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 72, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP_MCU = rx_buf[8];
	result_dest->VIP_OUTPUT.vipd_array[0] = rx_buf[10];

	result_dest->CC1.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[16];
	result_dest->CC1.VOLT_IN_MPPT = rx_buf[22];
	result_dest->CC1.CURR_IN_MPPT = rx_buf[24];
	result_dest->CC1.VOLT_OU_MPPT = rx_buf[26];
	result_dest->CC1.CURR_OU_MPPT = rx_buf[28];

	result_dest->CC2.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[30];
	result_dest->CC2.VOLT_IN_MPPT = rx_buf[36];
	result_dest->CC2.CURR_IN_MPPT = rx_buf[38];
	result_dest->CC2.VOLT_OU_MPPT = rx_buf[40];
	result_dest->CC2.CURR_OU_MPPT = rx_buf[42];

	result_dest->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[44];
	result_dest->CC3.VOLT_IN_MPPT = rx_buf[50];
	result_dest->CC3.CURR_IN_MPPT = rx_buf[52];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[54];
	result_dest->CC3.CURR_OU_MPPT = rx_buf[56];

	result_dest->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[58];
	result_dest->CC3.VOLT_IN_MPPT = rx_buf[64];
	result_dest->CC3.CURR_IN_MPPT = rx_buf[66];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[68];
	result_dest->CC3.CURR_OU_MPPT = rx_buf[70];

}

//----------------------------------------------------------------------------------------------------

void eps_get_configuration_parameter(GET_CONFIG_PARAM* result_dest, uint16_t PAR_ID) {
	const uint8_t CC = 0x82;
	uint8_t cmd_buf[6];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	cmd_buf[4] = PAR_ID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 6, HAL_MAX_DELAY);

	uint8_t rx_buf[16];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 16, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->PAR_ID = rx_buf[6];
	result_dest->PAR_VAL = rx_buf[8];

}

//-----------------------------------------------------------------------------------------------------
void eps_set_configuration_parameter(SET_CONFIG_PARAM* result_dest, uint16_t PAR_ID, uint8_t PAR_VAL) {
	const uint8_t CC = 0x84;
	uint8_t cmd_buf[14];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[3] = CC;
	cmd_buf[2] = EPS_COMMAND_BID;

	cmd_buf[4] = PAR_ID;
	cmd_buf[6] = PAR_VAL;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 14, HAL_MAX_DELAY);

	uint8_t rx_buf[16];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 16, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->PAR_ID = rx_buf[6];
	result_dest->PAR_VAL = rx_buf[8];

}

//-----------------------------------------------------------------------------------------------------

void eps_reset_configuration_parameter(RESET_CONFIG_PAR* result_dest, uint16_t PAR_ID) {
	const uint8_t CC = 0x86;
	uint8_t cmd_buf[6];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	cmd_buf[4] = PAR_ID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 6, HAL_MAX_DELAY);

	uint8_t rx_buf[16];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 16, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];
	result_dest->PAR_ID = rx_buf[6];
	result_dest->PAR_VAL = rx_buf[8];
}

//---------------------------------------------------------------------------------------------------------

void eps_reset_configuration(RESET_CONFIGURATION* result_dest) {
	const uint8_t CC = 0x90;
	uint8_t CONF_KEY = 0x87;

	uint8_t cmd_buf[5];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	cmd_buf[4] = CONF_KEY;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 5, HAL_MAX_DELAY);

	uint8_t rx_buf[16];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 16, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];

}

//---------------------------------------------------------------------------------------------------

void eps_load_configuration(LOAD_CONFIGURATION* result_dest) {
	const uint8_t CC = 0x92;
	uint8_t CONF_KEY = 0xA7;

	uint8_t cmd_buf[5];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	cmd_buf[4] = CONF_KEY;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 5, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];

}

//---------------------------------------------------------------------------------------------

void eps_save_configuration(SAVE_CONFIGURATION* result_dest) {
	const uint8_t CC = 0x94;
	uint8_t CONF_KEY = 0xA7;
	uint16_t CHECKSUM = 0;

	uint8_t cmd_buf[7];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	cmd_buf[4] = CONF_KEY;
	cmd_buf[5] = CHECKSUM;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 7, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];

}

//------------------------------------------------------------------------------------------------------------------

void eps_get_piu_housekeeping_data_raw(GET_PIU_HK* result_dest) {
	const uint8_t CC = 0xA0;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[274];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 274, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];


	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP = rx_buf[8];
	result_dest->VIP_DIST_INPUT.vipd_array[0] = rx_buf[10];
	result_dest->VIP_BATT_INPUT.vipd_array[0] = rx_buf[16];
	result_dest->SAT_CH_ON = rx_buf[22];
	result_dest->STAT_CH_OCF = rx_buf[24];

	result_dest->BAT_STAT = rx_buf[26];

	result_dest->BAT_TEMP2 = rx_buf[28];
	result_dest->BAT_TEMP3 = rx_buf[30];

	result_dest->VOLT_VD0 = rx_buf[32];
	result_dest->VOLT_VD1 = rx_buf[34];
	result_dest->VOLT_VD2 = rx_buf[36];

	result_dest->VIP_CH00.vipd_array[0] = rx_buf[38];
	result_dest->VIP_CH01.vipd_array[0] = rx_buf[44];
	result_dest->VIP_CH02.vipd_array[0] = rx_buf[50];
	result_dest->VIP_CH03.vipd_array[0] = rx_buf[56];
	result_dest->VIP_CH04.vipd_array[0] = rx_buf[62];
	result_dest->VIP_CH05.vipd_array[0] = rx_buf[68];
	result_dest->VIP_CH06.vipd_array[0] = rx_buf[74];
	result_dest->VIP_CH07.vipd_array[0] = rx_buf[80];
	result_dest->VIP_CH08.vipd_array[0] = rx_buf[86];

	result_dest->CC1.VOLT_IN_MPPT = rx_buf[92];
	result_dest->CC1.CURR_IN_MPPT = rx_buf[94];
	result_dest->CC1.VOLT_OU_MPPT = rx_buf[96];
	result_dest->CC1.VOLT_OU_MPPT = rx_buf[98];

	result_dest->CC2.VOLT_IN_MPPT = rx_buf[100];
	result_dest->CC2.CURR_IN_MPPT = rx_buf[102];
	result_dest->CC2.VOLT_OU_MPPT = rx_buf[104];
	result_dest->CC2.VOLT_OU_MPPT = rx_buf[106];

	result_dest->CC3.VOLT_IN_MPPT = rx_buf[108];
	result_dest->CC3.CURR_IN_MPPT = rx_buf[110];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[112];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[114];

	result_dest->VIP_CH09.vipd_array[0] = rx_buf[116];
	result_dest->VIP_CH10.vipd_array[0] = rx_buf[122];
	result_dest->VIP_CH11.vipd_array[0] = rx_buf[128];
	result_dest->VIP_CH12.vipd_array[0] = rx_buf[134];
	result_dest->VIP_CH13.vipd_array[0] = rx_buf[140];
	result_dest->VIP_CH14.vipd_array[0] = rx_buf[146];
	result_dest->VIP_CH15.vipd_array[0] = rx_buf[152];

	result_dest->CC4.VOLT_IN_MPPT = rx_buf[158];
	result_dest->CC4.CURR_IN_MPPT = rx_buf[160];
	result_dest->CC4.VOLT_OU_MPPT = rx_buf[162];
	result_dest->CC4.VOLT_OU_MPPT = rx_buf[164];

	result_dest->CC5.VOLT_IN_MPPT = rx_buf[166];
	result_dest->CC5.CURR_IN_MPPT = rx_buf[168];
	result_dest->CC5.VOLT_OU_MPPT = rx_buf[170];
	result_dest->CC5.VOLT_OU_MPPT = rx_buf[172];

	result_dest->STAT_CH_EXT_ON = rx_buf[174];
	result_dest->STAT_CH_EXT_OCF = rx_buf[176];

	result_dest->VIP_CH16.vipd_array[0] = rx_buf[178];
	result_dest->VIP_CH17.vipd_array[0] = rx_buf[184];
	result_dest->VIP_CH18.vipd_array[0] = rx_buf[190];
	result_dest->VIP_CH19.vipd_array[0] = rx_buf[196];
	result_dest->VIP_CH20.vipd_array[0] = rx_buf[202];
	result_dest->VIP_CH21.vipd_array[0] = rx_buf[208];
	result_dest->VIP_CH22.vipd_array[0] = rx_buf[214];
	result_dest->VIP_CH23.vipd_array[0] = rx_buf[220];
	result_dest->VIP_CH24.vipd_array[0] = rx_buf[226];
	result_dest->VIP_CH25.vipd_array[0] = rx_buf[232];
	result_dest->VIP_CH26.vipd_array[0] = rx_buf[238];
	result_dest->VIP_CH27.vipd_array[0] = rx_buf[244];
	result_dest->VIP_CH28.vipd_array[0] = rx_buf[250];
	result_dest->VIP_CH29.vipd_array[0] = rx_buf[256];
	result_dest->VIP_CH30.vipd_array[0] = rx_buf[262];
	result_dest->VIP_CH31.vipd_array[0] = rx_buf[268];

}

//-----------------------------------------------------------------------------------------------------

void eps_get_piu_housekeeping_data_eng(GET_PIU_HK* result_dest) {
	const uint8_t CC = 0xA2;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[274];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 274, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];


	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP = rx_buf[8];
	result_dest->VIP_DIST_INPUT.vipd_array[0] = rx_buf[10];
	result_dest->VIP_BATT_INPUT.vipd_array[0] = rx_buf[16];
	result_dest->SAT_CH_ON = rx_buf[22];
	result_dest->STAT_CH_OCF = rx_buf[24];

	result_dest->BAT_STAT = rx_buf[26];

	result_dest->BAT_TEMP2 = rx_buf[28];
	result_dest->BAT_TEMP3 = rx_buf[30];

	result_dest->VOLT_VD0 = rx_buf[32];
	result_dest->VOLT_VD1 = rx_buf[34];
	result_dest->VOLT_VD2 = rx_buf[36];

	result_dest->VIP_CH00.vipd_array[0] = rx_buf[38];
	result_dest->VIP_CH01.vipd_array[0] = rx_buf[44];
	result_dest->VIP_CH02.vipd_array[0] = rx_buf[50];
	result_dest->VIP_CH03.vipd_array[0] = rx_buf[56];
	result_dest->VIP_CH04.vipd_array[0] = rx_buf[62];
	result_dest->VIP_CH05.vipd_array[0] = rx_buf[68];
	result_dest->VIP_CH06.vipd_array[0] = rx_buf[74];
	result_dest->VIP_CH07.vipd_array[0] = rx_buf[80];
	result_dest->VIP_CH08.vipd_array[0] = rx_buf[86];

	result_dest->CC1.VOLT_IN_MPPT = rx_buf[92];
	result_dest->CC1.CURR_IN_MPPT = rx_buf[94];
	result_dest->CC1.VOLT_OU_MPPT = rx_buf[96];
	result_dest->CC1.VOLT_OU_MPPT = rx_buf[98];

	result_dest->CC2.VOLT_IN_MPPT = rx_buf[100];
	result_dest->CC2.CURR_IN_MPPT = rx_buf[102];
	result_dest->CC2.VOLT_OU_MPPT = rx_buf[104];
	result_dest->CC2.VOLT_OU_MPPT = rx_buf[106];

	result_dest->CC3.VOLT_IN_MPPT = rx_buf[108];
	result_dest->CC3.CURR_IN_MPPT = rx_buf[110];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[112];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[114];

	result_dest->VIP_CH09.vipd_array[0] = rx_buf[116];
	result_dest->VIP_CH10.vipd_array[0] = rx_buf[122];
	result_dest->VIP_CH11.vipd_array[0] = rx_buf[128];
	result_dest->VIP_CH12.vipd_array[0] = rx_buf[134];
	result_dest->VIP_CH13.vipd_array[0] = rx_buf[140];
	result_dest->VIP_CH14.vipd_array[0] = rx_buf[146];
	result_dest->VIP_CH15.vipd_array[0] = rx_buf[152];

	result_dest->CC4.VOLT_IN_MPPT = rx_buf[158];
	result_dest->CC4.CURR_IN_MPPT = rx_buf[160];
	result_dest->CC4.VOLT_OU_MPPT = rx_buf[162];
	result_dest->CC4.VOLT_OU_MPPT = rx_buf[164];

	result_dest->CC5.VOLT_IN_MPPT = rx_buf[166];
	result_dest->CC5.CURR_IN_MPPT = rx_buf[168];
	result_dest->CC5.VOLT_OU_MPPT = rx_buf[170];
	result_dest->CC5.VOLT_OU_MPPT = rx_buf[172];

	result_dest->STAT_CH_EXT_ON = rx_buf[174];
	result_dest->STAT_CH_EXT_OCF = rx_buf[176];

	result_dest->VIP_CH16.vipd_array[0] = rx_buf[178];
	result_dest->VIP_CH17.vipd_array[0] = rx_buf[184];
	result_dest->VIP_CH18.vipd_array[0] = rx_buf[190];
	result_dest->VIP_CH19.vipd_array[0] = rx_buf[196];
	result_dest->VIP_CH20.vipd_array[0] = rx_buf[202];
	result_dest->VIP_CH21.vipd_array[0] = rx_buf[208];
	result_dest->VIP_CH22.vipd_array[0] = rx_buf[214];
	result_dest->VIP_CH23.vipd_array[0] = rx_buf[220];
	result_dest->VIP_CH24.vipd_array[0] = rx_buf[226];
	result_dest->VIP_CH25.vipd_array[0] = rx_buf[232];
	result_dest->VIP_CH26.vipd_array[0] = rx_buf[238];
	result_dest->VIP_CH27.vipd_array[0] = rx_buf[244];
	result_dest->VIP_CH28.vipd_array[0] = rx_buf[250];
	result_dest->VIP_CH29.vipd_array[0] = rx_buf[256];
	result_dest->VIP_CH30.vipd_array[0] = rx_buf[262];
	result_dest->VIP_CH31.vipd_array[0] = rx_buf[268];

}

//------------------------------------------------------------------------------------------

void eps_get_piu_housekeeping_data_running_average(GET_PIU_HK* result_dest) {
	const uint8_t CC = 0xA4;
	uint8_t cmd_buf[4];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[274];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 274, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];


	result_dest->VOLT_BRDSUP = rx_buf[6];
	result_dest->TEMP = rx_buf[8];
	result_dest->VIP_DIST_INPUT.vipd_array[0] = rx_buf[10];
	result_dest->VIP_BATT_INPUT.vipd_array[0] = rx_buf[16];
	result_dest->SAT_CH_ON = rx_buf[22];
	result_dest->STAT_CH_OCF = rx_buf[24];

	result_dest->BAT_STAT = rx_buf[26];

	result_dest->BAT_TEMP2 = rx_buf[28];
	result_dest->BAT_TEMP3 = rx_buf[30];

	result_dest->VOLT_VD0 = rx_buf[32];
	result_dest->VOLT_VD1 = rx_buf[34];
	result_dest->VOLT_VD2 = rx_buf[36];

	result_dest->VIP_CH00.vipd_array[0] = rx_buf[38];
	result_dest->VIP_CH01.vipd_array[0] = rx_buf[44];
	result_dest->VIP_CH02.vipd_array[0] = rx_buf[50];
	result_dest->VIP_CH03.vipd_array[0] = rx_buf[56];
	result_dest->VIP_CH04.vipd_array[0] = rx_buf[62];
	result_dest->VIP_CH05.vipd_array[0] = rx_buf[68];
	result_dest->VIP_CH06.vipd_array[0] = rx_buf[74];
	result_dest->VIP_CH07.vipd_array[0] = rx_buf[80];
	result_dest->VIP_CH08.vipd_array[0] = rx_buf[86];

	result_dest->CC1.VOLT_IN_MPPT = rx_buf[92];
	result_dest->CC1.CURR_IN_MPPT = rx_buf[94];
	result_dest->CC1.VOLT_OU_MPPT = rx_buf[96];
	result_dest->CC1.VOLT_OU_MPPT = rx_buf[98];

	result_dest->CC2.VOLT_IN_MPPT = rx_buf[100];
	result_dest->CC2.CURR_IN_MPPT = rx_buf[102];
	result_dest->CC2.VOLT_OU_MPPT = rx_buf[104];
	result_dest->CC2.VOLT_OU_MPPT = rx_buf[106];

	result_dest->CC3.VOLT_IN_MPPT = rx_buf[108];
	result_dest->CC3.CURR_IN_MPPT = rx_buf[110];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[112];
	result_dest->CC3.VOLT_OU_MPPT = rx_buf[114];

	result_dest->VIP_CH09.vipd_array[0] = rx_buf[116];
	result_dest->VIP_CH10.vipd_array[0] = rx_buf[122];
	result_dest->VIP_CH11.vipd_array[0] = rx_buf[128];
	result_dest->VIP_CH12.vipd_array[0] = rx_buf[134];
	result_dest->VIP_CH13.vipd_array[0] = rx_buf[140];
	result_dest->VIP_CH14.vipd_array[0] = rx_buf[146];
	result_dest->VIP_CH15.vipd_array[0] = rx_buf[152];

	result_dest->CC4.VOLT_IN_MPPT = rx_buf[158];
	result_dest->CC4.CURR_IN_MPPT = rx_buf[160];
	result_dest->CC4.VOLT_OU_MPPT = rx_buf[162];
	result_dest->CC4.VOLT_OU_MPPT = rx_buf[164];

	result_dest->CC5.VOLT_IN_MPPT = rx_buf[166];
	result_dest->CC5.CURR_IN_MPPT = rx_buf[168];
	result_dest->CC5.VOLT_OU_MPPT = rx_buf[170];
	result_dest->CC5.VOLT_OU_MPPT = rx_buf[172];

	result_dest->STAT_CH_EXT_ON = rx_buf[174];
	result_dest->STAT_CH_EXT_OCF = rx_buf[176];

	result_dest->VIP_CH16.vipd_array[0] = rx_buf[178];
	result_dest->VIP_CH17.vipd_array[0] = rx_buf[184];
	result_dest->VIP_CH18.vipd_array[0] = rx_buf[190];
	result_dest->VIP_CH19.vipd_array[0] = rx_buf[196];
	result_dest->VIP_CH20.vipd_array[0] = rx_buf[202];
	result_dest->VIP_CH21.vipd_array[0] = rx_buf[208];
	result_dest->VIP_CH22.vipd_array[0] = rx_buf[214];
	result_dest->VIP_CH23.vipd_array[0] = rx_buf[220];
	result_dest->VIP_CH24.vipd_array[0] = rx_buf[226];
	result_dest->VIP_CH25.vipd_array[0] = rx_buf[232];
	result_dest->VIP_CH26.vipd_array[0] = rx_buf[238];
	result_dest->VIP_CH27.vipd_array[0] = rx_buf[244];
	result_dest->VIP_CH28.vipd_array[0] = rx_buf[250];
	result_dest->VIP_CH29.vipd_array[0] = rx_buf[256];
	result_dest->VIP_CH30.vipd_array[0] = rx_buf[262];
	result_dest->VIP_CH31.vipd_array[0] = rx_buf[268];

}

//--------------------------------------------------------------------------------------------------

void eps_correct_time(CORRECT_TIME_S* result_dest, int32_t time_correction) {
	const uint8_t CC = 0xC4;
	//Time correction in unix time (positive numbers added to time, negative values subtracted)
	int32_t CORRECTION = time_correction;

	uint8_t cmd_buf[8];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	cmd_buf[4] = CORRECTION;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 8, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];

}

//------------------------------------------------------------------------------------------------------

void eps_zero_reset_cause_counters(ZERO_RESET_CAUSE_COUNTERS_S* result_dest) {
	const uint8_t CC = 0xC6;
	//Time correction in unix time (positive numbers added to time, negative values subtracted)
	int32_t ZERO_KEY = 0xA7;

	uint8_t cmd_buf[5];

	cmd_buf[0] = EPS_COMMAND_STID;
	cmd_buf[1] = EPS_COMMAND_IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = EPS_COMMAND_BID;

	cmd_buf[4] = ZERO_KEY;

	HAL_I2C_Master_Transmit(&hi2c1, EPS_I2C_ADDR, cmd_buf, 5, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, EPS_I2C_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	result_dest->status = rx_buf[4];

}








