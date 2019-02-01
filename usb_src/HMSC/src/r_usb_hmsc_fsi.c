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
* File Name    : r_usb_hmsc_fsi.c
* Version      : 1.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB Host MSC BOT File system interface
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.08.2010 0.91    First Release
*         : 06.08.2010 0.92    Updated comments
*         : 29.10.2010 1.00    Mass Production Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"             /* Type define */
#include "r_usb_ckernelid.h"
#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
#include "r_usb_hmsc_extern.h"      /* Host MSC grobal define */

/* Condition compilation by the difference of the File system */
 #ifdef USB_TFAT_USE_PP
  #include  "r_TinyFAT.h"               /* TFAT library */
  #define   TFAT_SECT_SIZE  512         /* TFAT Sector Size */
 #endif /* USB_TFAT_USE_PP */

/* Condition compilation by the difference of the File system */
 #ifdef USB_FAT_USE_PP
   #include "rfilesys.h"                /* File System Libraly */
 #endif /* USB_FAT_USE_PP */

#include "r_usb_catapi_define.h"    /* Peripheral ATAPI Device extern */
#include "r_usb_usrconfig.h"        /* System definition */
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include "r_usb_api.h"
#include "r_usb_hmsc_api.h"

extern void         usb_cstd_dummy_function(uint16_t data1, uint16_t data2);

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
Renesas Abstracted HMSC Driver functions
******************************************************************************/

/* Condition compilation by the difference of the File system */
#ifdef USB_TFAT_USE_PP
    /* nothing */
#else   /* !USB_TFAT_USE_PP */

/******************************************************************************
Function Name   : dev_open
Description     : Drive open
Arguments       : int side : drive number
Return value    : int : [OK:0 / NG:-1]
******************************************************************************/
int dev_open(int side)
{
    uint16_t    error;

    /* close function */
    error = R_usb_hmsc_StrgDriveOpen( (uint16_t)side);

    return (0);
}   /* eof dev_open() */

/******************************************************************************
Function Name   : dev_close
Description     : Drive Close
Arguments       : int side : drive number
Return value    : int : [OK:0 / NG:-1]
******************************************************************************/
int dev_close(int side)
{
    uint16_t    error;

    /* close function */
    error = R_usb_hmsc_StrgDriveClose((uint16_t)side);

    return (0);
}   /* eof dev_close() */

/******************************************************************************
Function Name   : read_sector
Description     : Read sector
Arguments       : int side : drive number
                : unsigned char *buff : buffer address
                : unsigned long secno : sector number
                : long seccnt : sector size
Return value    : int : [OK:0 / NG:-1]
******************************************************************************/
int read_sector( int side, unsigned char *buff, unsigned long secno, long seccnt)
{
    uint16_t    error;
    uint32_t    trans_byte;

    /* change sector --> LBA */
    secno = secno + offset_sector[side];
    /* set transfer length */
    trans_byte = (uint32_t)seccnt * sector_size[side];

    /* read function */
    error = R_usb_hmsc_StrgReadSector( (uint16_t)side, buff, secno, (uint16_t)seccnt, trans_byte);

    return (0);
}   /* eof read_sector() */

/******************************************************************************
Function Name   : write_sector
Description     : Write sector
Arguments       : int side : drive number
                : unsigned char *buff : buffer address
                : unsigned long secno : sector number
                : long seccnt : sector size
Return value    : int : [OK:0 / NG:-1]
******************************************************************************/
int write_sector(usb_utr_t *ptr, int side, unsigned char *buff, unsigned long secno,
     long seccnt)
{
    uint16_t    error;
    uint32_t    trans_byte;

    /* change sector --> LBA */
    secno = secno + offset_sector[side];
    /* set transfer length */
    trans_byte = (uint32_t)seccnt * sector_size[side];

    /* write function */
    error = R_usb_hmsc_StrgWriteSector( (uint16_t)side, buff, secno, (uint16_t)seccnt, trans_byte);

    return (0);
}   /* eof write_sector() */
#endif  /* USB_TFAT_USE_PP */


