/*********************************************************************
*	Author : S�bastien PERREAU
*
*	Revision history	:
*		02/06/2017		- Initial release
*                       - Add E_EEPROM example
* 
*   Description:
*   ------------ 
*********************************************************************/

#include "../PLIB.h"

void _EXAMPLE_SWITCH()
{
    SWITCH_DEF(sw1, SWITCH1, ACTIVE_LOW);
    static state_machine_t sm_example = {0};
    
    switch (sm_example.index)
    {
        case _SETUP:
            sm_example.index = _MAIN;
            break;
            
        case _MAIN:
            
            switch (sw1.indice)
            {
                case 0:
                    mUpdateLedStatusD2(OFF);
                    mUpdateLedStatusD3(OFF);
                    break;
                case 1:
                    mUpdateLedStatusD2(fu_turn_indicator(ON, TICK_100MS, TICK_100MS));
                    break;
                case 2:
                    mUpdateLedStatusD2(fu_turn_indicator(ON, TICK_200MS, TICK_200MS));
                    break;
                case 3:
                    mUpdateLedStatusD2(fu_turn_indicator(ON, TICK_500MS, TICK_50MS));
                    break;
                case 4:
                    sw1.indice = 0;
                    break;
            }
            
            if (sw1.type_of_push == LONG_PUSH)
            {
                mUpdateLedStatusD2(BLINK);
                mUpdateLedStatusD3(BLINK_INV);
            }
            
            if (sw1.is_updated)
            {
                sw1.is_updated = false;
                // ...
            }
            
            fu_switch(&sw1);
            break;
    } 
}

void _EXAMPLE_ENCODER()
{
    ENCODER_DEF(encoder, __PE0, __PE1, ACTIVE_HIGH);
    static state_machine_t sm_example = {0};
    
    switch (sm_example.index)
    {
        case _SETUP:
            sm_example.index = _MAIN;
            break;
            
        case _MAIN:
            
            if (encoder.indice > 0)
            {
                mUpdateLedStatusD2(fu_turn_indicator(ON, TICK_1MS*encoder.indice, TICK_1MS*encoder.indice));
                mUpdateLedStatusD3(OFF);
            }
            else if (encoder.indice < 0)
            {
                mUpdateLedStatusD2(OFF);
                mUpdateLedStatusD3(fu_turn_indicator(ON, -(TICK_1MS*encoder.indice), -(TICK_1MS*encoder.indice)));
            }
            else
            {
                mUpdateLedStatusD2(BLINK);
                mUpdateLedStatusD3(BLINK_INV);
            }
            
            fu_encoder(&encoder);
            break;
    }   
}

void _EXAMPLE_AVERAGE_AND_NTC()
{
    NTC_DEF(ntc_1, AN1, 25, 10000, 3380);
    NTC_DEF(ntc_2, AN2, 25, 10000, 3380);
    AVERAGE_DEF(avg_1, AN3, 30, TICK_1MS);
    static state_machine_t sm_example = {0};
    
    switch (sm_example.index)
    {
        case _SETUP:
            adc10_init(AN1|AN2|AN3|AN15, ADC10_VREFP_VREFN);
            sm_example.index = _MAIN;
            break;
            
        case _MAIN:
            fu_ntc(&ntc_1);
            fu_ntc(&ntc_2);
            fu_adc_average(&avg_1);
            
            if (ntc_1.temperature > 32.0)
            {
                // ...
            }
            break;
    }   
}

