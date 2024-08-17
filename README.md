# CTS-SAT-1-EPS-Firmware-Dev
ISISpace EPS firmware development for the OBC. This repo will be archived and integrated with the main OBC firmware upon completion.

## ⚠️ Archival/End-of-Life Notice ⚠️

⚠️ This repo is no longer used, and is out-of-date. The EPS firmware has been migrated into the main CTS-SAT-1 OBC firmware repo. ⚠️


## EPS Lessons Learned
1. The Software ICD suggests that "waiting 20ms after transmitting" is enough to ensure you'll get a good response, but that polling is another option. This is a lie; polling appears to be the only option, as the reception is unreliable no matter how long you wait.
2. For I2C on the STM32, you must left-shift the address by 1, compared to what's in the Software ICD.
3. From the start, write out the TX and RX bytestreams to a debug UART when working with I2C.
4. The no_operation command doesn't appear to update the result register, and thus the RC (CC+1) byte in the response still shows the previous command's execution when using the no_operation command.
5. The watchdog timer must be serviced as part of the "MVP" implementation.

## To Do List
* Sometimes the OBC enters a state where all I2C tx commands fail (`tx_status != HAL_OK`). Resetting the STM32 fixes it. This issue must be investigated more.
