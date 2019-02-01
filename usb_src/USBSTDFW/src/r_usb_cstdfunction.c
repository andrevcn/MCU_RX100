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
* File Name    : r_usb_cstdfunction.c
* Version      : 2.10
* Description  : This is the USB basic firmware library function code.
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
#include "r_usb_regaccess.h"            /* Definition of the USB register access macro */
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
/* variables */

/* functions */
/* FIFO buffer access library function */
USB_STATIC  void        usb_cstd_buf_2_fifo(void);
USB_STATIC  void        usb_cstd_fifo_2_buf(void);
USB_STATIC  void        usb_cstd_data_end(uint16_t status);
USB_STATIC  uint16_t    usb_cstd_set_cfifo(uint16_t isel);
USB_STATIC  void        usb_cstd_set_cfifo_pipeX(void);
USB_STATIC  void        usb_cstd_clr_transaction_counter(void);
/* USB-IP register access library function (read/write/set/clear/etc) */
USB_STATIC  void        usb_cstd_do_sqtgl(uint16_t toggle);
USB_STATIC  uint16_t    usb_cstd_get_max_packet_size(void);
USB_STATIC  void        usb_cstd_set_pipe_config(uint16_t *table);
USB_STATIC  void        usb_cstd_reg_clr_pipexctr(uint16_t field);
USB_STATIC  uint16_t    usb_cstd_read_inbufm(void);
USB_STATIC  void        usb_cstd_do_aclrm(void);


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */
/*Interrupt global variables */
uint16_t        g_usb_LibTemp;                              /* USB register access macro temp */
usb_intinfo_t   g_usb_LibIntMessage[USB_INTMSGMAX];         /* Interrupt message */
uint8_t         g_usb_LibIntMsgCnt;                         /* Interrupt message count */
usb_utr_t       *g_usb_LibPipe[USB_MAX_PIPE_NO + 1u];       /* Message pipe */
uint8_t         *g_usb_LibDataPtr[USB_MAX_PIPE_NO + 1u];        /* PIPEn Buffer pointer(8bit) */
usb_leng_t      g_usb_LibDataCnt[USB_MAX_PIPE_NO + 1u];     /* PIPEn Buffer counter */
usb_pipe_t      g_usb_CurrentPipe;                          /* Pipe number */
/* Scheduler global variables */
usb_tskinfo_t   g_usb_LibBlock[USB_BLKMAX];
usb_msg_t       *g_usb_LibTableAdd[USB_IDMAX][USB_TABLEMAX];
uint8_t         g_usb_LibPriR[USB_IDMAX];
uint8_t         g_usb_LibPriW[USB_IDMAX];
/* Scheduler wait message */
usb_waitinfo_t  g_usb_LibWaitMessage[USB_IDMAX];
usb_waitque_t   g_usb_LibWaitQueue[USB_TABLEMAX];           /* Queuing as for some messages. */
/* Scheduler global flags */
usb_msg_t       *g_usb_LibNextAddr;
uint8_t         g_usb_LibNextId;        /* Priority Counter */
uint8_t         g_usb_LibReadPtr;       /* Priority Table Reading pointer */

#ifdef __RX
extern  void        usb_cpu_set_d0fifo_pipeX(void);
extern  uint16_t    usb_cpu_get_transfer_mode(void);
extern  void        usb_cpu_buf2fifo_start_dma();
extern  void        usb_cpu_fifo2buf_start_dma();
extern  void        usb_cpu_d0fifo_stop_dma();
extern  void        usb_cpu_d0fifo_stop_usb();
extern  uint16_t    usb_cpu_get_dtc_block_count();

/* DMA0 direction */
extern uint16_t        usb_gcstd_Dma0Dir;
/* DMA0 buffer size */
extern usb_leng_t      usb_gcstd_Dma0Size;
/* DMA0 FIFO buffer size */
extern uint16_t        usb_gcstd_Dma0Fifo;
/* DMA0 pipe number */
extern uint16_t        usb_gcstd_Dma0Pipe;
#endif  /* __RX */



/* functions */
/* global variables intialized */
void            usb_cstd_init_library(void);
/* scheduler library function */
void            usb_cstd_wait_scheduler(void);
void            usb_cstd_update_wait_queue(void);
/* FIFO buffer access library function */
void            usb_cstd_send_start(void);
uint8_t         usb_cstd_buf_2_cfifo(void);
void            usb_cstd_receive_start(void);
uint8_t         usb_cstd_cfifo_2_buf(void);
void            usb_cstd_forced_termination(uint16_t status);
void            usb_cstd_set_cfifo_pipe0r(void);
void            usb_cstd_set_cfifo_pipe0w(void);
void            usb_cstd_clr_cfifo_pipe0r(void);
void            usb_cstd_clr_cfifo_pipe0w(void);
/* FIFO buffer interrupt function */
void            usb_cstd_brdy_pipe(uint16_t bitsts);
void            usb_cstd_nrdy_pipe(uint16_t bitsts);
void            usb_cstd_bemp_pipe(uint16_t bitsts);
/* FIFO buffer interrupt enable/disable */
void            usb_cstd_brdy_enable(void);
void            usb_cstd_brdy_disable(void);
void            usb_cstd_bemp_enable(void);
void            usb_cstd_bemp_disable(void);
void            usb_cstd_nrdy_enable(void);
void            usb_cstd_nrdy_disable(void);
void            usb_cstd_berne_enable(void);
/* USB-IP register access library function (read/write/set/clear/etc) */
uint16_t        usb_cstd_get_pipe_dir(void);
void            usb_cstd_clr_pipe_config(void);
void            usb_cstd_reg_set_pipexctr(uint16_t field);
uint16_t        usb_cstd_reg_read_pipexctr(void);
uint16_t        usb_cstd_set_nak(void);
void            usb_cstd_clr_stall(void);
uint16_t        usb_cstd_get_pid(void);
/* Else library function */
void            usb_cstd_dummy_function(uint16_t data1, uint16_t data2);
void            usb_cstd_interrupt_disable(void);
void            usb_cstd_sw_reset(void);
void            usb_cstd_hw_stop(void);
void            usb_cstd_hw_start(void);
void            usb_cstd_set_pipe_registration(uint16_t *table, uint16_t command);

uint16_t        usb_cstd_read_frmnum(void);


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* variables */

/* functions */
extern uint8_t          usb_hstd_check_attach(void);



/******************************************************************************
Renesas Abstracted USB basic firmware library functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_init_library
Description     : Initialized global variables
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_init_library(void)
{
//  All global variables are initialized specifying it. 
//  However, an unnecessary variable to initialize ends comment by "//". 
//unnecessary   g_usb_LibDataPtr[];         /* PIPEn Buffer pointer(8bit) */
//unnecessary   g_usb_LibDataCnt[];         /* PIPEn Buffer counter */
//unnecessary   g_usb_LibTemp;              /* USB register access macro temp */
//unnecessary   g_usb_LibTableAdd[][];
//unnecessary   g_usb_LibNextAddr;
//unnecessary   g_usb_LibNextId;            /* Priority Counter */
///unnecessary  g_usb_LibReadPtr;           /* Priority Table Reading pointer */

    uint8_t i;

    /* Initialize priority table pointer and priority table */
    for (i = USB_0; i != USB_IDMAX; i++)
    {
        g_usb_LibPriR[i]    = USB_0;
        g_usb_LibPriW[i]    = USB_0;
        g_usb_LibWaitMessage[i].Counter = USB_0;
    }
    /* Initialize wait table */
    for (i = USB_0; i != USB_TABLEMAX;i ++)
    {
        g_usb_LibWaitQueue[i].TaskID    = USB_IDCLR;
    }

    /* Initialize block table */
    for (i = USB_0; i != USB_BLKMAX; i++)
    {
        g_usb_LibBlock[i].flag  = USB_IDCLR;
    }

    /* Initialize pipe transfer data */
    for( i = USB_PIPE0; i <= USB_MAX_PIPE_NO; i++ )
    {
        g_usb_LibPipe[i]        = (usb_utr_t*)USB_NULL;
    }

    /* Initialize interrupt message */
    for( i = USB_0; i < USB_INTMSGMAX; i++ )
    {
        g_usb_LibIntMessage[i].msginfo = USB_INTERRUPT;
    }

    g_usb_LibIntMsgCnt  = USB_0;
    g_usb_CurrentPipe   = USB_PIPE0;
}   /* eof usb_cstd_init_library() */


