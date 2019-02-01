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
* File Name    : r_usb_hdriver.c
* Version      : 2.10
* Device(s)    : Renesas R8C/USB-Series, RL78/USB-Series
* Tool-Chain   : Renesas M16C Standard Toolchain (rev6.0.0 ), CA78K0R V1.60
* OS           : Not use
* H/W Platform : Independent
* Description  : This is the USB host control driver code.
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
#pragma section program P_hcd
#pragma section bss B_hcd
#pragma section rom C_hcd
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
#include "r_usb_cdefusbip.h"            /* USB-FW Library Header */
#include "r_usb_regaccess.h"            /* Definition of the USB register access macro */
#include "r_usb_api.h"                  /* USB API public function header */


/******************************************************************************
Macro definitions
******************************************************************************/
#define CLSDATASIZE     512


/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* variables */
/* Host control driver variables */
USB_STATIC  usb_port_t          g_usb_MgrPort;
USB_STATIC  uint16_t            g_usb_HcdDcpRegister[USB_MAXDEVADDR+1u];    /* DEVSEL & DCPMAXP (Multiple device) */
USB_STATIC  uint8_t             g_usb_HcdDevSpeed;                          /* Reset handshake result */
/* Host manager variables */
USB_STATIC  uint8_t             g_usb_HcdRegPointer;                        /* Driver number */
USB_STATIC  usb_tskinfo_t       *g_usb_MgrMessage;                          /* Mgr Task receive message */
USB_STATIC  usb_cbinfo_t        g_usb_MgrCallback;                          /* API callback function */
USB_STATIC  usb_utr_t           g_usb_MgrControlMessage;
USB_STATIC  usb_hcdrequest_t    g_usb_MgrRequest;
USB_STATIC  uint8_t             g_usb_MgrDevDesc[USB_DEVICESIZE];
USB_STATIC  uint8_t             g_usb_MgrConfDescr[USB_CONFIGSIZE];
USB_STATIC  uint8_t             g_usb_MgrDummyData[CLSDATASIZE];
/* Host manager sequnce function table */
USB_STATIC  const uint8_t   usb_sequence_tbl0[20] =
{
/* When the sequence processing starts newly by the sequence unprocessing now. */
//  msginfo CONTINUE    DETACH      ATTACH      OVERCU      REMOTE      SUSPEN
            USB_NOSEQE0,USB_DETACH0,USB_ATTACH0,USB_OVERCU0,USB_REMOTE0,USB_SUSPEN0,
//          RESUME      PORT_E      PORT_D      CLRSTL
            USB_RESUME0,USB_PORT_E0,USB_PORT_D0,USB_CLRSTL0,
//Global suspend(Select port)
            USB_SUSPEN2,USB_RESUME2,
};
USB_STATIC  const uint8_t   usb_sequence_tbl1[11] =
{
/* When the sequence processing is continuas. */
//  process CONTINUE    Detach      Attach      OverCurrent Remote      Suspend
            USB_NOSEQE0,USB_NOSEQE0,USB_ENUMER1,USB_NOSEQE0,USB_REMOTE1,USB_SUSPEN1,
//          Resume      PortEnable  PortDisable StallClear
            USB_RESUME1,USB_PORT_E1,USB_PORT_D1,USB_CLRSTL1,
};
USB_STATIC  const uint8_t   usb_sequence_tbl2[11] =
{
/* When the detach request is generated while processing the sequence. */
//  process CONTINUE    Detach      Attach      Current     Remote      Suspend
            USB_NOSEQE0,USB_NOSEQE1,USB_DETACH2,USB_DETACH1,USB_DETACH1,USB_DETACH1,
//          Resume      PortEnable  PortDisable StallClear
            USB_DETACH1,USB_DETACH1,USB_NOSEQE0,USB_DETACH1,
};

/* functions */
/* HCD task / MGR task */
USB_STATIC  void                usb_hstd_no_sequence(void);
USB_STATIC  void                usb_hstd_wait_sequence(void);
USB_STATIC  void                usb_hstd_attach_start(void);
USB_STATIC  void                usb_hstd_enumeration_sequence(void);
USB_STATIC  void                usb_hstd_global_suspend(void);
USB_STATIC  void                usb_hstd_suspend_start(void);
USB_STATIC  void                usb_hstd_suspend_sequence(void);
USB_STATIC  void                usb_hstd_global_resume(void);
USB_STATIC  void                usb_hstd_resume_start(void);
USB_STATIC  void                usb_hstd_resume_sequence(void);
USB_STATIC  void                usb_hstd_remote_start(void);
USB_STATIC  void                usb_hstd_remote_sequence(void);
USB_STATIC  void                usb_hstd_detach_start(void);
USB_STATIC  void                usb_hstd_enumeration_detach(void);
USB_STATIC  void                usb_hstd_overcurrent_start(void);
USB_STATIC  void                usb_hstd_portenable_start(void);
USB_STATIC  void                usb_hstd_portenable_sequence(void);
USB_STATIC  void                usb_hstd_portdisable_start(void);
USB_STATIC  void                usb_hstd_portdisable_sequence(void);
USB_STATIC  void                usb_hstd_clearstall_start(void);
USB_STATIC  void                usb_hstd_clearstall_sequence(void);
USB_STATIC  void                usb_hstd_mgr_wait(uint16_t time);
USB_STATIC  void                usb_hstd_sequence_clear(void);
USB_STATIC  void                usb_hstd_sequence_end(void);
USB_STATIC  uint8_t             usb_hstd_sequence_port(void);
USB_STATIC  uint8_t             usb_hstd_sequence_addr(void);
USB_STATIC  uint8_t             usb_hstd_sequence_pipe(void);
USB_STATIC  uint8_t             usb_hstd_enumeration(void);
USB_STATIC  void                usb_hstd_update_devinfo(uint8_t state);
USB_STATIC  void                usb_hstd_update_devstate(uint8_t state);
USB_STATIC  usb_port_t          usb_hstd_devaddr_2_port(void);
USB_STATIC  void                usb_hstd_transfer_start(void);
USB_STATIC  usb_er_t            usb_hstd_get_descriptor(uint16_t CntValue);
USB_STATIC  usb_er_t            usb_hstd_set_asddress(void);
USB_STATIC  usb_er_t            usb_hstd_set_configuration(void);
USB_STATIC  void                usb_hstd_dummy_request(void);
/* control transfer function */
USB_STATIC  void                usb_hstd_setup_start(void);
/* USB Host control driver libraries */
USB_STATIC  uint16_t            usb_hstd_get_devaddx_register(void);
/* standard request functions */
USB_STATIC  usb_er_t            usb_hstd_set_feature(uint16_t epnum);
USB_STATIC  usb_er_t            usb_hstd_clear_feature(uint16_t epnum);
USB_STATIC  usb_er_t            usb_hstd_do_command(uint8_t* tranadr);
/* other library functions */
USB_STATIC  uint8_t             usb_hstd_pipe_2_epadr(void);
/* HCD to MGR, APL(class driver) to MGR, MGR to HCD, HCD to MGR command control function */
USB_STATIC  usb_er_t            usb_hstd_status_change(usb_strct_t msginfo, usb_strct_t keyword);
USB_STATIC  void                usb_hstd_status_result(usb_strct_t status);
USB_STATIC  void                usb_hstd_transfer_result(usb_utr_t *utrmsg);
USB_STATIC  void                usb_hstd_transferend_check(usb_strct_t status);
USB_STATIC  void                usb_hstd_setup_clear_status(void);
USB_STATIC  void                usb_hstd_setup_enable(void);
/* Processing function table */
USB_STATIC  const mgrfunc_t mgrfunctbl[] =
{
    usb_hstd_no_sequence,               /* No sequence processing. */
    usb_hstd_wait_sequence,             /* The message is preserved in the wait queue table. */
    usb_hstd_attach_start,              /* Start of attach sequence */
    usb_hstd_enumeration_sequence,      /* Enumeration sequence */
    usb_hstd_global_suspend,            /* Start of global suspend(select port) */
    usb_hstd_suspend_start,             /* Start of selective suspend(select device) */
    usb_hstd_suspend_sequence,          /* Suspend sequence */
    usb_hstd_global_resume,             /* Start of global resume(select port) */
    usb_hstd_resume_start,              /* Start of selective resume(select device) */
    usb_hstd_resume_sequence,           /* Resume sequence */
    usb_hstd_remote_start,              /* Start of remote wakeup sequence */
    usb_hstd_remote_sequence,           /* Remote wakeup sequence */
    usb_hstd_detach_start,              /* Start of detach processing */
    usb_hstd_detach_start,              /* Start of detach processing in sequence continuance */
    usb_hstd_enumeration_detach,        /* Start of detach processing in enumeration sequence */
    usb_hstd_overcurrent_start,         /* Start of over current processing */
    usb_hstd_portenable_start,          /* Start of port enable sequence */
    usb_hstd_portenable_sequence,       /* Port enable sequence */
    usb_hstd_portdisable_start,         /* Start of port disable sequence */
    usb_hstd_portdisable_sequence,      /* Port disable sequence */
    usb_hstd_clearstall_start,          /* Start of STALL clear sequence */
    usb_hstd_clearstall_sequence,       /* STALL clear sequence */
};


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */
/* Host control driver variables */
usb_port_t          g_usb_HcdPort;
usb_tskinfo_t       *g_usb_HcdMessage;                          /* Hcd Task receive message */
usb_hcdreg_t        g_usb_HcdDriver[USB_MAXDEVADDR];            /* Device driver (registration) */
uint8_t             g_usb_HcdRemotePort[USB_PORTSEL_PP];
uint8_t             g_usb_HcdCtsq;                              /* Control transfer stage management */
usb_addr_t          g_usb_HcdDevAddr;                           /* Device address */
usb_addr_t          g_usb_MgrDevAddr;                           /* Device address */
uint8_t             g_usb_HcdDevNum;                            /* Device driver number */
/* Host manager variables */
usb_hcddevinf_t     g_usb_HcdDevInfo[USB_MAXDEVADDR+1u];        /* port, status, config, speed, */
uint8_t             g_usb_MgrSequence;
uint8_t             g_usb_MgrSeqMode;
usb_pipe_t          g_usb_MgrPipe;                              /* Pipe number */

/* functions */
/* HCD task / MGR task */
void                usb_hstd_hcd_task(void);
void                usb_hstd_mgr_task(void);
void                usb_hstd_interrupt_handler(void);
void                usb_hstd_clear_device_infomation(void);
/* control transfer function */
uint16_t            usb_hstd_control_write_start(void);
void                usb_hstd_control_read_start(void);
void                usb_hstd_status_start(void);
void                usb_hstd_control_end(usb_strct_t Status);
/* FIFO buffer interrupt */
void                usb_hstd_brdy_pipe(void);
void                usb_hstd_nrdy_pipe(void);
void                usb_hstd_bemp_pipe(void);
/* USB Host control driver libraries */
uint8_t             usb_hstd_devaddr_2_speed(void);
uint8_t             usb_hstd_check_device_address(void);
void                usb_hstd_set_devaddx_register(uint8_t usbspd, usb_port_t port);
/* Device state control function */
uint8_t             usb_hstd_check_attach(void);
/* other library functions */
void                usb_hstd_pipe_2_hcddevaddr(void);
usb_addr_t          usb_hstd_pipe_2_addr(void);
/* HCD to MGR, APL(class driver) to MGR, MGR to HCD, HCD to MGR command control function */
void                usb_hstd_status_notification(usb_strct_t msginfo, usb_strct_t keyword);
void                usb_hstd_setup_disable(void);
void                usb_hstd_port_enable(void);



