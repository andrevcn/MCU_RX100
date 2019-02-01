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
* File Name    : r_usb_cstdapi.c
* Version      : 2.10
* Description  : This is the USB basic firmware library API function code.
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
#pragma section program P_usblib
#pragma section bss B_usblib
#endif
#ifdef  RL78USB
#endif
#ifdef  RL78USB_SECTION
#pragma section @@CODE   P_LIB
#pragma section @@BASE   T_LIB
#pragma section @@DATA   B_LIB
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
extern usb_msg_t        *g_usb_LibTableAdd[][USB_TABLEMAX];
extern uint8_t          g_usb_LibPriR[];
extern uint8_t          g_usb_LibPriW[];
extern usb_tskinfo_t    g_usb_LibBlock[];
extern usb_waitinfo_t   g_usb_LibWaitMessage[];
extern usb_waitque_t    g_usb_LibWaitQueue[];
extern usb_msg_t        *g_usb_LibNextAddr;
extern uint8_t          g_usb_LibNextId;                        /* Priority Counter */
extern uint8_t          g_usb_LibReadPtr;                       /* Priority Table Reading pointer */
extern usb_pipe_t       g_usb_CurrentPipe;                      /* Pipe number */

/* functions */
extern void             usb_cstd_wait_scheduler(void);
extern void             usb_cstd_update_wait_queue(void);
extern void             usb_cstd_reg_set_pipexctr(uint16_t field);
extern void             usb_cpu_int_enable(void);
extern void             usb_cpu_int_disable(void);
extern uint16_t         usb_cstd_read_frmnum(void);



/******************************************************************************
Renesas Abstracted USB basic firmware library API functions
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_Scheduler
Description     : Task scheduler
Argument        : none
Return          : uint8_t USB_FLGSET    Task that can be operated. 
                : uint8_t USB_FLGCLR    No task that can be operated. 
******************************************************************************/
uint8_t R_usb_cstd_Scheduler(void)
{
    uint8_t tsk_pri;        /* Priority Counter */
    uint8_t rd_ptr;         /* Priority Table Reading pointer */

    /* wait msg */
    usb_cstd_wait_scheduler();

    /* Priority table search */
    for ( tsk_pri = USB_0; tsk_pri < USB_IDMAX; tsk_pri++)//AUMENTAR O IDMAX
    {
        rd_ptr  = g_usb_LibPriR[tsk_pri];
        if (rd_ptr != g_usb_LibPriW[tsk_pri])
        {
            /* Next message read */
            rd_ptr++;
            if (rd_ptr >= USB_TABLEMAX)
            {
                rd_ptr = USB_0;
            }
            /* Set practice message */
            g_usb_LibNextAddr       = g_usb_LibTableAdd[tsk_pri][rd_ptr];
            g_usb_LibNextId         = tsk_pri;
            g_usb_LibReadPtr        = rd_ptr;
            return  USB_FLGSET;
        }
    }
    /* Task is not executing */
    return  USB_FLGCLR;
}   /* eof R_usb_cstd_Scheduler() */


/******************************************************************************
Function Name   : R_usb_cstd_RecMsg
Description     : Receive Message
Argument        : uint8_t   id              : ID Number
                : usb_msg_t **mess          : Message Pointer
Return          : uint16_t                  : USB_E_OK / USB_E_ERROR
******************************************************************************/
usb_er_t R_usb_cstd_RecMsg( uint8_t id, usb_msg_t** mess )
{
    if (id == g_usb_LibNextId)
    {
        /* Task is executing */
        *mess                           = g_usb_LibNextAddr;
        g_usb_LibPriR[g_usb_LibNextId]  = g_usb_LibReadPtr;
        return  USB_E_OK;
    }
    /* Task is not executing */
    return  USB_E_ERROR;
}   /* eof R_usb_cstd_RecMsg() */


/******************************************************************************
Function Name   : R_usb_cstd_SndMsg
Description     : Send Message
Argument        : uint8_t   id              : ID Number
                : usb_msg_t *mess           : Message Pointer
Return          : usb_er_t                  : USB_E_OK / USB_E_ERROR
******************************************************************************/
usb_er_t R_usb_cstd_SndMsg( uint8_t id, usb_msg_t* mess )
{
    usb_er_t    status;

    if (g_usb_LibWaitMessage[id].Counter == USB_0)
    {
        /* The message is preserved in the priority table while not waiting. */
        usb_cpu_int_disable();                      /* USB interrupt disable */
        status  = R_usb_cstd_iSndMsg(id,mess);      /* Message send */
        usb_cpu_int_enable();                       /* USB interrupt enable */
    }
    else
    {
        /* The message is preserved in the wait queue table because it is waiting. */
        status = R_usb_cstd_WaiMsg( id, mess, USB_1 );
    }
    return status;
}   /* eof R_usb_cstd_SndMsg() */


/******************************************************************************
Function Name   : R_usb_cstd_iSndMsg
Description     : Send Message(for interrupt)
Argument        : uint8_t   id              : ID Number
                : usb_msg_t *mess           : Message Pointer
Return          : usb_er_t                  : USB_E_OK / USB_E_ERROR
******************************************************************************/
usb_er_t R_usb_cstd_iSndMsg( uint8_t id, usb_msg_t* mess )
{
    uint8_t wr_ptr;                                     /* Priority table write pointer */

    if (id < USB_IDMAX)
    {
        /* Read priority and table pointer */
        wr_ptr  = g_usb_LibPriW[id];
        wr_ptr++;                                       /* Renewal write pointer */
        if (wr_ptr >= USB_TABLEMAX)
        {
            wr_ptr = USB_0;
        }
        if (wr_ptr != g_usb_LibPriR[id])                /* Check pointer */
        {
            /* Save message & Set priorty table */
            g_usb_LibTableAdd[id][wr_ptr]   = mess;
            g_usb_LibPriW[id]               = wr_ptr;
            return  USB_E_OK;
        }
        /* Error of Sucedule Table Full !!  */
    }
    /* Error of argument !!  */
    return  USB_E_ERROR;
}   /* eof R_usb_cstd_iSndMsg() */


