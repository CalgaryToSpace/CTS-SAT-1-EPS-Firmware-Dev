/*
 * eps_driver.h
 *
 *  Created on: Nov 14, 2023
 *      Author: frank
 */

#ifndef INC_EPS_DRIVER_H_
#define INC_EPS_DRIVER_H_


#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Data type structs for select functions

typedef enum {
	EPS_CHANNEL_VBATT_STACK = 0, // CH0
	EPS_CHANNEL_5V_STACK = 1, // CH1
	EPS_CHANNEL_5V_CH2_UNUSED = 2, // CH2
	EPS_CHANNEL_5V_CH3_UNUSED = 3, // CH3
	EPS_CHANNEL_5V_CH4_UNUSED = 4, // CH4
	EPS_CHANNEL_3V3_STACK = 5, // CH5
	EPS_CHANNEL_3V3_CAMERA = 6, // CH6
	EPS_CHANNEL_3V3_UHF_Ante_Depl = 7, // CH7
	EPS_CHANNEL_3V3_LoRA_MODS = 8, // CH8
	EPS_CHANNEL_VBATT_CH9_UNUSED = 9, // CH9
	EPS_CHANNEL_VBATT_CH10_UNUSED = 10, // CH10
	EPS_CHANNEL_VBATT_CH11_UNUSED = 11, // CH11
	EPS_CHANNEL_12V_MPI = 12, // CH12
	EPS_CHANNEL_12V_BOOM = 13, // CH13
	EPS_CHANNEL_3V3_CH14_UNUSED = 14, // CH14
	EPS_CHANNEL_3V3_CH15_UNUSED = 15, // CH15
	EPS_CHANNEL_28V6_CH16_UNUSED = 16, // CH16
} EPS_CHANNEL_t;


void eps_debug_get_and_print_channel_stats(EPS_CHANNEL_t eps_channel);



// Typedef structs

typedef struct voltage_current_power_datatype {

	int16_t vipd_array[6]; // 6 bytes in this array. 2 bytes for voltage, 2 bytes for current, and 2 bytes for power.

} VIPD; // FIXME: this one is deprecated
// TODO: must rewrite this struct to have 3 elements


typedef struct {
	int16_t voltage_raw;
	int16_t current_raw;
	int16_t power_raw;
} VIPD_raw_t;

typedef struct {
	int16_t voltage_mV;
	int16_t current_mA;
	int16_t power_cW; // centiWatts (x 10^-2 watts)
} VIPD_eng_t;

typedef struct battery_pack_datatype {

	VIPD VIP_BP_Input; //To access this do VIP_BP_Input.vipd_array[i]
	uint16_t STAT_BP;
	uint16_t VOLT_CELL1;
	uint16_t VOLT_CELL2;
	uint16_t VOLT_CELL3;
	uint16_t VOLT_CELL4;
	uint16_t BAT_TEMP1;
	uint16_t BAT_TEMP2;
	uint16_t BAT_TEMP3;

} BPD;

typedef struct conditioning_channel_datatype {

	VIPD VIP_CC_OUTPUT; //To access this do VIP_BP_Input.vipd_array[i]
	uint16_t VOLT_IN_MPPT;
	uint16_t CURR_IN_MPPT;
	uint16_t VOLT_OU_MPPT;
	uint16_t CURR_OU_MPPT;


} CCD;



typedef struct {
	uint8_t mode;
	uint8_t conf;
	uint8_t reset_cause;
	uint32_t uptime_sec;
	uint16_t error;
	uint16_t rst_cnt_pwron;
	uint16_t rst_cnt_wdg;
	uint16_t rst_cnt_cmd;
	uint16_t rst_cnt_mcu;
	uint16_t rst_cnt_emlopo;
	uint16_t prevcmd_elapsed;
	uint32_t unix_time;
	uint8_t unix_year;
	uint8_t unix_month;
	uint8_t unix_day;
	uint8_t unix_hour;
	uint8_t unix_minute;
	uint8_t unix_second;
} eps_result_sys_stat_t;

