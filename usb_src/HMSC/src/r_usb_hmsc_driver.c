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
* File Name    : r_usb_hmsc_driver.c
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
#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */
#include "r_usb_usrconfig.h"        /* System definition */
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
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
Private global variables and functions
******************************************************************************/
/* CBW headder */
USB_MSC_CBW_t       usb_ghmsc_Cbw[USB_MAXSTRAGE];
/* CSW headder */
USB_MSC_CSW_t       usb_ghmsc_Csw[USB_MAXSTRAGE];

/******************************************************************************
Private global variables and functions
******************************************************************************/

uint32_t            usb_ghmsc_CswDataResidue[USB_MAXSTRAGE];
/* CBW tag number */
uint32_t            usb_ghmsc_CbwTagNo[USB_MAXSTRAGE];
uint8_t             usb_ghmsc_AtapiFlag[USB_MAXSTRAGE];
uint16_t            usb_ghmsc_AttSts;

uint32_t            usb_ghmsc_Secno;
uint32_t            usb_ghmsc_TransSize;
uint16_t            usb_ghmsc_Side;
uint16_t            usb_ghmsc_Seccnt;
uint8_t             *usb_ghmsc_Buff;
uint16_t            usb_shmsc_Process;
uint16_t            usb_shmsc_NoDataSeq     = USB_SEQ_0;
uint16_t            usb_shmsc_DataInSeq     = USB_SEQ_0;
uint16_t            usb_shmsc_DataOutSeq    = USB_SEQ_0;
uint16_t            usb_shmsc_StallErrSeq   = USB_SEQ_0;
uint16_t            usb_shmsc_DataStallSeq  = USB_SEQ_0;
uint16_t            usb_ghmsc_RootDrive;
uint16_t            usb_ghmsc_CswErrRoop    = USB_NO;

void usb_hmsc_CommandResult(uint16_t result);

/******************************************************************************
External variables and functions
******************************************************************************/
extern uint16_t     usb_ghmsc_in_pipe_ctr;
extern uint16_t     usb_ghmsc_out_pipe_ctr;
extern uint16_t     usb_shmsc_StrgProcess;
extern void         usb_cstd_dummy_function(uint16_t data1, uint16_t data2);

/******************************************************************************
Renesas Abstracted HMSC Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_Task
Description     : USB HMSC Task
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hmsc_Task(void)
{
    usb_tskinfo_t       *mess;
    /* Error code */
    usb_er_t        err;

    /* Receive message */
    err = R_USB_RCV_MSG( USB_HMSC_MBX, (usb_msg_t**)&mess );
    if( err != USB_E_OK )
    {
        return;
    }

    switch( mess->msginfo )
    {
    case USB_MSG_HMSC_CLS_INIT:
        usb_hmsc_ClassCheck((usb_tskinfo_t *)mess);
        break;

    case USB_MSG_HMSC_NO_DATA:
        usb_hmsc_NoDataAct((usb_tskinfo_t *)mess);
        break;
    case USB_MSG_HMSC_DATA_IN:
        usb_hmsc_DataInAct((usb_tskinfo_t *)mess);
        break;
    case USB_MSG_HMSC_DATA_OUT:
        usb_hmsc_DataOutAct((usb_tskinfo_t *)mess);
        break;
    case USB_MSG_HMSC_DATA_STALL:
        usb_hmsc_DataStall((usb_tskinfo_t *)mess);
        break;
    case USB_MSG_HMSC_CBW_ERR:
        usb_hmsc_StallErr((usb_tskinfo_t *)mess);
        break;      
    case USB_MSG_HMSC_CSW_PHASE_ERR:
        usb_hmsc_StallErr((usb_tskinfo_t *)mess);
        break;
    default:
        break;
    }
    err = R_USB_REL_BLK(mess->flag);
    if( err != USB_E_OK )
    {
    }
}   /* eof usb_hmsc_Task() */