void _EXAMPLE_EEPROM()
{
    EEPROM_DEF(e_eeprom, SPI3A, csRD3, TICK_20MS, 150, 150);
    BUS_MANAGEMENT_DEF(bm_spi, &e_eeprom.spi_params.bus_management_params);
    static state_machine_t sm_example = {0};
    static uint64_t tickAntiFloodSw1 = 0;
    static uint64_t tickAntiFloodSw2 = 0;
    static uint64_t tickRead = 0;
    
    switch (sm_example.index)
    {
        case _SETUP:
            e_eeprom_check_for_erasing_memory(&e_eeprom, &bm_spi);
            sm_example.index = _MAIN;
            break;
            
        case _MAIN:
            if (!mGetIO(SWITCH1) && (mTickCompare(tickAntiFloodSw1) > TICK_200MS))
            {
                tickAntiFloodSw1 = mGetTick();
                if (!eEEPROMIsWriteInProgress(e_eeprom))
                {
                    // !! BE SURE TO HAVE ENOUGH SPACE INTO THE BUFFER !!
                    uint8_t i = 0;
                    e_eeprom.registers.dW.size = 5;
                    for(i = 0 ; i < (e_eeprom.registers.dW.size) ; i++)
                    {
                        e_eeprom.registers.dW.p[i] = 0x44+i;
                    }
                    eEEPROMWriteBytes(e_eeprom, 127);
                }
            }

            if (!mGetIO(SWITCH2) && (mTickCompare(tickAntiFloodSw2) > TICK_200MS))
            {
                tickAntiFloodSw2 = mGetTick();
                eEEPROMChipErase(e_eeprom);
            }

            if (mTickCompare(tickRead) >= TICK_100MS)
            {
                tickRead = mGetTick();
                eEEPROMReadBytes(e_eeprom, 127, 5);
            }

            if (!eEEPROMIsReadInProgress(e_eeprom))
            {
                if (e_eeprom.registers.dR.p[0] == 0x44)
                {
                    mUpdateLedStatusD2(ON);
                }
                else
                {
                    mUpdateLedStatusD2(OFF);
                }
            }
            
            fu_bus_management_task(&bm_spi);
            eEEPROMDeamon(&e_eeprom);
            break;
            
    }
}

void _EXAMPLE_MCP23S17()
{
    static MCP23S17_CONFIG e_mcp23s17;
    BUS_MANAGEMENT_DEF(bm, &e_mcp23s17.spi_params.bus_management_params);
    static bool isInitDone = false;
    static uint64_t tickAntiFloodSw1 = 0;
    
    if (!isInitDone)
    {
        eMCP23S17InitVar(SPI3A, csRD3, TICK_20MS, &e_mcp23s17);
        e_mcp23s17.write_registers.IODIRA = 0x00; 
        isInitDone = true;
    }
    else
    {
        if (!mGetIO(SWITCH1) && (mTickCompare(tickAntiFloodSw1) > TICK_200MS))
        {
            tickAntiFloodSw1 = mGetTick();
            
            e_mcp23s17.write_registers.OLATA = !e_mcp23s17.read_registers.GPIOA;
        }
        else
        {
            e_mcp23s17.write_registers.OLATA = 0x00;
        }
        
        if (GET_BIT(e_mcp23s17.read_registers.GPIOA, 0))
        {
            mUpdateLedStatusD2(ON);
        }
        else
        {
            mUpdateLedStatusD2(OFF);
        }
        
        fu_bus_management_task(&bm);
        eMCP23S17Deamon(&e_mcp23s17);
    }
}