/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* variables */
extern usb_leng_t       g_usb_LibDataCnt[];                 /* PIPEn Buffer counter */
extern uint8_t          *g_usb_LibDataPtr[];                    /* PIPEn Buffer pointer(8bit) */
extern usb_utr_t        *g_usb_LibPipe[];                   /* Message pipe */
extern uint16_t         g_usb_LibTemp;                      /* macro temp */
extern usb_pipe_t       g_usb_CurrentPipe;                  /* Pipe number */
extern usb_intinfo_t    g_usb_LibIntMessage[];
extern uint8_t          g_usb_LibIntMsgCnt;

/* functions */
#ifdef __RX
extern void             usb_cpu_d0fifo_interrupt_process();
#endif  /* __RX */

extern void             usb_cpu_delay_xms(uint16_t time);
#ifdef  __RX
extern void             usb_hstd_check_interrupt_source(usb_strct_t *inttype, uint16_t *bitsts);
#else   /* __RX */
extern void             usb_hstd_check_interrupt_source(usb_strct_t *inttype, usb_strct_t *bitsts);
#endif  /* __RX */
extern void             usb_hstd_interrupt_process(void);
extern void             usb_hstd_detach_control(void);
extern void             usb_hstd_read_lnst(uint16_t *buf);
extern void             usb_hstd_attach_control(void);
extern void             usb_hstd_bus_reset(void);
extern void             usb_hstd_resume_process(void);
extern void             usb_hstd_vbus_control_on(void);
extern void             usb_hstd_vbus_control_off(void);
extern void             usb_hstd_suspend_process(void);
extern usb_strct_t      usb_hstd_port_speed(void);
extern void             usb_cstd_send_start(void);
extern uint8_t          usb_cstd_buf_2_cfifo(void);
extern void             usb_cstd_receive_start(void);
extern uint8_t          usb_cstd_cfifo_2_buf(void);
extern void             usb_cstd_forced_termination(uint16_t status);
extern void             usb_cstd_set_cfifo_pipe0r(void);
extern void             usb_cstd_set_cfifo_pipe0w(void);
extern void             usb_cstd_brdy_pipe(uint16_t bitsts);
extern void             usb_cstd_nrdy_pipe(uint16_t bitsts);
extern void             usb_cstd_bemp_pipe(uint16_t bitsts);
extern void             usb_cstd_brdy_enable(void);
extern void             usb_cstd_brdy_disable(void);
extern void             usb_cstd_bemp_enable(void);
extern void             usb_cstd_bemp_disable(void);
extern void             usb_cstd_nrdy_enable(void);
extern void             usb_cstd_nrdy_disable(void);
extern uint16_t         usb_cstd_get_pipe_dir(void);
extern void             usb_cstd_reg_set_pipexctr(uint16_t field);
extern uint16_t         usb_cstd_reg_read_pipexctr(void);
extern void             usb_cstd_clr_stall(void);
extern uint16_t         usb_cstd_get_pid(void);
extern void             usb_cstd_dummy_function(uint16_t data1, uint16_t data2);

#ifdef USB_HOST_BC_ENABLE
extern usb_bc_status_t g_usb_hstd_bc[];
#endif


/******************************************************************************
Renesas Abstracted USB Host Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_hcd_task
Description     : USB Host Control Driver Task
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_hcd_task(void)
{
    usb_tskinfo_t   *mess;
    usb_er_t        err;

    err = R_USB_RCV_MSG(USB_HCD_MBX, (usb_msg_t**)&mess);

    /* Message check */
    if (err != USB_E_OK)
    {
        return;
    }

    g_usb_HcdMessage    = (usb_tskinfo_t*)mess;
    g_usb_HcdPort       = (usb_port_t)g_usb_HcdMessage->keyword;
    g_usb_CurrentPipe   = (usb_pipe_t)g_usb_HcdMessage->keyword;

    /* Branch Hcd Task receive Message Command */
    switch( g_usb_HcdMessage->msginfo )
    {
        /* USB interrupt */
        case USB_INTERRUPT:                 /* USB interrupt */
            usb_hstd_interrupt_process();
        break;

#ifdef __RX
        /* USB D0FIFO interrupt */
        case USB_DO_D0FIFO_INTERRUPT:           /* USB D0FIFO interrupt */
            usb_cpu_d0fifo_interrupt_process();
        break;
#endif  /* __RX */

        /* USB transfer */
        case USB_DO_TRANSFERSTART:          /* Transfer start */
            usb_hstd_transfer_start();
        break;
        /* USB Transfer End Stop */
        case USB_DO_TRANSFER_STP:
            usb_hstd_transferend_check(USB_DATA_STOP);
        break;
        /* USB Transfer End Timeout */
        case USB_DO_TRANSFER_TMO:
            usb_hstd_transferend_check(USB_DATA_TMO);
        break;
        /* USB state control */
        case USB_HCD_USBRESET:          /* USB bus reset */
            //delay aqui
            usb_hstd_bus_reset();
            usb_hstd_status_result(USB_HCD_USBRESET);
        break;
        case USB_HCD_SUSPEND:           /* Suspend device */
            usb_hstd_suspend_process();
            usb_hstd_status_result(USB_HCD_SUSPEND);
        break;
        case USB_HCD_RESUME:            /* USB resume */
            usb_hstd_resume_process();
            usb_hstd_status_result(USB_HCD_RESUME);
        break;
        case USB_HCD_VBON:              /* USB VBUS control ON */
            usb_hstd_vbus_control_on();
#ifndef USB_HOST_BC_ENABLE
            usb_cpu_delay_xms((uint16_t)100u);
#endif  /* USB_HOST_BC_ENABLE */
            usb_hstd_status_result(USB_HCD_VBON);
        break;
        case USB_HCD_VBOFF:             /* USB VBUS control OFF */
            usb_hstd_vbus_control_off();
#ifndef USB_HOST_BC_ENABLE
            usb_cpu_delay_xms((uint16_t)100);
#endif  /* USB_HOST_BC_ENABLE */
            usb_hstd_status_result(USB_HCD_VBOFF);
        break;
        case USB_HCD_CLEAR_STALL:
            usb_cstd_clr_stall();
            usb_cstd_reg_set_pipexctr(USB_SQCLR);
            usb_hstd_status_result(USB_HCD_CLEAR_STALL);
        break;

        default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_HOST | USB_DEBUG_HOOK_CODE3);
        break;
    }

}   /* eof usb_hstd_hcd_task() */


/******************************************************************************
Function Name   : usb_hstd_transfer_start
Description     : USB data transfer/control transfer start
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_transfer_start(void)
{
    uint8_t connect_inf;

    g_usb_CurrentPipe                   = (usb_pipe_t)g_usb_HcdMessage->keyword;
    g_usb_LibPipe[g_usb_CurrentPipe]    = (usb_utr_t*)g_usb_HcdMessage;

    /* Get device address from pipe number */
    if( g_usb_CurrentPipe == USB_PIPE0 )
    {
        g_usb_HcdDevAddr    = (usb_addr_t)g_usb_LibPipe[g_usb_CurrentPipe]->setup[4];

        /* Get device speed from device address */
        connect_inf = usb_hstd_devaddr_2_speed();
        if( connect_inf == USB_NOCONNECT )
        {
            /* Control Read/Write End */
            usb_hstd_control_end(USB_DATA_DTCH);
            return;
        }

        /* Control transfer idle stage ? */
        if( g_usb_HcdCtsq == USB_IDLEST )
        {
            usb_hstd_setup_start();
        }
        else
        {
            /* Control Read/Write End */
            usb_hstd_control_end(USB_DATA_ERR);
        }
    }
    else
    {
        usb_hstd_pipe_2_hcddevaddr();
        if (g_usb_HcdDevAddr == USB_DEVICE_0)
        {
            /* End of data transfer (IN/OUT) */
            usb_cstd_forced_termination(USB_DATA_ERR);
            return;
        }

        /* Get device speed from device address */
        connect_inf = usb_hstd_devaddr_2_speed();
        if( connect_inf == USB_NOCONNECT )
        {
            /* End of data transfer (IN/OUT) */
            usb_cstd_forced_termination(USB_DATA_DTCH);
            return;
        }

        /* Data transfer */
        if( usb_cstd_get_pipe_dir() == USB_DIR_H_IN )
        {
            usb_cstd_receive_start();
        }
        else
        {
            usb_cstd_send_start();
        }
    }
}   /* eof usb_hstd_transfer_start() */


/******************************************************************************
Function Name   : usb_hstd_interrupt_handler
Description     : USB interrupt handler
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_interrupt_handler(void)
{
    usb_intinfo_t       *ptr;

    ptr = &g_usb_LibIntMessage[g_usb_LibIntMsgCnt];
    usb_hstd_check_interrupt_source(&ptr->inttype, &ptr->bitsts);
    if( R_USB_ISND_MSG(USB_HCD_MBX, (usb_msg_t*)ptr) != USB_E_OK )
    {
        USB_DEBUG_HOOK(USB_DEBUG_HOOK_HOST | USB_DEBUG_HOOK_CODE4);
    }

    /* Renewal Message count */
    g_usb_LibIntMsgCnt++;
    if( g_usb_LibIntMsgCnt == USB_INTMSGMAX )
    {
        g_usb_LibIntMsgCnt  = USB_0;
    }
}   /* eof usb_hstd_interrupt_handler() */


/******************************************************************************
Function Name   : usb_hstd_mgr_task
Description     : Manager Task
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_mgr_task(void)
{
    usb_tskinfo_t   *mess;
    uint8_t         fncno;

    /* Receive message */
    if (R_USB_RCV_MSG(USB_MGR_MBX, (usb_msg_t**)&mess) != USB_E_OK)
    {
        return;
    }

    g_usb_MgrMessage    = (usb_tskinfo_t*)mess;
    if ((g_usb_MgrMessage->msginfo < USB_MGR_CONTINUE) || (g_usb_MgrMessage->msginfo > USB_MGR_NOSEQUENCE))
    {
        R_USB_REL_BLK(g_usb_MgrMessage->flag);
        return;
    }

    if (g_usb_MgrSeqMode == USB_MGR_NOSEQUENCE)
    {
        /* When the sequence processing starts newly by the sequence unprocessing now. */
        fncno = usb_sequence_tbl0[(g_usb_MgrMessage->msginfo - USB_MGR_CONTINUE)];
    }
    else if (g_usb_MgrMessage->msginfo == USB_RTP_DETACH)
    {
        /* When the detach request is generated while processing the sequence. */
        if (g_usb_MgrPort == g_usb_MgrMessage->keyword)
        {
            fncno = usb_sequence_tbl2[g_usb_MgrSeqMode];
        }
        else
        {
            fncno = USB_NOSEQE1;
        }
    }
    else if (g_usb_MgrMessage->msginfo == USB_MGR_CONTINUE)
    {
        /* When the sequence processing is continuas. */
        fncno = usb_sequence_tbl1[g_usb_MgrSeqMode];
    }
    else
    {
        /* Because the sequence is being processed, the request is reserved. */
        fncno = USB_NOSEQE1;
    }

    /* Sequence processing */
    (*mgrfunctbl[fncno])();

}   /* eof usb_hstd_mgr_task() */