/******************************************************************************
Function Name   : usb_hmsc_SetRwCbw
Description     : CBW parameter initialization for the READ10/WRITE10 command
Arguments       : uint16_t command : 
                : uint32_t secno : 
                : uint16_t seccnt : 
                : uint32_t trans_byte : 
                : uint16_t side : 
Return value    : none
******************************************************************************/
void usb_hmsc_SetRwCbw(uint16_t command, uint32_t secno, uint16_t seccnt,
     uint32_t trans_byte, uint16_t side)
{
    uint16_t    msgnum;

    /* Data IN */
    msgnum = usb_hmsc_SmpDrive2Msgnum(side);

    /* CBW parameter set */
    usb_ghmsc_Cbw[msgnum].dCBWTag = usb_ghmsc_CbwTagNo[msgnum];
    usb_ghmsc_Cbw[msgnum].dCBWDTL_Lo = (uint8_t)trans_byte;
    usb_ghmsc_Cbw[msgnum].dCBWDTL_ML = (uint8_t)(trans_byte >> 8);
    usb_ghmsc_Cbw[msgnum].dCBWDTL_MH = (uint8_t)(trans_byte >> 16);
    usb_ghmsc_Cbw[msgnum].dCBWDTL_Hi = (uint8_t)(trans_byte >> 24);
    usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 0;
    usb_ghmsc_Cbw[msgnum].bmCBWFlags.reserved7 = 0;
    usb_ghmsc_Cbw[msgnum].bCBWLUN.bCBWLUN = usb_hmsc_SmpDrive2Unit(side);
    usb_ghmsc_Cbw[msgnum].bCBWLUN.reserved4 = 0;
    usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 0;
    usb_ghmsc_Cbw[msgnum].bCBWCBLength.reserved3 = 0;

    /* ATAPI_COMMAND */
    usb_ghmsc_Cbw[msgnum].CBWCB[0] = (uint8_t)command;
    /* LUN */
    usb_ghmsc_Cbw[msgnum].CBWCB[1] = 0x00;
    /* sector address */
    usb_ghmsc_Cbw[msgnum].CBWCB[2] = (uint8_t)(secno >> 24);
    usb_ghmsc_Cbw[msgnum].CBWCB[3] = (uint8_t)(secno >> 16);
    usb_ghmsc_Cbw[msgnum].CBWCB[4] = (uint8_t)(secno >> 8);
    usb_ghmsc_Cbw[msgnum].CBWCB[5] = (uint8_t)secno;
    /* Reserved */
    usb_ghmsc_Cbw[msgnum].CBWCB[6] = 0x00;
    /* Sector length */
    usb_ghmsc_Cbw[msgnum].CBWCB[7] = (uint8_t)(seccnt >> 8);
    /* Block address */
    usb_ghmsc_Cbw[msgnum].CBWCB[8] = (uint8_t)seccnt;
    /* Control data */
    usb_ghmsc_Cbw[msgnum].CBWCB[9] = (uint8_t)0x00;

    /* ATAPI command check */
    switch( command )
    {
    case USB_ATAPI_TEST_UNIT_READY:
    case USB_ATAPI_REQUEST_SENSE:
    case USB_ATAPI_INQUIRY:
    case USB_ATAPI_MODE_SELECT6:
    case USB_ATAPI_MODE_SENSE6:
    case USB_ATAPI_START_STOP_UNIT:
    case USB_ATAPI_PREVENT_ALLOW:
    case USB_ATAPI_READ_FORMAT_CAPACITY:
    case USB_ATAPI_READ_CAPACITY:
        break;
    /* Initialized READ CBW TAG */
    case USB_ATAPI_READ10:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        /* 10bytes */
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    /* Initialized WRITE CBW TAG */
    case USB_ATAPI_WRITE10:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 0;
        /* 10bytes */
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    case USB_ATAPI_SEEK:
    case USB_ATAPI_WRITE_AND_VERIFY:
    case USB_ATAPI_VERIFY10:
    case USB_ATAPI_MODE_SELECT10:
    case USB_ATAPI_MODE_SENSE10:
    default:
        break;
    }

    if( usb_ghmsc_AtapiFlag[msgnum] == USB_ATAPI )
    {
        /* 12bytes */
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength
        = USB_MSC_CBWCB_LENGTH;
    }
}   /* eof usb_hmsc_SetRwCbw() */

