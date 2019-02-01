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
* File Name    : r_usb_h2port.c
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
#include "r_usb_cdefusbip.h"            /* USB-FW Library Header */
#include "r_usb_usbip.h"                /* Definition of the USB register & bit define */
#include "r_usb_regaccess.h"            /* Definition of the USB register access macro */
#include "r_usb_hport.h"                /* Description : USB host driver public functions */


#if USB_PORTSEL_PP == USB_2PORT_PP
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

/* functiona */
USB_STATIC  void        usb_hstd_int_disable(void);
USB_STATIC  void        usb_hstd_detach_process(void);
USB_STATIC  void        usb_hstd_attach_process(void);
USB_STATIC  void        usb_hstd_bchg_process(uint16_t buf);
USB_STATIC  void        usb_hstd_init_connect(void);


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */

/* functiona */
#ifdef  __RX
void        usb_hstd_check_interrupt_source(usb_strct_t *inttype, uint16_t *bitsts);
#else   /* __RX */
void        usb_hstd_check_interrupt_source(usb_strct_t *inttype, usb_strct_t *bitsts);
#endif  /* __RX */
void        usb_hstd_interrupt_process(void);
void        usb_hstd_detach_control(void);
void        usb_hstd_read_lnst(uint16_t *buf);
void        usb_hstd_attach_control(void);
void        usb_hstd_bus_reset(void);
void        usb_hstd_resume_process(void);
void        usb_hstd_vbus_control_on(void);
void        usb_hstd_vbus_control_off(void);
void        usb_hstd_suspend_process(void);
void        usb_hstd_set_hw_function(void);
usb_strct_t usb_hstd_port_speed(void);


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* There is no orivate global variables and functions. */


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* Host control driver variables */
extern uint8_t          g_usb_HcdRemotePort[];
extern uint8_t          g_usb_HcdCtsq;                  /* Control transfer stage management */
extern uint16_t         g_usb_LibTemp;                  /* macro temp */
extern usb_pipe_t       g_usb_CurrentPipe;              /* Pipe number */
extern usb_addr_t       g_usb_HcdDevAddr;               /* Device address */

/* Host manager variables */
extern usb_tskinfo_t    *g_usb_HcdMessage;              /* Hcd Task receive message */
extern usb_port_t       g_usb_HcdPort;

/* functions */
extern void             usb_cpu_delay_xms(uint16_t time);
extern void             usb_cpu_delay_1us(uint16_t time);
extern void             usb_hstd_bemp_pipe(void);
extern void             usb_hstd_control_end(usb_strct_t Status);
extern uint8_t          usb_hstd_check_device_address(void);
extern void             usb_hstd_nrdy_pipe(void);
extern void             usb_hstd_brdy_pipe(void);
extern void             usb_hstd_set_devaddx_register(uint8_t usbspd, usb_port_t port);
extern uint8_t          usb_hstd_check_attach(void);
extern usb_addr_t       usb_hstd_pipe_2_addr(void);
extern void             usb_hstd_control_read_start(void);
extern uint16_t         usb_hstd_control_write_start(void);
extern void             usb_hstd_status_start(void);
extern void             usb_hstd_setup_disable(void);
extern void             usb_hstd_port_enable(void);
extern void             usb_hstd_status_notification(usb_strct_t msginfo, usb_strct_t keyword);
extern void             usb_cstd_forced_termination(uint16_t status);
extern void             usb_cstd_berne_enable(void);
extern void             usb_cstd_clr_pipe_config(void);
extern uint16_t         usb_cstd_get_pid(void);
extern void             usb_cstd_interrupt_disable(void);
extern void             usb_cstd_hw_stop(void);

#ifdef USB_HOST_BC_ENABLE
extern usb_bc_status_t g_usb_hstd_bc[2u];
extern void usb_hstd_pddetint_process(void);
extern void (*usb_hstd_bc_func[USB_BC_STATE_MAX][USB_BC_EVENT_MAX])(void);
#endif /* USB_HOST_BC_ENABLE */

