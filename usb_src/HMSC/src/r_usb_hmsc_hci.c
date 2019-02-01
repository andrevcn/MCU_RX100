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
* File Name    : r_usb_hmsc_hci.c
* Version      : 1.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host MSC BOT driver
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.08.2010 0.91    First Release
*         : 06.08.2010 0.92    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_usb_cmsc_define.h"   /* USB  Mass Storage Class Header */
#include    "r_usb_hmsc_define.h"   /* Host Mass Storage Class Driver define */
#include    "r_usb_usrconfig.h"     /* System definition */
#include    "r_usb_cDefUSBIP.h"     /* USB-FW Library Header */
#include    "r_usb_cKernelId.h"     /* Kernel ID definition */
#include    "r_usb_hmsc_extern.h"   /* Host MSC grobal define */

#include "r_usb_api.h"
#include "r_usb_hmsc_api.h"
#include "r_usb_usbip.h"                /* Definition of the USB register & bit define */

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
Private global variables and functions
******************************************************************************/
//uint8_t                 usb_ghmsc_Data[2048];
uint8_t                 usb_ghmsc_Data[512]; /* TEMP - ALISON */

uint16_t get_max_lun_table[5] = {0xFEA1, 0x0000, 0x0000, 0x0001, 0x0000};
uint16_t mass_storage_reset_table[5] = {0xFF21, 0x0000, 0x0000, 0x0000, 0x0000};

/* Send data transfer message */
usb_utr_t               usb_ghmsc_TransData[USB_MAXSTRAGE];
/* Receive data transfer message */
usb_utr_t               usb_ghmsc_ReceiveData[USB_MAXSTRAGE];
/* Control data transfer message */
usb_utr_t               usb_ghmsc_ControlData;
/* Pipenumber / Pipectr(SQTGL) */
uint16_t                usb_ghmsc_OutPipe[USB_MAXSTRAGE][2];
/* Pipenumber / Pipectr(SQTGL) */
uint16_t                usb_ghmsc_InPipe[USB_MAXSTRAGE][2];
uint16_t                usb_ghmsc_strage_num;
uint16_t                usb_shmsc_ClearStallPipe;
usb_er_t                usb_hmsc_ClearStall( uint16_t Pipe, usb_cb_t complete);

void                    usb_hmsc_ControlEnd( uint16_t sts);

uint16_t    usb_hmsc_SendCbwReq( uint16_t drvnum);
uint16_t    usb_hmsc_GetDataReq( uint16_t drvnum);
uint16_t    usb_hmsc_SendDataReq( uint16_t drvnum);
uint16_t    usb_hmsc_GetCswReq( uint16_t drvnum);
void        usb_hmsc_DummyFunction( uint16_t data1, uint16_t data2);
usb_er_t    usb_hmsc_SubmitutrReq(uint16_t type, usb_utr_t *utr_table);

/******************************************************************************
External variables and functions
******************************************************************************/
extern uint16_t     usb_shmsc_Process;
extern uint8_t      usb_ghmsc_ClassData[];
extern usb_pipe_t   g_usb_CurrentPipe;    /* Pipe number */
extern uint16_t     usb_ghmsc_in_pipe_ctr;
extern uint16_t     usb_ghmsc_out_pipe_ctr;

extern uint16_t usb_hmsc_StdReqCheck(uint16_t errcheck);
extern void     usb_cstd_clr_stall(void);
extern void     usb_cstd_reg_set_pipexctr(uint16_t field);
extern void     usb_hstd_control_end(usb_strct_t Status);

