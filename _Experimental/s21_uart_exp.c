/*********************************************************************
*	UART modules
*	Author : Sébastien PERREAU
*
*	Revision history	:
*		05/11/2018		- Initial release
*********************************************************************/

#include "../PLIB.h"

extern const EXP_UART_REGISTERS * UartModules[];
const EXP_UART_REGISTERS * UartModules[] =
{
	(EXP_UART_REGISTERS*)_UART1_BASE_ADDRESS,
	(EXP_UART_REGISTERS*)_UART2_BASE_ADDRESS,
	(EXP_UART_REGISTERS*)_UART3_BASE_ADDRESS,
	(EXP_UART_REGISTERS*)_UART4_BASE_ADDRESS,
	(EXP_UART_REGISTERS*)_UART5_BASE_ADDRESS,
	(EXP_UART_REGISTERS*)_UART6_BASE_ADDRESS
};
static uint32_t real_baudrate_tab[EXP_UART_NUMBER_OF_MODULES] = {0};

/*******************************************************************************
 * Function: 
 *      void exp_uart_init( EXP_UART_MODULE id, 
 *                   EXP_UART_BAUDRATE baudrate, 
 *                   EXP_UART_ENABLE_MODE enable_mode,
 *                   EXP_UART_CONFIG_MODE config_mode,
 *                   EXP_UART_LINE_CONTROL_MODE control_mode,
 *                   EXP_UART_FIFO_MODE fifo_mode,
 *                   EXP_UART_ADDRESS_DETECTION address_detection)
 * 
 * Description:
 *      This routine is used to initialize a uart module.
 * 
 * Parameters:
 *      id: The UART module you want to use.
 *      baudrate: The desire baudrate in bit per second.
 *      enable_mode: Params follow the EXP_UART_ENABLE_MODE enumeration.
 *      config_mode: Params follow the EXP_UART_CONFIG_MODE enumeration.
 *      control_mode: Params follow the EXP_UART_LINE_CONTROL_MODE enumeration.
 *      fifo_mode: Params follow the EXP_UART_FIFO_MODE enumeration.
 *      address_detection: Params follow the EXP_UART_ADDRESS_DETECTION enumeration.
 * 
 * Return:
 *      none
 * 
 * Example:
 *      none
 ******************************************************************************/

void exp_uart_init( EXP_UART_MODULE id, 
                    EXP_UART_BAUDRATE baudrate, 
                    EXP_UART_ENABLE_MODE enable_mode,
                    EXP_UART_CONFIG_MODE config_mode,
                    EXP_UART_LINE_CONTROL_MODE control_mode,
                    EXP_UART_FIFO_MODE fifo_mode,
                    EXP_UART_ADDRESS_DETECTION address_detection)
{
    if (enable_mode & EXP_UART_ENABLE)
    {
        exp_uart_enable(id, EXP_UART_DISABLE);
        exp_uart_set_params(id, config_mode);
        exp_uart_set_line_control(id, control_mode);
        exp_uart_set_baudrate(id, baudrate);
        exp_uart_set_fifo(id, fifo_mode);
        exp_uart_set_adress_detection(id, (uint8_t) address_detection, address_detection & EXP_UART_ADDRESS_DETECTION_MASK);
        exp_uart_enable(id, enable_mode);
    }
    else
    {
        exp_uart_enable(id, EXP_UART_DISABLE);
    }
}

/*******************************************************************************
 * Function: 
 *      void exp_uart_enable(EXP_UART_MODULE id, EXP_UART_ENABLE_MODE enable_mode)
 * 
 * Description:
 *      This routine is used to enable the uart module and the RX/TX pins.
 * 
 * Parameters:
 *      id: The UART module you want to use.
 *      enable_mode: Params follow the EXP_UART_ENABLE_MODE enumeration.
 * 
 * Return:
 *      none
 * 
 * Example:
 *      none
 ******************************************************************************/
void exp_uart_enable(EXP_UART_MODULE id, EXP_UART_ENABLE_MODE enable_mode)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    if (enable_mode & EXP_UART_ENABLE)
    {
        p_uart->STASET = (enable_mode & (EXP_UART_ENABLE_RX_PIN | EXP_UART_ENABLE_TX_PIN));
        p_uart->MODESET = 0x00008000;
    }
    else
    {
        p_uart->STACLR = EXP_UART_ENABLE_RX_PIN | EXP_UART_ENABLE_TX_PIN;
        p_uart->MODECLR = 0x00008000;
    }
}

/*******************************************************************************
 * Function: 
 *      void exp_uart_set_params(EXP_UART_MODULE id, EXP_UART_CONFIG_MODE config_mode)
 * 
 * Description:
 *      This routine is used to configure the MODE register.
 *      This register does the following:
 *          - Enables or disables the UART module
 *          - Enables or disables the IrDA encoder and decoder
 *          - Enables or disables the WAKE, ABAUD and Loopback features
 *          - Enables or disables the UxRTS and UxCTS pins
 *          - Configures the UxRTS pin for the desired mode of operation
 *          - Configures the polarity of the UxRX pin
 *          - Selects the type of baud rate
 *          - Selects the number of data bits, parity and stop bits
 * 
 * Parameters:
 *      id: The UART module you want to use.
 *      config_mode: Params follow the EXP_UART_CONFIG_MODE enumeration.
 * 
 * Return:
 *      none
 * 
 * Example:
 *      none
 ******************************************************************************/