#define VALEO   "1MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"2MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"3MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"4MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"5MMMMMMMMMMMMMMMMMMMMMMMMMMMhhhhhhhhhhhhhhhhhhhhMMMMMMMMMMMMMMMMMMMhhhhhhhhhhhhhhhhhhmMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNhhhhhhhhhhhhhhhhhMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"6MMMMMMMMMMMMMMMMMMMMMMMMMMM+///////////////////NMMMMMMMMMMMMMMMMMd/////////////////+NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMh////////////////+MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"7MMMMMMMMMMMMMMMMMMMMMMMMMMMo///////////////////mMMMMMMMMMMMMMMMMMo/////////////////dMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMs////////////////sMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"8MMMMMMMMMMMMMMMMMMMMMMMMMMMo///////////////////mMMMMMMMMMMMMMMMMh/////////////////yMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM+////////////////hMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"9MMMMMMMMMMMMMMMMMMMMMMMMMMMs///////////////////mMMMMMMMMMMMMMMMN+////////////////oMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN/////////////////mMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"10MMMMMMMMMMMMMMMMMMMMMMMMMMy///////////////////mMMMMMMMMMMMMMMMs////////////////+NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMh/////////////////MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"11MMMMMMMMMMMMMMMMMMMMMMMMMMd///////////////////dMMMMMMMMMMMMMMm/////////////////dMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMs////////////////oMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"12MMMMMMMMMMMMMMMMMMMMMMMMMMm///////////////////hMMMMMMMMMMMMMN+////////////////yMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM+////////////////yMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"13MMMMMMMMMMMMMMMMMMMMMMMMMMN///////////////////hMMMMMMMMMMMMMy////////////////sMMMMMMMMMMMMNmmdhhhyyyyyyyyyyyyyyyyyhhhhhNMMMN/////////////////mMMMMMMMMMMMMMMNmdhyyssooooooossyhdmMMMMMMMMMMMMMMMMMMMMMMMMMMNmdhyyssooooooossyyhdmNMMMMMMMMMMMMMMMMMMM\n\r"\
				"14MMMMMMMMMMMMMMMMMMMMMMMMMMM+//////////////////yMMMMMMMMMMMMm////////////////oNMMMMMMMmdyo+++//////////////////////////+MMMMh/////////////////MMMMMMMMMMMmhs++///////////////////++shmMMMMMMMMMMMMMMMMMMNdyo++////////////////////++ydNMMMMMMMMMMMMMMM\n\r"\
				"15MMMMMMMMMMMMMMMMMMMMMMMMMMMs//////////////////sMMMMMMMMMMMN+///////////////+mMMMMMdy++////////////////////////////////sMMMMs////////////////oMMMMMMMMNho+///////////////////////////+odMMMMMMMMMMMMMNds+/////////////////////////////+smMMMMMMMMMMMMM\n\r"\
				"16MMMMMMMMMMMMMMMMMMMMMMMMMMMh//////////////////oMMMMMMMMMMMs////////////////dMMMNho////////////////////////////////////hMMMM+////////////////yMMMMMMNy+/////////////////////////////////oNMMMMMMMMMMh+///////////////////////////////////sNMMMMMMMMMMM\n\r"\
				"17MMMMMMMMMMMMMMMMMMMMMMMMMMMm//////////////////+MMMMMMMMMMh////////////////hMMNh+//////////////////////////////////////mMMMN/////////////////dMMMMMmo/////////////////++/////////////////+mMMMMMMMNs//////////////////////////////////////+NMMMMMMMMMM\n\r"\
				"18MMMMMMMMMMMMMMMMMMMMMMMMMMMM+//////////////////MMMMMMMMMm+///////////////yMMmo/////////////////+ossso////////////////+MMMMd/////////////////MMMMMd+///////////////ohmNNNdo///////////////oMMMMMMm+////////////////ohmNmds/////////////////oMMMMMMMMMM\n\r"\
				"19MMMMMMMMMMMMMMMMMMMMMMMMMMMMs//////////////////NMMMMMMMNo///////////////sMMd+////////////////sdNMMMMs////////////////sMMMMy////////////////oMMMMd////////////////yMMMMMMMNo///////////////dMMMMm+////////////////sNMMMMMMs/////////////////hMMMMMMMMM\n\r"\
				"20MMMMMMMMMMMMMMMMMMMMMMMMMMMMd//////////////////mMMMMMMMs///////////////sNMd+////////////////hMMMMMMM+////////////////hMMMM+////////////////yMMMN+///////////////sMMMMMMMMMy///////////////sMMMMo////////////////+NMMMMMMMm/////////////////sMMMMMMMMM\n\r"\
				"21MMMMMMMMMMMMMMMMMMMMMMMMMMMMN//////////////////hMMMMMMy///////////////oNMm+////////////////dMMMMMMMm/////////////////mMMMN/////////////////dMMMs////////////////mMMMMMMMMMs///////////////sMMMy/////////////////dMMMMMMMMM/////////////////+MMMMMMMMM\n\r"\
				"22MMMMMMMMMMMMMMMMMMMMMMMMMMMMMo/////////////////sMMMMMd///////////////oNMMo////////////////sMMMMMMMMh////////////////+MMMMd/////////////////NMMN/////////////////hhhhhhhhhh////////////////sMMN+////////////////+MMMMMMMMMN/////////////////+MMMMMMMMM\n\r"\
				"23MMMMMMMMMMMMMMMMMMMMMMMMMMMMMh/////////////////oMMMMm+//////////////+NMMd/////////////////mMMMMMMMMs////////////////oMMMMy////////////////oMMMh///////////////////////////////////////////yMMh/////////////////sMMMMMMMMMm/////////////////oMMMMMMMMM\n\r"\
				"24MMMMMMMMMMMMMMMMMMMMMMMMMMMMMN//////////////////MMMN+//////////////+mMMMo////////////////+MMMMMMMMM+////////////////hMMMMo////////////////yMMMo///////////////////////////////////////////dMMo/////////////////dMMMMMMMMMh/////////////////sMMMMMMMMM\n\r"\
				"25MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMo/////////////////mMNo//////////////+mMMMN/////////////////yMMMMMMMMN/////////////////mMMMN/////////////////dMMM///////////////////////////////////////////+NMM+/////////////////mMMMMMMMMMy/////////////////yMMMMMMMMM\n\r"\
				"26MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMd/////////////////hNo//////////////+mMMMMh/////////////////dMMMMMMMMh////////////////+MMMMd/////////////////NMMm///////////////////////////////////////////yMMN/////////////////+MMMMMMMMMMo/////////////////dMMMMMMMMM\n\r"\
				"27MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM+////////////////os//////////////+mMMMMMs/////////////////NMMMMMMMMs////////////////oMMMMy////////////////oMMMm/////////////////osssssssssssssssssssssssssmMMd/////////////////oMMMMMMMMMN//////////////////NMMMMMMMMM\n\r"\
				"28MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMy///////////////////////////////+mMMMMMMo/////////////////NMMMMMMMM+////////////////yMMMMo////////////////yMMMm/////////////////dMMMMMMMMMMMMMMMMMMMMMMMMMMMMd/////////////////yMMMMMMMMMd/////////////////sMMMMMMMMMM\n\r"\
				"29MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMm//////////////////////////////+dMMMMMMM+/////////////////NMMMMMMMN/////////////////mMMMN/////////////////dMMMm/////////////////oMMMMMMMMMMMMMMMMMMMMMMMMMMMMd/////////////////yMMMMMMMMMo/////////////////mMMMMMMMMMM\n\r"\
				"30MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMo////////////////////////////+dMMMMMMMM+/////////////////hMMMMMMMh/////////////////NMMMd/////////////////NMMMN//////////////////smNMMMMMMMMMMMMMMMMMNNMMMMMMd/////////////////sMMMMMMMMm/////////////////sMMMMMMMMMMM\n\r"\
				"31MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMd///////////////////////////+mMMMMMMMMMs//////////////////ymmmmhs+////////////////oMMMMy////////////////+MMMMMs///////////////////oshddmmmmmmddhhysoooMMMMMMN/////////////////+NMMMMMMNo////////////////oNMMMMMMMMMMM\n\r"\
				"32MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM+/////////////////////////+mMMMMMMMMMMh////////////////////++////////////////////yMMMMo////////////////yMMMMMm+/////////////////////////////////////yMMMMMMMs/////////////////omNMMNdo////////////////+mMMMMMMMMMMMM\n\r"\
				"33MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMh////////////////////////+mMMMMMMMMMMMN+/////////////////////////////////////////dMMMM/////////////////dMMMMMMd+////////////////////////////////////mMMMMMMMNo//////////////////ooo+/////////////////oNMMMMMMMMMMMMM\n\r"\
				"34MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN+//////////////////////oNMMMMMMMMMMMMMh/////////////////////////////////////////NMMMm/////////////////NMMMMMMMms+/////////////////////////////////oMMMMMMMMMNs////////////////////////////////////+hMMMMMMMMMMMMMMM\n\r"\
				"35MMMMMMMNMMMMMMMMMMMMMMMMMMMMMMMMMMy/////////////////////oNMMMMMMMMMMMMMMMh+/////////////////////++///////////////oMMMMy////////////////+MMMMMMMMMMmy+///////////////////////////////hMMMMMMMMMMMdo////////////////////////////////ohNMMMMMMMMMMMMMMMM\n\r"\
				"36MMMMMMMmhdmNMMMMMMMMMMMMMMMMMMMMMMN+///////////////////sNMMMMMMMMMMMMMMMMMNyo+/////////////++shmN+///////////////yMMMMo////////////////yMMMMMMMMMMMMNmhso++/////////////////////+++oNMMMMMMMMMMMMMmyo+////////////////////////+oydNMMMMMMMMMMMMMMMMMM\n\r"\
				"37MMMMMMMMNdyyhdmNMMMMMMMMMMMMMMMMMMMdhhhhhhhhhhhhhhhhhhhNMMMMMMMMMMMMMMMMMMMMMmdhsssooossyhdmNMMMMhhhhhhhhhhhhhhhhNMMMMhhhhhhhhhhhhhhhhhmMMMMMMMMMMMMMMMMMNmmdhhyysssssssyyyhhddmmNNMMMMMMMMMMMMMMMMMMmdhyoo++++++/+++++++osyhdNMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"38MMMMMMMMMMNhyyyyhdNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNNNmmmmmmmmNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"39MMMMMMMMMMMMmyyyyyyhdNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"40MMMMMMMMMMMMMMdyyyyyyyhdNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"41MMMMMMMMMMMMMMMNhyyyyyyyyhmMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNNNNNNNNmmmmmmddddddhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhhhhhhhhhhhhdddddddmmmmmmmNNNNNNNNNMMMMMMMMMMMMMMMMM\n\r"\
				"42MMMMMMMMMMMMMMMMMdyyyyyyyyyhmMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNNNNNNmmmddddhhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhdddddddmMMMMMMMMMMMMMMM\n\r"\
				"43MMMMMMMMMMMMMMMMMMNyyyyyyyyyyhmMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNNNNmmdddhhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhhhddddddmmmmmmmNNNNNNNNNNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"44MMMMMMMMMMMMMMMMMMMNhyyyyyyyyyyhmMMMMMMMMMMMMMMMMMNNNNmmddhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhddddmmmmNNNNNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"45MMMMMMMMMMMMMMMMMMMMMdyyyyyyyyyyyhNMMMMMNNmmddhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhddmmmmNNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"46MMMMMMMMMMMMMMMMMMMMMMmyyyyyyyyyyyyhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhddmmmNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"47MMMMMMMMMMMMMMMMMMMMMMMNyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhddmmmNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"48MMMMMMMMMMMMMMMMMMMMMMMMNyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhddmmNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"49MMMMMMMMMMMMMMMMMMMMMMMMMNyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhddmmNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"50MMMMMMMMMMMMMMMMMMMMMMMMMMNyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhddmNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"51MMMMMMMMMMMMMMMMMMMMMMMMMMMNyyyyyyyyyyyyyyyyyyyyyyyyyhdmmNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"52MMMMMMMMMMMMMMMMMMMMMMMMMMMMmyyyyyyyyyyyyyyyyyhdmmNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"53MMMMMMMMMMMMMMMMMMMMMMMMMMMMMdyyyyyyyyyyhdmNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"54MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMyyyyhdmNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"55MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNmNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"56MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"57MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"58MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n\r"\
				"59MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