/******************************************************************************
Renesas Abstracted HMSC Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SendCbw
Description     : Send CBW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendCbw( uint16_t drvnum)
{
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* Set CBW TAG */
    usb_hmsc_CbwTagCount(msgnum);

    /* Request CBW */
    /* Device number */
    usb_ghmsc_strage_num = msgnum;
    /* Transfer data address */
    usb_ghmsc_TransData[msgnum].tranadr     = (void*)&usb_ghmsc_Cbw[msgnum];
    /* Transfer data length */
    usb_ghmsc_TransData[msgnum].tranlen     = (uint32_t)USB_MSC_CBWLENGTH;
    /* Not control transfer */
    usb_ghmsc_TransData[msgnum].setup           = 0;
    /* Call Back Function Info */
    usb_ghmsc_TransData[msgnum].complete    = (usb_cb_t)&usb_hmsc_DummyFunction;

    return usb_hmsc_Submitutr( (uint16_t)USB_HMSC_TYPE_TRANSMIT, &usb_ghmsc_TransData[msgnum]);
    
}   /* eof usb_hmsc_SendCbw() */

/******************************************************************************
Function Name   : usb_hmsc_SendCbwReq
Description     : Send CBW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendCbwReq( uint16_t drvnum)
{
    usb_er_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);
    /* Call Back Function Info */
    usb_ghmsc_TransData[msgnum].complete    = (usb_cb_t)&usb_hmsc_CheckResultOutPipe;

    err = usb_hmsc_SubmitutrReq( (uint16_t)USB_HMSC_TYPE_TRANSMIT, &usb_ghmsc_TransData[msgnum]);
    if( err != USB_E_OK ) 
    {
        return USB_HMSC_SUBMIT_ERR;
    }
    return err;
}   /* eof usb_hmsc_SendCbwReq() */

/******************************************************************************
Function Name   : usb_hmsc_SendCbwCheck
Description     : Check send CBW 
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendCbwCheck( uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t    pipeno, msgnum;
    usb_tskinfo_t mess;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);
    switch( hmsc_retval ) 
    {
    case USB_DATA_OK:       /* Send CBW */
        pipeno = R_usb_hmsc_Information(usb_ghmsc_OutPipe[msgnum][0]);
        usb_ghmsc_OutPipe[msgnum][1] = usb_ghmsc_TransData[msgnum].pipectr;
        return USB_HMSC_OK;
        break;
    case USB_DATA_STALL:    /* Stall */
        usb_shmsc_Process = USB_MSG_HMSC_CBW_ERR;
        usb_shmsc_StallErrSeq = USB_SEQ_0;
        mess.keyword = drvnum;

        usb_hmsc_SpecifiedPath(&mess);
        return USB_DATA_STALL;
        break;
    case USB_DATA_TMO:      /* Timeout */
        pipeno  = R_usb_hmsc_Information(usb_ghmsc_OutPipe[msgnum][0]);
        R_usb_hstd_TransferEnd( pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        break;
    default:
        break;
    }
    return USB_HMSC_CBW_ERR;
}   /* eof usb_hmsc_SendCbwCheck() */

/******************************************************************************
Function Name   : usb_hmsc_GetData
Description     : Receive Data request
Arguments       : uint16_t drvnum           : Drive Number
                : uint8_t *buff             : Receive Data Buffer Address
                : uint32_t size             : Receive Data Size
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetData( uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    uint16_t    msgnum;

    msgnum  = usb_hmsc_SmpDrive2Msgnum(drvnum);
    /* Device number */
    usb_ghmsc_strage_num = msgnum;
    /* Transfer data address */
    usb_ghmsc_ReceiveData[msgnum].tranadr   = (void*)buff;
    /* Transfer data length */
    usb_ghmsc_ReceiveData[msgnum].tranlen   = size;
    /* Not control transfer */
    usb_ghmsc_ReceiveData[msgnum].setup     = 0;
    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[msgnum].complete  = (usb_cb_t)&usb_hmsc_DummyFunction;

    usb_hmsc_Submitutr( (uint16_t)USB_HMSC_TYPE_RECEIVE, &usb_ghmsc_ReceiveData[msgnum]);
    return USB_E_OK;
}   /* eof usb_hmsc_GetData() */