/******************************************************************************
Function Name   : usb_hmsc_SmpFsiSectorInitialized
Description     : Initialized global area
Arguments       : int side : drive number
                : uint32_t offset : buffer address
                : uint16_t size : sector size
Return value    : none
******************************************************************************/
void usb_hmsc_SmpFsiSectorInitialized(uint16_t side, uint32_t offset,
     uint16_t size)
{
/* Condition compilation by the difference of the File system */
#ifdef USB_TFAT_USE_PP

    R_tfat_disk_initialize(); 

#else   /* !USB_TFAT_USE_PP */

    /* Clear global area of filesystem */
    /* byte@sector */
    sector_size[side]   = size;
    /* sector@cluster */
    cluster_size[side]  = 0;
    /* FAT sector(reserved) */
    fat_top[side]       = (uint16_t)0;
    /* FAT number */
    fat_count[side]     = 0;
    /* drive type */
    dfs_mediatype[side] = 0;
    /* offset sector */
    offset_sector[side] = offset;

    /* FAT type */
    fat_type[side]      = 0x00;
    /* FAT sector number */
    fat_sector[side]    = (uint16_t)0;
    /* Total sector number */
    total_sector[side]  = (uint16_t)0;

    rootdir_top[side]   = (uint16_t)0;
    data_top[side]      = (uint16_t)0;

#endif  /* USB_TFAT_USE_PP*/
}   /* eof usb_hmsc_SmpFsiSectorInitialized() */

/******************************************************************************
Function Name   : usb_hmsc_SmpFsiOffsetSectorRead
Description     : Offset Sector Read
Arguments       : int side : drive number
                : uint16_t size : sector size
Return value    : uint32_t offset_sector[side]
******************************************************************************/
uint32_t usb_hmsc_SmpFsiOffsetSectorRead(uint16_t side)
{
/* Condition compilation by the difference of the File system */
#ifdef USB_TFAT_USE_PP
    return (uint32_t)0uL;
#else   /* !USB_TFAT_USE_PP */
    /* offset sector */
    return offset_sector[side];
#endif  /* USB_TFAT_USE_PP */
}   /* eof usb_hmsc_SmpFsiOffsetSectorRead() */

