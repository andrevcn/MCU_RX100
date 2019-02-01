/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_usb_hmsc_define.h
* Version      : 2.10
* Description  : USB User definition Pre-pro
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
******************************************************************************/
#ifndef __R_USB_HMSC_DEFINE_H__
#define __R_USB_HMSC_DEFINE_H__

/******************************************************************************
Enum define
******************************************************************************/
/* ERROR CODE */
enum usb_ghmsc_Error
{
    USB_HMSC_OK             = (uint8_t)0,
    USB_HMSC_STALL          = (uint8_t)-1,
    USB_HMSC_CBW_ERR        = (uint8_t)-2,     /* CBR error */
    USB_HMSC_DAT_RD_ERR     = (uint8_t)-3,     /* Data IN error */
    USB_HMSC_DAT_WR_ERR     = (uint8_t)-4,     /* Data OUT error */
    USB_HMSC_CSW_ERR        = (uint8_t)-5,     /* CSW error */
    USB_HMSC_CSW_PHASE_ERR  = (uint8_t)-6,     /* Phase error */
    USB_HMSC_SUBMIT_ERR     = (uint8_t)-7,     /* submit error */
};


/*****************************************************************************
Macro definitions
******************************************************************************/
#define USB_HMSC_DEV_DET    (uint8_t)0x00      /* detached device */
#define USB_HMSC_DEV_ATT    (uint8_t)0x01      /* attached device */
#define USB_HMSC_DEV_ENU    (uint8_t)0x02      /* Device enumeration */

#define USB_MAXDRIVE        (uint8_t)1        /* Max drive */
#define USB_MAXSTRAGE       (uint8_t)1         /* Max device */
/* ALISON - TEMP */
//#define USB_MAXDRIVE        (uint8_t)16        /* Max drive */
//#define USB_MAXSTRAGE       (uint8_t)8         /* Max device */

#define USB_DRIVE           0
#define USB_MEDIADRIVE      USB_DRIVE

#define USB_HMSC_TYPE_CTLR                  1   /*  */
#define USB_HMSC_TYPE_TRANSMIT              2   /*  */
#define USB_HMSC_TYPE_RECEIVE               3   /*  */

#define USB_MSG_HMSC_NO_DATA                (uint8_t)0x51
#define USB_MSG_HMSC_DATA_IN                (uint8_t)0x52
#define USB_MSG_HMSC_DATA_OUT               (uint8_t)0x53
#define USB_MSG_HMSC_CSW_ERR                (uint8_t)0x54
#define USB_MSG_HMSC_CSW_PHASE_ERR          (uint8_t)0x55
#define USB_MSG_HMSC_CBW_ERR                (uint8_t)0x56
#define USB_MSG_HMSC_DATA_STALL             (uint8_t)0x57

#define USB_MSG_HMSC_STRG_DRIVE_SEARCH      (uint8_t)0x61
#define USB_MSG_HMSC_DEV_READ_SECTOR_SIZE   (uint8_t)0x62
#define USB_MSG_HMSC_DEV_READ_PARTITION     (uint8_t)0x63
#define USB_MSG_HMSC_STRG_DRIVE_OPEN        (uint8_t)0x64
#define USB_MSG_HMSC_CLS_INIT               (uint8_t)0x65
#define USB_MSG_HMSC_CLS_WAIT               (uint8_t)0x66

#endif  /* __R_USB_HMSC_DEFINE_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
