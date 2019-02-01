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
* File Name    : r_usb_pdriver.c
* Version      : 2.10
* Device(s)    : Renesas R8C/USB-Series, RL78/USB-Series
* Tool-Chain   : Renesas M16C Standard Toolchain (rev6.0.0 ), CA78K0R V1.60
* OS           : Not use
* H/W Platform : Independent
* Description  : This is the USB peripheral control driver code.
* Operation    : 1. 
* Limitations  : 
* xxx          : 
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 16.02.2011 1.00    Mass Production Release
*           : 01.03.2011 1.01    Version 1.01 Release(Bug fixed)
*           : 31.03.2012 1.80    Basic FW mini Ver2.00(ES Version)
*           : 30.11.2012 2.00    Basic FW mini Ver2.00 Release
*           : 31.01.2013 2.01    Corresponded to RL78/L1C
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/

/******************************************************************************
Section definitions
******************************************************************************/
#ifdef  R8CUSB
#pragma section program P_pcd
#pragma section bss B_pcd
#endif
#ifdef  RL78USB
#endif
#ifdef  RL78USB_SECTION
#pragma section @@CODE   P_PCD
#pragma section @@BASE   T_PCD
#pragma section @@DATA   B_PCD
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
#include "r_usb_api.h"                  /* USB API public function header */
#if DEBUG_CONSOLE
    #include "string.h"
    #include <stdio.h>
#endif

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
USB_STATIC  usb_tskinfo_t   *g_usb_PcdMessage;                          /* Pcd Task receive message */
USB_STATIC  usb_request_t   g_usb_PcdRequest;                           /* Device Request - Request structure */
USB_STATIC  uint8_t         g_usb_PcdRequestData[USB_2];                /* Control transfer data */

/* functions */
/* PCD task */
USB_STATIC  void            usb_pstd_transfer_start(void);
USB_STATIC  void            usb_pstd_interrupt_process(void);
USB_STATIC  void            usb_pstd_save_request(void);
USB_STATIC  uint8_t         usb_pstd_chk_remote(void);
#ifdef  __RX
USB_STATIC  void            usb_pstd_check_interrupt_source(usb_strct_t *inttype, uint16_t *bitsts);
#else   /* __RX */
USB_STATIC  void            usb_pstd_check_interrupt_source(usb_strct_t *inttype, usb_strct_t *bitsts);
#endif  /* __RX */

/* USB standard request */
USB_STATIC  void            usb_pstd_stand_req1(void);
USB_STATIC  void            usb_pstd_stand_req2(void);
USB_STATIC  void            usb_pstd_stand_req3(void);
USB_STATIC  void            usb_pstd_stand_req4(void);
USB_STATIC  void            usb_pstd_stand_req5(void);
USB_STATIC  void            usb_pstd_stand_req0(void);
USB_STATIC  void            usb_pstd_clear_feature3(void);
USB_STATIC  void            usb_pstd_clear_feature0(void);
USB_STATIC  void            usb_pstd_get_configuration1(void);
USB_STATIC  void            usb_pstd_get_descriptor1(void);
USB_STATIC  void            usb_pstd_get_interface1(void);
USB_STATIC  void            usb_pstd_get_status1(void);
USB_STATIC  void            usb_pstd_set_address3(void);
USB_STATIC  void            usb_pstd_set_address0(void);
USB_STATIC  void            usb_pstd_set_configuration3(void);
USB_STATIC  void            usb_pstd_set_configuration0(void);
USB_STATIC  void            usb_pstd_set_feature3(void);
USB_STATIC  void            usb_pstd_set_feature0(void);
USB_STATIC  void            usb_pstd_set_interface3(void);
USB_STATIC  void            usb_pstd_set_interface0(void);
/* FIFO buffer interrupt */
USB_STATIC  void            usb_pstd_brdy_pipe(uint16_t bitsts);
USB_STATIC  void            usb_pstd_nrdy_pipe(uint16_t bitsts);
USB_STATIC  void            usb_pstd_bemp_pipe(uint16_t bitsts);
/* Device state control function */
USB_STATIC  void            usb_pstd_bus_reset(void);
USB_STATIC  void            usb_pstd_attach_process(void);
USB_STATIC  void            usb_pstd_detach_process(void);
USB_STATIC  void            usb_pstd_suspend_process(void);
USB_STATIC  void            usb_pstd_resume_process(void);
USB_STATIC  void            usb_pstd_chk_vbsts(void);
/* other library functions */
USB_STATIC  void            usb_pstd_epadr_2_pipe(void);
USB_STATIC  void            usb_pstd_set_pipe_registration(uint16_t command);


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */
usb_pcdreg_t    g_usb_PcdDriver;                            /* Driver registration */
uint16_t        g_usb_Pcdintsts0;                           /* INTSTS0 */
uint8_t         g_usb_PcdConfigNum;                         /* Configuration number */
uint8_t         g_usb_PcdRemoteWakeup;                      /* Remote wakeup enable flag */

/* functions */
/* PCD task */
void            usb_pstd_pcd_task(void);
void            usb_pstd_interrupt_handler(void);
uint8_t         usb_pstd_get_interface_num(void);
uint8_t         usb_pstd_chk_configured(void);
/* Device state control function */
void            usb_pstd_remote_wakeup(void);
void            usb_pstd_init_connect(void);
void            usb_pstd_port_enable(void);
void            usb_pstd_port_disable(void);
/* other library functions */
void            usb_pstd_set_ccpl(void);


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* variables */
extern usb_utr_t        *g_usb_LibPipe[];               /* Message pipe */
extern uint16_t         g_usb_LibTemp;                  /* USB register access macro temp */
extern usb_pipe_t       g_usb_CurrentPipe;              /* Pipe number */
extern usb_intinfo_t    g_usb_LibIntMessage[];
extern uint8_t          g_usb_LibIntMsgCnt;
#ifdef USB_PERI_BC_ENABLE
extern uint16_t         g_usb_bc_detect;
#endif

/* functions */
extern void             usb_cstd_send_start(void);
extern uint8_t          usb_cstd_buf_2_cfifo(void);
extern void             usb_cstd_receive_start(void);
extern uint8_t          usb_cstd_cfifo_2_buf(void);
extern void             usb_cstd_forced_termination(uint16_t status);
extern void             usb_cstd_brdy_pipe(uint16_t bitsts);
extern void             usb_cstd_bemp_pipe(uint16_t bitsts);
extern void             usb_cstd_brdy_disable(void);
extern void             usb_cstd_bemp_disable(void);
extern uint16_t         usb_cstd_get_pipe_dir(void);
extern void             usb_cstd_clr_pipe_config(void);
extern void             usb_cstd_reg_set_pipexctr(uint16_t field);
extern uint16_t         usb_cstd_set_nak(void);
extern void             usb_cstd_clr_stall(void);
extern uint16_t         usb_cstd_get_pid(void);
extern void             usb_cstd_set_pipe_registration(uint16_t *table, uint16_t command);
extern void             usb_cpu_delay_xms(uint16_t time);
#ifdef USB_PERI_BC_ENABLE
extern void             usb_pstd_bc_detect_process(void);
#endif