/******************************************************************************
Function Name   : usb_cstd_wait_scheduler
Description     : Wait request sceduling
Argument        : none
Return          : none
******************************************************************************/
void usb_cstd_wait_scheduler(void)
{
    uint8_t     i, j;

    for( j = USB_0; j < USB_IDMAX; j++ )
    {
      if(g_usb_LibWaitMessage[j].Counter != USB_0)
        {

            g_usb_LibWaitMessage[j].Counter--;
            if (g_usb_LibWaitMessage[j].Counter == USB_0)
            {
                if( R_usb_cstd_SndMsg(j, g_usb_LibWaitMessage[j].Address) != USB_E_OK )
                {
                    /* Message is carrying over next time because it was not delayed. */
                    g_usb_LibWaitMessage[j].Counter = 1;
                }
                else
                {
                    /* Because the message was able to be preserved in the priority table, */
                    /* the wait queue table is updated. */
                    for (i = 0; i < USB_TABLEMAX; i++)
                    {
                        /* The same ID that's in the wait queue table moves to the priority table. */
                        if (g_usb_LibWaitQueue[i].TaskID == j)
                        {
                            if( R_usb_cstd_SndMsg(j, g_usb_LibWaitQueue[j].Address) != USB_E_OK )
                            {
                                R_usb_cstd_WaiMsg( j, g_usb_LibWaitQueue[j].Address, USB_1 );
                            }
                            g_usb_LibWaitQueue[i].TaskID    = USB_IDCLR;
                        }
                    }
                    usb_cstd_update_wait_queue();
                }
            }
        }
    }
}   /* eof usb_cstd_wait_scheduler() */


/******************************************************************************
Function Name   : usb_cstd_update_wait_queue
Description     : Wait queue update
Argument        : none
Return          : none
******************************************************************************/
void usb_cstd_update_wait_queue(void)
{
    uint8_t     i, j;

    for (i = 0, j = 1; j < USB_TABLEMAX; i++, j++)
    {
        if (g_usb_LibWaitQueue[i].TaskID == USB_IDCLR)
        {
            if (g_usb_LibWaitQueue[j].TaskID != USB_IDCLR)
            {
                g_usb_LibWaitQueue[i].Address   = g_usb_LibWaitQueue[j].Address;
                g_usb_LibWaitQueue[i].TaskID    = g_usb_LibWaitQueue[j].TaskID;
                g_usb_LibWaitQueue[j].TaskID    = USB_IDCLR;
            }
            else
            {
                i--;
            }
        }
    }
}   /* eof usb_cstd_update_wait_queue() */


/******************************************************************************
Function Name   : usb_cstd_send_start
Description     : Send Data start
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_send_start(void)
{
    usb_utr_t       *ptr;

    /* Evacuation pointer */
    ptr     = g_usb_LibPipe[g_usb_CurrentPipe];

    /* Sequence toggle */
    usb_cstd_do_sqtgl(ptr->pipectr);

    /* Set data count, data pointer */
    usb_cstd_set_nak();
    g_usb_LibDataCnt[g_usb_CurrentPipe] = ptr->tranlen;
    g_usb_LibDataPtr[g_usb_CurrentPipe] = (uint8_t*)ptr->tranadr;

    /* Pipe number to FIFO port select */
    USB_CLR_STS(BEMPSTS, USB_BITSET(g_usb_CurrentPipe));
    USB_CLR_STS(BRDYSTS, USB_BITSET(g_usb_CurrentPipe));

#ifdef __RX
    if( USB_D0DMA == usb_cpu_get_transfer_mode() )
    {
        /* Setting for use PIPE number */
        usb_gcstd_Dma0Pipe = g_usb_CurrentPipe;
        /* PIPE direction */
        usb_gcstd_Dma0Dir  = usb_cstd_get_pipe_dir();
        /* Buffer size */
        usb_gcstd_Dma0Fifo = usb_cstd_get_max_packet_size();
        /* Check data count */
        if( g_usb_LibDataCnt[usb_gcstd_Dma0Pipe] <= usb_gcstd_Dma0Fifo )
        {
            /* Transfer data size */
            usb_gcstd_Dma0Size = (uint16_t)g_usb_LibDataCnt[usb_gcstd_Dma0Pipe];
            /* Enable Empty Interrupt */
            usb_cstd_bemp_enable();
        }
        else
        {
            /* Transfer data size */
            usb_gcstd_Dma0Size = usb_gcstd_Dma0Fifo;
        }

        usb_cpu_buf2fifo_start_dma();
        /* Set BUF */
        usb_cstd_reg_set_pipexctr(USB_PID_BUF);
    }
    else
    {
    /* Buffer to FIFO data write */
    usb_cstd_buf_2_fifo();
    usb_cstd_reg_set_pipexctr(USB_PID_BUF);
    }
#else   /* __RX */
    /* Buffer to FIFO data write */
    usb_cstd_buf_2_fifo();
    usb_cstd_reg_set_pipexctr(USB_PID_BUF);
#endif  /* __RX */
}   /* eof usb_cstd_send_start() */


/******************************************************************************
Function Name   : usb_cstd_buf_2_fifo
Description     : Buffer to FIFO data write
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_cstd_buf_2_fifo(void)
{
    /* Disable Ready Interrupt */
    usb_cstd_brdy_disable();

    /* Buffer to CFIFO write */
    switch( usb_cstd_buf_2_cfifo() )
    {
        case USB_WRITING:           /* Continue of data write */
            /* Enable BRDY/NRDY Interrupt */
            usb_cstd_brdy_enable();
            usb_cstd_nrdy_enable();
        break;
        case USB_WRITEEND:          /* End of data write */
        /* continue */
        case USB_WRITESHRT:         /* End of data write */
            /* Enable BEMP/NRDY Interrupt */
            usb_cstd_bemp_enable();
            usb_cstd_nrdy_enable();
        break;
        case USB_FIFOERROR:         /* FIFO access error */
        /* continue */
        default:
            usb_cstd_forced_termination(USB_DATA_ERR);
        break;
    }

}   /* eof usb_cstd_buf_2_fifo() */


/******************************************************************************
Function Name   : usb_cstd_buf_2_cfifo
Description     : Buffer to CFIFO data write
Arguments       : none
Return value    : uint16_t end_flag
******************************************************************************/
uint8_t usb_cstd_buf_2_cfifo(void)
{
    uint16_t    count, even, buffer, mxps;
    uint8_t     end_flag;
    usb_pipe_t  pipe;

    pipe    = g_usb_CurrentPipe;

    /* Changes FIFO port by the pipe. */
    if( pipe == USB_PIPE0 )
    {
        buffer = usb_cstd_set_cfifo(USB_ISEL);
    }
    else
    {
        buffer = usb_cstd_set_cfifo(USB_NULL);
    }
    /* Check error */
    if( buffer == USB_FIFOERROR )
    {
        return (USB_FIFOERROR);
    }

    mxps = usb_cstd_get_max_packet_size();

    /* Write continues */
    end_flag    = USB_WRITING;
    count       = mxps;

    /* Data size check */
    if( g_usb_LibDataCnt[pipe] <= (usb_leng_t)mxps )
    {
        count       = (uint16_t)g_usb_LibDataCnt[pipe];
        end_flag    = USB_WRITEEND;
        if ( (count == USB_0) || (count != mxps) )
        {
            /* Null Packet is end of write */
            /* Short Packet is end of write */
            end_flag    = USB_WRITESHRT;
        }
        else
        {
            if( pipe == USB_PIPE0 )
            {
                /* Just Send Size */
                end_flag    = USB_WRITING;
            }
        }
    }

    /* FIFO write */
#ifdef __RX /* RX family Big-Endian setting when process */
    for( even = count/2; (even != USB_0); --even )
    {
        /* 8bit access */
        USB_WR(CFIFO, (uint16_t)(*((uint16_t*)g_usb_LibDataPtr[pipe])));
        g_usb_LibDataPtr[pipe] += 2;
    }
    if((count % 2) == 1)
    {
        /* odd number data access */
        USB_CLR_PAT(CFIFOSEL,USB_MBW_16);
        USB_WR(CFIFO, (uint16_t)(*((uint8_t*)g_usb_LibDataPtr[pipe])));
        g_usb_LibDataPtr[pipe]++;
        USB_SET_PAT(CFIFOSEL,USB_MBW_16);
    }
#else /* Other MCU when process */
    for( even = count; (even != USB_0); --even )
    {
        /* 16bit access */
        USB_WR(CFIFO, (uint16_t)(*((uint8_t*)g_usb_LibDataPtr[pipe])));
        g_usb_LibDataPtr[pipe]++;
    }
#endif
    /* Check data count to remain */
    if( g_usb_LibDataCnt[pipe] < (usb_leng_t)mxps )
    {
        /* Short Packet */
        g_usb_LibDataCnt[pipe] = USB_0;
        USB_WR(CFIFOCTR, USB_BVAL);
    }
    else
    {
        /* Total data count - count */
        g_usb_LibDataCnt[pipe] -= count;
    }
    return end_flag;

}   /* eof usb_cstd_buf_2_cfifo() */



