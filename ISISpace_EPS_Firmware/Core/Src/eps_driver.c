/*
 * eps_driver.c
 *
 *  Created on: Nov 14, 2023
 *      Author: frank
 */


#include "eps_driver.h"

#define PWR_SYS_ADDR 0x30 // if this macro doesn't work, then just define it as a uint8_t below in another section

uint16_t CH_VIP[6];

void eps_debug_print_channel_stats_once(EPS_CHANNEL_t eps_channel) {
	PDU_HK_D* EPS_data_received;

	uint16_t CH_VIP[6]; // channel voltage, current, and power measurement buffer

	eps_get_pdu_housekeeping_data_eng(EPS_data_received);

	CH_VIP[0] = EPS_data_received->VIP_CH00.vipd_array[0]; //Voltage measurement in 2 bytes
	CH_VIP[2] = EPS_data_received->VIP_CH00.vipd_array[2]; //Current measurement in 2 bytes
	CH_VIP[4] = EPS_data_received->VIP_CH00.vipd_array[4]; //Power measurement in 2 bytes

	// at this point we have the Channel 0 voltage, current and power readings stored in CH_VIP.

	HAL_UART_Transmit(&hlpuart1, (uint8_t*)CH_VIP, 6, HAL_MAX_DELAY);
	HAL_UART_Transmit(&hlpuart1, (uint8_t*)newline, strlen((char*)newline), HAL_MAX_DELAY);
	//HAL_Delay(1000);


	CH_VIP[0] = EPS_data_received->VIP_CH01.vipd_array[0]; //Voltage measurement in 2 bytes
	CH_VIP[2] = EPS_data_received->VIP_CH01.vipd_array[2]; //Current measurement in 2 bytes
	CH_VIP[4] = EPS_data_received->VIP_CH01.vipd_array[4]; //Power measurement in 2 bytes

	// at this point we have the Channel 1 voltage, current and power readings stored in CH_VIP.

	HAL_UART_Transmit(&hlpuart1, (uint8_t*)CH_VIP, 6, HAL_MAX_DELAY);
	HAL_UART_Transmit(&hlpuart1, (uint8_t*)newline, strlen((char*)newline), HAL_MAX_DELAY);
	//HAL_Delay(1000);


	CH_VIP[0] = EPS_data_received->VIP_CH02.vipd_array[0]; //Voltage measurement in 2 bytes
	CH_VIP[2] = EPS_data_received->VIP_CH02.vipd_array[2]; //Current measurement in 2 bytes
	CH_VIP[4] = EPS_data_received->VIP_CH02.vipd_array[4]; //Power measurement in 2 bytes

	// at this point we have the Channel 1 voltage, current and power readings stored in CH_VIP.

	HAL_UART_Transmit(&hlpuart1, (uint8_t*)CH_VIP, 6, HAL_MAX_DELAY);
	HAL_UART_Transmit(&hlpuart1, (uint8_t*)newline, strlen((char*)newline), HAL_MAX_DELAY);
	HAL_Delay(1000);

}


//Driver functions

void eps_system_reset(){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0xAA;
	uint8_t BID = 0x20;
	uint8_t Reset_key = 0xA6;

	uint8_t cmd_buf[5];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;
	cmd_buf[4] = Reset_key;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 5, 1000);
}


uint8_t eps_no_operation(){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x02;
	uint8_t BID = 0x20;

    uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);


	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


uint8_t eps_cancel_oper(){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x04;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);


	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


void eps_watchdog() {

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x06;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);
}


