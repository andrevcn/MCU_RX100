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
* File Name    : r_usb_hmsc_extern.h
* Version      : 2.10
* Description  : USB common uItron header
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
******************************************************************************/
#ifndef __R_USB_HMSC_EXTERN_H__
#define __R_USB_HMSC_EXTERN_H__

#include "r_usb_cmsc_define.h"      /* USB  Mass Storage Class Header */

/*****************************************************************************
Public Valiables
******************************************************************************/
/* TEMP - ALISON */
/* Condition compilation by the difference of quality control */
#ifdef  USB_MISRA_CHECK_PP
extern uint32_t         data_top[];
extern uint32_t         fat_sector[];
extern uint32_t         fat_top[];
extern uint32_t         fsinfo_sector[];
extern uint32_t         offset_sector[];
extern uint32_t         rootdir_top[];
extern uint32_t         total_sector[];
extern uint8_t          cluster_size[];
extern uint8_t          dfs_mediatype[];
extern uint8_t          fat_count[];
extern uint8_t          fat_type[];
extern uint16_t         sector_size[];
#endif /* USB_MISRA_CHECK_PP */

extern uint16_t         usb_ghmsc_AttSts;
extern uint16_t         usb_ghmsc_DefEpTbl[];
extern uint16_t         usb_ghmsc_DriveChk[][5];
extern uint16_t         usb_ghmsc_InPipe[][2];
extern uint16_t         usb_ghmsc_MaxDrive;
extern uint16_t         usb_ghmsc_OutPipe[][2];
extern uint16_t         usb_ghmsc_Seccnt;
extern uint16_t         usb_ghmsc_Side;
extern uint16_t         usb_ghmsc_StrgCount;
extern uint16_t         usb_ghmsc_SuspendPipe[];
extern uint16_t         usb_ghmsc_TmpEpTbl[];
extern uint32_t         usb_ghmsc_CbwTagNo[];
extern uint32_t         usb_ghmsc_CswDataResidue[];
extern uint32_t         usb_ghmsc_MaxLun;
extern uint32_t         usb_ghmsc_Secno;
extern uint32_t         usb_ghmsc_TransSize;
extern uint8_t          *usb_ghmsc_Buff;
extern uint8_t          usb_ghmsc_AtapiFlag[];
extern uint8_t          usb_ghmsc_Data[];
extern uint8_t          usb_ghmsc_ClassData[];
extern USB_MSC_CBW_t    usb_ghmsc_Cbw[];
extern USB_MSC_CSW_t    usb_ghmsc_Csw[];
extern usb_utr_t        usb_ghmsc_ControlData;
extern usb_utr_t        usb_ghmsc_ReceiveData[];
extern usb_utr_t        usb_ghmsc_TransData[];

extern uint16_t         usb_shmsc_NoDataSeq;
extern uint16_t         usb_shmsc_DataInSeq;
extern uint16_t         usb_shmsc_DataOutSeq;
extern uint16_t         usb_shmsc_StallErrSeq;
extern uint16_t         usb_shmsc_DataStallSeq;


/*****************************************************************************
Public Functions
******************************************************************************/

