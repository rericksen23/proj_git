/********************************************************************************
* Copyright (C) 1998-2011 by Clearone Communications, Inc. All rights reserved. *
*                                                                               *
* PROPRIETARY INFORMATION OF CLEARONE COMMUNICATIONS, INC. DO NOT DISTRIBUTE.   *
*                                                                               *
* Confidential -- The contents of this material are confidential and proprietary*
* to ClearOne Communications and may not be reproduced, Published, or Disclosed *
* to others without company authorization.                                      *
********************************************************************************/

#include <stdio.h>              
#include <stdlib.h>            
#include <string.h>
#include <defBF700.h>
#include <ctype.h>
#include "common/flash.h"
#include "commandline.h" 
//#include "twi.h"
#include "version.h"
//#include "flash.h"
#include "bf700.h"
#include "settings.h"
#include "meters.h"
#include "gpio.h"
#include "twi.h"
#include "gain.h"
#include "flash.h"
#include "matrix.h"
#include "xmos_flash.h"
#include "fpga.h"
#include "uart.h"
#include "memory.h"
#include "spi0_flash.h"
#include "fpga_i2c.h"
#include "analog_gain.h"
#include "aes.h"


#define SPACE_BAR 0x20
#define TRUE 1
#define FALSE 0

//*********************************************************************
//Variable Declarations
//*********************************************************************
//static u8 KeyPressValue;
//_CommandLineStateVariable CommandLineStateVariable = COMMAND_LINE_IDLE;
char CommandLine[COMMAND_LINE_LENGTH];
char *pCommandLine;
char CommandLineIndex;

//static u8 KeyPressValue;
//bool CMDLinemenuprocessed = false;
//bool FPGABusEnabled = false;
//unsigned char CommandLineResult;
//bool CMDLineProcessingComplete = false;

const COMMAND_TABLE_T COMMAND_TABLE[];
const int COMMAND_TABLE_SIZE;
char CR_flag;

//void CloseCMDLine(void);
//u8 txt_ShowHelp(void);
//*********************************************************************
//Function Definitions
//*********************************************************************

void ProcessCommandLine(void)
{
	char *pCommand;
	int CommandLength = 0;
    char cmdMatch, cmdNum, rxChar;

	//Get the command
	pCommandLine 	= &CommandLine[0];
	pCommand		= pCommandLine;
	 
	while (	(*pCommandLine != SPACE_BAR) && (*pCommandLine != NULL) && (CommandLength < MAX_COMMAND_LENGTH))
	{
        rxChar = *pCommandLine++;
		*pCommand++ = toupper(rxChar);    //convert command to all upper case
		CommandLength++;	
	}

    if (CommandLength > 0) {   //if something other than enter key pressed
        *pCommand = '\0';
        //Pass the command along for processing
        pCommand = &CommandLine[0];
        if(CommandLength >= MAX_COMMAND_LENGTH )
        {
            CommandLength = 0;
            SendString("Exceeded max command length.\n\r");
        }
        else {
            cmdNum=0;
            do {
                cmdMatch = !strcmp(pCommand, COMMAND_TABLE[cmdNum++].name);
            } while ((!cmdMatch) && (cmdNum<COMMAND_TABLE_SIZE));
            cmdNum--;   //decrement back to found command
            if (cmdMatch) {
                pCommandLine++; //advance to command arguments
                //execute command function if command found
                if (COMMAND_TABLE[cmdNum].exec(pCommandLine) == CMD_ERROR) {
                    SendString("Argument error\r\n");
                }
            }
            else {
                //Bad command
                SendString("Command not recognized.\n\r");
            }
        }
    }
}

void reset()
{
    int i;

    *pREG_RCU0_CTL |= BITM_RCU_CTL_SYSRST;      //reset system peripherals

    for (i=0; i<75; i++) {
    }

    *pREG_RCU0_CTL = 0;

    asm("RAISE 1;");        //issue software reset for core

}

