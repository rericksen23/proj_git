/********************************************************************************
* Copyright (C) 1998-2011 by Clearone Communications, Inc. All rights reserved. *
*                                                                               *
* PROPRIETARY INFORMATION OF CLEARONE COMMUNICATIONS, INC. DO NOT DISTRIBUTE.   *
*                                                                               *
* Confidential -- The contents of this material are confidential and proprietary*
* to ClearOne Communications and may not be reproduced, Published, or Disclosed *
* to others without company authorization.                                      *
********************************************************************************/

#ifndef _COMMANDLINE_H_
#define _COMMANDLINE_H_

//*********************************************************************
           
//Include files

//*********************************************************************
#define COMMAND_LINE_LENGTH 100
#define MAX_COMMAND_LENGTH  16
#define MAX_ADDRESS_LENGTH  8
#define CMD_DONE 1
#define CMD_ERROR   0
//#define TRUE 1
//#define FALSE 0

/*External Blackfin SDRAM MTLC8M16A2P-75(16Mbytes) Address Map
MAP:			0x000004 - 0xFFFFFF
Code Space:		0x000004 - 0x7FFFFF ( 8Mbytes )
Memory Buffer:	0x800000 - 0x8FFFFF ( 1 Mbyte block )
Memory 			0x900000 - 0xFFFFFF ( Remaining 7 Mbytes )
*/

//typedef unsigned char u8;
//typedef unsigned short u16;
//typedef unsigned long u32;

typedef struct {
    char name[MAX_COMMAND_LENGTH];
    int (*exec) (char *);
} COMMAND_TABLE_T;
          
extern char CommandLine[COMMAND_LINE_LENGTH];

//*********************************************************************
//Function Declarations
//*********************************************************************
extern void ProcessCommandLine(void);
extern int txt_RESET(char *);
//extern u8 txt_VER(char *);
//extern u8 txt_BFCLKS(char *);
extern void CMDLineRxChar(char);
extern void Handle_CMDLine(void);
extern void PrintCmdPrompt(void);
extern void reset(void);
//extern void CP2SerialRxChar(char);

#endif //_COMMANDLINE_H

/********************************************************************************
* Copyright (C) 1998-2011 by Clearone Communications, Inc. All rights reserved. *
********************************************************************************/

