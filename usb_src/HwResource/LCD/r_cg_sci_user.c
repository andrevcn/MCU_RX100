/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING 
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT 
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR 
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE 
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software 
* and to discontinue the availability of this software.  By using this software, 
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_sci_user.c
* Version      : Applilet4 for RX111 V1.00.00.05 [06 Jun 2013]
* Device(s)    : R5F51115AxFM
* Tool-Chain   : CCRX
* Description  : This file implements device driver for SCI module.
* Creation Date: 13/06/2013
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_sci.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern uint8_t * gp_sci5_tx_address;         /* SCI5 send buffer address */
extern uint16_t  g_sci5_tx_count;            /* SCI5 send data number */
/* Start user code for global. Do not edit comment generated here */

/* The functions in this file are generated using "Application Leading Tool (Applilet)" for e2 studio.
 * Warnings exist for interrupt handlers. */

uint8_t sci5_tx_is_busy(void);

volatile uint8_t g_sci5_tx_in_process = 0;

static void r_sci5_callback_transmitend(void);

/* The id received from the master in the LIN receive interrupt handler */
volatile uint8_t g_id;

/* Flag set in the LIN receive interrupt handler to show id has been received*/
volatile uint8_t g_id_received = 0;

/* Flag to indicate when data sent from the sci12 UART */
volatile uint8_t g_tx12_done = 0;

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_sci5_transmit_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#if FAST_INTERRUPT_VECTOR == VECT_SCI5_TXI5
#pragma interrupt r_sci5_transmit_interrupt(vect=VECT(SCI5,TXI5),fint)
#else
#pragma interrupt r_sci5_transmit_interrupt(vect=VECT(SCI5,TXI5))
#endif
void r_sci5_transmit_interrupt(void)
{
    if(g_sci5_tx_count > 0U)
    {
        SCI5.TDR = *gp_sci5_tx_address;
        gp_sci5_tx_address++;
        g_sci5_tx_count--;
    } 
    else 
    {
        SCI5.SCR.BIT.TIE = 0U;
        SCI5.SCR.BIT.TEIE = 1U;
    }
}
/***********************************************************************************************************************
* Function Name: r_sci5_transmitend_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#if FAST_INTERRUPT_VECTOR == VECT_SCI5_TEI5
#pragma interrupt r_sci5_transmitend_interrupt(vect=VECT(SCI5,TEI5),fint)
#else
#pragma interrupt r_sci5_transmitend_interrupt(vect=VECT(SCI5,TEI5))
#endif
void r_sci5_transmitend_interrupt(void)
{
    SCI5.SCR.BIT.TIE = 0U;
    SCI5.SCR.BIT.TE = 0U;
    SCI5.SCR.BIT.TEIE = 0U;

    r_sci5_callback_transmitend();
}
/***********************************************************************************************************************
* Function Name: r_sci5_callback_transmitend
* Description  : This function is a callback function when SCI5 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_sci5_callback_transmitend(void)
{
    /* Start user code. Do not edit comment generated here */
     g_sci5_tx_in_process = 0U;
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/***********************************************************************************************************************
* Function Name: sendSPI5
* Description  : This function sends SPI5 data to slave device. Adds flagging around R_SCI5_SPI_Master_Send
* Arguments    : tx_buf -
*                    transfer buffer pointer (Not used when transmit data handled by DTC)
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
void sendSPI5 (uint8_t * const tx_buf, uint16_t const tx_num)
{
    g_sci5_tx_in_process = 1U;
    R_SCI5_SPI_Master_Send(tx_buf, tx_num);
}
/******************************************************************************
* End of function sendSPI5
******************************************************************************/

/******************************************************************************
* Function Name : sci5_tx_is_busy
* Description   : reports if sci5 is transmitting
* Argument      : none
* Return value  : None
******************************************************************************/
uint8_t sci5_tx_is_busy (void)
{
    return (g_sci5_tx_in_process);
}
/******************************************************************************
* End of function sci5_tx_is_busy
******************************************************************************/
/* End user code. Do not edit comment generated here */