#ifdef __RX
extern void             usb_cpu_d0fifo_interrupt_process();
#endif  /* __RX */


/******************************************************************************
Renesas Abstracted USB Peripheral Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_pcd_task
Description     : USB Peripheral control driver Task
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_pcd_task(void)
{
    usb_tskinfo_t   *mess;
    
    if (R_USB_RCV_MSG(USB_PCD_MBX, (usb_msg_t**)&mess) != USB_E_OK)
    {
        return;
    }

    g_usb_PcdMessage    = (usb_tskinfo_t*)mess;
    g_usb_CurrentPipe   = (usb_pipe_t)g_usb_PcdMessage->keyword;

    /* Branch Pcd Task receive Message Command */
    switch( g_usb_PcdMessage->msginfo )
    {

        /* USB interrupt */
        case USB_INTERRUPT:
            usb_pstd_interrupt_process();
        break;

#ifdef __RX
        /* USB D0FIFO interrupt */
        case USB_DO_D0FIFO_INTERRUPT:
            usb_cpu_d0fifo_interrupt_process();
        break;
#endif  /* __RX */

        /* USB transfer start */
        case USB_DO_TRANSFERSTART:
            usb_pstd_transfer_start();
        break;

        /* USB Transfer End Stop or Timeout */
        case USB_DO_TRANSFER_STP:
            usb_cstd_forced_termination(USB_DATA_STOP);
            R_USB_REL_BLK(g_usb_PcdMessage->flag);
        break;
        case USB_DO_TRANSFER_TMO:
            usb_cstd_forced_termination(USB_DATA_TMO);
            R_USB_REL_BLK(g_usb_PcdMessage->flag);
        break;

        default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_PERI | USB_DEBUG_HOOK_CODE1);
        break;
    }

}   /* eof usb_pstd_pcd_task() */


/******************************************************************************
Function Name   : usb_pstd_transfer_start
Description     : USB data transfer start
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_transfer_start(void)
{
    g_usb_CurrentPipe                   = (usb_pipe_t)g_usb_PcdMessage->keyword;
    g_usb_LibPipe[g_usb_CurrentPipe]    = (usb_utr_t*)g_usb_PcdMessage;

    if( usb_pstd_chk_configured() == USB_YES )
    {
        if( usb_cstd_get_pipe_dir() == USB_DIR_P_OUT )
        {   /* Out transfer */
            usb_cstd_receive_start();
        }
        else
        {
            /* In transfer */
            usb_cstd_send_start();
        }
    }
    else
    {
        /* Transfer stop */
        usb_cstd_forced_termination(USB_DATA_DTCH);
    }
}   /* eof usb_pstd_transfer_start() */


/******************************************************************************
Function Name   : usb_pstd_interrupt_process
Description     : USB interrupt process
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_interrupt_process(void)
{
    uint8_t     intseq;
    uint16_t    intdata;

    intdata     = g_usb_PcdMessage->status;         /* p->bitsts; */
    USB_RD(INTSTS0, g_usb_Pcdintsts0);

    switch( g_usb_PcdMessage->keyword )             /* p->inttype; */
    {
        case USB_INT_BRDY:              /* FIFO buffer interrupt BRDY, BEMP, NRDY */
            usb_pstd_brdy_pipe(intdata);
        break;
        case USB_INT_BEMP:
            usb_pstd_bemp_pipe(intdata);
        break;
        case USB_INT_NRDY:
            usb_pstd_nrdy_pipe(intdata);
        break;

        case USB_INT_RESM:              /* Resume */
            usb_pstd_resume_process();
            (*g_usb_PcdDriver.statediagram)(USB_NO_ARG, USB_STS_RESUME);
        break;
        case USB_INT_VBINT:             /* VBUS */
            usb_pstd_chk_vbsts();
        break;
        case USB_INT_SOFR:              /* SOF */
            /* User program */
        break;

        case USB_INT_DVST:              /* DVST */
            switch( (uint8_t)(intdata & USB_DVSQ) )
            {
                case USB_DS_POWR:                       /* Power state */
                break;
                case USB_DS_DFLT:                       /* Default state */
                    usb_pstd_bus_reset();
                    (*g_usb_PcdDriver.statediagram)(USB_NO_ARG, USB_STS_DEFAULT);
                break;
                case USB_DS_ADDS:                       /* Address state */
                    (*g_usb_PcdDriver.statediagram)(USB_NO_ARG, USB_STS_ADDRESS);
                break;
                case USB_DS_CNFG:                       /* Configured state */
                    (*g_usb_PcdDriver.statediagram)(g_usb_PcdConfigNum, USB_STS_CONFIGURED);
                break;
                case USB_DS_SPD_POWR:                   /* Power suspend state */
                /* Continue */
                case USB_DS_SPD_DFLT:                   /* Default suspend state */
                /* Continue */
                case USB_DS_SPD_ADDR:                   /* Address suspend state */
                /* Continue */
                case USB_DS_SPD_CNFG:                   /* Configured Suspend state */
                    usb_pstd_suspend_process();
                break;
                default:                                /* Error */
                    /* no processing */
                break;
            }
        break;

        case USB_INT_CTRT:              /* CTRT */
            intseq = (uint8_t)(intdata & USB_CTSQ);
            if( ((intseq == USB_CS_RDDS) || (intseq == USB_CS_WRDS)) || (intseq == USB_CS_WRND) )
            {
                /* Save request register */
                usb_pstd_save_request();
            }

            if( g_usb_PcdRequest.WORD.BIT.bType == USB_STANDARD )
            {
                switch( intseq )
                {
                    case USB_CS_IDST:                           /* Idle or setup stage */
                        usb_pstd_stand_req0();
                    break;
                    case USB_CS_RDDS:                           /* Control read data stage */
                        usb_pstd_stand_req1();
                    break;
                    case USB_CS_WRDS:                           /* Control write data stage */
                        usb_pstd_stand_req2();
                    break;
                    case USB_CS_WRND:                           /* Control write nodata status stage */
                        usb_pstd_stand_req3();
                        R_usb_pstd_ControlEnd(USB_CTRL_END);
                    break;
                    case USB_CS_RDSS:                           /* Control read status stage */
                        usb_pstd_stand_req4();
                        R_usb_pstd_ControlEnd(USB_CTRL_END);
                    break;
                    case USB_CS_WRSS:                           /* Control write status stage */
                        usb_pstd_stand_req5();
                        R_usb_pstd_ControlEnd(USB_CTRL_END);
                    break;
                    case USB_CS_SQER:                           /* Control sequence error */
                    default:                                    /* Illegal */
                        R_usb_pstd_ControlEnd(USB_DATA_ERR);
                    break;
                }
            }
            else
            {
                /* Class request .or. Vendor specific */
                (*g_usb_PcdDriver.ctrltrans)((usb_request_t*)&g_usb_PcdRequest, (uint16_t)intseq);
            }
        break;

        case USB_INT_UNKNOWN:               /* Error */
        /* continue */
        default:
            /* no processing */
        break;
    }

}   /* eof usb_pstd_interrupt_process() */


