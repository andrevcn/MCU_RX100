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
* File Name    : r_cg_lpc.c
* Version      : Applilet4 for RX111 V1.00.00.05 [06 Jun 2013]
* Device(s)    : R5F51115AxFM
* Tool-Chain   : IAR Systems icc78k0r
* Description  : This file implements device driver for LPC module.
* Creation Date: 08.10.2013
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
#include "r_cg_lpc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
uint8_t g_brk_command;
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_LPC_Create
* Description  : This function initializes the LPC module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LPC_Create(void)
{
    /* Set sub operating power control */
    SYSTEM.SOPCCR.BIT.SOPCM = _00_LPC_LOW_SPEED_MODE_DISABLED;

    while (1U == SYSTEM.SOPCCR.BIT.SOPCMTSF);

    /* Set operating power control */
    SYSTEM.OPCCR.BIT.OPCM = _00_LPC_HIGH_SPEED_MODE;

    while (1U == SYSTEM.OPCCR.BIT.OPCMTSF);

    /* Set sleep mode return clock source */
    SYSTEM.RSTCKCR.BYTE = _00_LPC_RETURN_DISABLED;
}

/***********************************************************************************************************************
* Function Name: R_LPC_AllModuleClockStop
* Description  : This function stops all module clock.
* Arguments    : None
* Return Value : status -
*                    MD_OK
***********************************************************************************************************************/
MD_STATUS R_LPC_AllModuleClockStop(void)
{
    MD_STATUS status = MD_OK;

    uint16_t protect_dummy = (uint16_t)(SYSTEM.PRCR.WORD & 0x000BU);
    uint32_t mstpcra_dummy = SYSTEM.MSTPCRA.LONG;
    uint32_t mstpcrb_dummy = SYSTEM.MSTPCRB.LONG;
    uint32_t mstpcrc_dummy = SYSTEM.MSTPCRC.LONG;

    /* Disable protect bit */
    SYSTEM.PRCR.WORD = 0xA503U;

    /* Set module stop for MTU, CMT, S12ADb, DA and DTC */
    SYSTEM.MSTPCRA.LONG = 0xFFFFFFFFU;

    /* Set module stop for SCI, DOC, ELC, RSPI, USB, RIIC and CRC */
    SYSTEM.MSTPCRB.LONG = 0xFFFFFFFFU;

    /* Set module stop for RAM and CRC. (DSLPE not set) */
    SYSTEM.MSTPCRC.LONG = 0x7FFFFFFFU;

    /* Select the command for the BRK interrupt routine */
    g_brk_command = BRK_ALL_MODULE_CLOCK_STOP;

    /* Trigger MCU power down by calling the brk function */
    brk();

    /* Wait for the CPU to re-start */
    while (g_brk_command == BRK_ALL_MODULE_CLOCK_STOP);

    /* Restore the modules that were enabled before stopping */
    SYSTEM.MSTPCRA.LONG = mstpcra_dummy;
    SYSTEM.MSTPCRB.LONG = mstpcrb_dummy;
    SYSTEM.MSTPCRC.LONG = mstpcrc_dummy;

    /* Restore the previous state of the protect register */
    SYSTEM.PRCR.WORD = (uint16_t)(0xA500U | protect_dummy);

    return status;
}

/***********************************************************************************************************************
* Function Name: R_LPC_Sleep
* Description  : This function enables sleep mode.
* Arguments    : None
* Return Value : status -
*                    MD_OK or MD_ERROR1
***********************************************************************************************************************/
MD_STATUS R_LPC_Sleep(void)
{
    MD_STATUS status = MD_OK;

    uint16_t protect_dummy = (uint16_t)(SYSTEM.PRCR.WORD & 0x000BU);
    
    /* Disable protect bit */
    SYSTEM.PRCR.WORD = 0xA503U;
    
    /* When RSTCKEN is enable, only possible clock source is LOCO */
    if ((SYSTEM.RSTCKCR.BIT.RSTCKEN == 1U) && (SYSTEM.SCKCR3.BIT.CKSEL != 0U))
    {
        status = MD_ERROR1;
    }

    /* Select the command for the BRK interrupt routine */
    g_brk_command = BRK_SLEEP;

    /* Trigger MCU power down by calling the brk function */
    brk();

    /* Wait for the CPU to re-start */
    while (g_brk_command == BRK_SLEEP);

    /* Restore the previous state of the protect register */
    SYSTEM.PRCR.WORD = (uint16_t)(0xA500U | protect_dummy);
    
    return status;
}

