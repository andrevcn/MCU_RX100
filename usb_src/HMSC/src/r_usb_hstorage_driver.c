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
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
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
#define     USB_HMSC_CLSDATASIZE        512

/******************************************************************************
External variables and functions
******************************************************************************/
extern usb_er_t R_usb_hstd_TransferStart(usb_utr_t *ptr);
extern void     usb_cstd_dummy_function(uint16_t data1, uint16_t data2);
extern void     usb_cpu_delay_xms(uint16_t time);

extern uint32_t     usb_ghmsc_TransSize;
extern uint16_t     usb_ghmsc_SmpAplProcess;

/******************************************************************************
Private global variables and functions
******************************************************************************/
uint8_t         usb_ghmsc_ClassData[USB_HMSC_CLSDATASIZE];
uint16_t        usb_ghmsc_DriveChk[USB_MAXDRIVE + 1][5];
uint8_t         usb_shmsc_DeviceReady[USB_MAXUNITNUM];
usb_hcdrequest_t    usb_shmsc_ClassRequest;
usb_utr_t       usb_shmsc_ClassControl;

/* Yes/No, Unit Number, Partition Number, device address, EPtbl offset */
uint16_t        usb_ghmsc_StrgCount;
uint16_t        usb_ghmsc_MaxDrive;
uint32_t        usb_ghmsc_MaxLUN;

uint16_t        usb_shmsc_StrgProcess                   = USB_NULL;
uint16_t        usb_shmsc_StrgDriveSearchSeq            = USB_SEQ_0;
uint16_t        usb_shmsc_StrgDriveSearchErrCount       = USB_SEQ_0;
uint16_t        usb_shmsc_StrgDriveSearchCount          = USB_SEQ_0;

/* Read Sector */
uint16_t        usb_shmsc_DevReadSectorSizeSeq          = USB_SEQ_0;
uint16_t        usb_shmsc_DevReadSectorSizeErrCount     = USB_SEQ_0;
uint16_t        usb_shmsc_DevReadPartitionSeq           = USB_SEQ_0;

/* Drive Open */
uint16_t        usb_shmsc_StrgDriveOpenSeq              = USB_SEQ_0;
uint16_t        usb_shmsc_StrgDriveOpenCount            = USB_SEQ_0;
uint16_t        usb_shmsc_StrgDriveOpenParCount         = USB_SEQ_0;

/* Partition */
uint8_t         usb_shmsc_PartitionInfo[USB_BOOTPARTNUM];
uint32_t        usb_shmsc_PartitionLba[USB_BOOTPARTNUM + 1u];

uint16_t        usb_ghmsc_RootDevaddr;
uint16_t        usb_ghmsc_DriveOpen = USB_ERROR;

uint16_t        usb_shmsc_NewDrive;
uint16_t        usb_shmsc_LoopCont = USB_SEQ_0;
uint16_t        usb_shmsc_Unit;
uint16_t        usb_ghmsc_PartTransSize;

uint16_t        usb_hmsc_GetStringDesc( uint16_t addr, uint16_t string, usb_cb_t complete);
uint16_t        usb_hmsc_CmdSubmit( usb_cb_t complete);

/******************************************************************************
Renesas Abstracted Peripheral Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_StrgDriveTask
Description     : Storage drive task
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hmsc_StrgDriveTask(void)
{
    usb_tskinfo_t   *mess;
    usb_er_t        err;                    /* Error code */
    
    /* receive message */
    err = R_USB_RCV_MSG( USB_HSTRG_MBX, (usb_msg_t**)&mess );
    if( err != USB_E_OK )
    {
        return;
    }

    switch( mess->msginfo )
    {
    case USB_MSG_HMSC_CLS_INIT:
        break;
    case USB_MSG_HMSC_STRG_DRIVE_SEARCH:
        /* Drive search */
        usb_hmsc_StrgDriveSearchAct((usb_tskinfo_t *)mess);
        break;
    case USB_MSG_HMSC_DEV_READ_SECTOR_SIZE:
        /* Read sector */
        usb_hmsc_SmpDevReadSectorSizeAct((usb_tskinfo_t *)mess);
        break;
    case USB_MSG_HMSC_DEV_READ_PARTITION:
        /* Read partition */
        usb_hmsc_SmpDevReadPartitionAct((usb_tskinfo_t *)mess);
        break;
    case USB_MSG_HMSC_STRG_DRIVE_OPEN:
        /* Drive open */
        usb_hmsc_SmpStrgDriveOpenAct((usb_tskinfo_t *)mess);
        break;

    default:
        break;
    }
    err = R_USB_REL_BLK(mess->flag);
    if( err != USB_E_OK )
    {
    }
}   /* eof usb_hmsc_StrgDriveTask() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDevNextDriveSearch
Description     : Next drive search
Arguments       : none
Return value    : uint16_t             : 
******************************************************************************/
uint16_t usb_hmsc_SmpDevNextDriveSearch(void)
{
    uint16_t i;

    for( i = 0; i < USB_MAXDRIVE; i++ )
    {
        if( usb_ghmsc_DriveChk[i][0] == USB_NO )
        {
            return i;
        }
    }
    return (uint16_t)0;
}   /* eof usb_hmsc_SmpDevNextDriveSearch() */