/******************************************************************************
Renesas Abstracted Host Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_check_interrupt_source
Description     : USB interrupt Handler
Argument        : usb_strct_t  *inttype         : Interrupt type
                : usb_strct_t  *bitsts          : Interrupt status
Return          : none
******************************************************************************/
#ifdef  __RX
void usb_hstd_check_interrupt_source(usb_strct_t *inttype, uint16_t *bitsts)
#else   /* __RX */
void usb_hstd_check_interrupt_source(usb_strct_t *inttype, usb_strct_t *bitsts)
#endif  /* __RX */
{
    status_t    intsts0, intsts1, intsts2, brdysts, nrdysts, bempsts;
    status_t    intenb0, intenb1, intenb2, brdyenb, nrdyenb, bempenb;
    intbit_t    ists_u;
    intbit2_t   ists1_w, ists2_w;
#ifdef  __RX
    status_t    bsts_d,  nsts_d,  ests_d;
#else   /* __RX */
    pipests_t   bsts_d,  nsts_d,  ests_d;
#endif  /* __RX */

    uint16_t    stat;
    usb_strct_t key;

    /* Interrupt Control & Status Register Save */
    USB_RD(INTSTS0, intsts0.WORD.w);
    USB_RD(INTSTS1, intsts1.WORD.w);
    USB_RD(INTSTS2, intsts2.WORD.w);
    USB_RD(BRDYSTS, brdysts.WORD.w);
    USB_RD(NRDYSTS, nrdysts.WORD.w);
    USB_RD(BEMPSTS, bempsts.WORD.w);
    USB_RD(INTENB0, intenb0.WORD.w);
    USB_RD(INTENB1, intenb1.WORD.w);
    USB_RD(INTENB2, intenb2.WORD.w);
    USB_RD(BRDYENB, brdyenb.WORD.w);
    USB_RD(NRDYENB, nrdyenb.WORD.w);
    USB_RD(BEMPENB, bempenb.WORD.w);

    /* Interrupt status get */
    ists_u.BYTE.b       = (uint8_t)(intsts0.WORD.BYTE.up & intenb0.WORD.BYTE.up);
    ists1_w.WORD.w      = (uint16_t)(intsts1.WORD.w & intenb1.WORD.w);
    ists2_w.WORD.w      = (uint16_t)(intsts2.WORD.w & intenb2.WORD.w);
#ifdef  __RX
    bsts_d.WORD.w       = (uint16_t)(brdysts.WORD.w & brdyenb.WORD.w);
    nsts_d.WORD.w       = (uint16_t)(nrdysts.WORD.w & nrdyenb.WORD.w);
    ests_d.WORD.w       = (uint16_t)(bempsts.WORD.w & bempenb.WORD.w);
#else   /* __RX */
    bsts_d.BYTE.b       = (uint8_t)(brdysts.WORD.BYTE.dn & brdyenb.WORD.BYTE.dn);
    nsts_d.BYTE.b       = (uint8_t)(nrdysts.WORD.BYTE.dn & nrdyenb.WORD.BYTE.dn);
    ests_d.BYTE.b       = (uint8_t)(bempsts.WORD.BYTE.dn & bempenb.WORD.BYTE.dn);
#endif  /* __RX */

    key     = USB_INT_UNKNOWN;
    stat    = USB_0;

    /***** Setup transaction processing *****/
    if( ists1_w.WORD.BIT.SACK )                 /***** Setup ACK *****/
    {
        usb_hstd_setup_disable();
        key     = USB_INT_SACK;
    }
    else if( ists1_w.WORD.BIT.SIGN )            /***** Setup Ignore *****/
    {
        usb_hstd_setup_disable();
        key     = USB_INT_SIGN;
    }

    /***** PIPE0-MAX_PIPE_NO data processing *****/
    else if( ists_u.BYTE.BIT.BRDY )             /***** EP0-7 BRDY *****/
    {
#ifdef  __RX
        stat    = bsts_d.WORD.w;
#else   /* __RX */
        stat    = bsts_d.BYTE.b;
#endif  /* __RX */
        USB_CLR_STS(BRDYSTS, stat);
        key     = USB_INT_BRDY;
    }
    else if( ists_u.BYTE.BIT.BEMP )             /***** EP0-7 BEMP *****/
    {
#ifdef  __RX
        stat    = ests_d.WORD.w;
#else   /* __RX */
        stat    = ests_d.BYTE.b;
#endif  /* __RX */
        USB_CLR_STS(BEMPSTS, stat);
        key     = USB_INT_BEMP;
    }
    else if( ists_u.BYTE.BIT.NRDY )             /***** EP0-7 NRDY *****/
    {
#ifdef  __RX
        stat    = nsts_d.WORD.w;
#else   /* __RX */
        stat    = nsts_d.BYTE.b;
#endif  /* __RX */
        USB_CLR_STS(NRDYSTS, stat);
        key     = USB_INT_NRDY;
    }

    /***** Port0 signal processing *****/
    else if( ists1_w.WORD.BIT.OVRCR )           /***** OVER CURRENT *****/
    {
        usb_hstd_ovrcr_clear_status_p0();
        key     = USB_INT_OVRCR0;
    }
    else if( ists1_w.WORD.BIT.ATTCH )           /***** ATTCH INT *****/
    {
        /* DTCH  interrupt disable */
        usb_hstd_bus_int_disable_p0();
        key     = USB_INT_ATTCH0;
    }
    else if( ists1_w.WORD.BIT.EOFERR )          /***** EOFERR INT *****/
    {
        USB_CLR_STS(INTSTS1, USB_EOFERR);
        key     = USB_INT_EOFERR0;
    }
    else if( ists1_w.WORD.BIT.BCHG )            /***** BCHG INT *****/
    {
        usb_hstd_bchg_disable_p0();
        key     = USB_INT_BCHG0;
    }
    else if( ists1_w.WORD.BIT.DTCH )            /***** DETACH INT *****/
    {
        /* DTCH  interrupt disable */
        usb_hstd_bus_int_disable_p0();
        key     = USB_INT_DTCH0;
    }
#ifdef USB_HOST_BC_ENABLE
    else if( ists1_w.WORD.BIT.PDDETINT )        /***** PDDETINT INT *****/
    {
        USB_CLR_STS(INTSTS1, USB_PDDETINT);
        key = USB_INT_PDDETINT0;
    }
#endif /* USB_HOST_BC_ENABLE */

    /***** Port1 signal processing *****/
    else if( ists2_w.WORD.BIT.OVRCR )           /***** OVER CURRENT *****/
    {
        usb_hstd_ovrcr_clear_status_p1();
        key = USB_INT_OVRCR1;
    }
    else if( ists2_w.WORD.BIT.ATTCH )           /***** ATTCH INT *****/
    {
        usb_hstd_bus_int_disable_p1();
        key = USB_INT_ATTCH1;
    }
    else if( ists2_w.WORD.BIT.EOFERR )          /***** EOFERR INT *****/
    {
        USB_CLR_STS(INTSTS2, USB_EOFERR);
        key = USB_INT_EOFERR1;
    }
    else if( ists2_w.WORD.BIT.BCHG )            /***** BCHG INT *****/
    {
        usb_hstd_bchg_disable_p1();
        key = USB_INT_BCHG1;
    }
    else if( ists2_w.WORD.BIT.DTCH )            /***** DETACH INT *****/
    {
        usb_hstd_bus_int_disable_p1();
        key = USB_INT_DTCH1;
    }
#ifdef USB_HOST_BC_ENABLE
    else if( ists2_w.WORD.BIT.PDDETINT )        /***** PDDETINT INT *****/
    {
        USB_CLR_STS(INTSTS2, USB_PDDETINT);
        key = USB_INT_PDDETINT1;
    }
#endif /* USB_HOST_BC_ENABLE */

    /***** VBUS/SOF signal processing *****/
    else if( ists_u.BYTE.BIT.VBINT )            /***** VBUS change *****/
    {
        USB_CLR_STS(INTSTS0, USB_VBINT);
        key = USB_INT_VBINT;
    }
    else if( ists_u.BYTE.BIT.SOFR )             /***** SOFR change *****/
    {
        USB_CLR_STS(INTSTS0, USB_SOFR);
        key = USB_INT_SOFR;
    }
    else
    {
        /* no processing */
    }
#ifdef  __RX
    *bitsts     = stat;
#else   /* __RX */
    *bitsts     = (usb_strct_t)stat;
#endif  /* __RX */
    *inttype    = key;
}   /* eof usb_hstd_check_interrupt_source() */