typedef struct PDU_PIU_Overcurrent_Fault_State {

	uint8_t status;
	uint16_t stat_ch_on;
	uint16_t stat_ch_ext_on;
	uint16_t stat_ch_ocf;
	uint16_t stat_ch_ext_ocf;

	uint16_t ocf_cnt_ch00;
	uint16_t ocf_cnt_ch01;
	uint16_t ocf_cnt_ch02;
	uint16_t ocf_cnt_ch03;
	uint16_t ocf_cnt_ch04;
	uint16_t ocf_cnt_ch05;
	uint16_t ocf_cnt_ch06;
	uint16_t ocf_cnt_ch07;
	uint16_t ocf_cnt_ch08;
	uint16_t ocf_cnt_ch09;
	uint16_t ocf_cnt_ch10;
	uint16_t ocf_cnt_ch11;
	uint16_t ocf_cnt_ch12;
	uint16_t ocf_cnt_ch13;
	uint16_t ocf_cnt_ch14;
	uint16_t ocf_cnt_ch15;
	uint16_t ocf_cnt_ch16;
	uint16_t ocf_cnt_ch17;
	uint16_t ocf_cnt_ch18;
	uint16_t ocf_cnt_ch19;
	uint16_t ocf_cnt_ch20;
	uint16_t ocf_cnt_ch21;
	uint16_t ocf_cnt_ch22;
	uint16_t ocf_cnt_ch23;
	uint16_t ocf_cnt_ch24;
	uint16_t ocf_cnt_ch25;
	uint16_t ocf_cnt_ch26;
	uint16_t ocf_cnt_ch27;
	uint16_t ocf_cnt_ch28;
	uint16_t ocf_cnt_ch29;
	uint16_t ocf_cnt_ch30;
	uint16_t ocf_cnt_ch31;

} PDU_PIU_OFS;

typedef struct PBU_ABF_Placed_State {

	uint8_t status;

	uint8_t ABF_Placed_0;
	uint8_t ABF_Placed_1;

}  PBU_ABF_PS;

typedef struct PDU_Housekeeping_Data {

	uint8_t status;

	uint16_t VOLT_BRDSUP;
	uint16_t TEMP_MCU;

	VIPD VIP_INPUT;

	uint16_t STAT_CH_ON;
	uint16_t STAT_CH_EXT_ON;
	uint16_t STAT_CH_OCF;
	uint16_t STAT_CH_EXT_OCF;

	VIPD VIP_VD0;
	VIPD VIP_VD1;
	VIPD VIP_VD2;
	VIPD VIP_VD3;
	VIPD VIP_VD4;
	VIPD VIP_VD5;
	VIPD VIP_VD6;

	VIPD VIP_CH00;
	VIPD VIP_CH01;
	VIPD VIP_CH02;
	VIPD VIP_CH03;
	VIPD VIP_CH04;
	VIPD VIP_CH05;
	VIPD VIP_CH06;
	VIPD VIP_CH07;
	VIPD VIP_CH08;
	VIPD VIP_CH09;
	VIPD VIP_CH10;
	VIPD VIP_CH11;
	VIPD VIP_CH12;
	VIPD VIP_CH13;
	VIPD VIP_CH14;
	VIPD VIP_CH15;
	VIPD VIP_CH16;
	VIPD VIP_CH17;
	VIPD VIP_CH18;
	VIPD VIP_CH19;
	VIPD VIP_CH20;
	VIPD VIP_CH21;
	VIPD VIP_CH22;
	VIPD VIP_CH23;
	VIPD VIP_CH24;
	VIPD VIP_CH25;
	VIPD VIP_CH26;
	VIPD VIP_CH27;
	VIPD VIP_CH28;
	VIPD VIP_CH29;
	VIPD VIP_CH30;
	VIPD VIP_CH31;

} PDU_HK_D;

typedef struct PBU_Housekeeping_Data {
	uint8_t status;
	uint16_t VOLT_BRDSUP;
	uint16_t TEMP_MCU;
	VIPD VIP_INPUT;
	uint16_t STAT_BU;

	BPD BP1;
	BPD BP2;
	BPD BP3;
} PBU_HK_D;

typedef struct PCU_HOUSEKEEPING_DATA {
	uint8_t status;
	uint16_t VOLT_BRDSUP;
	uint16_t TEMP_MCU;
	VIPD VIP_OUTPUT;

	CCD CC1;
	CCD CC2;
	CCD CC3;
	CCD CC4;
} PCU_HK_D;


typedef struct GET_CONFIGURATION_PARAMETER {
	uint8_t status;
	uint16_t PAR_ID;
	uint8_t PAR_VAL;
} GET_CONFIG_PARAM;


typedef struct SET_CONFIGURATION_PARAMETER{
	uint8_t status;
	uint16_t PAR_ID;
	uint8_t PAR_VAL;
} SET_CONFIG_PARAM;


typedef struct RESET_CONFIGURATION_PARAMETER {
	uint8_t status;
	uint8_t PAR_ID;
	uint8_t PAR_VAL;
} RESET_CONFIG_PAR;