/******************************************************************************
Function Name   : usb_pstd_interrupt_handler
Description     : USB interrupt handler
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_interrupt_handler(void)
{
    usb_intinfo_t   *ptr;

    ptr = &g_usb_LibIntMessage[g_usb_LibIntMsgCnt];
    usb_pstd_check_interrupt_source(&ptr->inttype, &ptr->bitsts);
    if( R_USB_ISND_MSG(USB_PCD_MBX, (usb_msg_t*)ptr) != USB_E_OK )
    {
        USB_DEBUG_HOOK(USB_DEBUG_HOOK_PERI | USB_DEBUG_HOOK_CODE2);
    }

    /* Renewal Message count */
    g_usb_LibIntMsgCnt++;
    if( g_usb_LibIntMsgCnt == USB_INTMSGMAX )
    {
        g_usb_LibIntMsgCnt  = USB_0;
    }
}   /* eof usb_pstd_interrupt_handler() */


/******************************************************************************
Function Name   : usb_pstd_save_request
Description     : save request register
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_save_request(void)
{

    /* Valid clear */
    USB_CLR_STS(INTSTS0, USB_VALID);

    /* Save USB Request  */
    USB_RD(USBREQ,  g_usb_PcdRequest.WORD.wRequest);
    USB_RDW(USBVAL,  g_usb_PcdRequest.wValue);
    USB_RDW(USBINDX, g_usb_PcdRequest.wIndex);
    USB_RDW(USBLENG, g_usb_PcdRequest.wLength);
}   /* eof usb_pstd_save_request() */


/******************************************************************************
Function Name   : usb_pstd_get_interface_num
Description     : Get Interface number
Arguments       : none
Return value    : uint8_t ; Number of this interface 
                : (bNumInterfaces)
******************************************************************************/
uint8_t usb_pstd_get_interface_num(void)
{
    return g_usb_PcdDriver.configtbl[USB_CON_NUM_INTERFACE];
}   /* eof usb_pstd_get_interface_num() */


/******************************************************************************
Function Name   : usb_pstd_chk_remote
Description     : Check remote wakeup bit (bmAttributes)
Arguments       : none
Return value    : uint8_t ; remote wakeup status(YES/NO)
******************************************************************************/
USB_STATIC uint8_t usb_pstd_chk_remote(void)
{

    if( g_usb_PcdConfigNum == USB_0 )
    {
        return USB_NO;
    }

    /* Check configuration descriptor - bmAttributes */
    if (g_usb_PcdDriver.configtbl[USB_CON_ATTRIBUTE] & USB_CF_RWUPON)
    {
        return USB_YES;
    }

    return USB_NO;
}   /* eof usb_pstd_chk_remote() */


/******************************************************************************
Function Name   : usb_pstd_chk_configured
Description     : Check state ( Configured )
Arguments       : none
Return value    : uint16_t ; Configuration state (YES/NO)
******************************************************************************/
uint8_t usb_pstd_chk_configured(void)
{
    uint16_t    buf;

    USB_RD(INTSTS0, buf);
    if( (buf & USB_DVSQ) == USB_DS_CNFG )
    {
        return USB_YES;     /* Configured */
    }
    return USB_NO;          /* not Configured */
}   /* eof usb_pstd_chk_configured() */


/******************************************************************************
Renesas Abstracted Peripheral Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_check_interrupt_source
Description     : USB interrupt Handler
Arguments       : uint8_t *inttype : Interrupt type
                : uint8_t *bitsts  : Pipe transfer bitsts
Return value    : none
******************************************************************************/
#ifdef  __RX
USB_STATIC void usb_pstd_check_interrupt_source(usb_strct_t *inttype, uint16_t *bitsts)
#else   /* __RX */
USB_STATIC void usb_pstd_check_interrupt_source(usb_strct_t *inttype, usb_strct_t *bitsts)
#endif  /* __RX */
{
    status_t    intsts0, brdysts, nrdysts, bempsts;
    status_t    intenb0, brdyenb, nrdyenb, bempenb;
    intbit_t    ists_u;
#ifdef  __RX
    status_t    bsts_d,  nsts_d,  ests_d;
#else   /* __RX */
    pipests_t   bsts_d,  nsts_d,  ests_d;
#endif  /* __RX */

    uint16_t    stat;
    usb_strct_t key;

    /* Register Save */
    USB_RD(INTSTS0, intsts0.WORD.w);
    USB_RD(BRDYSTS, brdysts.WORD.w);
    USB_RD(NRDYSTS, nrdysts.WORD.w);
    USB_RD(BEMPSTS, bempsts.WORD.w);
    USB_RD(INTENB0, intenb0.WORD.w);
    USB_RD(BRDYENB, brdyenb.WORD.w);
    USB_RD(NRDYENB, nrdyenb.WORD.w);
    USB_RD(BEMPENB, bempenb.WORD.w);

    /* Interrupt status get */
    ists_u.BYTE.b       = (uint8_t)(intsts0.WORD.BYTE.up & intenb0.WORD.BYTE.up);
#ifdef  __RX
    bsts_d.WORD.w       = (uint16_t)(brdysts.WORD.w & brdyenb.WORD.w);
    nsts_d.WORD.w       = (uint16_t)(nrdysts.WORD.w & nrdyenb.WORD.w);
    ests_d.WORD.w       = (uint16_t)(bempsts.WORD.w & bempenb.WORD.w);
#else   /* __RX */
    bsts_d.BYTE.b       = (uint8_t)(brdysts.WORD.BYTE.dn & brdyenb.WORD.BYTE.dn);
    nsts_d.BYTE.b       = (uint8_t)(nrdysts.WORD.BYTE.dn & nrdyenb.WORD.BYTE.dn);
    ests_d.BYTE.b       = (uint8_t)(bempsts.WORD.BYTE.dn & bempenb.WORD.BYTE.dn);
#endif  /* __RX */

    if( !ists_u.BYTE.b )
    {
        *bitsts     = USB_INT_UNKNOWN;
        *inttype    = USB_0;
        return;
    }

    key     = USB_INT_UNKNOWN;
    stat    = USB_0;

    /***** Processing USB bus signal *****/
    /***** Resume signal *****/
    if( ists_u.BYTE.BIT.RESM )
    {
        USB_CLR_STS(INTSTS0, USB_RESM);
        key = USB_INT_RESM;
    }
    /***** Vbus change *****/
    else if( ists_u.BYTE.BIT.VBINT )
    {
        USB_CLR_STS(INTSTS0, USB_VBINT);
        key = USB_INT_VBINT;
    }
    /***** SOFR change *****/
    else if( ists_u.BYTE.BIT.SOFR )
    {
        USB_CLR_STS(INTSTS0, USB_SOFR);
        key = USB_INT_SOFR;
    }

    /***** Processing device state *****/
    /***** DVST change *****/
    else if( ists_u.BYTE.BIT.DVST )
    {
        USB_CLR_STS(INTSTS0, USB_DVST);
        stat    = intsts0.WORD.w;
        key     = USB_INT_DVST;
    }

    /***** Processing PIPE0 data *****/
#ifdef  __RX
    else if( (ists_u.BYTE.BIT.BRDY == USB_1) && ((bsts_d.WORD.w & USB_1) == USB_1) )
#else   /* __RX */
    else if( (ists_u.BYTE.BIT.BRDY == USB_1) && (bsts_d.BYTE.BIT.PIPE0 == USB_1) )
#endif  /* __RX */
    {
        USB_CLR_STS(BRDYSTS, USB_BRDY0);
        stat    = USB_BRDY0;
        key     = USB_INT_BRDY;
    }
#ifdef  __RX
    else if( (ists_u.BYTE.BIT.BEMP == USB_1) && ((ests_d.WORD.w & USB_1) == USB_1) )
#else   /* __RX */
    else if( (ists_u.BYTE.BIT.BEMP == USB_1) && (ests_d.BYTE.BIT.PIPE0 == USB_1) )
#endif  /* __RX */
    {
        USB_CLR_STS(BEMPSTS, USB_BEMP0);
        stat    = USB_BEMP0;
        key     = USB_INT_BEMP;
    }
#ifdef  __RX
    else if( (ists_u.BYTE.BIT.NRDY == USB_1) && ((nsts_d.WORD.w & USB_1) == USB_1) )
#else   /* __RX */
    else if( (ists_u.BYTE.BIT.NRDY == USB_1) && (nsts_d.BYTE.BIT.PIPE0 == USB_1) )
#endif  /* __RX */
    {
        USB_CLR_STS(NRDYSTS, USB_NRDY0);
        stat    = USB_NRDY0;
        key     = USB_INT_NRDY;
    }

    /***** Processing setup transaction *****/
    else if( ists_u.BYTE.BIT.CTRT )
    {
        USB_RD(INTSTS0, stat);
        USB_CLR_STS(INTSTS0, USB_CTRT);
        key = USB_INT_CTRT;
        if( (uint8_t)(stat & USB_CTSQ) == USB_CS_SQER )
        {
            USB_CLR_STS(INTSTS0, USB_VALID);
            *bitsts     = USB_INT_UNKNOWN;
            *inttype    = USB_0;
            return;
        }
    }

    /***** Processing MIN_PIPE_NO-MAX_PIPE_NO data *****/
    /***** EP4-7 BRDY *****/
    else if( ists_u.BYTE.BIT.BRDY )
    {
#ifdef  __RX
        stat    = bsts_d.WORD.w;
#else   /* __RX */
        stat    = bsts_d.BYTE.b;
#endif  /* __RX */
        USB_CLR_STS(BRDYSTS, stat);
        key     = USB_INT_BRDY;
    }
    /***** EP4-7 BEMP *****/
    else if( ists_u.BYTE.BIT.BEMP )
    {
#ifdef  __RX
        stat    = ests_d.WORD.w;
#else   /* __RX */
        stat    = ests_d.BYTE.b;
#endif  /* __RX */
        USB_CLR_STS(BEMPSTS, stat);
        key     = USB_INT_BEMP;
    }
    /***** EP4-7 NRDY *****/
    else if( ists_u.BYTE.BIT.NRDY )
    {
#ifdef  __RX
        stat    = nsts_d.WORD.w;
#else   /* __RX */
        stat    = nsts_d.BYTE.b;
#endif  /* __RX */
        USB_CLR_STS(NRDYSTS, stat);
        key     = USB_INT_NRDY;
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
}   /* eof usb_pstd_check_interrupt_source() */


/******************************************************************************
Function Name   : usb_pstd_stand_req1
Description     : Standard request (control read data stage)
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_stand_req1(void)
{
    switch( g_usb_PcdRequest.WORD.BIT.bRequest )
    {
        case USB_GET_STATUS:
            usb_pstd_get_status1();
        break;
        case USB_GET_DESCRIPTOR:
            usb_pstd_get_descriptor1();
        break;
        case USB_GET_CONFIGURATION:
            usb_pstd_get_configuration1();
        break;
        case USB_GET_INTERFACE:
            usb_pstd_get_interface1();
        break;
        case USB_SYNCH_FRAME:       /* To responds STALL because SYNCH_FRAME is a uncorrespondence request. */
        /* continue */
        default:
            R_usb_pstd_SetStallPipe0();
        break;
    }
}   /* eof usb_pstd_stand_req1() */