void _EXAMPLE_LOG(ACQUISITIONS_VAR var)
{
    static state_machine_t sm_example = {0};
    static state_machine_t sm_log = {0};
    static char str1[] = "String 1";
    static char *str2 = "String 2";
    static uint32_t val = 0x546389;
    static float val_f = 346.946;
        
    
    switch (sm_example.index)
    {
        case _SETUP:
            // Enable LOG with cfg_log(ENABLE, UART1) if using UART1.
            sm_example.index = _MAIN;
            break;
            
        case _MAIN:
            switch (sm_log.index)
            {
                case 0:
                    if (mTickCompare(sm_log.tick) >= TICK_200MS)
                    {
                        LOG_BLANCK("!! Start to LOG !!");
                        sm_log.index++;
                        sm_log.tick = mGetTick();
                    }
                    break;
                    
                case 1:
                    if (mTickCompare(sm_log.tick) >= TICK_200MS)
                    {
                        LOG("Binary log - @: %x - 32 digits: %32b - necessary digits: %b", (uint32_t) &val, val, val);
                        sm_log.index++;
                        sm_log.tick = mGetTick();
                    }
                    break;
                    
                case 2:
                    if (mTickCompare(sm_log.tick) >= TICK_200MS)
                    {
                        LOG("Octave log - @: %x - 10 digits: %10o - necessary digits: %o", (uint32_t) &val, val, val);
                        sm_log.index++;
                        sm_log.tick = mGetTick();
                    }
                    break;
                    
                case 3:
                    if (mTickCompare(sm_log.tick) >= TICK_200MS)
                    {
                        LOG("Decade log - @: %x - 5 digits: %5d - necessary digits: %d", (uint32_t) &val, val, val);
                        sm_log.index++;
                        sm_log.tick = mGetTick();
                    }
                    break;
                    
                case 4:
                    if (mTickCompare(sm_log.tick) >= TICK_200MS)
                    {
                        LOG("Hex log - @: %x - 10 digits: %10x - necessary digits: %x", (uint32_t) &val, val, val);
                        sm_log.index++;
                        sm_log.tick = mGetTick();
                    }
                    break;
                    
                case 5:
                    if (mTickCompare(sm_log.tick) >= TICK_200MS)
                    {
                        LOG("Float log (using p_float(var) is mandatory / result is approximative >3 digits after coma) - 5 digits: %5f - 1 digit: %1f - default 3 digits: %f", p_float(val_f), p_float(val_f), p_float(val_f));
                        sm_log.index++;
                        sm_log.tick = mGetTick();
                    }
                    break;
                    
                case 6:
                    if (mTickCompare(sm_log.tick) >= TICK_200MS)
                    {
                    LOG_SHORT("Char log - %c %c %c", 'a', 'B', 'c');
                        sm_log.index++;
                        sm_log.tick = mGetTick();
                    }
                    break;
                    
                case 7:
                    if (mTickCompare(sm_log.tick) >= TICK_200MS)
                    {
                        LOG_SHORT("Strings log short (case %d) - String + %s + %s + %s", sm_log.index, p_string("String 0"), p_string(str1), p_string(str2));
                        sm_log.index++;
                        sm_log.tick = mGetTick();
                    }
                    break;
                    
                case 8:
                    if (mTickCompare(sm_log.tick) >= TICK_200MS)
                    {
                        LOG_BLANCK("%s", p_string(VALEO));
                        sm_log.index++;
                        sm_log.tick = mGetTick();
                    }
                    break;
                    
                default:
                    if (mTickCompare(sm_log.tick) >= TICK_1S)
                    {
                        LOG("@: %x - Voltage = %fV - Current = %fA - Power Consumption = %fW - Temperature = %1f�C", (uint32_t) &var, p_float(var.voltage.average), p_float(var.current.average), p_float(var.power_consumption), p_float(var.ntc.temperature));
                        sm_log.tick = mGetTick();
                    }
                    break;
            }
            break;
    } 
}