void exp_uart_set_params(EXP_UART_MODULE id, EXP_UART_CONFIG_MODE config_mode)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    p_uart->MODECLR = (EXP_UART_CONFIG_MASK & 0x0000ffff);
    p_uart->STACLR = ((EXP_UART_CONFIG_MASK >> 16) & 0x0000ffff);
    p_uart->MODESET = (config_mode & 0x0000ffff);
    p_uart->STASET = ((config_mode >> 16) & 0x0000ffff);
}

/*******************************************************************************
 * Function: 
 *      void exp_uart_set_line_control(EXP_UART_MODULE id, EXP_UART_LINE_CONTROL_MODE control_mode)
 * 
 * Description:
 *      This routine is used to configure the data format .
 *      This register does the following:
 *          - Enables or disables the UART module
 *          - Enables or disables the IrDA encoder and decoder
 *          - Enables or disables the WAKE, ABAUD and Loopback features
 *          - Enables or disables the UxRTS and UxCTS pins
 *          - Configures the UxRTS pin for the desired mode of operation
 *          - Configures the polarity of the UxRX pin
 *          - Selects the type of baud rate
 *          - Selects the number of data bits, parity and stop bits
 * 
 * Parameters:
 *      id: The UART module you want to use.
 *      control_mode: Params follow the EXP_UART_LINE_CONTROL_MODE enumeration.
 * 
 * Return:
 *      none
 * 
 * Example:
 *      none
 ******************************************************************************/
void exp_uart_set_line_control(EXP_UART_MODULE id, EXP_UART_LINE_CONTROL_MODE control_mode)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    p_uart->MODECLR = EXP_UART_LINE_CONTROL_MASK;
    p_uart->MODESET = (control_mode & EXP_UART_LINE_CONTROL_MASK);
}

/*******************************************************************************
 * Function: 
 *      void exp_uart_set_fifo(EXP_UART_MODULE id, EXP_UART_FIFO_MODE fifo_mode)
 * 
 * Description:
 *      This routine is used to set the type of interruption for Rx et Tx.
 * 
 * Parameters:
 *      id: The UART module you want to use.
 *      fifo_mode: Params follow the EXP_UART_FIFO_MODE enumeration.
 * 
 * Return:
 *      none
 * 
 * Example:
 *      none
 ******************************************************************************/
void exp_uart_set_fifo(EXP_UART_MODULE id, EXP_UART_FIFO_MODE fifo_mode)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    p_uart->STACLR = EXP_UART_FIFO_MASK;
    p_uart->STASET = (fifo_mode & EXP_UART_FIFO_MASK);
}

/*******************************************************************************
 * Function: 
 *      void exp_uart_set_adress_detection(EXP_UART_MODULE id, uint8_t address, EXP_UART_ADDRESS_DETECTION address_detection)
 * 
 * Description:
 *      This routine is used to set the address when detection is enable.
 * 
 * Parameters:
 *      id: The UART module you want to use.
 *      address: Set the address you want to detect.
 *      address_detection: Enable or disable the address detection.
 * 
 * Return:
 *      none
 * 
 * Example:
 *      none
 ******************************************************************************/
void exp_uart_set_adress_detection(EXP_UART_MODULE id, uint8_t address, EXP_UART_ADDRESS_DETECTION address_detection)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    p_uart->STACLR = EXP_UART_ADDRESS_DETECTION_MASK;
    p_uart->STASET = (((uint32_t) address) << 16) | address_detection;
}

/*******************************************************************************
 * Function: 
 *      void exp_uart_set_baudrate(EXP_UART_MODULE id, uint32_t baudrate)
 * 
 * Description:
 *      This routine is used to set the baudrate for the UART module.
 *      Note:   The desired baudrate may not be the one passed as parameter. It
 *              can be an approximation, that is why the "real baudrate" is 
 *              store in a tab.
 * 
 * Parameters:
 *      id: The UART module you want to use.
 *      baudrate: The desire baudrate in bit per second.
 * 
 * Return:
 *      none
 * 
 * Example:
 *      none
 ******************************************************************************/
void exp_uart_set_baudrate(EXP_UART_MODULE id, uint32_t baudrate)
{
	EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    uint32_t v_baudrate;
    uint32_t v_source_clock = (PERIPHERAL_FREQ >> 1);

    if(!(p_uart->MODE & _U1MODE_BRGH_MASK))
    {
        v_source_clock >>= 2;
    }

    v_baudrate = v_source_clock / baudrate;
    v_baudrate++;
    v_baudrate >>= 1;
    v_baudrate--;

    p_uart->BRG = v_baudrate & 0x0000FFFF;

    real_baudrate_tab[id] = (v_source_clock >> 1) / ( v_baudrate + 1 );
}