/******************************************************************************
Function Name   : usb_hmsc_StrgDriveSearchAct
Description     : Storage drive search
Arguments       : usb_tskinfo_t *mess   : Message
Return value    : none
******************************************************************************/
void usb_hmsc_StrgDriveSearchAct(usb_tskinfo_t *mess)
{
    uint32_t    j, result;
    uint16_t    resultk;
    uint16_t    offset, new_count, addr;
    usb_er_t err;

    switch( usb_shmsc_StrgDriveSearchSeq )
    {
    case USB_SEQ_0:
        /* Unit number set */
        addr = mess->keyword;
        usb_ghmsc_RootDevaddr = addr;

        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        err = R_usb_hmsc_GetMaxUnit( addr, (usb_cb_t)usb_hmsc_StrgCheckResult);
        if( err == USB_E_QOVR )
        {
            /* Resend message */
            err = R_USB_GET_SND(USB_HSTRG_MBX, mess->msginfo, &usb_cstd_dummy_function, (usb_strct_t)mess->keyword);
        }
        else
        {
            usb_shmsc_StrgDriveSearchSeq++;
        }
        break;
    case USB_SEQ_1:
        addr = usb_ghmsc_RootDevaddr;
        /* Get MAX_LUN */
        usb_ghmsc_MaxLUN =  usb_hmsc_GetMaxUnitCheck( mess->keyword);
        if( usb_ghmsc_MaxLUN == USB_ERROR )
        {
            usb_ghmsc_MaxLUN = (uint16_t)0;
        }
        else if( usb_ghmsc_MaxLUN > (uint32_t)USB_MAXUNITNUM )
        {
            usb_ghmsc_MaxLUN = USB_MAXUNITNUM - 1u;
        }
        else
        {
        }

        /* Set pipe information */
        offset  = (uint16_t)(2u * USB_EPL*usb_ghmsc_StrgCount);
        usb_ghmsc_TmpEpTbl[offset + 2u] |= USB_ADDR2DEVSEL(addr);
        usb_ghmsc_TmpEpTbl[(offset + 2u) + USB_EPL] |= USB_ADDR2DEVSEL(addr);

        /* Check connection */
        usb_cpu_delay_xms(100);
        /* Drive yes */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][0] = USB_YES;
        /* Device address */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][3] = addr;
        /* Device number */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][4] = usb_ghmsc_StrgCount;

        usb_shmsc_StrgDriveSearchSeq++;
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);
        break;
    case USB_SEQ_2:
        addr = usb_ghmsc_RootDevaddr;
        /* Unit Number */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][1] = (uint16_t)usb_shmsc_StrgDriveSearchCount;
        /* Inquiry */
        resultk = R_usb_hmsc_Inquiry( USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data);
        usb_shmsc_DeviceReady[usb_shmsc_StrgDriveSearchCount] = USB_PDT_UNKNOWN;
        
        usb_shmsc_StrgDriveSearchSeq++;
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        break;
    case USB_SEQ_3:
        addr = usb_ghmsc_RootDevaddr;
        resultk = mess->keyword;
        if( resultk == USB_HMSC_OK )
        {
            usb_shmsc_DeviceReady[usb_shmsc_StrgDriveSearchCount] = usb_ghmsc_Data[0];
            usb_shmsc_StrgDriveSearchErrCount = USB_SEQ_0;
            usb_shmsc_StrgDriveSearchCount++;
            usb_shmsc_StrgDriveSearchSeq = USB_SEQ_2;

            if( usb_shmsc_StrgDriveSearchCount > usb_ghmsc_MaxLUN )
            {
                usb_shmsc_StrgDriveSearchCount = USB_SEQ_0;
                usb_shmsc_StrgDriveSearchSeq = USB_SEQ_4;
            }
        }
        else if( resultk == USB_HMSC_CSW_ERR )
        {
            /* Inquiry error */
            usb_shmsc_StrgDriveSearchErrCount++;
            usb_shmsc_StrgDriveSearchSeq = USB_SEQ_2;
            if( usb_shmsc_StrgDriveSearchErrCount >= 3 )
            {
                usb_shmsc_StrgDriveSearchErrCount = USB_SEQ_0;
                usb_shmsc_StrgDriveSearchCount++;
                if( usb_shmsc_StrgDriveSearchCount > usb_ghmsc_MaxLUN )
                {
                    usb_shmsc_StrgDriveSearchCount = USB_SEQ_0;
                    usb_shmsc_StrgDriveSearchSeq = USB_SEQ_4;
                }
            }
        }
        else
        {
            usb_shmsc_StrgDriveSearchErrCount = USB_SEQ_0;
            usb_shmsc_StrgDriveSearchCount++;
            usb_shmsc_StrgDriveSearchSeq = USB_SEQ_2;

            if( usb_shmsc_StrgDriveSearchCount > usb_ghmsc_MaxLUN )
            {
                usb_shmsc_StrgDriveSearchCount = USB_SEQ_0;
                usb_shmsc_StrgDriveSearchSeq = USB_SEQ_4;
            }
        }
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);
        break;

    case USB_SEQ_4:
        /* Read Format Capacity */
        R_usb_hmsc_ReadFormatCapacity( USB_MAXDRIVE
            , (uint8_t*)&usb_ghmsc_Data);
        usb_shmsc_StrgDriveSearchSeq++;
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        break;

    case USB_SEQ_5:
        /* Read Capacity */
      R_usb_hmsc_ReadCapacity( USB_MAXDRIVE, (uint8_t*)usb_ghmsc_Data);