/******************************************************************************
Function Name   : usb_hstd_no_sequence
Description     : No sequence processing
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_no_sequence(void)
{
    R_USB_REL_BLK(g_usb_MgrMessage->flag);
}   /* eof usb_hstd_no_sequence() */


/******************************************************************************
Function Name   : usb_hstd_wait_sequence
Description     : The message is preserved in the wait queue table
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_wait_sequence(void)
{
    usb_hstd_mgr_wait(50);
}   /* eof usb_hstd_wait_sequence() */


/******************************************************************************
    Attach sequence functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_attach_start
Description     : Start of attach sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_attach_start(void)
{
    usb_hcdreg_t    *driver;
    uint8_t         md;

    /* Attaching under the configuration is disregarded. */
    for( g_usb_MgrDevAddr = USB_MAXDEVADDR;  USB_DEVICEADDR <= g_usb_MgrDevAddr; g_usb_MgrDevAddr-- )
    {
        if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].port == g_usb_MgrMessage->keyword)
        {
            usb_hstd_sequence_end();
            return;
        }
    }

    usb_hstd_sequence_port();
    for( md = 0; md < g_usb_HcdDevNum; md++ )
    {
        /* The callback is done to all registered drivers.  */
        driver  = &g_usb_HcdDriver[md];
        if (driver->devaddr == USB_NODEVICE)
        {
            g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = USB_STS_ATTACH;
#ifdef USB_HOST_BC_ENABLE
            (*driver->statediagram)( g_usb_hstd_bc[g_usb_HcdPort].state, USB_STS_ATTACH);
#else /* USB_HOST_BC_ENABLE */
            (*driver->statediagram)(g_usb_MgrPort, USB_STS_ATTACH);
#endif /* USB_HOST_BC_ENABLE */
        }
    }
    usb_hstd_mgr_wait(USB_TATTDB);
    return;
}   /* eof usb_hstd_attach_start() */


/******************************************************************************
    Enumeration sequence functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_enumeration_sequence
Description     : Enumeration sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_enumeration_sequence(void)
{
    usb_hcdreg_t    *driver;
    uint8_t         enume_mode;
    uint8_t         md;

    switch( g_usb_MgrSequence )
    {
        case    USB_0:
            g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = USB_STS_DEFAULT;
            if ((g_usb_MgrDevAddr == USB_DEVICEADDR) || (g_usb_MgrDevAddr == (USB_DEVICEADDR+1)))
            {
                usb_hstd_status_change(USB_HCD_USBRESET, (usb_strct_t)g_usb_MgrPort);
            }
        break;
        case    USB_1:
            for( md = 0; md < g_usb_HcdDevNum; md++ )
            {
                /* The callback is done to all registered drivers.  */
                driver  = &g_usb_HcdDriver[md];
                (*driver->statediagram)(g_usb_MgrPort, USB_STS_DEFAULT);
            }
            g_usb_HcdPort               = g_usb_MgrPort;
            g_usb_HcdDevSpeed           = usb_hstd_port_speed();
            g_usb_HcdDevAddr            = USB_0;
            if (g_usb_HcdDevSpeed == USB_NOCONNECT)
            {
                usb_hstd_set_devaddx_register(USB_NOCONNECT, g_usb_HcdPort);
            }
            else
            {
                usb_hstd_set_devaddx_register(g_usb_HcdDevSpeed, g_usb_HcdPort);
            }
            g_usb_HcdDcpRegister[USB_0] = (uint16_t)(USB_DEFPACKET);

            if (usb_hstd_devaddr_2_speed() == USB_LSCONNECT)
            {
                USB_SET_PAT(SOFCFG, USB_TRNENSEL);
            }
            else
            {
                USB_CLR_PAT(SOFCFG, USB_TRNENSEL);
            }
    
            usb_hstd_get_descriptor(USB_0);
            R_USB_REL_BLK(g_usb_MgrMessage->flag);
        break;
        case    USB_7:
            switch(g_usb_MgrMessage->keyword)
            {
                case    USB_NO:             /* The class driver responds when it is not possible to operate. */
                    /* The pre-register is released and the class driver is retrieved again */
                    driver = &g_usb_HcdDriver[g_usb_HcdRegPointer];
                    driver->rootport    = USB_NOPORT;
                    driver->devaddr     = USB_NODEVICE;
                    g_usb_HcdRegPointer++;
                    g_usb_MgrSequence   = USB_6;
                    usb_hstd_enumeration();
                break;
                case    USB_YES:            /* The class driver responds when it is possible to operate. */
                    usb_hstd_set_configuration();
                break;
                case    USB_ERROR:          /* The class driver is not found.  */
                /* continue */
                default:
                break;
            }
            R_USB_REL_BLK(g_usb_MgrMessage->flag);
        break;
        default:
            if (g_usb_MgrControlMessage.status == USB_CTRL_END)
            {
                enume_mode = usb_hstd_enumeration();
                switch( enume_mode )
                {
                    case USB_NONDEVICE:         /* Detach detection or enumeration error */
                        usb_hstd_sequence_clear();
                    break;
                    case USB_COMPLETEPIPESET:   /* Enumeration complete */
                        g_usb_MgrDevAddr    = USB_DEVICE_0;     /* DEVICE_0is not connected */
                        usb_hstd_clear_device_infomation();
                        usb_hstd_sequence_clear();
                    break;
                    default:                    /* Enumeration sequence */
                    break;
                }
            }
            else
            {
                /* Enumeration error */
                usb_hstd_sequence_clear();
            }
        break;
    }
}   /* eof usb_hstd_enumeration_sequence() */


/******************************************************************************
    Suspend sequence functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_global_suspend
Description     : Start of global suspend(select port)
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_global_suspend(void)
{
    for( g_usb_MgrDevAddr = USB_MAXDEVADDR;  USB_DEVICEADDR <= g_usb_MgrDevAddr; g_usb_MgrDevAddr-- )
    {
        if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].port == g_usb_MgrMessage->keyword)
        {
            if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_STS_CONFIGURED)
            {
                g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = USB_DO_GLOBAL_SUSPEND;
                R_USB_GET_SND(USB_MGR_MBX, USB_DO_SELECTIVE_SUSPEND, g_usb_MgrMessage->complete, g_usb_MgrDevAddr);
            }
        }
    }
    R_USB_REL_BLK(g_usb_MgrMessage->flag);
}   /* eof usb_hstd_global_suspend() */

/******************************************************************************
Function Name   : usb_hstd_suspend_start
Description     : Start of selective suspend(select device)
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_suspend_start(void)
{
    if (usb_hstd_sequence_addr() == USB_CONNECT)
    {
        if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_STS_CONFIGURED)
        {
            g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = USB_DO_SELECTIVE_SUSPEND;
            usb_hstd_get_descriptor(USB_4);
            R_USB_REL_BLK(g_usb_MgrMessage->flag);
        }
        else if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_DO_GLOBAL_SUSPEND)
        {
            usb_hstd_get_descriptor(USB_4);
            R_USB_REL_BLK(g_usb_MgrMessage->flag);
        }
        else
        {
            usb_hstd_sequence_end();
        }
    }
}   /* eof usb_hstd_suspend_start() */

/******************************************************************************
Function Name   : usb_hstd_suspend_sequence
Description     : Suspend sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_suspend_sequence(void)
{
    switch( g_usb_MgrSequence )
    {
        case USB_1:
            /* Because it is that the Control forwarding is sure to end correctly, */
            /* GetDescriptor omits the normal termination of control forwarding judgment. */
            if (g_usb_MgrConfDescr[USB_CON_ATTRIBUTE] & USB_CF_RWUPON)
            {
                usb_hstd_set_feature((uint16_t)0xFF);
            }
            else
            {
                g_usb_MgrSequence++;
                g_usb_HcdRemotePort[g_usb_MgrPort] = USB_NO;
                usb_hstd_status_change(USB_HCD_SUSPEND, (usb_strct_t)g_usb_MgrPort);
            }
        break;
        case USB_2:
            /* Because it is that the Control forwarding is sure to end correctly, */
            /* SetFeature(enable wakeup) omits the normal termination of control forwarding judgment. */
            g_usb_HcdRemotePort[g_usb_MgrPort] = USB_YES;
            usb_hstd_status_change(USB_HCD_SUSPEND, (usb_strct_t)g_usb_MgrPort);
        break;
        case USB_3:
            usb_hstd_update_devstate(USB_STS_SUSPEND);
            if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_DO_SELECTIVE_SUSPEND)
            {
                g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = USB_STS_DS_SUSPEND;
            }
            if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_DO_GLOBAL_SUSPEND)
            {
                g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = USB_STS_PS_SUSPEND;
            }
            (*g_usb_MgrCallback)( g_usb_MgrDevAddr, USB_STS_SUSPEND );
            usb_hstd_sequence_end();
        break;
        default:
            /* no processing */
            usb_hstd_sequence_end();
        break;
    }
}   /* eof usb_hstd_suspend_sequence() */


/******************************************************************************
    Resume sequence functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_global_resume
Description     : Start of global resume(select port)
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_global_resume(void)
{
    for( g_usb_MgrDevAddr = USB_DEVICEADDR; g_usb_MgrDevAddr <= USB_MAXDEVADDR; g_usb_MgrDevAddr++ )
    {
        if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].port == g_usb_MgrMessage->keyword)
        {
            if ((g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_STS_DS_SUSPEND) ||
                (g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_STS_PS_SUSPEND))
            {
                g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = USB_DO_GLOBAL_RESUME;
                R_USB_GET_SND(USB_MGR_MBX, USB_DO_SELECTIVE_RESUME, g_usb_MgrMessage->complete, g_usb_MgrDevAddr);
            }
        }
    }
    R_USB_REL_BLK(g_usb_MgrMessage->flag);
}   /* eof usb_hstd_global_resume() */

/******************************************************************************
Function Name   : usb_hstd_resume_start
Description     : Start of selective resume(select device)
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_resume_start(void)
{
    if (usb_hstd_sequence_addr() == USB_CONNECT)
    {
        if ((g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_STS_DS_SUSPEND) ||
            (g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_STS_PS_SUSPEND))
        {
            usb_hstd_status_change(USB_HCD_RESUME, (usb_strct_t)g_usb_MgrPort);
        }
        else if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_DO_GLOBAL_RESUME)
        {
            if ((g_usb_MgrDevAddr == USB_DEVICEADDR) || (g_usb_MgrDevAddr == (USB_DEVICEADDR+1)))
            {
                usb_hstd_status_change(USB_HCD_RESUME, (usb_strct_t)g_usb_MgrPort);
            }
        }
        else
        {
            usb_hstd_sequence_end();
        }
    }

    R_USB_REL_BLK( g_usb_MgrMessage->flag );
}   /* eof usb_hstd_resume_start() */

