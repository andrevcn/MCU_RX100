/*******************************************************************************
* File Name    : r_usb_hport.h
* Version      : 2.10
* Description  : USB Basic firmware library function prototype declaration
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 31.03.2012 1.80    Basic FW mini Ver2.00(ES Version)
*           : 30.11.2012 2.00    Basic FW mini Ver2.00 Release
*           : 31.01.2013 2.01    Corresponded to RL78/L1C
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/
#ifndef __R_USB_HPORT_H__
#define __R_USB_HPORT_H__


/******************************************************************************
Public Functions (HCD)
******************************************************************************/
/* por0 function prototype declaration */
void        usb_hstd_bchg_enable_p0(void);
void        usb_hstd_bchg_disable_p0(void);
void        usb_hstd_set_uact_p0(void);
void        usb_hstd_ovrcr_clear_status_p0(void);
void        usb_hstd_ovrcr_enable_p0(void);
void        usb_hstd_ovrcr_disable_p0(void);
void        usb_hstd_attch_clear_status_p0(void);
void        usb_hstd_detch_clear_status_p0(void);
void        usb_hstd_attch_enable_p0(void);
void        usb_hstd_attch_disable_p0(void);
void        usb_hstd_detach_enable_p0(void);
void        usb_hstd_detach_disable_p0(void);
void        usb_hstd_vbus_control_on_p0(void);
void        usb_hstd_vbus_control_off_p0(void);
void        usb_hstd_bus_int_disable_p0(void);

/* por1 function prototype declaration */
void        usb_hstd_bchg_enable_p1(void);
void        usb_hstd_bchg_disable_p1(void);
void        usb_hstd_set_uact_p1(void);
void        usb_hstd_ovrcr_clear_status_p1(void);
void        usb_hstd_ovrcr_enable_p1(void);
void        usb_hstd_ovrcr_disable_p1(void);
void        usb_hstd_attch_clear_status_p1(void);
void        usb_hstd_detch_clear_status_p1(void);
void        usb_hstd_attch_enable_p1(void);
void        usb_hstd_attch_disable_p1(void);
void        usb_hstd_detach_enable_p1(void);
void        usb_hstd_detach_disable_p1(void);
void        usb_hstd_vbus_control_on_p1(void);
void        usb_hstd_vbus_control_off_p1(void);
void        usb_hstd_bus_int_disable_p1(void);



#endif  /* __R_USB_HPORT_H__ */
/******************************************************************************
End of file
******************************************************************************/