//        R_usb_hmsc_ReadCapacity( USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data); /* TEMP - ALISON */
        usb_shmsc_StrgDriveSearchSeq++;
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        break;

    case USB_SEQ_6:
        resultk = mess->keyword;
        if( resultk != USB_HMSC_OK )
        {
            /* TestUnitReady */
            R_usb_hmsc_TestUnit( USB_MAXDRIVE);
            usb_shmsc_StrgDriveSearchSeq++;
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        }
        else
        {
            /* Pass TestUnitReady  */
            usb_shmsc_StrgDriveSearchSeq= USB_SEQ_8;
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
            usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);
        };
        break;

    case USB_SEQ_7:
        resultk = mess->keyword;
        if( resultk != USB_HMSC_OK )
        {
            /* TestUnitReady */
            R_usb_hmsc_TestUnit( USB_MAXDRIVE);
//            usb_shmsc_StrgDriveSearchSeq == USB_SEQ_7; /* TEMP - ALISON */
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        }
        else
        {
            /* Read Capacity */
          R_usb_hmsc_ReadCapacity( USB_MAXDRIVE, (uint8_t*)usb_ghmsc_Data);
//            R_usb_hmsc_ReadCapacity( USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data); /* TEMP - ALISON */
            usb_shmsc_StrgDriveSearchSeq++;
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        };
        break;

    case USB_SEQ_8:
        addr = usb_ghmsc_RootDevaddr;
        /* Read & set partition information */
        result = 0;
        for( j = (uint32_t)0; j <= usb_ghmsc_MaxLUN; j++ )
        {
            /* Set sector size & block address */
            switch( usb_shmsc_DeviceReady[j] )
            {
            case USB_PDT_DIRECT:
                /* Unit Number */
                usb_ghmsc_DriveChk[USB_MAXDRIVE][1] = (uint16_t)j;
                offset = usb_hmsc_SmpDevReadPartition( (uint16_t)j, (uint32_t)512);
                result++;                                   
                break;
            case USB_PDT_SEQUENTIAL:
                /* Not support: Sequential device */
                break;
            case USB_PDT_WRITEONCE:
                /* Not support: Write once device */
                break;
            case USB_PDT_CDROM:
                /* Not support: CD-ROM device */
                break;
            case USB_PDT_OPTICAL:
                /* Not support: Optivasl device */
                break;
            case USB_PDT_UNKNOWN:
                /* Not support: Unknown device */
                break;
            default:
                /* Not support: Not direct access device */
                break;
            }
        }
        usb_shmsc_StrgDriveSearchSeq++;
        if( result == 0 )
        {
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
            usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);

        }   
        break;

    case USB_SEQ_9:
        addr = usb_ghmsc_RootDevaddr;
        new_count = USB_ERROR;
        for( j = (uint32_t)0; j <= usb_ghmsc_MaxLUN; j++ )
        {
            /* Set sector size & block address */
            if( usb_shmsc_DeviceReady[j] == USB_PDT_DIRECT )
            {
                new_count = mess->keyword;
            }
        }
        if( new_count == USB_E_OK )
        {
            usb_ghmsc_StrgCount++;
        }
        usb_hmsc_StrgCommandResult((usb_tskinfo_t *)mess);

        usb_shmsc_StrgDriveSearchSeq = USB_SEQ_0;
        usb_shmsc_StrgProcess = USB_NULL;
        break;
    default:
        usb_shmsc_StrgProcess = USB_NULL;
        usb_shmsc_StrgDriveSearchSeq = USB_SEQ_0;
        usb_shmsc_StrgDriveSearchCount = USB_SEQ_0;
        usb_shmsc_StrgDriveSearchErrCount = USB_SEQ_0;
        break;
    }
}   /* eof usb_hmsc_StrgDriveSearchAct() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDevReadSectorSize
Description     : Searches drive SndMsg
Arguments       : uint16_t size        : Size
Return value    : uint32_t             : Sector size
******************************************************************************/
uint32_t usb_hmsc_SmpDevReadSectorSize(uint16_t size)
{
    usb_shmsc_DevReadSectorSizeSeq = USB_SEQ_0;

    return R_USB_GET_SND(USB_HSTRG_MBX, USB_MSG_HMSC_DEV_READ_SECTOR_SIZE, &usb_cstd_dummy_function, (usb_strct_t)size);
}   /* eof usb_hmsc_SmpDevReadSectorSize() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDevReadSectorSizeAct
Description     : Drive read sector size
Arguments       : usb_tskinfo_t *mess  : Message
Return value    : none
******************************************************************************/
void usb_hmsc_SmpDevReadSectorSizeAct(usb_tskinfo_t *mess)
{
    uint32_t    trans_byte = (uint32_t)0;
    uint16_t    side;

    side = mess->keyword;
    switch( usb_shmsc_DevReadSectorSizeSeq )
    {
    case USB_SEQ_0:
        /* Read capacity */
        R_usb_hmsc_ReadCapacity( side, (uint8_t*)&usb_ghmsc_Data);

        usb_shmsc_StrgProcess = USB_MSG_HMSC_DEV_READ_SECTOR_SIZE;
        usb_shmsc_DevReadSectorSizeSeq++;
        break;
    case USB_SEQ_1:
        usb_shmsc_DevReadSectorSizeSeq = USB_SEQ_0;
        if( mess->keyword == USB_HMSC_OK )
        {
            trans_byte  =  (uint32_t)usb_ghmsc_Data[7];
            trans_byte  |= ((uint32_t)(usb_ghmsc_Data[6]) << 8);
            trans_byte  |= ((uint32_t)(usb_ghmsc_Data[5]) << 16);
            trans_byte  |= ((uint32_t)(usb_ghmsc_Data[4]) << 24);
            if( trans_byte != (uint32_t)0x0200 )
            {
                /* Sector size error */
                trans_byte = (uint32_t)0;
            }
            else
            {
                /* FSI Sector initialize */
                usb_hmsc_SmpFsiSectorInitialized(side, (uint32_t)0, (uint16_t)trans_byte);
            }
            usb_shmsc_DevReadSectorSizeErrCount = USB_SEQ_0;
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
        }
        else
        {
            /* Sector size error */
            trans_byte =  (uint32_t)0x200;
            /* FSI sector initialize */
            usb_hmsc_SmpFsiSectorInitialized(side, (uint32_t)0, (uint16_t)trans_byte);
            usb_shmsc_StrgProcess = USB_MSG_HMSC_DEV_READ_SECTOR_SIZE;
            usb_shmsc_DevReadSectorSizeErrCount++;
            if( usb_shmsc_DevReadSectorSizeErrCount >= 3 )
            {
                usb_shmsc_DevReadSectorSizeErrCount = USB_SEQ_0;
                usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
            }
        }
        usb_ghmsc_TransSize = trans_byte;
        usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);
        break;
    default:
        usb_shmsc_StrgProcess = USB_NULL;
        usb_shmsc_DevReadSectorSizeSeq = USB_SEQ_0;
        usb_shmsc_DevReadSectorSizeErrCount = USB_SEQ_0;
        break;
    }
}   /* eof usb_hmsc_SmpDevReadSectorSizeAct() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDevReadPartition
Description     : Searches drive SndMsg
Arguments       : uint16_t unit        : Unit
                : uint32_t trans_byte  : Trans byte
Return value    : uint16_t
******************************************************************************/
uint16_t usb_hmsc_SmpDevReadPartition(uint16_t unit, uint32_t trans_byte)
{
    usb_shmsc_DevReadPartitionSeq = USB_SEQ_0;
    usb_ghmsc_TransSize = trans_byte;
    return R_USB_GET_SND(USB_HSTRG_MBX, USB_MSG_HMSC_DEV_READ_PARTITION, &usb_cstd_dummy_function, (usb_strct_t)unit);
}   /* eof usb_hmsc_SmpDevReadPartition() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDevReadPartitionAct
Description     : Drive read partition
Arguments       : usb_tskinfo_t *mess : Message
Return value    : uint16_t            : [USB_E_OK/USB_ERROR]
******************************************************************************/
uint16_t usb_hmsc_SmpDevReadPartitionAct(usb_tskinfo_t *mess)
{
    uint32_t    i;
    uint16_t    result;
    uint16_t    new_drive, parcount = 0;
    uint16_t    unit;
    uint8_t     partition_info[USB_BOOTPARTNUM];
    uint32_t    partition_lba[USB_BOOTPARTNUM + 1u];
    uint32_t    trans_byte;

    trans_byte = usb_ghmsc_TransSize;
    new_drive = usb_shmsc_NewDrive;

    switch( usb_shmsc_DevReadPartitionSeq )
    {
    case USB_SEQ_0:
        if( usb_shmsc_LoopCont == USB_SEQ_0 )
        {
            usb_shmsc_Unit = mess->keyword;
            usb_ghmsc_PartTransSize = trans_byte;
        }
        else
        {
            trans_byte = usb_ghmsc_PartTransSize;
        }
        partition_lba[0]                = (uint32_t)0;
        partition_lba[USB_BOOTPARTNUM]  = (uint32_t)0;
    
        /* set drive number */
        new_drive = usb_hmsc_SmpDevNextDriveSearch();
        usb_shmsc_NewDrive = new_drive;

        /* Read10 */
        result = R_usb_hmsc_Read10(
                    USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data, partition_lba[0], (uint16_t)1, trans_byte);
        usb_shmsc_DevReadPartitionSeq++;
        usb_shmsc_StrgProcess = USB_MSG_HMSC_DEV_READ_PARTITION;
        break;
    case USB_SEQ_1:
        unit = usb_shmsc_Unit;
        usb_shmsc_DevReadPartitionSeq = USB_SEQ_0;

        if( mess->keyword == USB_HMSC_OK )
        {
            /* Check boot record */
            result = usb_hmsc_SmpDevCheckBootRecord((uint8_t*)&usb_ghmsc_Data,
                        (uint32_t*)&partition_lba, (uint8_t*)&partition_info, (uint16_t)0 );
            /* Display partition information */
            if( result != (uint16_t)USB_BOOT_ERROR )
            {
                result = USB_E_OK;

                for( i = (uint32_t)0; i < (uint32_t)USB_BOOTPARTNUM; i++ )
                {
                    switch( partition_info[i] )
                    {
                    case USB_PT_FAT12:
                    case USB_PT_FAT16:
                    case USB_PT_FAT32:
                        /* Drive yes */
                        usb_ghmsc_DriveChk[new_drive][0] = USB_YES;
                        /* Unit Number */
                        usb_ghmsc_DriveChk[new_drive][1] = unit;
                        /* Partition Number */
                        usb_ghmsc_DriveChk[new_drive][2] = parcount;
                        /* Device address */
                        usb_ghmsc_DriveChk[new_drive][3] = usb_ghmsc_DriveChk[USB_MAXDRIVE][3];
                        /* Endpoint table offset */
                        usb_ghmsc_DriveChk[new_drive][4] = usb_ghmsc_DriveChk[USB_MAXDRIVE][4];
                        usb_ghmsc_MaxDrive++;
                        if( usb_ghmsc_MaxDrive == USB_MAXDRIVE )
                        {
                            i = (uint32_t)USB_BOOTPARTNUM;
                        }
                        else
                        {
                            /* Next drive search */
                            new_drive = usb_hmsc_SmpDevNextDriveSearch();
                            usb_shmsc_NewDrive = new_drive;
                        }
                        parcount++;
                        break;
                    case USB_PT_EPRT:
                        if( partition_lba[USB_BOOTPARTNUM] == (uint32_t)0 )
                        {
                            /* Master Boot */
                            partition_lba[USB_BOOTPARTNUM] = partition_lba[i];
                            /* Next EMBR sector */
                            partition_lba[0] = partition_lba[i];
                        }
                        else
                        {
                            /* Next EBMR sector */
                            partition_lba[0] = partition_lba[i] + partition_lba[USB_BOOTPARTNUM];
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                /* Drive read error */
                result = USB_ERROR;
            }
        }
        else
        {
            /* Drive read error */
            usb_shmsc_LoopCont++;
            result = (uint16_t)USB_EMBR_ADDR;
            if( usb_shmsc_LoopCont == (uint32_t)10 )
            {
                result = USB_ERROR;
                usb_shmsc_LoopCont = USB_SEQ_0;
            }
        }

        if( result != (uint16_t)USB_EMBR_ADDR )
        {
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
            mess->keyword = result;
            usb_shmsc_LoopCont = USB_SEQ_0;
        }
        usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);
        break;
    default:
        usb_shmsc_DevReadPartitionSeq = USB_SEQ_0;
        mess->keyword = USB_ERROR;
        usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);
        break;
    }
    return 0;
}   /* eof usb_hmsc_SmpDevReadPartitionAct() */


/******************************************************************************
Function Name   : usb_hmsc_SmpStrgDriveOpenAct
Description     : Mounts the drive
Arguments       : usb_tskinfo_t *mess  : Message
Return value    : none
******************************************************************************/
void usb_hmsc_SmpStrgDriveOpenAct( usb_tskinfo_t *mess )
{
    uint16_t    result, unit, partition;
    uint32_t    trans_byte;
    uint16_t    side;
    uint16_t    endflg, retval;

    endflg = USB_NO;
    retval = USB_E_OK;
    switch( usb_shmsc_StrgDriveOpenSeq )
    {
    case USB_SEQ_0:
        side = mess->keyword;
        /* Check drive numberk */
        if( side > usb_hmsc_SmpTotalDrive() )
        {
            endflg = USB_YES;
            retval = USB_ERROR;
        }
        /* Device Status */
        if( R_usb_hmsc_GetDevSts() != USB_HMSC_DEV_ATT )
        {
            endflg = USB_YES;
            retval = USB_ERROR;
        }
        unit        = usb_hmsc_SmpDrive2Unit(side);
        partition   = usb_hmsc_SmpDrive2Part(side);
        if( (unit == USB_ERROR) || (partition == USB_ERROR) )
        {
            endflg = USB_YES;
            retval = USB_ERROR;
        }
        /* Set secor_size & block_address */
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
        usb_shmsc_StrgDriveOpenSeq++;
        usb_hmsc_SmpDevReadSectorSize(side);
        
        usb_shmsc_PartitionLba[0]               = (uint32_t)0;
        usb_shmsc_PartitionLba[USB_BOOTPARTNUM] = (uint32_t)0;
        break;
    case USB_SEQ_1:
        side = mess->keyword;
        trans_byte = usb_ghmsc_TransSize;

        /* Partition sector */
        usb_hmsc_SmpFsiSectorInitialized(side, usb_shmsc_PartitionLba[0], (uint16_t)trans_byte);
        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
        usb_shmsc_StrgDriveOpenSeq++;

        /* Read10 */
        R_usb_hmsc_Read10( side, (uint8_t*)&usb_ghmsc_Data, usb_shmsc_PartitionLba[0], (uint16_t)1, trans_byte);
        break;
    case USB_SEQ_2:
        side = mess->keyword;

        /* Check boot record */
        result = usb_hmsc_SmpDevCheckBootRecord(
                    (uint8_t*)&usb_ghmsc_Data, (uint32_t*)&usb_shmsc_PartitionLba,
                    (uint8_t*)&usb_shmsc_PartitionInfo, (uint16_t)1);

        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
        usb_shmsc_StrgDriveOpenSeq++;
        usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);
        break;
    case USB_SEQ_3:
        side = mess->keyword;
        result = mess->keyword;
        partition   = usb_hmsc_SmpDrive2Part(side);
        trans_byte = usb_ghmsc_TransSize;
        
        /* Drive open */
        switch( usb_shmsc_PartitionInfo[usb_shmsc_StrgDriveOpenCount] )
        {
        case USB_PT_FAT12:
        case USB_PT_FAT16:
        case USB_PT_FAT32:
            if( partition == usb_shmsc_StrgDriveOpenParCount )
            {
                switch( result )
                {
                case USB_PBR_ADDR:
                    /* Offset sector read */
                    usb_shmsc_PartitionLba[0] = usb_hmsc_SmpFsiOffsetSectorRead(side);
                    /* Initial FFS */
                    result = usb_hmsc_SmpFsiFileSystemInitialized(
                                side, (uint8_t*)&usb_ghmsc_Data, usb_shmsc_PartitionLba[0]);
                    endflg = USB_YES;
                    retval = USB_E_OK;
                    break;
                case USB_MBR_ADDR:
                case USB_EMBR_ADDR:
                    if( usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount] == (uint16_t)0 )
                    {
                        /* Culc LBA from CHS */
                        endflg = USB_YES;
                        retval = USB_ERROR;
                    }
                    else
                    {
                        /* FSI offset sector read */
                        usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount] += usb_hmsc_SmpFsiOffsetSectorRead(side);

                        usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
                        usb_shmsc_StrgDriveOpenSeq++;
                        /* Read10 */
                        R_usb_hmsc_Read10( side, (uint8_t*)&usb_ghmsc_Data,
                            usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount],(uint16_t)1, trans_byte);
                    }
                    break;
                default:
                    endflg = USB_YES;
                    retval = USB_ERROR;
                    break;
                }
            }
            else
            {
                usb_shmsc_StrgDriveOpenParCount++;
                usb_shmsc_StrgDriveOpenCount++;
                if( usb_shmsc_StrgDriveOpenCount >= USB_BOOTPARTNUM )
                {
                    usb_shmsc_StrgDriveOpenCount = USB_SEQ_0;
//                    usb_shmsc_StrgDriveOpenSeq == USB_SEQ_1; /* TEMP - ALISON */
                }
                else
                {
//                    usb_shmsc_StrgDriveOpenSeq == USB_SEQ_3; /* TEMP - ALISON */
                }
                usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
                usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);
            }
            break;
        case USB_PT_EPRT:
            if( usb_shmsc_PartitionLba[USB_BOOTPARTNUM] == (uint16_t)0 )
            {
                /* Memory Master Boot */
                usb_shmsc_PartitionLba[USB_BOOTPARTNUM] = usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount];
                /* Next EMBR sector */
                usb_shmsc_PartitionLba[0] = usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount];
            }
            else
            {
                /* Next EBMR sector */
                usb_shmsc_PartitionLba[0] = usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount] +
                                            usb_shmsc_PartitionLba[USB_BOOTPARTNUM];
            }
            usb_shmsc_StrgDriveOpenCount++;
            if( usb_shmsc_StrgDriveOpenCount >= USB_BOOTPARTNUM )
            {
                usb_shmsc_StrgDriveOpenCount = USB_SEQ_0;
//                usb_shmsc_StrgDriveOpenSeq == USB_SEQ_1; /* TEMP - ALISON */
            }
            else
            { 
//                usb_shmsc_StrgDriveOpenSeq == USB_SEQ_3; /* TEMP - ALISON */
            }
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
            usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);
            break;
        default:
            usb_shmsc_StrgDriveOpenCount++;
            if( usb_shmsc_StrgDriveOpenCount >= USB_BOOTPARTNUM )
            {
                usb_shmsc_StrgDriveOpenCount = USB_SEQ_0;
//                usb_shmsc_StrgDriveOpenSeq == USB_SEQ_1; /* TEMP - ALISON */
            }
            else
            {
//                usb_shmsc_StrgDriveOpenSeq == USB_SEQ_3; /* TEMP - ALISON */
            }
            usb_shmsc_StrgProcess = USB_MSG_HMSC_STRG_DRIVE_OPEN;
            usb_hmsc_StrgSpecifiedPath((usb_tskinfo_t *)mess);
            break;
        }
        break;
    case USB_SEQ_4:
        side = mess->keyword;
        /* Initial FFS */
        result  = usb_hmsc_SmpFsiFileSystemInitialized(side,
                    (uint8_t*)&usb_ghmsc_Data, usb_shmsc_PartitionLba[usb_shmsc_StrgDriveOpenCount]);
        endflg = USB_YES;
        retval = USB_E_OK;
        break;
    default:
        usb_shmsc_StrgProcess = USB_NULL;
        usb_shmsc_StrgDriveOpenSeq = USB_SEQ_0;
        usb_shmsc_StrgDriveOpenCount = USB_SEQ_0;
        usb_shmsc_StrgDriveOpenParCount = USB_SEQ_0;
        usb_ghmsc_DriveOpen = USB_ERROR;
        break;
    }
    
    if( endflg == USB_YES )
    {
        usb_shmsc_StrgProcess = USB_NULL;
        usb_shmsc_StrgDriveOpenSeq = USB_SEQ_0;
        usb_shmsc_StrgDriveOpenCount = USB_SEQ_0;
        usb_shmsc_StrgDriveOpenParCount = USB_SEQ_0;
        usb_ghmsc_DriveOpen = retval;
        usb_hmsc_StrgCommandResult((usb_tskinfo_t *)mess);
    }
}   /* eof usb_hmsc_SmpStrgDriveOpenAct() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor1
Description     : Get String descriptor
Arguments       : uint16_t devaddr          : device address
                : uint16_t index            : descriptor index
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor1( uint16_t devaddr, uint16_t index, usb_cb_t complete)
{
    usb_hmsc_GetStringDesc( devaddr, (uint16_t)0, complete);

    return  USB_E_OK;
}   /* eof usb_hmsc_GetStringDescriptor1() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor2
Description     : Get String descriptor
Arguments       : uint16_t devaddr          : device address
                : uint16_t index            : descriptor index
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor2( uint16_t devaddr, uint16_t index, usb_cb_t complete)
{
    usb_hmsc_GetStringDesc( devaddr, index, complete);

    return  USB_E_OK;
}   /* eof usb_hmsc_GetStringDescriptor2() */