/******************************************************************************
Function Name   : usb_hstd_interrupt_process
Description     : USB interrupt process
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_interrupt_process(void)
{
    uint16_t        inttype;
    uint16_t        buf;

    inttype = g_usb_HcdMessage->keyword;        /* p->inttype; */

    /***** common processing *****/
    switch( inttype )
    {
        /***** PIPE0-MAX_PIPE_NO data processing *****/
        case USB_INT_BRDY:
            usb_hstd_brdy_pipe();
        break;
        case USB_INT_BEMP:
            usb_hstd_bemp_pipe();
        break;
        case USB_INT_NRDY:
            usb_hstd_nrdy_pipe();
        break;

        /***** Setup transaction processing *****/
        case USB_INT_SACK:
            switch( g_usb_HcdCtsq )
            {
                case USB_SETUPRD:
                    /* Next stage to Control read data */
                    usb_hstd_control_read_start();
                break;
                case USB_SETUPWR:
                    /* Next stage to Control Write data */
                    if( usb_hstd_control_write_start() == USB_FIFOERROR )
                    {
                        /* Control Read/Write End */
                        usb_hstd_control_end(USB_DATA_ERR);
                    }
                break;
                case USB_SETUPNDC:
                    /* Next stage to Control write no data */
                    usb_hstd_status_start();
                break;
                default:
                    /* no processing */
                break;
            }
        break;
        case USB_INT_SIGN:
            /* Setup Device Ignore count over */
            usb_hstd_control_end(USB_DATA_ERR);
        break;

        /***** VBUS/SOF signal processing *****/
        case USB_INT_VBINT:
            /* User program */
            USB_CLR_PAT(INTENB0, USB_VBSE);
        break;
        case USB_INT_SOFR:
            /* User program */
            USB_CLR_PAT(INTENB0, USB_SOFE);
        break;
        /*** ERROR ***/
        case USB_INT_UNKNOWN:
        /* continue */
        default:
            /* no processing */
        break;
    }

    /***** Port0 signal processing *****/
    g_usb_HcdPort = USB_PORT0;
    switch( inttype )
    {
        case USB_INT_OVRCR0:
            /* Port0 OVCR interrupt function */
            USB_RD(SYSSTS0, buf);
            if( (uint16_t)(buf & USB_OVCBIT) == USB_0 )
            {
                /* OVRCR interrupt disable */
                usb_hstd_ovrcr_disable_p0();
                usb_hstd_vbus_control_off_p0();
                usb_hstd_status_notification(USB_RTP_OVERCURRENT, USB_PORT0);
            }
        break;
        case USB_INT_EOFERR0:
            /* User program */
        break;
        case USB_INT_ATTCH0:
            /* Port0 ATCH interrupt function */
            usb_hstd_attach_process();
        break;
        case USB_INT_BCHG0:
            /* Device State Control Register - Resume enable check */
            USB_RD(DVSTCTR0, buf);
            usb_hstd_bchg_process(buf);
        break;
        case USB_INT_DTCH0:
            /* Port0 DTCH interrupt function */
            usb_hstd_detach_process();
        break;
#ifdef USB_HOST_BC_ENABLE
        case USB_INT_PDDETINT0:
            /* Port0 PDDETINT interrupt function */
            usb_hstd_pddetint_process();
        break;
#endif /* USB_HOST_BC_ENABLE */
        default:
            /* no processing */
        break;
    }

    /***** Port1 signal processing *****/
    g_usb_HcdPort = USB_PORT1;
    switch( inttype )
    {
        case USB_INT_OVRCR1:
            USB_RD(SYSSTS1, buf);
            if( (uint16_t)(buf&USB_OVCBIT) == 0 )
            {
                /* OVRCR interrupt disable */
                usb_hstd_ovrcr_disable_p1();
                usb_hstd_vbus_control_off_p1();
                usb_hstd_status_notification(USB_RTP_OVERCURRENT, USB_PORT1);
            }
        break;
        case USB_INT_EOFERR1:
            /* User program */
        break;
        case USB_INT_ATTCH1:
            /* USB attach / detach */
            usb_hstd_attach_process();
        break;
        case USB_INT_BCHG1:
            /* Device State Control Register - Resume enable check */
            USB_RD(DVSTCTR1, buf);
            usb_hstd_bchg_process(buf);
        break;
        case USB_INT_DTCH1:
            /* USB detach process */
            usb_hstd_detach_process();
        break;
#ifdef USB_HOST_BC_ENABLE
        case USB_INT_PDDETINT1:
            /* Port1 PDDETINT interrupt function */
            usb_hstd_pddetint_process();
        break;
#endif /* USB_HOST_BC_ENABLE */

        default:
            /* no processing */
        break;
    }

}   /* eof usb_hstd_interrupt_process() */


