/************************************************************************
*
* Device     : RX/RX100/RX111
*
* File Name  : vecttbl.c
*
* Abstract   : Initialize of Vector Table and MDE register.
*
* History    : 1.00 (2012-09-03) [Hardware Manual Revision : 0.40]
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2012 Renesas Electronics Corporation.
* Renesas Solutions Corp. All rights reserved.
*
************************************************************************/

#include "vect.h"

#pragma section C FIXEDVECT

void (*const Fixed_Vectors[])(void) = {
//;0xffffffd0  Exception(Supervisor Instruction)
    Excep_SuperVisorInst,
//;0xffffffd4  Reserved
    Dummy,
//;0xffffffd8  Reserved
    Dummy,
//;0xffffffdc  Exception(Undefined Instruction)
    Excep_UndefinedInst,
//;0xffffffe0  Reserved
    Dummy,
//;0xffffffe4  Reserved
    Dummy,
//;0xffffffe8  Reserved
    Dummy,
//;0xffffffec  Reserved
    Dummy,
//;0xfffffff0  Reserved
    Dummy,
//;0xfffffff4  Reserved
    Dummy,
//;0xfffffff8  NMI
    NonMaskableInterrupt,
//;0xfffffffc  RESET
//;<<VECTOR DATA START (POWER ON RESET)>>
//;Power On Reset PC
    /*(void*)*/ PowerON_Reset_PC                                                                                                                 
//;<<VECTOR DATA END (POWER ON RESET)>>
};

#pragma address _MDEreg=0xffffff80 // MDE register (Single Chip Mode)
#ifdef __BIG
    const unsigned long _MDEreg = 0xfffffff8; // big
#else
    const unsigned long _MDEreg = 0xffffffff; // little
#endif
