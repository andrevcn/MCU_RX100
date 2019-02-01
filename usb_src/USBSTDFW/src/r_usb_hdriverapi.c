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
* File Name    : r_usb_hdriverapi.c
* Version      : 2.10
* Description  : This is the USB host control driver API function code.
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
extern usb_utr_t        *g_usb_LibPipe[];                   /* Message pipe */
extern usb_pipe_t       g_usb_CurrentPipe;                  /* Pipe number */

#ifdef USB_HOST_BC_ENABLE
extern usb_bc_status_t  g_usb_hstd_bc[];
#endif

/* USB Host driver global variables */
extern usb_port_t       g_usb_HcdPort;
extern usb_port_t       g_usb_MgrPort;
extern usb_tskinfo_t    *g_usb_HcdMessage;                  /* Hcd Task receive message */
extern usb_hcdreg_t     g_usb_HcdDriver[];                  /* Device driver (registration) */
extern uint16_t         g_usb_HcdDcpRegister[];             /* DEVSEL & DCPMAXP (Multiple device) */
extern uint8_t          g_usb_HcdRemotePort[];
extern uint8_t          g_usb_HcdCtsq;                      /* Control transfer stage management */
extern usb_addr_t       g_usb_HcdDevAddr;                   /* Device address */
extern usb_addr_t       g_usb_MgrDevAddr;                   /* Device address */
extern uint8_t          g_usb_HcdDevSpeed;                  /* Reset handshake result */
extern uint8_t          g_usb_HcdDevNum;                    /* Device driver number */
extern usb_hcddevinf_t  g_usb_HcdDevInfo[];                 /* device address, status, config, speed, */
extern uint8_t          g_usb_HcdRegPointer;                /* Driver number retrieval that can be operated */
extern usb_tskinfo_t    *g_usb_MgrMessage;                  /* Mgr Task receive message */
extern usb_cbinfo_t     g_usb_MgrCallback;                  /* API callback function */
extern usb_utr_t        g_usb_MgrControlMessage;
extern uint8_t          g_usb_MgrSequence;
extern uint8_t          g_usb_MgrSeqMode;
extern usb_hcdrequest_t g_usb_MgrRequest;
extern uint8_t          g_usb_MgrDevDesc[];
extern uint8_t          g_usb_MgrConfDescr[];
extern uint8_t          g_usb_MgrDummyData[];

/* functions */
extern void             usb_hstd_hcd_task(void);
extern void             usb_hstd_mgr_task(void);
extern uint8_t          usb_hstd_devaddr_2_speed(void);
extern void             usb_hstd_pipe_2_hcddevaddr(void);
extern void             usb_hstd_driver_init(void);                 /* Initialized g_usb_HcdDriver[] */
extern void             usb_hstd_clear_device_infomation(void);     /* Initialized g_usb_HcdDevInfo */
extern void             usb_hstd_set_hw_function(void);
extern void             usb_cstd_init_library(void);
extern void             usb_cstd_sw_reset(void);
extern void             usb_cstd_hw_start(void);
extern void             usb_hstd_vbus_control_on(void);
//extern void             usb_hstd_vbus_control_off(void);
extern void             usb_cstd_set_pipe_registration(uint16_t *table, uint16_t command);
extern void             usb_cstd_dummy_function(uint16_t data1, uint16_t data2);


/******************************************************************************
Renesas Abstracted USB Host Driver API functions
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hstd_HcdTask
Description     : USB Host Control Driver Task
Argument        : none
Return          : none
******************************************************************************/
void R_usb_hstd_HcdTask(void)
{
    usb_hstd_hcd_task();
}   /* eof R_usb_hstd_HcdTask() */


/******************************************************************************
Function Name   : R_usb_hstd_MgrTask
Description     : Manager Task
Argument        : none
Return          : none
******************************************************************************/
void R_usb_hstd_MgrTask(void)
{
    usb_hstd_mgr_task();
}   /* eof R_usb_hstd_MgrTask() */


