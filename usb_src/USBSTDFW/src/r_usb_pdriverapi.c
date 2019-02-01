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
* File Name    : r_usb_pdriverapi.c
* Version      : 2.10
* Description  : This is the USB peripheral control driver API function code.
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
#include "r_usb_api.h"                  /* USB API public function header */


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
/* There is no orivate global variables and functions. */


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* Exported global variables and functions are declared with the header file. */


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* variables */
extern usb_pcdreg_t g_usb_PcdDriver;                /* Driver registration */
extern uint16_t     g_usb_Pcdintsts0;               /* INTSTS0 */
extern uint8_t      g_usb_PcdConfigNum;             /* Configuration number */
extern uint8_t      g_usb_PcdRemoteWakeup;          /* Remote wakeup enable flag */
extern usb_utr_t    *g_usb_LibPipe[];               /* Message pipe */
extern usb_leng_t   g_usb_LibDataCnt[];             /* PIPEn Buffer counter */
extern uint8_t      *g_usb_LibDataPtr[];                /* PIPEn Buffer pointer(8bit) */
extern usb_pipe_t   g_usb_CurrentPipe;              /* Pipe number */

/* functions */
extern void         usb_pstd_pcd_task(void);
extern uint8_t      usb_pstd_get_interface_num(void);
extern uint8_t      usb_pstd_chk_configured(void);
extern void         usb_pstd_port_enable(void);
extern void         usb_pstd_port_disable(void);
extern void         usb_pstd_remote_wakeup(void);
extern void         usb_pstd_init_connect(void);
extern void         usb_pstd_set_ccpl(void);
extern void         usb_cstd_init_library(void);
extern uint8_t      usb_cstd_buf_2_cfifo(void);
extern void         usb_cstd_clr_cfifo_pipe0r(void);
extern void         usb_cstd_clr_cfifo_pipe0w(void);
extern void         usb_cstd_brdy_enable(void);
extern void         usb_cstd_brdy_disable(void);
extern void         usb_cstd_bemp_enable(void);
extern void         usb_cstd_bemp_disable(void);
extern void         usb_cstd_nrdy_disable(void);
extern void         usb_cstd_berne_enable(void);
extern void         usb_cstd_reg_set_pipexctr(uint16_t field);
extern uint16_t     usb_cstd_set_nak(void);
extern void         usb_cstd_sw_reset(void);
extern void         usb_cstd_hw_start(void);
extern void         usb_cstd_set_pipe_registration(uint16_t *table, uint16_t command);
extern void         usb_cstd_dummy_function(uint16_t data1, uint16_t data2);



/******************************************************************************
Renesas Abstracted USB Peripheral Driver API functions
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_pstd_PcdTask
Description     : USB Peripheral control driver Task
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_pstd_PcdTask(void)
{
    usb_pstd_pcd_task();
}   /* eof R_usb_pstd_PcdTask() */


/******************************************************************************
Function Name   : R_usb_pstd_PcdOpen
Description     : Usb Driver Open
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_pstd_PcdOpen(void)
{
//  All global variables are initialized specifying it. 
//  However, an unnecessary variable to initialize ends comment by "//". 
//unnecessary   g_usb_PcdMessage;               /* Pcd Task receive message */
//unnecessary   g_usb_PcdDriver;                /* Driver registration */
//unnecessary   g_usb_PcdRequest;               /* Device Request - Request structure */
//unnecessary   g_usb_PcdCallback;              /* API callback function */
//unnecessary   g_usb_PcdRequestData[];         /* Control transfer data */
    g_usb_Pcdintsts0        = USB_0;            /* INTSTS0 */
    g_usb_PcdConfigNum      = USB_0;            /* Configuration number (Not configuration) */
    g_usb_PcdRemoteWakeup   = USB_NO;           /* Remote wakeup enable flag */
    usb_cstd_init_library();                    /* Initialized global variables */
}   /* eof R_usb_pstd_PcdOpen() */


/******************************************************************************
Function Name   : R_usb_pstd_DriverRegistration
Description     : Callback registration
Arguments       : usb_pcdreg_t *registinfo : class driver structure
Return value    : none
******************************************************************************/
void R_usb_pstd_DriverRegistration(usb_pcdreg_t *registinfo)
{
    /* Driver registration */
    g_usb_PcdDriver.pipetbl         = registinfo->pipetbl;
    g_usb_PcdDriver.devicetbl       = registinfo->devicetbl;
    g_usb_PcdDriver.configtbl       = registinfo->configtbl;
    g_usb_PcdDriver.stringtbl       = registinfo->stringtbl;
    g_usb_PcdDriver.statediagram    = registinfo->statediagram;
    g_usb_PcdDriver.ctrltrans       = registinfo->ctrltrans;
}   /* eof R_usb_pstd_DriverRegistration() */