/******************************************************************************
Function Name   : usb_pstd_stand_req2
Description     : Standard Request (control write data stage)
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_stand_req2(void)
{
    /* To responds STALL because SET_DESCRIPTOR is a uncorrespondence request. */
    R_usb_pstd_SetStallPipe0();
}   /* eof usb_pstd_stand_req2() */


/******************************************************************************
Function Name   : usb_pstd_stand_req3
Description     : Standard request (control write nodata status stage)
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_stand_req3(void)
{
    switch( g_usb_PcdRequest.WORD.BIT.bRequest )
    {
        case USB_CLEAR_FEATURE:
            usb_pstd_clear_feature3();
        break;
        case USB_SET_FEATURE:
            usb_pstd_set_feature3();
        break;
        case USB_SET_ADDRESS:
            usb_pstd_set_address3();
        break;
        case USB_SET_CONFIGURATION:
            usb_pstd_set_configuration3();
        break;
        case USB_SET_INTERFACE:
            usb_pstd_set_interface3();
        break;
        default:
            R_usb_pstd_SetStallPipe0();
        break;
    }
}   /* eof usb_pstd_stand_req3() */


/******************************************************************************
Function Name   : usb_pstd_stand_req4
Description     : Standard request (control read status stage)
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_stand_req4(void)
{
    switch( g_usb_PcdRequest.WORD.BIT.bRequest )
    {
        case USB_GET_STATUS:                /* GetStatus4 */
        /* continue */
        case USB_GET_DESCRIPTOR:            /* GetDescriptor4 */
        /* continue */
        case USB_GET_CONFIGURATION:         /* GetConfiguration4 */
        /* continue */
        case USB_GET_INTERFACE:             /* GetInterface4 */
        /* continue */
        case USB_SYNCH_FRAME:               /* SynchFrame4 */
        break;
        default:
            R_usb_pstd_SetStallPipe0();
        break;
    }
}   /* eof usb_pstd_stand_req4() */


/******************************************************************************
Function Name   : usb_pstd_stand_req5
Description     : Standard request (control write status stage)
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_stand_req5(void)
{
    if( g_usb_PcdRequest.WORD.BIT.bRequest != USB_SET_DESCRIPTOR )
    {
        R_usb_pstd_SetStallPipe0();
    }
}   /* eof usb_pstd_stand_req5() */


/******************************************************************************
Function Name   : usb_pstd_stand_req0
Description     : Standard request (idle or setup stage)
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_stand_req0(void)
{
    switch( g_usb_PcdRequest.WORD.BIT.bRequest )
    {
        case USB_CLEAR_FEATURE:
            usb_pstd_clear_feature0();
        break;
        case USB_SET_FEATURE:
            usb_pstd_set_feature0();
        break;
        case USB_SET_ADDRESS:
            usb_pstd_set_address0();
        break;
        case USB_SET_CONFIGURATION:
            usb_pstd_set_configuration0();
        break;
        case USB_SET_INTERFACE:
            usb_pstd_set_interface0();
        break;
        default:
            /* no processing */
        break;
    }
}   /* eof usb_pstd_stand_req0() */