/*******************************************************************************
 * Function: 
 *      uint32_t exp_uart_get_baudrate(EXP_UART_MODULE id)
 * 
 * Description:
 *      This routine is used to get the real baudrate for the selected UART module.
 * 
 * Parameters:
 *      id: The UART module you want to use.
 * 
 * Return:
 *      The real baudrate in bit per second.
 * 
 * Example:
 *      none
 ******************************************************************************/
uint32_t exp_uart_get_baudrate(EXP_UART_MODULE id)
{
    return real_baudrate_tab[id];
}

/*******************************************************************************
 * Function: 
 *      bool exp_uart_transmission_has_completed(EXP_UART_MODULE id)
 * 
 * Description:
 *      This routine is used to get the status of a transmit data byte for a 
 *      selected UART module.
 * 
 * Parameters:
 *      id: The UART module you want to use.
 * 
 * Return:
 *      The status of the transmission (true = completed / false = on going).
 * 
 * Example:
 *      none
 ******************************************************************************/
bool exp_uart_transmission_has_completed(EXP_UART_MODULE id)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    return (_U1STA_TRMT_MASK == (p_uart->STA & _U1STA_TRMT_MASK));
}

/*******************************************************************************
 * Function: 
 *      bool exp_uart_is_tx_ready(EXP_UART_MODULE id)
 * 
 * Description:
 *      This routine is used to get the status of a transmit buffer for a 
 *      selected UART module.
 * 
 * Parameters:
 *      id: The UART module you want to use.
 * 
 * Return:
 *      The status of the tx buffer (true = ready / false = not ready).
 * 
 * Example:
 *      none
 ******************************************************************************/
bool exp_uart_is_tx_ready(EXP_UART_MODULE id)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    return (bool)(!(p_uart->STA & _U1STA_UTXBF_MASK));
}

/*******************************************************************************
 * Function: 
 *      bool exp_uart_is_rx_data_available(EXP_UART_MODULE id)
 * 
 * Description:
 *      This routine is used to get the status of a receive buffer for a 
 *      selected UART module.
 * 
 * Parameters:
 *      id: The UART module you want to use.
 * 
 * Return:
 *      The status of the rx buffer (true = data available / false = data not available.
 * 
 * Example:
 *      none
 ******************************************************************************/
bool exp_uart_is_rx_data_available(EXP_UART_MODULE id)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    return (bool)(_U1STA_URXDA_MASK == (p_uart->STA & _U1STA_URXDA_MASK));
}

/*******************************************************************************
 * Function: 
 *      void exp_uart_send_break(EXP_UART_MODULE id)
 * 
 * Description:
 *      This routine is used to send a break characters.
 *      A Break character transmit consists of a Start bit, followed by twelve 
 *      bits of ?0?, and a Stop bit. A Frame Break character is sent whenever 
 *      the UART module is enabled, and the UTXBRK (UxSTA<11>) and UTXEN 
 *      (UxSTA<10>) bits are set while the UxTXREG register is loaded with data. 
 *      A dummy write to the UxTXREG register is necessary to initiate the Break 
 *      character transmission. The data value written to the UxTXREG register 
 *      for the Break character is ignored. 
 * 
 * Parameters:
 *      id: The UART module you want to use.
 * 
 * Return:
 *      The status of the break transmission (0: done / 1: not sent)
 * 
 * Example:
 *      none
 ******************************************************************************/
bool exp_uart_send_break(EXP_UART_MODULE id)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    if ((bool)(!(p_uart->STA & _U1STA_UTXBF_MASK)))
    {
        p_uart->STASET = _U1STA_UTXBRK_MASK;
        p_uart->TX = 0;
        return 0;
    }
    return 1;
}

/*******************************************************************************
 * Function: 
 *      bool exp_uart_send_data(EXP_UART_MODULE id, uint16_t data)
 * 
 * Description:
 *      This routine is used to send a data (8 bits or 9 bits).
 * 
 * Parameters:
 *      id: The UART module you want to use.
 *      data: The data (8 or 9 bits) to send.
 * 
 * Return:
 *      The status of the data transmission (0: done / 1: not sent)
 * 
 * Example:
 *      none
 ******************************************************************************/
bool exp_uart_send_data(EXP_UART_MODULE id, uint16_t data)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    if ((bool)(!(p_uart->STA & _U1STA_UTXBF_MASK)))
    {
        p_uart->TX = data;
        return 0;
    }
    return 1;
}

/*******************************************************************************
 * Function: 
 *      bool exp_uart_get_data(EXP_UART_MODULE id, uint16_t *data)
 * 
 * Description:
 *      This routine is used to get a data (8 bits or 9 bits).
 * 
 * Parameters:
 *      id: The UART module you want to use.
 *      p_data: A pointer to store the data receive.
 * 
 * Return:
 *      The status of the data reception (0: done / 1: not receive)
 * 
 * Example:
 *      none
 ******************************************************************************/
bool exp_uart_get_data(EXP_UART_MODULE id, uint16_t *p_data)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    if ((bool)(_U1STA_URXDA_MASK == (p_uart->STA & _U1STA_URXDA_MASK)))
    {
        *p_data = (uint16_t) p_uart->RX;
        return 0;
    }
    return 1;
}
