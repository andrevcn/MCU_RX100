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
* File Name    : r_usb_pbc.c
* Version      : 2.10
* Description  : This is the USB peripheral battery charging code.
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
#ifdef  RL78USB
#endif
#ifdef  RL78USB_SECTION
#pragma section @@CODE   P_BC
#pragma section @@BASE   T_BC
#pragma section @@DATA   B_BC
#endif


/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"             /* Type define */
#include "r_usb_usrconfig.h"            /* System definition, USB-FW Library Header */
#include "r_usb_usbip.h"                /* Definition of the USB register & bit define */
#include "r_usb_regaccess.h"            /* Definition of the USB register access macro */

#ifdef USB_PERI_BC_ENABLE
/******************************************************************************
Macro definitions
******************************************************************************/
#define USB_BC_DCD_TIME         600
#define USB_BC_DCD_DBNC         11
#define USB_BC_VDPSRC_ON        42
#define USB_BC_VDMSRC_DIS       22
#define USB_BC_VDMSRC_ON        42

#define USB_BC_DCD_TIMEOUT      0
#define USB_BC_DCD_COMP_SE0     1

/* BC Port Detect Result */
#define USB_BC_PORT_SDP         0x00
#define USB_BC_PORT_CDP         0x01
#define USB_BC_PORT_DCP         0x02


/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */
uint16_t g_usb_bc_detect;

/* functions */
void usb_pstd_bc_detect_process(void);


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* variables */

/* functions */
uint8_t usb_pstd_bc_data_contact_detect(void);
uint8_t usb_pstd_bc_primary_detection(void);
uint8_t usb_pstd_bc_secondary_detection(void);


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* variables */

/* functions */
extern void usb_cpu_delay_1us(uint16_t time);
extern void usb_cpu_delay_xms(uint16_t time);



/******************************************************************************
Renesas Abstracted USB peripheral battery charging driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_bc_process
Description     : Charging Port Detect Process
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_bc_detect_process(void)
{
    USB_SET_PAT(USBBCCTRL0, USB_BATCHGE);

    usb_pstd_bc_data_contact_detect();
    
    if ( usb_pstd_bc_primary_detection() )
    {
        if( usb_pstd_bc_secondary_detection() )
        {
            g_usb_bc_detect = USB_BC_PORT_DCP;
        }
        else
        {
            g_usb_bc_detect = USB_BC_PORT_CDP;
        }
    }
    else
    {
        g_usb_bc_detect = USB_BC_PORT_SDP;
    }

    USB_CLR_PAT(USBBCCTRL0, USB_BATCHGE);
}   /* eof usb_pstd_bc_process() */


/******************************************************************************
Function Name   : usb_pstd_bc_data_contact_detect
Description     : Data Contact Detect
Arguments       : none
Return value    : uint8_t : DCD Complete Status
******************************************************************************/
uint8_t usb_pstd_bc_data_contact_detect(void)
{
    uint16_t    buf, timer=0;

    USB_SET_PAT(SYSCFG, USB_CNEN);
    USB_SET_PAT(USBBCCTRL0, (USB_IDPSRCE | USB_RPDME));
    usb_cpu_delay_xms((uint16_t)5);                                 /* wait stabilization */

    while( timer < USB_BC_DCD_TIME )                                /* [BC1.2 Spec] DCD_TIMEOUT (300-900ms) */
    {
        USB_RD(SYSSTS0, buf);                                       /* Store LNST */
        if( (buf & USB_LNST) == USB_SE0)
        {
            usb_cpu_delay_xms((uint16_t)USB_BC_DCD_DBNC);           /* [BC1.2 Spec] DCD_DBNC (min:10ms) */
            timer += USB_BC_DCD_DBNC;
            USB_RD(SYSSTS0, buf);                                   /* Store LNST */
            if( (buf & USB_LNST) == USB_SE0)
            {
                USB_CLR_PAT(USBBCCTRL0, (USB_IDPSRCE | USB_RPDME));
                USB_CLR_PAT(SYSCFG, USB_CNEN);
                return USB_BC_DCD_COMP_SE0;                         /* Connected Data Line */
            }
        }
        usb_cpu_delay_xms((uint16_t)1);
        timer++;
    }

    USB_CLR_PAT(USBBCCTRL0, (USB_IDPSRCE | USB_RPDME));
    USB_CLR_PAT(SYSCFG, USB_CNEN);
    return USB_BC_DCD_TIMEOUT;                                      /* DCD Timeout */
}   /* eof usb_pstd_bc_data_contact_detect() */


/******************************************************************************
Function Name   : usb_pstd_bc_primary_detection
Description     : Primary Detection
Arguments       : none
Return value    : uint8_t : Detection Port
******************************************************************************/
uint8_t usb_pstd_bc_primary_detection(void)
{
    uint16_t    buf;

    USB_SET_PAT(USBBCCTRL0, (USB_VDPSRCE | USB_IDMSINKE));
    usb_cpu_delay_xms((uint16_t)USB_BC_VDPSRC_ON);          /* [BC1.2 Spec] TVDPSRC_ON (min:40ms) */
    USB_RD(USBBCCTRL0, buf);                                /* store CHGDETSTS */
    USB_CLR_PAT(USBBCCTRL0, (USB_VDPSRCE | USB_IDMSINKE));
    usb_cpu_delay_xms((uint16_t)USB_BC_VDMSRC_DIS);         /* [BC1.2 Spec] TVDMSRC_DIS (max:20ms) */

    if( buf & USB_CHGDETSTS )
    {
        return 1;                                           /* Detected Charging Port */
    }
    else
    {
        return 0;                                           /* Detected SDP */
    }
}   /* eof usb_pstd_bc_primary_detection() */


/******************************************************************************
Function Name   : usb_pstd_secondary_detection
Description     : Secondary Detection
Arguments       : none
Return value    : uint8_t : Detection Port
******************************************************************************/
uint8_t usb_pstd_bc_secondary_detection(void)
{
    uint16_t    buf;

    USB_SET_PAT(USBBCCTRL0, (USB_VDMSRCE | USB_IDPSINKE));
    usb_cpu_delay_xms((uint16_t)USB_BC_VDMSRC_ON);          /* [BC1.2 Spec] TVDMSRC_ON (min:40ms) */
    USB_RD(USBBCCTRL0, buf);                                /* store PDDETSTS */
    USB_CLR_PAT(USBBCCTRL0, (USB_VDMSRCE | USB_IDPSINKE));

    if( buf & USB_PDDETSTS )
    {
        return 1;                                           /* Detected DCP */
    }
    else
    {
        return 0;                                           /* Detected CDP */
    }
}   /* eof usb_pstd_secondary_detection() */

#endif /* USB_PERI_BC_ENABLE */

/******************************************************************************
End of file
******************************************************************************/
