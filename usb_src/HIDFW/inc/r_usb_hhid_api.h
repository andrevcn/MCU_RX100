/******************************************************************************
* File Name     : r_usb_hhid_api.h
* Version       : 2.10
* Description   : USB Host HID Driver API declaration
******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
******************************************************************************/
#ifndef __R_USB_HHID_API_H__
#define __R_USB_HHID_API_H__


/*****************************************************************************
Public Functions (API)
******************************************************************************/
void        R_usb_hhid_task(void);
void        R_usb_hhid_ClassCheck(uint8_t **table);
void        R_usb_hhid_DriverStart(void);
void        R_usb_hhid_DriverStop(void);
void        R_usb_hhid_SetPipeRegistration(usb_addr_t devadr);
usb_er_t    R_usb_hhid_PipeTransferExample(uint8_t *table, usb_leng_t size, usb_cb_t complete);
usb_er_t    R_usb_hhid_TransferEnd(void);
usb_er_t    R_usb_hhid_class_request(USB_HHID_CLASS_REQUEST_PARM_t *pram);
void        R_usb_hhid_DeviceInformation(uint16_t *deviceinfo);
void        R_usb_hhid_ChangeDeviceState(uint16_t msginfo, usb_port_t port, usb_cbinfo_t callback);
uint16_t    R_usb_hhid_GetReportLength(void);
uint8_t     R_usb_hhid_get_hid_protocol(void);


#endif  /* __R_USB_HHID_API_H__ */
/******************************************************************************
End Of File
******************************************************************************/