/******************************************************************************
Function Name   : usb_cstd_receive_start
Description     : Receive Data start
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_receive_start(void)
{
    usb_utr_t       *ptr;
    uint16_t        trncnt;

    /* Evacuation pointer */
    ptr     = g_usb_LibPipe[g_usb_CurrentPipe];

    /* Sequence toggle */
    usb_cstd_do_sqtgl(ptr->pipectr);

    /* Set data count, data pointer */
    usb_cstd_set_nak();
    g_usb_LibDataCnt[g_usb_CurrentPipe] = ptr->tranlen;
    g_usb_LibDataPtr[g_usb_CurrentPipe] = (uint8_t*)ptr->tranadr;
#ifdef __RX
    if( USB_D0DMA == usb_cpu_get_transfer_mode() )
    {
        usb_gcstd_Dma0Pipe = g_usb_CurrentPipe;              /* Setting for use PIPE number */
        usb_gcstd_Dma0Dir  = usb_cstd_get_pipe_dir();        /* PIPE direction */
        usb_gcstd_Dma0Fifo = usb_cstd_get_max_packet_size(); /* Buffer size */
        /* Transfer data size */

        /* Check data count */
        if( g_usb_LibDataCnt[usb_gcstd_Dma0Pipe] < usb_gcstd_Dma0Fifo )
        {
            usb_gcstd_Dma0Size = (uint16_t)g_usb_LibDataCnt[usb_gcstd_Dma0Pipe];
        }
        else
        {
            usb_gcstd_Dma0Size = usb_gcstd_Dma0Fifo;
        }
        usb_cpu_fifo2buf_start_dma();
    }
    else
    {
#endif  /* __RX */
    /* Pipe number to FIFO port select */
    usb_cstd_set_cfifo_pipeX();

    if( ptr->tranlen != USB_0 )
    {
//@@@   It is describing necessary for APN, that the Bulk-Pipe is limited as the MAXP=64. 
        trncnt  = (uint16_t)(ptr->tranlen>>6);          /* Packet size is 64byte only when bulk pipe is selected. */
        if ((ptr->tranlen&0x003Fu) != USB_0)
        {
            trncnt++;
        }
        switch( g_usb_CurrentPipe )
        {
#ifdef  __RX
            case USB_PIPE1:             /* Pipe1 transaction counter register set & count enable  */
                USB_SET_PAT(PIPE1TRE, USB_TRCLR);
                USB_WRW(PIPE1TRN, trncnt);
                USB_SET_PAT(PIPE1TRE, USB_TRENB);
            break;
            case USB_PIPE2:             /* Pipe2 transaction counter register set & count enable  */
                USB_SET_PAT(PIPE2TRE, USB_TRCLR);
                USB_WRW(PIPE2TRN, trncnt);
                USB_SET_PAT(PIPE2TRE, USB_TRENB);
            break;
            case USB_PIPE3:             /* Pipe3 transaction counter register set & count enable  */
                USB_SET_PAT(PIPE3TRE, USB_TRCLR);
                USB_WRW(PIPE3TRN, trncnt);
                USB_SET_PAT(PIPE3TRE, USB_TRENB);
            break;
#endif  /* __RX */
            case USB_PIPE4:             /* Pipe4 transaction counter register set & count enable  */
                USB_SET_PAT(PIPE4TRE, USB_TRCLR);
                USB_WRW(PIPE4TRN, trncnt);
                USB_SET_PAT(PIPE4TRE, USB_TRENB);
            break;
            case USB_PIPE5:             /* Pipe5 transaction counter register set & count enable  */
                USB_SET_PAT(PIPE5TRE, USB_TRCLR);
                USB_WRW(PIPE5TRN, trncnt);
                USB_SET_PAT(PIPE5TRE, USB_TRENB);
            break;
            default:
                /* no processing */
            break;
        }
    }

    /* Enable Brdy/Nrdy Interrupt */
    usb_cstd_reg_set_pipexctr(USB_PID_BUF);
    usb_cstd_brdy_enable();
    usb_cstd_nrdy_enable();
#ifdef __RX
    }
#endif  /* __RX */
}   /* eof usb_cstd_receive_start() */


/******************************************************************************
Function Name   : usb_cstd_fifo_2_buf
Description     : FIFO to Buffer data read
                : CFIFO     : Operation on the conditions of CNTMD=0 is possible
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_cstd_fifo_2_buf(void)
{
    /* CFIFO to buffer read */
    switch( usb_cstd_cfifo_2_buf() )
    {
        case USB_READING:
            /* Continue of data read (no processing) */
        break;
        case USB_READEND:
            /* End of data read */
            usb_cstd_data_end(USB_DATA_OK);
        break;
        case USB_READSHRT:
            /* End of data read */
            usb_cstd_data_end(USB_DATA_SHT);
        break;
        case USB_READOVER:
            /* Buffer over */
            usb_cstd_forced_termination(USB_DATA_OVR);
        break;
        case USB_FIFOERROR:
        /* continue */
        default:
            usb_cstd_forced_termination(USB_DATA_ERR);
        break;
    }
}   /* eof usb_cstd_fifo_2_buf() */


/******************************************************************************
Function Name   : usb_cstd_cfifo_2_buf
Description     : CFIFO to Buffer data read
                : Operation on the conditions of DBLB=0/CNTMD=0 is possible.
Arguments       : none
Return value    : uint16_t end_flag
******************************************************************************/
uint8_t usb_cstd_cfifo_2_buf(void)
{
    uint16_t    count, even, buffer, mxps, dtln;
    uint8_t     end_flag;
    usb_pipe_t  pipe;

    pipe    = g_usb_CurrentPipe;

    /* Changes FIFO port by the pipe. */
    buffer = usb_cstd_set_cfifo(USB_NULL);
    if( buffer == USB_FIFOERROR )
    {
        /* FIFO access error */
        return (USB_FIFOERROR);
    }

    dtln = (uint16_t)(buffer & USB_DTLN);
    /* Max Packet Size */
    mxps = usb_cstd_get_max_packet_size();

    if( g_usb_LibDataCnt[pipe] < dtln )                 /* Buffer Over ? */
    {
        end_flag = USB_READOVER;
        count = (uint16_t)g_usb_LibDataCnt[pipe];
        g_usb_LibDataCnt[pipe] = dtln;
    }
    else if( g_usb_LibDataCnt[pipe] == dtln )           /* Just Receive Size */
    {
        count = dtln;
        end_flag = USB_READEND;
#if USB_FUNCSEL_PP == USB_PERI_PP
        if( (pipe == USB_PIPE0) && (dtln == mxps) )
        {
            end_flag = USB_READING;
        }
#endif  /* USB_PERI_PP */
    }
    else                                                /* Continues Receive data */
    {
        count = dtln;
        end_flag = USB_READING;
        if ( (count == USB_0) || (count != mxps) )
        {
            /* Null Packet is end of read */
            /* Short Packet is end of read */
            end_flag = USB_READSHRT;
        }
    }
    if (end_flag != USB_READING)
    {
        usb_cstd_set_nak();
    }

    if( dtln == USB_0 )
    {
        /* 0 length packet */
        USB_WR(CFIFOCTR, USB_BCLR);
    }
    else
    {
#ifdef __RX /* RX Family Big-endian setting when process */
    for( even = count/2; (even != USB_0); --even )
    {
        USB_RD(CFIFO, buffer);
        *(uint16_t*)g_usb_LibDataPtr[pipe]  =(uint16_t)buffer;
        g_usb_LibDataPtr[pipe] += 2;
    }
    if(count%2 == 1)
    {
        USB_RD(CFIFO, buffer);
#ifdef __BIG
        buffer >>= 8;
#else
        buffer &= 0xFF;
#endif
        *g_usb_LibDataPtr[pipe] =(uint8_t)buffer;
        g_usb_LibDataPtr[pipe]++;
    }
#else /* Other MCU when process */
    for( even = count; (even != USB_0); --even )
    {
        USB_RD(CFIFO, buffer);
        *g_usb_LibDataPtr[pipe] =(uint8_t)buffer;
        g_usb_LibDataPtr[pipe]++;
    }
#endif
    }
    
    g_usb_LibDataCnt[pipe] -= count;
    return (end_flag);
}   /* eof usb_cstd_cfifo_2_buf() */