//*****************************************************************
//Function :	txt_VER()
//Parameters:	None
//Description:	Read code versions for: Blackfin, FPGA
//***************************************************************** 	
int txt_VER(char *pCommandLine)
{
    char string[128];
    FileStruct_ *fp;
    u16 version;
    u8 month, day, year, build;
    int flash_addr;
    char buf[256];
    char *ptr;

    flash_set_mode(flash_info, QUAD_INPUT);

    flash_addr = MONDO_HEADER_ADDR;
    flash_read(flash_info, flash_addr, (u8 *)buf, sizeof(buf));
    fp = &MondoFile;
    Enumerate_File_Header((char *)buf, fp, flash_addr);

    ptr = buf;
    ptr += sprintf(ptr, "Mondo File:  %s\r\n", fp->version);


	//Blackfin Firmware revision.
    ptr += sprintf(ptr,"Blackfin BF700:\t%d.%d.%d.%d\r\n",
            VERSION_MAJOR, VERSION_MINOR, VERSION_RELEASE, VERSION_ENGINEERING);

    //FPGA version
    version = read_fpga(FPGA_VERSION_MONTH_DAY);
    month = version >> 8;
    day = version & 0xff;
    version = read_fpga(FPGA_VERSION_YEAR_BUILD);
    year = version >> 8;
    build = version & 0xff;
    ptr += sprintf(ptr, "FPGA:\t\t%x/%x/%x %02x\r\n", month, day, year, build);

    SendString(buf);
    //send_udp_text(buf);

    return CMD_DONE;
	
}

int txt_TEST(char *pCommandLine)
{
    char string[20];
    char *rem;
    int arg1, arg2, arg3;

    arg1 = (int)strtol(pCommandLine, &rem, 16);
    pCommandLine = rem;
    arg2 = (int)strtol(pCommandLine, &rem, 16);
    pCommandLine = rem;
    arg3 = (int)strtol(pCommandLine, &rem, 16);
    pCommandLine = rem;

    sprintf(string, "arg1 is %X\r\n", arg1);
    SendString(string);
    sprintf(string, "arg2 is %X\r\n", arg2);
    SendString(string);
    sprintf(string, "arg3 is %X\r\n", arg3);
    SendString(string);

    return CMD_DONE;
}

int txt_RESET(char *pCommandLine)
{
    reset();

    return CMD_DONE;
}


int txt_ShowHelp(char *pCommandLine)
{
    SendString("Type ? or HELP to show command list\r\n"); 
	SendString("Command List:\r\n");
    SendString("TEST\r\n");
    SendString("VER\r\n");
#if 0
    SendString("BAUD <9600, 57600, 115200, 230400, 460800, 921600>\r\n");
    SendString("BROWSE\r\n");
    SendString("BSPI\r\n");
    SendString("CONFIG\r\n");
    SendString("CONMON\r\n");
    SendString("DANTEAPPVER\r\n");
    SendString("DANTEBRIDGE\r\n");
    SendString("DANTECAPVER\r\n");
    SendString("DANTEADDR\r\n");
    SendString("DANTEEVBRD\r\n");
    SendString("DANTEINCHAN\r\n");
    SendString("DANTELINK\r\n");
    SendString("DANTEMAC\r\n");
    SendString("DANTENAME\r\n");
    SendString("DANTEOUTCHAN\r\n");
    SendString("DANTERESET\r\n");
    SendString("DANTEUPDATE\r\n");
    SendString("DANTEVER\r\n");
    SendString("DEFAULT\r\n");
    SendString("DEVICE\r\n");
    SendString("DID\r\n");
    SendString("DIGMICSEN\r\n");
#if USE_EEPROM
    SendString("EEPROMR <address> <length>\r\n");
    SendString("EEPROMW <address> <data>\r\n");
#endif
    SendString("ERASEF <address>\n\r");
    SendString("ERASEFALL - erase entire contents of flash\n\r");
    SendString("FPGAR <address> <length>\r\n");
    SendString("FPGAW <address> <data>\r\n");
    SendString("FPROML - load FPGA fprom\n\r");
    SendString("GLTEXT - send text to proxy\r\n");
    SendString("I2CR <address> <length>\r\n");
    SendString("I2CW <address> <data> <length>\r\n");
    SendString("IDENTIFY\r\n");
    //SendString("ISVALID - check if settings structures contain valid data\r\n");
    SendString("LEDTEST\r\n");
    SendString("LOAD - load Blackfin, FPGA, Dante or all (.mdo file)\n\r");
    SendString("LOCROOM - set or report room string\r\n");
    SendString("MTRX <input> <output> <set>\r\n");
    SendString("MTRXCLEAR\r\n");
    SendString("MUTE\r\n");
    SendString("NAME - set or report Unit Name\r\n");
    SendString("NETWORK\r\n");
    SendString("OPEN\r\n");
    SendString("ORDER\r\n");
    SendString("PLLTEST - 0, 1, 2 for test off, min, max\r\n");
    SendString("READF <address> <length>\n\r");
    SendString("READM <address> <length>\n\r");
    SendString("RESET\n\r");
    SendString("SAVE - Uart baud rate to flash\r\n");
    SendString("SERIALNO - set or report unit serial number\r\n");
    SendString("SETTINGS - show unit settings\r\n");
    SendString("SHOWFFS - show flash file system\r\n");
    SendString("SHOWTX - debug show glink tx data\r\n");
    SendString("SPI\r\n");
    SendString("SUB\r\n");
    SendString("SUBS\r\n");
    SendString("SUBSCLEAR\r\n");
    SendString("UID\r\n");
    SendString("WRITEM <address> <data>\n\r");
    SendString("WRITEF <address> <data>\n\r");
	SendString("VER - show versions\n\r");
    SendString("\r\n");
#endif
    return CMD_DONE;
			
}