/******************************************************************************
Function Name   : usb_hmsc_SetElsCbw
Description     : CBW parameter initialization for other commands
Arguments       : uint8_t *data : 
                : uint32_t trans_byte : 
                : uint16_t side : 
Return value    : none
******************************************************************************/
void usb_hmsc_SetElsCbw(uint8_t *data, uint32_t trans_byte, uint16_t side)
{
    uint8_t     i;
    uint16_t    msgnum;

    /* Data IN */
    msgnum = usb_hmsc_SmpDrive2Msgnum(side);

    /* CBW parameter set */
    usb_ghmsc_Cbw[msgnum].dCBWTag = usb_ghmsc_CbwTagNo[msgnum];
    usb_ghmsc_Cbw[msgnum].dCBWDTL_Lo = (uint8_t)trans_byte;
    usb_ghmsc_Cbw[msgnum].dCBWDTL_ML = (uint8_t)(trans_byte >> 8);
    usb_ghmsc_Cbw[msgnum].dCBWDTL_MH = (uint8_t)(trans_byte >> 16);
    usb_ghmsc_Cbw[msgnum].dCBWDTL_Hi = (uint8_t)(trans_byte >> 24);
    /* Receive */
    usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 0;
    usb_ghmsc_Cbw[msgnum].bmCBWFlags.reserved7 = 0;
    usb_ghmsc_Cbw[msgnum].bCBWLUN.bCBWLUN = usb_hmsc_SmpDrive2Unit(side);
    usb_ghmsc_Cbw[msgnum].bCBWLUN.reserved4 = 0;
    usb_ghmsc_Cbw[msgnum].bCBWCBLength.reserved3 = 0;

    for( i = 0; i < 12; i++ )
    {
        usb_ghmsc_Cbw[msgnum].CBWCB[i] = data[i];
    }

    /* ATAPI command check */
    switch( data[0] )
    {
    /* No data */
    case USB_ATAPI_TEST_UNIT_READY:
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* Receive */
    case USB_ATAPI_REQUEST_SENSE:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* Send */
    case USB_ATAPI_FORMAT_UNIT:
        break;
    /* Receive */
    case USB_ATAPI_INQUIRY:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    case USB_ATAPI_MODE_SELECT6:
    case USB_ATAPI_MODE_SENSE6:
        break;
    /* No data */
    case USB_ATAPI_START_STOP_UNIT:
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* No data */
    case USB_ATAPI_PREVENT_ALLOW:
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* Receive */
    case USB_ATAPI_READ_FORMAT_CAPACITY:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    /* Receive */
    case USB_ATAPI_READ_CAPACITY:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    case USB_ATAPI_READ10:
    case USB_ATAPI_WRITE10:
        break;
    case USB_ATAPI_SEEK:
    case USB_ATAPI_WRITE_AND_VERIFY:
    case USB_ATAPI_VERIFY10:
        break;
    /* Send */
    case USB_ATAPI_MODE_SELECT10:
        break;
    /* Receive */
    case USB_ATAPI_MODE_SENSE10:
        usb_ghmsc_Cbw[msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    default:
        break;
    }

    if( usb_ghmsc_AtapiFlag[msgnum] == USB_ATAPI )
    {
        /* 12bytes */
        usb_ghmsc_Cbw[msgnum].bCBWCBLength.bCBWCBLength
        = USB_MSC_CBWCB_LENGTH;
    }
}   /* eof usb_hmsc_SetElsCbw() */

/******************************************************************************
Function Name   : usb_hmsc_CbwTagCount
Description     : Updates tag information
Arguments       : uint16_t msgnum : 
Return value    : none
******************************************************************************/
void usb_hmsc_CbwTagCount(uint16_t msgnum)
{
    usb_ghmsc_CbwTagNo[msgnum]++;
    if( usb_ghmsc_CbwTagNo[msgnum] == (uint16_t)0 )
    {
        usb_ghmsc_CbwTagNo[msgnum] = (uint16_t)1;
    }
}   /* eof usb_hmsc_CbwTagCount() */

/******************************************************************************
Function Name   : usb_hmsc_CheckCsw
Description     : CSW check
Arguments       : uint16_t drvnum : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_CheckCsw(uint16_t drvnum)
{
    uint16_t    msgnum;
    usb_tskinfo_t mess;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    /* CSW Check */
    usb_ghmsc_CswDataResidue[msgnum]    =
                 (uint32_t)(usb_ghmsc_Csw[msgnum].dCSWDataResidue_Lo)
                | (uint32_t)(usb_ghmsc_Csw[msgnum].dCSWDataResidue_ML) << 8
                | (uint32_t)(usb_ghmsc_Csw[msgnum].dCSWDataResidue_MH) << 16
                | (uint32_t)(usb_ghmsc_Csw[msgnum].dCSWDataResidue_Hi) << 24;

    if( usb_ghmsc_Csw[msgnum].dCSWSignature != USB_MSC_CSW_SIGNATURE )
    {
        return USB_HMSC_CSW_ERR;
    }
    if( usb_ghmsc_Csw[msgnum].dCSWTag != usb_ghmsc_Cbw[msgnum].dCBWTag )
    {
        return USB_HMSC_CSW_ERR;
    }
    switch( usb_ghmsc_Csw[msgnum].bCSWStatus )
    {
    case USB_MSC_CSW_OK:
        return USB_HMSC_OK;
        break;
    case USB_MSC_CSW_NG:
        return USB_HMSC_CSW_ERR;
        break;
    case USB_MSC_CSW_PHASE_ERR:
        usb_shmsc_Process = USB_MSG_HMSC_CSW_PHASE_ERR;
        usb_shmsc_StallErrSeq = USB_SEQ_0;
        mess.keyword = drvnum;
        usb_hmsc_SpecifiedPath((usb_tskinfo_t *)&mess );
        return USB_HMSC_CSW_PHASE_ERR;
        break;
    default:
        break;
    }
    return USB_HMSC_CSW_ERR;
}   /* eof usb_hmsc_CheckCsw() */