/******************************************************************************
Function Name   : usb_cstd_data_end
Description     : End of data transfer (IN/OUT)
                : In the case of timeout status, it does not call back.
Arguments       : uint16_t status       ; Transfer status type
Return value    : none
******************************************************************************/
USB_STATIC void usb_cstd_data_end(uint16_t status)
{
    usb_utr_t       *ptr;

    /* Evacuation pointer */
    ptr     = g_usb_LibPipe[g_usb_CurrentPipe];
    g_usb_LibPipe[g_usb_CurrentPipe]    = (usb_utr_t*)USB_NULL;

    /* Set NAK */
    usb_cstd_set_nak();

    /* Disable Interrupt */
    usb_cstd_brdy_disable();
    usb_cstd_nrdy_disable();
    usb_cstd_bemp_disable();

    /* Disable Transaction count */
    usb_cstd_clr_transaction_counter();

#ifdef __RX
    if( USB_D0DMA == usb_cpu_get_transfer_mode() )
    {
        /* DMA buffer clear mode clear */
        USB_CLR_PAT(D0FIFOSEL, USB_DCLRM);
        USB_MDF_PAT(D0FIFOSEL, USB_MBW_16, USB_MBW);
    }
#endif  /* __RX */

    /* Call Back */
    if( ptr != USB_NULL )
    {
        /* Transfer information set */
        ptr->tranlen    = g_usb_LibDataCnt[g_usb_CurrentPipe];
        ptr->status     = (usb_strct_t)status;
        ptr->pipectr    = usb_cstd_reg_read_pipexctr();
#ifdef __CA78K0R__
        ((void (*)(usb_utr_t*))*ptr->complete)(ptr);    /* for CA78K0R compiler */
#else
        (*ptr->complete)(ptr);
#endif
    }
}   /* eof usb_cstd_data_end() */


/******************************************************************************
Function Name   : usb_cstd_forced_termination
Description     : End of data transfer (IN/OUT)
Arguments       : uint16_t status       ; Transfer status type
Return value    : none
Note            : In the case of timeout status, it does not call back.
******************************************************************************/
void usb_cstd_forced_termination(uint16_t status)
{
    usb_utr_t       *ptr;
    uint16_t        buffer;
#ifdef __RX
    usb_pipe_t      current_pipe_bak;
#endif  /* __RX */

    /* Evacuation pointer */
    ptr     = g_usb_LibPipe[g_usb_CurrentPipe];
    g_usb_LibPipe[g_usb_CurrentPipe]    = (usb_utr_t*)USB_NULL;

    /* PID = NAK */
    usb_cstd_set_nak();

    /* Disable Interrupt */
    usb_cstd_brdy_disable();
    usb_cstd_nrdy_disable();
    usb_cstd_bemp_disable();

    usb_cstd_clr_transaction_counter();

#ifdef __RX
    USB_RD(D0FIFOSEL, buffer);
    if( (buffer & USB_CURPIPE) == g_usb_CurrentPipe )
    {
        /* Stop DMA,FIFO access */
        usb_cpu_d0fifo_stop_dma();
        usb_cpu_d0fifo_stop_usb();
        USB_CLR_PAT(D0FIFOSEL, USB_DCLRM);
        USB_MDF_PAT(D0FIFOSEL, USB_MBW, USB_MBW_16);

        current_pipe_bak = g_usb_CurrentPipe;
        g_usb_CurrentPipe = USB_PIPE0;
        /* Changes the FIFO port by the pipe. */
        usb_cpu_set_d0fifo_pipeX();
        g_usb_CurrentPipe = current_pipe_bak;

        /* Call Back */
        if( (status != USB_DATA_TMO) && (ptr != USB_NULL) )
        {
            /* Transfer information set */
            ptr->tranlen    = g_usb_LibDataCnt[g_usb_CurrentPipe];
            ptr->status     = (usb_strct_t)status;
            ptr->pipectr    = usb_cstd_reg_read_pipexctr();
            (*ptr->complete)(ptr);
        }
    }

    USB_RD(CFIFOSEL, buffer);
    if( (buffer & USB_CURPIPE) == g_usb_CurrentPipe )
    {
#endif  /* __RX */
    /* Changes the FIFO port by the pipe. */
    usb_cstd_set_cfifo_pipeX();
    USB_RD(CFIFOCTR, buffer);
    if( (uint16_t)(buffer & USB_FRDY) == USB_FRDY )
    {
        /* Clear BVAL */
        USB_WR(CFIFOCTR, USB_BCLR);
    }

    /* FIFO buffer SPLIT transaction initialized */
    usb_cstd_set_cfifo_pipe0r();

    /* Call Back */
    if( (status != USB_DATA_TMO) && (ptr != USB_NULL) )
    {
        /* Transfer information set */
        ptr->tranlen    = g_usb_LibDataCnt[g_usb_CurrentPipe];
        ptr->status     = (usb_strct_t)status;
        ptr->pipectr    = usb_cstd_reg_read_pipexctr();
#ifdef  __CA78K0R__
        ((void (*)(usb_utr_t*))*ptr->complete)(ptr);    /* for CA78K0R compiler */
#else
        (*ptr->complete)(ptr);
#endif
    }
#ifdef __RX
    }
#endif  /* __RX */

}   /* eof usb_cstd_forced_termination() */


/******************************************************************************
Function Name   : usb_cstd_set_cfifo
Description     : Changes the specified FIFO port by the specified pipe.
                : Please change the wait time for your MCU
Arguments       : uint16_t isel         ; ISEL bit status
Return value    : uint16_t FRDY status
******************************************************************************/
USB_STATIC uint16_t usb_cstd_set_cfifo(uint16_t isel)
{
    uint16_t    buffer;
    uint8_t     i;

    if ( isel == USB_ISEL )
    {
        usb_cstd_set_cfifo_pipe0w();        /* The FIFO port is changed with ISEL bit. */
    }
    else
    {
        usb_cstd_set_cfifo_pipeX();         /* The FIFO port is changed without ISEL bit. */
    }

    for( i = USB_0; i < USB_4; i++ )
    {
        USB_RD(CFIFOCTR, buffer);
        if( (uint16_t)(buffer & USB_FRDY) == USB_FRDY )
        {
            return (buffer);
        }
    /* Cautions!!!
     * Depending on the external bus speed of CPU, you may need to wait for 100ns here.
     * For details, please look at the data sheet.   */
    /***** The example of reference. *****/
        /* Wait for About 60ns */
        /* Wait for About 60ns */
        USB_RD(SYSCFG, buffer);
        USB_RD(SYSSTS0, buffer);
    /*************************************/
    }
    return (USB_FIFOERROR);
}   /* eof usb_cstd_set_cfifo() */


/******************************************************************************
Function Name   : usb_cstd_set_cfifo_pipeX
Description     : Changes the specified FIFO port by the specified pipe.
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_cstd_set_cfifo_pipeX(void)
{
    uint16_t    buffer;

    /* CURPIPE=pipeX */
    USB_MDF_PAT(CFIFOSEL, (uint16_t)(USB_RCNT|g_usb_CurrentPipe), (uint16_t)(USB_RCNT|USB_ISEL|USB_CURPIPE));
    do
    {
        USB_RD(CFIFOSEL, buffer);
    }
    while( (buffer & (uint16_t)(USB_ISEL|USB_CURPIPE)) != (uint16_t)(g_usb_CurrentPipe) );
}   /* eof usb_cstd_set_cfifo_pipeX() */