/******************************************************************************
Function Name   : R_usb_hstd_HcdOpen
Description     : Usb Driver Open
Argument        : none
Return          : none
******************************************************************************/
void R_usb_hstd_HcdOpen(void)
{
//  All global variables are initialized specifying it.
//  However, an unnecessary variable to initialize ends comment by "//".
//unnecessary   g_usb_HcdPort;
//unnecessary   g_usb_MgrPort
//unnecessary   g_usb_HcdMessage;                               /* Hcd Task receive message */
//unnecessary   g_usb_HcdDcpRegister[];                         /* DEVSEL & DCPMAXP (Multiple device) */
//unnecessary   g_usb_HcdDevSpeed;
//unnecessary   g_usb_HcdRegPointer;
//unnecessary   g_usb_MgrMessage;                               /* Mgr Task receive message */
//unnecessary   g_usb_MgrCallback;                              /* API callback function */
//unnecessary   g_usb_MgrControlMessage;
//unnecessary   g_usb_MgrRequest[];
//unnecessary   g_usb_MgrDevDesc[];
//unnecessary   g_usb_MgrConfDescr[];
//unnecessary   g_usb_MgrDummyData[];

    if( USB_MAXDEVADDR < USB_DEVICEADDR )
    {
        /* >yes no process */
        USB_DEBUG_HOOK(USB_DEBUG_HOOK_HOST | USB_DEBUG_HOOK_CODE7);
    }

    /* Initialized Manager Mode */
    g_usb_MgrSequence   = USB_0;
    g_usb_MgrSeqMode    = USB_MGR_NOSEQUENCE;

    /* Initialized driver registration area */
    for( g_usb_HcdDevNum = USB_0; g_usb_HcdDevNum < USB_MAXDEVADDR; g_usb_HcdDevNum++ )
    {
        usb_hstd_driver_init();
    }

    /* Initial device address & driver number */
    g_usb_HcdDevAddr    = (usb_addr_t)USB_0;
    g_usb_HcdDevNum     = USB_0;

    /* Initialized device information area */
    for( g_usb_MgrDevAddr = USB_0; g_usb_MgrDevAddr <= USB_MAXDEVADDR; g_usb_MgrDevAddr++ )
    {
        usb_hstd_clear_device_infomation();
    }

    /* Control transfer stage management */
    g_usb_HcdCtsq               = USB_IDLEST;
    g_usb_HcdRemotePort[USB_0]  = USB_NO;
#if USB_PORTSEL_PP == USB_2PORT_PP
    g_usb_HcdRemotePort[USB_1]  = USB_NO;
#endif  /* USB_PORTSEL_PP */

    usb_cstd_init_library();                    /* Initialized global variables */

#ifdef USB_HOST_BC_ENABLE
    g_usb_hstd_bc[USB_PORT0].state = USB_BC_STATE_INIT;
#if USB_PORTSEL_PP == USB_2PORT_PP
    g_usb_hstd_bc[USB_PORT1].state = USB_BC_STATE_INIT;
#endif  /* USB_PORTSEL_PP */
#endif

}   /* eof R_usb_hstd_HcdOpen() */


/******************************************************************************
Function Name   : R_usb_hstd_DriverRegistration
Description     : Class driver registration
Argument        : usb_hcdreg_t *registinfo    : class driver structure
Return          : none
******************************************************************************/
void R_usb_hstd_DriverRegistration(usb_hcdreg_t *registinfo)
{
    usb_hcdreg_t    *driver;

    /* Driver registration area */
    if( g_usb_HcdDevNum < USB_MAXDEVADDR )
    {
        usb_hstd_driver_init();
        driver = &g_usb_HcdDriver[g_usb_HcdDevNum];
        driver->ifclass     = registinfo->ifclass;
        driver->classcheck  = registinfo->classcheck;
        driver->statediagram= registinfo->statediagram;
        g_usb_HcdDevNum++;
    }
    else
    {
        USB_DEBUG_HOOK(USB_DEBUG_HOOK_HOST | USB_DEBUG_HOOK_CODE8);
    }

}   /* eof R_usb_hstd_DriverRegistration() */