typedef struct RESET_CONFIGURATION {
	uint8_t status;
} RESET_CONFIGURATION;

typedef struct LOAD_CONFIGURATION {
	uint8_t status;
} LOAD_CONFIGURATION;


typedef struct SAVE_CONFIGURATION {
	uint8_t status;
} SAVE_CONFIGURATION;



typedef struct conditioning_channel_short_datatype {
	uint16_t VOLT_IN_MPPT;
	uint16_t CURR_IN_MPPT;
	uint16_t VOLT_OU_MPPT;
	uint16_t CURR_OU_MPPT;
} CCSD;

typedef struct GET_PIU_HK {
	uint8_t status;
	uint16_t VOLT_BRDSUP;
	uint16_t TEMP;
	VIPD VIP_DIST_INPUT;
	VIPD VIP_BATT_INPUT;
	uint16_t SAT_CH_ON;
	uint16_t STAT_CH_OCF;

	uint16_t BAT_STAT;

	uint16_t BAT_TEMP2;
	uint16_t BAT_TEMP3;

	uint16_t VOLT_VD0;
	uint16_t VOLT_VD1;
	uint16_t VOLT_VD2;

	VIPD VIP_CH00;
	VIPD VIP_CH01;
	VIPD VIP_CH02;
	VIPD VIP_CH03;
	VIPD VIP_CH04;
	VIPD VIP_CH05;
	VIPD VIP_CH06;
	VIPD VIP_CH07;
	VIPD VIP_CH08;

	CCSD CC1;
	CCSD CC2;
	CCSD CC3;

	VIPD VIP_CH09;
	VIPD VIP_CH10;
	VIPD VIP_CH11;
	VIPD VIP_CH12;
	VIPD VIP_CH13;
	VIPD VIP_CH14;
	VIPD VIP_CH15;

	CCSD CC4;
	CCSD CC5;

	uint16_t STAT_CH_EXT_ON;
	uint16_t STAT_CH_EXT_OCF;

	VIPD VIP_CH16;
	VIPD VIP_CH17;
	VIPD VIP_CH18;
	VIPD VIP_CH19;
	VIPD VIP_CH20;
	VIPD VIP_CH21;
	VIPD VIP_CH22;
	VIPD VIP_CH23;
	VIPD VIP_CH24;
	VIPD VIP_CH25;
	VIPD VIP_CH26;
	VIPD VIP_CH27;
	VIPD VIP_CH28;
	VIPD VIP_CH29;
	VIPD VIP_CH30;
	VIPD VIP_CH31;
} GET_PIU_HK;


typedef struct CORRECT_TIME_S {
	uint8_t status;
} CORRECT_TIME_S;



typedef struct ZERO_RESET_CAUSE_COUNTERS_S {
	uint8_t status;
} ZERO_RESET_CAUSE_COUNTERS_S;



void eps_debug_uart_print_sys_stat(eps_result_sys_stat_t* sys_stat);


void eps_system_reset();
// 20230406 Frank:
// - This function is used to send a system reset command to the ISIS Power system.
// - We are not excepting to receive anything useful back, or we may not even be able to poll the power system for a reply while it is resetting. Therefore its return type is "void".
// - CC indicates the type of command that will be sent to the power system. In this case, the user will be sending CC of "0xAA"


uint8_t eps_no_operation();
//20230416 Frank:
// This function is useful to check the functionality of the power system.


uint8_t eps_cancel_oper();
//20230416 Frank:
// This function switches off any command enable output bus channels.


void eps_watchdog();
//20230416 Frank:
// This function resets the watchdog timer, keeping the power system from resetting.


uint8_t eps_output_bus_group_on(uint16_t CH_BF,  uint16_t CH_EXT_BF);
// 20230415 Frank:
// The bits enabled as '1's in CH_BF are the ones that are switched on. A '0' means no action on the bus.
// If the busses 16-31 specified by CH_EXT_BF are not used, then the user will specify CH_EXT_BF as 0x00 to leave those busses unchanged.


uint8_t eps_output_bus_group_off(uint16_t CH_BF,  uint16_t CH_EXT_BF);
// 20230415 Frank:
// The bits enabled as '1's in CH_BF are the ones that are switched off. A '0' means no action on the bus.
// If the busses 16-31 specified by CH_EXT_BF are not used, then the user will specify CH_EXT_BF as 0x00 to leave those busses unchanged.


uint8_t eps_output_bus_group_state(uint16_t CH_BF,  uint16_t CH_EXT_BF);
//20230415 Frank: In this command, the user must specify which state every bus will be in. Busses 0 through 31.
// A '0' means that a bus is off, while a '1' means that the bus is on.


