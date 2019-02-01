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
* File Name    : r_usb_hstorage_driver.c
* Version      : 1.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB sample data declaration
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 17.03.2010 0.80    First Release
*         : 30.07.2010 0.90    Updated comments
*         : 02.08.2010 0.91    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_cTypedef.h"         /* Type definition */
#include "r_usb_usrconfig.h"
#include "r_usb_cdefusbip.h"            /* USB-FW Library Header */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_catapi_define.h"        /* Peripheral ATAPI Device extern */
#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
#include "r_usb_hmsc_extern.h"      /* MSC global definition */

#include "r_usb_api.h"
#include "r_usb_hmsc_api.h"

/******************************************************************************
Section    <Section Definition> , "Project Sections"
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
Constant macro definitions
******************************************************************************/

/******************************************************************************
External variables and functions
******************************************************************************/
extern void     usb_cstd_dummy_function(uint16_t data1, uint16_t data2);

extern uint16_t        usb_shmsc_StrgDriveSearchSeq;

/* Drive Open */
extern uint16_t        usb_shmsc_StrgDriveOpenSeq;

/******************************************************************************
Private global variables and functions
******************************************************************************/


/******************************************************************************
Renesas Abstracted Peripheral Driver functions
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_StrgDriveSearch
Description     : Searches drive SndMsg
Arguments       : uint16_t addr        : Address
Return value    : uint16_t             : Status
******************************************************************************/
uint16_t R_usb_hmsc_StrgDriveSearch(uint16_t addr)
{
    usb_shmsc_StrgDriveSearchSeq = USB_SEQ_0;

    return R_USB_GET_SND(USB_HSTRG_MBX, USB_MSG_HMSC_STRG_DRIVE_SEARCH, &usb_cstd_dummy_function, (usb_strct_t)addr);
}   /* eof R_usb_hmsc_StrgDriveSearch() */


/******************************************************************************
Function Name   : R_usb_hmsc_StrgDriveOpen
Description     : Mounts the drive
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Status
******************************************************************************/
usb_er_t R_usb_hmsc_StrgDriveOpen(uint16_t side )
{
    usb_shmsc_StrgDriveOpenSeq = USB_SEQ_0;

    return R_USB_GET_SND(USB_HSTRG_MBX, USB_MSG_HMSC_STRG_DRIVE_OPEN, &usb_cstd_dummy_function, (usb_strct_t)side);
}   /* eof R_usb_hmsc_StrgDriveOpen() */


/******************************************************************************
Function Name   : R_usb_hmsc_StrgDriveClose
Description     : Releases drive
Arguments       : uint16_t side    : Side
Return value    : uint16_t         : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgDriveClose( uint16_t side )
{
    /* Check drive numberk */
    if( side > usb_hmsc_SmpTotalDrive() )
    {
        return (USB_ERROR);
    }

    if( (USB_MEDIADRIVE != USB_DRIVE) && (side == USB_MEDIADRIVE) )
    {   /* Memory device */
        usb_hmsc_SmpFsiSectorInitialized(side, (uint32_t)0, (uint16_t)0);
        return (USB_E_OK);
    }
    else
    {   /* USB device */
        /* Device Status */
        if( R_usb_hmsc_GetDevSts() != USB_HMSC_DEV_ATT )
        {
            return (USB_ERROR);
        }
        usb_hmsc_SmpFsiSectorInitialized(side, (uint32_t)0, (uint16_t)0);
        return (USB_E_OK);
    }
}   /* eof R_usb_hmsc_StrgDriveClose() */

/******************************************************************************
Function Name   : R_usb_hmsc_StrgReadSector
Description     : Releases drive
Arguments       : uint16_t side        : Side
                : uint8_t  *buff       : Buffer address
                : uint32_t secno       : Sector number
                : uint16_t seccnt      : Sector count
                : uint32_t trans_byte  : Trans byte
Return value    : uint16_t             : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgReadSector( uint16_t side, uint8_t *buff
    , uint32_t secno, uint16_t seccnt, uint32_t trans_byte )
{
    uint16_t    result;

    /* Check drive numberk */
    if( side > usb_hmsc_SmpTotalDrive() )
    {
        return (USB_ERROR);
    }

    /* Device Status */
    if( R_usb_hmsc_GetDevSts() != USB_HMSC_DEV_ATT )
    {
        return (USB_ERROR);
    }
    result = R_usb_hmsc_Read10( side, buff, secno, seccnt, trans_byte);

    return (result);
}   /* eof R_usb_hmsc_StrgReadSector() */

