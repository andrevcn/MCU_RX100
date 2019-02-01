/*******************************************************************************
* File Name    : r_usb_api.h
* Version      : 2.10
* Description  : USB Basic firmware API function prototype declaration
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 31.03.2012 1.80    Basic FW mini Ver2.00(ES Version)
*           : 30.11.2012 2.00    Basic FW mini Ver2.00 Release
*           : 31.01.2013 2.01    Corresponded to RL78/L1C
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/
#ifndef __R_USB_API_H__
#define __R_USB_API_H__


/******************************************************************************
Public Functions (API)
******************************************************************************/
/* HCD API function (HCD control) */
void            R_usb_hstd_HcdTask(void);
void            R_usb_hstd_MgrTask(void);
void            R_usb_hstd_HcdOpen(void);
void            R_usb_hstd_DriverRegistration(usb_hcdreg_t *registinfo);
void            R_usb_hstd_DriverRelease(uint8_t devclass);
usb_er_t        R_usb_hstd_TransferStart(usb_utr_t *utr_table);
usb_er_t        R_usb_hstd_TransferEnd(usb_pipe_t pipe, usb_strct_t msginfo);
usb_er_t        R_usb_hstd_MgrChangeDeviceState(usb_cbinfo_t complete, usb_strct_t msginfo, usb_strct_t keyword);
usb_er_t        R_usb_hstd_ChangeDeviceState(usb_strct_t msginfo);
void            R_usb_hstd_DeviceInformation(usb_addr_t addr, uint16_t *table);

/* USB interrupt handler */
void            usb_hstd_interrupt_handler(void);

/* USB-IP register control */
usb_er_t        R_usb_hstd_ChkPipeInfo(uint16_t *table, uint8_t *descriptor);
void            R_usb_hstd_ReturnEnuMGR(uint16_t cls_result);
void            R_usb_hstd_SetPipeRegistration(uint16_t *table, uint16_t command);

/* PCD API function (PCD control) */
void            R_usb_pstd_PcdTask(void);
void            R_usb_pstd_PcdOpen(void);
void            R_usb_pstd_DriverRegistration(usb_pcdreg_t *registinfo);
usb_er_t        R_usb_pstd_TransferStart(usb_utr_t *utr_table);
usb_er_t        R_usb_pstd_TransferEnd(usb_pipe_t pipe, usb_strct_t msginfo);
usb_er_t        R_usb_pstd_PcdChangeDeviceState(usb_strct_t msginfo);
void            R_usb_pstd_DeviceInformation(uint16_t *table);
void            R_usb_pstd_SetPipeRegister(uint16_t *table, uint16_t command);

/* USB interrupt handler */
void            usb_pstd_interrupt_handler(void);

/* USB-IP register control */
void            R_usb_pstd_SetStallPipe0(void);
usb_er_t        R_usb_pstd_SetPipeStall(usb_pipe_t pipe);

/* Control transfer API function */
uint8_t         R_usb_pstd_ControlRead(usb_leng_t bsize, uint8_t *table);
void            R_usb_pstd_ControlWrite(usb_leng_t bsize, uint8_t *table);
void            R_usb_pstd_ControlEnd(uint16_t status);

/* Scheduler basic functions */
uint8_t         R_usb_cstd_Scheduler(void);

/* Scheduler API functions */
usb_er_t        R_usb_cstd_RecMsg( uint8_t id, usb_msg_t** mess );
usb_er_t        R_usb_cstd_SndMsg( uint8_t id, usb_msg_t* mess );
usb_er_t        R_usb_cstd_iSndMsg( uint8_t id, usb_msg_t* mess );
usb_er_t        R_usb_cstd_WaiMsg( uint8_t id, usb_msg_t* mess, uint16_t count );
usb_er_t        R_usb_cstd_PgetSend( uint8_t id, usb_strct_t msginfo, usb_cbinfo_t complete, usb_strct_t keyword );
usb_er_t        R_usb_cstd_RelBlk( uint8_t blk_num );

/* USB library API functions */
void            R_usb_cstd_SetBufPipe0(void);
void            R_usb_cstd_debug_hook(uint16_t error_code);


/******************************************************************************
Macro definitions (Debug hook)
******************************************************************************/
#ifdef USB_DEBUG_HOOK_USE
  #define USB_DEBUG_HOOK(x)     R_usb_cstd_debug_hook(x)
#else
  #define USB_DEBUG_HOOK(x)
#endif


#endif  /* __R_USB_API_H__ */
/******************************************************************************
End of file
******************************************************************************/