/******************************************************************************
Function Name   : usb_hmsc_StrgSpecifiedPath
Description     : Next Process Selector
Arguments       : usb_tskinfo_t *mess  : Message
Return value    : none
******************************************************************************/
void usb_hmsc_StrgSpecifiedPath(usb_tskinfo_t *mess)
{
    R_USB_GET_SND(USB_HSTRG_MBX, usb_shmsc_StrgProcess, &usb_cstd_dummy_function, (usb_strct_t)mess->keyword);
}   /* eof usb_hmsc_StrgSpecifiedPath() */

/******************************************************************************
Function Name   : usb_hmsc_StrgCheckResult
Description     : Hub class check result
Arguments       : usb_tskinfo_t *mess    : Message
Return value    : none
******************************************************************************/
void usb_hmsc_StrgCheckResult(usb_tskinfo_t *mess)
{
    R_USB_GET_SND(USB_HSTRG_MBX, usb_shmsc_StrgProcess, &usb_cstd_dummy_function, (usb_strct_t)mess->status);
}   /* eof usb_hmsc_StrgCheckResult() */

/******************************************************************************
Function Name   : usb_hmsc_StrgSpecifiedPath
Description     : Next Process Selector
Argument        : usb_tskinfo_t *mess
Return          : none
******************************************************************************/
void usb_hmsc_StrgCommandResult(usb_tskinfo_t *mess)
{
    R_USB_GET_SND(USB_HMSCSMP_MBX, usb_ghmsc_SmpAplProcess, &usb_cstd_dummy_function, (usb_strct_t)mess->keyword);
}   /* eof usb_hmsc_StrgCommandResult() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringDesc
Description     : Set GetDescriptor
Arguments       : uint16_t addr            : device address
                : uint16_t string          : descriptor index
                : usb_cb_t complete       : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDesc( uint16_t addr, uint16_t string
            , usb_cb_t complete)
{
    uint16_t        i;

    if( string == 0 )
    {
        usb_shmsc_ClassRequest.wIndex   = (uint16_t)0x0000;
        usb_shmsc_ClassRequest.wLength  = (uint16_t)0x0004;
    }
    else
    {
        /* Set LanguageID */
        usb_shmsc_ClassRequest.wIndex   = (uint16_t)(usb_ghmsc_ClassData[2]);
        usb_shmsc_ClassRequest.wIndex   |= (uint16_t)((uint16_t)(usb_ghmsc_ClassData[3]) << 8);
        usb_shmsc_ClassRequest.wLength  = (uint16_t)USB_HMSC_CLSDATASIZE;
    }
    usb_shmsc_ClassRequest.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_DEV_TO_HOST,USB_STANDARD,USB_DEVICE);
    usb_shmsc_ClassRequest.WORD.BYTE.bRequest       = USB_GET_DESCRIPTOR;
    usb_shmsc_ClassRequest.wValue   = (uint16_t)(USB_STRING_DESCRIPTOR + string);
    usb_shmsc_ClassRequest.Address  = addr;

    for( i = 0; i < usb_shmsc_ClassRequest.wLength; i++ )
    {
        usb_ghmsc_ClassData[i]  = (uint8_t)0xFF;
    }

    return usb_hmsc_CmdSubmit( complete);
}   /* eof usb_hmsc_GetStringDesc() */