/******************************************************************************
Function Name   : R_usb_hmsc_StrgWriteSector
Description     : Writes sector information
Arguments       : uint16_t side        : Side
                : uint8_t  *buff       : Buffer address
                : uint32_t secno       : Sector number
                : uint16_t seccnt      : Sector count
                : uint32_t trans_byte  : Trans byte
Return value    : uint16_t             : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgWriteSector( uint16_t side, uint8_t *buff
    , uint32_t secno, uint16_t seccnt, uint32_t trans_byte )
{
    uint16_t    result;

    /* Check drive numberk */
    if( side > usb_hmsc_SmpTotalDrive() )
    {
        return (USB_ERROR);
    }

    /* Device Status */
    if( R_usb_hmsc_GetDevSts() != USB_HMSC_DEV_ATT )
    {
        return (USB_ERROR);
    }
    result = R_usb_hmsc_Write10( side, buff, secno, seccnt, trans_byte);
    return(result);
}   /* eof R_usb_hmsc_StrgWriteSector() */

/******************************************************************************
Function Name   : R_usb_hmsc_StrgUserCommand
Description     : USB Mass Storage Command
Arguments       : uint16_t side        : Side
                : uint16_t command     : Command
                : uint8_t  *buff       : Buffer address
Return value    : uint16_t             : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgUserCommand( uint16_t side, uint16_t command
    , uint8_t *buff)
{
    uint16_t    result;

    /* Check drive number */
    if( side > usb_hmsc_SmpTotalDrive() )
    {
        return USB_ERROR;
    }

    if( (USB_MEDIADRIVE != USB_DRIVE) && (side == USB_MEDIADRIVE) )
    {   /* Memory device */
        return (USB_ERROR);
    }
    else
    {   /* USB device */
        /* Device Status */
        if( R_usb_hmsc_GetDevSts() != USB_HMSC_DEV_ATT )
        {
            return (USB_ERROR);
        }

        switch( command )
        {
        case USB_ATAPI_TEST_UNIT_READY:
            /* Test unit */
            result = R_usb_hmsc_TestUnit( side);
            break;
        case USB_ATAPI_REQUEST_SENSE:
            /*Request sense */
            result = R_usb_hmsc_RequestSense( side, buff);
            break;
        case USB_ATAPI_FORMAT_UNIT:
            return (USB_ERROR);
            break;
        case USB_ATAPI_INQUIRY:
            /* Inquiry */
            result = R_usb_hmsc_Inquiry( side, buff);
            break;
        case USB_ATAPI_MODE_SELECT6:
            /* Mode select6 */
            result = R_usb_hmsc_ModeSelect6( side, buff);
            break;
        case USB_ATAPI_MODE_SENSE6:
            return (USB_ERROR);
            break;
        case USB_ATAPI_START_STOP_UNIT:
            return (USB_ERROR);
            break;
        case USB_ATAPI_PREVENT_ALLOW:
            /* Prevent allow */
            result = R_usb_hmsc_PreventAllow( side, buff);
            break;
        case USB_ATAPI_READ_FORMAT_CAPACITY:
            /* Read format capacity */
            result = R_usb_hmsc_ReadFormatCapacity( side, buff);
            break;
        case USB_ATAPI_READ_CAPACITY:
            /* Read capacity */
            result = R_usb_hmsc_ReadCapacity( side, buff);
            break;
        case USB_ATAPI_READ10:
        case USB_ATAPI_WRITE10:
            return (USB_ERROR);
            break;
        case USB_ATAPI_SEEK:
        case USB_ATAPI_WRITE_AND_VERIFY:
        case USB_ATAPI_VERIFY10:
            return (USB_ERROR);
            break;
        case USB_ATAPI_MODE_SELECT10:
            return (USB_ERROR);
            break;
        case USB_ATAPI_MODE_SENSE10:
            /* Mode sense10 */
            result = R_usb_hmsc_ModeSense10( side, buff);
            break;
        default:
            return (USB_ERROR);
            break;
        }
        if( result == USB_HMSC_OK )
        {
            return (USB_E_OK);
        }
        else
        {
            return (USB_ERROR);
        }
    }
}   /* eof R_usb_hmsc_StrgUserCommand() */


/******************************************************************************
End  Of File
******************************************************************************/
