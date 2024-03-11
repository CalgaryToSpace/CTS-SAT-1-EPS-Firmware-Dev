
#ifndef __INCLUDE_GUARD__EPS_DRIVER_H__
#define __INCLUDE_GUARD__EPS_DRIVER_H__


#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


uint8_t eps_system_reset();
// - This function is used to send a system reset command to the ISIS Power system.
// - We are not excepting to receive anything useful back, or we may not even be able to poll the power system for a reply while it is resetting. Therefore its return type is "void".
// - CC indicates the type of command that will be sent to the power system. In this case, the user will be sending CC of "0xAA"


uint8_t eps_no_operation();
// This function is useful to check the functionality of the power system.


uint8_t eps_cancel_oper();
// This function switches off any command enable output bus channels.


uint8_t eps_watchdog();
// This function resets the watchdog timer, keeping the power system from resetting.


uint8_t eps_output_bus_group_on(uint16_t CH_BF,  uint16_t CH_EXT_BF);
// The bits enabled as '1's in CH_BF are the ones that are switched on. A '0' means no action on the bus.
// If the busses 16-31 specified by CH_EXT_BF are not used, then the user will specify CH_EXT_BF as 0x00 to leave those busses unchanged.


uint8_t eps_output_bus_group_off(uint16_t CH_BF,  uint16_t CH_EXT_BF);
// The bits enabled as '1's in CH_BF are the ones that are switched off. A '0' means no action on the bus.
// If the busses 16-31 specified by CH_EXT_BF are not used, then the user will specify CH_EXT_BF as 0x00 to leave those busses unchanged.


uint8_t eps_output_bus_group_state(uint16_t CH_BF,  uint16_t CH_EXT_BF);
//20230415 Frank: In this command, the user must specify which state every bus will be in. Busses 0 through 31.
// A '0' means that a bus is off, while a '1' means that the bus is on.


uint8_t eps_output_bus_channel_on(uint8_t CH_IDX);
// With this function, you will only switch one output bus to be ON. The power system will reject the switching on of multiple bus channels if this command is used.


uint8_t eps_output_bus_channel_off(uint8_t CH_IDX);
// With this function, you will only switch one output bus to be ON. The power system will reject the switching on of multiple bus channels if this command is used.
// Keep in mind that this function cannot switch off channels that are forced enabled (FE), and doing so will return an error.

uint8_t eps_switch_to_nominal_mode();
//This function is used for switching the Power System into Nomimal mode. The user does not need to input any parameters into the function, therefore it is void.
//This function doesn't need to return anything.

uint8_t eps_switch_to_safety_mode();
//This function is used for switching the Power System into Safety mode. The user does not need to input any parameters into the function, therefore it is void.
//This function doesn't need to return anything.

uint8_t eps_get_system_status(eps_result_system_status_t* result_dest);

uint8_t eps_get_pdu_piu_overcurrent_fault_state(PDU_PIU_OFS* result_dest);
// Prepare the response buffer with output bus over current events. Over current fault counters are incremented each time a bus is latched off due to an overcurrent event. (Overcurrent event is when normal load current is exceeded)

uint8_t eps_get_pbu_abf_placed_state(PBU_ABF_PS* result_dest);
// Prepare the response buffer with ABF placed state information.

uint8_t eps_get_pdu_housekeeping_data_raw(PDU_HK_D* result_dest);
//are the response buffer with housekeeping data. The housekeeping data is returned in raw form, as received from the hardware, unaltered by the main controller.

uint8_t eps_get_pdu_housekeeping_data_eng(PDU_HK_D* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering form.

uint8_t eps_get_pdu_housekeeping_data_running_average(PDU_HK_D* result_dest);
//Prepare the response buffer with running average housekeeping data. The housekeeping data is returned in engineering values.

uint8_t eps_get_pbu_housekeeping_data_raw(PBU_HK_D* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in raw form, as received from the hardware, unaltered by the main controller.

uint8_t eps_get_pbu_housekeeping_data_eng(PBU_HK_D* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering values.

uint8_t eps_get_pbu_housekeeping_data_running_average(PBU_HK_D* result_dest);
//Prepare the response buffer with running average housekeeping data. The housekeeping data is returned in engineering values.

uint8_t eps_get_pcu_housekeeping_data_raw(PCU_HK_D* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in raw form, as received from the hardware, unaltered by the main controller.

uint8_t eps_get_pcu_housekeeping_data_eng(PCU_HK_D* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering values.

uint8_t eps_get_pcu_housekeeping_data_running_average(PCU_HK_D* result_dest);
//Prepare the response buffer with running average housekeeping data. The housekeeping data is returned in engineering values.

uint8_t eps_get_configuration_parameter(GET_CONFIG_PARAM* result_dest, uint16_t PAR_ID);
//get the value of a configuration parameter.

uint8_t eps_set_configuration_parameter(SET_CONFIG_PARAM* result_dest, uint16_t PAR_ID, uint8_t PAR_VAL);
//change a configuration parameter. The change will take effect immediately and any function using the parameter will use the new value.

uint8_t eps_reset_configuration_parameter(RESET_CONFIG_PAR* result_dest, uint16_t PAR_ID);
//reset a parameter to its default hard-coded value. All parameters have this value at system power-up or after the software reset command.

uint8_t eps_reset_configuration();
//Reset all configuration parameters to hard-coded defaults, discarding any changes made, in volatile memory (only!).

uint8_t eps_load_configuration();
//Load all configuration parameters from non-volatile memory, discarding any changes made in volatile memory.

uint8_t eps_save_configuration();
//Commit all read/write configuration parameters kept in volatile memory to non-volatile memory.

uint8_t eps_get_piu_housekeeping_data_raw(GET_PIU_HK* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in raw form, as received from the hardware, unaltered by the main controller.

uint8_t eps_get_piu_housekeeping_data_eng(GET_PIU_HK* result_dest);
//Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering values.

uint8_t eps_correct_time(int32_t time_correction);
//Correct the unit’s unix time with the specified amount of seconds

uint8_t eps_zero_reset_cause_counters();
//Write all reset cause counters to zero in persistent memory.



#endif /* __INCLUDE_GUARD__EPS_DRIVER_H__ */