/***********************************************************************************************************************
* Function Name: R_LPC_DeepSleep
* Description  : This function enables deep sleep mode.
* Arguments    : None
* Return Value : status -
*                    MD_OK
***********************************************************************************************************************/
MD_STATUS R_LPC_DeepSleep(void)
{
    MD_STATUS status = MD_OK;

    uint16_t protect_dummy = (uint16_t)(SYSTEM.PRCR.WORD & 0x000BU);
    
    /* Save the current state of DTC */
    uint8_t dummy_mstpa28 = (uint8_t)(SYSTEM.MSTPCRA.BIT.MSTPA28 >> 28U);
    uint8_t dummy_dtcst = DTC.DTCST.BIT.DTCST;

    /* Disable protect bit */
    SYSTEM.PRCR.WORD = 0xA503U;
    
    /* Set DTC module to stop state */
    DTC.DTCST.BIT.DTCST = 0U;
    SYSTEM.MSTPCRA.BIT.MSTPA28 = 1U;

    /* Select the command for the BRK interrupt routine */
    g_brk_command = BRK_DEEP_SLEEP;

    /* Trigger MCU power down by calling the brk function */
    brk();

    /* Wait for the CPU to re-start */
    while (g_brk_command == BRK_DEEP_SLEEP);

    /* Restore the state of DTC if necessary */
    if (0U == dummy_mstpa28)
    {
        SYSTEM.MSTPCRA.BIT.MSTPA28 = 0U;
        DTC.DTCST.BIT.DTCST = dummy_dtcst;
    }
    
    /* Restore the previous state of the protect register */
    SYSTEM.PRCR.WORD = (uint16_t)(0xA500U | protect_dummy);

    return status;
}

/***********************************************************************************************************************
* Function Name: R_LPC_SoftwareStandby
* Description  : This function enables software standby mode.
* Arguments    : None
* Return Value : status -
*                    MD_OK or MD_ERROR1
***********************************************************************************************************************/
MD_STATUS R_LPC_SoftwareStandby(void)
{
    MD_STATUS status = MD_OK;

    uint16_t protect_dummy = (uint16_t)(SYSTEM.PRCR.WORD & 0x000BU);
    
    /* Disable protect bit */
    SYSTEM.PRCR.WORD = 0xA503U;
    
    /* When oscillation stop detection function is enabled, SSBY bit is invalid */
    if (SYSTEM.OSTDCR.BIT.OSTDE == 1U)
    {
        status = MD_ERROR1;
    }
    
    /* Select the command for the BRK interrupt routine */
    g_brk_command = BRK_STANDBY;
            
             SYSTEM.SBYCR.BIT.SSBY = 1U;

            /* Prevent out-of-order execution */
            while (SYSTEM.SBYCR.BIT.SSBY != 1U);

            /* Initiate the low-power mode */
            wait();

    /* Trigger MCU power down by calling the brk function */
    brk();

    /* Wait for the CPU to re-start */
    while (g_brk_command == BRK_STANDBY);

    /* Restore the previous state of the protect register */
    SYSTEM.PRCR.WORD = (uint16_t)(0xA500U | protect_dummy);
    
    return status;
}