/******************************************************************************


































Function Name   : usb_hmsc_SmpBotDescriptor
Description     : BOT Descriptor
Arguments       : uint8_t *table : 
                : uint16_t msgnum : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_SmpBotDescriptor(uint8_t *table, uint16_t msgnum)
{
    /* Check Descriptor */
    switch( table[1] )
    {
    /* Device Descriptor */
    case USB_DT_DEVICE:
        return USB_ERROR;
        break;
    /* Configuration Descriptor */
    case USB_DT_CONFIGURATION:
        return USB_ERROR;
        break;
    /* String Descriptor */
    case USB_DT_STRING:
        return USB_ERROR;
        break;
    /* Interface Descriptor ? */
    case USB_DT_INTERFACE:
        /* Check Interface Descriptor (deviceclass) */
        if( table[5] != USB_IFCLS_MAS )
        {
            return USB_ERROR;
        }
        /* Check Interface Descriptor (subclass) */
        if( table[6] == USB_ATAPI )
        {
            /* ATAPI Command */
            usb_ghmsc_AtapiFlag[msgnum] = USB_ATAPI;
        }
        else if (table[6] == USB_SCSI)
        {
            /* SCSI Command */
            usb_ghmsc_AtapiFlag[msgnum] = USB_SCSI;
        }
        else if (table[6] == USB_ATAPI_MMC5)
        {
            /* ATAPI Command */
            usb_ghmsc_AtapiFlag[msgnum] = USB_ATAPI_MMC5;
        }
        else
        {
            /* Unknown Command */
            usb_ghmsc_AtapiFlag[msgnum] = USB_NULL;
            return USB_ERROR;
        }
        /* Check Interface Descriptor (protocol) */
        if( table[7] == USB_BOTP )
        {
        }
        else
        {
            return USB_ERROR;
        }
        /* Check Endpoint number */
        if( table[4] < USB_TOTALEP )
        {
            return USB_ERROR;
        }
        return USB_E_OK;
        break;
    /* Endpoint Descriptor */
    case USB_DT_ENDPOINT:
        return USB_ERROR;
        break;
    /* Device Qualifier Descriptor */
    case USB_DT_DEVICE_QUALIFIER:
        return USB_ERROR;
        break;
    /* Other Speed Configuration Descriptor */
    case USB_DT_OTHER_SPEED_CONF:
        return USB_ERROR;
        break;
    /* Interface Power Descriptor */
    case USB_DT_INTERFACE_POWER:
        return USB_ERROR;
        break;
    /* Not Descriptor */
    default:
        break;
    }
    return USB_ERROR;
}   /* eof usb_hmsc_SmpBotDescriptor() */

/******************************************************************************
Function Name   : usb_hmsc_SmpPipeInfo
Description     : Pipe Information
Arguments       : uint8_t *table : 
                : uint16_t msgnum : 
                : uint16_t length : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_SmpPipeInfo(uint8_t *table, uint16_t msgnum, uint16_t length)
{
    uint16_t        dirflag = 0;
    uint16_t        ofdsc, offset;
    uint16_t        retval;

    /* Check Descriptor */
    if( table[1] != USB_DT_INTERFACE )
    {
        /* Configuration Descriptor */
        return USB_ERROR;
    }

    offset = (uint16_t)(2u * USB_EPL * msgnum);

    /* Check Endpoint Descriptor */
    ofdsc = table[0];
    /* Pipe initial */
    usb_ghmsc_OutPipe[msgnum][0]    = USB_NOPORT;
    usb_ghmsc_InPipe[msgnum][0]     = USB_NOPORT;
    /* Toggle clear */
    usb_ghmsc_OutPipe[msgnum][1]    = 0;
    usb_ghmsc_InPipe[msgnum][1]     = 0;
    while( ofdsc < (length - table[0]) )
    {
        /* Search within Interface */
        switch( table[ofdsc + 1] )
        {
        /* Device Descriptor ? */
        case USB_DT_DEVICE:
        /* Configuration Descriptor ? */
        case USB_DT_CONFIGURATION:
        /* String Descriptor ? */
        case USB_DT_STRING:
        /* Interface Descriptor ? */
        case USB_DT_INTERFACE:
            return USB_ERROR;
            break;
        /* Endpoint Descriptor */
        case USB_DT_ENDPOINT:
            /* Bulk Endpoint */
            if( (table[ofdsc + 3] & USB_EP_TRNSMASK) == USB_EP_BULK )
            {
                switch( dirflag )
                {
                case 0:
                    retval = R_usb_hstd_ChkPipeInfo( &usb_ghmsc_TmpEpTbl[offset], &table[ofdsc]);
                    if( retval == USB_DIR_H_OUT )
                    {
                        usb_ghmsc_OutPipe[msgnum][0] = offset;
                    }
                    if( retval == USB_DIR_H_IN )
                    {
                        usb_ghmsc_InPipe[msgnum][0] = offset;
                    }
                    dirflag++;
                    break;
                case 1:
                    retval = R_usb_hstd_ChkPipeInfo( &usb_ghmsc_TmpEpTbl[offset + USB_EPL], &table[ofdsc]);
                    if( retval == USB_DIR_H_OUT )
                    {
                        usb_ghmsc_OutPipe[msgnum][0] = (uint16_t)(offset + USB_EPL);
                    }
                    if( retval == USB_DIR_H_IN )
                    {
                        usb_ghmsc_InPipe[msgnum][0] = (uint16_t)(offset + USB_EPL);
                    }
                    if( (usb_ghmsc_InPipe[msgnum][0] != USB_NOPORT) && (usb_ghmsc_OutPipe[msgnum][0] != USB_NOPORT) )
                    {
                        return USB_E_OK;
                    }
                    break;
                default:
                    break;
                }
            }
            ofdsc += table[ofdsc];
            break;
        /* Device Qualifier Descriptor */
        case USB_DT_DEVICE_QUALIFIER:
        /* Other Speed Configuration Descriptor */
        case USB_DT_OTHER_SPEED_CONF:
        /* Interface Power Descriptor */
        case USB_DT_INTERFACE_POWER:
            return USB_ERROR;
            break;
        /* Antoher Descriptor */
        default:
            ofdsc += table[ofdsc];
            break;
        }
    }
    return USB_ERROR;
}   /* eof usb_hmsc_SmpPipeInfo() */

