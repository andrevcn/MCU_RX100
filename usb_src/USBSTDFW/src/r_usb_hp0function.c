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
* File Name    : r_usb_hp0function.c
* Version      : 2.10
* Description  : This is the USB host control driver code.
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 31.03.2012 1.80    Basic FW mini Ver2.00(ES Version)
*           : 30.11.2012 2.00    Basic FW mini Ver2.00 Release
*           : 31.01.2013 2.01    Corresponded to RL78/L1C
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/

/******************************************************************************
Section definitions
******************************************************************************/
#ifdef  R8CUSB
#pragma section program P_hcd
#pragma section bss B_hcd
#endif
#ifdef  RL78USB
#endif
#ifdef  RL78USB_SECTION
#pragma section @@CODE   P_HCD
#pragma section @@BASE   T_HCD
#pragma section @@DATA   B_HCD
#endif


/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"             /* Type define */
#include "r_usb_ckernelid.h"            /* Kernel ID definition */
#include "r_usb_usrconfig.h"            /* System definition, USB-FW Library Header */
#include "r_usb_usbip.h"                /* Definition of the USB register & bit define */
#include "r_usb_regaccess.h"            /* Definition of the USB register access macro */


/******************************************************************************
Macro definitions
******************************************************************************/
/* There is no macro definition. */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* variables */

/* functions */
USB_STATIC  void        usb_hstd_bchg_clear_status_p0(void);


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* Exported global variables and functions are declared with the header file. */


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* variables */
extern uint16_t     g_usb_LibTemp;                          /* macro temp */



/******************************************************************************
Renesas Abstracted Host Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_bchg_clear_status_p0
Description     : Clear port BCHG interrupt status
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_hstd_bchg_clear_status_p0(void)
{
  USB_CLR_STS(INTSTS1, USB_BCHG);     /* Status Clear */
}   /* eof usb_hstd_bchg_clear_status_p0() */


/******************************************************************************
Function Name   : usb_hstd_bchg_enable_p0
Description     : Enable port BCHG interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_bchg_enable_p0(void)
{
  usb_hstd_bchg_clear_status_p0();
  USB_SET_PAT(INTENB1, USB_BCHGE);
}   /* eof usb_hstd_bchg_enable_p0() */


/******************************************************************************
Function Name   : usb_hstd_bchg_disable_p0
Description     : Disable port BCHG interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_bchg_disable_p0(void)
{
  usb_hstd_bchg_clear_status_p0();
  USB_CLR_PAT(INTENB1, USB_BCHGE);
}   /* eof usb_hstd_bchg_disable_p0() */


/******************************************************************************
Function Name   : usb_hstd_set_uact_p0
Description     : SOF start
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_set_uact_p0(void)
{
  USB_MDF_PAT(DVSTCTR0, (USB_UACT), (USB_USBRST | USB_RESUME | USB_UACT));
}   /* eof usb_hstd_set_uact_p0() */


/******************************************************************************
Function Name   : usb_hstd_ovrcr_clear_status_p0
Description     : Clear port OVRCR interrupt status
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_ovrcr_clear_status_p0(void)
{
  USB_CLR_STS(INTSTS1, USB_OVRCR);
}   /* eof usb_hstd_ovrcr_clear_status_p0() */


/******************************************************************************
Function Name   : usb_hstd_ovrcr_enable_p0
Description     : Enable OVRCR interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_ovrcr_enable_p0(void)
{
  usb_hstd_ovrcr_clear_status_p0();
  USB_SET_PAT(INTENB1, USB_OVRCRE);
}   /* eof usb_hstd_ovrcr_enable_p0() */


/******************************************************************************
Function Name   : usb_hstd_ovrcr_disable_p0
Description     : Disable OVRCR interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_ovrcr_disable_p0(void)
{
  usb_hstd_ovrcr_clear_status_p0();
  USB_CLR_PAT(INTENB1, USB_OVRCRE);
}   /* eof usb_hstd_ovrcr_disable_p0() */


/******************************************************************************
Function Name   : usb_hstd_attch_clear_status_p0
Description     : Clear port ATTCH interrupt status
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_attch_clear_status_p0(void)
{
  USB_CLR_STS(INTSTS1, USB_ATTCH);
}   /* eof usb_hstd_attch_clear_status_p0() */


/******************************************************************************
Function Name   : usb_hstd_attch_enable_p0
Description     : Enable ATTCH interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_attch_enable_p0(void)
{
  usb_hstd_attch_clear_status_p0();
  USB_SET_PAT(INTENB1, USB_ATTCHE);
}   /* eof usb_hstd_attch_enable_p0() */


/******************************************************************************
Function Name   : usb_hstd_attch_disable_p0
Description     : Disable ATTCH interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_attch_disable_p0(void)
{
  usb_hstd_attch_clear_status_p0();
  USB_CLR_PAT(INTENB1, USB_ATTCHE);
}   /* eof usb_hstd_attch_disable_p0() */


/******************************************************************************
Function Name   : usb_hstd_detch_clear_status_p0
Description     : Clear port DTCH interrupt status
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_detch_clear_status_p0(void)
{
  USB_CLR_STS(INTSTS1, USB_DTCH);
}   /* eof usb_hstd_detch_clear_status_p0() */


/******************************************************************************
Function Name   : usb_hstd_detach_enable_p0
Description     : Enable DTCH interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_detach_enable_p0(void)
{
  usb_hstd_detch_clear_status_p0();
  USB_SET_PAT(INTENB1, USB_DTCHE);
}   /* eof usb_hstd_detach_enable_p0() */


/******************************************************************************
Function Name   : usb_hstd_detach_disable_p0
Description     : Disable DTCH interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_detach_disable_p0(void)
{
  usb_hstd_detch_clear_status_p0();
  USB_CLR_PAT(INTENB1, USB_DTCHE);
}   /* eof usb_hstd_detach_disable_p0() */


/******************************************************************************
Function Name   : usb_hstd_vbus_control_on_p0
Description     : USB VBUS control
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_vbus_control_on_p0(void)
{
  // USB_SET_PAT(DVSTCTR0, USB_VBOUT); // ALISON - 22/01/14
  USB_CLR_PAT(DVSTCTR0, USB_VBOUT);
}   /* eof usb_hstd_vbus_control_on_p0() */


/******************************************************************************
Function Name   : usb_hstd_vbus_control_off_p0
Description     : USB VBUS control
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_vbus_control_off_p0(void)
{
  // USB_CLR_PAT(DVSTCTR0, USB_VBOUT); // ALISON - 22/01/14
  USB_SET_PAT(DVSTCTR0, USB_VBOUT);
}   /* eof usb_hstd_vbus_control_off_p0() */


/******************************************************************************
Function Name   : usb_hstd_bus_int_disable_p0
Description     : Disable USB Bus-interrupt
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bus_int_disable_p0(void)
{
  /* OVRCR, ATTCH, DTCH, BCHG  interrupt disable */
  usb_hstd_ovrcr_disable_p0();
  usb_hstd_attch_disable_p0();
  usb_hstd_detach_disable_p0();
  usb_hstd_bchg_disable_p0();
}   /* eof usb_hstd_bus_int_disable_p0() */


/******************************************************************************
End of file
******************************************************************************/
