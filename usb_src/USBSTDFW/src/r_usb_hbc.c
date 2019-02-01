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
* File Name    : r_usb_hbc.c
* Version      : 2.10
* Description  : This is the USB host battery charging code.
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
#include "r_usb_ckernelid.h"            /* Kernel ID definition */
#include "r_usb_usrconfig.h"            /* System definition, USB-FW Library Header */
#include "r_usb_cdefusbip.h"            /* USB-FW Library Header */
#include "r_usb_usbip.h"                /* Definition of the USB register & bit define */
#include "r_usb_regaccess.h"            /* Definition of the USB register access macro */
#include "r_usb_hport.h"                /* Description : USB host driver public functions */
#include "r_usb_api.h"                  /* USB API public function header */

#ifdef USB_HOST_BC_ENABLE
/******************************************************************************
Macro definitions
******************************************************************************/
/* PD Detect Define */
#define USB_BC_NODET        (0x00u)
#define USB_BC_PDDET        (0x01u)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */
usb_bc_status_t g_usb_hstd_bc[2u];

/* functions */
void usb_hstd_pddetint_process(void);

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* BC State change function */
void usb_hstd_bc_err(void);
void usb_hstd_bc_init_vb(void);
void usb_hstd_bc_det_at(void);
void usb_hstd_bc_cdp_dt(void);
void usb_hstd_bc_sdp_dt(void);
/* BC State entry/exit function */
void usb_hstd_bc_det_entry(void);
void usb_hstd_bc_det_exit(void);
void usb_hstd_bc_cdp_entry(void);
void usb_hstd_bc_cdp_exit(void);
void usb_hstd_bc_sdp_entry(void);
void usb_hstd_bc_sdp_exit(void);
void usb_hstd_bc_dcp_entry(void);

/* BC State change function table */
void (*usb_hstd_bc_func[USB_BC_STATE_MAX][USB_BC_EVENT_MAX])(void) =
{
    /* VBUS_ON              ATTACH               DETACH */
    { usb_hstd_bc_init_vb , usb_hstd_bc_err    , usb_hstd_bc_err    },
    { usb_hstd_bc_err     , usb_hstd_bc_det_at , usb_hstd_bc_err    },
    { usb_hstd_bc_err     , usb_hstd_bc_err    , usb_hstd_bc_cdp_dt },
    { usb_hstd_bc_err     , usb_hstd_bc_err    , usb_hstd_bc_sdp_dt },
    { usb_hstd_bc_err     , usb_hstd_bc_err    , usb_hstd_bc_err    }
};

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* variables */
extern usb_port_t g_usb_HcdPort;

/* functions */
extern void usb_cpu_delay_xms(uint16_t time);
extern void usb_cpu_delay_1us(uint16_t time);

/******************************************************************************
Renesas Abstracted USB host battery charging driver functions
******************************************************************************/

#if USB_PORTSEL_PP == USB_1PORT_PP
/******************************************************************************
Function Name   : usb_hstd_pddetint_process
Description     : PDDETINT process
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_pddetint_process(void)
{
    uint16_t    buf[3];

    /* PDDETSTS chattering cut */
    do
    {
        USB_RD(USBBCCTRL0, buf[0]);
        usb_cpu_delay_1us(10);
        USB_RD(USBBCCTRL0, buf[1]);
        usb_cpu_delay_1us(10);
        USB_RD(USBBCCTRL0, buf[2]);
    }
    while( ((buf[0] & USB_PDDETSTS) != (buf[1] & USB_PDDETSTS)) ||
            ((buf[1] & USB_PDDETSTS) != (buf[2] & USB_PDDETSTS)) );

    if( (buf[0] & USB_PDDETSTS) == USB_PDDETSTS )       /* VDPSRC Detect */
    {
        if( (buf[0] & USB_VDMSRCE) != USB_VDMSRCE )
        {
            USB_SET_PAT(USBBCCTRL0, USB_VDMSRCE);
        }
    }
    else                                                /* VDPSRC Not detect */
    {
        if( (buf[0] & USB_VDMSRCE) == USB_VDMSRCE )
        {
            USB_CLR_PAT(USBBCCTRL0, USB_VDMSRCE);
            g_usb_hstd_bc[g_usb_HcdPort].pd_detect = USB_BC_PDDET;
        }
    }
}   /* eof usb_hstd_pddetint_process() */


/******************************************************************************
Function Name   : usb_hstd_bc_err
Description     : BC State change function [ERROR]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_err(void)
{
    /* Nothing */
}   /* eof usb_hstd_bc_err() */