/******************************************************************************
Function Name   : usb_hstd_int_disable
Description     : OVRCR, ATTCH, DTCH, BCHG interrupt disable
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_hstd_int_disable(void)
{
    if (g_usb_HcdPort == USB_PORT0)
    {
        usb_hstd_ovrcr_disable_p0();
        usb_hstd_attch_disable_p0();
        usb_hstd_detach_disable_p0();
    }
    else
    {
        usb_hstd_ovrcr_disable_p1();
        usb_hstd_attch_disable_p1();
        usb_hstd_detach_disable_p1();
    }
    usb_hstd_bchg_disable_p0();
    usb_hstd_bchg_disable_p1();
}   /* eof usb_hstd_int_disable() */


/******************************************************************************
Function Name   : usb_hstd_detach_control
Description     : USB detach
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_detach_control(void)
{
#ifdef USB_HOST_BC_ENABLE
    usb_hstd_bc_func[g_usb_hstd_bc[g_usb_HcdPort].state][USB_BC_EVENT_DT]();
#endif /* USB_HOST_BC_ENABLE */

    if (g_usb_HcdPort == USB_PORT0)
    {
        /* ATTCH interrupt enable */
        USB_CLR_PAT(DVSTCTR0, (uint16_t)(USB_RWUPE | USB_USBRST | USB_RESUME | USB_UACT));
        usb_hstd_attch_enable_p0();
        usb_cpu_delay_xms((uint16_t)1);
        usb_hstd_bchg_enable_p0();
    }
    else
    {
        /* ATTCH interrupt enable */
        USB_CLR_PAT(DVSTCTR1, (uint16_t)(USB_RWUPE | USB_USBRST | USB_RESUME | USB_UACT));
        usb_hstd_attch_enable_p1();
        usb_cpu_delay_xms((uint16_t)1);
        usb_hstd_bchg_enable_p1();
    }
    usb_hstd_status_notification(USB_RTP_DETACH, (usb_strct_t)g_usb_HcdPort);
}   /* eof usb_hstd_detach_control() */