/******************************************************************************
Function Name   : usb_cstd_set_cfifo_pipe0r
Description     : Changes the specified FIFO port by the specified pipe.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_set_cfifo_pipe0r(void)
{
    uint16_t    buffer;

    /* ISEL=0, CURPIPE=0 */
    USB_MDF_PAT(CFIFOSEL, (uint16_t)(USB_RCNT|USB_PIPE0), (uint16_t)(USB_RCNT|USB_ISEL|USB_CURPIPE));
    do
    {
        USB_RD(CFIFOSEL, buffer);
    }
    while( (buffer & (uint16_t)(USB_ISEL|USB_CURPIPE)) != (uint16_t)(USB_PIPE0) );

}   /* eof usb_cstd_set_cfifo_pipe0r() */


/******************************************************************************
Function Name   : usb_cstd_set_cfifo_pipe0w
Description     : Changes the specified FIFO port by the specified pipe.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_set_cfifo_pipe0w(void)
{
    uint16_t    buffer;

    /* ISEL=1, CURPIPE=0 */
    USB_MDF_PAT(CFIFOSEL, (uint16_t)(USB_RCNT|USB_ISEL|USB_PIPE0), (uint16_t)(USB_RCNT|USB_ISEL|USB_CURPIPE));
    do
    {
        USB_RD(CFIFOSEL, buffer);
    }
    while( (buffer & (uint16_t)(USB_ISEL|USB_CURPIPE)) != (uint16_t)(USB_ISEL|USB_PIPE0) );

}   /* eof usb_cstd_set_cfifo_pipe0w() */


/******************************************************************************
Function Name   : usb_cstd_clr_cfifo_pipe0r
Description     : Changes the specified FIFO port by the specified pipe.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_clr_cfifo_pipe0r(void)
{
    usb_cstd_set_cfifo_pipe0r();
    USB_WR(CFIFOCTR, USB_BCLR);
}   /* eof usb_cstd_clr_cfifo_pipe0r() */


/******************************************************************************
Function Name   : usb_cstd_clr_cfifo_pipe0w
Description     : Changes the specified FIFO port by the specified pipe.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_clr_cfifo_pipe0w(void)
{
    usb_cstd_set_cfifo_pipe0w();
    USB_WR(CFIFOCTR, USB_BCLR);
    USB_CLR_STS(BEMPSTS, USB_BIT0);
}   /* eof usb_cstd_clr_cfifo_pipe0w() */


/******************************************************************************
Function Name   : usb_cstd_clr_transaction_counter
Description     : Clear Transaction counter
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_cstd_clr_transaction_counter(void)
{
    /* Check PIPE */
    switch( g_usb_CurrentPipe )
    {
#ifdef  __RX
        case USB_PIPE1:             /* Pipe1 transaction counter register clear & count disable  */
            USB_CLR_PAT(PIPE1TRE, USB_TRENB);
            USB_SET_PAT(PIPE1TRE, USB_TRCLR);
        break;
        case USB_PIPE2:             /* Pipe2 transaction counter register clear & count disable  */
            USB_CLR_PAT(PIPE2TRE, USB_TRENB);
            USB_SET_PAT(PIPE2TRE, USB_TRCLR);
        break;
        case USB_PIPE3:             /* Pipe3 transaction counter register clear & count disable  */
            USB_CLR_PAT(PIPE3TRE, USB_TRENB);
            USB_SET_PAT(PIPE3TRE, USB_TRCLR);
        break;
#endif  /* __RX */
        case USB_PIPE4:             /* Pipe4 transaction counter register clear & count disable  */
            USB_CLR_PAT(PIPE4TRE, USB_TRENB);
            USB_SET_PAT(PIPE4TRE, USB_TRCLR);
        break;
        case USB_PIPE5:             /* Pipe5 transaction counter register clear & count disable  */
            USB_CLR_PAT(PIPE5TRE, USB_TRENB);
            USB_SET_PAT(PIPE5TRE, USB_TRCLR);
        break;
        default:
            /* no processing */
        break;
    }

}   /* eof usb_cstd_clr_transaction_counter() */



/******************************************************************************
Function Name   : usb_cstd_brdy_pipe
Description     : BRDY interrupt
Arguments       : uint16_t bitsts       ; BRDYSTS Register & BRDYENB Register
Return value    : none
******************************************************************************/
void usb_cstd_brdy_pipe(uint16_t bitsts)
{
#ifdef __RX
    uint16_t buffer;
    uint16_t maxps;
    uint16_t set_dtc_brock_cnt;
    uint16_t trans_dtc_block_cnt;

    maxps = usb_gcstd_Dma0Fifo;
#endif  /* __RX */

    for( g_usb_CurrentPipe = USB_MIN_PIPE_NO; g_usb_CurrentPipe <= USB_MAX_PIPE_NO; g_usb_CurrentPipe++ )
    {
        if( (bitsts & USB_BITSET(g_usb_CurrentPipe)) != USB_0 )
        {
            /* Interrupt check */
            USB_CLR_STS(BEMPSTS, USB_BITSET(g_usb_CurrentPipe));
            if( g_usb_LibPipe[g_usb_CurrentPipe] != USB_NULL )
            {
#ifdef __RX
                if( USB_D0DMA == usb_cpu_get_transfer_mode() )
                {

                    /* DMA Transfer request disable */
                    USB_MDF_PAT(D0FIFOSEL, (uint16_t)(0), (uint16_t)(USB_DREQE));

                    /* DMA stop */
                    usb_cpu_d0fifo_stop_dma();

                    /* Changes the FIFO port by the pipe. */
                    usb_cpu_set_d0fifo_pipeX();
                    USB_RD( D0FIFOCTR, buffer );

                    set_dtc_brock_cnt = (uint16_t)((g_usb_LibDataCnt[usb_gcstd_Dma0Pipe] -1)
                        / usb_gcstd_Dma0Fifo) +1;

                    trans_dtc_block_cnt = usb_cpu_get_dtc_block_count();
                    /* Get D0fifo Receive Data Length */
                    usb_gcstd_Dma0Size
                        = (uint32_t)(buffer & USB_DTLN) + (set_dtc_brock_cnt - (trans_dtc_block_cnt + 1)) * maxps;

                    /* Check data count */
                    if( usb_gcstd_Dma0Size == g_usb_LibDataCnt[g_usb_CurrentPipe] )
                    {
                        g_usb_LibDataCnt[g_usb_CurrentPipe] = 0;
                        /* End of data transfer */
                        usb_cstd_data_end( USB_DATA_OK);
                    }
                    else if( usb_gcstd_Dma0Size > g_usb_LibDataCnt[g_usb_CurrentPipe] )
                    {
                        /* D0FIFO access DMA stop */
                        usb_cpu_d0fifo_stop_usb();
                        /* End of data transfer */
                        usb_cstd_data_end( USB_DATA_OVR);
                    }
                    else
                    {
                        /* Changes FIFO port by the pipe. */
                        buffer = usb_cstd_set_cfifo( USB_NO);

                        usb_gcstd_Dma0Size = (uint16_t)(buffer & USB_DTLN);

                        /* D0FIFO access DMA stop */
                        usb_cpu_d0fifo_stop_usb();
                        /* End of data transfer */
                        usb_cstd_data_end( USB_DATA_SHT);
                    }
                    /* Set BCLR */
                    USB_WR(D0FIFOCTR, USB_BCLR);
                }
                else
                {
#endif  /* __RX */

                /* Pipe number to FIFO port select */
                if( usb_cstd_get_pipe_dir() == USB_BUF2FIFO )
                {
                    /* Buffer to FIFO data write */
                    usb_cstd_buf_2_fifo();
                }
                else
                {
                    /* FIFO to Buffer data read */
                    usb_cstd_fifo_2_buf();
                }
#ifdef __RX
                }
#endif  /* __RX */
            }
        }
    }

}   /* eof usb_cstd_brdy_pipe() */