/******************************************************************************
Function Name   : usb_pstd_clear_feature3
Description     : Clear Feature3
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_clear_feature3(void)
{
//  The check on "g_usb_PcdRequest.wLength == 0" has been executed with H/W. 
    switch( g_usb_PcdRequest.WORD.BIT.bRecipient )
    {
        case USB_DEVICE:
            if( (g_usb_PcdRequest.wValue == USB_DEVICE_REMOTE_WAKEUP) && (g_usb_PcdRequest.wIndex == 0) )
            {
                if( usb_pstd_chk_remote() == USB_YES )
                {
                    g_usb_PcdRemoteWakeup = USB_NO;
                    (*g_usb_PcdDriver.ctrltrans)((usb_request_t*)&g_usb_PcdRequest, USB_CLEARREMOTE);
                    return;
                }
            }
        break;
        case USB_ENDPOINT:
            if( g_usb_PcdRequest.wValue == USB_ENDPOINT_HALT )
            {
                /* Send endpoint0 information */
                if( (uint8_t)(g_usb_PcdRequest.wIndex & USB_EPNUMFIELD) == USB_0 )
                {
                    g_usb_CurrentPipe = USB_PIPE0;
                    usb_cstd_clr_stall();
                    return;
                }

                /* Send another endpoint information */
                /* EPNUM does the mask in the EPNUMFIELD field, the judgment is unnecessary. */
                /* The usb_pstd_epadr_2_pipe() function sets the value of g_usb_CurrentPipe. */
                usb_pstd_epadr_2_pipe();
                if( g_usb_CurrentPipe != USB_ERROR )
                {
                    if( usb_cstd_get_pid() == USB_PID_BUF )
                    {
                        /* When PID=BUF, to set to PID=NAK and the toggle is cleared. */
                        /* And, to return it to PID=BUF again at the end.  */
                        usb_cstd_set_nak();
                        usb_cstd_reg_set_pipexctr(USB_SQCLR);
                        usb_cstd_reg_set_pipexctr(USB_PID_BUF);
                    }
                    else
                    {
                        /* When not PID=BUF, to set to PID=STALL and the toggle is cleared. */
                        usb_cstd_clr_stall();
                        usb_cstd_reg_set_pipexctr(USB_SQCLR);
                    }
                    (*g_usb_PcdDriver.ctrltrans)((usb_request_t*)&g_usb_PcdRequest, USB_CLEARSTALL);
                    return;
                }
            }
        break;
        case USB_INTERFACE:
        /* continue */
        default:
            /* no processing */
        break;
    }
    R_usb_pstd_SetStallPipe0();
}   /* eof usb_pstd_clear_feature3() */


/******************************************************************************
Function Name   : usb_pstd_clear_feature0
Description     : Clear Feature0
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_clear_feature0(void)
{
}   /* eof usb_pstd_clear_feature0() */



/******************************************************************************
Function Name   : usb_pstd_get_configuration1
Description     : Get Configuration1
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_get_configuration1(void)
{
    /* check request */
    if( (((g_usb_PcdRequest.WORD.BIT.bRecipient == USB_DEVICE) && (g_usb_PcdRequest.wValue == USB_0))
     && (g_usb_PcdRequest.wIndex == USB_0)) && (g_usb_PcdRequest.wLength == USB_1) )
    {
        R_usb_pstd_ControlRead(USB_1, &g_usb_PcdConfigNum);
        return;
    }
    R_usb_pstd_SetStallPipe0();
}   /* eof usb_pstd_get_configuration1() */


/******************************************************************************
Function Name   : usb_pstd_get_descriptor1
Description     : Get Descriptor1
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_get_descriptor1(void)
{
    uint8_t         idx;
    totallen_t      len;
    uint8_t         *table;

    if( g_usb_PcdRequest.WORD.BIT.bRecipient == USB_DEVICE )
    {
        idx = (uint8_t)(g_usb_PcdRequest.wValue & USB_DT_INDEX);
        #if DEBUG_CONSOLE
            printf("get_descriptor(); PcdRequest.wValue was 0x%X.\n", idx);
        #endif
        switch( (uint8_t)USB_GET_DT_TYPE(g_usb_PcdRequest.wValue) )
        {
            /*---- Device descriptor ----*/
            case USB_DT_DEVICE:
                if((g_usb_PcdRequest.wIndex == USB_0) && (idx == USB_0))
                {
                    table = g_usb_PcdDriver.devicetbl;
                    len.WORD.BYTE.up    = USB_0;
                    len.WORD.BYTE.dn    = table[USB_DESCRIPTOR_SIZE];
                    if( g_usb_PcdRequest.wLength < len.WORD.w )
                    {
                        len.WORD.w   = g_usb_PcdRequest.wLength;
                    }
                    /* Control read start */
                    R_usb_pstd_ControlRead((usb_leng_t)len.WORD.w, table);
                    return;
                }
            break;
            /*---- Configuration descriptor ----*/
            case USB_DT_CONFIGURATION:
                if((g_usb_PcdRequest.wIndex == USB_0) && (idx == USB_0))
                {
                    table = g_usb_PcdDriver.configtbl;
                    len.WORD.BYTE.up    = table[USB_CON_TOTALLENG_HI];
                    len.WORD.BYTE.dn    = table[USB_CON_TOTALLENG_LO];
                    /* Descriptor > wLength */
                    if( g_usb_PcdRequest.wLength < len.WORD.w )
                    {
                        len.WORD.w   = g_usb_PcdRequest.wLength;
                    }
                    /* Control read start */
                    R_usb_pstd_ControlRead((usb_leng_t)len.WORD.w, table);
                    return;
                }
            break;
            /*---- String descriptor ----*/
            case USB_DT_STRING:
                if( idx < USB_STRINGNUM )
                {
                    table = g_usb_PcdDriver.stringtbl[idx];
                    len.WORD.BYTE.up    = USB_0;
                    len.WORD.BYTE.dn    = table[USB_DESCRIPTOR_SIZE];
                    if( g_usb_PcdRequest.wLength < len.WORD.w )
                    {
                        len.WORD.w   = g_usb_PcdRequest.wLength;
                    }
                    /* Control read start */
                    R_usb_pstd_ControlRead((usb_leng_t)len.WORD.w, table);
                    return;
                }
            break;
            case USB_DT_INTERFACE:
            /* continue */
            case USB_DT_ENDPOINT:
            /* continue */
            case USB_DT_DEVICE_QUALIFIER:
            /* continue */
            case USB_DT_OTHER_SPEED_CONF:
            /* continue */
            case USB_DT_INTERFACE_POWER:
            /* continue */
            default:
                /* no processing */
            break;
        }
    }

    if( g_usb_PcdRequest.WORD.BIT.bRecipient == USB_INTERFACE )
    {
        (*g_usb_PcdDriver.ctrltrans)((usb_request_t*)&g_usb_PcdRequest, USB_RECIPIENT);
        return;
    }
    R_usb_pstd_SetStallPipe0();

}   /* eof usb_pstd_get_descriptor1() */