/******************************************************************************
Function Name   : usb_hmsc_NoData
Description     : HMSC No data
Arguments       : uint16_t drvnum : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_NoData(uint16_t drvnum)
{
    usb_tskinfo_t mess;

    mess.keyword = drvnum;
    usb_shmsc_Process = USB_MSG_HMSC_NO_DATA;
    usb_hmsc_SpecifiedPath(&mess);
    usb_shmsc_NoDataSeq = USB_SEQ_0;

    return USB_E_OK;
}   /* eof usb_hmsc_NoData() */

/******************************************************************************
Function Name   : usb_hmsc_NoDataAct
Description     : No Data Request
Arguments       : usb_tskinfo_t *mess : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_NoDataAct(usb_tskinfo_t *mess)
{
    uint16_t    hmsc_retval, result;
    uint16_t    drvnum;
    
    drvnum = usb_ghmsc_RootDrive;
    result = mess->keyword;
    
    switch( usb_shmsc_NoDataSeq )
    {
    case USB_SEQ_0:
        /* CBW */
        drvnum = mess->keyword;
        usb_ghmsc_RootDrive = drvnum;
        hmsc_retval = usb_hmsc_SendCbw(drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_NO_DATA;
        usb_shmsc_NoDataSeq++;
        usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
        break;
    case USB_SEQ_1:
        hmsc_retval = usb_hmsc_SendCbwReq(drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_NO_DATA;
        usb_shmsc_NoDataSeq++;
        break;
    case USB_SEQ_2:
        hmsc_retval = usb_hmsc_SendCbwCheck(drvnum, result);
        if( hmsc_retval == USB_DATA_STALL )
        {
            usb_shmsc_NoDataSeq = USB_SEQ_0;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            usb_shmsc_NoDataSeq = USB_SEQ_0;
            usb_hmsc_CommandResult(hmsc_retval);
        }
        else
        {
            /* CSW */
            hmsc_retval = usb_hmsc_GetCsw(drvnum);
            
            usb_shmsc_Process = USB_MSG_HMSC_NO_DATA;
            usb_shmsc_NoDataSeq++;
            usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
        }
        break;
    case USB_SEQ_3:
        hmsc_retval = usb_hmsc_GetCswReq(drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_NO_DATA;
        usb_shmsc_NoDataSeq++;
        break;
    case USB_SEQ_4:
        hmsc_retval = usb_hmsc_GetCswCheck(drvnum, result);

        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if(usb_ghmsc_CswErrRoop == USB_YES)
            {
                usb_ghmsc_CswErrRoop = USB_NO;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult( hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            usb_ghmsc_CswErrRoop = USB_YES;
            drvnum = usb_ghmsc_RootDrive;
            R_usb_hmsc_RequestSense(drvnum, (uint8_t *)usb_ghmsc_Data);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
            mess->keyword = drvnum;
            usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
            break;
        default:
            if(usb_ghmsc_CswErrRoop == USB_YES)
            {
                usb_ghmsc_CswErrRoop  =USB_NO;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult( hmsc_retval);
            break;
        }
        usb_shmsc_NoDataSeq = USB_SEQ_0;
        break;
    default:
        usb_hmsc_CommandResult( hmsc_retval);
        usb_shmsc_NoDataSeq = USB_SEQ_0;
        break;
    }
    return (hmsc_retval);
}   /* eof usb_hmsc_NoDataAct() */

/******************************************************************************
Function Name   : usb_hmsc_DataIn
Description     : HMSC Data In
Arguments       : uint16_t drvnum : 
                : uint8_t *buff : 
                : uint32_t size : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataIn( uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    usb_tskinfo_t mess;

    mess.keyword = drvnum;
    usb_ghmsc_Buff = buff;
    usb_ghmsc_TransSize = size;

    usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
    usb_hmsc_SpecifiedPath(&mess);
    usb_shmsc_DataInSeq = USB_SEQ_0;

    return USB_E_OK;
}   /* eof usb_hmsc_DataIn() */

/******************************************************************************
Function Name   : usb_hmsc_DataInAct
Description     : Receive Data request
Arguments       : usb_tskinfo_t *mess : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataInAct(usb_tskinfo_t *mess)
{
    uint16_t    hmsc_retval, result;
    uint16_t    drvnum;
    uint32_t    size;
    uint8_t     *buff;

    drvnum = usb_ghmsc_RootDrive;
    buff = usb_ghmsc_Buff;
    size = usb_ghmsc_TransSize;
    result = mess->keyword;

    switch( usb_shmsc_DataInSeq )
    {
    case USB_SEQ_0:
        /* CBW */
        drvnum = mess->keyword;
        usb_ghmsc_RootDrive = drvnum;
        hmsc_retval = usb_hmsc_SendCbw(drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
        usb_shmsc_DataInSeq++;
        usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
        break;
    case USB_SEQ_1:
        hmsc_retval = usb_hmsc_SendCbwReq(drvnum);
        usb_shmsc_DataInSeq++;
        break;
    case USB_SEQ_2:
        hmsc_retval = usb_hmsc_SendCbwCheck(drvnum, result);
        if( hmsc_retval == USB_DATA_STALL )
        {
            usb_shmsc_DataInSeq = USB_SEQ_0;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            usb_shmsc_DataInSeq = USB_SEQ_0;
            usb_hmsc_CommandResult( hmsc_retval);
        }
        else
        {
/* Data */
            hmsc_retval = usb_hmsc_GetData(drvnum, buff, size);
            usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
            usb_shmsc_DataInSeq++;
            usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
        }
        break;
    case USB_SEQ_3:
        hmsc_retval = usb_hmsc_GetDataReq(drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
        usb_shmsc_DataInSeq++;
        break;
    case USB_SEQ_4:
        hmsc_retval = usb_hmsc_GetDataCheck(drvnum, result);
        if( hmsc_retval == USB_HMSC_STALL )
        {
            usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
            usb_shmsc_DataInSeq++;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            usb_hmsc_CommandResult( hmsc_retval);
            usb_shmsc_DataInSeq = USB_SEQ_0;
        }
        else
        {
            /* CSW */
            hmsc_retval = usb_hmsc_GetCsw(drvnum);

            usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
            usb_shmsc_DataInSeq++;
            usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
        }
        break;
    case USB_SEQ_5:
        hmsc_retval = usb_hmsc_GetCswReq(drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_IN;
        usb_shmsc_DataInSeq++;
        break;
    case USB_SEQ_6:
        hmsc_retval = usb_hmsc_GetCswCheck(drvnum, result);
        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if(usb_ghmsc_CswErrRoop == USB_YES)
            {
                usb_ghmsc_CswErrRoop = USB_NO;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult( hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            usb_ghmsc_CswErrRoop = USB_YES;
            drvnum = usb_ghmsc_RootDrive;
            R_usb_hmsc_RequestSense(drvnum, (uint8_t*)usb_ghmsc_Data);
//            R_usb_hmsc_RequestSense(drvnum, (uint8_t*)&usb_ghmsc_Data); /* TEMP - ALISON */
            break;
        case USB_MSG_HMSC_DATA_STALL:
            usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
            mess->keyword = drvnum;
            usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
            break;
        default:
            if(usb_ghmsc_CswErrRoop == USB_YES)
            {
                usb_ghmsc_CswErrRoop = USB_NO;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult( hmsc_retval);
            break;
        }
        
        usb_shmsc_DataInSeq = USB_SEQ_0;
        break;
    default:
        usb_hmsc_CommandResult( hmsc_retval);
        usb_shmsc_DataInSeq = USB_SEQ_0;
        break;
    }
    /* Data read error */
    return (hmsc_retval);
}   /* eof usb_hmsc_DataInAct() */

/******************************************************************************
Function Name   : usb_hmsc_DataOut
Description     : HMSC Data Out
Arguments       : uint16_t drvnum : 
                : uint8_t *buff : 
                : uint32_t size : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataOut( uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    usb_tskinfo_t mess;

    mess.keyword = drvnum;
    usb_ghmsc_Buff = buff;
    usb_ghmsc_TransSize = size;
    usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
    usb_hmsc_SpecifiedPath(&mess);
//    usb_shmsc_DataOutSeq = USB_SEQ_0;
    usb_shmsc_DataOutSeq = usb_shmsc_DataOutSeq;
    
    return USB_E_OK;
}   /* eof usb_hmsc_DataOut() */

/******************************************************************************
Function Name   : usb_hmsc_DataOutAct
Description     : Send Data request
Arguments       : usb_tskinfo_t *mess : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataOutAct(usb_tskinfo_t *mess)
{
    uint16_t    hmsc_retval, result;
    uint16_t    drvnum;
    uint8_t     *buff;
    uint32_t    size;

    drvnum = usb_ghmsc_RootDrive;
    buff = usb_ghmsc_Buff;
    size = usb_ghmsc_TransSize;
    result = mess->keyword;
    
    switch( usb_shmsc_DataOutSeq )
    {
    case USB_SEQ_0:
        /* CBW */
        drvnum = mess->keyword;
        usb_ghmsc_RootDrive = drvnum;
        hmsc_retval = usb_hmsc_SendCbw(drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq++;
        usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
        break;
    case USB_SEQ_1:
        hmsc_retval = usb_hmsc_SendCbwReq(drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq++;
        break;
    case USB_SEQ_2:
        hmsc_retval = usb_hmsc_SendCbwCheck(drvnum, result);
        if( hmsc_retval == USB_DATA_STALL )
        {
            usb_shmsc_DataOutSeq = USB_SEQ_0;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            usb_shmsc_DataOutSeq = USB_SEQ_0;
            usb_hmsc_CommandResult( hmsc_retval);
        }
        else
        {
            /* Data */
            usb_hmsc_SendData(drvnum, buff, size);
            usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
            usb_shmsc_DataOutSeq++;
            usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
        }
        break;
    case USB_SEQ_3:
        usb_hmsc_SendDataReq(drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq++;
        break;
    case USB_SEQ_4:
        hmsc_retval = usb_hmsc_SendDataCheck(drvnum, result);
        if( hmsc_retval == USB_HMSC_STALL )
        {
            usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
            usb_shmsc_DataOutSeq++;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            usb_hmsc_CommandResult( hmsc_retval);
            usb_shmsc_DataOutSeq = USB_SEQ_0;
        }
        else
        {
            /* CSW */
            hmsc_retval = usb_hmsc_GetCsw(drvnum);
            usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
            usb_shmsc_DataOutSeq++;
            usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
        }
        break;
    case USB_SEQ_5:
        hmsc_retval = usb_hmsc_GetCswReq(drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq++;
        break;
    case USB_SEQ_6:
        hmsc_retval = usb_hmsc_GetCswCheck(drvnum,result);
        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if( usb_ghmsc_CswErrRoop == USB_YES )
            {
                usb_ghmsc_CswErrRoop = USB_NO;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult( hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            usb_ghmsc_CswErrRoop = USB_YES;
            drvnum = usb_ghmsc_RootDrive;
            R_usb_hmsc_RequestSense(drvnum, (uint8_t*)usb_ghmsc_Data);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
            mess->keyword = drvnum;
            usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
            break;
        default:
            if( usb_ghmsc_CswErrRoop == USB_YES )
            {
                usb_ghmsc_CswErrRoop = USB_NO;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult( hmsc_retval);
            break;
        }

        usb_shmsc_DataOutSeq = USB_SEQ_0;
        break;
    default:
        usb_shmsc_DataOutSeq = USB_SEQ_0;
        usb_hmsc_CommandResult( hmsc_retval);
        break;
    }
    /* Data read error */
    return (hmsc_retval);
}   /* eof usb_hmsc_DataOutAct() */

/******************************************************************************
Function Name   : usb_hmsc_SpecifiedPath
Description     : Next Process Selector
Arguments       : usb_tskinfo_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_SpecifiedPath(usb_tskinfo_t *mess)
{
    R_USB_GET_SND(USB_HMSC_MBX, usb_shmsc_Process, &usb_cstd_dummy_function, (usb_strct_t)mess->keyword );
}   /* eof usb_hmsc_SpecifiedPath() */

/******************************************************************************
Function Name   : usb_hmsc_CheckResult
Description     : Hub class check result
Arguments       : usb_utr_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_CheckResult(usb_utr_t *mess, uint16_t data1, uint16_t data2)
{
    R_USB_GET_SND(USB_HMSC_MBX, usb_shmsc_Process, &usb_cstd_dummy_function, (usb_strct_t)mess->status );
}   /* eof usb_hmsc_CheckResult() */

/******************************************************************************
Function Name   : usb_hmsc_CheckResultInPipe
Description     : Hub class check result
Arguments       : usb_utr_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_CheckResultInPipe(usb_utr_t *mess, uint16_t data1, uint16_t data2)
{
    usb_ghmsc_in_pipe_ctr = mess->pipectr;
    R_USB_GET_SND(USB_HMSC_MBX, usb_shmsc_Process, &usb_cstd_dummy_function, (usb_strct_t)mess->status );
}   /* eof usb_hmsc_CheckResult() */

/******************************************************************************
Function Name   : usb_hmsc_CheckResultOutPipe
Description     : Hub class check result
Arguments       : usb_utr_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_CheckResultOutPipe(usb_utr_t *mess, uint16_t data1, uint16_t data2)
{
    usb_ghmsc_out_pipe_ctr = mess->pipectr;
    R_USB_GET_SND(USB_HMSC_MBX, usb_shmsc_Process, &usb_cstd_dummy_function, (usb_strct_t)mess->status );
}   /* eof usb_hmsc_CheckResult() */

/******************************************************************************
Function Name   : usb_hmsc_StallErr
Description     : HMSC Stall Error
Arguments       : usb_utr_t *ptr : 
Return value    : none
******************************************************************************/
void usb_hmsc_StallErr(usb_tskinfo_t *ptr)
{
    uint16_t    drvnum, msgnum, result;
    usb_er_t    err;

    drvnum = usb_ghmsc_RootDrive;
    result = ptr->keyword;
    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    switch( usb_shmsc_StallErrSeq )
    {
    case USB_SEQ_0:
        drvnum = ptr->keyword;

        usb_ghmsc_RootDrive = drvnum;

        err = R_usb_hmsc_MassStorageReset( drvnum, (usb_cb_t)usb_hmsc_CheckResult);
        /* Control Transfer overlaps */
        if( err == USB_E_QOVR )
        {
            err = R_USB_GET_SND(USB_HMSC_MBX, ptr->msginfo, &usb_cstd_dummy_function, (usb_strct_t)ptr->keyword );
        }
        else
        {
            /* Control Transfer not overlaps */
            usb_shmsc_StallErrSeq++;
        }
        break;

    case USB_SEQ_1:
        usb_hmsc_MassStorageResetCheck( result);
        R_usb_hmsc_ClearStall( (uint16_t)USB_HMSC_TYPE_TRANSMIT, msgnum,(usb_cb_t)usb_hmsc_CheckResultOutPipe);
        usb_shmsc_StallErrSeq++;
        break;
    case USB_SEQ_2:
        usb_hmsc_ClearStallCheck( result);
        R_usb_hmsc_ClearStall( (uint16_t)USB_HMSC_TYPE_RECEIVE, msgnum,(usb_cb_t)usb_hmsc_CheckResultInPipe);
        usb_shmsc_StallErrSeq++;
        break;
    case USB_SEQ_3:
        usb_hmsc_ClearStallCheck( result);
        if( ptr->msginfo == USB_MSG_HMSC_CSW_PHASE_ERR )
        {
            result = USB_HMSC_CSW_PHASE_ERR;
        }
        else
        {
            result = USB_HMSC_CBW_ERR;
        }
        usb_hmsc_CommandResult( result);
        usb_shmsc_StallErrSeq = USB_SEQ_0;
        break;

    case USB_SEQ_4:
        drvnum = (uint16_t)ptr->keyword;                               /* Device no. set */

        usb_ghmsc_RootDrive = drvnum;

        err = R_usb_hmsc_MassStorageReset( drvnum, (usb_cb_t)usb_hmsc_CheckResult);
        /* Control Transfer overlaps */
        if( err == USB_E_QOVR )
        {
            err = R_USB_GET_SND(USB_HMSC_MBX, ptr->msginfo, &usb_cstd_dummy_function, (usb_strct_t)ptr->keyword );
        }
        else
        {
            /* Control Transfer not overlaps */
            usb_shmsc_StallErrSeq = USB_SEQ_1;
        }
        break;

    default:
        if( ptr->msginfo == USB_MSG_HMSC_CSW_PHASE_ERR )
        {
            result = USB_HMSC_CSW_PHASE_ERR;
        }
        else
        {
            result = USB_HMSC_CBW_ERR;
        }
        usb_hmsc_CommandResult( result);
        usb_shmsc_StallErrSeq = USB_SEQ_0;
        break;
    }
}   /* eof usb_hmsc_StallErr() */

/******************************************************************************
Function Name   : usb_hmsc_DataStall
Description     : HMSC Data Stall
Arguments       : usb_tskinfo_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_DataStall(usb_tskinfo_t *mess)
{
    uint16_t status, drvnum, msgnum, hmsc_retval;

    drvnum = usb_ghmsc_RootDrive;
    status = mess->keyword;

    msgnum = usb_hmsc_SmpDrive2Msgnum(drvnum);

    switch( usb_shmsc_DataStallSeq )
    {
    case USB_SEQ_0:
        drvnum = mess->keyword;
        usb_ghmsc_RootDrive = drvnum;

        R_usb_hmsc_ClearStall( (uint16_t)USB_HMSC_TYPE_RECEIVE, msgnum, (usb_cb_t)usb_hmsc_CheckResultInPipe);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
        usb_shmsc_DataStallSeq++;
        break;
    case USB_SEQ_1:
        usb_hmsc_ClearStallCheck( status);
        usb_hmsc_GetCsw( drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
        usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
        usb_shmsc_DataStallSeq++;
        break;
    case USB_SEQ_2:
        //usb_ghmsc_in_pipe_ctr = 0;
		/* ALISON - TEMP */
        usb_hmsc_GetCswReq( drvnum);
        usb_shmsc_Process = USB_MSG_HMSC_DATA_STALL;
        usb_shmsc_DataStallSeq++;
        break;
    case USB_SEQ_3:
        hmsc_retval = usb_hmsc_GetCswCheck( drvnum,status);
        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if( usb_ghmsc_CswErrRoop == USB_YES )
            {
                usb_ghmsc_CswErrRoop = USB_NO;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult( hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            usb_ghmsc_CswErrRoop = USB_YES;
            drvnum = usb_ghmsc_RootDrive;
            R_usb_hmsc_RequestSense( drvnum, (uint8_t*)usb_ghmsc_Data);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            usb_shmsc_Process = USB_MSG_HMSC_CSW_PHASE_ERR;
            mess->keyword = drvnum;
            usb_hmsc_SpecifiedPath((usb_tskinfo_t *)mess);
            break;
        default:
            if( usb_ghmsc_CswErrRoop == USB_YES )
            {
                usb_ghmsc_CswErrRoop = USB_NO;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult( hmsc_retval);
            break;
        }
        usb_shmsc_DataStallSeq = USB_SEQ_0;
        break;
    default:
        usb_hmsc_CommandResult(USB_HMSC_CSW_ERR);
        usb_shmsc_DataStallSeq = USB_SEQ_0;
        break;
    }
}   /* eof usb_hmsc_DataStall() */

/******************************************************************************
Function Name   : usb_hmsc_CommandResult
Description     : Hub class check result
Arguments       : uint16_t result     : Result
Return value    : none
******************************************************************************/
void usb_hmsc_CommandResult( uint16_t result)
{
    R_USB_GET_SND(USB_HSTRG_MBX, usb_shmsc_StrgProcess, &usb_cstd_dummy_function, (usb_strct_t)result );
}   /* eof usb_hmsc_CommandResult() */

/******************************************************************************
End  Of File
******************************************************************************/