/******************************************************************************
Function Name   : usb_hmsc_GetDataReq
Description     : Get Data request
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetDataReq( uint16_t drvnum )
{
    usb_er_t    err;
    uint16_t    msgnum;

    msgnum  = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[msgnum].complete  = (usb_cb_t)&usb_hmsc_CheckResultInPipe;

    err = usb_hmsc_SubmitutrReq( (uint16_t)USB_HMSC_TYPE_RECEIVE, &usb_ghmsc_ReceiveData[msgnum]);
    if( err != USB_E_OK ) 
    {
        return USB_HMSC_SUBMIT_ERR;
    }
    return err;
}   /* eof usb_hmsc_GetDataReq() */

/******************************************************************************
Function Name   : usb_hmsc_GetDataCheck
Description     : Check Get Data 
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetDataCheck( uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t pipeno, msgnum;
    
    msgnum  = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* NonOS */
    switch( hmsc_retval ) 
    {
    case USB_DATA_SHT:
        /* Continue */
    case USB_DATA_OK:
        pipeno = R_usb_hmsc_Information(usb_ghmsc_InPipe[msgnum][0]);
        usb_ghmsc_InPipe[msgnum][1] = usb_ghmsc_ReceiveData[msgnum].pipectr;
        return  USB_HMSC_OK;
        break;
    case USB_DATA_STALL:
        R_usb_hmsc_ClearStall( (uint16_t)USB_HMSC_TYPE_RECEIVE, msgnum, (usb_cb_t)usb_hmsc_ClearStallCheck2);
        return  USB_HMSC_STALL;
        break;
    case USB_DATA_TMO:
        pipeno = R_usb_hmsc_Information(usb_ghmsc_InPipe[msgnum][0]);
        R_usb_hstd_TransferEnd( pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        break;
    case USB_DATA_OVR:
        break;
    default:
        break;
    }
    return USB_HMSC_DAT_RD_ERR;
}   /* eof usb_hmsc_GetDataCheck() */

/******************************************************************************
Function Name   : usb_hmsc_SendData
Description     : Send Pipe Data
Arguments       : uint16_t drvnum           : Drive Number
                : uint8_t  *buff            : Data Info Address
                : uint32_t size             : Data Size
Return value    : uint16_t                  : Error Code(USB_E_OK)
******************************************************************************/
uint16_t usb_hmsc_SendData( uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);
    /* Device number */
    usb_ghmsc_strage_num = msgnum;
    /* Transfer data address */
    usb_ghmsc_TransData[msgnum].tranadr     = (void*)buff;
    /* Transfer data length */
    usb_ghmsc_TransData[msgnum].tranlen     = size;
    /* Not control transfer */
    usb_ghmsc_TransData[msgnum].setup       = 0;
    /* Call Back Function Info */
    usb_ghmsc_TransData[msgnum].complete = (usb_cb_t)&usb_hmsc_DummyFunction;

    usb_hmsc_Submitutr( (uint16_t)USB_HMSC_TYPE_TRANSMIT, &usb_ghmsc_TransData[msgnum]);
    return USB_E_OK;
}   /* eof usb_hmsc_SendData() */

/******************************************************************************
Function Name   : usb_hmsc_SendDataReq
Description     : Send Pipe Data
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendDataReq( uint16_t drvnum )
{
    usb_er_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);
    /* Call Back Function Info */
    usb_ghmsc_TransData[msgnum].complete    = (usb_cb_t)&usb_hmsc_CheckResultOutPipe;

    err = usb_hmsc_SubmitutrReq( (uint16_t)USB_HMSC_TYPE_TRANSMIT, &usb_ghmsc_TransData[msgnum]);
    if( err != USB_E_OK ) 
    {
        return USB_HMSC_SUBMIT_ERR;
    }
    return err;
}   /* eof usb_hmsc_SendDataReq() */

