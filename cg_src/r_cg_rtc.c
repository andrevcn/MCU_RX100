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
* File Name    : r_cg_rtc.c
* Version      : Applilet4 for RX111 V1.00.00.05 [06 Jun 2013]
* Device(s)    : R5F51115AxFL
* Tool-Chain   : IAR Systems icc78k0r
* Description  : This file implements device driver for RTC module.
* Creation Date: 6/18/2014
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
#include "r_cg_rtc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_RTC_Create
* Description  : This function initializes the RTC module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Create(void)
{
    uint32_t w_count;

    /* Disable ALM, PRD and CUP interrupts */
    IEN(RTC,ALM) = 0U;
    IEN(RTC,PRD) = 0U;
    IEN(RTC,CUP) = 0U;

    /* Set sub-clock oscillator */
    RTC.RCR3.BYTE = _04_RTC_SUBCLOCK_DRIVE_LOW | _01_RTC_SUBCLOCK_ENABLE;

    for (w_count = 0U; w_count <= _00000370_RTC_RTCSCLK_STABLE_CYCLE; w_count++)
    {
        asm ("nop");
    }

    /* Stop all counters */
    RTC.RCR2.BIT.START = 0U;
    
    while (RTC.RCR2.BIT.START != 0U);

    /* Select count mode */
    RTC.RCR2.BIT.CNTMD = 0U;
    
    while (RTC.RCR2.BIT.CNTMD != 0U);

    /* Execute RTC software reset */
    RTC.RCR2.BIT.RESET = 1U;
    
    while (RTC.RCR2.BIT.RESET != 0U);
    
    /* Set control registers */
    RTC.RCR1.BYTE = _00_RTC_ALARM_INT_DISABLE | _00_RTC_CARRY_INT_DISABLE | _00_RTC_PERIOD_INT_DISABLE | 
                    _00_RTC_PERIODIC_INT_PERIOD_DISABLE;
    RTC.RCR2.BYTE = _40_RTC_HOUR_MODE_24;

    /* Set time *///retirado marcos
    //RTC.RSECCNT.BYTE = _15_RTC_COUNT_SECOND_VALUE;
    //RTC.RMINCNT.BYTE = _15_RTC_COUNT_MINUTE_VALUE;
    //RTC.RHRCNT.BYTE = _15_RTC_COUNT_HOUR_VALUE;
    //RTC.RWKCNT.BYTE = _03_RTC_COUNT_WEEK_VALUE;
    //RTC.RDAYCNT.BYTE = _18_RTC_COUNT_DAY_VALUE;
    //RTC.RMONCNT.BYTE = _06_RTC_COUNT_MONTH_VALUE;
    //RTC.RYRCNT.WORD = _0014_RTC_COUNT_YEAR_VALUE;

}
/***********************************************************************************************************************
* Function Name: R_RTC_Start
* Description  : This function starts RTC counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Start(void)
{
    /* Set the START bit to 1 */
    RTC.RCR2.BIT.START = 1U;
    
    while (RTC.RCR2.BIT.START != 1U);
}
/***********************************************************************************************************************
* Function Name: R_RTC_Stop
* Description  : This function stops RTC counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Stop(void)
{
    /* Stop all counters */
    RTC.RCR2.BIT.START = 0U;
    
    while (RTC.RCR2.BIT.START != 0U);
}
/***********************************************************************************************************************
* Function Name: R_RTC_Restart
* Description  : This function restarts RTC counter.
* Arguments    : counter_write_val -
*                    counter write value
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Restart(rtc_calendarcounter_value_t counter_write_val)
{
    /* Disable ALM, PRD and CUP interrupts */
    IEN(RTC,ALM) = 0U;
    IEN(RTC,PRD) = 0U;
    IEN(RTC,CUP) = 0U;

    /* Stop all counters */
    RTC.RCR2.BIT.START = 0U;
    
    while (RTC.RCR2.BIT.START != 0U);

    /* Execute RTC software reset */
    RTC.RCR2.BIT.RESET = 1U;
    
    while (RTC.RCR2.BIT.RESET != 0U);
    
    /* Set control registers */
    RTC.RCR2.BYTE = _40_RTC_HOUR_MODE_24;

    /* Set counter values */
    RTC.RSECCNT.BYTE = counter_write_val.rseccnt;
    RTC.RMINCNT.BYTE = counter_write_val.rmincnt;
    RTC.RHRCNT.BYTE = counter_write_val.rhrcnt;
    RTC.RWKCNT.BYTE = counter_write_val.rwkcnt;
    RTC.RDAYCNT.BYTE = counter_write_val.rdaycnt;
    RTC.RMONCNT.BYTE = counter_write_val.rmoncnt;
    RTC.RYRCNT.WORD = counter_write_val.ryrcnt;

    /* Set the START bit to 1 */
    RTC.RCR2.BIT.START = 1U;
    
    while (RTC.RCR2.BIT.START != 1U);
}
/***********************************************************************************************************************
* Function Name: R_RTC_Get_CalendarCounterValue
* Description  : This function get RTC calendar counter value.
* Arguments    : counter_read_val -
*                    counter read pointer
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Get_CalendarCounterValue(rtc_calendarcounter_value_t * const counter_read_val)
{
    IR(RTC,CUP) = 0U;
    RTC.RCR1.BYTE |= _02_RTC_CARRY_INT_ENABLE;

    do
    {
        counter_read_val->rseccnt = RTC.RSECCNT.BIT.SEC10*10 +  RTC.RSECCNT.BIT.SEC1;
        counter_read_val->rmincnt = RTC.RMINCNT.BIT.MIN10*10 + RTC.RMINCNT.BIT.MIN1;
        counter_read_val->rhrcnt = RTC.RHRCNT.BIT.HR10*10 + RTC.RHRCNT.BIT.HR1;
        counter_read_val->rwkcnt = RTC.RWKCNT.BYTE;
        counter_read_val->rdaycnt = RTC.RDAYCNT.BYTE;
        counter_read_val->rmoncnt = RTC.RMONCNT.BYTE;
        counter_read_val->ryrcnt = RTC.RYRCNT.WORD;
    } while (IR(RTC,CUP) == 1U);

    RTC.RCR1.BYTE &= ~(_02_RTC_CARRY_INT_ENABLE);
}
/***********************************************************************************************************************
* Function Name: R_RTC_Set_CalendarCounterValue
* Description  : This function set RTC calendar counter value.
* Arguments    : counter_write_val -
*                    counter write value
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Set_CalendarCounterValue(rtc_calendarcounter_value_t counter_write_val)
{
    /* Stop all counters */