/******************************************************************************
Function Name   : usb_hmsc_SmpFsiFileSystemInitialized
Description     : Initialized global area
Arguments       : uint16_t side    : drive number
                : uint8_t *Data    : partition table
                : uint32_t Offset  : offset address
Return value    : uint16_t DONE
******************************************************************************/
uint16_t usb_hmsc_SmpFsiFileSystemInitialized(uint16_t side, uint8_t *Data,
     uint32_t Offset)
{
/* Condition compilation by the difference of the File system */
#ifdef USB_TFAT_USE_PP

#else   /* !USB_TFAT_USE_PP */

    USB_PBR_t       *pbr_data;
    USB_FAT1216_t   *fat1216;
    USB_FAT32_t     *fat32;
    uint16_t        SectorSize, ReservedSector, FATSector, TotalSector;
    uint16_t        FSinfoSector, RootdirEntry;
    uint32_t        OffsetSector, TotalSector32, FATSector32;

    pbr_data        = (USB_PBR_t*)Data;
    fat1216         = (USB_FAT1216_t*)&(pbr_data->FATSigData);
    fat32           = (USB_FAT32_t*)&(pbr_data->FATSigData);

    SectorSize      =  (uint16_t)(pbr_data->SectorSize[0]);
    SectorSize      |= (uint16_t)((uint16_t)(pbr_data->SectorSize[1]) << 8);
    ReservedSector  =  (uint16_t)(pbr_data->ReservedSector[0]);
    ReservedSector  |= (uint16_t)((uint16_t)(pbr_data->ReservedSector[1]) << 8);
    RootdirEntry    =  (uint16_t)(pbr_data->RootDirTop[0]);
    RootdirEntry    |= (uint16_t)((uint16_t)(pbr_data->RootDirTop[1]) << 8);
    FATSector       =  (uint16_t)(pbr_data->FATSector[0]);
    FATSector       |= (uint16_t)((uint16_t)(pbr_data->FATSector[1]) << 8);
    TotalSector     =  (uint16_t)(pbr_data->TotalSector0[0]);
    TotalSector     |= (uint16_t)((uint16_t)(pbr_data->TotalSector0[1]) << 8);
    FSinfoSector    =  (uint16_t)(fat32->FSinfoSector[0]);
    FSinfoSector    |= (uint16_t)((uint16_t)(fat32->FSinfoSector[1]) << 8);
    OffsetSector    =  (uint32_t)(pbr_data->OffsetSector[0]);
    OffsetSector    |= ((uint32_t)(pbr_data->OffsetSector[1]) << 8);
    OffsetSector    |= ((uint32_t)(pbr_data->OffsetSector[2]) << 16);
    OffsetSector    |= ((uint32_t)(pbr_data->OffsetSector[3]) << 24);
    TotalSector32   =  (uint32_t)(pbr_data->TotalSector1[0]);
    TotalSector32   |= ((uint32_t)(pbr_data->TotalSector1[1]) << 8);
    TotalSector32   |= ((uint32_t)(pbr_data->TotalSector1[2]) << 16);
    TotalSector32   |= ((uint32_t)(pbr_data->TotalSector1[3]) << 24);
    FATSector32     =  (uint32_t)(fat32->FATSector[0]);
    FATSector32     |= ((uint32_t)(fat32->FATSector[1]) << 8);
    FATSector32     |= ((uint32_t)(fat32->FATSector[2]) << 16);
    FATSector32     |= ((uint32_t)(fat32->FATSector[3]) << 24);

    /* Set global area of filesystem  */
    /* byte@sector */
    sector_size[side]   = SectorSize;
    /* sector@cluster */
    cluster_size[side]  = pbr_data->ClusterSize;
    /* FAT sector(reserved) */
    fat_top[side]       = ReservedSector;
    /* FAT number */
    fat_count[side]     = pbr_data->FatCount;
    /* drive type */
    dfs_mediatype[side] = pbr_data->DfsMediaType;
    /* offset sector */
    offset_sector[side] = Offset;

    /* FAT sector number */
    fat_sector[side]    = FATSector;
    /* Total sector number */
    total_sector[side]  = TotalSector;
    if( TotalSector == 0 ) 
    {
        total_sector[side] = TotalSector32;
    }

    if( FATSector == 0 ) 
    {
        /* FAT type */
        if( TotalSector32 != (uint16_t)0 ) 
        {
            /* FAT32 */
            fat_type[side]      = 0x03;
            /* FAT sector number */
            fat_sector[side]    = FATSector32;
            /* FSINFO sector number */
            fsinfo_sector[side] = FSinfoSector;
        } 
        else 
        {
            /* Un-known */
            fat_type[side]      = 0x00;
        }
    } 
    else 
    {
        if( (fat1216->FileSystemType[3] == 0x31) && (fat1216->FileSystemType[4] == 0x32) ) 
        {
            /* FAT12 */
            fat_type[side]      = 0x01;
        } 
        else if( (fat1216->FileSystemType[3] == 0x31) && (fat1216->FileSystemType[4] == 0x36) ) 
        {
            /* FAT16 */
            fat_type[side]      = 0x02;
        } 
        else 
        {
            /* Un-known */
            fat_type[side]      = 0x00;
        }
    }

    rootdir_top[side] = fat_top[side] + (fat_sector[side] * fat_count[side]);
    data_top[side] = rootdir_top[side] + (uint32_t)((RootdirEntry * 32 + sector_size[side] - 1) / sector_size[side]);
#endif  /* USB_TFAT_USE_PP */

    return USB_E_OK;
}   /* eof usb_hmsc_SmpFsiFileSystemInitialized() */

/******************************************************************************
End  Of File
******************************************************************************/