/******************************************************************************
Function Name   : usb_pstd_get_interface1
Description     : Get Interface1
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_get_interface1(void)
{
    /* check request */
    if (((g_usb_PcdRequest.WORD.BIT.bRecipient == USB_INTERFACE) && (g_usb_PcdRequest.wValue == USB_0))
     && (g_usb_PcdRequest.wLength == USB_1))
    {
        if( g_usb_PcdRequest.wIndex < usb_pstd_get_interface_num() )
        {
            g_usb_PcdRequestData[USB_0] = USB_0;
            R_usb_pstd_ControlRead(USB_1, g_usb_PcdRequestData);
            return;
        }
    }
    R_usb_pstd_SetStallPipe0();
}   /* eof usb_pstd_get_interface1() */


/******************************************************************************
Function Name   : usb_pstd_get_status1
Description     : Get Status1
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_get_status1(void)
{
    uint16_t    buffer;

    if( (g_usb_PcdRequest.wValue == USB_0) && (g_usb_PcdRequest.wLength == USB_2) )
    {
        g_usb_PcdRequestData[USB_0] = USB_0;
        g_usb_PcdRequestData[USB_1] = USB_0;
        switch( g_usb_PcdRequest.WORD.BIT.bRecipient )
        {
            case USB_DEVICE:
                if( g_usb_PcdRequest.wIndex == USB_0 )
                {
                    /* Default is a bus powered setting because it has initialized with USB_0. */
                    if (g_usb_PcdDriver.configtbl[USB_CON_ATTRIBUTE] & USB_CF_SELFP)
                    {
                        g_usb_PcdRequestData[USB_0] = USB_GS_SELFPOWERD;
                    }
                    if (g_usb_PcdRemoteWakeup == USB_YES)
                    {
                        g_usb_PcdRequestData[USB_0] |= USB_GS_REMOTEWAKEUP;
                    }
                    R_usb_pstd_ControlRead(USB_2, g_usb_PcdRequestData);
                    return;
                }
            break;
            case USB_INTERFACE:
                if( usb_pstd_chk_configured() == USB_YES )
                {
                    if( g_usb_PcdRequest.wIndex < usb_pstd_get_interface_num() )
                    {
                        R_usb_pstd_ControlRead(USB_2, g_usb_PcdRequestData);
                        return;
                    }
                }
            break;
            case USB_ENDPOINT:

                /* Send endpoint0 information */
                if ((uint8_t)(g_usb_PcdRequest.wIndex & USB_EPNUMFIELD) == USB_0)
                {
                    USB_RD(DCPCTR, buffer);
                    if (buffer & USB_PID_STALL)
                    {
                        g_usb_PcdRequestData[USB_0] = USB_GS_HALT;
                    }
                    R_usb_pstd_ControlRead(USB_2, g_usb_PcdRequestData);
                    return;
                }

                /* Send another endpoint information */
                if( usb_pstd_chk_configured() == USB_YES )
                {
                    /* EPNUM does the mask in the EPNUMFIELD field, the judgment is unnecessary. */
                    /* The usb_pstd_epadr_2_pipe() function sets the value of g_usb_CurrentPipe. */
                    usb_pstd_epadr_2_pipe();
                    if( g_usb_CurrentPipe != USB_ERROR )
                    {
                        buffer = usb_cstd_get_pid();
                        if (buffer & USB_PID_STALL)
                        {
                            g_usb_PcdRequestData[USB_0] = USB_GS_HALT;
                        }
                        R_usb_pstd_ControlRead(USB_2, g_usb_PcdRequestData);
                        return;
                    }
                }
            break;
            default:
                /* no processing */
            break;
        }
    }
    R_usb_pstd_SetStallPipe0();

}   /* eof usb_pstd_get_status1() */


/******************************************************************************
Function Name   : usb_pstd_set_address3
Description     : Set Address3
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_set_address3(void)
{
    /* When the request makes an error, this function is called */
    /* because USB-IP responds the SET_ADDRESS request automatically. */
    R_usb_pstd_SetStallPipe0();
}   /* eof usb_pstd_set_address3() */


/******************************************************************************
Function Name   : usb_pstd_set_address0
Description     : Set Address0
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_set_address0(void)
{
}   /* eof usb_pstd_set_address0() */


/******************************************************************************
Function Name   : usb_pstd_set_configuration3
Description     : Set Configuration3
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_set_configuration3(void)
{
    if( g_usb_PcdRequest.WORD.BIT.bRecipient == USB_DEVICE )
    {
        /* The check on "g_usb_PcdRequest.wLength == 0" has been executed with H/W.  */
        if( ((g_usb_PcdRequest.wValue == g_usb_PcdDriver.configtbl[USB_CON_CONFIG_VAL])||
            (g_usb_PcdRequest.wValue == USB_0)) && (g_usb_PcdRequest.wIndex == USB_0) )
        {
            if( g_usb_PcdConfigNum != (uint8_t)g_usb_PcdRequest.wValue )
            {
                /* Only when the configuration number is changed, the pipe is set. */
                usb_pstd_set_pipe_registration(USB_NO);             /* clear pipe registration */
                if( (uint8_t)g_usb_PcdRequest.wValue > USB_0 )
                {
                    usb_pstd_set_pipe_registration(USB_YES);        /* set pipe registration */
                }
            }
            return;
        }
    }
    R_usb_pstd_SetStallPipe0();

}   /* eof usb_pstd_set_configuration3() */


/******************************************************************************
Function Name   : usb_pstd_set_configuration0
Description     : Set Configuration0
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_set_configuration0(void)
{
    /* Configuration number set */
    g_usb_PcdConfigNum      = (uint8_t)g_usb_PcdRequest.wValue;
}   /* eof usb_pstd_set_configuration0() */


/******************************************************************************
Function Name   : usb_pstd_set_feature3
Description     : Set Feature3
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_set_feature3(void)
{
    /*  The check on "g_usb_PcdRequest.wLength == 0" has been executed with H/W. */
    switch( g_usb_PcdRequest.WORD.BIT.bRecipient )
    {
        case USB_DEVICE:
            if ( g_usb_PcdRequest.wValue == USB_DEVICE_REMOTE_WAKEUP)
            {
                if( g_usb_PcdRequest.wIndex == USB_0 )
                {
                    if( usb_pstd_chk_remote() == USB_YES )
                    {
                        g_usb_PcdRemoteWakeup = USB_YES;
                        (*g_usb_PcdDriver.ctrltrans)((usb_request_t*)&g_usb_PcdRequest, USB_SETREMOTE);
                        return;
                    }
                }
            }
        break;
        case USB_ENDPOINT:
            if( g_usb_PcdRequest.wValue == USB_ENDPOINT_HALT )
            {
                /* Send endpoint0 information */
                if( (uint16_t)(g_usb_PcdRequest.wIndex & USB_EPNUMFIELD) == USB_0 )
                {
                    return;
                }

                /* Send another endpoint information */
                /* EPNUM does the mask in the EPNUMFIELD field, the judgment is unnecessary. */
                /* The usb_pstd_epadr_2_pipe() function sets the value of g_usb_CurrentPipe. */
                usb_pstd_epadr_2_pipe();
                if( g_usb_CurrentPipe != USB_ERROR )
                {
                    (*g_usb_PcdDriver.ctrltrans)((usb_request_t*)&g_usb_PcdRequest, USB_SETSTALL);
                    R_usb_pstd_SetPipeStall(g_usb_CurrentPipe);
                    return;
                }
            }
        break;
        case USB_INTERFACE:
        /* continue */
        default:
            /* no processing */
        break;
    }
    R_usb_pstd_SetStallPipe0();

}   /* eof usb_pstd_set_feature3() */


