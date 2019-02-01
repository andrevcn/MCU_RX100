/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
* This software is owned by Renesas Electronics Corporation and is  protected
* under all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR  A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE  EXPRESSLY
* DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************
* File Name    : r_usb_hmsc_ddi.c
* Version      : 1.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host MSC BOT ddi
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.08.2010 0.91    First Release
*         : 06.08.2010 0.92    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver define */
#include "r_usb_catapi_define.h"    /* Peripheral ATAPI Device extern define */
#include "r_usb_usrconfig.h"        /* System definition */
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_hmsc_extern.h"      /* Host MSC grobal define */

#include "r_usb_api.h"
#include "r_usb_hmsc_api.h"

/******************************************************************************
Section    <Section Difinition> , "Project Sections"
******************************************************************************/
#ifdef  R8CUSB
#pragma section program P_hmsc
#pragma section bss B_hmsc
#endif
#ifdef  RL78USB
#endif
#ifdef  RL78USB_SECTION
#pragma section @@CODE   P_HMSC
#pragma section @@BASE   T_HMSC
#pragma section @@DATA   B_HMSC
#endif

/******************************************************************************
External variables and functions
******************************************************************************/
extern uint16_t         usb_shmsc_Process;
extern const uint16_t   usb_gapl_devicetpl[];

extern uint16_t usb_hmsc_GetStringDescriptor1(uint16_t devaddr, uint16_t index, usb_cb_t complete);
extern uint16_t usb_hmsc_GetStringDescriptor2(uint16_t devaddr, uint16_t index, usb_cb_t complete);
extern uint16_t usb_hmsc_GetStringDescriptor1Check(uint16_t errcheck);
extern uint16_t usb_hmsc_GetStringDescriptor2Check(uint16_t errcheck);

/******************************************************************************
Private global variables and functions
******************************************************************************/
uint8_t     *usb_ghmsc_DeviceTable;
uint8_t     *usb_ghmsc_ConfigTable;
uint8_t     *usb_ghmsc_InterfaceTable;
uint16_t    usb_ghmsc_Devaddr;
uint16_t    usb_shmsc_InitSeq   =   USB_SEQ_0;
uint16_t    usb_ghmsc_in_pipe_ctr;
uint16_t    usb_ghmsc_out_pipe_ctr;

/******************************************************************************
Private global variables and functions
******************************************************************************/
/******************************************************************************
* Endpoint Configuration Data Format
*******************************************************************************
* LINE1: Pipe Window Select Register (0x64)
*   CPU Access PIPE             : PIPE1 to MAX_PIPE_NO  : Class Driver
*   LINE2: Pipe Configuration Register (0x66)
*       Transfer Type           : USB_NULL              : Endpoint Descriptor
*       Buffer Ready interrupt  : USB_BFREOFF           : Class Driver
*       Double Buffer Mode      : USB_DBLBON            : Class Driver
*       Continuous Transmit:    : USB_CNTMDON           : Class Driver
*       Short NAK               : USB_NULL              : Endpoint Descriptor
*       Transfer Direction      : USB_NULL              : Endpoint Descriptor
*       Endpoint Number         : USB_NULL              : Endpoint Descriptor
*   LINE3: Pipe Buffer Configuration Register (0x68)
*       Buffer Size             : USB_BUF_SIZE(x)       : Class Driver
*       Buffer Top Number       : USB_BUF_NUMB(x)       : Class Driver
*   LINE4: Pipe Maxpacket Size Register (0x6A)
*       Max Packet Size         : USB_NULL              : Endpoint Descriptor
*   LINE5: Pipe Cycle Configuration Register (0x6C)
*       ISO Buffer Flush Mode   : USB_NULL              : Class Driver
*       ISO Interval Value      : USB_NULL              : Class Driver
*   LINE6: use FIFO port
*       CUSE/D1USE/D0DMA        : USB_CUSE              : Class Driver
******************************************************************************/

uint16_t usb_ghmsc_DefEpTbl[] =
{
    USB_PIPE1,
    USB_NULL|USB_BFREOFF|USB_DBLBOFF|USB_SHTNAKOFF,
    USB_NULL,
    USB_NULL,
/* Pipe end */
    USB_PDTBLEND,
};