/******************************************************************************
Function Name   : usb_hstd_resume_sequence
Description     : Resume sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_resume_sequence(void)
{
    switch( g_usb_MgrSequence )
    {
        case USB_1:
            usb_hstd_get_descriptor(USB_4);
            R_USB_REL_BLK(g_usb_MgrMessage->flag);
        break;
        case USB_2:
            /* Because it is that the Control forwarding is sure to end correctly, */
            /* GetDescriptor omits the normal termination of control forwarding judgment. */
            if (g_usb_MgrConfDescr[USB_CON_ATTRIBUTE] & USB_CF_RWUPON)
            {
                usb_hstd_clear_feature((uint16_t)0xFF);
            }
            else
            {
                usb_hstd_update_devinfo(USB_STS_CONFIGURED);
                (*g_usb_MgrCallback)( g_usb_MgrDevAddr, USB_STS_RESUME );
                usb_hstd_sequence_end();
            }
        break;
        case USB_3:
            /* Because it is that the Control forwarding is sure to end correctly, */
            /* ClearFeature(disable wakeup) omits the normal termination of control forwarding judgment. */
            usb_hstd_update_devinfo(USB_STS_CONFIGURED);
            (*g_usb_MgrCallback)( g_usb_MgrDevAddr, USB_STS_RESUME );
            usb_hstd_sequence_end();
        break;
        default:
            /* no processing */
            usb_hstd_sequence_end();
        break;
    }
}   /* eof usb_hstd_resume_sequence() */


/******************************************************************************
    Remote wakeup sequence functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_remote_start
Description     : Start of remote wakeup sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_remote_start(void)
{
    if (usb_hstd_sequence_port() == USB_CONNECT)
    {
        if ((g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_STS_DS_SUSPEND) ||
            (g_usb_HcdDevInfo[g_usb_MgrDevAddr].state == USB_STS_PS_SUSPEND))
        {
            usb_hstd_status_change(USB_HCD_RESUME, (usb_strct_t)g_usb_MgrPort);
        }
    }
    R_USB_REL_BLK(g_usb_MgrMessage->flag);
}   /* eof usb_hstd_remote_start() */

/******************************************************************************
Function Name   : usb_hstd_remote_sequence
Description     : Remote wakeup sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_remote_sequence(void)
{
    usb_hcdreg_t    *driver;
    uint8_t         md;

    switch( g_usb_MgrSequence )
    {
        case USB_1:
            usb_hstd_clear_feature((uint16_t)0xFF);
            R_USB_REL_BLK(g_usb_MgrMessage->flag);
        break;
        case USB_2:
            /* Because it is that the Control forwarding is sure to end correctly, */
            /* ClearFeature(disable wakeup) omits the normal termination of control forwarding judgment. */
            usb_hstd_update_devinfo(USB_STS_CONFIGURED);
            for( md = 0; md < g_usb_HcdDevNum; md++ )
            {
                driver  = &g_usb_HcdDriver[md];
                if (g_usb_MgrDevAddr == driver->devaddr)
                {
                    (*driver->statediagram)(g_usb_MgrDevAddr, USB_STS_WAKEUP);
                }
            }
            usb_hstd_sequence_end();
        break;
        default:
            /* no processing */
            usb_hstd_sequence_end();
        break;
    }
}   /* eof usb_hstd_remote_sequence() */


/******************************************************************************
    Detach processing functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_detach_start
Description     : Start of detach processing
                  Start of detach processing in sequence continuance
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_detach_start(void)
{
    usb_hcdreg_t    *driver;
    uint8_t         md;

    usb_hstd_sequence_port();

    for( md = 0; md < g_usb_HcdDevNum; md++ )
    {
        driver  = &g_usb_HcdDriver[md];
        if (g_usb_MgrDevAddr == driver->devaddr)
        {
            (*driver->statediagram)(g_usb_MgrDevAddr, USB_STS_DETACH);
            usb_hstd_clear_device_infomation();
            driver->rootport                = USB_NOPORT;
            driver->devaddr                 = USB_NODEVICE;
            driver->devstate                = USB_STS_DETACH;
        }
    }
    usb_hstd_sequence_end();
}   /* eof usb_hstd_detach_start() */

/******************************************************************************
Function Name   : usb_hstd_enumeration_detach
Description     : Start of detach processing in enumeration sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_enumeration_detach(void)
{
    usb_hcdreg_t    *driver;

    if (g_usb_MgrSequence == USB_6)
    {
        /* The detach request is notified while processing the class check. */
        driver = &g_usb_HcdDriver[g_usb_HcdRegPointer];
        (*driver->statediagram)(g_usb_MgrDevAddr, USB_STS_DETACH);
        usb_hstd_clear_device_infomation();
    }
    usb_hstd_detach_start();
}   /* eof usb_hstd_enumeration_detach() */


/******************************************************************************
    Over current processing functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_overcurrent_start
Description     : Start of over current processing
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_overcurrent_start(void)
{
    usb_hcdreg_t    *driver;
    uint8_t         md;

    usb_hstd_sequence_port();
    usb_hstd_clear_device_infomation();

    for( md = 0; md < g_usb_HcdDevNum; md++ )
    {
        driver = &g_usb_HcdDriver[md];
        if( driver->rootport == g_usb_MgrPort )
        {
            (*driver->statediagram)(driver->devaddr, USB_STS_OVERCURRENT);
            g_usb_MgrDevAddr    = driver->devaddr;
            usb_hstd_clear_device_infomation();
            driver->rootport    = USB_NOPORT;
            driver->devaddr     = USB_NODEVICE;
            driver->devstate    = USB_STS_PORTOFF;
        }
    }
    usb_hstd_sequence_end();
}   /* eof usb_hstd_overcurrent_start() */


/******************************************************************************
    Port enable sequence functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_portenable_start
Description     : Start of port enable sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_portenable_start(void)
{
    usb_hstd_sequence_port();
    usb_hstd_status_change(USB_HCD_VBON, (usb_strct_t)g_usb_MgrPort);
}   /* eof usb_hstd_portenable_start() */


/******************************************************************************
Function Name   : usb_hstd_portenable_sequence
Description     : Port enable sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_portenable_sequence(void)
{
    if (g_usb_MgrSequence == USB_1)
    {
        usb_hstd_update_devinfo(USB_STS_POWER);
        (*g_usb_MgrCallback)( g_usb_MgrPort, USB_STS_POWER );
    }
    usb_hstd_sequence_end();
}   /* eof usb_hstd_portenable_sequence() */


/******************************************************************************
    Port disable sequence functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_portdisable_start
Description     : Start of port disable sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_portdisable_start(void)
{
    usb_hstd_sequence_port();
    usb_hstd_status_change(USB_HCD_VBOFF, (usb_strct_t)g_usb_MgrPort);
}   /* eof usb_hstd_portdisable_start() */

/******************************************************************************
Function Name   : usb_hstd_portdisable_sequence
Description     : Port disable sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_portdisable_sequence(void)
{
    if (g_usb_MgrSequence == USB_1)
    {
        usb_hstd_update_devinfo(USB_STS_PORTOFF);
        (*g_usb_MgrCallback)( g_usb_MgrPort, USB_STS_PORTOFF );
    }
    usb_hstd_sequence_end();
}   /* eof usb_hstd_portdisable_sequence() */


/******************************************************************************
    Clear_Stall sequence functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_clearstall_start
Description     : Start of STALL clear sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_clearstall_start(void)
{
    uint16_t        dir_ep;

    if (usb_hstd_sequence_pipe() == USB_CONNECT)
    {
    dir_ep = (uint16_t)usb_hstd_pipe_2_epadr();
    usb_hstd_pipe_2_hcddevaddr();
    usb_hstd_clear_feature(dir_ep);
    }
    R_USB_REL_BLK( g_usb_MgrMessage->flag );
}   /* eof usb_hstd_clearstall_start() */

/******************************************************************************
Function Name   : usb_hstd_clearstall_sequence
Description     : STALL clear sequence
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_clearstall_sequence(void)
{
    switch( g_usb_MgrSequence )
    {
        case USB_1:
            /* Because it is that the Control forwarding is sure to end correctly, */
            /* ClearFeature(clear stall) omits the normal termination of control forwarding judgment. */
            usb_hstd_status_change(USB_HCD_CLEAR_STALL, (usb_strct_t)g_usb_MgrPipe);
        break;
        case USB_2:
            (*g_usb_MgrCallback)( g_usb_MgrPipe, USB_STALL_SUCCESS );
            usb_hstd_sequence_end();
        break;
        default:
            /* no processing */
            usb_hstd_sequence_end();
        break;
    }
}   /* eof usb_hstd_clearstall_sequence() */


/******************************************************************************
    Common wait sequence
******************************************************************************/
/******************************************************************************
Function Name   : usb_hstd_mgr_wait
Description     : Manager sequence message wait
Argument        : wait times
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_mgr_wait(uint16_t count)
{
    usb_er_t        err;

    err = R_USB_WAI_MSG(USB_MGR_MBX, (usb_msg_t*)g_usb_MgrMessage, count);
    if( err != USB_E_OK )
    {
        USB_DEBUG_HOOK(USB_DEBUG_HOOK_HOST | USB_DEBUG_HOOK_CODE5);
    }
}   /* eof usb_hstd_mgr_wait() */


/******************************************************************************
Function Name   : usb_hstd_sequence_clear
Description     : Sequence clear
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_sequence_clear(void)
{
    g_usb_MgrSeqMode    = USB_MGR_NOSEQUENCE;
    g_usb_MgrSequence   = USB_0;
}   /* eof usb_hstd_sequence_clear() */


/******************************************************************************
Function Name   : usb_hstd_sequence_end
Description     : Sequence end
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_sequence_end(void)
{
    usb_hstd_sequence_clear();
    R_USB_REL_BLK(g_usb_MgrMessage->flag);
}   /* eof usb_hstd_sequence_end() */


/******************************************************************************
Function Name   : usb_hstd_sequence_port
Description     : Sequence port start
Argument        : none
Return          : USB_CONNECT/USB_NOCONNECT
******************************************************************************/
USB_STATIC uint8_t usb_hstd_sequence_port(void)
{
    g_usb_MgrPort       = (usb_port_t)g_usb_MgrMessage->keyword;
    g_usb_MgrCallback   = g_usb_MgrMessage->complete;
    g_usb_MgrSeqMode    = (g_usb_MgrMessage->msginfo - USB_MGR_CONTINUE);
    g_usb_MgrSequence   = USB_0;
    g_usb_MgrDevAddr    = (usb_addr_t)(g_usb_MgrPort + USB_DEVICEADDR);
    g_usb_MgrMessage->msginfo   = USB_MGR_CONTINUE;
    if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].port == g_usb_MgrPort)
    {
        return USB_CONNECT;
    }
    return USB_NOCONNECT;
}   /* eof usb_hstd_sequence_port() */


