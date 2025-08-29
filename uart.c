/*********************************************************************************

Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/
/*!
* @file      char_echo.c
*
* @brief     This is the primary source file for UART character echo example.
*
*/

//#ifdef _MISRA_RULES
//#pragma diag(push)
//#pragma diag(suppress:misra_rules_all:"suppress all MISRA rules for test")
//#endif /* _MISRA_RULES */


#include "uart.h"
#include <stdio.h>
#include <string.h>
#include <services\int\adi_int.h>
#include <services\pwr\adi_pwr.h>
#include <services\int\adi_sec.h>
#include "common/flash.h"
#include "commandline.h"
#include "flash.h"
#include "fpga.h"

#define BAUD_57600     1
#define BAUD_115200    2
#define BAUD_230400    3
#define BAUD_460800    4
#define BAUD_921600    5

#define BAUD_RATE_57600         0x6C8
#define BAUD_RATE_115200        0x364
#define BAUD_RATE_230400        0x1B2
#define BAUD_RATE_460800        0xD9
#define BAUD_RATE_921600        0x6D


#define NUM_UARTS       1

//UART1 enable bits
#define UART0_TX_PORTB_MUX  ((uint32_t) ((uint32_t) 0<<16))
#define UART0_RX_PORTB_MUX  ((uint32_t) ((uint32_t) 0<<18))

#define UART1_TX_PORTC_MUX  ((uint32_t) ((uint32_t) 0<<16))
#define UART1_RX_PORTC_MUX  ((uint32_t) ((uint32_t) 0<<18))

#define UART1_TX_PORTC_FER  ((uint32_t) ((uint32_t) 1<<0))
#define UART1_RX_PORTC_FER  ((uint32_t) ((uint32_t) 1<<1))

#define UART0_TX_PORTB_FER  ((uint32_t) ((uint32_t) 1<<8))
#define UART0_RX_PORTB_FER  ((uint32_t) ((uint32_t) 1<<9))



/* Number of Tx and Rx buffers */
#define NUM_BUFFERS     2

/* ADI initialization header */
#include "adi_initialize.h"
/* UART Handle */
//static ADI_UART_HANDLE  ghUART;
ADI_UART_HANDLE  ghUART;
ADI_UART_HANDLE  ghUART1;

/* Rx and Tx buffers */
static uint8_t RxBuffer[NUM_BUFFERS];
static uint8_t RxBuffer1[NUM_BUFFERS];

/* Memory required for operating UART in interrupt mode */
static uint8_t  gUARTMemory[ADI_UART_BIDIR_DMA_MEMORY_SIZE];
static uint8_t  gUART1Memory[ADI_UART_BIDIR_DMA_MEMORY_SIZE];

uint8_t baud_setting;
u8  gbl_uart0_mode = MODE_CMD_LINE;
volatile u8  sw_uart_mode;
volatile u8  uarta_tx_head;
volatile u8  uarta_tx_tail;

void Init_Uart0(void);
int GetChar(uint8_t *cVal);
void SendChar(uint8_t cVal);

#pragma optimize_for_speed /* enable the compiler optimizer */
/* dispatched interrupt handler declaration */
EX_DISPATCHED_HANDLER_STATIC_NESTED(UartR_ISR, iid, idx, callback);
 
/* dispatched interrupt handler definition */
EX_DISPATCHED_HANDLER_STATIC_NESTED(UartR_ISR, iid, idx, callback) 
{
    uint8_t cVal;

    if (GetChar(&cVal)) {
        if (serial_load_flag) {
            serial_load_buf[serial_load_head++] = cVal;
        }
        else {
            if (cVal == '$') {
                sw_uart_mode = 1;
                uarta_tx_head = 0;    
                uarta_tx_tail = 0;
            }
            else {
                if (gbl_uart0_mode == MODE_CMD_LINE) {
                    CMDLineRxChar(cVal);
                    SendChar(cVal);
                }
                else {          //MODE_DANTE_CONSOLE
                    serial_load_buf[uarta_tx_head++] = cVal;
                }
            }
        }
    }
}