/******************************************************************************
Function Name   : R_usb_hstd_DriverRelease
Description     : Class driver release
Argument        : uint8_t devclass          : interface class
Return          : none
******************************************************************************/
void R_usb_hstd_DriverRelease(uint8_t devclass)
{
    usb_hcdreg_t    *driver;
    uint8_t         i;

    if( devclass == USB_IFCLS_NOT )
    {
        /* Device driver number */
        while (g_usb_HcdDevNum)
        {
            usb_hstd_driver_init();
            g_usb_HcdDevNum--;
        }
    }
    else
    {
        for( i = USB_0; i <= USB_MAXDEVADDR; i++ )
        {
            driver = &g_usb_HcdDriver[i];
            /* Driver release */
            if( driver->ifclass == devclass )
            {
                driver->rootport    = USB_NOPORT;
                driver->devaddr     = USB_NODEVICE;
                driver->devstate    = USB_STS_DETACH;
                driver->ifclass     = USB_IFCLS_NOT;
                g_usb_HcdDevNum--;
                return;
            }
        }
    }

}   /* eof R_usb_hstd_DriverRelease() */


/******************************************************************************
Function Name   : R_usb_hstd_TransferStart
Description     : Request Data transfer
Argument        : usb_utr_t *utr_table      : message
Return          : usb_er_t                  : USB_E_OK/USB_E_QOVR/USB_E_ERROR
******************************************************************************/
usb_er_t R_usb_hstd_TransferStart(usb_utr_t *utr_table)
{

    g_usb_CurrentPipe = (usb_pipe_t)utr_table->pipenum;
    /* Pipe Transfer Process check */
    if( g_usb_LibPipe[g_usb_CurrentPipe] != USB_NULL )
    {
        return USB_E_QOVR;
    }

    /* Get device address from pipe number */
    if( g_usb_CurrentPipe == USB_PIPE0 )
    {
        g_usb_HcdDevAddr    = (usb_addr_t)utr_table->setup[4];
    }
    else
    {
        usb_hstd_pipe_2_hcddevaddr();
        if (g_usb_HcdDevAddr == USB_DEVICE_0)
        {
            return USB_E_ERROR;
        }
    }

    /* Get device speed from device address */
    if( usb_hstd_devaddr_2_speed() == USB_NOCONNECT )
    {
        return USB_E_ERROR;
    }

    utr_table->msginfo  = USB_DO_TRANSFERSTART;
    return  R_USB_SND_MSG(USB_HCD_MBX, (usb_msg_t*)utr_table);

}   /* eof R_usb_hstd_TransferStart() */

/******************************************************************************
Function Name   : R_usb_hstd_TransferEnd
Description     : Data transfer end request
Argument        : usb_pipe_t pipe           : pipe number
                : uint16_t status           : message status
Return          : usb_er_t                  : USB_E_OK etc
******************************************************************************/
usb_er_t R_usb_hstd_TransferEnd(usb_pipe_t pipe, usb_strct_t msginfo)
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
            return  R_USB_GET_SND(USB_HCD_MBX, USB_DO_TRANSFER_STP, &usb_cstd_dummy_function, (usb_strct_t)pipe);
        break;
        case USB_DO_TRANSFER_TMO:
            return  R_USB_GET_SND(USB_HCD_MBX, USB_DO_TRANSFER_TMO, &usb_cstd_dummy_function, (usb_strct_t)pipe);
        break;
        default:
            return  USB_E_ERROR;
        break;
    }
}   /* eof R_usb_hstd_TransferEnd() */


/******************************************************************************
Function Name   : R_usb_hstd_MgrChangeDeviceState
Description     : Change HUB down port device status
Argument        : usb_cbinfo_t complete     : callback function pointer
                : uint16_t msginfo           : Message Info
                : uint16_t keyword           : address, rootport, etc
Return          : usb_er_t                   : USB_E_OK etc
******************************************************************************/
usb_er_t R_usb_hstd_MgrChangeDeviceState(usb_cbinfo_t complete, usb_strct_t msginfo, usb_strct_t keyword)
{
    switch (msginfo)
    {
        case USB_DO_PORT_ENABLE:
        /* continue */
        case USB_DO_PORT_DISABLE:
        /* continue */
        case USB_DO_GLOBAL_SUSPEND:
        /* continue */
        case USB_DO_GLOBAL_RESUME:
            if ((keyword == USB_PORT0) || (keyword == USB_PORT1))
            {
                return  R_USB_GET_SND(USB_MGR_MBX, msginfo, complete, keyword);
            }
        break;
        case USB_DO_CLEAR_STALL:
            if (( keyword <= USB_MAX_PIPE_NO ) && ( keyword >= USB_MIN_PIPE_NO ))
            {
                return  R_USB_GET_SND(USB_MGR_MBX, USB_DO_CLEAR_STALL, complete, keyword);
            }
        break;
        default:
            /* no processing */
        break;
    }
    return  USB_E_ERROR;
}   /* eof R_usb_hstd_MgrChangeDeviceState() */