/******************************************************************************
Function Name   : usb_hstd_sequence_addr
Description     : Sequence device address start
Argument        : none
Return          : USB_CONNECT/USB_NOCONNECT
******************************************************************************/
USB_STATIC uint8_t usb_hstd_sequence_addr(void)
{
    g_usb_MgrDevAddr    = (usb_addr_t)g_usb_MgrMessage->keyword;
    g_usb_MgrCallback   = g_usb_MgrMessage->complete;
    g_usb_MgrSeqMode    = (g_usb_MgrMessage->msginfo - USB_MGR_CONTINUE);
    g_usb_MgrSequence   = USB_0;
    g_usb_HcdDevAddr    = g_usb_MgrDevAddr;
    g_usb_MgrPort       = usb_hstd_devaddr_2_port();
    g_usb_HcdPort       = g_usb_MgrPort;
    if (g_usb_HcdDevInfo[g_usb_MgrDevAddr].port == g_usb_MgrPort)
    {
        return USB_CONNECT;
    }
    usb_hstd_sequence_end();
    return USB_NOCONNECT;
}   /* eof usb_hstd_sequence_addr() */

/******************************************************************************
Function Name   : usb_hstd_sequence_pipe
Description     : Sequence pipe start
Argument        : none
Return          : USB_CONNECT/USB_NOCONNECT
******************************************************************************/
USB_STATIC uint8_t usb_hstd_sequence_pipe(void)
{
    g_usb_CurrentPipe   = (usb_port_t)g_usb_MgrMessage->keyword;
    g_usb_MgrPipe       = g_usb_CurrentPipe;
    g_usb_MgrCallback   = g_usb_MgrMessage->complete;
    g_usb_MgrSeqMode    = (g_usb_MgrMessage->msginfo - USB_MGR_CONTINUE);
    g_usb_MgrSequence   = USB_0;
    g_usb_MgrDevAddr    = usb_hstd_pipe_2_addr();
    g_usb_MgrMessage->msginfo   = USB_MGR_CONTINUE;
    if (g_usb_MgrDevAddr != USB_DEVICE_0)
    {
        return USB_CONNECT;
    }
    return USB_NOCONNECT;
}   /* eof usb_hstd_sequence_pipe() */


/******************************************************************************
Function Name   : usb_hstd_devaddr_2_port
Description     : Get device port from device address
Arguments       : none
Return value    : usb_port_t                  : root port
Note            : Use also to a connection check is possible
******************************************************************************/
USB_STATIC usb_port_t usb_hstd_devaddr_2_port(void)
{
    usb_port_t  port;

    port    = (usb_port_t)(g_usb_MgrDevAddr - USB_DEVICEADDR);
    if (port < USB_PORTSEL_PP)
    {
        return port;
    }
    return USB_NODEVICE;
}   /* eof usb_hstd_devaddr_2_port() */


/******************************************************************************
Function Name   : usb_hstd_enumeration
Description     : USB Enumeration
Argument        : none
Return          : uint16_t                  : enumeration status
******************************************************************************/
USB_STATIC uint8_t usb_hstd_enumeration(void)
{
    uint16_t        md;
    usb_hcdreg_t    *driver;
    uint8_t         *table[3];

    switch( g_usb_MgrSequence )
    {
        case USB_2:                 /* Receive Device Descriptor */
            /* Set device speed */
            g_usb_HcdPort       = g_usb_MgrPort;
            g_usb_HcdDevAddr    = g_usb_MgrDevAddr;
            usb_hstd_set_devaddx_register(g_usb_HcdDevSpeed, g_usb_HcdPort);
            g_usb_HcdDcpRegister[g_usb_MgrDevAddr]
                = (uint16_t)((uint16_t)(g_usb_MgrDevDesc[USB_DEV_MAX_PKT_SIZE] & USB_MAXPFIELD) |
                             (USB_ADDR2DEVSEL(g_usb_MgrDevAddr)));
            usb_hstd_set_asddress();
            g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = USB_STS_ADDRESS;
        break;
        case USB_3:             /* Set Address */
        /* continue */
        case USB_4:             /* Receive Device Descriptor(18) */
        /* continue */
        case USB_5:             /* Receive Configuration Descriptor(9) */
            if (usb_hstd_get_descriptor(g_usb_MgrSequence) != USB_E_OK)
            {
                return (USB_NONDEVICE);
            }
            g_usb_HcdRegPointer = 0;
        break;
        case USB_6:             /* Receive Configuration Descriptor(xx) */
            /* Device enumeration function */
            for( ; g_usb_HcdRegPointer < g_usb_HcdDevNum; g_usb_HcdRegPointer++ ) 
            {
                driver = &g_usb_HcdDriver[g_usb_HcdRegPointer];
                if( driver->devstate == USB_STS_DETACH ) 
                {
                    /* To judge the detach while the class driver is operating, the pre-register is executed. */
                    driver->rootport    = g_usb_MgrPort;
                    driver->devaddr     = g_usb_MgrDevAddr;
                    table[0]    = g_usb_MgrDevDesc;
                    table[1]    = g_usb_MgrConfDescr;
                    table[2]    = (uint8_t*)&g_usb_MgrDevAddr;

                    g_usb_HcdDevInfo[g_usb_MgrDevAddr].port     = g_usb_MgrPort;
                    g_usb_HcdDevInfo[g_usb_MgrDevAddr].speed    = g_usb_HcdDevSpeed;

                    (*driver->classcheck)(table);
                    return (USB_DEVICEENUMERATION);
                    /* In this function, check device class of enumeration flow move to class */
                    /* "R_usb_hstd_ReturnEnuMGR()" is used to return */
                }
            }
            if (g_usb_HcdRegPointer == g_usb_HcdDevNum)
            {
                R_usb_hstd_ReturnEnuMGR(USB_ERROR);
            }
        break;
        case USB_8:             /* Set Configuration */
            /* Device enumeration function */
            for( md = 0; md < g_usb_HcdDevNum; md++ )
            {
                driver = &g_usb_HcdDriver[md];
                if( g_usb_MgrDevAddr == driver->devaddr )
                {
                    g_usb_HcdDevInfo[g_usb_MgrDevAddr].port     = g_usb_MgrPort;
                    g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = USB_STS_CONFIGURED;
                    g_usb_HcdDevInfo[g_usb_MgrDevAddr].config   = g_usb_MgrConfDescr[USB_CON_CONFIG_VAL];
                    g_usb_HcdDevInfo[g_usb_MgrDevAddr].speed    = g_usb_HcdDevSpeed;
                    /* Device state */
                    driver->devstate                        = USB_STS_CONFIGURED;
                    (*driver->statediagram)(driver->devaddr, USB_STS_CONFIGURED);
                    return (USB_COMPLETEPIPESET);
                }
            }
            return (USB_COMPLETEPIPESET);
        break;
        default:
            usb_hstd_dummy_request();
        break;
    }
    return (USB_DEVICEENUMERATION);
}   /* eof usb_hstd_enumeration() */


/******************************************************************************
Function Name   : usb_hstd_get_descriptor
Description     : Set GetDescriptor
Argument        : uint16_t cnt_value        : Enumeration sequence
Return          : usb_er_t
******************************************************************************/
USB_STATIC usb_er_t usb_hstd_get_descriptor(uint16_t cnt_value)
{
    uint8_t     *data_table;

    g_usb_MgrRequest.WORD.BYTE.bmRequestType    = USB_REQUEST_TYPE(USB_DEV_TO_HOST,USB_STANDARD,USB_DEVICE);
    g_usb_MgrRequest.WORD.BYTE.bRequest         = USB_GET_DESCRIPTOR;
    g_usb_MgrRequest.Address                    = (uint16_t)g_usb_MgrDevAddr;
    switch( cnt_value )
    {
        case 0:
            g_usb_MgrRequest.Address        = (uint16_t)USB_0;
        /* continue */
        case 1:
        /* continue */
        case 2:
        /* continue */
        case 3:
        /* continue */
        case 6:
            g_usb_MgrRequest.wValue     = USB_DEV_DESCRIPTOR;
            g_usb_MgrRequest.wIndex     = 0x0000;
            g_usb_MgrRequest.wLength    = USB_DEVICESIZE;
            data_table                  = (uint8_t*)&g_usb_MgrDevDesc;
        break;
        case 4:
            g_usb_MgrRequest.wValue     = USB_CONF_DESCRIPTOR;
            g_usb_MgrRequest.wIndex     = 0x0000;
            g_usb_MgrRequest.wLength    = 0x0009;
            data_table                  = (uint8_t*)&g_usb_MgrConfDescr;
        break;
        case 5:
            g_usb_MgrRequest.wValue     = USB_CONF_DESCRIPTOR;
            g_usb_MgrRequest.wIndex     = 0x0000;
            g_usb_MgrRequest.wLength    = (uint16_t)(((uint16_t)g_usb_MgrConfDescr[USB_CON_TOTALLENG_HI] << 8)
                                        + (uint16_t)g_usb_MgrConfDescr[USB_CON_TOTALLENG_LO]);
            if( g_usb_MgrRequest.wLength > USB_CONFIGSIZE )
            {
                g_usb_MgrRequest.wLength = USB_CONFIGSIZE;
            }
            data_table                  = (uint8_t*)&g_usb_MgrConfDescr;
        break;
        default:
            return USB_E_ERROR;
        break;
    }

    return usb_hstd_do_command(data_table);
}   /* eof usb_hstd_get_descriptor() */

/******************************************************************************
Function Name   : usb_hstd_set_asddress
Description     : Set SetAddress
Argument        : none
Return          : usb_er_t
******************************************************************************/
USB_STATIC usb_er_t usb_hstd_set_asddress(void)
{
    g_usb_MgrRequest.WORD.BYTE.bmRequestType    = USB_REQUEST_TYPE(USB_HOST_TO_DEV,USB_STANDARD,USB_DEVICE);
    g_usb_MgrRequest.WORD.BYTE.bRequest         = USB_SET_ADDRESS;
    g_usb_MgrRequest.wValue                     = (uint16_t)g_usb_MgrDevAddr;
    g_usb_MgrRequest.wIndex                     = 0x0000;
    g_usb_MgrRequest.wLength                    = 0x0000;
    g_usb_MgrRequest.Address                    = USB_0;
    return usb_hstd_do_command((uint8_t*)&g_usb_MgrDummyData);
}   /* eof usb_hstd_set_asddress() */

/******************************************************************************
Function Name   : usb_hstd_set_configuration
Description     : Set SetConfiguration
Argument        : none
Return          : usb_er_t
******************************************************************************/
USB_STATIC usb_er_t usb_hstd_set_configuration(void)
{
    g_usb_MgrRequest.WORD.BYTE.bmRequestType    = USB_REQUEST_TYPE(USB_HOST_TO_DEV,USB_STANDARD,USB_DEVICE);
    g_usb_MgrRequest.WORD.BYTE.bRequest         = USB_SET_CONFIGURATION;
    g_usb_MgrRequest.wValue                     = (uint16_t)(g_usb_MgrConfDescr[USB_CON_CONFIG_VAL]);
    g_usb_MgrRequest.wIndex                     = 0x0000;
    g_usb_MgrRequest.wLength                    = 0x0000;
    g_usb_MgrRequest.Address                    = (uint16_t)g_usb_MgrDevAddr;
    return usb_hstd_do_command((uint8_t*)&g_usb_MgrDummyData);
}   /* eof usb_hstd_set_configuration() */

/******************************************************************************
Function Name   : usb_hstd_dummy_request
Description     : Set Dummy Request
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_dummy_request(void)
{
}   /* eof usb_hstd_dummy_request() */