/******************************************************************************
Function Name   : usb_pstd_set_feature0
Description     : Set Feature0
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_set_feature0(void)
{
}   /* eof usb_pstd_set_feature0() */


/******************************************************************************
Function Name   : usb_pstd_set_interface3
Description     : Set Interface3
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_set_interface3(void)
{
    /* Configured ? */
    if( (usb_pstd_chk_configured() == USB_YES) && (g_usb_PcdRequest.WORD.BIT.bRecipient == USB_INTERFACE) )
    {
        /*  The check on "g_usb_PcdRequest.wLength == 0" has been executed with H/W. */
        if (g_usb_PcdRequest.wIndex < usb_pstd_get_interface_num())
        {
            if (g_usb_PcdRequest.wValue == USB_0)
            {
                return;
            }
        }
    }
    R_usb_pstd_SetStallPipe0();
}   /* eof usb_pstd_set_interface3() */


/******************************************************************************
Function Name   : usb_pstd_set_interface0
Description     : Set Interface0
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_set_interface0(void)
{
}   /* eof usb_pstd_set_interface0() */


/******************************************************************************
Function Name   : usb_pstd_brdy_pipe
Description     : BRDY interrupt
Arguments       : uint16_t bitsts ; BRDYSTS Register & BRDYENB Register
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_brdy_pipe(uint16_t bitsts)
{
    /* When operating by the peripheral function, usb_cstd_brdy_pipe() is executed with PIPEx request because */
    /* two BRDY messages are issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
    if( (bitsts & USB_BRDY0) == USB_BRDY0 )
    {
        g_usb_CurrentPipe = USB_PIPE0;
        switch( usb_cstd_cfifo_2_buf() )
        {
            case USB_READEND:           /* End of data read */
            /* Continue */
            case USB_READSHRT:          /* End of data read */
                usb_cstd_brdy_disable();
            break;
            case USB_READING:           /* Continue of data read */
                /* PID = BUF */
                R_usb_cstd_SetBufPipe0();
            break;
            case USB_READOVER:          /* buffer over */
                /* Control transfer stop(end) */
                USB_WR(CFIFOCTR, USB_BCLR);
                R_usb_pstd_ControlEnd(USB_DATA_OVR);
            break;
            case USB_FIFOERROR:         /* FIFO access error */
                /* Control transfer stop(end) */
                R_usb_pstd_ControlEnd(USB_DATA_ERR);
            break;
            default:
                /* no processing */
            break;
        }
    }
    else
    {
        usb_cstd_brdy_pipe(bitsts);
    }
}   /* eof usb_pstd_brdy_pipe() */


/******************************************************************************
Function Name   : usb_pstd_nrdy_pipe
Description     : NRDY interrupt
Arguments       : uint16_t bitsts ; NRDYSTS Register & NRDYENB Register
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_nrdy_pipe(uint16_t bitsts)
{
    /* This program does not support isochronous transfer. */
}   /* eof usb_pstd_nrdy_pipe() */


/******************************************************************************
Function Name   : usb_pstd_bemp_pipe
Description     : BEMP interrupt
Arguments       : uint16_t bitsts ; BEMPSTS Register & BEMPENB Register
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_bemp_pipe(uint16_t bitsts)
{
    /* When operating by the peripheral function, usb_cstd_bemp_pipe() is executed with PIPEx request because */
    /* two BEMP messages are issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
    if( (bitsts & USB_BEMP0) == USB_BEMP0 )
    {
        g_usb_CurrentPipe = USB_PIPE0;
        switch( usb_cstd_buf_2_cfifo() )
        {
            case USB_WRITEEND:          /* End of data write (not null) */
            /* Continue */
            case USB_WRITESHRT:         /* End of data write */
                /* Enable empty interrupt */
                usb_cstd_bemp_disable();
            break;
            case USB_WRITING:           /* Continue of data write */
                /* PID = BUF */
                R_usb_cstd_SetBufPipe0();
            break;
            case USB_FIFOERROR:         /* FIFO access error */
                /* Control transfer stop(end) */
                R_usb_pstd_ControlEnd(USB_DATA_ERR);
            break;
            default:
                /* no processing */
            break;
        }
    }
    else
    {
        usb_cstd_bemp_pipe(bitsts);
    }
}   /* eof usb_pstd_bemp_pipe() */


/******************************************************************************
Function Name   : usb_pstd_bus_reset
Description     : USB bus reset
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_bus_reset(void)
{
    /* Memory clear, Initialize DCP configuration register & DCP max packet size register */
    g_usb_PcdRemoteWakeup   = USB_NO;
    g_usb_PcdConfigNum      = USB_0;
    USB_WR(DCPCFG,  USB_0);
    USB_WR(DCPMAXP, g_usb_PcdDriver.devicetbl[USB_DEV_MAX_PKT_SIZE]);
}   /* eof usb_pstd_bus_reset() */


/******************************************************************************
Function Name   : usb_pstd_remote_wakeup
Description     : USB remotewakeup
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_remote_wakeup(void)
{
    uint16_t    buf;

    /* Enable remote wakeup ? */
    if( g_usb_PcdRemoteWakeup == USB_YES )
    {
        USB_CLR_PAT(INTENB0, USB_RSME);             /* RESM interrupt disable */

        USB_RD(INTSTS0, buf);                       /* RESM status read */
        if (buf & USB_RESM)
        {
            USB_CLR_STS(INTSTS0, USB_RESM);         /* RESM status clear */
        }
        else
        {
            if (buf & USB_DS_SUSP)
            {
                USB_SET_PAT(DVSTCTR0, USB_WKUP);    /* Remote wakeup set */
                g_usb_Pcdintsts0    &= (uint16_t)(~USB_DS_SUSP);
            }
        }
    }
}   /* eof usb_pstd_remote_wakeup() */


/******************************************************************************
Function Name   : usb_pstd_init_connect
Description     : Check connection
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_init_connect(void)
{
    /* Interrupt enable */
    USB_CLR_STS(INTSTS0, USB_VBINT);
    USB_SET_PAT(INTENB0, (USB_VBSE | USB_DVSE | USB_CTRE ));
    usb_pstd_chk_vbsts();
}   /* eof usb_pstd_init_connect() */


/******************************************************************************
Function Name   : usb_pstd_port_enable
Description     : D+/D- Line Pull-up Enable
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_port_enable(void)
{
    usb_pstd_resume_process();              /* The resume interrupt is disregarded. */
#ifdef  USB_LSPERI_PP
    USB_SET_PAT(SYSCFG, USB_DMRPU);
#endif  /* USB_LSPERI_PP */
#ifdef  USB_FSPERI_PP
    USB_SET_PAT(SYSCFG, USB_DPRPU);
#endif  /* USB_FSPERI_PP */
#ifdef USB_PERI_BC_ENABLE
    (*g_usb_PcdDriver.statediagram)(g_usb_bc_detect, USB_PORTENABLE);