/******************************************************************************
Function Name   : R_usb_cstd_WaiMsg
Description     : Wait Message
Argument        : uint8_t   id              : ID Number
                : usb_msg_t *mess           : Message pointer
                : uint16_t  count           : Timeout Counter
Return          : usb_er_t                  : USB_E_OK / USB_E_ERROR
******************************************************************************/
usb_er_t R_usb_cstd_WaiMsg( uint8_t id, usb_msg_t* mess, uint16_t count )
{
    uint8_t     i;

    if (id < USB_IDMAX)
    {
        if (count != USB_0)
        {
            /* Set delay timer */
            if (g_usb_LibWaitMessage[id].Counter == USB_0)
            {
                /* The counter is set and the message is preserved temporarily while not waiting. */
                g_usb_LibWaitMessage[id].Address    = mess;
                g_usb_LibWaitMessage[id].Counter    = count;
                return  USB_E_OK;
            }
            else
            {
                /* The message is preserved in the wait queue table because it is waiting(Time is forgotten). */
                for (i = 0; i < USB_TABLEMAX; i++)
                {
                    if (g_usb_LibWaitQueue[i].TaskID == USB_IDCLR)
                    {
                        g_usb_LibWaitQueue[i].TaskID    = id;
                        g_usb_LibWaitQueue[i].Address   = mess;
                        return  USB_E_OK;
                    }
                }
            }
        }
        else
        {
            /* Clear delay timer */
            g_usb_LibWaitMessage[id].Counter    = USB_0;
            for (i = 0; i < USB_TABLEMAX; i++)
            {
                if (g_usb_LibWaitQueue[i].TaskID == id)
                {
                    g_usb_LibWaitQueue[i].TaskID    = USB_IDCLR;
                }
            }
            usb_cstd_update_wait_queue();
            return  USB_E_OK;
        }
    }
    /* Error of argument !!  */
    return  USB_E_ERROR;
}   /* eof R_usb_cstd_WaiMsg() */


/******************************************************************************
Function Name   : R_usb_cstd_PgetSend
Description     : Get memory block
Argument        : uint8_t  id               : ID Number
                : usb_strct_t  msginfo      : Message Info
                : usb_cbinfo_t complete     : Callback function
                : usb_strct_t  keyword      : Message Keyword
Return          : usb_er_t                  : USB_E_OK / USB_E_ERROR
******************************************************************************/
usb_er_t R_usb_cstd_PgetSend(uint8_t id, usb_strct_t msginfo, usb_cbinfo_t complete, usb_strct_t keyword)
{
    usb_tskinfo_t   *ptr;
    uint8_t         blk_num;

    if (id < USB_IDMAX)
    {
        for ( blk_num = USB_0; blk_num < USB_BLKMAX; blk_num++)
        {
            ptr     = &g_usb_LibBlock[blk_num];
            if (ptr->flag == USB_IDCLR)
            {
                /* Acquire fixed-size memory block & send message */
                ptr->keyword    = keyword;
                ptr->msginfo    = msginfo;
                ptr->complete   = complete;
                if (R_USB_SND_MSG(id, (usb_msg_t*)ptr) == USB_E_OK)
                {
                    ptr->flag   = blk_num;
                    return  USB_E_OK;
                }
                /* Error of Send_Message !!  */
                return  USB_E_ERROR;
            }
        }
        /* Error of block table full !!  */
    }
    /* Error of argument !!  */
    return  USB_E_ERROR;
}   /* eof R_usb_cstd_PgetSend() */

/******************************************************************************
Function Name   : R_usb_cstd_RelBlk
Description     : Release memory block
Argument        : uint8_t blk_num           : Used block index
Return          : usb_er_t                  : USB_E_OK / USB_E_ERROR
******************************************************************************/
usb_er_t R_usb_cstd_RelBlk( uint8_t blk_num )
{
    if ((blk_num < USB_BLKMAX) && (g_usb_LibBlock[blk_num].flag == blk_num))
    {
        g_usb_LibBlock[blk_num].flag    = USB_IDCLR;
        return  USB_E_OK;
    }
    /* Error of argument !!  */
    return  USB_E_ERROR;
}   /* eof R_usb_cstd_RelBlk() */


/******************************************************************************
Function Name   : R_usb_cstd_SetBufPipe0
Description     : Set pipe0 USB_PID_BUF
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_cstd_SetBufPipe0(void)
{
    g_usb_CurrentPipe = USB_PIPE0;
    usb_cstd_reg_set_pipexctr(USB_PID_BUF);
}   /* eof R_usb_cstd_RelBlk() */


/******************************************************************************
Function Name   : R_usb_cstd_debug_hook
Description     : Debug hook
Arguments       : uint16_t error_code          : error code
Return value    : none
******************************************************************************/
void R_usb_cstd_debug_hook(uint16_t error_code)
{
   while(1);
}   /* eof R_usb_cstd_debug_hook() */

/******************************************************************************
End of file
******************************************************************************/