/******************************************************************************
Function Name   : usb_hstd_detach_process
Description     : USB detach process
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_hstd_detach_process(void)
{
    /* OVRCR, ATTCH, DTCH, BCHG interrupt disable */
    usb_hstd_int_disable();

    for( g_usb_HcdDevAddr = USB_DEVICE_0; g_usb_HcdDevAddr <= USB_MAXDEVADDR; g_usb_HcdDevAddr++ )
    {
        if( usb_hstd_check_device_address() != USB_NOCONNECT )
        {
            if( g_usb_HcdCtsq != USB_IDLEST )
            {
                /* Control Read/Write End */
                usb_hstd_setup_disable();
                usb_hstd_control_end(USB_DATA_DTCH);
            }
            for( g_usb_CurrentPipe = USB_MIN_PIPE_NO; g_usb_CurrentPipe <= USB_MAX_PIPE_NO; g_usb_CurrentPipe++ )
            {
                /* Not control transfer */
                if( usb_hstd_pipe_2_addr() == g_usb_HcdDevAddr )
                {
                    /* Agreement device address */
                    if( usb_cstd_get_pid() == USB_PID_BUF )
                    {
                        /* End of data transfer (IN/OUT) */
                        usb_cstd_forced_termination(USB_DATA_DTCH);
                    }
                    usb_cstd_clr_pipe_config();
                }
            }
            usb_hstd_set_devaddx_register(USB_NOCONNECT, g_usb_HcdPort);
        }
    }

    /* Decide USB Line state (ATTACH) */
    if( usb_hstd_check_attach() == USB_LNST_ATTACH )
    {
        usb_hstd_status_notification(USB_RTP_DETACH, (usb_strct_t)g_usb_HcdPort);
        usb_hstd_attach_control();
    }
    else
    {
        usb_hstd_detach_control();
    }
}   /* eof usb_hstd_detach_process() */