uint8_t eps_output_bus_group_on(uint16_t CH_BF,  uint16_t CH_EXT_BF) {

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x10;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[8];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;
	cmd_buf[4] = CH_BF;
	cmd_buf[6] = CH_EXT_BF;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 8, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


uint8_t eps_output_bus_group_off(uint16_t CH_BF,  uint16_t CH_EXT_BF) {

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x12;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[8];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;
	cmd_buf[4] = CH_BF;
	cmd_buf[6] = CH_EXT_BF;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 8, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


uint8_t eps_output_bus_group_state(uint16_t CH_BF,  uint16_t CH_EXT_BF) {

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x14;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[8];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;
	cmd_buf[4] = CH_BF;
	cmd_buf[6] = CH_EXT_BF;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 8, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


uint8_t eps_output_bus_channel_on(uint8_t CH_IDX){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x16;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[5];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;
	cmd_buf[4] = CH_IDX;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 5, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


uint8_t eps_output_bus_channel_off(uint8_t CH_IDX){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x18;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[5];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;
	cmd_buf[4] = CH_IDX;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 5, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	uint8_t status = rx_buf[4];

	return status;
}


void eps_switch_to_nominal_mode(){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x30;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);
}


void eps_switch_to_safety_mode(){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x32;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);
}


void eps_get_sys_status(sys_stat* temp){
	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x40;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[36];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 36, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->mode = rx_buf[5];
	temp->conf = rx_buf[6];
	temp->reset_cause = rx_buf[7];
	temp->uptime = rx_buf[8];
	temp->error = rx_buf[12];
	temp->rc_cnt_pwron = rx_buf[14];
	temp->rc_cnt_wdg = rx_buf[16];
	temp->rc_cnt_cmd = rx_buf[18];
	temp->rc_cnt_mcu = rx_buf[20];
	temp->rc_cnt_emlopo = rx_buf[22];
	temp->prevcmd_elapsed = rx_buf[24];
	temp->unix_time = rx_buf[26];
	temp->unix_year = rx_buf[30];
	temp->unix_month = rx_buf[31];
	temp->unix_day = rx_buf[32];
	temp->unix_hour = rx_buf[33];
	temp->unix_minute = rx_buf[34];
	temp->unix_second = rx_buf[35];

}


void eps_get_pdu_piu_overcurrent_fault_state(PDU_PIU_OFS* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x42;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[78];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 78, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->stat_ch_on = rx_buf[6];
	temp->stat_ch_ext_on = rx_buf[8];
	temp->stat_ch_ocf = rx_buf[10];
	temp->stat_ch_ext_ocf = rx_buf[12];

	temp->ocf_cnt_ch00 = rx_buf[14];
	temp->ocf_cnt_ch01 = rx_buf[16];
	temp->ocf_cnt_ch02 = rx_buf[18];
	temp->ocf_cnt_ch03 = rx_buf[20];
	temp->ocf_cnt_ch04 = rx_buf[22];
	temp->ocf_cnt_ch05 = rx_buf[24];
	temp->ocf_cnt_ch06 = rx_buf[26];
	temp->ocf_cnt_ch07 = rx_buf[28];
	temp->ocf_cnt_ch08 = rx_buf[30];
	temp->ocf_cnt_ch09 = rx_buf[32];
	temp->ocf_cnt_ch10 = rx_buf[34];
	temp->ocf_cnt_ch11 = rx_buf[36];
	temp->ocf_cnt_ch12 = rx_buf[38];
	temp->ocf_cnt_ch13 = rx_buf[40];
	temp->ocf_cnt_ch14 = rx_buf[42];
	temp->ocf_cnt_ch15 = rx_buf[44];
	temp->ocf_cnt_ch16 = rx_buf[46];
	temp->ocf_cnt_ch17 = rx_buf[48];
	temp->ocf_cnt_ch18 = rx_buf[50];
	temp->ocf_cnt_ch19 = rx_buf[52];
	temp->ocf_cnt_ch20 = rx_buf[54];
	temp->ocf_cnt_ch21 = rx_buf[56];
	temp->ocf_cnt_ch22 = rx_buf[58];
	temp->ocf_cnt_ch23 = rx_buf[60];
	temp->ocf_cnt_ch24 = rx_buf[62];
	temp->ocf_cnt_ch25 = rx_buf[64];
	temp->ocf_cnt_ch26 = rx_buf[66];
	temp->ocf_cnt_ch27 = rx_buf[68];
	temp->ocf_cnt_ch28 = rx_buf[70];
	temp->ocf_cnt_ch29 = rx_buf[72];
	temp->ocf_cnt_ch30 = rx_buf[74];
	temp->ocf_cnt_ch31 = rx_buf[76];

}


//__________________________________________________________________________________________________

void eps_get_pbu_abf_placed_state(PBU_ABF_PS* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x44;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[8];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 8, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->ABF_Placed_0 = rx_buf[6];
	temp->ABF_Placed_1 = rx_buf[7];


}

//_________________________________________________________________________________________________



void eps_get_pdu_housekeeping_data_raw(PDU_HK_D* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x50;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[258];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 258, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP_MCU = rx_buf[8];
	temp->VIP_INPUT.vipd_array[0] = rx_buf[10];
	temp->STAT_CH_ON = rx_buf[16];
	temp->STAT_CH_EXT_ON = rx_buf[18];
	temp->STAT_CH_OCF = rx_buf[20];
	temp->STAT_CH_EXT_OCF = rx_buf[22];

	temp->VIP_VD0.vipd_array[0] = rx_buf[24];
	temp->VIP_VD1.vipd_array[0] = rx_buf[30];
	temp->VIP_VD2.vipd_array[0] = rx_buf[36];
	temp->VIP_VD3.vipd_array[0] = rx_buf[42];
	temp->VIP_VD4.vipd_array[0] = rx_buf[48];
	temp->VIP_VD5.vipd_array[0] = rx_buf[54];
	temp->VIP_VD6.vipd_array[0] = rx_buf[60];

	temp->VIP_CH00.vipd_array[0] = rx_buf[66];
	temp->VIP_CH01.vipd_array[0] = rx_buf[72];
	temp->VIP_CH02.vipd_array[0] = rx_buf[78];
	temp->VIP_CH03.vipd_array[0] = rx_buf[84];
	temp->VIP_CH04.vipd_array[0] = rx_buf[90];
	temp->VIP_CH05.vipd_array[0] = rx_buf[96];
	temp->VIP_CH06.vipd_array[0] = rx_buf[102];
	temp->VIP_CH07.vipd_array[0] = rx_buf[108];
	temp->VIP_CH08.vipd_array[0] = rx_buf[114];
	temp->VIP_CH09.vipd_array[0] = rx_buf[120];
	temp->VIP_CH10.vipd_array[0] = rx_buf[126];
	temp->VIP_CH11.vipd_array[0] = rx_buf[132];
	temp->VIP_CH12.vipd_array[0] = rx_buf[138];
	temp->VIP_CH13.vipd_array[0] = rx_buf[144];
	temp->VIP_CH14.vipd_array[0] = rx_buf[150];
	temp->VIP_CH15.vipd_array[0] = rx_buf[156];
	temp->VIP_CH16.vipd_array[0] = rx_buf[162];
	temp->VIP_CH17.vipd_array[0] = rx_buf[168];
	temp->VIP_CH18.vipd_array[0] = rx_buf[174];
	temp->VIP_CH19.vipd_array[0] = rx_buf[180];
	temp->VIP_CH20.vipd_array[0] = rx_buf[186];
	temp->VIP_CH21.vipd_array[0] = rx_buf[192];
	temp->VIP_CH22.vipd_array[0] = rx_buf[198];
	temp->VIP_CH23.vipd_array[0] = rx_buf[204];
	temp->VIP_CH24.vipd_array[0] = rx_buf[210];
	temp->VIP_CH25.vipd_array[0] = rx_buf[216];
	temp->VIP_CH26.vipd_array[0] = rx_buf[222];
	temp->VIP_CH27.vipd_array[0] = rx_buf[228];
	temp->VIP_CH28.vipd_array[0] = rx_buf[234];
	temp->VIP_CH29.vipd_array[0] = rx_buf[240];
	temp->VIP_CH30.vipd_array[0] = rx_buf[246];
	temp->VIP_CH31.vipd_array[0] = rx_buf[252];

}

//_________________________________________________________________________________________________

void eps_get_pdu_housekeeping_data_eng(PDU_HK_D* temp) {
	// We'll be using this function

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x52;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[258];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 258, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP_MCU = rx_buf[12];
	temp->VIP_INPUT.vipd_array[0] = rx_buf[16];
	temp->STAT_CH_ON = rx_buf[16];
	temp->STAT_CH_EXT_ON = rx_buf[18];
	temp->STAT_CH_OCF = rx_buf[20];
	temp->STAT_CH_EXT_OCF = rx_buf[22];

	temp->VIP_VD0.vipd_array[0] = rx_buf[24];
	temp->VIP_VD1.vipd_array[0] = rx_buf[30];
	temp->VIP_VD2.vipd_array[0] = rx_buf[36];
	temp->VIP_VD3.vipd_array[0] = rx_buf[42];
	temp->VIP_VD4.vipd_array[0] = rx_buf[48];
	temp->VIP_VD5.vipd_array[0] = rx_buf[54];
	temp->VIP_VD6.vipd_array[0] = rx_buf[60];

	temp->VIP_CH00.vipd_array[0] = rx_buf[66];
	temp->VIP_CH01.vipd_array[0] = rx_buf[72];
	temp->VIP_CH02.vipd_array[0] = rx_buf[78];
	temp->VIP_CH03.vipd_array[0] = rx_buf[84];
	temp->VIP_CH04.vipd_array[0] = rx_buf[90];
	temp->VIP_CH05.vipd_array[0] = rx_buf[96];
	temp->VIP_CH06.vipd_array[0] = rx_buf[102];
	temp->VIP_CH07.vipd_array[0] = rx_buf[108];
	temp->VIP_CH08.vipd_array[0] = rx_buf[114];
	temp->VIP_CH09.vipd_array[0] = rx_buf[120];
	temp->VIP_CH10.vipd_array[0] = rx_buf[126];
	temp->VIP_CH11.vipd_array[0] = rx_buf[132];
	temp->VIP_CH12.vipd_array[0] = rx_buf[138];
	temp->VIP_CH13.vipd_array[0] = rx_buf[144];
	temp->VIP_CH14.vipd_array[0] = rx_buf[150];
	temp->VIP_CH15.vipd_array[0] = rx_buf[156];
	temp->VIP_CH16.vipd_array[0] = rx_buf[162];
	temp->VIP_CH17.vipd_array[0] = rx_buf[168];
	temp->VIP_CH18.vipd_array[0] = rx_buf[174];
	temp->VIP_CH19.vipd_array[0] = rx_buf[180];
	temp->VIP_CH20.vipd_array[0] = rx_buf[186];
	temp->VIP_CH21.vipd_array[0] = rx_buf[192];
	temp->VIP_CH22.vipd_array[0] = rx_buf[198];
	temp->VIP_CH23.vipd_array[0] = rx_buf[204];
	temp->VIP_CH24.vipd_array[0] = rx_buf[210];
	temp->VIP_CH25.vipd_array[0] = rx_buf[216];
	temp->VIP_CH26.vipd_array[0] = rx_buf[222];
	temp->VIP_CH27.vipd_array[0] = rx_buf[228];
	temp->VIP_CH28.vipd_array[0] = rx_buf[234];
	temp->VIP_CH29.vipd_array[0] = rx_buf[240];
	temp->VIP_CH30.vipd_array[0] = rx_buf[246];
	temp->VIP_CH31.vipd_array[0] = rx_buf[252];

}

//__________________________________________________________________________________________________________________


void eps_get_pdu_housekeeping_data_running_average(PDU_HK_D* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x54;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[258];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 258, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP_MCU = rx_buf[12];
	temp->VIP_INPUT.vipd_array[0] = rx_buf[16];
	temp->STAT_CH_ON = rx_buf[16];
	temp->STAT_CH_EXT_ON = rx_buf[18];
	temp->STAT_CH_OCF = rx_buf[20];
	temp->STAT_CH_EXT_OCF = rx_buf[22];

	temp->VIP_VD0.vipd_array[0] = rx_buf[24];
	temp->VIP_VD1.vipd_array[0] = rx_buf[30];
	temp->VIP_VD2.vipd_array[0] = rx_buf[36];
	temp->VIP_VD3.vipd_array[0] = rx_buf[42];
	temp->VIP_VD4.vipd_array[0] = rx_buf[48];
	temp->VIP_VD5.vipd_array[0] = rx_buf[54];
	temp->VIP_VD6.vipd_array[0] = rx_buf[60];

	temp->VIP_CH00.vipd_array[0] = rx_buf[66];
	temp->VIP_CH01.vipd_array[0] = rx_buf[72];
	temp->VIP_CH02.vipd_array[0] = rx_buf[78];
	temp->VIP_CH03.vipd_array[0] = rx_buf[84];
	temp->VIP_CH04.vipd_array[0] = rx_buf[90];
	temp->VIP_CH05.vipd_array[0] = rx_buf[96];
	temp->VIP_CH06.vipd_array[0] = rx_buf[102];
	temp->VIP_CH07.vipd_array[0] = rx_buf[108];
	temp->VIP_CH08.vipd_array[0] = rx_buf[114];
	temp->VIP_CH09.vipd_array[0] = rx_buf[120];
	temp->VIP_CH10.vipd_array[0] = rx_buf[126];
	temp->VIP_CH11.vipd_array[0] = rx_buf[132];
	temp->VIP_CH12.vipd_array[0] = rx_buf[138];
	temp->VIP_CH13.vipd_array[0] = rx_buf[144];
	temp->VIP_CH14.vipd_array[0] = rx_buf[150];
	temp->VIP_CH15.vipd_array[0] = rx_buf[156];
	temp->VIP_CH16.vipd_array[0] = rx_buf[162];
	temp->VIP_CH17.vipd_array[0] = rx_buf[168];
	temp->VIP_CH18.vipd_array[0] = rx_buf[174];
	temp->VIP_CH19.vipd_array[0] = rx_buf[180];
	temp->VIP_CH20.vipd_array[0] = rx_buf[186];
	temp->VIP_CH21.vipd_array[0] = rx_buf[192];
	temp->VIP_CH22.vipd_array[0] = rx_buf[198];
	temp->VIP_CH23.vipd_array[0] = rx_buf[204];
	temp->VIP_CH24.vipd_array[0] = rx_buf[210];
	temp->VIP_CH25.vipd_array[0] = rx_buf[216];
	temp->VIP_CH26.vipd_array[0] = rx_buf[222];
	temp->VIP_CH27.vipd_array[0] = rx_buf[228];
	temp->VIP_CH28.vipd_array[0] = rx_buf[234];
	temp->VIP_CH29.vipd_array[0] = rx_buf[240];
	temp->VIP_CH30.vipd_array[0] = rx_buf[246];
	temp->VIP_CH31.vipd_array[0] = rx_buf[252];

}

//_____________________________________________________________________________________________________________________________

void eps_get_pbu_housekeeping_data_raw(PBU_HK_D* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x60;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[84];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 84, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP_MCU = rx_buf[8];
	temp->VIP_INPUT.vipd_array[0] = rx_buf[10];
	temp->STAT_BU = rx_buf[16];

	temp->BP1.VIP_BP_Input.vipd_array[0] = rx_buf[18];
	temp->BP1.STAT_BP = rx_buf[24];
	temp->BP1.VOLT_CELL1 = rx_buf[26];
	temp->BP1.VOLT_CELL2 = rx_buf[28];
	temp->BP1.VOLT_CELL3 = rx_buf[30];
	temp->BP1.VOLT_CELL4 = rx_buf[32];
	temp->BP1.BAT_TEMP1 = rx_buf[34];
	temp->BP1.BAT_TEMP2 = rx_buf[36];
	temp->BP1.BAT_TEMP3 = rx_buf[38];


	temp->BP2.VIP_BP_Input.vipd_array[0] = rx_buf[40];
	temp->BP2.STAT_BP = rx_buf[46];
	temp->BP2.VOLT_CELL1 = rx_buf[48];
	temp->BP2.VOLT_CELL2 = rx_buf[50];
	temp->BP2.VOLT_CELL3 = rx_buf[52];
	temp->BP2.VOLT_CELL4 = rx_buf[54];
	temp->BP2.BAT_TEMP1 = rx_buf[56];
	temp->BP2.BAT_TEMP2 = rx_buf[58];
	temp->BP2.BAT_TEMP3 = rx_buf[60];


	temp->BP3.VIP_BP_Input.vipd_array[0] = rx_buf[62];
	temp->BP3.STAT_BP = rx_buf[68];
	temp->BP3.VOLT_CELL1 = rx_buf[70];
	temp->BP3.VOLT_CELL2 = rx_buf[72];
	temp->BP3.VOLT_CELL3 = rx_buf[74];
	temp->BP3.VOLT_CELL4 = rx_buf[76];
	temp->BP3.BAT_TEMP1 = rx_buf[78];
	temp->BP3.BAT_TEMP2 = rx_buf[80];
	temp->BP3.BAT_TEMP3 = rx_buf[82];


}

//_______________________________________________________________________________________________

void eps_get_pbu_housekeeping_data_eng(PBU_HK_D* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x62;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[84];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 84, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP_MCU = rx_buf[12];
	temp->VIP_INPUT.vipd_array[0] = rx_buf[10];
	temp->STAT_BU = rx_buf[16];

	temp->BP1.VIP_BP_Input.vipd_array[0] = rx_buf[18];
	temp->BP1.STAT_BP = rx_buf[24];
	temp->BP1.VOLT_CELL1 = rx_buf[26];
	temp->BP1.VOLT_CELL2 = rx_buf[28];
	temp->BP1.VOLT_CELL3 = rx_buf[30];
	temp->BP1.VOLT_CELL4 = rx_buf[32];
	temp->BP1.BAT_TEMP1 = rx_buf[34];
	temp->BP1.BAT_TEMP2 = rx_buf[36];
	temp->BP1.BAT_TEMP3 = rx_buf[38];


	temp->BP2.VIP_BP_Input.vipd_array[0] = rx_buf[40];
	temp->BP2.STAT_BP = rx_buf[46];
	temp->BP2.VOLT_CELL1 = rx_buf[48];
	temp->BP2.VOLT_CELL2 = rx_buf[50];
	temp->BP2.VOLT_CELL3 = rx_buf[52];
	temp->BP2.VOLT_CELL4 = rx_buf[54];
	temp->BP2.BAT_TEMP1 = rx_buf[56];
	temp->BP2.BAT_TEMP2 = rx_buf[58];
	temp->BP2.BAT_TEMP3 = rx_buf[60];


	temp->BP3.VIP_BP_Input.vipd_array[0] = rx_buf[62];
	temp->BP3.STAT_BP = rx_buf[68];
	temp->BP3.VOLT_CELL1 = rx_buf[70];
	temp->BP3.VOLT_CELL2 = rx_buf[72];
	temp->BP3.VOLT_CELL3 = rx_buf[74];
	temp->BP3.VOLT_CELL4 = rx_buf[76];
	temp->BP3.BAT_TEMP1 = rx_buf[78];
	temp->BP3.BAT_TEMP2 = rx_buf[80];
	temp->BP3.BAT_TEMP3 = rx_buf[82];

}

//________________________________________________________________________________________

void eps_get_pbu_housekeeping_data_running_average(PBU_HK_D* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x64;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[84];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 84, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP_MCU = rx_buf[12];
	temp->VIP_INPUT.vipd_array[0] = rx_buf[10];
	temp->STAT_BU = rx_buf[16];

	temp->status = rx_buf[4];
	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP_MCU = rx_buf[12];
	temp->VIP_INPUT.vipd_array[0] = rx_buf[10];
	temp->STAT_BU = rx_buf[16];

	temp->BP1.VIP_BP_Input.vipd_array[0] = rx_buf[18];
	temp->BP1.STAT_BP = rx_buf[24];
	temp->BP1.VOLT_CELL1 = rx_buf[26];
	temp->BP1.VOLT_CELL2 = rx_buf[28];
	temp->BP1.VOLT_CELL3 = rx_buf[30];
	temp->BP1.VOLT_CELL4 = rx_buf[32];
	temp->BP1.BAT_TEMP1 = rx_buf[34];
	temp->BP1.BAT_TEMP2 = rx_buf[36];
	temp->BP1.BAT_TEMP3 = rx_buf[38];


	temp->BP2.VIP_BP_Input.vipd_array[0] = rx_buf[40];
	temp->BP2.STAT_BP = rx_buf[46];
	temp->BP2.VOLT_CELL1 = rx_buf[48];
	temp->BP2.VOLT_CELL2 = rx_buf[50];
	temp->BP2.VOLT_CELL3 = rx_buf[52];
	temp->BP2.VOLT_CELL4 = rx_buf[54];
	temp->BP2.BAT_TEMP1 = rx_buf[56];
	temp->BP2.BAT_TEMP2 = rx_buf[58];
	temp->BP2.BAT_TEMP3 = rx_buf[60];


	temp->BP3.VIP_BP_Input.vipd_array[0] = rx_buf[62];
	temp->BP3.STAT_BP = rx_buf[68];
	temp->BP3.VOLT_CELL1 = rx_buf[70];
	temp->BP3.VOLT_CELL2 = rx_buf[72];
	temp->BP3.VOLT_CELL3 = rx_buf[74];
	temp->BP3.VOLT_CELL4 = rx_buf[76];
	temp->BP3.BAT_TEMP1 = rx_buf[78];
	temp->BP3.BAT_TEMP2 = rx_buf[80];
	temp->BP3.BAT_TEMP3 = rx_buf[82];


}

//_________________________________________________________________________________________________

void eps_get_pcu_housekeeping_data_raw(PCU_HK_D* temp){


	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x70;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[72];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 72, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP_MCU = rx_buf[8];
	temp->VIP_OUTPUT.vipd_array[0] = rx_buf[10];

	temp->CC1.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[16];
	temp->CC1.VOLT_IN_MPPT = rx_buf[22];
	temp->CC1.CURR_IN_MPPT = rx_buf[24];
	temp->CC1.VOLT_OU_MPPT = rx_buf[26];
	temp->CC1.CURR_OU_MPPT = rx_buf[28];

	temp->CC2.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[30];
	temp->CC2.VOLT_IN_MPPT = rx_buf[36];
	temp->CC2.CURR_IN_MPPT = rx_buf[38];
	temp->CC2.VOLT_OU_MPPT = rx_buf[40];
	temp->CC2.CURR_OU_MPPT = rx_buf[42];

	temp->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[44];
	temp->CC3.VOLT_IN_MPPT = rx_buf[50];
	temp->CC3.CURR_IN_MPPT = rx_buf[52];
	temp->CC3.VOLT_OU_MPPT = rx_buf[54];
	temp->CC3.CURR_OU_MPPT = rx_buf[56];

	temp->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[58];
	temp->CC3.VOLT_IN_MPPT = rx_buf[64];
	temp->CC3.CURR_IN_MPPT = rx_buf[66];
	temp->CC3.VOLT_OU_MPPT = rx_buf[68];
	temp->CC3.CURR_OU_MPPT = rx_buf[70];

}

//_______________________________________________________________________________________

void eps_get_pcu_housekeeping_data_eng(PCU_HK_D* temp){


	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x72;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[72];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 72, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP_MCU = rx_buf[8];
	temp->VIP_OUTPUT.vipd_array[0] = rx_buf[10];

	temp->CC1.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[16];
	temp->CC1.VOLT_IN_MPPT = rx_buf[22];
	temp->CC1.CURR_IN_MPPT = rx_buf[24];
	temp->CC1.VOLT_OU_MPPT = rx_buf[26];
	temp->CC1.CURR_OU_MPPT = rx_buf[28];

	temp->CC2.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[30];
	temp->CC2.VOLT_IN_MPPT = rx_buf[36];
	temp->CC2.CURR_IN_MPPT = rx_buf[38];
	temp->CC2.VOLT_OU_MPPT = rx_buf[40];
	temp->CC2.CURR_OU_MPPT = rx_buf[42];

	temp->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[44];
	temp->CC3.VOLT_IN_MPPT = rx_buf[50];
	temp->CC3.CURR_IN_MPPT = rx_buf[52];
	temp->CC3.VOLT_OU_MPPT = rx_buf[54];
	temp->CC3.CURR_OU_MPPT = rx_buf[56];

	temp->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[58];
	temp->CC3.VOLT_IN_MPPT = rx_buf[64];
	temp->CC3.CURR_IN_MPPT = rx_buf[66];
	temp->CC3.VOLT_OU_MPPT = rx_buf[68];
	temp->CC3.CURR_OU_MPPT = rx_buf[70];
}

//_______________________________________________________________________________________

void eps_get_pcu_housekeeping_data_running_average(PCU_HK_D* temp){


	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x74;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[72];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 72, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP_MCU = rx_buf[8];
	temp->VIP_OUTPUT.vipd_array[0] = rx_buf[10];

	temp->CC1.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[16];
	temp->CC1.VOLT_IN_MPPT = rx_buf[22];
	temp->CC1.CURR_IN_MPPT = rx_buf[24];
	temp->CC1.VOLT_OU_MPPT = rx_buf[26];
	temp->CC1.CURR_OU_MPPT = rx_buf[28];

	temp->CC2.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[30];
	temp->CC2.VOLT_IN_MPPT = rx_buf[36];
	temp->CC2.CURR_IN_MPPT = rx_buf[38];
	temp->CC2.VOLT_OU_MPPT = rx_buf[40];
	temp->CC2.CURR_OU_MPPT = rx_buf[42];

	temp->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[44];
	temp->CC3.VOLT_IN_MPPT = rx_buf[50];
	temp->CC3.CURR_IN_MPPT = rx_buf[52];
	temp->CC3.VOLT_OU_MPPT = rx_buf[54];
	temp->CC3.CURR_OU_MPPT = rx_buf[56];

	temp->CC3.VIP_CC_OUTPUT.vipd_array[0] = rx_buf[58];
	temp->CC3.VOLT_IN_MPPT = rx_buf[64];
	temp->CC3.CURR_IN_MPPT = rx_buf[66];
	temp->CC3.VOLT_OU_MPPT = rx_buf[68];
	temp->CC3.CURR_OU_MPPT = rx_buf[70];

}

//----------------------------------------------------------------------------------------------------

void eps_get_configuration_parameter(GET_CONFIG_PARAM* temp, uint16_t PAR_ID){


	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x82;
	uint8_t BID = 0x20;


	uint8_t cmd_buf[6];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	cmd_buf[4] = PAR_ID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 6, HAL_MAX_DELAY);

	uint8_t rx_buf[16];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 16, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->PAR_ID = rx_buf[6];
	temp->PAR_VAL = rx_buf[8];

}

//-----------------------------------------------------------------------------------------------------
void eps_set_configuration_parameter(SET_CONFIG_PARAM* temp, uint16_t PAR_ID, uint8_t PAR_VAL){


	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x84;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[14];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[3] = CC;
	cmd_buf[2] = BID;

	cmd_buf[4] = PAR_ID;
	cmd_buf[6] = PAR_VAL;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 14, HAL_MAX_DELAY);

	uint8_t rx_buf[16];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 16, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->PAR_ID = rx_buf[6];
	temp->PAR_VAL = rx_buf[8];

}

