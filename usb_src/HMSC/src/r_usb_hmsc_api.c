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
* File Name    : r_usb_hmsc_api.c
* Version      : 1.00
* Device(s)    : Renesas
* Tool-Chain   : Renesas
* OS           : Common to None and uITRON
* H/W Platform : Independent
* Description  : USB Peripheral Sample Code
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 17.03.2010 0.80    First Release
*         : 30.07.2010 0.90    Updated comments
*         : 02.08.2010 0.91    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/

/* $Id$ */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_cTypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */
#include "r_usb_catapi_define.h"        /* Peripheral ATAPI Device extern */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
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
extern uint8_t     *usb_ghmsc_DeviceTable;
extern uint8_t     *usb_ghmsc_ConfigTable;
extern uint16_t    usb_ghmsc_Devaddr;
extern uint16_t    usb_shmsc_InitSeq;

extern uint16_t get_max_lun_table[5];
extern uint16_t mass_storage_reset_table[5];

extern usb_er_t    usb_hmsc_SubmitutrReq(uint16_t type, usb_utr_t *utr_table);
extern void     usb_cstd_dummy_function(uint16_t data1, uint16_t data2);

/******************************************************************************
Renesas Host MSC Sample Code functions
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hmsc_driver_start
Description     : USB Host Initialize process
Argument        : none
Return          : none
******************************************************************************/
void R_usb_hmsc_driver_start()
{
}   /* eof R_usb_hmsc_driver_start() */

/******************************************************************************
Function Name   : R_usb_hmsc_ClearStall
Description     : Clear Stall
Argument        : uint16_t type             : Data transmit/Data Receive
                : uint16_t msgnum           : Message Number
                : usb_cb_t complete         : Callback Function
Return value    : none
******************************************************************************/
void R_usb_hmsc_ClearStall( uint16_t type, uint16_t msgnum, usb_cb_t complete)
{
    uint16_t        pipeno;

    switch( type ) 
    {
    case USB_HMSC_TYPE_TRANSMIT:                         /* Data transmit */
        pipeno = R_usb_hmsc_Information(usb_ghmsc_OutPipe[msgnum][0]);
        usb_ghmsc_OutPipe[msgnum][1] = 0;
        usb_hmsc_ClearStall( pipeno, complete);
        break;
    case USB_HMSC_TYPE_RECEIVE:                           /* Data recieve */
        pipeno = R_usb_hmsc_Information(usb_ghmsc_InPipe[msgnum][0]);
        usb_ghmsc_InPipe[msgnum][1] = 0;
        usb_hmsc_ClearStall( pipeno, complete);
        break;
    default:
        break;
    }
}   /* eof R_usb_hmsc_ClearStall() */

/******************************************************************************
Function Name   : R_usb_hmsc_TaskOpen
Description     : Open Mass storage class driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_TaskOpen(uint16_t data1, uint16_t data2)
{
}   /* eof R_usb_hmsc_TaskOpen() */

/******************************************************************************
Function Name   : R_usb_hmsc_TaskClose
Description     : Close Mass storage class driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_TaskClose()
{
}   /* eof R_usb_hmsc_TaskClose() */

/******************************************************************************
Function Name   : R_usb_hmsc_Initialized
Description     : initialized USB_HMSC_TSK
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_Initialized()
{
    uint16_t    i;

    /* 0x00 */
    usb_ghmsc_AttSts = USB_HMSC_DEV_DET;

    for( i = 0; i < USB_MAXSTRAGE; i++ )
    {
        usb_ghmsc_CbwTagNo[i] = (uint16_t)1;

        usb_ghmsc_Cbw[i].dCBWSignature = USB_MSC_CBW_SIGNATURE;
        usb_ghmsc_Cbw[i].dCBWTag = usb_ghmsc_CbwTagNo[i];
        usb_ghmsc_Cbw[i].dCBWDTL_Lo = 0;
        usb_ghmsc_Cbw[i].dCBWDTL_ML = 0;
        usb_ghmsc_Cbw[i].dCBWDTL_MH = 0;
        usb_ghmsc_Cbw[i].dCBWDTL_Hi = 0;
        usb_ghmsc_Cbw[i].bmCBWFlags.CBWdir = 0;
        usb_ghmsc_Cbw[i].bmCBWFlags.reserved7 = 0;
        usb_ghmsc_Cbw[i].bCBWLUN.bCBWLUN = 0;
        usb_ghmsc_Cbw[i].bCBWLUN.reserved4 = 0;
        usb_ghmsc_Cbw[i].bCBWCBLength.bCBWCBLength = 0;
        usb_ghmsc_Cbw[i].bCBWCBLength.reserved3 = 0;

        usb_ghmsc_Cbw[i].CBWCB[0]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[1]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[2]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[3]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[4]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[5]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[6]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[7]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[8]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[9]  = 0;
        usb_ghmsc_Cbw[i].CBWCB[10] = 0;
        usb_ghmsc_Cbw[i].CBWCB[11] = 0;
        usb_ghmsc_Cbw[i].CBWCB[12] = 0;
        usb_ghmsc_Cbw[i].CBWCB[13] = 0;
        usb_ghmsc_Cbw[i].CBWCB[14] = 0;
        usb_ghmsc_Cbw[i].CBWCB[15] = 0;
    }
}   /* eof R_usb_hmsc_Initialized() */

