/*
 * (C) Copyright 2014 - Analog Devices, Inc.  All rights reserved.
 *
 *    FILE:     pbled_test.h ( )
 *
 *    CHANGES:  1.00.0  - initial release
 */
#ifndef _FPGA_H_
#define _FPGA_H_

//#include <sys/exception.h>
#include "bf700.h"
#include "plink.h"

//FPGA addresses
#define REMOTE_DATA_IN_HI       0x24
#define REMOTE_DATA_IN_LO       0x25
#define REMOTE_CLK_CTRL         0x26
#define REMOTE_ADDR             0x27
//#define REMOTE_READ_SOURCE      0x28
//#define REMOTE_DATA_OUT_HI      0x29
//#define REMOTE_DATA_OUT_LO      0x2a
//#define REMOTE_BUSY             0x2b

//Remote update bit defines
#define REMOTE_UPDATE_RD        0x2
#define REMOTE_UPDATE_WR        0x4
#define REMOTE_RECONFIG         0x8
#define REMOTE_RESET            0x20

#define FPGA_VERSION_MONTH_DAY  0x08
#define FPGA_VERSION_YEAR_BUILD 0x09

#define FPGA_LED_REG    0x01
//#define FPGA_SAFETYMUTE_REG    0x0A
//fpga bit masks
#define FPGA_LED_BLUE   0x01
#define FPGA_LED_RED    0x02

extern void Init_FPGA(void);

extern void write_fpga(int, int);
extern int read_fpga(int);

/********************************************************************************
* Text commands *
********************************************************************************/
extern int txt_FPGAW(char *);
extern int txt_FPGAR(char *);
extern int txt_FPGARESET(char *);

/********************************************************************************
* Plink commands *
********************************************************************************/
extern u8 cmd_FPGATST(CtrlPacket_ *);
extern u8 cmd_FPGAVER(CtrlPacket_ *);


#endif //_FPGA_