uint16_t    usb_hmsc_CheckCsw(uint16_t drvnum);
uint16_t    usb_hmsc_DataIn(uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_DataOut(uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_GetCsw(uint16_t drvnum);
uint16_t    usb_hmsc_GetData(uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_GetMaxUnitCheck(uint16_t Err);
uint16_t    usb_hmsc_NoData(uint16_t drvnum);
uint16_t    usb_hmsc_SendCbw(uint16_t drvnum);
uint16_t    usb_hmsc_SendData(uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_SmpBotDescriptor(uint8_t *Table, uint16_t msgnum);
uint16_t    usb_hmsc_SmpCheckAddr(uint16_t side);
uint16_t    usb_hmsc_SmpDevCheckBootRecord(uint8_t *Data, uint32_t *ParLBA, uint8_t *ParInfo, uint16_t flag);
uint16_t    usb_hmsc_SmpDevNextDriveSearch(void);
uint16_t    usb_hmsc_SmpDevReadPartition(uint16_t unit, uint32_t trans_byte);
uint32_t    usb_hmsc_SmpDevReadSectorSize(uint16_t side);
uint16_t    usb_hmsc_SmpDrive2Addr(uint16_t side);
uint16_t    usb_hmsc_SmpDrive2Msgnum(uint16_t side);
uint16_t    usb_hmsc_SmpDrive2Part(uint16_t side);
uint16_t    usb_hmsc_SmpDrive2Unit(uint16_t side);
uint16_t    usb_hmsc_SmpFsiFileSystemInitialized(uint16_t side, uint8_t *Data, uint32_t Offset);
uint32_t    usb_hmsc_SmpFsiOffsetSectorRead(uint16_t side);
uint16_t    usb_hmsc_SmpPipeInfo(uint8_t *table, uint16_t msgnum, uint16_t length);
uint16_t    usb_hmsc_SmpTotalDrive(void);
usb_er_t    usb_hmsc_SubmitCtrEnd(usb_utr_t *utr_table);
usb_er_t    usb_hmsc_Submitutr(uint16_t type, usb_utr_t *utr_table);
void        usb_hmsc_CbwTagCount(uint16_t msgnum);
void        usb_hmsc_ClrData(uint16_t len, uint8_t *buf);
void        usb_hmsc_ProcessResult(uint16_t data, uint16_t dummy);
void        usb_hmsc_SetElsCbw(uint8_t *data, uint32_t trans_byte, uint16_t side);
void        usb_hmsc_SetRwCbw(uint16_t command, uint32_t secno, uint16_t seccnt, uint32_t trans_byte, uint16_t side);
void        usb_hmsc_SmpFsiDriveClear(uint16_t addr);
void        usb_hmsc_SmpFsiSectorInitialized(uint16_t side, uint32_t offset, uint16_t size);
void        usb_hmsc_StrgCheckResult(usb_tskinfo_t *mess);
void        usb_hmsc_StrgCommandResult(usb_tskinfo_t *mess);
void        usb_hmsc_StrgDriveTask(void);
void        usb_hmsc_StrgSpecifiedPath(usb_tskinfo_t *mess);
uint16_t    usb_hmsc_GetStringInfo(uint16_t devaddr, uint8_t *table);
void        usb_hmsc_SmplRegistration(usb_utr_t *ptr);

usb_er_t    usb_hmsc_ClearStall(uint16_t Pipe, usb_cb_t complete);
uint16_t    usb_hmsc_DataInAct(usb_tskinfo_t *mess);
uint16_t    usb_hmsc_DataOutAct(usb_tskinfo_t *mess);
uint16_t    usb_hmsc_GetCswCheck(uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_GetCswReq(uint16_t drvnum);
uint16_t    usb_hmsc_GetDataCheck(uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_GetDataReq(uint16_t drvnum);
uint16_t    usb_hmsc_GetStringInfoCheck(uint16_t devaddr);
uint16_t    usb_hmsc_MassStorageResetCheck(uint16_t Err);
uint16_t    usb_hmsc_NoDataAct(usb_tskinfo_t *mess);
uint16_t    usb_hmsc_SendCbwCheck(uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_SendCbwReq(uint16_t drvnum);
uint16_t    usb_hmsc_SendDataCheck(uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_SendDataReq(uint16_t drvnum);
uint16_t    usb_hmsc_SmpDevReadPartitionAct(usb_tskinfo_t *mess);
void        usb_hmsc_CheckResult(usb_utr_t *mess, uint16_t, uint16_t);
void        usb_hmsc_CheckResultInPipe(usb_utr_t *mess, uint16_t, uint16_t);
void        usb_hmsc_CheckResultOutPipe(usb_utr_t *mess, uint16_t, uint16_t);
void        usb_hmsc_ClassCheck(usb_tskinfo_t *mess);
void        usb_hmsc_ClearStallCheck(uint16_t errcheck);
void        usb_hmsc_ClearStallCheck2(usb_tskinfo_t *mess);
void        usb_hmsc_CommandResult(uint16_t result);

void        usb_hmsc_DataStall(usb_tskinfo_t *mess);
void        usb_hmsc_SmpDevReadSectorSizeAct(usb_tskinfo_t *mess);
void        usb_hmsc_SmpStrgDriveOpenAct( usb_tskinfo_t *mess );
void        usb_hmsc_SpecifiedPath(usb_tskinfo_t *mess);
void        usb_hmsc_StallErr(usb_tskinfo_t *mess);
void        usb_hmsc_StrgDriveSearchAct(usb_tskinfo_t *mess);
void        usb_hmsc_Task(void);

int         dev_close(int side);
int         dev_open(int side);
int         read_sector(int side, unsigned char *buff, unsigned long secno, long seccnt);
int         write_sector(int side, unsigned char *buff, unsigned long secno, long seccnt);

#endif  /* __R_USB_HMSC_EXTERN_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