/******************************************************************************
Function Name   : R_usb_hmsc_ClassCheck
Description     : check connected device
Arguments       : uint16_t **table : 
Return value    : none
******************************************************************************/
void R_usb_hmsc_ClassCheck(uint8_t **table)
{
    usb_ghmsc_DeviceTable       = (uint8_t*)(table[0]);
    usb_ghmsc_ConfigTable       = (uint8_t*)(table[1]);
    usb_ghmsc_Devaddr           = *table[2];

    usb_shmsc_InitSeq = USB_SEQ_0;

    R_USB_GET_SND(USB_HMSC_MBX, USB_MSG_HMSC_CLS_INIT, &usb_cstd_dummy_function, (usb_strct_t)0);
}   /* eof R_usb_hmsc_ClassCheck() */

/******************************************************************************
Function Name   : R_usb_hmsc_Read10
Description     : Read10
Arguments       : uint16_t side : 
                : uint8_t *buff : 
                : uint32_t secno : 
                : uint16_t seccnt : 
                : uint32_t trans_byte : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Read10(uint16_t side, uint8_t *buff, uint32_t secno,
     uint16_t seccnt, uint32_t trans_byte)
{
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        return (USB_ERROR);
    }

    /* set CBW parameter */
    usb_hmsc_SetRwCbw((uint16_t)USB_ATAPI_READ10, secno, seccnt, trans_byte, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(side, buff, trans_byte);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_Read10() */

/******************************************************************************
Function Name   : R_usb_hmsc_Write10
Description     : Write10
Arguments       : uint16_t side : 
                : uint8_t *buff : 
                : uint32_t secno : 
                : uint16_t seccnt : 
                : uint32_t trans_byte : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Write10(uint16_t side, uint8_t *buff, uint32_t secno,
     uint16_t seccnt, uint32_t trans_byte)
{
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        return (USB_ERROR);
    }

    /* set CBW parameter */
    usb_hmsc_SetRwCbw((uint16_t)USB_ATAPI_WRITE10, secno, seccnt, trans_byte, side);

    /* Data OUT */
    hmsc_retval = usb_hmsc_DataOut(side, buff, trans_byte);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_Write10() */

/******************************************************************************
Function Name   : R_usb_hmsc_PreventAllow
Description     : PreventAllow
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_PreventAllow(uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_PREVENT_ALLOW;
    /* Reserved */
    data[4] = buff[0];

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)0, side);

    /* No Data */
    hmsc_retval = usb_hmsc_NoData(side);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_PreventAllow() */

/******************************************************************************
Function Name   : R_usb_hmsc_TestUnit
Description     : TestUnit
Arguments       : uint16_t side : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_TestUnit(uint16_t side)
{
    uint8_t     data[12];
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_TEST_UNIT_READY;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)0, side);

    /* No Data */
    hmsc_retval = usb_hmsc_NoData(side);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_TestUnit() */

/******************************************************************************
Function Name   : R_usb_hmsc_RequestSense
Description     : RequestSense
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_RequestSense(uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 18;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_REQUEST_SENSE;
    /* Allocation length */
    data[4] = length;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_RequestSense() */

/******************************************************************************
Function Name   : R_usb_hmsc_Inquiry
Description     : Inquiry
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Inquiry(uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 36;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_INQUIRY;
    /* Allocation length */
    data[4] = length;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_Inquiry() */

/******************************************************************************
Function Name   : R_usb_hmsc_ReadCapacity
Description     : ReadCapacity
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ReadCapacity(uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 8;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_READ_CAPACITY;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_ReadCapacity() */

/******************************************************************************
Function Name   : R_usb_hmsc_ReadFormatCapacity
Description     : ReadFormatCapacity
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ReadFormatCapacity(uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 0x20;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_READ_FORMAT_CAPACITY;
    /* Allocation length */
    data[8] = length;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_ReadFormatCapacity() */