/******************************************************************************
Function Name   : usb_hstd_clear_device_infomation
Description     : Device information clear
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_clear_device_infomation(void)
{
    g_usb_HcdDevInfo[g_usb_MgrDevAddr].port     = USB_NOPORT;
    g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = USB_STS_DETACH;
    g_usb_HcdDevInfo[g_usb_MgrDevAddr].config   = USB_0;
    g_usb_HcdDevInfo[g_usb_MgrDevAddr].speed    = USB_NOCONNECT;
}   /* eof usb_hstd_clear_device_infomation() */


/******************************************************************************
Function Name   : usb_hstd_update_devinfo
Description     : 
Argument        : state
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_update_devinfo(uint8_t state)
{
    usb_hstd_update_devstate(state);
    g_usb_HcdDevInfo[g_usb_MgrDevAddr].state    = state;
}   /* eof usb_hstd_update_devinfo() */


/******************************************************************************
Function Name   : usb_hstd_update_devstate
Description     : 
Argument        : state
Return          : none
******************************************************************************/
/* The state of the device of the registered driver is updated.  */
USB_STATIC void usb_hstd_update_devstate(uint8_t state)
{
    usb_hcdreg_t    *driver;
    uint8_t         md;

    for( md = 0; md < g_usb_HcdDevNum; md++ )
    {
        driver  = &g_usb_HcdDriver[md];
        if (g_usb_MgrDevAddr == driver->devaddr)
        {
            driver->devstate    = state;
            break;
        }
    }
}   /* eof usb_hstd_update_devstate() */


/******************************************************************************
Function Name   : usb_hstd_control_write_start
Description     : Control write start
Argument        : none
Return          : uint16_t                  : USB_WRITEEND / USB_WRITING
                :                           : USB_WRITESHRT / USB_FIFOERROR
******************************************************************************/
uint16_t usb_hstd_control_write_start(void)
{
    uint16_t    end_flag;

    /* PID=NAK & clear STALL */
    g_usb_CurrentPipe = USB_PIPE0;
    usb_cstd_clr_stall();

    /* DCP Configuration Register  (0x5C) */
    USB_WR(DCPCFG, (USB_CNTMDFIELD | USB_DIRFIELD));
    USB_WR(DCPCTR, USB_SQSET);

    USB_CLR_STS(BEMPSTS, USB_BIT0);

    /* Host Control sequence */
    end_flag = usb_cstd_buf_2_cfifo();

    switch( end_flag )
    {
        case USB_WRITESHRT:             /* End of data write */
            /* Next stage is Control write status stage */
            g_usb_HcdCtsq = USB_STATUSWR;
            usb_cstd_bemp_enable();
            usb_cstd_nrdy_enable();
            R_usb_cstd_SetBufPipe0();
        break;
        case USB_WRITEEND:              /* End of data write (not null) */
            /* continue */
        case USB_WRITING:               /* Continuance of data write */
            /* Next stage is Control write data stage */
            g_usb_HcdCtsq = USB_DATAWR;
            usb_cstd_bemp_enable();
            usb_cstd_nrdy_enable();
            R_usb_cstd_SetBufPipe0();
        break;
        /* FIFO access error */
        case USB_FIFOERROR:
        /* continue */
        default:
            /* no processing */
        break;
    }
    return (end_flag);
}   /* eof usb_hstd_control_write_start() */

/******************************************************************************
Function Name   : usb_hstd_control_read_start
Description     : Control read start
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_control_read_start(void)
{
    /* PID=NAK & clear STALL */
    g_usb_CurrentPipe = USB_PIPE0;
    usb_cstd_clr_stall();
    USB_WR(DCPCFG, USB_SHTNAKFIELD);
    USB_WR(DCPCTR, USB_SQSET);

    /* Next stage is Control read data stage */
    g_usb_HcdCtsq = USB_DATARD;

    /* Interrupt enable */
    usb_cstd_brdy_enable();
    usb_cstd_nrdy_enable();
    R_usb_cstd_SetBufPipe0();
}   /* eof usb_hstd_control_read_start() */


/******************************************************************************
Function Name   : usb_hstd_status_start
Description     : Control Read/Write Status
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_status_start(void)
{
    uint16_t    end_flag;

    /* Interrupt Disable */
    g_usb_CurrentPipe = USB_PIPE0;
    usb_cstd_bemp_disable();
    usb_cstd_brdy_disable();
    g_usb_LibPipe[USB_PIPE0]->tranlen = g_usb_LibDataCnt[USB_PIPE0];
    g_usb_LibDataCnt[USB_PIPE0] = 0;
    g_usb_LibDataPtr[USB_PIPE0] = (uint8_t*)&g_usb_MgrDummyData;

    /* Branch by the Control transfer stage management */
    switch( g_usb_HcdCtsq )
    {
        case USB_DATARD:                    /* Control Read Data */
            /* Control read status, Control write start */
            g_usb_HcdCtsq = USB_DATARD;
            end_flag = usb_hstd_control_write_start();

            if( end_flag == USB_FIFOERROR )
            {
                /* Control Read/Write End */
                usb_hstd_control_end(USB_DATA_ERR);
            }
            else
            {
                /* Next stage is Control read status stage */
                g_usb_HcdCtsq = USB_STATUSRD;
            }
        break;
        case USB_STATUSWR:                  /* Control Write Data */
            /* continue */
        case USB_SETUPNDC:                  /* NoData Control */
            /* Next stage is Control write status stage */
            usb_hstd_control_read_start();
            g_usb_HcdCtsq = USB_STATUSWR;
        break;
        default:
            return;
        break;
    }

}   /* eof usb_hstd_status_start() */


/******************************************************************************
Function Name   : usb_hstd_control_end
Description     : Control Read/Write End
Argument        : usb_strct_t status           : Transfer status
Return          : none
******************************************************************************/
void usb_hstd_control_end(usb_strct_t status)
{
    usb_utr_t       *ptr;

    /* Evacuation pointer */
    ptr     = g_usb_LibPipe[USB_PIPE0];
    g_usb_LibPipe[USB_PIPE0]    = (usb_utr_t*)USB_NULL;

    /* Interrupt Disable */
    g_usb_CurrentPipe = USB_PIPE0;
    usb_cstd_bemp_disable();
    usb_cstd_brdy_disable();
    usb_cstd_nrdy_disable();

    /* PID=NAK & clear STALL */
    usb_cstd_clr_stall();
    USB_WR(DCPCTR, (uint16_t)(USB_SUREQCLR|USB_SQCLR));

    /* CFIFO buffer clear */
    usb_cstd_set_cfifo_pipe0r();
    USB_WR(CFIFOCTR, USB_BCLR);
    usb_cstd_set_cfifo_pipe0w();
    USB_WR(CFIFOCTR, USB_BCLR);

    /* Next stage is idle */
    g_usb_HcdCtsq = USB_IDLEST;

    /* Call Back */
    if ((ptr != USB_NULL) && (status != USB_DATA_TMO))
    {
        /* Transfer information set */
        ptr->status     = (usb_strct_t)status;
        ptr->pipectr    = usb_cstd_reg_read_pipexctr();
#ifdef  __CA78K0R__
        ((void (*)(usb_utr_t*))*ptr->complete)(ptr);    /* for CA78K0R compiler */
#else
        (*ptr->complete)(ptr);
#endif
    }

}   /* eof usb_hstd_control_end() */


/******************************************************************************
Function Name   : usb_hstd_setup_start
Description     : Setup start
Argument        : none
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_setup_start(void)
{
    uint16_t        dir, setup_req, setup_val, setup_indx, setup_leng;
    uint16_t        *p_setup;

    g_usb_CurrentPipe = USB_PIPE0;
    p_setup     = g_usb_LibPipe[USB_PIPE0]->setup;

    /* Set Setup Request data */
    setup_req   = *p_setup++;
    setup_val   = *p_setup++;
    setup_indx  = *p_setup++;
    setup_leng  = *p_setup++;

    /* Max Packet Size + Device Number select */
    USB_WR(DCPMAXP, g_usb_HcdDcpRegister[*p_setup]);

    /* Transfer Length check */

    if( g_usb_LibPipe[USB_PIPE0]->tranlen < setup_leng )
    {
        setup_leng = (uint16_t)g_usb_LibPipe[USB_PIPE0]->tranlen;
    }

    if( setup_leng < g_usb_LibPipe[USB_PIPE0]->tranlen )
    {
        g_usb_LibPipe[USB_PIPE0]->tranlen = (usb_leng_t)setup_leng;
    }

    g_usb_LibDataCnt[USB_PIPE0] = (usb_leng_t)g_usb_LibPipe[USB_PIPE0]->tranlen;
    g_usb_LibDataPtr[USB_PIPE0] = (uint8_t*)g_usb_LibPipe[USB_PIPE0]->tranadr;


    /* Control sequence setting */
    dir = (uint16_t)(setup_req & USB_BMREQUESTTYPEDIR);
    /* Check wLength field */
    if( setup_leng == 0 )
    {
        /* Check direction field */
        if( dir == 0 )
        {
            /* Next stage is NoData control status stage */
            g_usb_HcdCtsq = USB_SETUPNDC;
        }
        else
        {
            /* Error */
            g_usb_HcdCtsq = USB_IDLEST;
        }
    }
    else
    {
        /* Check direction field */
        if( dir == 0 )
        {
                /* Next stage is Control Write data stage */
                g_usb_HcdCtsq = USB_SETUPWR;
        }
        else
        {
                /* Next stage is Control read data stage */
                g_usb_HcdCtsq = USB_SETUPRD;
        }
    }

    /* Control transfer idle stage ? */
    if( g_usb_HcdCtsq == USB_IDLEST )
    {
        /* Control Read/Write End */
        usb_hstd_control_end(USB_DATA_ERR);
    }
    else
    {
        /* SETUP request set */
        USB_WR(USBREQ, setup_req);
        USB_WRW(USBVAL, setup_val);
        USB_WRW(USBINDX, setup_indx);
        USB_WRW(USBLENG, setup_leng);
        /* Interrupt enable */
        usb_hstd_setup_enable();
    }

}   /* eof usb_hstd_setup_start() */


/******************************************************************************
Function Name   : usb_hstd_brdy_pipe
Description     : INTR interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_brdy_pipe(void)
{
#ifdef  __RX
    uint16_t    bitsts;
#else   /* __RX */
    uint8_t bitsts;
#endif  /* __RX */

    bitsts  = g_usb_HcdMessage->status;         /* p->bitsts; */

    /* When operating by the host function, usb_cstd_brdy_pipe() is executed without fail because */
    /* only one BRDY message is issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
    if( (bitsts & USB_BRDY0) == USB_BRDY0 )
    {
        /* Branch  by the Control transfer stage management */
        switch( g_usb_HcdCtsq )
        {
            case USB_DATARD:                    /* Data stage of Control read transfer */
                g_usb_CurrentPipe = USB_PIPE0;
                switch( usb_cstd_cfifo_2_buf() )
                {
                    case USB_READEND:           /* End of data read */
                    /* continue */
                    case USB_READSHRT:          /* End of data read */
                        usb_hstd_status_start();
                    break;
                    case USB_READING:           /* Continuance of data read */
                    break;
                    case USB_READOVER:          /* bufer over */
                        /* Control Read/Write End */
                        usb_hstd_control_end(USB_DATA_OVR);
                    break;
                    case USB_FIFOERROR:         /* FIFO access error */
                        /* Control Read/Write End */
                        usb_hstd_control_end(USB_DATA_ERR);
                    break;
                    default:
                        /* no processing */
                    break;
                }
            break;
            case USB_STATUSWR:                  /* Status stage of Control write (NoData control) transfer */
                /* Control Read/Write End */
                usb_hstd_control_end(USB_CTRL_END);
            break;
            default:
                /* no processing */
            break;
        }
    }
    usb_cstd_brdy_pipe(bitsts);

}   /* eof usb_hstd_brdy_pipe() */