/******************************************************************************
Function Name   : usb_hmsc_SendDataCheck
Description     : Check Send Data
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendDataCheck( uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t    pipeno, msgnum;
    
    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    switch( hmsc_retval ) 
    {
    case USB_DATA_OK:
        pipeno = R_usb_hmsc_Information(usb_ghmsc_OutPipe[msgnum][0]);
        usb_ghmsc_OutPipe[msgnum][1] = usb_ghmsc_TransData[msgnum].pipectr;
        return USB_HMSC_OK;
        break;
    case USB_DATA_STALL:
        R_usb_hmsc_ClearStall( (uint16_t)USB_HMSC_TYPE_TRANSMIT, msgnum, (usb_cb_t)usb_hmsc_ClearStallCheck2);
        return USB_HMSC_STALL;
        break;
    case USB_DATA_TMO:
        pipeno  = R_usb_hmsc_Information(usb_ghmsc_OutPipe[msgnum][0]);
        R_usb_hstd_TransferEnd( pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        break;
    default:
        break;
    }
    return USB_HMSC_DAT_WR_ERR;
}   /* eof usb_hmsc_SendDataCheck() */

/******************************************************************************
Function Name   : usb_hmsc_GetCsw
Description     : Receive CSW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetCsw( uint16_t drvnum)
{
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* Request */
    /* Device number */
    usb_ghmsc_strage_num = msgnum;
    /* Transfer data address */
    usb_ghmsc_ReceiveData[msgnum].tranadr   = (void*)&usb_ghmsc_Csw[msgnum];
    /* Transfer data length */
    usb_ghmsc_ReceiveData[msgnum].tranlen   = (uint32_t)USB_MSC_CSW_LENGTH;
    /* Not control transfer */
    usb_ghmsc_ReceiveData[msgnum].setup     = 0;
    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[msgnum].complete  = (usb_cb_t)&usb_hmsc_DummyFunction;

    usb_hmsc_Submitutr( (uint16_t)USB_HMSC_TYPE_RECEIVE, &usb_ghmsc_ReceiveData[msgnum]);
    return USB_E_OK;
}   /* eof usb_hmsc_GetCsw() */

/******************************************************************************
Function Name   : usb_hmsc_GetCswReq
Description     : Request Receive CSW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetCswReq( uint16_t drvnum)
{
    usb_er_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* Transfer data length */
    usb_ghmsc_ReceiveData[msgnum].tranlen   = (uint32_t)USB_MSC_CSW_LENGTH;
    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[msgnum].complete  = (usb_cb_t)&usb_hmsc_CheckResultInPipe;

    err = usb_hmsc_SubmitutrReq( (uint16_t)USB_HMSC_TYPE_RECEIVE, &usb_ghmsc_ReceiveData[msgnum]);
    if( err != USB_E_OK ) 
    {
        return USB_HMSC_SUBMIT_ERR;
    }
    return err;
}   /* eof usb_hmsc_GetCswReq() */

/******************************************************************************
Function Name   : usb_hmsc_GetCswCheck
Description     : Check Receive CSW
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetCswCheck( uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t        pipeno, msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    switch( hmsc_retval ) 
    {
    case USB_DATA_SHT:
        /* Continue */
    case USB_DATA_OK:
        /* CSW Check */
        pipeno = R_usb_hmsc_Information(usb_ghmsc_InPipe[msgnum][0]);
        usb_ghmsc_InPipe[msgnum][1] = usb_ghmsc_ReceiveData[msgnum].pipectr;
        return usb_hmsc_CheckCsw( drvnum);
        break;
    case USB_DATA_STALL:
        /* Stall */
        return USB_MSG_HMSC_DATA_STALL;
        break;
    case USB_DATA_TMO:
        /* Timeout */
        pipeno = R_usb_hmsc_Information(usb_ghmsc_InPipe[msgnum][0]);
        R_usb_hstd_TransferEnd( pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        break;
    case USB_DATA_OVR:
        break;
    default:
        break;
    }
    return USB_HMSC_CSW_ERR;
}   /* eof usb_hmsc_GetCswCheck() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringInfoCheck
Description     : Check Get string descriptor infomation
Arguments       : uint16_t devaddr          : Device Address
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetStringInfoCheck(uint16_t devaddr)
{
    if( usb_ghmsc_ClassData[0] < (uint8_t)(30 * 2 + 2) ) 
    {
        usb_ghmsc_ClassData[0]  = (uint8_t)(usb_ghmsc_ClassData[0] / 2);
        usb_ghmsc_ClassData[0]  = (uint8_t)(usb_ghmsc_ClassData[0] - 1);
    } 
    else 
    {
        usb_ghmsc_ClassData[0]  = 30;
    }
    return USB_E_OK;
}   /* eof usb_hmsc_GetStringInfoCheck() */