//-----------------------------------------------------------------------------------------------------

void eps_reset_configuration_parameter(RESET_CONFIG_PAR* temp, uint16_t PAR_ID){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x86;
	uint8_t BID = 0x20;


	uint8_t cmd_buf[6];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	cmd_buf[4] = PAR_ID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 6, HAL_MAX_DELAY);

	uint8_t rx_buf[16];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 16, HAL_MAX_DELAY);

	temp->status = rx_buf[4];
	temp->PAR_ID = rx_buf[6];
	temp->PAR_VAL = rx_buf[8];
}

//---------------------------------------------------------------------------------------------------------

void eps_reset_configuration(RESET_CONFIGURATION* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x90;
	uint8_t BID = 0x20;

	uint8_t CONF_KEY = 0x87;

	uint8_t cmd_buf[5];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	cmd_buf[4] = CONF_KEY;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 5, HAL_MAX_DELAY);

	uint8_t rx_buf[16];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 16, HAL_MAX_DELAY);

	temp->status = rx_buf[4];

}

//---------------------------------------------------------------------------------------------------

void eps_load_configuration(LOAD_CONFIGURATION* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x92;
	uint8_t BID = 0x20;

	uint8_t CONF_KEY = 0xA7;

	uint8_t cmd_buf[5];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	cmd_buf[4] = CONF_KEY;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 5, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	temp->status = rx_buf[4];

}