/******************************************************************************
Function Name   : usb_hstd_nrdy_pipe
Description     : INTN interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_nrdy_pipe(void)
{
    uint16_t        buffer;
#ifdef  __RX
    uint16_t    bitsts;
#else   /* __RX */
    uint8_t bitsts;
#endif  /* __RX */

    bitsts  = g_usb_HcdMessage->status;         /* p->bitsts; */

    /* When operating by the host function, usb_cstd_nrdy_pipe() is executed without fail because */
    /* only one NRDY message is issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
    if( (bitsts & USB_NRDY0) == USB_NRDY0 )
    {
        /* Get Pipe PID from pipe number */
        g_usb_CurrentPipe = USB_PIPE0;
        buffer = usb_cstd_get_pid();
        /* STALL ? */
        if( (buffer & USB_PID_STALL) == USB_PID_STALL )
        {
            /* PIPE0 STALL callback */
            usb_hstd_control_end(USB_DATA_STALL);
        }
        else
        {
            /* PIPE0 detach callback */
            usb_hstd_control_end(USB_DATA_DTCH);
        }
    }
    usb_cstd_nrdy_pipe(bitsts);

}   /* eof usb_hstd_nrdy_pipe() */


/******************************************************************************
Function Name   : usb_hstd_bemp_pipe
Description     : BEMP interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_bemp_pipe(void)
{
    uint16_t        buffer;
#ifdef  __RX
    uint16_t    bitsts;
#else   /* __RX */
    uint8_t bitsts;
#endif  /* __RX */

    bitsts  = g_usb_HcdMessage->status;         /* p->bitsts; */

    /* When operating by the host function, usb_cstd_bemp_pipe() is executed without fail because */
    /* only one BEMP message is issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
    if( (bitsts & USB_BEMP0) == USB_BEMP0 )
    {
        /* Get Pipe PID from pipe number */
        g_usb_CurrentPipe = USB_PIPE0;
        buffer = usb_cstd_get_pid();
        /* MAX packet size error ? */
        if( (buffer & USB_PID_STALL) == USB_PID_STALL )
        {
            usb_hstd_control_end(USB_DATA_STALL);
        } 
        else
        {
            /* Branch by the Control transfer stage management */
            switch( g_usb_HcdCtsq )
            {
                case USB_DATAWR:                    /* Continuance of data stage (Control write) */
                    /* Buffer to CFIFO data write */
                    switch( usb_cstd_buf_2_cfifo() ) 
                    {
                        case USB_WRITESHRT:         /* End of data write */
                            /* Next stage is Control write status stage */
                            g_usb_HcdCtsq = USB_STATUSWR;
                            usb_cstd_bemp_enable();
                            usb_cstd_nrdy_enable();
                        break;
                        case USB_WRITEEND:          /* End of data write (not null) */
                            /* continue */
                        case USB_WRITING:           /* Continue of data write */
                            usb_cstd_bemp_enable();
                            usb_cstd_nrdy_enable();
                        break;
                        case USB_FIFOERROR:         /* FIFO access error */
                            /* Control Read/Write End */
                            usb_hstd_control_end(USB_DATA_ERR);
                        break;
                        default:
                            /* no processing */
                        break;
                    }
                break;
                case USB_STATUSWR:                  /* End of data stage (Control write) */
                    usb_hstd_status_start();
                break;
                case USB_STATUSRD:                  /* Status stage of Control read transfer */
                    /* Control Read/Write End */
                    usb_hstd_control_end(USB_CTRL_END);
                break;
                default:
                    /* no processing */
                break;
            }
        }
    }
    usb_cstd_bemp_pipe(bitsts);

}   /* eof usb_hstd_bemp_pipe() */



/******************************************************************************
Function Name   : usb_hstd_devaddr_2_speed
Description     : Get device speed from device address
Arguments       : none
Return value    : uint8_t                  : device speed
Note            : Use also to a connection check is possible
******************************************************************************/
uint8_t usb_hstd_devaddr_2_speed(void)
{
    uint16_t    buffer;

    /* Get device address configuration register from device address */
    buffer = usb_hstd_get_devaddx_register();
    if (buffer != USB_ERROR)
    {
        buffer = (uint16_t)(buffer & USB_USBSPD);
        if (buffer == USB_FULLSPEED)
        {
            return  (USB_FSCONNECT);
        }
        else if (buffer == USB_LOWSPEED)
        {
            return  (USB_LSCONNECT);
        }
    }
    return USB_NOCONNECT;
}   /* eof usb_hstd_devaddr_2_speed() */


/******************************************************************************
Function Name   : usb_hstd_check_device_address
Description     : Check device connected from device address & root port
Arguments       : none
Return value    : uint8_t                  : YES/NO
******************************************************************************/
uint8_t usb_hstd_check_device_address(void)
{
    uint16_t    buffer;

    /* Get device address configuration register from device address */
    buffer = usb_hstd_get_devaddx_register();
    if ((buffer != USB_ERROR ) && ((usb_port_t)(buffer & USB_RTPORT) == g_usb_HcdPort))
    {
        buffer = (uint16_t)(buffer & USB_USBSPD);
        if (buffer == USB_FULLSPEED)
        {
            return  (USB_FSCONNECT);
        }
        else if (buffer == USB_LOWSPEED)
        {
            return  (USB_LSCONNECT);
        }
    }
    return USB_NOCONNECT;
}   /* eof usb_hstd_check_device_address() */


/******************************************************************************
Function Name   : usb_hstd_get_devaddx_register
Description     : Get device address configuration register from device address
Arguments       : none
Return value    : uint16_t                  : configuration register
******************************************************************************/
USB_STATIC uint16_t usb_hstd_get_devaddx_register(void)
{
    uint16_t    buffer;

    /* Branch Device Address */
    switch( g_usb_HcdDevAddr )
    {
        case USB_DEVICE_0:
            USB_RD(DEVADD0, buffer);
        break;
        case USB_DEVICE_1:
            USB_RD(DEVADD1, buffer);
        break;
        case USB_DEVICE_2:
            USB_RD(DEVADD2, buffer);
        break;
        case USB_DEVICE_3:
            USB_RD(DEVADD3, buffer);
        break;
        case USB_DEVICE_4:
            USB_RD(DEVADD4, buffer);
        break;
        case USB_DEVICE_5:
            USB_RD(DEVADD5, buffer);
        break;
        default:
            buffer = USB_ERROR;
        break;
    }

    return buffer;
}   /* eof usb_hstd_get_devaddx_register() */


/******************************************************************************
Function Name   : usb_hstd_set_devaddx_register
Description     : Set device speed
Arguments       : uint8_t    usbspd     : device speed
                : usb_port_t port       : message keyword
Return value    : none
******************************************************************************/
void usb_hstd_set_devaddx_register(uint8_t usbspd, usb_port_t port)
{
    uint16_t    data;

    if (usbspd == USB_FSCONNECT)
    {
        data    = (uint16_t)(USB_FULLSPEED|port);
    }
    else if (usbspd == USB_LSCONNECT)
    {
        data    = (uint16_t)(USB_LOWSPEED|port);
    }
    else                                                /* else if (usbspd == USB_NOCONNECT) */
    {
        data    = (uint16_t)(USB_NOTUSED|USB_PORT0);    /* RTPT is set to port0 when unused. */
    }

    /* Branch Device Address */
    switch( g_usb_HcdDevAddr )
    {
        case USB_DEVICE_0:
            USB_MDF_PAT(DEVADD0, data, (uint16_t)(USB_USBSPD | USB_RTPORT));
        break;
        case USB_DEVICE_1:
            USB_MDF_PAT(DEVADD1, data, (uint16_t)(USB_USBSPD | USB_RTPORT));
        break;
        case USB_DEVICE_2:
            USB_MDF_PAT(DEVADD2, data, (uint16_t)(USB_USBSPD | USB_RTPORT));
        break;
        case USB_DEVICE_3:
            USB_MDF_PAT(DEVADD3, data, (uint16_t)(USB_USBSPD | USB_RTPORT));
        break;
        case USB_DEVICE_4:
            USB_MDF_PAT(DEVADD4, data, (uint16_t)(USB_USBSPD | USB_RTPORT));
        break;
        case USB_DEVICE_5:
            USB_MDF_PAT(DEVADD5, data, (uint16_t)(USB_USBSPD | USB_RTPORT));
        break;
        default:
            /* no processing */
        break;
    }

}   /* eof usb_hstd_set_devaddx_register() */


/******************************************************************************
Function Name   : usb_hstd_check_attach
Description     : Check Atacch
Arguments       : none
Return value    : uint8_t                  : connection status(ATTACH/DETACH)
Note            : Please change for your SYSTEM
******************************************************************************/
uint8_t usb_hstd_check_attach(void)
{
    uint16_t    buf[3];

    usb_hstd_read_lnst((uint16_t*)&buf);

    if( (uint16_t)(buf[1] & USB_RHST) == USB_UNDECID )      /* RHST is at non-connection */
    {
        if( (buf[0] & USB_LNST) == USB_FS_JSTS )            /* High/Full speed device */
        {
            return USB_LNST_ATTACH;
        }
        else if( (buf[0] & USB_LNST) == USB_LS_JSTS )       /* Low speed device */
        {
            return USB_LNST_ATTACH;
        }
        else if( (buf[0] & USB_LNST) == USB_SE0 )
        {
        }
        else 
        {
        }
        return USB_LNST_DETACH;
    }
    return USB_RHST_ATTACH;
}   /* eof usb_hstd_check_attach() */


/******************************************************************************
Function Name   : usb_hstd_set_feature
Description     : Set SetFeature
Arguments       : uint16_t epnum           : endpoint number
Return          : usb_er_t
******************************************************************************/
USB_STATIC usb_er_t usb_hstd_set_feature(uint16_t epnum)
{
    if( epnum == 0xFF )
    {
        /* SetFeature(Device) */
        g_usb_MgrRequest.WORD.BYTE.bmRequestType    = USB_REQUEST_TYPE(USB_HOST_TO_DEV,USB_STANDARD,USB_DEVICE);
        g_usb_MgrRequest.WORD.BYTE.bRequest         = USB_SET_FEATURE;
        g_usb_MgrRequest.wValue                     = USB_DEVICE_REMOTE_WAKEUP;
        g_usb_MgrRequest.wIndex                     = (uint16_t)0x0000;
    }
    else
    {
        /* SetFeature(endpoint) */
        g_usb_MgrRequest.WORD.BYTE.bmRequestType    = USB_REQUEST_TYPE(USB_HOST_TO_DEV,USB_STANDARD,USB_ENDPOINT);
        g_usb_MgrRequest.WORD.BYTE.bRequest         = USB_SET_FEATURE;
        g_usb_MgrRequest.wValue                     = USB_ENDPOINT_HALT;
        g_usb_MgrRequest.wIndex                     = epnum;
    }
    g_usb_MgrRequest.wLength                        = (uint16_t)0x0000;
    g_usb_MgrRequest.Address                        = (uint16_t)g_usb_MgrDevAddr;
    return usb_hstd_do_command((uint8_t*)&g_usb_MgrDummyData);
}   /* eof usb_hstd_set_feature() */


