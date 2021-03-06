# PROTOTYPE LIBRARY

**Last release ver. *December 18***

## HARDWARE BASED ON:

Target: ***SPCB*** - (**S**)tandard (**P**)rototype (**C**)alculator (**B**)oard

- BLE PICKIT rev.C (*Prototype dev.*) or ETH PICKIT
- Ethernet Starter KIT II (*PIC32MX795F512L*)
- PICAdapter rev.D (*Prototype dev.*)


## HOW TO USE THE LIBRARY:

Download the last PLIB project. It is a MPLAB X Library Project with gcc compilation parameters set to 3 (Optimize yet more favoring speed).

First, you have to include ***PLIB.h***, which includes all library's header file, in your project:
* In your project **BLANCK_PROJECT**, open **config.h** and at the top of the file, include the **PLIB.h** following the directory where it is stored. 
* Then include the file **PLIB.h** in your **Header folder** project. Right click on the **Header folder** and **Add Existing Item...**. Search and select the file to include it in your project.

Now you have to include the **Library project** in your project:

* Right click on your **project --> Properties --> Conf: [default] --> Libraries**.
* Click on **Add Library Project...** and select **PLIB.X** (*the MPLABX IDE library project*).
* Remove previous library if present.

## LIBRARY STATUS

<img width="951" alt="test" src="https://user-images.githubusercontent.com/44413525/49344862-f1542000-f67c-11e8-82e9-d5e20e313c85.png">

Name | Validated | Commented | Autonomous | Example | Dependencies | IRQs
 :--- | :---: | :---: | :---: | :---: | :---: | :---: 
**Low Level** | ************ | ************ | ************ | ************ | ************ | ************
*s08_interrupt_mapping* | yes | yes | yes | | |
*s12_ports* | yes |  |  |  | |
*s14_timers* | yes | yes | yes | | |
*s16_output_compare* | | | | | T2 & T3 |
*s17_adc* | | | | | |
*s23_spi* | | | | | T1 & GPIO & \*DMAx |
*s24_i2c* | | | | | T1 |
*s34_can* | | | | | T1 |
*s35_ethernet* | | | | | T1 |
**High Level** | ************ | ************ | ************ | ************ | ************ | ************
*software_pwm (config.c/h)* | yes | yes | yes | | T5 & GPIO | T5
*utilities* | | | | | T1 & ADC | -
*string_advance* | | | | | | -
*one_wire_communication* | | | | | | -
*lin* | | | | | T4 & UART*2* & UART*5* | T4 & UART_RX
*ble* | | | | | T1 & UART*4* & DMA*2* | UART_RX & DMA_TX
**External Components** | ************ | ************ | ************ | ************ | ************ | ************
*25lc512* | | | | | SPI*x* & DMA*x* |
*mcp23s17* | | | | | SPI*x* & DMA*x* |
*ws2812b* | | | | | SPI*x* & DMA*x* |
*qt2100* | | | | | SPI*x* & DMA*x* |
*amis30621* | | | | | LIN*2* & LIN*5* |
*tmc429* | | | | | SPI*x* & DMA*x* |
**Experimental** | ************ | ************ | ************ | ************ | ************ | ************
*EXP_log* | | | yes | yes | UART*x* & DMA*x* | -
*EXP_s21_uart* | | yes | yes | | | -