/******************************************************************************
Function Name   : R_usb_hstd_ChangeDeviceState
Description     : Change Device State
Arguments       : uint16_t msginfo          : new device status
Return value    : usb_er_t                  : Error code
******************************************************************************/
usb_er_t R_usb_hstd_ChangeDeviceState(usb_strct_t msginfo)
{
    switch (msginfo)
    {
        case USB_DO_PORT_ENABLE:
            usb_hstd_vbus_control_on();
        break;
//      case USB_DO_PORT_DISABLE:
//          usb_hstd_vbus_control_off();
//      break;
        case USB_DO_INITHWFUNCTION:
            usb_cstd_hw_start();                        /* Begining the clock supply of USB-IP*/
            usb_cstd_sw_reset();                        /* USB-IP initialized */
        break;
        case USB_DO_SETHWFUNCTION:
            /* USB interrupt message initialize, select HW function */
            usb_hstd_set_hw_function();
        break;
        default:
            return  USB_E_ERROR;
        break;
    }
    return  USB_E_OK;
}   /* eof R_usb_hstd_ChangeDeviceState() */


/******************************************************************************
Function Name   : R_usb_hstd_DeviceInformation
Description     : Return to device information
Argument        : usb_addr_t devaddr        : Device address
                : uint16_t *tbl             : Table Pointer
Return          : None
******************************************************************************/
void R_usb_hstd_DeviceInformation(usb_addr_t addr, uint16_t *table)
{
    if( addr == 0 )
    {
        /* When "0" is specified for the device address, it responds as follows. */
        /* When the device is not operating the enumeration */
        /*      table[0]    = USB_NOPORT        */
        /*      table[1]    = USB_STS_DETACH    */
        /* When the device is operating the enumeration */
        /*      table[0]    = Port number       */
        /*      table[1]    = USB_STS_DEFAULT   */
        table[0]    = g_usb_HcdDevInfo[0].port;
        table[1]    = USB_STS_DEFAULT;
        if( table[0] == USB_NOPORT )
        {
            table[1]    = USB_STS_DETACH;
        }
        return;
    }

    if (g_usb_MgrSequence != USB_0)
    {
//@@@   Should respond while processing the sequence?
        return;
    }

//@@@   Trouble occurs when this function is called because g_usb_HcdDevInfo[] cannot correct the setting.
    table[0]    = g_usb_HcdDevInfo[addr].port;
    table[1]    = g_usb_HcdDevInfo[addr].state;
    table[2]    = g_usb_HcdDevInfo[addr].config;
    table[3]    = g_usb_HcdDevInfo[addr].speed;

    /* The code conversion is necessary for the suspend status. */
    if ((g_usb_HcdDevInfo[addr].state == USB_STS_DS_SUSPEND) ||
        (g_usb_HcdDevInfo[addr].state == USB_STS_PS_SUSPEND))
    {
        table[1]    = USB_STS_SUSPEND;
    }
//@@@   delete g_usb_MgrMode[]
//  table[4] = g_usb_MgrMode[0];
//  table[5] = g_usb_MgrMode[1];
}   /* eof R_usb_hstd_DeviceInformation() */