/******************************************************************************
Function Name   : usb_hstd_clear_feature
Description     : Set ClearFeature
Arguments       : uint16_t epnum           : endpoint number
Return          : usb_er_t
******************************************************************************/
USB_STATIC usb_er_t usb_hstd_clear_feature(uint16_t epnum)
{
    if( epnum == 0xFF )
    {
        /* ClearFeature(Device) */
        g_usb_MgrRequest.WORD.BYTE.bmRequestType    = USB_REQUEST_TYPE(USB_HOST_TO_DEV,USB_STANDARD,USB_DEVICE);
        g_usb_MgrRequest.WORD.BYTE.bRequest         = USB_CLEAR_FEATURE;
        g_usb_MgrRequest.wValue                     = USB_DEVICE_REMOTE_WAKEUP;
        g_usb_MgrRequest.wIndex                     = (uint16_t)0x0000;
    }
    else
    {
        /* ClearFeature(endpoint) */
        g_usb_MgrRequest.WORD.BYTE.bmRequestType    = USB_REQUEST_TYPE(USB_HOST_TO_DEV,USB_STANDARD,USB_ENDPOINT);
        g_usb_MgrRequest.WORD.BYTE.bRequest         = USB_CLEAR_FEATURE;
        g_usb_MgrRequest.wValue                     = USB_ENDPOINT_HALT;
        g_usb_MgrRequest.wIndex                     = epnum;
    }
    g_usb_MgrRequest.wLength                        = (uint16_t)0x0000;
    g_usb_MgrRequest.Address                        = (uint16_t)g_usb_MgrDevAddr;
    return usb_hstd_do_command((uint8_t*)&g_usb_MgrDummyData);
}   /* eof usb_hstd_clear_feature() */


/******************************************************************************
Function Name   : usb_hstd_do_command
Description     : command submit
Arguments       : uint8_t  *data_table      : Transfer data
Return          : usb_er_t
******************************************************************************/
USB_STATIC usb_er_t usb_hstd_do_command(uint8_t *data_table)
{
    g_usb_MgrControlMessage.tranadr     = (void*)data_table;
    g_usb_MgrControlMessage.complete    = (usb_cb_t)&usb_hstd_transfer_result;
    g_usb_MgrControlMessage.tranlen     = (usb_leng_t)g_usb_MgrRequest.wLength;
    g_usb_MgrControlMessage.pipenum     = USB_PIPE0;
    g_usb_MgrControlMessage.setup       = (void*)&g_usb_MgrRequest;
    return R_usb_hstd_TransferStart(&g_usb_MgrControlMessage);
}   /* eof usb_hstd_do_command() */


/******************************************************************************
Function Name   : usb_hstd_pipe_2_epadr
Description     : pipe number to endpoint address
Arguments       : none
Return value    : uint8_t               : Endpoint Number + Direction
******************************************************************************/
USB_STATIC uint8_t usb_hstd_pipe_2_epadr(void)
{

    uint16_t    buffer;
    uint8_t     direp=0;

    if( g_usb_CurrentPipe == USB_PIPE0 )
    {
        USB_RD(DCPCFG, buffer);
    }
    else
    {
        /* Pipe select */
        USB_WR(PIPESEL, g_usb_CurrentPipe);
        USB_RD(PIPECFG, buffer);
        direp = (uint8_t)(buffer & USB_EPNUM);
    }

    direp   |= (uint8_t)( ((buffer & USB_DIR)^USB_DIR) << 3);

    return direp;

}   /* eof usb_hstd_pipe_2_epadr() */


/******************************************************************************
Function Name   : usb_hstd_pipe_2_hcddevaddr
Description     : Get device address from pipe number
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_pipe_2_hcddevaddr(void)
{
    uint16_t    buffer;

    if( g_usb_CurrentPipe == USB_PIPE0 )
    {
        USB_RD(DCPMAXP, buffer);
    }
    else
    {
        /* Pipe select */
        USB_WR(PIPESEL, g_usb_CurrentPipe);
        USB_RD(PIPEMAXP, buffer);
    }

    /* Device address */
    g_usb_HcdDevAddr    = USB_DEVSEL2ADDR(buffer & USB_DEVSEL);
}   /* eof usb_hstd_pipe_2_hcddevaddr() */


/******************************************************************************
Function Name   : usb_hstd_pipe_2_addr
Description     : Get device address from pipe number
Arguments       : none
Return value    : usb_addr_t                    : device address
******************************************************************************/
usb_addr_t usb_hstd_pipe_2_addr(void)
{
    uint16_t    buffer;

    if( g_usb_CurrentPipe == USB_PIPE0 )
    {
        USB_RD(DCPMAXP, buffer);
    }
    else
    {
        /* Pipe select */
        USB_WR(PIPESEL, g_usb_CurrentPipe);
        USB_RD(PIPEMAXP, buffer);
    }

    /* Device address */
    return  USB_DEVSEL2ADDR(buffer & USB_DEVSEL);
}   /* eof usb_hstd_pipe_2_addr() */


/******************************************************************************
Function Name   : usb_hstd_status_notification
Description     : Notification status
Argument        : uint16_t msginfo       : message info
                : uint8_t keyword        : message keyword
Return          : none
******************************************************************************/
void usb_hstd_status_notification(usb_strct_t msginfo, usb_strct_t keyword)
{
    if (R_USB_GET_SND(USB_MGR_MBX, msginfo, &usb_cstd_dummy_function, keyword) != USB_E_OK)
    {
        USB_DEBUG_HOOK(USB_DEBUG_HOOK_HOST | USB_DEBUG_HOOK_CODE6);
    }
}   /* eof usb_hstd_status_notification() */


/******************************************************************************
Function Name   : usb_hstd_status_change
Description     : Notification status change
Argument        : uint16_t msginfo       : message info
                : uint8_t keyword        : message keyword
Return          : usb_er_t
******************************************************************************/
USB_STATIC usb_er_t usb_hstd_status_change(usb_strct_t msginfo, usb_strct_t keyword)
{
    g_usb_MgrMessage->msginfo   = msginfo;
    g_usb_MgrMessage->keyword   = keyword;
    return  R_USB_SND_MSG(USB_HCD_MBX, (usb_msg_t*)g_usb_MgrMessage);
}   /* eof usb_hstd_status_change() */


/******************************************************************************
Function Name   : usb_hstd_status_result
Description     : Notification status result
Argument        : usb_strct_t status
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_status_result(usb_strct_t status)
{
    g_usb_MgrSequence++;
    g_usb_HcdMessage->msginfo   = USB_MGR_CONTINUE;
    g_usb_HcdMessage->keyword   = g_usb_HcdPort;
    g_usb_HcdMessage->status    = status;
    R_USB_SND_MSG(USB_MGR_MBX, (usb_msg_t*)g_usb_HcdMessage);
}   /* eof usb_hstd_status_result() */


/******************************************************************************
Function Name   : usb_hstd_transfer_result
Description     : Notification transfer result
Argument        : usb_utr_t *utrmsg      : message
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_transfer_result(usb_utr_t *utrmsg)
{
    g_usb_MgrSequence++;
    utrmsg->msginfo = USB_MGR_CONTINUE;
    R_USB_SND_MSG(USB_MGR_MBX, (usb_msg_t*)utrmsg);
}   /* eof usb_hstd_transfer_result() */


/******************************************************************************
Function Name   : usb_hstd_transferend_check
Description     : Transfer End Check
Argument        : usb_strct_t status
Return          : none
******************************************************************************/
USB_STATIC void usb_hstd_transferend_check(usb_strct_t status)
{
    if( g_usb_CurrentPipe == USB_PIPE0 )
    {
        /* Control Read/Write End */
        usb_hstd_control_end(status);
    }
    else
    {
        /* Transfer stop */
        usb_cstd_forced_termination(status);
    }
    R_USB_REL_BLK(g_usb_HcdMessage->flag);
}   /* eof usb_hstd_transferend_check() */


/******************************************************************************
Function Name   : usb_hstd_driver_init
Description     : dreiver initialized
Argument        : none
Return          : none
******************************************************************************/
void    usb_hstd_driver_init(void)
{
    g_usb_HcdDriver[g_usb_HcdDevNum].rootport   = USB_NOPORT;
    g_usb_HcdDriver[g_usb_HcdDevNum].devaddr    = USB_NODEVICE;
    g_usb_HcdDriver[g_usb_HcdDevNum].devstate   = USB_STS_DETACH;
    g_usb_HcdDriver[g_usb_HcdDevNum].ifclass    = USB_IFCLS_NOT;
//  When the local variable is used, 
//  the code conversion to which the compiler is mistaken because of optimization(-qx2) is done.
//  usb_hcdreg_t    *driver;
//  driver = &g_usb_HcdDriver[g_usb_HcdDevNum];
//  driver->rootport    = USB_NOPORT;
//  driver->devaddr     = USB_NODEVICE;
//  driver->devstate    = USB_STS_DETACH;
//  driver->ifclass     = USB_IFCLS_NOT;
}   /* eof usb_hstd_driver_init() */


/******************************************************************************
Function Name   : usb_hstd_setup_clear_status
Description     : Clear setup interrupt status
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_hstd_setup_clear_status(void)
{
    USB_CLR_STS(INTSTS1, (USB_SIGN | USB_SACK));
}   /* eof usb_hstd_setup_clear_status() */

/******************************************************************************
Function Name   : usb_hstd_setup_enable
Description     : Enable setup interrupt
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_hstd_setup_enable(void)
{
    usb_hstd_setup_clear_status();
    USB_SET_PAT(INTENB1, USB_SIGNE | USB_SACKE);
    USB_SET_PAT(DCPCTR, USB_SUREQ);
}   /* eof usb_hstd_setup_enable() */

/******************************************************************************
Function Name   : usb_hstd_setup_disable
Description     : Disable setup interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_setup_disable(void)
{
    usb_hstd_setup_clear_status();
    USB_CLR_PAT(INTENB1, USB_SIGNE | USB_SACKE);
}   /* eof usb_hstd_setup_disable() */


/******************************************************************************
Function Name   : usb_hstd_port_enable
Description     : Check connection
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_port_enable(void)
{
    usb_hstd_vbus_control_on();
#ifndef USB_HOST_BC_ENABLE
    usb_cpu_delay_xms((uint16_t)100);
#endif  /* USB_HOST_BC_ENABLE */

    if( usb_hstd_check_attach() == USB_LNST_ATTACH )
    {
        usb_hstd_attach_control();
    }
    else
    {
        usb_hstd_detach_control();
    }
}   /* eof usb_hstd_port_enable() */


/******************************************************************************
End of file
******************************************************************************/