/******************************************************************************
Function Name   : usb_cstd_nrdy_pipe
Description     : Nrdy Pipe interrupt (host only)
Arguments       : uint16_t bitsts       ; NRDYSTS Register & NRDYENB Register
Return value    : none
******************************************************************************/
void usb_cstd_nrdy_pipe(uint16_t bitsts)
{
/* PERI spec                                                            */
/*  transmission pipe                                                   */
/* @5       a) only NRDY                                                */
/* @1       b) NRDY+OVRN    (Isochronous)                               */
/*  Receive pipe                                                        */
/* @5       a) only NRDY                                                */
/* @1       b) NRDY+OVRN    (Isochronous)                               */
/* @2       c) only NRDY    (interval error of isochronous)             */
/* HOST spec                                                            */
/*  transmission pipe                                                   */
/* @1       a) NRDY+OVRN    (Isochronous)                               */
/* @4       b) NRDY+NAK     (Ignore)                                    */
/* @3       c) NRDY+STALL   (Recieve STALL)                             */
/*  Receive pipe                                                        */
/* @1       a) NRDY+OVRN    (Isochronous)                               */
/* @4       b) NRDY+NAK     (Ignore)                                    */
/* @2       c) NRDY         (Ignore of isochronous)                     */
/* @2       d) NRDY         (CRC error of isochrnous)                   */
/* @3       e) NRDY+STALL   (Recieve STALL)                             */

/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_PP == USB_HOST_PP
    uint16_t        buffer;
#endif  /* USB_HOST_PP */

    for( g_usb_CurrentPipe = USB_MIN_PIPE_NO; g_usb_CurrentPipe <= USB_MAX_PIPE_NO; g_usb_CurrentPipe++ )
    {
        if( (bitsts & USB_BITSET(g_usb_CurrentPipe)) != USB_0 )
        {
            /* Interrupt check */
            if( g_usb_LibPipe[g_usb_CurrentPipe] != USB_NULL )
            {
/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_PP == USB_PERI_PP
                /* @5 (do nothing) */
#endif  /* USB_PERI_PP */
/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_PP == USB_HOST_PP
                buffer = usb_cstd_get_pid();
                /* STALL ? */
                if( (buffer & USB_PID_STALL) == USB_PID_STALL )
                {
                    /* @3 (End of data transfer) */
                    usb_cstd_forced_termination(USB_DATA_STALL);
                }
                else
                {
                    /* @4 (Device Ignore X 3 callback (End of data transfer)) */
//@@@   Is connected judgment good in the usb_hstd_check_attach() function ?
                    if( usb_hstd_check_attach() == USB_LNST_ATTACH)
                    {
                        usb_cstd_forced_termination(USB_DATA_ERR);
                    }
                    else
                    {
                        usb_cstd_forced_termination(USB_DATA_DTCH);
                    }
                }
#endif  /* USB_HOST_PP */
            }
        }
    }

}   /* eof usb_cstd_nrdy_pipe() */


/******************************************************************************
Function Name   : usb_cstd_bemp_pipe
Description     : BEMP interrupt
Arguments       : uint16_t bitsts       ; BEMPSTS Register & BEMPENB Register
Return value    : none
******************************************************************************/
void usb_cstd_bemp_pipe(uint16_t bitsts)
{
    uint16_t        buffer;

    for( g_usb_CurrentPipe = USB_MIN_PIPE_NO; g_usb_CurrentPipe <= USB_MAX_PIPE_NO; g_usb_CurrentPipe++ )
    {
        if( (bitsts & USB_BITSET(g_usb_CurrentPipe)) != USB_0 )
        {
            if( g_usb_LibPipe[g_usb_CurrentPipe] != USB_NULL )
            {
                buffer = usb_cstd_get_pid();
                /* MAX packet size error ? */
                if( (buffer & USB_PID_STALL) == USB_PID_STALL )
                {
                    usb_cstd_forced_termination(USB_DATA_STALL);
                }
                else
                {
                    /* Because interrupt pipe can read "0" at this bit, the distinction of BULK/INT is unnecessary. */
                    if( usb_cstd_read_inbufm() != USB_INBUFM )
                    {

#ifdef __RX
                        if( USB_D0DMA == usb_cpu_get_transfer_mode() )
                        {
                            USB_MDF_PAT(D0FIFOSEL, (uint16_t)(0), (uint16_t)(USB_DREQE));

                            /* DMA stop */
                            usb_cpu_d0fifo_stop_dma();
                        }
#endif  /* __RX */

                        /* End of data transfer */
                        usb_cstd_data_end(USB_DATA_OK);
                    }
                }
            }
        }
    }

}   /* eof usb_cstd_bemp_pipe() */


/******************************************************************************
Function Name   : usb_cstd_brdy_enable
Description     : Enable pipe BRDY interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_brdy_enable(void)
{
    USB_SET_PAT(BRDYENB, USB_BITSET(g_usb_CurrentPipe));
}   /* eof usb_cstd_brdy_enable() */


/******************************************************************************
Function Name   : usb_cstd_brdy_disable
Description     : Disable pipe BRDY interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_brdy_disable(void)
{
    USB_CLR_PAT(BRDYENB, USB_BITSET(g_usb_CurrentPipe));
}   /* eof usb_cstd_brdy_disable() */


/******************************************************************************
Function Name   : usb_cstd_bemp_enable
Description     : Enable pipe BEMP interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_bemp_enable(void)
{
    USB_SET_PAT(BEMPENB, USB_BITSET(g_usb_CurrentPipe));
}   /* eof usb_cstd_bemp_enable() */


/******************************************************************************
Function Name   : usb_cstd_bemp_disable
Description     : Disable pipe BEMP interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_bemp_disable(void)
{
    USB_CLR_PAT(BEMPENB, USB_BITSET(g_usb_CurrentPipe));
}   /* eof usb_cstd_bemp_disable() */


/******************************************************************************
Function Name   : usb_cstd_nrdy_enable
Description     : Enable pipe NRDY interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_nrdy_enable(void)
{
/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_PP == USB_PERI_PP
    /*  At the peripheral operation, interrupt is disenabled */
    /*  because handler becomes busy. */
#endif  /* USB_PERI_PP */
/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_PP == USB_HOST_PP
    USB_SET_PAT(NRDYENB, USB_BITSET(g_usb_CurrentPipe));
#endif  /* USB_HOST_PP */

}   /* eof usb_cstd_nrdy_enable() */


/******************************************************************************
Function Name   : usb_cstd_nrdy_disable
Description     : Disable pipe NRDY interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_nrdy_disable(void)
{
    USB_CLR_PAT(NRDYENB, USB_BITSET(g_usb_CurrentPipe));
}   /* eof usb_cstd_nrdy_disable() */


/******************************************************************************
Function Name   : usb_cstd_berne_enable
Description     : Enable BEMP/BRDY/NRDY interrupt
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_berne_enable(void)
{
    USB_SET_PAT(INTENB0, (USB_BEMPE|USB_NRDYE|USB_BRDYE));
}   /* eof usb_cstd_berne_enable() */


/******************************************************************************
Function Name   : usb_cstd_do_sqtgl
Description     : Do pipe SQTGL
Arguments       : none
                : uint16_t toggle       ; SQMON data
Return value    : none
******************************************************************************/
USB_STATIC void usb_cstd_do_sqtgl(uint16_t toggle)
{
    /* Check toggle */
    if( (toggle & USB_SQMON) == USB_SQMON )
    {
        usb_cstd_reg_set_pipexctr(USB_SQSET);
    }
    else
    {
        usb_cstd_reg_set_pipexctr(USB_SQCLR);
    }
}   /* eof usb_cstd_do_sqtgl() */


/******************************************************************************
Function Name   : usb_cstd_get_max_packet_size
Description     : Get max packet size from pipe number
Arguments       : none
Return value    : uint16_t Max Packet Size
******************************************************************************/
USB_STATIC uint16_t usb_cstd_get_max_packet_size(void)
{
    uint16_t    buffer;

    if( g_usb_CurrentPipe == USB_PIPE0 )
    {
        /* Max Packet Size */
        USB_RD(DCPMAXP, buffer);
//      size = (uint16_t)(buffer & USB_MAXP);
    }
    else
    {
        /* Pipe select */
        USB_WR(PIPESEL, g_usb_CurrentPipe);
        USB_RD(PIPEMAXP, buffer);
//      size = (uint16_t)(buffer & USB_MXPS);
    }
    /* The USB_MXPS macro is length of 11bit when the FW was used at USB-ASSP. */
    /* When the DCPMAXP register is read, "0" can be read to bit11-7.  */
    /* When the PIPEMAXP register is read, "0" can be read to bit11-9.  */
    /* Therefore, a correct result can respond even if the mask is done by "USB_MXPS" macro with large bit length. */
    return (uint16_t)(buffer & USB_MXPS);
}   /* eof usb_cstd_get_max_packet_size() */