int txt_Generic(char *pCommandLine)
{
    SendString("Valid command\r\n");
    return CMD_DONE;
}

void CMDLineRxChar(char rxChar)
{
    if (rxChar == '\b') {           //check for backspace
        if (CommandLineIndex > 0) { //if characters in buffer then delete
            CommandLineIndex--;
        }
    }
    else if (rxChar == '\n') {      //discard newline character
    }
    else if (rxChar == '\r') {      //carriage return
        CR_flag = TRUE;             //flag a new command to be processed
        CommandLine[CommandLineIndex++] = NULL;     //replace CR with NULL
        CommandLine[CommandLineIndex] = NULL;       //add another null to wipe out previous arguments
        CommandLineIndex = 0;       //reset index
    }
    else {                          //all other characters
        CommandLine[CommandLineIndex++] = rxChar;       //toupper(rxChar);
        if (CommandLineIndex >= COMMAND_LINE_LENGTH) {    //prevent pointer from exceeding the bounds of rx buffer
            CommandLineIndex = COMMAND_LINE_LENGTH-1;
        }
    }
}

void PrintCmdPrompt(void)
{
    SendString("> ");
}

void Handle_CMDLine(void)
{
    if (CR_flag == TRUE) {
        CR_flag = FALSE;
        SendString("\r\n");             //insert new line
        ProcessCommandLine();
        PrintCmdPrompt();
    }
}