void _EXAMPLE_WS2812B()
{
    WS2812B_DEF(ws2812b, SPI1, csRA0, 18, 6, 12, 18);   // Total of 3 segments: Seg0: 6 leds, Seg1: 6 leds & Seg2: 6 leds
    SWITCH_DEF(sw1, SWITCH1, ACTIVE_LOW);
    static WS2812B_ANIMATION animation;
    static state_machine_t sm_example = {0};
    static bool _execute = false;
    
    switch (sm_example.index)
    {
        case _SETUP:          
      
            sm_example.index = _MAIN;
            break;
            
        case _MAIN:
            if (sw1.is_updated)
            {
                sw1.is_updated = false;
                _execute = true;
            }
            
            switch (sw1.indice)
            {
                case 0:
                    // Do nothing
                    break;
                    
                case 1:
                    if (_execute)
                    {
                        _execute = false;
                        eWS2812BPutSegment(0, FIRST_LED, LAST_LED, COLOR_BLUE, COLOR_BLUE, WS2812B_EFFECT_NONE | WS2812B_LED_EFFECT_MIXED | WS2812B_RESTORE_COLOR, WS2812B_REPETITION_INFINITE, 0, &ws2812b);
                        eWS2812BPutSegment(1, FIRST_LED, LAST_LED, COLOR_WHITE, COLOR_WHITE, WS2812B_EFFECT_NONE | WS2812B_LED_EFFECT_MIXED | WS2812B_RESTORE_COLOR, WS2812B_REPETITION_INFINITE, 0, &ws2812b);
                        eWS2812BPutSegment(2, FIRST_LED, LAST_LED, COLOR_RED, COLOR_RED, WS2812B_EFFECT_NONE | WS2812B_LED_EFFECT_MIXED | WS2812B_RESTORE_COLOR, WS2812B_REPETITION_INFINITE, 0, &ws2812b);
                    }
                    break;
                    
                case 2:
                    if (_execute)
                    {
                        _execute = false;
                        eWS2812BSetParamsTraceur(0, FIRST_LED, LAST_LED, TICK_2S, TICK_2S, COLOR_RED, COLOR_RED, 1, WS2812B_LED_EFFECT_1, WS2812B_LED_EFFECT_1, WS2812B_REPETITION_INFINITE, 0, TICK_1S, &animation, ws2812b);
                    }
                    break;
                    
                case 3:
                    if (_execute)
                    {
                        _execute = false;
                        eWS2812BSetParamsChenillard(0, FIRST_LED, LAST_LED, 4, COLOR_RED, COLOR_RED, WS2812B_ANIM_TRACEUR_END_LOOP_TFwOffTF, WS2812B_LED_EFFECT_2, WS2812B_LED_EFFECT_2, WS2812B_REPETITION_INFINITE, -1, TICK_1S, &animation, ws2812b);
                    }
            }
            
            fu_switch(&sw1);
            eWS2812BAnimation(&animation, &ws2812b);
            eWS2812BFlush(TICK_100US, &ws2812b);
            break;
    }   
}