uint8_t eps_output_bus_channel_on(uint8_t CH_IDX);
//20230416 Frank:
// With this function, you will only switch one output bus to be ON. The power system will reject the switching on of multiple bus channels if this command is used.


uint8_t eps_output_bus_channel_off(uint8_t CH_IDX);
//20230416 Frank:
// With this function, you will only switch one output bus to be ON. The power system will reject the switching on of multiple bus channels if this command is used.
// Keep in mind that this function cannot switch off channels that are forced enabled (FE), and doing so will return an error.

void eps_switch_to_nominal_mode();
//20230418 Frank:
//This function is used for switching the Power System into Nomimal mode. The user does not need to input any parameters into the function, therefore it is void.
//This function doesn't need to return anything.

void eps_switch_to_safety_mode();
//20230418 Frank:
//This function is used for switching the Power System into Safety mode. The user does not need to input any parameters into the function, therefore it is void.
//This function doesn't need to return anything.

uint8_t eps_get_sys_status(eps_result_sys_stat_t* result_dest);

void eps_get_pdu_piu_overcurrent_fault_state(PDU_PIU_OFS* result_dest);
// Prepare the response buffer with output bus over current events. Over current fault counters are incremented each time a bus is latched off due to an overcurrent event. (Overcurrent event is when normal load current is exceeded)

void eps_get_pbu_abf_placed_state(PBU_ABF_PS* result_dest);
// Prepare the response buffer with ABF placed state information.

void eps_get_pdu_housekeeping_data_raw(PDU_HK_D* result_dest);
//are the response buffer with housekeeping data. The housekeeping data is returned in raw form, as received from the hardware, unaltered by the main controller.

void eps_get_pdu_housekeeping_data_eng(PDU_HK_D* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering form.

void eps_get_pdu_housekeeping_data_running_average(PDU_HK_D* result_dest);
//Prepare the response buffer with running average housekeeping data. The housekeeping data is returned in engineering values.

void eps_get_pbu_housekeeping_data_raw(PBU_HK_D* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in raw form, as received from the hardware, unaltered by the main controller.

void eps_get_pbu_housekeeping_data_eng(PBU_HK_D* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering values.

void eps_get_pbu_housekeeping_data_running_average(PBU_HK_D* result_dest);
//Prepare the response buffer with running average housekeeping data. The housekeeping data is returned in engineering values.

void eps_get_pcu_housekeeping_data_raw(PCU_HK_D* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in raw form, as received from the hardware, unaltered by the main controller.

void eps_get_pcu_housekeeping_data_eng(PCU_HK_D* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering values.

void eps_get_pcu_housekeeping_data_running_average(PCU_HK_D* result_dest);
//Prepare the response buffer with running average housekeeping data. The housekeeping data is returned in engineering values.

void eps_get_configuration_parameter(GET_CONFIG_PARAM* result_dest, uint16_t PAR_ID);
//get the value of a configuration parameter.

void eps_set_configuration_parameter(SET_CONFIG_PARAM* result_dest, uint16_t PAR_ID, uint8_t PAR_VAL);
//change a configuration parameter. The change will take effect immediately and any function using the parameter will use the new value.

void eps_reset_configuration_parameter(RESET_CONFIG_PAR* result_dest, uint16_t PAR_ID);
//reset a parameter to its default hard-coded value. All parameters have this value at system power-up or after the software reset command.

void eps_reset_configuration(RESET_CONFIGURATION* result_dest);
//Reset all configuration parameters to hard-coded defaults, discarding any changes made, in volatile memory (only!).

void eps_load_configuration(LOAD_CONFIGURATION* result_dest);
//Load all configuration parameters from non-volatile memory, discarding any changes made in volatile memory.

void eps_save_configuration(SAVE_CONFIGURATION* result_dest);
//Commit all read/write configuration parameters kept in volatile memory to non-volatile memory.

void eps_get_piu_housekeeping_data_raw(GET_PIU_HK* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in raw form, as received from the hardware, unaltered by the main controller.

void eps_get_piu_housekeeping_data_eng(GET_PIU_HK* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering values.

void eps_correct_time(CORRECT_TIME_S* result_dest, int32_t time_correction);
//Correct the unit’s unix time with the specified amount of seconds

void eps_zero_reset_cause_counters(ZERO_RESET_CAUSE_COUNTERS_S* result_dest);
//Write all reset cause counters to zero in persistent memory.



#endif /* INC_EPS_DRIVER_H_ */