/******************************************************************************
Function Name   : usb_cstd_get_pipe_dir
Description     : Get PIPE DIR
Arguments       : none
Return value    : uint16_t Pipe direction
******************************************************************************/
uint16_t usb_cstd_get_pipe_dir(void)
{
    uint16_t        buffer;

    /* Pipe select */
    USB_WR(PIPESEL, g_usb_CurrentPipe);
    USB_RD(PIPECFG, buffer);
    return (uint16_t)(buffer & USB_DIRFIELD);
}   /* eof usb_cstd_get_pipe_dir() */


/******************************************************************************
Function Name   : usb_cstd_set_pipe_config
Description     : Clear PIPE Setting
Arguments       : uint16_t *table           ; ep table
Return value    : none
******************************************************************************/
USB_STATIC void usb_cstd_set_pipe_config(uint16_t *table)
{
    uint16_t        field;

    g_usb_CurrentPipe   = (usb_pipe_t)table[USB_0];

    g_usb_LibPipe[g_usb_CurrentPipe] = (usb_utr_t*)USB_NULL;
    field = USB_BITSET(g_usb_CurrentPipe);

    /* Pipe Configuration Disable */
    USB_CLR_PAT(BRDYENB, field);
    USB_CLR_PAT(NRDYENB, field);
    USB_CLR_PAT(BEMPENB, field);

#ifdef __RX
    if( USB_D0DMA == usb_cpu_get_transfer_mode() )
    {
        table[USB_1] |= USB_BFREON;
    }
#endif /* __RX */

    usb_cstd_clr_stall();
    USB_WR(PIPESEL,  g_usb_CurrentPipe);
    USB_WR(PIPECFG,  table[USB_1]);
    USB_WR(PIPEMAXP, table[USB_2]);
#if USB_FUNCSEL_PP == USB_HOST_PP
    USB_WR(PIPEPERI, table[USB_3]);
#endif  /* USB_HOST_PP */

    /* FIFO buffer initialized */
    USB_WR(PIPESEL, USB_PIPE0);
    usb_cstd_reg_set_pipexctr(USB_SQCLR);
    usb_cstd_do_aclrm();

    /* Interrupt status clear */
    USB_CLR_STS(BRDYSTS, field);
    USB_CLR_STS(NRDYSTS, field);
    USB_CLR_STS(BEMPSTS, field);

}   /* eof usb_cstd_set_pipe_config() */


/******************************************************************************
Function Name   : usb_cstd_clr_pipe_config
Description     : Clear pipe configuration register
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_clr_pipe_config(void)
{
    uint16_t    field;
#ifdef __RX
    uint16_t    buf;
    usb_pipe_t      current_pipe_bak;
#endif /* __RX */

    g_usb_LibPipe[g_usb_CurrentPipe] = (usb_utr_t*)USB_NULL;
    field = USB_BITSET(g_usb_CurrentPipe);

    /* PID=NAK & clear STALL */
    usb_cstd_clr_stall();

    /* Pipe Configuration Disable */
    USB_CLR_PAT(BRDYENB, field);
    USB_CLR_PAT(NRDYENB, field);
    USB_CLR_PAT(BEMPENB, field);

    usb_cstd_set_cfifo_pipe0r();

#ifdef __RX
    /* Clear D0FIFO-port */
    USB_RD( D0FIFOSEL, buf );
    if( (buf & USB_CURPIPE) == g_usb_CurrentPipe )
    {
        usb_cpu_d0fifo_stop_dma();
        usb_cpu_d0fifo_stop_usb();

        current_pipe_bak = g_usb_CurrentPipe;
        g_usb_CurrentPipe = USB_PIPE0;
        /* Changes the FIFO port by the pipe. */
        usb_cpu_set_d0fifo_pipeX();
        g_usb_CurrentPipe = current_pipe_bak;

    }
#endif  /* __RX */

    USB_WR(PIPESEL,  g_usb_CurrentPipe);
    USB_WR(PIPECFG,  USB_0);
    USB_WR(PIPEMAXP, USB_0);
#if USB_FUNCSEL_PP == USB_HOST_PP
    USB_WR(PIPEPERI, USB_0);
#endif  /* USB_HOST_PP */

    /* FIFO buffer initialized */
    USB_WR(PIPESEL,  USB_PIPE0);
    usb_cstd_reg_set_pipexctr(USB_SQCLR);
    usb_cstd_do_aclrm();
    usb_cstd_clr_transaction_counter();

    /* Interrupt status clear */
    USB_CLR_STS(BRDYSTS, field);
    USB_CLR_STS(NRDYSTS, field);
    USB_CLR_STS(BEMPSTS, field);
}   /* eof usb_cstd_clr_pipe_config() */


/******************************************************************************
Function Name   : usb_cstd_reg_clr_pipexctr
Description     : PIPE control register clear
Arguments       : uint16_t field          : clear pipe position
Return value    : none
******************************************************************************/
USB_STATIC void usb_cstd_reg_clr_pipexctr(uint16_t field)
{
    /* Select PIPE */
    switch( g_usb_CurrentPipe )
    {
        /* PIPE control register clear */
        case USB_PIPE0:
            USB_CLR_PAT(DCPCTR,   field);
        break;
#ifdef  __RX
        case USB_PIPE1:
            USB_CLR_PAT(PIPE1CTR, field);
        break;
        case USB_PIPE2:
            USB_CLR_PAT(PIPE2CTR, field);
        break;
        case USB_PIPE3:
            USB_CLR_PAT(PIPE3CTR, field);
        break;
#endif  /* __RX */
        case USB_PIPE4:
            USB_CLR_PAT(PIPE4CTR, field);
        break;
        case USB_PIPE5:
            USB_CLR_PAT(PIPE5CTR, field);
        break;
        case USB_PIPE6:
            USB_CLR_PAT(PIPE6CTR, field);
        break;
        case USB_PIPE7:
            USB_CLR_PAT(PIPE7CTR, field);
        break;
#ifdef  __RX
        case USB_PIPE8:
            USB_CLR_PAT(PIPE8CTR, field);
        break;
        case USB_PIPE9:
            USB_CLR_PAT(PIPE9CTR, field);
        break;
#endif  /* __RX */
        default:
            /* no processing */
        break;
    }
}   /* eof usb_cstd_reg_clr_pipexctr() */


/******************************************************************************
Function Name   : usb_cstd_reg_set_pipexctr
Description     : PIPE control register set
Arguments       : uint16_t field          : clear pipe position
Return value    : none
******************************************************************************/
void usb_cstd_reg_set_pipexctr(uint16_t field)
{
    switch( g_usb_CurrentPipe )
    {
        /* PIPE control register set */
        case USB_PIPE0:
            USB_SET_PAT(DCPCTR,   field);
        break;
#ifdef  __RX
        case USB_PIPE1:
            USB_SET_PAT(PIPE1CTR, field);
        break;
        case USB_PIPE2:
            USB_SET_PAT(PIPE2CTR, field);
        break;
        case USB_PIPE3:
            USB_SET_PAT(PIPE3CTR, field);
        break;
#endif  /* __RX */
        case USB_PIPE4:
            USB_SET_PAT(PIPE4CTR, field);
        break;
        case USB_PIPE5:
            USB_SET_PAT(PIPE5CTR, field);
        break;
        case USB_PIPE6:
            USB_SET_PAT(PIPE6CTR, field);
        break;
        case USB_PIPE7:
            USB_SET_PAT(PIPE7CTR, field);
        break;
#ifdef  __RX
        case USB_PIPE8:
            USB_SET_PAT(PIPE8CTR, field);
        break;
        case USB_PIPE9:
            USB_SET_PAT(PIPE9CTR, field);
        break;
#endif  /* __RX */
        default:
            /* no processing */
        break;
    }

}   /* eof usb_cstd_reg_set_pipexctr() */