/******************************************************************************
Function Name   : usb_hstd_bc_init_vb
Description     : BC State change function [INIT] [VbusOn]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_init_vb(void)
{
    g_usb_hstd_bc[USB_PORT0].dcpmode = USB_BC_DCPMODE;
    if( g_usb_hstd_bc[USB_PORT0].dcpmode )
    {
        g_usb_hstd_bc[USB_PORT0].state = USB_BC_STATE_DCP;
        usb_hstd_bc_dcp_entry();
    }
    else
    {
        g_usb_hstd_bc[USB_PORT0].state = USB_BC_STATE_DET;
        usb_hstd_bc_det_entry();
    }
}   /* eof usb_hstd_bc_init_vb() */


/******************************************************************************
Function Name   : usb_hstd_bc_det_at
Description     : BC State change function [DET] [Attach]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_det_at(void)
{
    usb_hstd_bc_det_exit();
    if( g_usb_hstd_bc[USB_PORT0].pd_detect )
    {
        g_usb_hstd_bc[USB_PORT0].state = USB_BC_STATE_CDP;
        usb_hstd_bc_cdp_entry();
    }
    else
    {
        g_usb_hstd_bc[USB_PORT0].state = USB_BC_STATE_SDP;
        usb_hstd_bc_sdp_entry();
    }
}   /* eof usb_hstd_bc_det_at() */


/******************************************************************************
Function Name   : usb_hstd_bc_cdp_dt
Description     : BC State change function [CDP] [Detach]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_cdp_dt(void)
{
    usb_hstd_bc_cdp_exit();
    g_usb_hstd_bc[USB_PORT0].state = USB_BC_STATE_DET;
    usb_hstd_bc_det_entry();
}   /* eof usb_hstd_bc_cdp_dt() */


/******************************************************************************
Function Name   : usb_hstd_bc_sdp_dt
Description     : BC State change function [SDP] [Detach]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_sdp_dt(void)
{
    usb_hstd_bc_sdp_exit();
    g_usb_hstd_bc[USB_PORT0].state = USB_BC_STATE_DET;
    usb_hstd_bc_det_entry();
}   /* eof usb_hstd_bc_sdp_dt() */


/******************************************************************************
Function Name   : usb_hstd_bc_det_entry
Description     : BC State entry function [DET]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_det_entry(void)
{
    USB_SET_PAT(USBBCCTRL0, USB_BATCHGE);
    USB_SET_PAT(USBBCCTRL0, USB_IDPSINKE);
    USB_CLR_STS(INTSTS1, USB_PDDETINT);
    USB_SET_PAT(INTENB1, USB_PDDETINTE);
}   /* eof usb_hstd_bc_det_entry() */


/******************************************************************************
Function Name   : usb_hstd_bc_det_exit
Description     : BC State exit function [DET]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_det_exit(void)
{
    USB_CLR_PAT(INTENB1, USB_PDDETINTE);
    USB_CLR_STS(INTSTS1, USB_PDDETINT);
    USB_CLR_PAT(USBBCCTRL0, USB_IDPSINKE);
    USB_CLR_PAT(USBBCCTRL0, USB_BATCHGE);
}   /* eof usb_hstd_bc_det_exit() */


/******************************************************************************
Function Name   : usb_hstd_bc_cdp_entry
Description     : BC State entry function [CDP]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_cdp_entry(void)
{
}   /* eof usb_hstd_bc_cdp_entry() */


/******************************************************************************
Function Name   : usb_hstd_bc_cdp_exit
Description     : BC State exit function [CDP]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_cdp_exit(void)
{
    g_usb_hstd_bc[USB_PORT0].pd_detect = USB_BC_NODET;
}   /* eof usb_hstd_bc_cdp_exit() */


/******************************************************************************
Function Name   : usb_hstd_bc_sdp_entry
Description     : BC State entry function [SDP]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_sdp_entry(void)
{
}   /* eof usb_hstd_bc_sdp_entry() */


/******************************************************************************
Function Name   : usb_hstd_bc_sdp_exit
Description     : BC State exit function [SDP]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_sdp_exit(void)
{
    /* Nothing */
}   /* eof usb_hstd_bc_sdp_exit() */


