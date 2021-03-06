#ifndef __DEF_EXAMPLES
#define __DEF_EXAMPLES

typedef enum
{
    _SETUP = 0,
    _MAIN
} _EXAMPLE_STATUS;

void _EXAMPLE_TIMER();
void _EXAMPLE_SWITCH();
void _EXAMPLE_ENCODER();
void _EXAMPLE_AVERAGE_AND_NTC();
void _EXAMPLE_EEPROM();
void _EXAMPLE_MCP23S17();
void _EXAMPLE_LOG(ACQUISITIONS_VAR var);
void _EXAMPLE_WS2812B_MULTI_SEGMENTS();
void _EXAMPLE_UART();
void _EXAMPLE_PCA9685();
void _EXAMPLE_BLE(ble_params_t * p_ble);

#endif