/******************************************************************************
Function Name   : usb_cstd_reg_read_pipexctr
Description     : PIPE control register read
Arguments       : none
Return value    : uint16_t                : PIPE Control Reg Read
******************************************************************************/
uint16_t usb_cstd_reg_read_pipexctr(void)
{
    uint16_t    buf;

    switch( g_usb_CurrentPipe )
    {
        /* PIPE control register read */
        case USB_PIPE0:
            USB_RD(DCPCTR,   buf);
        break;
#ifdef  __RX
        case USB_PIPE1:
            USB_RD(PIPE1CTR, buf);
        break;
        case USB_PIPE2:
            USB_RD(PIPE2CTR, buf);
        break;
        case USB_PIPE3:
            USB_RD(PIPE3CTR, buf);
        break;
#endif  /* __RX */
        case USB_PIPE4:
            USB_RD(PIPE4CTR, buf);
        break;
        case USB_PIPE5:
            USB_RD(PIPE5CTR, buf);
        break;
        case USB_PIPE6:
            USB_RD(PIPE6CTR, buf);
        break;
        case USB_PIPE7:
            USB_RD(PIPE7CTR, buf);
        break;
#ifdef  __RX
        case USB_PIPE8:
            USB_RD(PIPE8CTR, buf);
        break;
        case USB_PIPE9:
            USB_RD(PIPE9CTR, buf);
        break;
#endif  /* __RX */
        default:
            buf = 0;
        break;
    }

    return buf;
}   /* eof usb_cstd_reg_read_pipexctr() */


/******************************************************************************
Function Name   : usb_cstd_read_inbufm
Description     : Read pipe INBUFM
Arguments       : none
Return value    : uint16_t USB_INBUFM bit status
******************************************************************************/
USB_STATIC uint16_t usb_cstd_read_inbufm(void)
{
    uint16_t    buf;

    /* PIPE control register read */
    buf = usb_cstd_reg_read_pipexctr();
    return (uint16_t)(buf & USB_INBUFM);
}   /* eof usb_cstd_read_inbufm() */


/******************************************************************************
Function Name   : usb_cstd_do_aclrm
Description     : Do pipe ACLRM
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_cstd_do_aclrm(void)
{
    /* PIPE control register set */
    usb_cstd_reg_set_pipexctr(USB_ACLRM);
    usb_cstd_reg_clr_pipexctr(USB_ACLRM);
}   /* eof usb_cstd_do_aclrm() */


/******************************************************************************
Function Name   : usb_cstd_read_frmnum
Description     : Read FRMNUM
Arguments       : none
Return value    : USB_FRMNUM register
******************************************************************************/
uint16_t usb_cstd_read_frmnum(void)
{
    uint16_t    framenum;

    USB_RD(FRMNUM, framenum);
    return  framenum;
}   /* eof usb_cstd_read_frmnum() */


/******************************************************************************
Function Name   : usb_cstd_set_nak
Description     : Set pipe PID_NAK
Arguments       : none
Return value    : USB_YES, USB_NO
******************************************************************************/
uint16_t usb_cstd_set_nak(void)
{
    uint16_t        buf, n;

    /* Set NAK */
    usb_cstd_reg_clr_pipexctr(USB_PID_BUF);

    /* The state of PBUSY continues while transmitting the packet when it is a detach. */
    /* 1ms comes off when leaving because the packet duration might not exceed 1ms.  */
    /* Whether it is PBUSY release or 1ms passage can be judged. */
    for( n = USB_0; n < 0xFFFFu; ++n )
    {
        /* PIPE control register read */
        buf = usb_cstd_reg_read_pipexctr();
        if( (uint16_t)(buf & USB_PBUSY) == USB_0 )
        {
            return  USB_YES;
        }
    }
    return  USB_NO;
}   /* eof usb_cstd_set_nak() */


/******************************************************************************
Function Name   : usb_cstd_clr_stall
Description     : Clear pipe STALL
Arguments       : none
Return value    : none
Note            : PID is set to NAK.
******************************************************************************/
void usb_cstd_clr_stall(void)
{
    /* Set NAK & clear STALL */
    usb_cstd_set_nak();
    usb_cstd_reg_clr_pipexctr(USB_PID_STALL);
}   /* eof usb_cstd_clr_stall() */


/******************************************************************************
Function Name   : usb_cstd_get_pid
Description     : Get Pipe PID from pipe number
Arguments       : none
Return value    : uint16_t PID bit status
******************************************************************************/
uint16_t usb_cstd_get_pid(void)
{
    uint16_t    buf;

    /* PIPE control register read */
    buf = usb_cstd_reg_read_pipexctr();
    return (uint16_t)(buf & USB_PID);
}   /* eof usb_cstd_get_pid() */


/******************************************************************************
Function Name   : usb_cstd_dummy_function
Description     : dummy function
Arguments       : uint16_t data1            : not use
                : uint16_t data2            : not use
Return value    : none
******************************************************************************/
void usb_cstd_dummy_function(uint16_t data1, uint16_t data2)
{
}   /* eof usb_cstd_dummy_function() */


/******************************************************************************
Function Name   : usb_cstd_interrupt_disable
Description     : USB interrupt disable
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_interrupt_disable(void)
{
    /* Interrupt Disable(BRDY,NRDY,USB_BEMP) */
    USB_WR(INTENB0,USB_0);
    USB_WR(BRDYENB,USB_0);
    USB_WR(NRDYENB,USB_0);
    USB_WR(BEMPENB,USB_0);

    /* Interrupt status clear(USB_BRDY,NRDY,USB_BEMP) */
    USB_WR(INTSTS0,USB_0);
    USB_WR(BRDYSTS,USB_0);
    USB_WR(NRDYSTS,USB_0);
    USB_WR(BEMPSTS,USB_0);
}   /* eof usb_cstd_interrupt_disable() */


/******************************************************************************
Function Name   : usb_cstd_sw_reset
Description     : Software reset
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_sw_reset(void)
{
    USB_CLR_PAT(SYSCFG, USB_USBE);
    USB_SET_PAT(SYSCFG, USB_USBE);
}   /* eof usb_cstd_sw_reset() */


/******************************************************************************
Function Name   : usb_cstd_hw_stop
Description     : USB H/W Stop
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_hw_stop(void)
{
    /* USB Reset, SCLK Disable */
    USB_CLR_PAT(SYSCFG, USB_USBE);
    USB_WR(SYSCFG, USB_0);
}   /* eof usb_cstd_hw_stop() */


/******************************************************************************
Function Name   : usb_cstd_hw_start
Description     : Start Oscillation: H/W reset
                : Reset start
                : Set bus interface register after started clock
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_hw_start(void)
{
    USB_WR(SYSCFG, USB_SCKE);
#ifdef __RX
#ifdef __BIG
    /* Big-endian */
    USB_WR(CFIFOSEL, (USB_MBW_16|USB_FIFO_BIG));
    USB_WR(D0FIFOSEL, (USB_MBW_16|USB_FIFO_BIG));
#else
    /* Little-endian */
    USB_WR(CFIFOSEL, (USB_MBW_16|USB_FIFO_LITTLE));
    USB_WR(D0FIFOSEL, (USB_MBW_16|USB_FIFO_LITTLE));
#endif  
#else   
    USB_WR(CFIFOSEL, (USB_MBW_8|USB_FIFO_LITTLE));  
#endif
}   /* eof usb_cstd_hw_start() */


/******************************************************************************
Function Name   : usb_cstd_set_pipe_registration
Description     : Set pipe configuration register
Arguments       : uint16_t *table       : pipe information table
                : uint16_t command      : control command
Return value    : none
******************************************************************************/
void usb_cstd_set_pipe_registration(uint16_t *table, uint16_t command)
{
    usb_cstd_set_cfifo_pipe0r();

    for( ; *table != USB_PDTBLEND; table += USB_EPL )
    {
        /* Pipe number */
        g_usb_CurrentPipe = (usb_pipe_t)(*table & USB_CURPIPE);
        usb_cstd_clr_pipe_config();
        if (command == USB_YES)
        {
            usb_cstd_set_pipe_config(table);
        }
    }
}   /* eof usb_cstd_set_pipe_registration() */


/******************************************************************************
End of file
******************************************************************************/