uint16_t usb_ghmsc_TmpEpTbl[] =
{
    USB_PIPE1,
    /* TYPE/BFRE/DBLB/CNTMD/SHTNAK/DIR/EPNUM */
    USB_NULL|USB_BFREOFF|USB_DBLBOFF|USB_SHTNAKOFF,
    USB_NULL,
    USB_NULL,

    USB_PIPE1,
    /* TYPE/BFRE/DBLB/CNTMD/SHTNAK/DIR/EPNUM */
    USB_NULL|USB_BFREOFF|USB_DBLBOFF|USB_SHTNAKOFF,
    USB_NULL,
    USB_NULL,

    USB_PIPE1,
    /* TYPE/BFRE/DBLB/CNTMD/SHTNAK/DIR/EPNUM */
    USB_NULL|USB_BFREOFF|USB_DBLBOFF|USB_SHTNAKOFF,
    USB_NULL,
    USB_NULL,

    USB_PIPE1,
    /* TYPE/BFRE/DBLB/CNTMD/SHTNAK/DIR/EPNUM */
    USB_NULL|USB_BFREOFF|USB_DBLBOFF|USB_SHTNAKOFF,
    USB_NULL,
    USB_NULL,

    /* Pipe end */
    USB_PDTBLEND,
};

/******************************************************************************
Renesas Abstracted HMSC Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_ClassCheck
Description     : check class
Arguments       : usb_utr_t *mess : message
Return value    : none
******************************************************************************/
void usb_hmsc_ClassCheck(usb_tskinfo_t *mess)
{
    uint16_t    result;
    totallen_t  device_id, length2;
    uint16_t    length1;
    uint8_t     i;

    result = USB_E_OK;
    switch( usb_shmsc_InitSeq )
    {
    case USB_SEQ_0:
        device_id.WORD.BYTE.up      =  usb_ghmsc_DeviceTable[USB_DEV_VENDOR_HI];
        device_id.WORD.BYTE.dn      =  usb_ghmsc_DeviceTable[USB_DEV_VENDOR_LO];

        /* Check TPL */
        for( i = 0, result = USB_ERROR; i < usb_gapl_devicetpl[0]; i++ )
        {
            if ((usb_gapl_devicetpl[(i * 2) + 2] == USB_NOVENDOR) ||
                (usb_gapl_devicetpl[(i * 2) + 2] == device_id.WORD.w))
            {
                device_id.WORD.BYTE.up      =  usb_ghmsc_DeviceTable[USB_DEV_PRODUCT_HI];
                device_id.WORD.BYTE.dn      =  usb_ghmsc_DeviceTable[USB_DEV_PRODUCT_LO];
                if ((usb_gapl_devicetpl[(i * 2) + 3] == USB_NOPRODUCT) ||
                    (usb_gapl_devicetpl[(i * 2) + 3] == device_id.WORD.w))
                {
                    result = USB_E_OK;
                }
            }
        }
        if( result == USB_ERROR )
        {
            break;
        }

        /* Check interface class */
        length1 = 0;
        length2.WORD.BYTE.up        =  usb_ghmsc_ConfigTable[USB_CON_TOTALLENG_HI];
        length2.WORD.BYTE.dn        =  usb_ghmsc_ConfigTable[USB_CON_TOTALLENG_LO];
        if( length2.WORD.w > USB_CONFIGSIZE )
        {
            length2.WORD.w = USB_CONFIGSIZE;
        }

        /* Search within configuration total-length */
        result = USB_ERROR;
        while( length1 < length2.WORD.w )
        {               
            if( usb_ghmsc_ConfigTable[length1+1] == USB_DT_INTERFACE )
            {
                usb_ghmsc_InterfaceTable  = (uint8_t*)&usb_ghmsc_ConfigTable[length1];
                if( (uint16_t)usb_ghmsc_ConfigTable[length1+USB_INTF_CLASS] == USB_IFCLS_MAS )
                {
                    result = USB_E_OK;
                    break;
                }
            }
            length1 += usb_ghmsc_ConfigTable[length1];
        }

        /* Check device count */
        if( usb_ghmsc_StrgCount == USB_MAXSTRAGE )
        {
            result = USB_ERROR;
        }

        /* Check drive count */
        if( usb_ghmsc_MaxDrive >= USB_MAXDRIVE )
        {
            result = USB_ERROR;
        }

        /* Descriptor check */
        result = usb_hmsc_SmpBotDescriptor(usb_ghmsc_InterfaceTable, usb_ghmsc_StrgCount);

        /* Serial number check */
        if( result != USB_ERROR )
        {
            /* no string device (STALL) */
            if( usb_ghmsc_DeviceTable[14] == 0
                && usb_ghmsc_DeviceTable[15] == 0
                && usb_ghmsc_DeviceTable[16] == 0 ) {

                result = usb_hmsc_SmpPipeInfo(usb_ghmsc_InterfaceTable
                            , usb_ghmsc_StrgCount, (uint16_t)usb_ghmsc_ConfigTable[2]);
                if( result != USB_ERROR )
                {
                    result = USB_YES;
                    usb_ghmsc_in_pipe_ctr = 0;
                    usb_ghmsc_out_pipe_ctr = 0;
                }
                R_usb_hstd_ReturnEnuMGR(result);    /* return to MGR */
                usb_shmsc_InitSeq = USB_SEQ_0;
                return;
            }

            usb_shmsc_Process = USB_MSG_HMSC_CLS_INIT;
            result = usb_hmsc_GetStringDescriptor1(usb_ghmsc_Devaddr,
                     (uint16_t)usb_ghmsc_DeviceTable[16],
                     (usb_cb_t)usb_hmsc_CheckResult );
            usb_shmsc_InitSeq++;
        }
        break;
    case USB_SEQ_1:
        result = usb_hmsc_GetStringDescriptor1Check(mess->keyword);
        if( result != USB_ERROR )
        {
            result = usb_hmsc_GetStringDescriptor2(usb_ghmsc_Devaddr,
                     (uint16_t)usb_ghmsc_DeviceTable[15],
                     (usb_cb_t)usb_hmsc_CheckResult );
            usb_shmsc_InitSeq++;
        }
        break;
    case USB_SEQ_2:
        /* Serial number check */
        result = usb_hmsc_GetStringDescriptor2Check(mess->keyword);
        result = usb_hmsc_GetStringInfoCheck(usb_ghmsc_Devaddr);
        
        if( result != USB_ERROR )
        {
            /* Pipe Information table set */
            result = usb_hmsc_SmpPipeInfo(usb_ghmsc_InterfaceTable,
                     usb_ghmsc_StrgCount, (uint16_t)usb_ghmsc_ConfigTable[2]);
            if( result != USB_ERROR )
            {
                result = USB_YES;
                usb_ghmsc_in_pipe_ctr = 0;
                usb_ghmsc_out_pipe_ctr = 0;
            }
            /* Return to MGR */
            R_usb_hstd_ReturnEnuMGR(result);
            usb_shmsc_InitSeq = USB_SEQ_0;
        }
        break;
    default:
        result = USB_ERROR;
        break;
    }
    
    if( result == USB_ERROR )
    {
        usb_shmsc_InitSeq = USB_SEQ_0;
        /* Return to MGR */
        R_usb_hstd_ReturnEnuMGR(USB_ERROR);
    }
}   /* eof usb_hmsc_ClassCheck() */

/******************************************************************************
Renesas Abstracted Mass Storage Device Class command
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_ClrData
Description     : data clear
Arguments       : uint16_t len : 
                : uint8_t *buf : 
Return value    : none
******************************************************************************/
void usb_hmsc_ClrData(uint16_t len, uint8_t *buf)
{
    uint16_t    i;

    for( i = 0; i < len; ++i )
    {
        *buf++ = 0x00;
    }
}   /* eof usb_hmsc_ClrData() */

/******************************************************************************
End  Of File
******************************************************************************/