void Init_Uart0()
{

   volatile uint32_t Rtlcheck;
   ADI_INT_STATUS Intcheck;
   u32 baud;

   //get saved baud rate from flash
   flash_read(flash_info, BAUD_FLASH_ADDR, &baud_setting, 1);

   switch (baud_setting) {
   case BAUD_57600:
       baud = BAUD_RATE_57600;
       break;
   case BAUD_115200:
       baud = BAUD_RATE_115200;
       break;
   case BAUD_230400:
       baud = BAUD_RATE_230400;
       break;
   case BAUD_460800:
       baud = BAUD_RATE_460800;
       break;
   case BAUD_921600:
       baud = BAUD_RATE_921600;
       break;
   default:
       baud = BAUD_RATE_115200;
       break;
   }



   *pREG_PORTB_FER_SET = BITM_PORT_FER_SET_PX8 | BITM_PORT_FER_SET_PX9;                   /* Setup portB8/9 for Uart0 */
   *pREG_PORTB_MUX &= ~(BITM_PORT_MUX_MUX8|BITM_PORT_MUX_MUX9);                   /* Setup portB8/9 for Uart0 */
   //*pREG_UART0_CLK = BITM_UART_CLK_EDBO|(BITM_UART_CLK_DIV & BAUD_RATE_115200);     
   *pREG_UART0_CLK = BITM_UART_CLK_EDBO | (BITM_UART_CLK_DIV & baud);
   *pREG_UART0_CTL = ENUM_UART_CTL_WL8BITS|ENUM_UART_CTL_CLK_EN;

   Intcheck = adi_int_InstallHandler(INTR_UART0_STAT, NULL, /*callback param*/NULL, /*enable*/false);
    if (Intcheck != ADI_INT_SUCCESS)
         return;

   /* Call the RTL-level dispatcher register function to install the custom handler
   ** to replace the SSL-level dispatched handler.
   */

    Rtlcheck = adi_rtl_register_dispatched_handler (ADI_RTL_SEC_IID(INTR_UART0_STAT), UartR_ISR, (adi_dispatched_callback_t) NULL);
   if (Rtlcheck == 1)
        return;

   /* Enable uart0 rx status */
   Rtlcheck = *pREG_UART0_RBR;
   Rtlcheck = *pREG_UART0_STAT;
   //*pREG_UART1_STAT = Rtlcheck;
   //Do not set line status interrupt mask
   //*pREG_UART0_IMSK_SET = BITM_UART_IMSK_SET_ERXS|BITM_UART_IMSK_SET_ERBFI|BITM_UART_IMSK_SET_ELSI;
   *pREG_UART0_IMSK_SET = BITM_UART_IMSK_SET_ERXS|BITM_UART_IMSK_SET_ERBFI;

   //*pILAT |= 0x0800; // clear pending SEC interrupts
   adi_sec_ClearPendStatus(INTR_UART0_STAT);
   Intcheck = adi_int_EnableInt(INTR_UART0_STAT, true);
   if (Intcheck != ADI_INT_SUCCESS)
        return;

}


void Init_UART()
{
    Init_Uart0();
}

void Handle_UART()                           
{
    if (sw_uart_mode) {
        sw_uart_mode = 0;

        if (gbl_uart0_mode == MODE_CMD_LINE) {
            gbl_uart0_mode = MODE_DANTE_CONSOLE;
            //Set FPGA to drive uart pin
            write_fpga(ADDR_CONNECT_UARTA, 1);
        }
        else {      //revert back to command line mode
            gbl_uart0_mode = MODE_CMD_LINE;
            //Set FPGA to tristate uart pin
            write_fpga(ADDR_CONNECT_UARTA, 0);
        }
    }

    //if (gbl_uart0_mode == MODE_DANTE_CONSOLE) {
    //    //See if data needs to be written to UART FIFO
    //    while (uarta_tx_head != uarta_tx_tail) {
    //        write_fpga(FPGA_UART_DATA, serial_load_buf[uarta_tx_tail++]);
    //    }
    //}
    

}

void SendString (char *pString)
{
    while (*pString != '\0') {
        SendChar(*pString);
        //adi_uart_Write(ghUART, pString, 1);
        //adi_uart_Write(ghUART1, pString, 1);
        pString += 1;
    }
}

int GetChar(uint8_t *cVal)
{
    int status;

    if (*pREG_UART0_STAT & BITM_UART_STAT_DR) {
        *cVal = *pREG_UART0_RBR;
        status = 1;
    }
    else {
        status = 0;
    }

    return status;
}

void SendChar(uint8_t cVal)
{
    //wait until UART is ready to send
    while ((*pREG_UART0_STAT & BITM_UART_STAT_THRE)==ENUM_UART_STAT_THR_NOT_EMPTY);
    *pREG_UART0_THR = cVal;
}


/********************************************************************************
* Text commands *
********************************************************************************/
int txt_BAUD(char *pCommandLine)
{
    char string[40];
    char *rem;
    u32 baud;

    if (*pCommandLine != NULL) {
        //chan = (u8) strtol(pCommandLine, &rem, 10);
        //pCommandLine = rem;
    }
    else {
        //If here give explanation for BAUD command
        SendString("BAUD <baud>\r\n");
        SendString("baud:  57600, 115200, 230400, 460800, 921600\r\n");
        return CMD_DONE;
    }

    strcpy(string, pCommandLine);

    //get baud rate
    if (*pCommandLine != NULL) {
        if (!strcmp(string, "921600")) {
            baud = BAUD_RATE_921600;
            baud_setting = BAUD_921600;
        }
        else if (!strcmp(string, "460800")) {
            baud = BAUD_RATE_460800;
            baud_setting = BAUD_460800;
        }
        else if (!strcmp(string, "230400")) {
            baud = BAUD_RATE_230400;
            baud_setting = BAUD_230400;
        }
        else if (!strcmp(string, "115200")) {
            baud = BAUD_RATE_115200;
            baud_setting = BAUD_115200;
        }
        else if (!strcmp(string, "57600")) {
            baud = BAUD_RATE_57600;
            baud_setting = BAUD_57600;
        }
        else {
            SendString("Choose 57600, 115200, 230400, 460800, 921600\r\n");
            return CMD_DONE;
        }
    }
    else {
        return CMD_ERROR;
    }

    *pREG_UART0_CLK = BITM_UART_CLK_EDBO | (BITM_UART_CLK_DIV & baud);

    return CMD_DONE;
}

int txt_SAVE(char *pCommandLine)
//save the baud rate to flash
{
    char string[40];
             
    EraseFlash(BAUD_FLASH_ADDR, 1);
    flash_program(flash_info, BAUD_FLASH_ADDR, &baud_setting, 1);

    return CMD_DONE;
}



