# CTS-SAT-1-EPS-Firmware-Dev
ISISpace EPS firmware development for the OBC. This repo will be archived and integrated with the main OBC firmware upon completion.

## EPS Lessons Learned
1. The Software ICD suggests that "waiting 20ms after transmitting" is enough to ensure you'll get a good response, but that polling is another option. This is a lie; polling appears to be the only option, as the reception is unreliable no matter how long you wait.
2. For I2C on the STM32, you must left-shift the address by 1, compared to what's in the Software ICD.
3. From the start, write out the TX and RX bytestreams to a debug UART when working with I2C.
