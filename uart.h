/*********************************************************************************

Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/
/*!
* @file      uart.h
*
* @brief     This is the primary header file for UART character echo example.
*
*/

#ifndef _UART_H
#define _UART_H

#include <drivers\uart\adi_uart.h>
#include "bf700.h"



/*
 * User configurations
 */

/* Un-comment this line to use DMA */
//#define USE_DMA_MODE       

/* Baud rate to be used for char echo */
//#define BAUD_RATE           9600u
#define BAUD_RATE           115200u


/*
 * Macros used in the example
 */

/* Example result definitions */
#define FAILED              (-1)
#define PASSED              0

/* Macro for reporting errors */
#define REPORT_ERROR        printf



/*
 * Processor specific macros
 */

/* UART Device Number to test */
#define UART_DEVICE_NUM     0u
#define UART_DEVICE_1       1

/* default power settings */
//#define MHZTOHZ       (1000000)

//#define DF_DEFAULT    (0x0)
//#define MSEL_DEFAULT  (0x10)
//#define SSEL_DEFAULT  (0x8)
//#define CSEL_DEFAULT  (0x4)

//#define CLKIN         (25 * MHZTOHZ)
//#define CORE_MAX      (500 * MHZTOHZ)
//#define SYSCLK_MAX    (250 * MHZTOHZ)
//#define SCLK_MAX      (125 * MHZTOHZ)
//#define VCO_MIN       (72 * MHZTOHZ)

#define MODE_CMD_LINE       0
#define MODE_DANTE_CONSOLE  1

#define ADDR_CONNECT_UARTA  0x02

extern u8  gbl_uart0_mode;

extern ADI_UART_HANDLE  ghUART;
extern ADI_UART_HANDLE  ghUART1;


extern void Init_UART(void);
extern void Handle_UART(void);
extern void SendString (char *);

/********************************************************************************
* Text commands *
********************************************************************************/
extern int txt_BAUD(char *);
extern int txt_SAVE(char *);


#endif /* CHAR_ECHO_H */