const COMMAND_TABLE_T COMMAND_TABLE[] = 
{
    "?",            txt_ShowHelp,
    "HELP",         txt_ShowHelp,
    "TEST",         txt_TEST,
    "READM",        txt_READM,
    //"READMF",       txt_READMF,
    "WRITEM",       txt_WRITEM,
    "LOAD",         txt_LOAD,
    "FLASHI",       txt_FLASHI,
    "FLASHEN",      txt_FLASHEN,
    "FLASHR",       txt_FLASHR,
    "FLASHW",       txt_FLASHW,
    "FLASHSE",      txt_FLASHSE,
    "XFLASHR",      txt_XFLASHR,
    "XFLASHW",      txt_XFLASHW,
    "XFLASHSE",     txt_XFLASHSE,
    "XFLASHCTRL",   txt_XFLASHCTRL,
    "XFLASHUP",     txt_XFLASHUP,
    "XFLASHGU",     txt_XFLASHGU,
    "XFLASHBE",     txt_XFLASHBE,
    //"DANTERESET",   txt_DANTERESET,
    "FPGAW",        txt_FPGAW,
    "FPGAR",        txt_FPGAR,
    "FPGARESET",    txt_FPGARESET,
    "MTRX",         txt_MTRX,
    "MTRXCLEAR",    txt_MTRXCLEAR,
    "SHOWMTRX",     txt_SHOWMTRX,
    "RESET",        txt_RESET,
    "BAUD",         txt_BAUD,
    "SAVE",         txt_SAVE,
    "GAIN",         txt_GAIN,
    "MUTE",         txt_MUTE,
    //"LED",          txt_LED,
    //"LEDMODE",      txt_LEDMODE,
    //"A2BSLED",      txt_A2BSLED,
    //"A2BERR",       txt_A2BErr,

    "IOSEL",        txt_IOSEL,
    "MGAIN",        txt_MGAIN,
    "PP",           txt_PP,

#if USE_EEPROM
    // I2C
    "EEPROMT",      txt_EEPROMT,
    "EEPROMW",      txt_EEPROMW,
    "EEPROMR",      txt_EEPROMR,
    "I2CW",         txt_I2CW,
    "I2CR",         txt_I2CR,
#endif

    //Settings
    "DEFAULT",      txt_DEFAULT,
    "LOCROOM",          txt_LOCROOM,
    "NAME",             txt_NAME,
    "SETTINGS",         txt_SETTINGS,
    "SERIALNO",         txt_SERIALNO,
    "UID",              txt_SERIALNO,
    "PCBNUM",           txt_PCBNUM,
    "ISVALID",          txt_ISVALID,
    "DANTENAME",        txt_DANTENAME,

    "LVL",          txt_LVL,
    //"METERS",       txt_METERS,
    //"METERSEN",     txt_METERSEN,

    "LEDTEST",      txt_LEDTEST,
    "LOCATE",       txt_LOCATE,
    "DISABLE",      txt_DISABLE,
    "PSED",         txt_PSED,
    "GPI",          txt_GPI,
    "INIT",         txt_INIT,
    "UNITID",       txt_UNITID,
    "SHOWTX",       txt_SHOWTX,


#if USE_BDY_SPI
    "SPIR",         txt_SPIR,
#endif

    "READAES",      txt_READAES,

    "VER",          txt_VER

#if 0
    "READM",        txt_READM,
    "WRITEM",       txt_WRITEM,
    "WRITEF",       txt_WRITEF,
    "ERASEF",       txt_ERASEF,
    "ERASEFALL",    txt_ERASEFALL,
    "READF",        txt_READF,
    //"READFS",       txt_READFS,
    "LOAD",         txt_LOAD,
    "VER",          txt_VER, 
    //"BFCLKS",       txt_BFCLKS,
    "FPROML",       txt_FPROML,
    "SHOWFFS",      txt_SHOWFFS,
    "DID",          txt_DID,
    "FPGAR",        txt_FPGAR,
    "FPGAW",        txt_FPGAW,
    "PLLTEST",      txt_PLLTEST,
    "SAVE",         txt_SAVE,
    "GLTEXT",       txt_GLTEXT,

    "XFLASHW",      txt_XFLASHW,
    "XFLASHR",      txt_XFLASHR,
    "XFLASHSE",     txt_XFLASHSE,
    //"DFDUMP",       txt_DFDUMP,

    "FPGARESET",    txt_FPGARESET,
    "REMOTER",      txt_REMOTER,
    "REMOTEW",      txt_REMOTEW,
    "REMOTECLR",    txt_REMOTECLR,
    "FPGAAPP",      txt_FPGAAPP,
    "FPGAMODE",     txt_FPGAMODE,



#endif
};

const int COMMAND_TABLE_SIZE = sizeof(COMMAND_TABLE)/sizeof(COMMAND_TABLE_T);

/********************************************************************************
* Copyright (C) 1998-2011 by Clearone Communications, Inc. All rights reserved. *
********************************************************************************/