#else
    (*g_usb_PcdDriver.statediagram)(USB_NO_ARG, USB_PORTENABLE);
#endif
}   /* eof usb_pstd_port_enable() */


/******************************************************************************
Function Name   : usb_pstd_port_disable
Description     : D+/D- Line Pull-up Disable
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_port_disable(void)
{
    usb_pstd_resume_process();              /* The resume interrupt is disregarded. */
    USB_CLR_PAT(SYSCFG, (USB_DPRPU | USB_DMRPU));
    g_usb_PcdRemoteWakeup   = USB_NO;
    g_usb_Pcdintsts0        = USB_0;
    g_usb_PcdConfigNum      = USB_0;
}   /* eof usb_pstd_port_disable() */


/******************************************************************************
Function Name   : usb_pstd_attach_process
Description     : USB attach
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_attach_process(void)
{
    /* It provides for 100ms or less in T2(TSIGATT) in Chapter 7.1.7.3. */
    /* The delay time of 10ms was installed in Basic-FW to respond roughly */
    /* additionally with about 15ms at the chattering removal time of the VBUS signal. */
    usb_cpu_delay_xms((uint16_t)10);
    (*g_usb_PcdDriver.statediagram)(USB_NO_ARG, USB_STS_ATTACH);
#ifdef USB_PERI_BC_ENABLE
    usb_pstd_bc_detect_process();
#endif
    usb_pstd_port_enable();             /* Pull-up enable */
}   /* eof usb_pstd_attach_process() */


/******************************************************************************
Function Name   : usb_pstd_detach_process
Description     : USB detach
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_detach_process(void)
{
    uint16_t    *table;

    usb_pstd_port_disable();                /* Pull-up disable */

    table   = g_usb_PcdDriver.pipetbl;

    for( ; *table != USB_PDTBLEND; table += USB_EPL )
    {
        g_usb_CurrentPipe = (usb_pipe_t)(*table & USB_CURPIPE);
        usb_cstd_forced_termination(USB_DATA_DTCH);
        usb_cstd_clr_pipe_config();
    }
    /* Callback */
    (*g_usb_PcdDriver.statediagram)(USB_NO_ARG, USB_STS_DETACH);
}   /* eof usb_pstd_detach_process() */


/******************************************************************************
Function Name   : usb_pstd_suspend_process
Description     : USB suspend
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_suspend_process(void)
{
//    status_t    data1, data2;

    /* Because the space can be done, the interruption is permitted first, */
    /* the suspension release is done, and the cod clearing is done to processing. */
    USB_SET_PAT(INTENB0, USB_RSME);                 /* Resume interrupt enable */

//    USB_RD(INTSTS0, data1.WORD.w);
//    USB_RD(SYSSTS0, data2.WORD.w);

#ifdef  USB_LSPERI_PP
    if((data1.WORD.BYTE.dn & USB_DS_SUSP) && ((data2.WORD.BYTE.dn & USB_LNST) == USB_LS_JSTS))
    {
        (*g_usb_PcdDriver.statediagram)(USB_NO_ARG, USB_STS_SUSPEND);   /* Callback */
    }
    else
    {
        usb_pstd_resume_process();
    }
#endif  /* USB_LSPERI_PP */
#ifdef  USB_FSPERI_PP
    if((data1.WORD.BYTE.dn & USB_DS_SUSP) && ((data2.WORD.BYTE.dn & USB_LNST) == USB_FS_JSTS))
    {
        (*g_usb_PcdDriver.statediagram)(USB_NO_ARG, USB_STS_SUSPEND);   /* Callback */
    }
    else
    {
        usb_pstd_resume_process();
    }
#endif  /* USB_FSPERI_PP */
}   /* eof usb_pstd_suspend_process() */


/******************************************************************************
Function Name   : usb_pstd_resume_process
Description     : USB resume
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_resume_process(void)
{
    /* RESM interrupt disable */
    USB_CLR_STS(INTSTS0, USB_RESM);
    USB_CLR_PAT(INTENB0, USB_RSME);
}   /* eof usb_pstd_resume_process() */


/******************************************************************************
Function Name   : usb_pstd_chk_vbsts
Description     : Check VBUS status
Arguments       : none
Return          : uint8_t connection status (ATTACH/DETACH)
******************************************************************************/
USB_STATIC void usb_pstd_chk_vbsts(void)
{
    status_t    buf1, buf2, buf3;

    /* VBUS chattering cut */
    do
    {
        USB_RD(INTSTS0, buf1.WORD.w);
        usb_cpu_delay_xms((uint16_t)1);
        USB_RD(INTSTS0, buf2.WORD.w);
        usb_cpu_delay_xms((uint16_t)1);
        USB_RD(INTSTS0, buf3.WORD.w);
        buf1.WORD.BYTE.dn   ^= buf3.WORD.BYTE.dn;
        buf2.WORD.BYTE.dn   ^= buf3.WORD.BYTE.dn;
        buf1.WORD.BYTE.dn   |= buf2.WORD.BYTE.dn;
    }
    while (buf1.WORD.BYTE.dn & USB_VBSTS);

    /* VBUS status judge */
    if (buf3.WORD.BYTE.dn & USB_VBSTS)
    {
        /* Attach */
        usb_pstd_attach_process();
    }
    else
    {
        /* Detach */
        usb_pstd_detach_process();
    }
}   /* eof usb_pstd_chk_vbsts() */


/******************************************************************************
Function Name   : usb_pstd_epadr_2_pipe
Description     : Endpoint address to pipe number
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_epadr_2_pipe(void)
{
    uint16_t    *table;
    uint8_t     direp, tmp;

    /* Get PIPE Number from Endpoint address */
    direp           = (uint8_t)(g_usb_PcdRequest.wIndex & 0x0Fu);
    direp           |= (uint8_t)((g_usb_PcdRequest.wIndex & 0x80u) >> 3);

    g_usb_CurrentPipe   = USB_ERROR;
    table               = g_usb_PcdDriver.pipetbl;

    /* Information table search */
    for( ; *table != USB_PDTBLEND; table += USB_EPL )
    {
        /* EP table endpoint dir check */
        tmp = (uint8_t)(table[1] & (USB_DIRFIELD | USB_EPNUMFIELD));
        if( direp == tmp )
        {
            g_usb_CurrentPipe = (usb_pipe_t)(*table);
            return;
        }
    }
}   /* eof usb_pstd_epadr_2_pipe() */


/******************************************************************************
Function Name   : usb_pstd_set_ccpl
Description     : Control transfer status stage enable
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_set_ccpl(void)
{
    R_usb_cstd_SetBufPipe0();               /* Request ok */
    USB_SET_PAT(DCPCTR, USB_CCPL);          /* Status stage start */
}   /* eof usb_pstd_set_ccpl() */


/******************************************************************************
Function Name   : usb_pstd_set_pipe_registration
Description     : Set pipe configuration register
Arguments       : uint16_t command
Return value    : none
******************************************************************************/
USB_STATIC void usb_pstd_set_pipe_registration(uint16_t command)
{
    usb_cstd_set_pipe_registration(g_usb_PcdDriver.pipetbl, command);
}   /* eof usb_pstd_set_pipe_registration() */


/******************************************************************************
End of file
******************************************************************************/