/******************************************************************************
Function Name   : usb_hstd_read_lnst
Description     : Check Atacch
Arguments       : uint16_t *buf             : RHST bit(return value)
Return value    : none
Note            : Please change for your SYSTEM
******************************************************************************/
void usb_hstd_read_lnst(uint16_t *buf)
{
    if( g_usb_HcdPort == USB_PORT0 )
    {
        do
        {
            USB_RD(SYSSTS0, buf[0]);
            /* 30ms wait */
            usb_cpu_delay_xms((uint16_t)30);
            USB_RD(SYSSTS0, buf[1]);
            if( (buf[0] & USB_LNST) == (buf[1] & USB_LNST) )
            {
                /* 20ms wait */
                usb_cpu_delay_xms((uint16_t)20);
                USB_RD(SYSSTS0, buf[1]);
            }
        }
        while( (buf[0] & USB_LNST) != (buf[1] & USB_LNST) );
        USB_RD(DVSTCTR0, buf[1]);
    }
    else
    {
        do
        {
            USB_RD(SYSSTS1, buf[0]);
            /* 30ms wait */
            usb_cpu_delay_xms((uint16_t)30);
            USB_RD(SYSSTS1, buf[1]);
            if( (buf[0] & USB_LNST) == (buf[1] & USB_LNST) )
            {
                /* 20ms wait */
                usb_cpu_delay_xms((uint16_t)20);
                USB_RD(SYSSTS1, buf[1]);
            }
        }
        while( (buf[0] & USB_LNST) != (buf[1] & USB_LNST) );
        USB_RD(DVSTCTR1, buf[1]);
    }
}   /* eof usb_hstd_read_lnst() */


/******************************************************************************
Function Name   : usb_hstd_attach_control
Description     : USB attach
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_attach_control(void)
{
    /* OVRCR, DTCH interrupt Enable */
    if (g_usb_HcdPort == USB_PORT0)
    {
        usb_hstd_detach_enable_p0();
    }
    else
    {
        usb_hstd_detach_enable_p1();
    }
    usb_cstd_berne_enable();
    usb_hstd_status_notification(USB_RTP_ATTACH, (usb_strct_t)g_usb_HcdPort);

#ifdef USB_HOST_BC_ENABLE
    usb_hstd_bc_func[g_usb_hstd_bc[g_usb_HcdPort].state][USB_BC_EVENT_AT]();
#endif /* USB_HOST_BC_ENABLE */
}   /* eof usb_hstd_attach_control() */


/******************************************************************************
Function Name   : usb_hstd_attach_process
Description     : USB attach / detach
Arguments       : none
Return value    : none
Note            : Please change for your SYSTEM
******************************************************************************/
USB_STATIC void usb_hstd_attach_process(void)
{

    /* OVRCR, ATTCH, DTCH, BCHG interrupt disable */
    usb_hstd_int_disable();

    /* Decide USB Line state (ATTACH) */
    if( usb_hstd_check_attach() == USB_LNST_DETACH )
    {
        usb_hstd_detach_control();
    }
    else
    {
        usb_hstd_attach_control();
    }
}   /* eof usb_hstd_attach_process() */


