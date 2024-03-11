
#ifndef __INCLUDE_GUARD__EPS_TYPES_H__
#define __INCLUDE_GUARD__EPS_TYPES_H__

#include <stdint.h>

typedef enum {
	// TODO: add the 5V MPI channel
	EPS_CHANNEL_VBATT_STACK             = 0, // CH0
	EPS_CHANNEL_5V_STACK                = 1, // CH1
	EPS_CHANNEL_5V_CH2_UNUSED           = 2, // CH2
	EPS_CHANNEL_5V_CH3_UNUSED           = 3, // CH3
	EPS_CHANNEL_5V_CH4_UNUSED           = 4, // CH4
	EPS_CHANNEL_3V3_STACK               = 5, // CH5
	EPS_CHANNEL_3V3_CAMERA              = 6, // CH6
	EPS_CHANNEL_3V3_UHF_ANTENNA_DEPLOY  = 7, // CH7
	EPS_CHANNEL_3V3_LORA_MODULES        = 8, // CH8
	EPS_CHANNEL_VBATT_CH9_UNUSED        = 9, // CH9
	EPS_CHANNEL_VBATT_CH10_UNUSED       = 10, // CH10
	EPS_CHANNEL_VBATT_CH11_UNUSED       = 11, // CH11
	EPS_CHANNEL_12V_MPI                 = 12, // CH12
	EPS_CHANNEL_12V_BOOM                = 13, // CH13
	EPS_CHANNEL_3V3_CH14_UNUSED         = 14, // CH14
	EPS_CHANNEL_3V3_CH15_UNUSED         = 15, // CH15
	EPS_CHANNEL_28V6_CH16_UNUSED        = 16, // CH16
} EPS_CHANNEL_enum_t;

// "Complex Datatype: VIPD" (raw)
typedef struct {
	int16_t voltage_raw;
	int16_t current_raw;
	int16_t power_raw;
} eps_vpid_raw_t;

// "Complex Datatype: VIPD" (eng)
typedef struct {
	int16_t voltage_mV;
	int16_t current_mA;
	int16_t power_cW; // centiWatts (x 10^-2 watts)
} eps_vpid_eng_t;

// "Complex Datatype: BPD" (raw)
typedef struct {
	eps_vpid_raw_t vip_bp_input_raw;
	uint16_t bp_stat_bitfield; // See Table 3-18 for bitfield definitions
	uint16_t cell_voltage_raw[4];
    uint16_t battery_temperature_raw[3];
} eps_battery_pack_datatype_raw_t;

// "Complex Datatype: BPD (Battery Pack Datatype)" (eng)
typedef struct {
    eps_vpid_eng_t vip_bp_input_eng;
	uint16_t bp_stat_bitfield; // See Table 3-18 for bitfield definitions
    int16_t cell_voltage_mV[4];
    int16_t battery_temperature_cC[3]; // centiCelsius = 1e-2 degrees C
} eps_battery_pack_datatype_eng_t;

// "Complex Datatype: CCD (Conditioning Channel Datatype)" (raw)
// The conditioning channel refers to the MPPT system.
typedef struct {
	eps_vpid_raw_t vip_cc_output_raw;
    uint16_t volt_in_mppt_raw;
    uint16_t curr_in_mppt_raw;
    uint16_t volt_ou_mppt_raw;
    uint16_t curr_ou_mppt_raw;
} eps_conditioning_channel_datatype_raw_t;

// "Complex Datatype: CCD (Conditioning Channel Datatype)" (eng)
// The conditioning channel refers to the MPPT system.
typedef struct {
	eps_vpid_eng_t vip_cc_output_eng;
    int16_t volt_in_mppt_mV;
    int16_t curr_in_mppt_mA;
    int16_t volt_ou_mppt_mV;
    int16_t curr_ou_mppt_mA;
} eps_conditioning_channel_datatype_eng_t;

// "Complex Datatype: CCSD (Conditioning Channel Short Datatype)" (raw)
// The conditioning channel refers to the MPPT system.
typedef struct {
    uint16_t volt_in_mppt_raw;
    uint16_t curr_in_mppt_raw;
    uint16_t volt_ou_mppt_raw;
    uint16_t curr_ou_mppt_raw;
} eps_conditioning_channel_short_datatype_raw_t;

// "Complex Datatype: CCSD (Conditioning Channel Short Datatype)" (eng)
// The conditioning channel refers to the MPPT system.
typedef struct {
    int16_t volt_in_mppt_mV;
    int16_t curr_in_mppt_mA;
    int16_t volt_ou_mppt_mV;
    int16_t curr_ou_mppt_mA;
} eps_conditioning_channel_short_datatype_eng_t;

// Command Response: 0x40: Get System Status
typedef struct {
	uint8_t mode; // 0=startup, 1=nominal, 2=safety, 3=emergency_low_power
	uint8_t config_changed_since_boot; // 0=not_altered, 1=changed_since_boot
	uint8_t reset_cause; // 0=power_on, 1=watchdog, 2=commanded, 3=control_system_reset, 4=emergency_low_power
	uint32_t uptime_sec;
	uint16_t error_code;
	uint16_t rst_cnt_pwron;
	uint16_t rst_cnt_wdg;
	uint16_t rst_cnt_cmd;
	uint16_t rst_cnt_mcu;
	uint16_t rst_cnt_emlopo; // reset count due to Emergency Low Power
	uint16_t time_since_prev_cmd_sec;
	uint32_t unix_time_sec; // seconds since 1970-01-01
	uint8_t calendar_years_since_2000; // real year = 2000 + unix_years_since_2000
	uint8_t calendar_month;
	uint8_t calendar_day;
	uint8_t calendar_hour;
	uint8_t calendar_minute;
	uint8_t calendar_second;
} eps_result_system_status_t;

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



#endif /* __INCLUDE_GUARD__EPS_TYPES_H__ */