/***********************************************************************************************************************
* Function Name: R_LPC_ChangeOperatingPowerControl
* Description  : This function allows operating power control to be changed.
* Arguments    : mode -
*                    clock operation mode
* Return Value : status -
*                    MD_OK, MD_ERROR1, MD_ERROR2 or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_LPC_ChangeOperatingPowerControl(operating_mode_t mode)
{
    MD_STATUS status = MD_OK;
    operating_mode_t old_mode;

    uint16_t protect_dummy = (uint16_t)(SYSTEM.PRCR.WORD & 0x000BU);
    
    /* Get the current mode */
    if (1U == SYSTEM.SOPCCR.BIT.SOPCM)
    {
        old_mode = LOW_SPEED;
    }
    else
    {
        if (0U == SYSTEM.OPCCR.BIT.OPCM)
        {
            old_mode = HIGH_SPEED;
        }
        else
        {
            old_mode = MIDDLE_SPEED;
        }
    }

    /* Disable protect bit */
    SYSTEM.PRCR.WORD = 0xA503U;
    
    if (mode != old_mode)
    {
        /* Set the desired operating power control mode */
        switch (mode)
        {
            case LOW_SPEED:
                /* SOPCM should not be set during transition or */
                /* when PLL, HOCO, Main Oscillator or LOCO are operating */
                if ((0U == SYSTEM.SOPCCR.BIT.SOPCMTSF) &&
                    (1U == SYSTEM.PLLCR2.BIT.PLLEN) &&
                    (1U == SYSTEM.HOCOCR.BIT.HCSTP) &&
                    (1U == SYSTEM.MOSCCR.BIT.MOSTP) &&
                    (1U == SYSTEM.LOCOCR.BIT.LCSTP))
                {
                    /* Set low-speed operating mode */
                    SYSTEM.SOPCCR.BIT.SOPCM = 1U;

                    while (SYSTEM.SOPCCR.BIT.SOPCMTSF != 0);
                }
                else
                {
                    status = MD_ERROR1;
                }

                break;

            case MIDDLE_SPEED:
            case HIGH_SPEED:

                /* SOPCCR should not be set during transition */
                if (0U == SYSTEM.SOPCCR.BIT.SOPCMTSF)
                {
                    SYSTEM.SOPCCR.BIT.SOPCM = 0U;

                    while (SYSTEM.SOPCCR.BIT.SOPCMTSF != 0);
                }

                /* OPCCR should not be set when OPCMTSF bit is 1 */
                if (0U == SYSTEM.OPCCR.BIT.OPCMTSF)
                {
                    if (mode == MIDDLE_SPEED)
                    {
                        /* Middle-speed mode should not be set when PLL is in operation */
                        if (1U == SYSTEM.PLLCR2.BIT.PLLEN)
                        {
                            /* Set middle-speed operating mode */
                            SYSTEM.OPCCR.BIT.OPCM = 2U;
                        }
                        else
                        {
                            status = MD_ERROR2;
                        }
                    }
                    else
                    {
                        /* Set high-speed operating mode */
                        SYSTEM.OPCCR.BIT.OPCM = 0U;
                    }

                    while (SYSTEM.OPCCR.BIT.OPCMTSF != 0);
                }

                break;

            default:

                status =  MD_ARGERROR;

                break;
        }
    }

    /* Restore the previous state of the protect register */
    SYSTEM.PRCR.WORD = (uint16_t)(0xA500U | protect_dummy);
    
    return status;
}

/***********************************************************************************************************************
* Function Name: R_LPC_ChangeSleepModeReturnClock
* Description  : This function allows sleep mode return clock to be changed.
* Arguments    : clock -
*                    return clock operation mode
* Return Value : status -
*                    MD_OK, MD_ARGERROR or MD_ERROR1
***********************************************************************************************************************/
MD_STATUS R_LPC_ChangeSleepModeReturnClock(return_clock_t clock)
{
    MD_STATUS status = MD_OK;
    return_clock_t old_clock;

    uint16_t protect_dummy = (uint16_t)(SYSTEM.PRCR.WORD & 0x000BU);
    
    /* Get the current return clock */
    if (0U == SYSTEM.RSTCKCR.BIT.RSTCKEN)
    {
        old_clock = RETURN_DISABLE;
    }
    else
    {
        if (0U == SYSTEM.RSTCKCR.BIT.RSTCKSEL)
        {
            old_clock = RETURN_LOCO;
        }
        else if (1U == SYSTEM.RSTCKCR.BIT.RSTCKSEL)
        {
            old_clock = RETURN_HOCO;
        }
        else if (2U == SYSTEM.RSTCKCR.BIT.RSTCKSEL)
        {
            old_clock = RETURN_MAIN_CLOCK;
        }
        else
        {
            return MD_ERROR1;
        }
    }

    /* Disable protect bit */
    SYSTEM.PRCR.WORD = 0xA503U;
    
    if (clock != old_clock)
    {
        /* Set the desired sleep mode return clock source */
        switch (clock)
        {
            case RETURN_DISABLE:
                SYSTEM.RSTCKCR.BYTE = _00_LPC_RETURN_DISABLED;
                break;

            case RETURN_LOCO:
                SYSTEM.RSTCKCR.BYTE = _80_LPC_RETURN_CLK_LOCO;
                break;

            case RETURN_HOCO:
                SYSTEM.RSTCKCR.BYTE = _81_LPC_RETURN_CLK_HOCO;
                break;

            case RETURN_MAIN_CLOCK:
                SYSTEM.RSTCKCR.BYTE = _82_LPC_RETURN_CLK_MAIN_OSC;
                break;

            default:
                status =  MD_ARGERROR;
                break;
        }
    }

    /* Restore the previous state of the protect register */
    SYSTEM.PRCR.WORD = (uint16_t)(0xA500U | protect_dummy);
    
    return status;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