/******************************************************************************
Function Name   : usb_hmsc_CmdSubmit
Description     : command submit
Arguments       : usb_cb_t complete         : callback info
Return value    : uint16_t                  : USB_E_OK
******************************************************************************/
uint16_t usb_hmsc_CmdSubmit( usb_cb_t complete)
{
    usb_shmsc_ClassControl.tranadr  = (void *)usb_ghmsc_ClassData;
    usb_shmsc_ClassControl.complete = complete;
    usb_shmsc_ClassControl.tranlen  = (usb_leng_t)usb_shmsc_ClassRequest.wLength;
    usb_shmsc_ClassControl.pipenum  = USB_PIPE0;
    usb_shmsc_ClassControl.setup    = (void*)&usb_shmsc_ClassRequest;

    R_usb_hstd_TransferStart(&usb_shmsc_ClassControl);
    
    return USB_E_OK;
}   /* eof usb_hmsc_CmdSubmit() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDevCheckBootRecord
Description     : Device check boot record
Arguments       : uint8_t  *data      : Data
                : uint32_t *par_lba   : Par LBA
                : uint8_t  *par_info   : Par info
                : uint16_t flag       : Flag
Return value    : uint16_t            : Error code [USB_E_OK/USB_ERROR]
******************************************************************************/
uint16_t usb_hmsc_SmpDevCheckBootRecord(uint8_t *data, uint32_t *par_lba, uint8_t *par_info, uint16_t flag)
{
    USB_MBR_t       *mbr_data;
    USB_PBR_t       *pbr_data;
    USB_FAT1216_t   *fat1216;
    USB_PTBL_t      *partition;
    uint16_t        fat_sector, i, embr_flag;
    uint32_t        total_sector32, dummy;

    mbr_data = (USB_MBR_t*)data;
    pbr_data = (USB_PBR_t*)data;

    /* BOOT Recorder ? */
    dummy   = (uint32_t)(pbr_data->Signature[0]);
    dummy   |= (uint32_t)((uint16_t)(pbr_data->Signature[1]) << 8);
    if( dummy != (uint32_t)USB_BOOTRECORD_SIG )
    {
        par_info[0] = USB_PT_NONE;
        return USB_BOOT_ERROR;
    }

    embr_flag = USB_PT_NONE;

    /* MBR check (Partition n) */
    for( i = 0; i < USB_BOOTPARTNUM; i++ )
    {
        partition   = (USB_PTBL_t*)&(mbr_data->PartitionTable[i * 16]);
        par_info[i] = USB_PT_NONE;
        par_lba[i]  = (uint32_t)(partition->StartSectorNum[0]);
        par_lba[i]  |= (uint32_t)(partition->StartSectorNum[1]) << 8;
        par_lba[i]  |= (uint32_t)(partition->StartSectorNum[2]) << 16;
        par_lba[i]  |= (uint32_t)(partition->StartSectorNum[3]) << 24;
        switch( partition->PartitionType )
        {
        case USB_PT_NONE:
            break;
        case USB_PT_EPRTA:
        case USB_PT_EPRTB:
            embr_flag   = USB_EMBR_ADDR;
            par_info[i] = USB_PT_EPRT;
            break;
        case USB_PT_FAT12A:
            if( embr_flag == USB_PT_NONE )
            {
                embr_flag = USB_MBR_ADDR;
            }
            par_info[i] = USB_PT_FAT12;
            break;
        case USB_PT_FAT16A:
        case USB_PT_FAT16B:
        case USB_PT_FAT16X:
            if( embr_flag == USB_PT_NONE )
            {
                embr_flag = USB_MBR_ADDR;
            }
            par_info[i] = USB_PT_FAT16;
            break;
        case USB_PT_FAT32A:
        case USB_PT_FAT32X:
            if( embr_flag == USB_PT_NONE )
            {
                embr_flag = USB_MBR_ADDR;
            }
            par_info[i] = USB_PT_FAT32;
            break;
        default:
            if( flag != 0 )
            {
            }
            break;
        }
    }

    switch( embr_flag )
    {
    case USB_MBR_ADDR:
    case USB_EMBR_ADDR:
        return embr_flag;
        break;
    default:
        break;
    }

    /* PBR check */
    fat1216     = (USB_FAT1216_t*)&(pbr_data->FATSigData);

    fat_sector      =  (uint16_t)(pbr_data->FATSector[0]);
    fat_sector      |= (uint16_t)((uint16_t)(pbr_data->FATSector[1]) << 8);
    total_sector32  =  (uint32_t)(pbr_data->TotalSector1[0]);
    total_sector32  |= ((uint32_t)(pbr_data->TotalSector1[1]) << 8);
    total_sector32  |= ((uint32_t)(pbr_data->TotalSector1[2]) << 16);
    total_sector32  |= ((uint32_t)(pbr_data->TotalSector1[3]) << 24);

    if( ((pbr_data->JMPcode == USB_JMPCODE1) &&
        (pbr_data->NOPcode == USB_NOPCODE)) ||
        (pbr_data->JMPcode == USB_JMPCODE2) )
    {
        if( fat_sector == 0 )
        {
            if( total_sector32 != (uint32_t)0 )
            {
                par_info[0] = USB_PT_FAT32; /* FAT32 spec */
            }
        }
        else
        {
            if( (fat1216->FileSystemType[3] == 0x31) && (fat1216->FileSystemType[4] == 0x32) )
            {
                par_info[0] = USB_PT_FAT12; /* FAT12 spec */
            }
            else if( (fat1216->FileSystemType[3] == 0x31) && (fat1216->FileSystemType[4] == 0x36) )
            {
                par_info[0] = USB_PT_FAT16; /* FAT16 spec */
            }
            else
            {
            }
        }
    }

    if( par_info[0] == USB_PT_NONE )
    {
        return USB_BOOT_ERROR;
    }
    else
    {
        return USB_PBR_ADDR;
    }
}   /* eof usb_hmsc_SmpDevCheckBootRecord() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDevCheckBootRecord
Description     : Device check boot record
Arguments       : uint16_t addr       : Address
Return value    : none
******************************************************************************/
void usb_hmsc_SmpFsiDriveClear( uint16_t addr)
{
    uint16_t        i, offset, msgnum;
    uint16_t        find = USB_NO;

    for( i = 0; i < USB_MAXDRIVE; i++ )
    {
        if( usb_ghmsc_DriveChk[i][3] == addr )
        {
            msgnum = usb_hmsc_SmpDrive2Msgnum(i);
            offset = (uint16_t)(2u * USB_EPL * msgnum);

            usb_ghmsc_TmpEpTbl[offset + 1]  = USB_NULL|USB_BFREOFF|USB_DBLBOFF|USB_SHTNAKOFF;
            usb_ghmsc_TmpEpTbl[offset + 2]  = USB_NULL;
            usb_ghmsc_TmpEpTbl[offset + 3]  = USB_NULL;
            offset += USB_EPL;

            usb_ghmsc_TmpEpTbl[offset + 1]  = USB_NULL|USB_BFREOFF|USB_DBLBOFF|USB_SHTNAKOFF;
            usb_ghmsc_TmpEpTbl[offset + 2]  = USB_NULL;
            usb_ghmsc_TmpEpTbl[offset + 3]  = USB_NULL;

            usb_ghmsc_DriveChk[i][0] = USB_NO;  /* Yes/No */
            usb_ghmsc_DriveChk[i][1] = 0;       /* Unit Number */
            usb_ghmsc_DriveChk[i][2] = 0;       /* Partition Number */
            usb_ghmsc_DriveChk[i][3] = 0;       /* Device address */
            usb_ghmsc_DriveChk[i][4] = 0;       /* Device number */
            usb_ghmsc_MaxDrive--;
            find = USB_YES;
        }
    }

    if( find == USB_NO )
    {
        if( usb_ghmsc_DriveChk[USB_MAXDRIVE][3] == addr )
        {
            msgnum = usb_hmsc_SmpDrive2Msgnum(USB_MAXDRIVE);
            offset = (uint16_t)(2u * USB_EPL * msgnum);

            usb_ghmsc_TmpEpTbl[offset + 1]  = USB_NULL|USB_BFREOFF|USB_DBLBOFF|USB_SHTNAKOFF;
            usb_ghmsc_TmpEpTbl[offset + 2]  = USB_NULL;
            usb_ghmsc_TmpEpTbl[offset + 3]  = USB_NULL;
            offset += USB_EPL;

            usb_ghmsc_TmpEpTbl[offset + 1]  = USB_NULL|USB_BFREOFF|USB_DBLBOFF|USB_SHTNAKOFF;
            usb_ghmsc_TmpEpTbl[offset + 2]  = USB_NULL;
            usb_ghmsc_TmpEpTbl[offset + 3]  = USB_NULL;

            usb_ghmsc_DriveChk[USB_MAXDRIVE][0] = USB_NO;  /* Yes/No */
            usb_ghmsc_DriveChk[USB_MAXDRIVE][1] = 0;       /* Unit Number */
            usb_ghmsc_DriveChk[USB_MAXDRIVE][2] = 0;       /* Partition Number */
            usb_ghmsc_DriveChk[USB_MAXDRIVE][3] = 0;       /* Device address */
            usb_ghmsc_DriveChk[USB_MAXDRIVE][4] = 0;       /* Device number */
        }
    }

    if( usb_ghmsc_StrgCount != 0 )
    {
        usb_ghmsc_StrgCount--;
    }

    usb_ghmsc_OutPipe[msgnum][0]    = USB_NOPORT;   /* Pipe initial */
    usb_ghmsc_OutPipe[msgnum][1]    = 0;            /* Toggle clear */
    usb_ghmsc_InPipe[msgnum][0]     = USB_NOPORT;
    usb_ghmsc_InPipe[msgnum][1]     = 0;

    if( usb_ghmsc_StrgCount == 0 )
    {
        R_usb_hmsc_TaskClose();
    }
}   /* eof usb_hmsc_SmpFsiDriveClear() */