/******************************************************************************
Function Name   : usb_hmsc_GetMaxUnitCheck
Description     : Check Get Max LUN request
Argument        : uint16_t err              : Error Code
Return value    : usb_er_t                 : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetMaxUnitCheck( uint16_t err)
{
    if( err == (uint16_t)USB_DATA_STALL ) 
    {
        return USB_ERROR;
    } 
    else if( err == (uint16_t)USB_DATA_TMO ) 
    {
        usb_hmsc_ControlEnd( (uint16_t)USB_DATA_TMO);
        return USB_ERROR;
    } 
    else if( err != (uint16_t)USB_CTRL_END ) 
    {
        return USB_ERROR;
    } 
    else 
    {
    }
    return (usb_ghmsc_Data[0]);
}   /* eof usb_hmsc_GetMaxUnitCheck() */

/******************************************************************************
Function Name   : usb_hmsc_MassStorageResetCheck
Description     : Check Mass Strage Reset request
Argument        : uint16_t err              : Error Code
Return value    : usb_er_t                 : Error Code
******************************************************************************/
uint16_t usb_hmsc_MassStorageResetCheck( uint16_t err)
{
    if( err == (uint16_t)USB_DATA_STALL ) 
    {
        return USB_ERROR;
    } 
    else if( err == (uint16_t)USB_DATA_TMO ) 
    {
        usb_hmsc_ControlEnd( (uint16_t)USB_DATA_TMO);
        return USB_ERROR;
    } 
    else if( err != (uint16_t)USB_CTRL_END ) 
    {
        return USB_ERROR;
    } 
    else 
    {
    }
    return USB_E_OK;
}   /* eof usb_hmsc_MassStorageResetCheck() */

/******************************************************************************
Function Name   : usb_hmsc_ClearStall
Description     : Clear Stall
Arguments       : uint16_t Pipe : 
                : usb_cb_t complete : 
Return value    : uint16_t
******************************************************************************/
usb_er_t usb_hmsc_ClearStall( uint16_t Pipe, usb_cb_t complete)
{
    usb_er_t    err;

    err = R_usb_hstd_MgrChangeDeviceState( (usb_cbinfo_t)complete, USB_DO_CLEAR_STALL, Pipe);

    return err;
}   /* eof usb_hmsc_ClearStall() */

/******************************************************************************
Function Name   : usb_hmsc_ClearStallCheck
Description     : Clear Stall Check
Arguments       : uint16_t errcheck : 
Return value    : uint16_t
******************************************************************************/
void usb_hmsc_ClearStallCheck( uint16_t errcheck)
{
    uint16_t retval;

    retval = usb_hmsc_StdReqCheck(errcheck);
    if( retval == USB_E_OK )
    {
        g_usb_CurrentPipe = usb_shmsc_ClearStallPipe;
        usb_cstd_clr_stall();
        usb_cstd_reg_set_pipexctr(USB_SQCLR);
    }
}   /* eof usb_hmsc_ClearStallCheck() */

/******************************************************************************
Function Name   : usb_hmsc_ClearStallCheck2
Description     : Clear Stall Check 2
Arguments       : usb_utr_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_ClearStallCheck2(usb_tskinfo_t *mess)
{
    uint16_t errcheck;

    errcheck = mess->keyword;
    usb_hmsc_ClearStallCheck(errcheck);

    mess->msginfo = usb_shmsc_Process;
    usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
}   /* eof usb_hmsc_ClearStallCheck2() */