//---------------------------------------------------------------------------------------------

void eps_save_configuration(SAVE_CONFIGURATION* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0x94;
	uint8_t BID = 0x20;

	uint8_t CONF_KEY = 0xA7;
	uint16_t CHECKSUM = 0;

	uint8_t cmd_buf[7];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	cmd_buf[4] = CONF_KEY;
	cmd_buf[5] = CHECKSUM;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 7, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	temp->status = rx_buf[4];

}

//------------------------------------------------------------------------------------------------------------------

void eps_get_piu_housekeeping_data_raw(GET_PIU_HK* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0xA0;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[274];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 274, HAL_MAX_DELAY);

	temp->status = rx_buf[4];


	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP = rx_buf[8];
	temp->VIP_DIST_INPUT.vipd_array[0] = rx_buf[10];
	temp->VIP_BATT_INPUT.vipd_array[0] = rx_buf[16];
	temp->SAT_CH_ON = rx_buf[22];
	temp->STAT_CH_OCF = rx_buf[24];

	temp->BAT_STAT = rx_buf[26];

	temp->BAT_TEMP2 = rx_buf[28];
	temp->BAT_TEMP3 = rx_buf[30];

	temp->VOLT_VD0 = rx_buf[32];
	temp->VOLT_VD1 = rx_buf[34];
	temp->VOLT_VD2 = rx_buf[36];

	temp->VIP_CH00.vipd_array[0] = rx_buf[38];
	temp->VIP_CH01.vipd_array[0] = rx_buf[44];
	temp->VIP_CH02.vipd_array[0] = rx_buf[50];
	temp->VIP_CH03.vipd_array[0] = rx_buf[56];
	temp->VIP_CH04.vipd_array[0] = rx_buf[62];
	temp->VIP_CH05.vipd_array[0] = rx_buf[68];
	temp->VIP_CH06.vipd_array[0] = rx_buf[74];
	temp->VIP_CH07.vipd_array[0] = rx_buf[80];
	temp->VIP_CH08.vipd_array[0] = rx_buf[86];

	temp->CC1.VOLT_IN_MPPT = rx_buf[92];
	temp->CC1.CURR_IN_MPPT = rx_buf[94];
	temp->CC1.VOLT_OU_MPPT = rx_buf[96];
	temp->CC1.VOLT_OU_MPPT = rx_buf[98];

	temp->CC2.VOLT_IN_MPPT = rx_buf[100];
	temp->CC2.CURR_IN_MPPT = rx_buf[102];
	temp->CC2.VOLT_OU_MPPT = rx_buf[104];
	temp->CC2.VOLT_OU_MPPT = rx_buf[106];

	temp->CC3.VOLT_IN_MPPT = rx_buf[108];
	temp->CC3.CURR_IN_MPPT = rx_buf[110];
	temp->CC3.VOLT_OU_MPPT = rx_buf[112];
	temp->CC3.VOLT_OU_MPPT = rx_buf[114];

	temp->VIP_CH09.vipd_array[0] = rx_buf[116];
	temp->VIP_CH10.vipd_array[0] = rx_buf[122];
	temp->VIP_CH11.vipd_array[0] = rx_buf[128];
	temp->VIP_CH12.vipd_array[0] = rx_buf[134];
	temp->VIP_CH13.vipd_array[0] = rx_buf[140];
	temp->VIP_CH14.vipd_array[0] = rx_buf[146];
	temp->VIP_CH15.vipd_array[0] = rx_buf[152];

	temp->CC4.VOLT_IN_MPPT = rx_buf[158];
	temp->CC4.CURR_IN_MPPT = rx_buf[160];
	temp->CC4.VOLT_OU_MPPT = rx_buf[162];
	temp->CC4.VOLT_OU_MPPT = rx_buf[164];

	temp->CC5.VOLT_IN_MPPT = rx_buf[166];
	temp->CC5.CURR_IN_MPPT = rx_buf[168];
	temp->CC5.VOLT_OU_MPPT = rx_buf[170];
	temp->CC5.VOLT_OU_MPPT = rx_buf[172];

	temp->STAT_CH_EXT_ON = rx_buf[174];
	temp->STAT_CH_EXT_OCF = rx_buf[176];

	temp->VIP_CH16.vipd_array[0] = rx_buf[178];
	temp->VIP_CH17.vipd_array[0] = rx_buf[184];
	temp->VIP_CH18.vipd_array[0] = rx_buf[190];
	temp->VIP_CH19.vipd_array[0] = rx_buf[196];
	temp->VIP_CH20.vipd_array[0] = rx_buf[202];
	temp->VIP_CH21.vipd_array[0] = rx_buf[208];
	temp->VIP_CH22.vipd_array[0] = rx_buf[214];
	temp->VIP_CH23.vipd_array[0] = rx_buf[220];
	temp->VIP_CH24.vipd_array[0] = rx_buf[226];
	temp->VIP_CH25.vipd_array[0] = rx_buf[232];
	temp->VIP_CH26.vipd_array[0] = rx_buf[238];
	temp->VIP_CH27.vipd_array[0] = rx_buf[244];
	temp->VIP_CH28.vipd_array[0] = rx_buf[250];
	temp->VIP_CH29.vipd_array[0] = rx_buf[256];
	temp->VIP_CH30.vipd_array[0] = rx_buf[262];
	temp->VIP_CH31.vipd_array[0] = rx_buf[268];

}