/******************************************************************************
Function Name   : usb_hmsc_SmpTotalDrive
Description     : Total drive information
Arguments       : none
Return value    : uint16_t             : Max drive
******************************************************************************/
uint16_t usb_hmsc_SmpTotalDrive(void)
{
    return usb_ghmsc_MaxDrive;
}   /* eof usb_hmsc_SmpTotalDrive() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Unit
Description     : Total drive information
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Unit number
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Unit(uint16_t side)
{
    if( usb_ghmsc_DriveChk[side][0] != USB_YES )
    {
        return USB_ERROR;
    }
    return (usb_ghmsc_DriveChk[side][1]);           /* Unit Number */
}   /* eof usb_hmsc_SmpDrive2Unit() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Part
Description     : Retrieves partition number
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Partition number
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Part(uint16_t side)
{
    if( usb_ghmsc_DriveChk[side][0] != USB_YES )
    {
        return USB_ERROR;
    }
    return (usb_ghmsc_DriveChk[side][2]);   /* Parttition Number */
}   /* eof usb_hmsc_SmpDrive2Part() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Addr
Description     : Retrieves device address
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Device address
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Addr(uint16_t side)
{
    return (usb_ghmsc_DriveChk[side][3]);   /* Device Address */
}   /* eof usb_hmsc_SmpDrive2Addr() */