/******************************************************************************
Function Name   : usb_hstd_bc_dcp_entry
Description     : BC State entry function [DCP]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_dcp_entry(void)
{
    USB_CLR_PAT(SYSCFG, USB_DRPD);
    USB_SET_PAT(USBBCCTRL0, USB_BATCHGE);
    USB_SET_PAT(USBBCCTRL0, USB_DCPMODE);
    USB_DEBUG_HOOK(USB_DEBUG_HOOK_HOST | USB_DEBUG_HOOK_CODE1);
}   /* eof usb_hstd_bc_dcp_entry() */
#endif  /* USB_PORTSEL_PP == USB_1PORT_PP */


#if USB_PORTSEL_PP == USB_2PORT_PP
/******************************************************************************
Function Name   : usb_hstd_pddetint_process
Description     : PDDETINT process
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_pddetint_process(void)
{
    uint16_t    buf[3];

    /* PDDETSTS chattering cut */
    do
    {
        if( g_usb_HcdPort == USB_PORT0 )
        {
            USB_RD(USBBCCTRL0, buf[0]);
            usb_cpu_delay_1us(10);
            USB_RD(USBBCCTRL0, buf[1]);
            usb_cpu_delay_1us(10);
            USB_RD(USBBCCTRL0, buf[2]);
        }
        else
        {
            USB_RD(USBBCCTRL1, buf[0]);
            usb_cpu_delay_1us(10);
            USB_RD(USBBCCTRL1, buf[1]);
            usb_cpu_delay_1us(10);
            USB_RD(USBBCCTRL1, buf[2]);
        }
    }
    while( ((buf[0] & USB_PDDETSTS) != (buf[1] & USB_PDDETSTS)) ||
            ((buf[1] & USB_PDDETSTS) != (buf[2] & USB_PDDETSTS)) );

    if( (buf[0] & USB_PDDETSTS) == USB_PDDETSTS )       /* VDPSRC Detect */
    {
        if( (buf[0] & USB_VDMSRCE) != USB_VDMSRCE )
        {
            if( g_usb_HcdPort == USB_PORT0 )
            {
                USB_SET_PAT(USBBCCTRL0, USB_VDMSRCE);
            }
            else
            {
                USB_SET_PAT(USBBCCTRL1, USB_VDMSRCE);
            }
        }
    }
    else                                                /* VDPSRC Not detect */
    {
        if( (buf[0] & USB_VDMSRCE) == USB_VDMSRCE )
        {
            if( g_usb_HcdPort == USB_PORT0 )
            {
                USB_CLR_PAT(USBBCCTRL0, USB_VDMSRCE);
            }
            else
            {
                USB_CLR_PAT(USBBCCTRL1, USB_VDMSRCE);
            }
            g_usb_hstd_bc[g_usb_HcdPort].pd_detect = USB_BC_PDDET;
        }
    }
}   /* eof usb_hstd_pddetint_process() */


/******************************************************************************
Function Name   : usb_hstd_bc_err
Description     : BC State change function [ERROR]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_err(void)
{
    /* Nothing */
}   /* eof usb_hstd_bc_err() */


/******************************************************************************
Function Name   : usb_hstd_bc_init_vb
Description     : BC State change function [INIT] [VbusOn]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_init_vb(void)
{
    g_usb_hstd_bc[g_usb_HcdPort].dcpmode = USB_BC_DCPMODE;
    if( g_usb_hstd_bc[g_usb_HcdPort].dcpmode )
    {
        g_usb_hstd_bc[g_usb_HcdPort].state = USB_BC_STATE_DCP;
        usb_hstd_bc_dcp_entry();
    }
    else
    {
        g_usb_hstd_bc[g_usb_HcdPort].state = USB_BC_STATE_DET;
        usb_hstd_bc_det_entry();
    }
}   /* eof usb_hstd_bc_init_vb() */


/******************************************************************************
Function Name   : usb_hstd_bc_det_at
Description     : BC State change function [DET] [Attach]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_det_at(void)
{
    usb_hstd_bc_det_exit();
    if( g_usb_hstd_bc[g_usb_HcdPort].pd_detect )
    {
        g_usb_hstd_bc[g_usb_HcdPort].state = USB_BC_STATE_CDP;
        usb_hstd_bc_cdp_entry();
    }
    else
    {
        g_usb_hstd_bc[g_usb_HcdPort].state = USB_BC_STATE_SDP;
        usb_hstd_bc_sdp_entry();
    }
}   /* eof usb_hstd_bc_det_at() */


/******************************************************************************
Function Name   : usb_hstd_bc_cdp_dt
Description     : BC State change function [CDP] [Detach]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_cdp_dt(void)
{
    usb_hstd_bc_cdp_exit();
    g_usb_hstd_bc[g_usb_HcdPort].state = USB_BC_STATE_DET;
    usb_hstd_bc_det_entry();
}   /* eof usb_hstd_bc_cdp_dt() */