/******************************************************************************
Function Name   : usb_hstd_bchg_process
Description     : USB D+/D- line change
Arguments       : uint16_t buf
Return value    : none
******************************************************************************/
USB_STATIC void usb_hstd_bchg_process(uint16_t buf)
{
    /* Check remote wakeup enabled */
    if ((g_usb_HcdRemotePort[g_usb_HcdPort] == USB_YES) && ((uint16_t)(buf & USB_RESUME) == USB_RESUME))
    {
        /* Change device state to resume */
        g_usb_HcdRemotePort[g_usb_HcdPort] = USB_NO;
        usb_hstd_status_notification(USB_RTP_REMOTEWAKEUP, g_usb_HcdPort);
    }
    else
    {
        /* Decide USB Line-State (ATTACH) */
        switch(usb_hstd_check_attach())
        {
            case USB_LNST_DETACH:
                g_usb_HcdRemotePort[g_usb_HcdPort] = USB_NO;
                usb_hstd_detach_control();
            break;
            case USB_LNST_ATTACH:
                g_usb_HcdRemotePort[g_usb_HcdPort] = USB_NO;
                usb_hstd_attach_control();
            break;
            case USB_RHST_ATTACH:
                if( g_usb_HcdPort == USB_PORT0 )
                {
                    usb_hstd_bchg_enable_p0();
                }
                else
                {
                    usb_hstd_bchg_enable_p1();
                }
            break;
            default:
            break;
        }
    }
}   /* eof usb_hstd_bchg_process() */


/******************************************************************************
Function Name   : usb_hstd_bus_reset
Description     : USB bus reset
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_bus_reset(void)
{
    /* Cautions!!!
     * The DTCH interrupt is detected during the recovery time after the SE0 drive ends.
     * Therefore, even if the DTCH interruption and the ATTCH interrupt are generated while driving SE0,
     * connected detection of the device cannot be done.
     * In this case, the sequence since the USB reset processing is continued
     * as generation of neither cutting nor the reconnection.
     */

    /* SE0 drivet & wait 50ms(reset signal), SOF output & wait 20ms(reset recovery) */
    if( g_usb_HcdPort == USB_PORT0 )
    {
        USB_MDF_PAT(DVSTCTR0, USB_USBRST, (USB_USBRST | USB_UACT));
        usb_cpu_delay_xms((uint16_t)50);
        usb_hstd_set_uact_p0();
    }
    else
    {
        USB_MDF_PAT(DVSTCTR1, USB_USBRST, (USB_USBRST | USB_UACT));
        usb_cpu_delay_xms((uint16_t)50);
        usb_hstd_set_uact_p1();
    }
    /* Wait 10ms or more (USB reset recovery) */
    USB_WR(DCPMAXP,(uint16_t)(USB_DEFPACKET + USB_DEVICE_0));
    usb_cpu_delay_xms((uint16_t)20);
}   /* eof usb_hstd_bus_reset() */


/******************************************************************************
Function Name   : usb_hstd_resume_process
Description     : USB resume
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_resume_process(void)
{
    /* RESUME signal output & wait 20ms, SOF output & wait 5ms */
    if( g_usb_HcdPort == USB_PORT0 )
    {
        usb_hstd_bchg_disable_p0();
        USB_MDF_PAT(DVSTCTR0, USB_RESUME, (USB_RESUME | USB_RWUPE));
        usb_cpu_delay_xms((uint16_t)20);
        usb_hstd_set_uact_p0();
    }
    else
    {
        usb_hstd_bchg_disable_p1();
        USB_MDF_PAT(DVSTCTR1, USB_RESUME, (USB_RESUME | USB_RWUPE));
        usb_cpu_delay_xms((uint16_t)20);
        usb_hstd_set_uact_p1();
    }
    usb_cpu_delay_xms((uint16_t)5);
}   /* eof usb_hstd_resume_process() */


/******************************************************************************
Function Name   : usb_hstd_vbus_control_on
Description     : USB VBUS control
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_vbus_control_on(void)
{
    if (g_usb_HcdPort == USB_PORT0)
    {
        usb_hstd_vbus_control_on_p0();
    }
    else
    {
        usb_hstd_vbus_control_on_p1();
    }

#ifdef USB_HOST_BC_ENABLE
        usb_hstd_bc_func[g_usb_hstd_bc[g_usb_HcdPort].state][USB_BC_EVENT_VB]();
#endif /* USB_HOST_BC_ENABLE */
}   /* eof usb_hstd_vbus_control_on() */


