/*
 *  (C) Copyright 2014 - Analog Devices, Inc. All Rights Reserved.
 *
 * This software is proprietary and confidential.  By using this software
 * you agree to the terms of the associated Analog Devices License Agreement.
 *
 * Project Name:  	Power_On_Self_Test
 *
 * Hardware:		ADSP-BF706 EZ-Kit MINI
 *
 * Description:	This examples performs pushbutton and LED tests on the ADSP-BF706 EZ-Kit.
 */

#include <stdio.h>
#include <services/gpio/adi_gpio.h>
#include "fpga.h"
#include "commandline.h"
#include "uart.h"

#define FPGA_CLK    ADI_GPIO_PIN_6      //PB_06
#define FPGA_DATA   ADI_GPIO_PIN_7      //PB_07

#define HOST_ADDR_MASK      0xff
#define HOST_DATA_MASK_HI   0xfc
#define HOST_DATA_MASK_LOW  0xc0

void Init_FPGA()
{
    int temp;

    //Setup FPGA transfer lines
    temp = FPGA_CLK | FPGA_DATA;

	adi_gpio_SetDirection(
	    ADI_GPIO_PORT_B,
	    temp,
	    ADI_GPIO_DIRECTION_OUTPUT);

    adi_gpio_Clear(ADI_GPIO_PORT_B, temp);

    read_fpga(0);       //one read to get aligned with fpga
}

void write_fpga(int address, int data)
//pb06 used for clock - fpga_data0
//pb07 used for data  - fpga_data1
{
    int i, temp;

    //set data bus pin as output to write address and data
	adi_gpio_SetDirection(
	    ADI_GPIO_PORT_B,
	    FPGA_DATA,
	    ADI_GPIO_DIRECTION_OUTPUT);

    temp = address << 1;        //set lsb to 0 for a write

    //write the address on the dsp bus
    for (i=0; i<8; i++) {
        if (temp & 0x80) {
            //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_DATA);
            *pREG_PORTB_DATA_SET = FPGA_DATA;
        }
        else {
            //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_DATA);
            *pREG_PORTB_DATA_CLR = FPGA_DATA;
        }

        //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_CLK);    //toggle clock
        //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_CLK);
        *pREG_PORTB_DATA_SET = FPGA_CLK;        //toggle clock
        *pREG_PORTB_DATA_CLR = FPGA_CLK;


        temp = temp << 1;

    }

    //write 16 bit data to the dsp bus
    temp = data;
    for (i=0; i<16; i++) {

        if (temp & 0x8000) {
            //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_DATA);
            *pREG_PORTB_DATA_SET = FPGA_DATA;
        }
        else {
            //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_DATA);
            *pREG_PORTB_DATA_CLR = FPGA_DATA;
        }

        //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_CLK);    //toggle clock
        //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_CLK);
        *pREG_PORTB_DATA_SET = FPGA_CLK;        //toggle clock
        *pREG_PORTB_DATA_CLR = FPGA_CLK;

        temp = temp << 1;
    }

    //toggle clock to lower write enable strobe in fpga
    //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_CLK);    //toggle clock
    //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_CLK);
    *pREG_PORTB_DATA_SET = FPGA_CLK;        //toggle clock
    *pREG_PORTB_DATA_CLR = FPGA_CLK;

    //toggle clock to raise write enable strobe in fpga
    //this also matches the same number of clocks as a read
    //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_CLK);    //toggle clock
    //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_CLK);
    *pREG_PORTB_DATA_SET = FPGA_CLK;        //toggle clock
    *pREG_PORTB_DATA_CLR = FPGA_CLK;

    //toggle clock to raise chip select in fpga
    //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_CLK);    //toggle clock
    //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_CLK);
    *pREG_PORTB_DATA_SET = FPGA_CLK;        //toggle clock
    *pREG_PORTB_DATA_CLR = FPGA_CLK;

}