//-----------------------------------------------------------------------------------------------------

void eps_get_piu_housekeeping_data_eng(GET_PIU_HK* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0xA2;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[274];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 274, HAL_MAX_DELAY);

	temp->status = rx_buf[4];


	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP = rx_buf[8];
	temp->VIP_DIST_INPUT.vipd_array[0] = rx_buf[10];
	temp->VIP_BATT_INPUT.vipd_array[0] = rx_buf[16];
	temp->SAT_CH_ON = rx_buf[22];
	temp->STAT_CH_OCF = rx_buf[24];

	temp->BAT_STAT = rx_buf[26];

	temp->BAT_TEMP2 = rx_buf[28];
	temp->BAT_TEMP3 = rx_buf[30];

	temp->VOLT_VD0 = rx_buf[32];
	temp->VOLT_VD1 = rx_buf[34];
	temp->VOLT_VD2 = rx_buf[36];

	temp->VIP_CH00.vipd_array[0] = rx_buf[38];
	temp->VIP_CH01.vipd_array[0] = rx_buf[44];
	temp->VIP_CH02.vipd_array[0] = rx_buf[50];
	temp->VIP_CH03.vipd_array[0] = rx_buf[56];
	temp->VIP_CH04.vipd_array[0] = rx_buf[62];
	temp->VIP_CH05.vipd_array[0] = rx_buf[68];
	temp->VIP_CH06.vipd_array[0] = rx_buf[74];
	temp->VIP_CH07.vipd_array[0] = rx_buf[80];
	temp->VIP_CH08.vipd_array[0] = rx_buf[86];

	temp->CC1.VOLT_IN_MPPT = rx_buf[92];
	temp->CC1.CURR_IN_MPPT = rx_buf[94];
	temp->CC1.VOLT_OU_MPPT = rx_buf[96];
	temp->CC1.VOLT_OU_MPPT = rx_buf[98];

	temp->CC2.VOLT_IN_MPPT = rx_buf[100];
	temp->CC2.CURR_IN_MPPT = rx_buf[102];
	temp->CC2.VOLT_OU_MPPT = rx_buf[104];
	temp->CC2.VOLT_OU_MPPT = rx_buf[106];

	temp->CC3.VOLT_IN_MPPT = rx_buf[108];
	temp->CC3.CURR_IN_MPPT = rx_buf[110];
	temp->CC3.VOLT_OU_MPPT = rx_buf[112];
	temp->CC3.VOLT_OU_MPPT = rx_buf[114];

	temp->VIP_CH09.vipd_array[0] = rx_buf[116];
	temp->VIP_CH10.vipd_array[0] = rx_buf[122];
	temp->VIP_CH11.vipd_array[0] = rx_buf[128];
	temp->VIP_CH12.vipd_array[0] = rx_buf[134];
	temp->VIP_CH13.vipd_array[0] = rx_buf[140];
	temp->VIP_CH14.vipd_array[0] = rx_buf[146];
	temp->VIP_CH15.vipd_array[0] = rx_buf[152];

	temp->CC4.VOLT_IN_MPPT = rx_buf[158];
	temp->CC4.CURR_IN_MPPT = rx_buf[160];
	temp->CC4.VOLT_OU_MPPT = rx_buf[162];
	temp->CC4.VOLT_OU_MPPT = rx_buf[164];

	temp->CC5.VOLT_IN_MPPT = rx_buf[166];
	temp->CC5.CURR_IN_MPPT = rx_buf[168];
	temp->CC5.VOLT_OU_MPPT = rx_buf[170];
	temp->CC5.VOLT_OU_MPPT = rx_buf[172];

	temp->STAT_CH_EXT_ON = rx_buf[174];
	temp->STAT_CH_EXT_OCF = rx_buf[176];

	temp->VIP_CH16.vipd_array[0] = rx_buf[178];
	temp->VIP_CH17.vipd_array[0] = rx_buf[184];
	temp->VIP_CH18.vipd_array[0] = rx_buf[190];
	temp->VIP_CH19.vipd_array[0] = rx_buf[196];
	temp->VIP_CH20.vipd_array[0] = rx_buf[202];
	temp->VIP_CH21.vipd_array[0] = rx_buf[208];
	temp->VIP_CH22.vipd_array[0] = rx_buf[214];
	temp->VIP_CH23.vipd_array[0] = rx_buf[220];
	temp->VIP_CH24.vipd_array[0] = rx_buf[226];
	temp->VIP_CH25.vipd_array[0] = rx_buf[232];
	temp->VIP_CH26.vipd_array[0] = rx_buf[238];
	temp->VIP_CH27.vipd_array[0] = rx_buf[244];
	temp->VIP_CH28.vipd_array[0] = rx_buf[250];
	temp->VIP_CH29.vipd_array[0] = rx_buf[256];
	temp->VIP_CH30.vipd_array[0] = rx_buf[262];
	temp->VIP_CH31.vipd_array[0] = rx_buf[268];

}