/******************************************************************************
Function Name   : usb_hstd_bc_sdp_dt
Description     : BC State change function [SDP] [Detach]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_sdp_dt(void)
{
    usb_hstd_bc_sdp_exit();
    g_usb_hstd_bc[g_usb_HcdPort].state = USB_BC_STATE_DET;
    usb_hstd_bc_det_entry();
}   /* eof usb_hstd_bc_sdp_dt() */


/******************************************************************************
Function Name   : usb_hstd_bc_det_entry
Description     : BC State entry function [DET]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_det_entry(void)
{
    if( g_usb_HcdPort == USB_PORT0 )
    {
        USB_SET_PAT(USBBCCTRL0, USB_BATCHGE);
        USB_SET_PAT(USBBCCTRL0, USB_IDPSINKE);
        USB_CLR_STS(INTSTS1, USB_PDDETINT);
        USB_SET_PAT(INTENB1, USB_PDDETINTE);
    }
    else
    {
        USB_SET_PAT(USBBCCTRL1, USB_BATCHGE);
        USB_SET_PAT(USBBCCTRL1, USB_IDPSINKE);
        USB_CLR_STS(INTSTS2, USB_PDDETINT);
        USB_SET_PAT(INTENB2, USB_PDDETINTE);
    }
}   /* eof usb_hstd_bc_det_entry() */


/******************************************************************************
Function Name   : usb_hstd_bc_det_exit
Description     : BC State exit function [DET]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_det_exit(void)
{
    if( g_usb_HcdPort == USB_PORT0 )
    {
        USB_CLR_PAT(INTENB1, USB_PDDETINTE);
        USB_CLR_STS(INTSTS1, USB_PDDETINT);
        USB_CLR_PAT(USBBCCTRL0, USB_IDPSINKE);
        USB_CLR_PAT(USBBCCTRL0, USB_BATCHGE);
    }
    else
    {
        USB_CLR_PAT(INTENB2, USB_PDDETINTE);
        USB_CLR_STS(INTSTS2, USB_PDDETINT);
        USB_CLR_PAT(USBBCCTRL1, USB_IDPSINKE);
        USB_CLR_PAT(USBBCCTRL1, USB_BATCHGE);
    }
}   /* eof usb_hstd_bc_det_exit() */


/******************************************************************************
Function Name   : usb_hstd_bc_cdp_entry
Description     : BC State entry function [CDP]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_cdp_entry(void)
{
}   /* eof usb_hstd_bc_cdp_entry() */


/******************************************************************************
Function Name   : usb_hstd_bc_cdp_exit
Description     : BC State exit function [CDP]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_cdp_exit(void)
{
    g_usb_hstd_bc[g_usb_HcdPort].pd_detect = USB_BC_NODET;
}   /* eof usb_hstd_bc_cdp_exit() */


/******************************************************************************
Function Name   : usb_hstd_bc_sdp_entry
Description     : BC State entry function [SDP]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_sdp_entry(void)
{
}   /* eof usb_hstd_bc_sdp_entry() */


/******************************************************************************
Function Name   : usb_hstd_bc_sdp_exit
Description     : BC State exit function [SDP]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_sdp_exit(void)
{
    /* Nothing */
}   /* eof usb_hstd_bc_sdp_exit() */


/******************************************************************************
Function Name   : usb_hstd_bc_dcp_entry
Description     : BC State entry function [DCP]
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_bc_dcp_entry(void)
{
    if( g_usb_HcdPort == USB_PORT0 )
    {
        USB_CLR_PAT(SYSCFG, USB_DRPD);
        USB_SET_PAT(USBBCCTRL0, USB_BATCHGE);
        USB_SET_PAT(USBBCCTRL0, USB_DCPMODE);
    }
    else
    {
        USB_CLR_PAT(SYSCFG1, USB_DRPD);
        USB_SET_PAT(USBBCCTRL1, USB_BATCHGE);
        USB_SET_PAT(USBBCCTRL1, USB_DCPMODE);
    }
    USB_DEBUG_HOOK(USB_DEBUG_HOOK_HOST | USB_DEBUG_HOOK_CODE2);
}   /* eof usb_hstd_bc_dcp_entry() */
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */

#endif  /* USB_HOST_BC_ENABLE */

/******************************************************************************
End of file
******************************************************************************/