/******************************************************************************
Function Name   : R_usb_pstd_TransferStart
Description     : Data transfer request
Arguments       : usb_utr_t *utr_table : message
Return value    : usb_er_t Error Info
******************************************************************************/
usb_er_t R_usb_pstd_TransferStart(usb_utr_t *utr_table)
{
    /* Range check of argument */
    if (( utr_table->pipenum > USB_MAX_PIPE_NO ) || ( utr_table->pipenum < USB_MIN_PIPE_NO ))
    {
        return USB_E_ERROR;
    }

    /* Judge of data transfer */
    if( g_usb_LibPipe[utr_table->pipenum] != USB_NULL )
    {
        return USB_E_QOVR;
    }

    /* Check state (device configured) */
    if( usb_pstd_chk_configured() != USB_YES )
    {
        return USB_E_ERROR;
    }

    utr_table->msginfo  = USB_DO_TRANSFERSTART;
    return  R_USB_SND_MSG(USB_PCD_MBX, (usb_msg_t*)utr_table);
}   /* eof R_usb_pstd_TransferStart() */


/******************************************************************************
Function Name   : R_usb_pstd_TransferEnd
Description     : Data transfer end request
Arguments       : usb_pipe_t pipe       : pipe number
                : usb_strct_t msginfo   : transfer end status
Return value    : usb_er_t Error Info
******************************************************************************/
usb_er_t R_usb_pstd_TransferEnd(usb_pipe_t pipe, usb_strct_t msginfo)
{
    /* Range check of argument */
    if (pipe > USB_MAX_PIPE_NO)
    {
        return USB_E_ERROR;
    }

    /* Judge of data transfer */
    if( g_usb_LibPipe[pipe] == USB_NULL )
    {
        return USB_E_QOVR;
    }

    switch (msginfo)
    {
        case USB_DO_TRANSFER_STP:
            return  R_USB_GET_SND(USB_PCD_MBX, USB_DO_TRANSFER_STP, &usb_cstd_dummy_function, (usb_strct_t)pipe);
        break;
        case USB_DO_TRANSFER_TMO:
            return  R_USB_GET_SND(USB_PCD_MBX, USB_DO_TRANSFER_TMO, &usb_cstd_dummy_function, (usb_strct_t)pipe);
        break;
        default:
            /* no processing */
        break;
    }
    return  USB_E_ERROR;
}   /* eof R_usb_pstd_TransferEnd() */


/******************************************************************************
Function Name   : R_usb_pstd_PcdChangeDeviceState
Description     : Change Device State
Arguments       : uint16_t msginfo          : new device status
Return value    : usb_er_t                  : Error code
******************************************************************************/
usb_er_t R_usb_pstd_PcdChangeDeviceState(usb_strct_t msginfo)
{
    switch (msginfo)
    {
        case USB_DO_PORT_ENABLE:
            usb_pstd_port_enable();
        break;
        case USB_DO_PORT_DISABLE:
            usb_pstd_port_disable();
        break;
        case USB_DO_REMOTEWAKEUP:
            usb_pstd_remote_wakeup();
        break;
        case USB_DO_INITHWFUNCTION:
            usb_cstd_hw_start();                                /* Begining the clock supply of USB-IP*/
            usb_cstd_sw_reset();                                /* USB-IP initialized */
        break;
        case USB_DO_SETHWFUNCTION:
            /* USB interrupt message initialize, select HW function */
            usb_cstd_berne_enable();
            usb_pstd_init_connect();
        break;
        default:
            return  USB_E_ERROR;
        break;
    }
    return  USB_E_OK;
}   /* eof R_usb_pstd_PcdChangeDeviceState() */


/******************************************************************************
Function Name   : R_usb_pstd_DeviceInformation
Description     : Get Device Information
Arguments       : uint16_t *table    : Device Information Store Pointer TBL
Return value    : none
******************************************************************************/
void R_usb_pstd_DeviceInformation(uint16_t *table)
{
    /* Device status */
    table[0] = g_usb_Pcdintsts0 & (uint16_t)(USB_VBSTS|USB_DVSQ);

    /* Configuration number */
    table[1] = (uint16_t)g_usb_PcdConfigNum;

    /* Interface number */
    table[2] = (uint16_t)usb_pstd_get_interface_num();

    /* Remote Wakeup Flag */
    table[3] = g_usb_PcdRemoteWakeup;
}   /* eof R_usb_pstd_DeviceInformation() */


/******************************************************************************
Function Name   : R_usb_pstd_SetStallPipe0
Description     : Set pipe0 USB_PID_STALL
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_pstd_SetStallPipe0(void)
{
    g_usb_CurrentPipe = USB_PIPE0;
    usb_cstd_reg_set_pipexctr(USB_PID_STALL);
}   /* eof R_usb_pstd_SetStallPipe0() */