/******************************************************************************
Function Name   : usb_hmsc_Submitutr
Description     : Data transfer request
Argument        : uint16_t type             : Data Transmit/Data Receive
                : usb_utr_t *utr_table     : Information Table
Return value    : usb_er_t                 : Error Code
******************************************************************************/
usb_er_t usb_hmsc_Submitutr( uint16_t type, usb_utr_t *utr_table)
{
    uint16_t    pipeno, offset2,i;

    switch( type ) 
    {
    case USB_HMSC_TYPE_CTLR:                      /* Control transfer */
        break;
    case USB_HMSC_TYPE_TRANSMIT:                     /* Data transmit */
        offset2             = usb_ghmsc_OutPipe[usb_ghmsc_strage_num][0];
        pipeno              = R_usb_hmsc_Information(offset2);
        utr_table->pipenum  = pipeno;

        for( i = 0; i < USB_EPL; i++ )
        {
            usb_ghmsc_DefEpTbl[i] = usb_ghmsc_TmpEpTbl[offset2 + i];
        }

        R_usb_hstd_SetPipeRegistration( (uint16_t*)&usb_ghmsc_DefEpTbl, pipeno);
        break;
    case USB_DATA_OK:                           /* Data recieve */
        offset2             = usb_ghmsc_InPipe[usb_ghmsc_strage_num][0];
        pipeno              = R_usb_hmsc_Information(offset2);
        utr_table->pipenum  = pipeno;

        for( i = 0; i < USB_EPL; i++ )
        {
            usb_ghmsc_DefEpTbl[i] = usb_ghmsc_TmpEpTbl[offset2 + i];
        }

        R_usb_hstd_SetPipeRegistration( (uint16_t*)&usb_ghmsc_DefEpTbl, pipeno);
        break;
    default:
        break;
    }
    return USB_E_OK;
}   /* eof usb_hmsc_Submitutr() */

/******************************************************************************
Function Name   : usb_hmsc_SubmitutrReq
Description     : Data transfer request
Argument        : uint16_t type             : Data Transmit/Data Receive
                : usb_utr_t *utr_table      : Information Table
Return value    : usb_er_t                  : Error Code
******************************************************************************/
usb_er_t usb_hmsc_SubmitutrReq( uint16_t type, usb_utr_t *utr_table)
{
    usb_er_t    err;

    switch( type ) 
    {
    case USB_HMSC_TYPE_CTLR:                      /* Control transfer */
        err = R_usb_hstd_TransferStart(utr_table);
        break;
    case USB_HMSC_TYPE_TRANSMIT:                     /* Data transmit */
        utr_table->pipectr = usb_ghmsc_out_pipe_ctr;
        err = R_usb_hstd_TransferStart(utr_table);
        break;
    case USB_HMSC_TYPE_RECEIVE:                       /* Data recieve */
        utr_table->pipectr = usb_ghmsc_in_pipe_ctr;
        err = R_usb_hstd_TransferStart(utr_table);
        break;
    default:
        err = (usb_er_t)USB_HMSC_SUBMIT_ERR;
        break;
    }
    return err;
}   /* eof usb_hmsc_SubmitutrReq() */

/******************************************************************************
Function Name   : usb_hmsc_ControlEnd
Description     : Control end function
Argument        : uint16_t  sts             : Status
Return value    : none
******************************************************************************/
void usb_hmsc_ControlEnd( uint16_t sts)
{
    usb_hstd_control_end(sts);
}   /* eof usb_hmsc_ControlEnd() */

/******************************************************************************
Function Name   : usb_hmsc_DummyFunction
Description     : dummy function
Arguments       : uint16_t data1            : not use
                : uint16_t data2            : not use
Return value    : none
******************************************************************************/
void usb_hmsc_DummyFunction(uint16_t data1, uint16_t data2)
{
}   /* eof usb_hmsc_DummyFunction() */

/******************************************************************************
End  Of File
******************************************************************************/