//------------------------------------------------------------------------------------------

void eps_get_piu_housekeeping_data_running_average(GET_PIU_HK* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0xA4;
	uint8_t BID = 0x20;

	uint8_t cmd_buf[4];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 4, HAL_MAX_DELAY);

	uint8_t rx_buf[274];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 274, HAL_MAX_DELAY);

	temp->status = rx_buf[4];


	temp->VOLT_BRDSUP = rx_buf[6];
	temp->TEMP = rx_buf[8];
	temp->VIP_DIST_INPUT.vipd_array[0] = rx_buf[10];
	temp->VIP_BATT_INPUT.vipd_array[0] = rx_buf[16];
	temp->SAT_CH_ON = rx_buf[22];
	temp->STAT_CH_OCF = rx_buf[24];

	temp->BAT_STAT = rx_buf[26];

	temp->BAT_TEMP2 = rx_buf[28];
	temp->BAT_TEMP3 = rx_buf[30];

	temp->VOLT_VD0 = rx_buf[32];
	temp->VOLT_VD1 = rx_buf[34];
	temp->VOLT_VD2 = rx_buf[36];

	temp->VIP_CH00.vipd_array[0] = rx_buf[38];
	temp->VIP_CH01.vipd_array[0] = rx_buf[44];
	temp->VIP_CH02.vipd_array[0] = rx_buf[50];
	temp->VIP_CH03.vipd_array[0] = rx_buf[56];
	temp->VIP_CH04.vipd_array[0] = rx_buf[62];
	temp->VIP_CH05.vipd_array[0] = rx_buf[68];
	temp->VIP_CH06.vipd_array[0] = rx_buf[74];
	temp->VIP_CH07.vipd_array[0] = rx_buf[80];
	temp->VIP_CH08.vipd_array[0] = rx_buf[86];

	temp->CC1.VOLT_IN_MPPT = rx_buf[92];
	temp->CC1.CURR_IN_MPPT = rx_buf[94];
	temp->CC1.VOLT_OU_MPPT = rx_buf[96];
	temp->CC1.VOLT_OU_MPPT = rx_buf[98];

	temp->CC2.VOLT_IN_MPPT = rx_buf[100];
	temp->CC2.CURR_IN_MPPT = rx_buf[102];
	temp->CC2.VOLT_OU_MPPT = rx_buf[104];
	temp->CC2.VOLT_OU_MPPT = rx_buf[106];

	temp->CC3.VOLT_IN_MPPT = rx_buf[108];
	temp->CC3.CURR_IN_MPPT = rx_buf[110];
	temp->CC3.VOLT_OU_MPPT = rx_buf[112];
	temp->CC3.VOLT_OU_MPPT = rx_buf[114];

	temp->VIP_CH09.vipd_array[0] = rx_buf[116];
	temp->VIP_CH10.vipd_array[0] = rx_buf[122];
	temp->VIP_CH11.vipd_array[0] = rx_buf[128];
	temp->VIP_CH12.vipd_array[0] = rx_buf[134];
	temp->VIP_CH13.vipd_array[0] = rx_buf[140];
	temp->VIP_CH14.vipd_array[0] = rx_buf[146];
	temp->VIP_CH15.vipd_array[0] = rx_buf[152];

	temp->CC4.VOLT_IN_MPPT = rx_buf[158];
	temp->CC4.CURR_IN_MPPT = rx_buf[160];
	temp->CC4.VOLT_OU_MPPT = rx_buf[162];
	temp->CC4.VOLT_OU_MPPT = rx_buf[164];

	temp->CC5.VOLT_IN_MPPT = rx_buf[166];
	temp->CC5.CURR_IN_MPPT = rx_buf[168];
	temp->CC5.VOLT_OU_MPPT = rx_buf[170];
	temp->CC5.VOLT_OU_MPPT = rx_buf[172];

	temp->STAT_CH_EXT_ON = rx_buf[174];
	temp->STAT_CH_EXT_OCF = rx_buf[176];

	temp->VIP_CH16.vipd_array[0] = rx_buf[178];
	temp->VIP_CH17.vipd_array[0] = rx_buf[184];
	temp->VIP_CH18.vipd_array[0] = rx_buf[190];
	temp->VIP_CH19.vipd_array[0] = rx_buf[196];
	temp->VIP_CH20.vipd_array[0] = rx_buf[202];
	temp->VIP_CH21.vipd_array[0] = rx_buf[208];
	temp->VIP_CH22.vipd_array[0] = rx_buf[214];
	temp->VIP_CH23.vipd_array[0] = rx_buf[220];
	temp->VIP_CH24.vipd_array[0] = rx_buf[226];
	temp->VIP_CH25.vipd_array[0] = rx_buf[232];
	temp->VIP_CH26.vipd_array[0] = rx_buf[238];
	temp->VIP_CH27.vipd_array[0] = rx_buf[244];
	temp->VIP_CH28.vipd_array[0] = rx_buf[250];
	temp->VIP_CH29.vipd_array[0] = rx_buf[256];
	temp->VIP_CH30.vipd_array[0] = rx_buf[262];
	temp->VIP_CH31.vipd_array[0] = rx_buf[268];

}