int read_fpga(int address)
//pb06 used for clock - fpga_data0
//pb07 used for data  - fpga_data1
{
    int i, data;
    uint32_t temp;

    //set data bus pin as outputs to first write address
	adi_gpio_SetDirection(
	    ADI_GPIO_PORT_B,
	    FPGA_DATA,
	    ADI_GPIO_DIRECTION_OUTPUT);

    temp = address << 1;
    temp |= 0x1;        //add in the rw bit to signify read;


    //write the address on the dsp bus
    for (i=0; i<8; i++) {
        if (temp & 0x80) {
            //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_DATA);
            *pREG_PORTB_DATA_SET = FPGA_DATA;
        }
        else {
            //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_DATA);
            *pREG_PORTB_DATA_CLR = FPGA_DATA;
        }

        //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_CLK);    //toggle clock
        //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_CLK);
        *pREG_PORTB_DATA_SET = FPGA_CLK;        //toggle clock
        *pREG_PORTB_DATA_CLR = FPGA_CLK;

        temp = temp << 1;

    }

    //read 16 bit data from the dsp bus
    //set data bus pins as inputs to read data
	adi_gpio_SetDirection(
	    ADI_GPIO_PORT_B,
	    FPGA_DATA,
	    ADI_GPIO_DIRECTION_INPUT);

    //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_CLK);    //toggle clock to set output enable
    //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_CLK);
    *pREG_PORTB_DATA_SET = FPGA_CLK;        //toggle clock
    *pREG_PORTB_DATA_CLR = FPGA_CLK;

    //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_CLK);    //toggle clock to latch in read data
    //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_CLK);  //and to clear output enable        
    *pREG_PORTB_DATA_SET = FPGA_CLK;        //toggle clock
    *pREG_PORTB_DATA_CLR = FPGA_CLK;

    data = 0;
    for (i=0; i<16; i++) {
        temp = 0;
        data = data << 1;

        //read data
        //adi_gpio_GetData(ADI_GPIO_PORT_B, &temp);    
        temp = *pREG_PORTB_DATA;
        if (temp & FPGA_DATA) {
            data |= 0x1;
        }

        //adi_gpio_Set(ADI_GPIO_PORT_B, FPGA_CLK);    //toggle clock
        //adi_gpio_Clear(ADI_GPIO_PORT_B, FPGA_CLK);  
        *pREG_PORTB_DATA_SET = FPGA_CLK;        //toggle clock
        *pREG_PORTB_DATA_CLR = FPGA_CLK;
    }

    return data;
}

/********************************************************************************
* Text commands *
********************************************************************************/
int txt_FPGAW(char *pCommandLine)
{
    char string[40];
    char *rem;
    int value, addr;
    int data;

    if (*pCommandLine != NULL) {
        addr = strtoul(pCommandLine, &rem, 16);
        pCommandLine = rem;
    }
    else {
        SendString("FLASHW <address> <value>\r\n");
        return CMD_DONE;
    }
    if (*pCommandLine != NULL) {
        value = strtol(pCommandLine, &rem, 16);
    }
    else {
        return CMD_ERROR;
    }

    write_fpga(addr, value);

    return CMD_DONE;
}

int txt_FPGAR(char *pCommandLine)
{
    char string[40];
    char *rem;
    int i, length, addr, data;

    if (*pCommandLine != NULL) {
        addr = strtoul(pCommandLine, &rem, 16);
        pCommandLine = rem;
    }
    else {
        SendString("FPGAR <address> <length>\r\n");
        return CMD_DONE;
    }
    if (*pCommandLine != NULL) {
        length = strtol(pCommandLine, &rem, 16);
    }
    else {
        return CMD_ERROR;
    }

    //ignore length for now
    for(i=0; i<length; i++)
    {
        data = read_fpga(addr);
        addr += 1;
        sprintf(string, "%X\r\n", data);
        SendString(string);
    }

    return CMD_DONE;
}

int txt_FPGARESET(char *pCommandLine)
//This toggles the FPGA config line
{
    write_fpga(REMOTE_CLK_CTRL, REMOTE_RECONFIG);
    return CMD_DONE;
}


/********************************************************************************
* Plink commands *
********************************************************************************/
u8 cmd_FPGATST(CtrlPacket_ *pkt)
{
    char String[40];
    u16 address, data;

    SendString("FPGATST command\r\n");

    //Show_Command_Packet(pkt);

    address = byteswap4(pkt->CommandWords[0]);

    if (pkt->Flags & PKT_FLAG_QUERY) {
        //if here then command is a query
        data = read_fpga(address);
        pkt->CommandWords[1] = byteswap4(data);
    }
    else {
        //if here then command is a write
        data = byteswap4(pkt->CommandWords[1]);
        write_fpga(address, data);
    }

    pkt->Flags &= ~PKT_FLAG_QUERY;      //turn off query bit if set

    SendGlinkPacket(pkt);

    return CMD_DONE;
}

u8 cmd_FPGAVER(CtrlPacket_ *pkt)
{
    char String[40];
    //u16 month, day, year, hour, minute, temp;
    u16 version;
    u8 month, day, year, build;
    char *ptr;

    SendString("FPGAVER command\r\n");

    //Show_Command_Packet(pkt);
    pkt->Flags &= ~PKT_FLAG_QUERY;      //turn off query bit

    //FPGA version
    version = read_fpga(FPGA_VERSION_MONTH_DAY);
    month = version >> 8;
    day = version & 0xff;
    version = read_fpga(FPGA_VERSION_YEAR_BUILD);
    year = version >> 8;
    build = version & 0xff;

    #pragma diag(errors)        //suppress warning given for next line
    ptr = (char *)&pkt->CommandWords[0];
    sprintf(ptr,"%X/%X/%X %02x", month, day, year, build);

    SendGlinkPacket(pkt);

    return CMD_DONE;
}