extern uint8_t         AjusteHora;
extern uint8_t         AjusteMinuto;  
  
  RTC.RCR2.BIT.START = 0U;
    
    while (RTC.RCR2.BIT.START != 0U);

    /* Execute RTC software reset */
    RTC.RCR2.BIT.RESET = 1U;
    
    while (RTC.RCR2.BIT.RESET != 0U);
    
    /* Set control registers */
    RTC.RCR2.BYTE = _40_RTC_HOUR_MODE_24;

    /* Set counter values */
  //  RTC.RSECCNT.BYTE = //counter_write_val.rseccnt;
  //  RTC.RMINCNT.BYTE = //counter_write_val.rmincnt;
  //  RTC.RHRCNT.BYTE = //counter_write_val.rhrcnt;
  //  RTC.RWKCNT.BYTE = //counter_write_val.rwkcnt;
  //  RTC.RDAYCNT.BYTE =// counter_write_val.rdaycnt;
  //  RTC.RMONCNT.BYTE = //counter_write_val.rmoncnt;
  //  RTC.RYRCNT.WORD = //counter_write_val.ryrcnt;
      
    RTC.RSECCNT.BYTE = _15_RTC_COUNT_SECOND_VALUE;
    //RTC.RMINCNT.BIT.MIN1 = 1;//AjusteMinuto%10;//_15_RTC_COUNT_MINUTE_VALUE;
    //RTC.RMINCNT.BIT.MIN10 = 1;//AjusteMinuto/10;
    //RTC.RHRCNT.BIT.HR1 = 1;//AjusteHora%10;
    //RTC.RHRCNT.BIT.HR10 = 1;//AjusteHora/10;

    unsigned char bin;
    
    bin = (AjusteHora/10)*16;
    bin += AjusteHora%10; 
    RTC.RHRCNT.BYTE = bin;
    bin = (AjusteMinuto/10)*16;
    bin += AjusteMinuto%10;
    RTC.RMINCNT.BYTE = bin;
    RTC.RWKCNT.BYTE = _03_RTC_COUNT_WEEK_VALUE;
    RTC.RDAYCNT.BYTE = _18_RTC_COUNT_DAY_VALUE;
    RTC.RMONCNT.BYTE = _06_RTC_COUNT_MONTH_VALUE;
    RTC.RYRCNT.WORD = _0014_RTC_COUNT_YEAR_VALUE;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