/******************************************************************************
Function Name   : R_usb_pstd_SetPipeStall
Description     : Set pipe USB_PID_STALL
Arguments       : usb_pipe_t pipe           ; Pipe Number
Return value    : usb_er_t                  : Error code
******************************************************************************/
usb_er_t R_usb_pstd_SetPipeStall(usb_pipe_t pipe)
{
    if (( pipe <= USB_MAX_PIPE_NO ) && ( pipe >= USB_MIN_PIPE_NO ))
    {
        g_usb_CurrentPipe = pipe;
        usb_cstd_reg_set_pipexctr(USB_PID_STALL);
        return  USB_E_OK;
    }
    return  USB_E_ERROR;
}   /* eof R_usb_pstd_SetPipeStall() */


/******************************************************************************
Function Name   : R_usb_pstd_ControlRead
Description     : Control read start
Arguments       : usb_leng_t bsize
                : uint8_t *table
Return value    : uint8_t
******************************************************************************/
uint8_t R_usb_pstd_ControlRead(usb_leng_t bsize, uint8_t *table)
{
    uint8_t end_flag;

    g_usb_CurrentPipe = USB_PIPE0;

    g_usb_LibDataCnt[USB_PIPE0] = bsize;
    g_usb_LibDataPtr[USB_PIPE0] = table;

    /* Pipe 0 write buffer clear */
    usb_cstd_clr_cfifo_pipe0w();

    /* Peripheral Control sequence */
    end_flag = usb_cstd_buf_2_cfifo();

    /* Peripheral control sequence */
    switch( end_flag )
    {
//      case USB_WRITEEND:          /* End of data write (not null) */
//      /* continue */
            /* USB_WRITEEND is not generated by PIPE0. */
        case USB_WRITING:           /* Continue of data write */
            usb_cstd_bemp_enable();
            R_usb_cstd_SetBufPipe0();
        break;
        case USB_WRITESHRT:         /* End of data write */
            R_usb_cstd_SetBufPipe0();
        break;
        case USB_FIFOERROR:         /* FIFO access error */
        /* continue */
        default:
            /* no processing */
        break;
    }
    /* End or error or continue */
    return (end_flag);
}   /* eof R_usb_pstd_ControlRead() */


/******************************************************************************
Function Name   : R_usb_pstd_ControlWrite
Description     : Control write start
Arguments       : usb_leng_t bsize
                : uint8_t *table
Return value    : none
******************************************************************************/
void R_usb_pstd_ControlWrite(usb_leng_t bsize, uint8_t *table)
{
    g_usb_CurrentPipe = USB_PIPE0;
    g_usb_LibDataCnt[USB_PIPE0] = bsize;
    g_usb_LibDataPtr[USB_PIPE0] = table;

    /* Pipe 0 read buffer clear */
    usb_cstd_clr_cfifo_pipe0r();

    /* Interrupt enable */
    usb_cstd_brdy_enable();

    /* Set PID=BUF */
    R_usb_cstd_SetBufPipe0();
}   /* eof R_usb_pstd_ControlWrite() */


/******************************************************************************
Function Name   : R_usb_pstd_ControlEnd
Description     : Control transfer stop(end)
Arguments       : uint16_t status
Return value    : none
******************************************************************************/
void R_usb_pstd_ControlEnd(uint16_t status)
{
    /* Interrupt disable */
    g_usb_CurrentPipe = USB_PIPE0;
    usb_cstd_bemp_disable();
    usb_cstd_brdy_disable();
    usb_cstd_nrdy_disable();

    switch( status )
    {
        case USB_CTRL_END:
            usb_pstd_set_ccpl();                    /* Status stage start */
        break;
        case USB_DATA_STOP:
            usb_cstd_set_nak();                     /* Pipe stop */
        break;
        case USB_DATA_ERR:
        /* continue */
        case USB_DATA_OVR:
        /* continue */
        default:
            R_usb_pstd_SetStallPipe0();             /* Request error */
        break;
    }

}   /* eof R_usb_pstd_ControlEnd() */


/******************************************************************************
Function Name   : R_usb_pstd_SetPipeRegister
Description     : Set pipe configuration register
Arguments       : uint16_t *table       : pipe information table
                : uint16_t command      : control command
Return value    : none
******************************************************************************/
void R_usb_pstd_SetPipeRegister(uint16_t *table, uint16_t command)
{
    usb_cstd_set_pipe_registration(table, command);
}   /* eof R_usb_pstd_SetPipeRegister() */


/******************************************************************************
End of file
******************************************************************************/