/******************************************************************************
Function Name   : usb_hstd_vbus_control_off
Description     : USB VBUS control
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_vbus_control_off(void)
{
    if (g_usb_HcdPort == USB_PORT0)
    {
        usb_hstd_vbus_control_off_p0();
    }
    else
    {
        usb_hstd_vbus_control_off_p1();
    }
}   /* eof usb_hstd_vbus_control_off() */


/******************************************************************************
Function Name   : usb_hstd_suspend_process
Description     : USB suspend process
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_suspend_process(void)
{
    /* SOF stop & SOF check & enable remote wakeup & enable BCHG interrupt */
    if( g_usb_HcdRemotePort[g_usb_HcdPort] == USB_YES )
    {
        /* Suspend state of remote wake up permission */
        if( g_usb_HcdPort == USB_PORT0 )
        {
            USB_CLR_PAT(DVSTCTR0, USB_UACT);
            usb_cpu_delay_xms((uint16_t)1);
            USB_SET_PAT(DVSTCTR0, USB_RWUPE);
            usb_hstd_bchg_enable_p0();
        }
        else
        {
            USB_CLR_PAT(DVSTCTR1, USB_UACT);
            usb_cpu_delay_xms((uint16_t)1);
            USB_SET_PAT(DVSTCTR1, USB_RWUPE);
            usb_hstd_bchg_enable_p1();
        }
    }
    else
    {
        /* Suspend state of remote wake up prohibition */
        if( g_usb_HcdPort == USB_PORT0 )
        {
            USB_CLR_PAT(DVSTCTR0, USB_UACT);
        }
        else
        {
            USB_CLR_PAT(DVSTCTR1, USB_UACT);
        }
    }
    /* 5ms should continue even if the suspend state is the lowest. */
    usb_cpu_delay_xms((uint16_t)5);
}   /* eof usb_hstd_suspend_process() */


/******************************************************************************
Function Name   : usb_hstd_set_hw_function
Description     : Select HW function
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_set_hw_function(void)
{
    /* HOST mode, Interrupt Enable */
    USB_SET_PAT(SYSCFG, (USB_DCFM | USB_DRPD));
    USB_SET_PAT(SYSCFG1, USB_DRPD);
    usb_cstd_berne_enable();
    usb_hstd_ovrcr_enable_p0();
    usb_hstd_ovrcr_enable_p1();
    usb_cpu_delay_1us((uint16_t)10);
    usb_hstd_init_connect();
}   /* eof usb_hstd_set_hw_function() */


/******************************************************************************
Function Name   : usb_hstd_init_connect
Description     : Check connection
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_hstd_init_connect(void)
{
    usb_hstd_attch_clear_status_p0();
    usb_hstd_detch_clear_status_p0();
    g_usb_HcdPort   = USB_PORT0;
    usb_hstd_port_enable();

    usb_hstd_attch_clear_status_p1();
    usb_hstd_detch_clear_status_p1();
    g_usb_HcdPort   = USB_PORT1;
    usb_hstd_port_enable();
}   /* eof usb_hstd_init_connect() */


/******************************************************************************
Function Name   : usb_hstd_port_speed
Description     : Check current port speed
Arguments       : none
Return value    : usb_strct_t       ; FSCONNECT : Full-Speed
                :                   ; LSCONNECT : Low-Speed
                :                   ; NOCONNECT : not connect
******************************************************************************/
usb_strct_t usb_hstd_port_speed(void)
{
    uint16_t    buf;

    /* Reset handshake status get */
    USB_RD(DVSTCTR1, buf);
    if( g_usb_HcdPort == USB_PORT0 )
    {
        USB_RD(DVSTCTR0, buf);
    }
    buf = (uint16_t)(buf & USB_RHST);

    /* Check RHST */
    if (buf == USB_FSMODE)
    {
        return  (USB_FSCONNECT);
    }
    if (buf == USB_LSMODE)
    {
        return  (USB_LSCONNECT);
    }
    return  (USB_NOCONNECT);
}   /* eof usb_hstd_port_speed() */
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */


/******************************************************************************
End of file
******************************************************************************/
