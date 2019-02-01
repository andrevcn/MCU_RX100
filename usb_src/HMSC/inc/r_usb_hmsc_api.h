/******************************************************************************
* File Name     : r_usb_hmsc_api.h
* Version       : 2.10
* Description   : USB Host MSC Driver API declaration
******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
******************************************************************************/
#ifndef __R_USB_HMSC_API_H__
#define __R_USB_HMSC_API_H__

/******************************************************************************
Macro definitions
******************************************************************************/
#define USB_SEQ_0               (uint16_t)0x0000
#define USB_SEQ_1               (uint16_t)0x0001
#define USB_SEQ_2               (uint16_t)0x0002
#define USB_SEQ_3               (uint16_t)0x0003
#define USB_SEQ_4               (uint16_t)0x0004
#define USB_SEQ_5               (uint16_t)0x0005
#define USB_SEQ_6               (uint16_t)0x0006
#define USB_SEQ_7               (uint16_t)0x0007
#define USB_SEQ_8               (uint16_t)0x0008
#define USB_SEQ_9               (uint16_t)0x0009
#define USB_SEQ_10              (uint16_t)0x000a

/*****************************************************************************
Public Functions (API)
******************************************************************************/

uint16_t    R_usb_hmsc_GetDevSts(void);
uint16_t    R_usb_hmsc_Information(uint16_t PipeOffset);
uint16_t    R_usb_hmsc_Inquiry(uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_ModeSelect6(uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_ModeSense10(uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_PreventAllow(uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_Read10(uint16_t side, uint8_t *buff,
                uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
uint16_t    R_usb_hmsc_ReadCapacity(uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_ReadFormatCapacity(uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_RequestSense(uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_SetDevSts(uint16_t data);
uint16_t    R_usb_hmsc_StrgDriveClose(uint16_t side);
usb_er_t    R_usb_hmsc_StrgDriveOpen(uint16_t side);
uint16_t    R_usb_hmsc_StrgDriveSearch(uint16_t addr);
uint16_t    R_usb_hmsc_StrgReadSector(uint16_t side, uint8_t *buff
                , uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
uint16_t    R_usb_hmsc_StrgUserCommand(uint16_t side, uint16_t command, uint8_t *buff);
uint16_t    R_usb_hmsc_StrgWriteSector(uint16_t side, uint8_t *buff
                , uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
uint16_t    R_usb_hmsc_TestUnit(uint16_t side);
uint16_t    R_usb_hmsc_Write10(uint16_t side, uint8_t *buff
                , uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
void        R_usb_hmsc_ClassCheck(uint8_t **table);
void        R_usb_hmsc_Initialized();
void        R_usb_hmsc_Release();
void        R_usb_hmsc_TaskClose();
void        R_usb_hmsc_TaskOpen(uint16_t data1, uint16_t data2);
void        R_usb_hmsc_driver_start();

void        R_usb_hmsc_ClearStall(uint16_t type, uint16_t msgnum, usb_cb_t complete);
usb_er_t    R_usb_hmsc_MassStorageReset(uint16_t drvnum, usb_cb_t complete);
usb_er_t    R_usb_hmsc_GetMaxUnit(uint16_t addr, usb_cb_t complete);

#endif  /* __R_USB_HMSC_API_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
