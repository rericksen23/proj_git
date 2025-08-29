/*
 *  (C) Copyright 2014 - Analog Devices, Inc. All Rights Reserved.
 *
 * This software is proprietary and confidential.  By using this software
 * you agree to the terms of the associated Analog Devices License Agreement.
 *
 * Project Name:  	Power_On_Self_Test
 * Hardware:		ADSP-BF706 Ez-Kit MINI
 *
 * Description:	This example performs POST on the ADSP-BF706 Ez-Kit MINI.
 *
 * Please view the readme.html file for detailed information related to
 * switch and jumper settings, LED indicators, etc.
 *
 * Enter POST standard test loop by pressing PB1 at startup, or by entering
 * a sequence using PB1 to choose a specific test.
 */

#include "bf700.h"
#include "post_common.h"
#include "adi_initialize.h"
#include "uart.h"
#include "commandline.h"
#include "memory.h"
#include "flash.h"
#include "fpga.h"
#include "plink.h"
#include "gain.h"
#include "spi0_flash.h"
#include "twi.h"
#include "meters.h"
#include "gpio.h"
#include "PWR_Freq_Mode.h"
#include "analog_gain.h"

int main()
{
	/**
	 * Initialize managed drivers and/or services that have been added to
	 * the project.
	 * @return zero on success
	 */
	adi_initComponents();

	adi_osal_Init();
    
    /* Initialize Power service */
    power_init();
    
	Init_Timers();
	Init_Timer_Interrupts();

    Init_LEDs();
    Init_FPGA();

    Init_Flash();       //must come before Init_UART()
    Init_UART();
    Init_SPI0Flash();

#if USE_EEPROM
    Init_TWI0();
#endif
    Init_UnitValues();
    //Check_FPGA();

    Init_GPIO();
    Init_Gain();
    //Init_bhip();

    SendString("\r\nBF700 BF1 running...\r\n");

    Init_FFS();
    send_DUPDATE_pkt("RESET");     
    PrintCmdPrompt();

    //comment added in master

    while (1) {
        Handle_UART();
        Handle_CMDLine();
        Handle_Plink();
        Update_eeprom();
        Handle_Meters();
        Handle_GPIO();
    }

}