/******************************************************************************
Function Name   : R_usb_hmsc_ModeSense10
Description     : ModeSense10
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ModeSense10(uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 26;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_MODE_SENSE10;
    /* Set LUN / DBD=1 */
    data[1] = 0x08;
    /* Allocation length */
    data[7] = 0x00;
    /* Allocation length */
    data[8] = 0x02;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_ModeSense10() */

/******************************************************************************
Function Name   : R_usb_hmsc_ModeSelect6
Description     : ModeSelect6
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ModeSelect6(uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 18;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(side);
    if( unit == USB_ERROR )
    {
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_MODE_SELECT6;
    /* SP=0 */
    data[1] = 0x10;
    /* Parameter list length ??? */
    data[4] = 0x18;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw((uint8_t*)&data, (uint32_t)length, side);

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)24, buff);

    /* Data set */
    buff[3] = 0x08;
    buff[10] = 0x02;
    buff[12] = 0x08;
    buff[13] = 0x0A;

    /* Data OUT */
    hmsc_retval = usb_hmsc_DataOut(side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_ModeSelect6() */

/******************************************************************************
Function Name   : R_usb_hmsc_Release
Description     : Release Mass Strage Class driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_Release()
{
    R_usb_hstd_DriverRelease((uint8_t)USB_IFCLS_MAS);
}   /* eof R_usb_hmsc_Release() */

/******************************************************************************
Function Name   : R_usb_hmsc_SetDevSts
Description     : Sets HMSCD operation state
Arguments       : uint16_t data : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_SetDevSts(uint16_t data)
{
    usb_ghmsc_AttSts = data;
    return USB_E_OK;
}   /* eof R_usb_hmsc_SetDevSts() */

/******************************************************************************
Function Name   : R_usb_hmsc_GetDevSts
Description     : Responds to HMSCD operation state
Arguments       : none
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_GetDevSts(void)
{
    return(usb_ghmsc_AttSts);
}   /* eof R_usb_hmsc_GetDevSts() */

/******************************************************************************
Function Name   : R_usb_hmsc_Information
Description     : EP Table Information
Arguments       : uint16_t pipe_offset : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Information(uint16_t pipe_offset)
{
    return(usb_ghmsc_TmpEpTbl[pipe_offset]);
}   /* eof R_usb_hmsc_Information() */

/******************************************************************************
Function Name   : R_usb_hmsc_GetMaxUnit
Description     : Get Max LUN request
Argument        : uint16_t addr             : Device Address
                : usb_cb_t complete         : CallBack Function
Return value    : usb_er_t                  : Error Code
******************************************************************************/
usb_er_t R_usb_hmsc_GetMaxUnit( uint16_t addr, usb_cb_t complete)
{
    usb_er_t err;

    /* Device address set */
    get_max_lun_table[4] = addr;

    /* Recieve MaxLUN */
    usb_ghmsc_ControlData.pipenum   = USB_PIPE0;
    usb_ghmsc_ControlData.tranadr   = (void*)usb_ghmsc_Data;
    usb_ghmsc_ControlData.tranlen   = (uint32_t)1;
    usb_ghmsc_ControlData.setup     = get_max_lun_table;
    usb_ghmsc_ControlData.complete  = complete;

    err = usb_hmsc_SubmitutrReq( (uint16_t)USB_HMSC_TYPE_CTLR, &usb_ghmsc_ControlData);
    return err;
}   /* eof R_usb_hmsc_GetMaxUnit() */

/******************************************************************************
Function Name   : R_usb_hmsc_MassStorageReset
Description     : Mass Strage Reset request
Argument        : uint16_t drvnum           : Drive Number
                : usb_cb_t complete        : Callback Funtion
Return value    : usb_er_t                 : Error Code
******************************************************************************/
usb_er_t R_usb_hmsc_MassStorageReset( uint16_t drvnum, usb_cb_t complete)
{
    usb_er_t err;

    /* Device address set */
    mass_storage_reset_table[4] = usb_hmsc_SmpDrive2Addr(drvnum);

    /* Set MassStorageReset */
    usb_ghmsc_ControlData.pipenum   = USB_PIPE0;
    usb_ghmsc_ControlData.tranadr   = (void*)usb_ghmsc_Data;
    usb_ghmsc_ControlData.tranlen   = (uint32_t)0;
    usb_ghmsc_ControlData.setup     = mass_storage_reset_table;
    usb_ghmsc_ControlData.complete  = complete;

    err = usb_hmsc_SubmitutrReq( (uint16_t)USB_HMSC_TYPE_CTLR, &usb_ghmsc_ControlData);
    return err;
}   /* eof R_usb_hmsc_MassStorageReset() */