/******************************************************************************
Function Name   : usb_hmsc_SmpCheckAddr
Description     : Retrieves device address
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Device address
******************************************************************************/
uint16_t usb_hmsc_SmpCheckAddr(uint16_t side)
{
    if( usb_ghmsc_DriveChk[side][0] != USB_YES )
    {
        return USB_ERROR;
    }
    return (usb_ghmsc_DriveChk[side][3]);   /* Device Address */
}   /* eof usb_hmsc_SmpCheckAddr() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Msgnum
Description     : Checks drive number
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Drive address
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Msgnum(uint16_t side)
{
    return (usb_ghmsc_DriveChk[side][4]);
}   /* eof usb_hmsc_SmpDrive2Msgnum() */

/******************************************************************************
Function Name   : usb_hmsc_StdReqCheck
Description     : Sample Standard Request Check
Arguments       : uint16_t errcheck        : error
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hmsc_StdReqCheck(uint16_t errcheck)
{
    if( errcheck == USB_DATA_TMO )
    {
        return  USB_ERROR;
    }
    else if( errcheck == USB_DATA_STALL )
    {
        return  USB_ERROR;
    }
    else if( errcheck != USB_CTRL_END )
    {
        return  USB_ERROR;
    }
    else
    {
    }
    return  USB_E_OK;
}   /* eof usb_hmsc_StdReqCheck() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor1Check
Description     : Get String descriptor Check
Arguments       : uint16_t errcheck         : errcheck
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor1Check( uint16_t errcheck)
{
    if( errcheck == (usb_er_t)USB_DATA_STALL )
    {
        return USB_ERROR;
    }
    else if( errcheck != (usb_er_t)USB_CTRL_END )
    {
        return USB_ERROR;
    }
    else
    {
    }

    return  USB_E_OK;
}   /* eof usb_hmsc_GetStringDescriptor1Check() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor2Check
Description     : Get String descriptor Check
Arguments       : uint16_t errcheck         : errcheck
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor2Check( uint16_t errcheck)
{
    if( errcheck == (usb_er_t)USB_DATA_STALL )
    {
        return USB_ERROR;
    }
    else if( errcheck != (usb_er_t)USB_CTRL_END )
    {
        return USB_ERROR;
    }
    else
    {
    }

    return USB_E_OK;
}   /* eof usb_hmsc_GetStringDescriptor2Check() */

/******************************************************************************
End  Of File
******************************************************************************/