/******************************************************************************
Function Name   : R_usb_hstd_ChkPipeInfo
Description     : Check pipe information
Argument        : uint16_t *table          : DEF_EP table pointer
                : uint8_t  *descriptor      : Endpoint Descriptor table
Return          : usb_er_t                  : DIR_H_IN / DIR_H_OUT / USB_ERROR
******************************************************************************/
usb_er_t R_usb_hstd_ChkPipeInfo(uint16_t *table, uint8_t *descriptor)
{

    uint16_t    pipe_cfg, pipe_maxp, pipe_peri;
    uint16_t    retval, work1, work2;

    /* Check Endpoint descriptor */
    if( descriptor[USB_DESCRIPTOR_TYPE] != USB_DT_ENDPOINT )
    {
        return USB_E_ERROR;
    }

    /* set pipe configuration value */
    switch( (uint16_t)(descriptor[USB_EDNP_ATTRIBUTE] & USB_EP_TRNSMASK) )
    {
        /* Bulk Endpoint */
        case USB_EP_BULK:
            if( (table[0] < USB_MIN_PIPE_NO) || (table[0] > USB_PIPE5) )
            {
                return USB_E_ERROR;
            }
            pipe_cfg = USB_BULK;
        break;
        /* Interrupt Endpoint */
        case USB_EP_INT:
            if( (table[0] < USB_PIPE6) || (table[0] > USB_MAX_PIPE_NO) )
            {
                return USB_E_ERROR;
            }
            pipe_cfg = USB_INT;
        break;
        default:
            return USB_E_ERROR;
        break;
    }

    /* Set pipe configuration table */
    if( (descriptor[USB_EDNP_ADDRESS] & USB_EP_DIRMASK) == USB_EP_IN )
    {
        /* IN(receive) */
        pipe_cfg    |= (uint16_t)((table[1] & USB_DBLBFIELD) | USB_SHTNAKON | USB_DIR_H_IN);
        retval      = USB_DIR_H_IN;
    }
    else
    {
        /* OUT(send) */
        pipe_cfg    |= (uint16_t)((table[1] & USB_DBLBFIELD) | USB_DIR_H_OUT);
        retval      = USB_DIR_H_OUT;
    }

    /* Endpoint number set */
    pipe_cfg    |= (uint16_t)(descriptor[USB_EDNP_ADDRESS] & USB_EP_NUMMASK);

    /* set max packet size */
    pipe_maxp   =  (uint16_t)descriptor[USB_EDNP_MAXPACKETSIZE_LO];

    /* set interval counter */
	uint16_t tmp = USB_IITVFIELD;
	tmp = ~tmp;
    pipe_peri   = (uint16_t)(table[3] & tmp);
    if( descriptor[USB_EDNP_INTERVAL] != 0 )
    {
        /* The time of the FS/LS interrupt forwarding of the interval is specified by the unit of ms. */
        /* It is necessary to calculate to specify USB-IP by the n-th power of two. */
        /* The NAK rate of the control and the bulk transfer doesn't correspond. */
        if ((pipe_cfg & USB_TYPFIELD) == USB_INT)
        {
            work1 = descriptor[6];
            work2 = 0;
            for(  ; work1 != 0; work2++ )
            {
                work1 = (uint16_t)(work1 >> 1);
            }
            if( work2 != 0 )
            {
                /* Interval time */
                pipe_peri |= (uint16_t)(work2 - 1);
            }
        }
    }


    /* Buffer flash */
    table[1]    = pipe_cfg;
    table[2]    = pipe_maxp;
    table[3]    = pipe_peri;

    return (retval);
}   /* eof R_usb_hstd_ChkPipeInfo() */


/******************************************************************************
Function Name   : R_usb_hstd_ReturnEnuMGR
Description     : Return to Enumeration of MGR
Argument        : uint16_t cls_result       : class check result
Return          : none
******************************************************************************/
void R_usb_hstd_ReturnEnuMGR(uint16_t cls_result)
{
    g_usb_MgrSequence++;
    R_USB_GET_SND(USB_MGR_MBX, USB_MGR_CONTINUE, &usb_cstd_dummy_function, (usb_strct_t)cls_result);
}   /* eof R_usb_hstd_ReturnEnuMGR() */


/******************************************************************************
Function Name   : R_usb_hstd_SetPipeRegistration
Description     : Set pipe configuration register
Arguments       : uint16_t *table       : pipe information table
                : uint16_t command      : control command
Return value    : none
******************************************************************************/
void R_usb_hstd_SetPipeRegistration(uint16_t *table, uint16_t command)
{
    usb_cstd_set_pipe_registration(table, command);
}   /* eof R_usb_hstd_SetPipeRegistration() */


/******************************************************************************
End of file
******************************************************************************/