//--------------------------------------------------------------------------------------------------

void eps_correct_time(CORRECT_TIME_S* temp, int32_t time_correction){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0xC4;
	uint8_t BID = 0x20;
	//Time correction in unix time (positive numbers added to time, negative values subtracted)
	int32_t CORRECTION = time_correction;

	uint8_t cmd_buf[8];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	cmd_buf[4] = CORRECTION;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 8, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	temp->status = rx_buf[4];

}

//------------------------------------------------------------------------------------------------------

void eps_zero_reset_cause_counters(ZERO_RESET_CAUSE_COUNTERS_S* temp){

	uint8_t STID = 0x1A;
	uint8_t IVID = 0x07;
	uint8_t CC = 0xC6;
	uint8_t BID = 0x20;
	//Time correction in unix time (positive numbers added to time, negative values subtracted)
	int32_t ZERO_KEY = 0xA7;

	uint8_t cmd_buf[5];

	cmd_buf[0] = STID;
	cmd_buf[1] = IVID;
	cmd_buf[2] = CC;
	cmd_buf[3] = BID;

	cmd_buf[4] = ZERO_KEY;

	HAL_I2C_Master_Transmit(&hi2c1, PWR_SYS_ADDR, cmd_buf, 5, HAL_MAX_DELAY);

	uint8_t rx_buf[5];

	HAL_I2C_Master_Receive(&hi2c1, PWR_SYS_ADDR, rx_buf, 5, HAL_MAX_DELAY);

	temp->status = rx_buf[4];

}








