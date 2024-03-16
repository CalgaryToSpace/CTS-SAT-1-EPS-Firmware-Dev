
#include "eps_types_to_json.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>


/*
 ***** To create these eps_<type>_TO_json(...) functions, the following ChatGPT prompt was used: ***** 

=================================================================================================
Write a C code function with a signature like: 
uint8_t eps_<type>_TO_json(const eps_<type>_t *data, char json_output_str[], uint16_t json_output_str_len).

This function should convert the following struct type to a JSON string. The JSON string should be
written to the json_output_str buffer. The length of the JSON string should not exceed the
json_output_str_len. The JSON string should be null-terminated. Use sprintf to write to the buffer.
Do not include whitespaces in the JSON output. The function should return 1 on error.

Assume that for any unknown types, a similar _TO_json function exists and can be used.

typedef struct {
	int16_t voltage_raw;
	int16_t current_raw;
	int16_t power_raw;
} eps_vpid_raw_t;

=================================================================================================

Note that modification are sometimes required, including:
0. In the prompt, rx_buf[5] is sometimes rx_buf[6].
1. Byte order check to be like the example given.
2. Function name.
3. Start/end conditions.
4. Update any multiplier, because LLMs are bad at guessing in advance how many bytes-per-loop are used.

*/

uint8_t eps_vpid_raw_TO_json(const eps_vpid_raw_t *data, char json_output_str[], uint16_t json_output_str_len) {
    if (data == NULL || json_output_str == NULL || json_output_str_len < 10) {
        return 1; // Error: Invalid input
    }

    int snprintf_ret = snprintf(
        json_output_str, json_output_str_len,
        "{\"voltage_raw\":%d,\"current_raw\":%d,\"power_raw\":%d}",
        data->voltage_raw, data->current_raw, data->power_raw);

    if (snprintf_ret < 0) {
        return 2; // Error: snprintf encoding error
    }
    if (snprintf >= json_output_str_len) {
        return 3; // Error: string buffer too short
    }
    return 0; // Success
}

uint8_t eps_vpid_eng_TO_json(const eps_vpid_eng_t *data, char json_output_str[], uint16_t json_output_str_len) {
    if (data == NULL || json_output_str == NULL || json_output_str_len < 10) {
        return 1; // Error: Invalid input
    }

    int snprintf_ret = snprintf(
        json_output_str, json_output_str_len,
        "{\"voltage_mV\":%d,\"current_mA\":%d,\"power_cW\":%d}",
        data->voltage_mV, data->current_mA, data->power_cW);

    if (snprintf_ret < 0) {
        return 2; // Error: snprintf encoding error
    }
    if (snprintf >= json_output_str_len) {
        return 3; // Error: string buffer too short
    }
    return 0; // Success
}

uint8_t eps_battery_pack_datatype_raw_TO_json(const eps_battery_pack_datatype_raw_t *data, char json_output_str[], uint16_t json_output_str_len) {
    if (data == NULL || json_output_str == NULL || json_output_str_len < 10) {
        return 1; // Error: Invalid input
    }

    char vip_bp_input_raw_json[100];
    const uint8_t json_ret_code = eps_battery_pack_datatype_raw_TO_json(&(data->vip_bp_input_raw), vip_bp_input_raw_json, 100);
    if (json_ret_code != 0) {
        return json_ret_code; // Error: subfunction error
    }

    int snprintf_ret = snprintf(
        json_output_str, json_output_str_len,
        "{\"vip_bp_input_raw\":%s,\"bp_status_bitfield\":%d,\"cell_voltage_each_cell_raw\":[%d,%d,%d,%d],\"battery_temperature_each_sensor_raw\":[%d,%d,%d]}",
        vip_bp_input_raw_json,
        data->bp_status_bitfield,
        data->cell_voltage_each_cell_raw[0],
        data->cell_voltage_each_cell_raw[1],
        data->cell_voltage_each_cell_raw[2],
        data->cell_voltage_each_cell_raw[3],
        data->battery_temperature_each_sensor_raw[0],
        data->battery_temperature_each_sensor_raw[1],
        data->battery_temperature_each_sensor_raw[2]);

    if (snprintf_ret < 0) {
        return 2; // Error: snprintf encoding error
    }
    if (snprintf >= json_output_str_len) {
        return 3; // Error: string buffer too short
    }
    return 0; // Success
}

uint8_t eps_battery_pack_datatype_eng_TO_json(const eps_battery_pack_datatype_eng_t *data, char json_output_str[], uint16_t json_output_str_len) {
    if (data == NULL || json_output_str == NULL || json_output_str_len < 10) {
        return 1; // Error: Invalid input
    }

    char vip_bp_input_json[100];
    const uint8_t json_ret_code = eps_battery_pack_datatype_eng_TO_json(&(data->vip_bp_input), vip_bp_input_json, 100);
    if (json_ret_code != 0) {
        return json_ret_code; // Error: subfunction error
    }

    int snprintf_ret = snprintf(
        json_output_str, json_output_str_len,
        "{\"vip_bp_input\":%s,\"bp_status_bitfield\":%d,\"cell_voltage_each_cell_eng\":[%d,%d,%d,%d],\"battery_temperature_each_sensor_eng\":[%d,%d,%d]}",
        vip_bp_input_json,
        data->bp_status_bitfield,
        data->cell_voltage_each_cell_mV[0],
        data->cell_voltage_each_cell_mV[1],
        data->cell_voltage_each_cell_mV[2],
        data->cell_voltage_each_cell_mV[3],
        data->battery_temperature_each_sensor_cC[0],
        data->battery_temperature_each_sensor_cC[1],
        data->battery_temperature_each_sensor_cC[2]);

    if (snprintf_ret < 0) {
        return 2; // Error: snprintf encoding error
    }
    if (snprintf >= json_output_str_len) {
        return 3; // Error: string buffer too short
    }
    return 0; // Success
}
