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
* File Name    : r_cg_s12adb.c
* Version      : Applilet4 for RX111 V1.00.00.05 [06 Jun 2013]
* Device(s)    : R5F51115AxFL
* Tool-Chain   : IAR Systems icc78k0r
* Description  : This file implements device driver for S12ADb module.
* Creation Date: 6/9/2014
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
#include "r_cg_s12adb.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */


/***********************************************************************************************************************
* Function Name: R_S12ADb_Create
* Description  : This function initializes the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_S12AD_Create(void)
{
    MSTP(S12AD) = 0U;                   /* Cancel S12AD module stop state */
    S12AD.ADCSR.BIT.ADIE = 0U;          /* Disable S12ADI0 interrupt */
    IR(S12AD,S12ADI0) = 0U;             /* Clear S12ADI0 interrupt flag */

    S12AD.ADCSR.WORD |= _0000_AD_GBADI_DISABLE |
						_0000_AD_DBLTRIGGER_DISABLE |
						_0000_AD_INTERNAL_TRIGGER |
                        _0400_AD_HIGH_SPEED_CONVERSION |
						_1000_AD_SCAN_END_INTERRUPT_ENABLE |
                        _0000_AD_SINGLE_SCAN_MODE;

    S12AD.ADCER.WORD = _0020_AD_AUTO_CLEARING_ENABLE | _0000_AD_RIGHT_ALIGNMENT;

    /* Set S12ADI0 priority */
    IPR(S12AD,S12ADI0) = _0F_AD_PRIORITY_LEVEL15;

    /* Set channels */
    S12AD.ADANSA.WORD = _0800_AD_CHANNEL_SELECT_A;
    S12AD.ADANSB.WORD = _0000_AD_CHANNEL_SELECT_B;
    S12AD.ADADS.WORD = _0000_AD_VALUE_ADDITION_SELECT;
    S12AD.ADADC.BYTE = _00_AD_1_TIME_CONVERSION;

    S12AD.ADEXICR.WORD = _0002_AD_IREF_ADDITION_ENABLE |
						 _0000_AD_TEMPERATURE_SENSOR_DISABLE |
                         _0000_AD_IREF_CONVERSION_DISABLE;

    S12AD.ADSSTRL = _14_AD_SAMPLING_STATE_L;
    MPC.PJ6PFS.BYTE = _00_AD_REF_HIGH_AVCC0;
    MPC.PJ7PFS.BYTE = _00_AD_REF_LOW_AVSS0;

    /* Set AN011 pin */
    MPC.PE3PFS.BYTE = 0x80U;
    PORTE.PMR.BYTE &= 0xF7U;
    PORTE.PCR.BYTE &= 0xF7U;
    PORTE.PDR.BYTE &= 0xF7U;
}

/***********************************************************************************************************************
* Function Name: R_S12ADb_Start
* Description  : This function starts the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_S12AD_Start(void)
{
    IR(S12AD,S12ADI0) = 0U;     /* Clear S12ADI0 interrupt flag */
    IEN(S12AD,S12ADI0) = 1U;    /* Enable S12ADI0 interrupt */
    S12AD.ADCSR.BIT.ADST = 1U;  /* Enable AD conversion */
}

/***********************************************************************************************************************
* Function Name: R_S12ADb_Stop
* Description  : This function stops the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_S12AD_Stop(void)
{
    S12AD.ADCSR.BIT.ADST = 0U;  /* Disable AD conversion */
    IEN(S12AD,S12ADI0) = 0U;    /* Disable S12ADI0 interrupt */
    IR(S12AD,S12ADI0) = 0U;     /* Clear S12ADI0 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_S12ADb_Get_ValueResult
* Description  : This function gets result from the AD converter.
* Arguments    : channel -
*                    channel of data register to be read
*                buffer -
*                    buffer pointer
* Return Value : None
***********************************************************************************************************************/
void R_S12AD_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer)
{
    if (channel == ADCHANNEL0)
    {
        *buffer = (uint16_t)(S12AD.ADDR0);
    }
    else if (channel == ADCHANNEL1)
    {
        *buffer = (uint16_t)(S12AD.ADDR1);
    }
    else if (channel == ADCHANNEL2)
    {
        *buffer = (uint16_t)(S12AD.ADDR2);
    }
    else if (channel == ADCHANNEL3)
    {
        *buffer = (uint16_t)(S12AD.ADDR3);
    }
    else if (channel == ADCHANNEL4)
    {
        *buffer = (uint16_t)(S12AD.ADDR4);
    }
    else if (channel == ADCHANNEL6)
    {
        *buffer = (uint16_t)(S12AD.ADDR6);
    }
    else if (channel == ADCHANNEL8)
    {
        *buffer = (uint16_t)(S12AD.ADDR8);
    }
    else if (channel == ADCHANNEL9)
    {
        *buffer = (uint16_t)(S12AD.ADDR9);
    }
    else if (channel == ADCHANNEL10)
    {
        *buffer = (uint16_t)(S12AD.ADDR10);
    }
    else if (channel == ADCHANNEL11)
    {
        *buffer = (uint16_t)(S12AD.ADDR11);
    }
    else if (channel == ADCHANNEL12)
    {
        *buffer = (uint16_t)(S12AD.ADDR12);
    }
    else if (channel == ADCHANNEL13)
    {
        *buffer = (uint16_t)(S12AD.ADDR13);
    }
    else if (channel == ADCHANNEL14)
    {
        *buffer = (uint16_t)(S12AD.ADDR14);
    }
    else if (channel == ADCHANNEL15)
    {
        *buffer = (uint16_t)(S12AD.ADDR15);
    }
    else if (channel == ADTEMPSENSOR)
    {
        *buffer = (uint16_t)(S12AD.ADTSDR);
    }
    else if (channel == ADINTERREFVOLT)
    {
        *buffer = (uint16_t)(S12AD.ADOCDR);
    }
    else if (channel == ADDATADUPLICATION)
    {
        *buffer = (uint16_t)(S12AD.ADDBLDR);
    }
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
