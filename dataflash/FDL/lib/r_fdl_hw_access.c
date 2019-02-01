#ifdef ENABLE_QAC_TEST
    #pragma PRQA_MESSAGES_OFF 0292
#endif
/***********************************************************************************************************************
* Library       : Data Flash Access Library for Renesas RX111 devices, based on the MF3 Flash technology
*
* File Name     : $Source: r_fdl_hw_access.c $
* Lib. Version  : $RX111_FDL_LIB_VERSION_T01: V1.01 $
* Mod. Revision : $Revision: 1.16 $
* Mod. Date     : $Date: 2013/06/25 10:52:19MESZ $
* Device(s)     : MF3 Flash based RX111 microcontroller
* Description   : FDL hardware interface functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only  intended for use with
* Renesas products. No other uses are authorized. This software is owned by Renesas Electronics
* Corporation and is protected under all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR
* ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR
* CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the
* availability of this software. By using this software, you agree to the additional terms and conditions
* found by accessing the  following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012-2013 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

#ifdef ENABLE_QAC_TEST
    #pragma PRQA_MESSAGES_ON 0292
#endif

/***********************************************************************************************************************
* MISRA Rule:   MISRA-C 2004 rule 3.1 (QAC message 0292)
* Reason:       To support automatic insertion of revision, module name etc. by the source
*               revision control system it is necessary to violate the rule, because the
*               system uses non basic characters as placeholders.
* Verification: The placeholders are used in commentars only. Therefore rule violation cannot
*               influency code compilation.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_fdl_env.h"
#include "r_fdl_types.h"
#include "r_fdl.h"
#include "r_fdl_global.h"
#include "fdl_cfg.h"

/***********************************************************************************************************************
 Compiler and device specific
***********************************************************************************************************************/
#if   FDL_COMPILER == FDL_COMP_GCC
    #include "rx111.h"
#elif FDL_COMPILER == FDL_COMP_GHS
#elif FDL_COMPILER == FDL_COMP_IAR
    #include "iorx111.h"
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #include "rx111_iodefine_v050.h"
    #pragma section P R_FDL_TEXT
#endif

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
FDL_STATIC uint16_t       R_FDL_ReadMemoryU16     (const uint32_t addr_u32) GCC_SECTION ("R_FDL_TEXT");
FDL_STATIC uint8_t        R_FDL_ReadMemoryU08     (const uint32_t addr_u32) GCC_SECTION ("R_FDL_TEXT");
FDL_STATIC void           R_FDL_WriteMemoryU16    (const uint32_t addr_u32,
                                                   const uint16_t val_u16) GCC_SECTION ("R_FDL_TEXT");
FDL_STATIC r_fdl_status_t R_FDL_ChkAccessRight    (const r_fdl_accessType_t access_type_enu,
                                                   const uint16_t fcu_cmd_u16) GCC_SECTION ("R_FDL_TEXT");
FDL_STATIC void           R_FDL_FCB_WriteRegFLPMC (const uint16_t val_u16) GCC_SECTION ("R_FDL_TEXT");

FDL_STATIC void           R_FDL_WaitUs            (uint32_t tim_u32,
                                                   uint32_t freq_u32) GCC_ATTRIB_NOINLINE GCC_SECTION ("R_FDL_TEXT");
#if FDL_COMPILER == FDL_COMP_REC
    static void           wait_us_asm             (uint32_t tim_u32, uint32_t freq_u32);
#endif
/***********************************************************************************************************************
 * Function name: R_FDL_ReadMemoryU16
 **********************************************************************************************************************/
/**
 * Function to read a 16-bit IO register or memory
 *
 * @param[in]  addr_u32 - source address
 * @return     16-bit register contents
 */
/**********************************************************************************************************************/
/***********************************************************************************************************************
* MISRA Rule:   MISRA-C 2004 rule 11.3 (QAC message 0303)
* Reason:       For effective embedded programming, integer to pointer conversions are used
* Verification: The converted addresses are essential for complete code execution. Incorrect
*               conversion would result in test fails.
***********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC uint16_t R_FDL_ReadMemoryU16 (const uint32_t addr_u32)
{
    return ( *( (volatile uint16_t *)(addr_u32) ) );                           /* PRQA S 0303 */
}


/***********************************************************************************************************************
 * Function name: R_FDL_ReadMemoryU08
 **********************************************************************************************************************/
/**
 * Function to read a 8-bit IO register or memory
 *
 * @param[in]  addr_u32 - source address
 * @return     8-bit register contents
 */
/**********************************************************************************************************************/
/***********************************************************************************************************************
* MISRA Rule:   MISRA-C 2004 rule 11.3 (QAC message 0303)
* Reason:       For effective embedded programming, integer to pointer conversions are used
* Verification: The converted addresses are essential for complete code execution. Incorrect
*               conversion would result in test fails.
***********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC uint8_t R_FDL_ReadMemoryU08 (const uint32_t addr_u32)
{
    return ( *( (volatile uint8_t *)(addr_u32) ) );                            /* PRQA S 0303 */
}

/***********************************************************************************************************************
 * Function name: R_FDL_WriteMemoryU16
 **********************************************************************************************************************/
/**
 * Function to write a 16-bit IO register or memory
 *
 * @param[in]  addr_u32 - write destination address
 * @param[in]  val_u16  - 16-bit write data
 * @return     void
 */
/**********************************************************************************************************************/
/***********************************************************************************************************************
* MISRA Rule:   MISRA-C 2004 rule 11.3 (QAC message 0303)
* Reason:       For effective embedded programming, integer to pointer conversions are used
* Verification: The converted addresses are essential for complete code execution. Incorrect
*               conversion would result in test fails.
***********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC void R_FDL_WriteMemoryU16 (const uint32_t addr_u32, const uint16_t val_u16)
{
    ( *( (volatile uint16_t *)(addr_u32) ) ) = val_u16;                        /* PRQA S 0303 */
}


/***********************************************************************************************************************
 * Function name: R_FDL_ChkAccessRight
 **********************************************************************************************************************/
/**
 * The check is a robustness feature against modification from outside the library as the parameter
 * check during command initiation should have detected any parameter error already. This check
 * shall block wrong settings directly before starting the Flash hardware. So,
 * on violation, the returned error is a protection error, not a parameter error. \n
 * The function reads back the access parameters from the FCB for the check and compares them
 * against the access rights defined by the descriptor.
 *
 * @param[in]  access_type_enu - type of FDL access (user or EEL)
 * @param[in]  fcu_cmd_u16  - FCB command
 *
 * @return      Returns the access check result
 * @li          R_FDL_OK (check was OK)
 * @li          R_FDL_ERR_PROTECTION (parameter detected)
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC r_fdl_status_t R_FDL_ChkAccessRight (const r_fdl_accessType_t access_type_enu,
                                                  const uint16_t fcu_cmd_u16)
{
    r_fdl_status_t status;
    uint32_t       start_addr;
    uint32_t       byte_cnt;

    start_addr =  R_FDL_ReadMemoryU08 (FCB_REGADD_FLAPH_U08);
    start_addr <<= NO_16;
    start_addr += R_FDL_ReadMemoryU16 (FCB_REGADD_FLAPL_U16);

    if (FCB_REGPAT_FSSQ_CMD_WRT == fcu_cmd_u16)
    {
        byte_cnt = FDL_WRITE_SIZE; /* Write one write unit */
    }
    else
    {
        byte_cnt =  R_FDL_ReadMemoryU08 (FCB_REGADD_FLSEDH_U08);
        byte_cnt <<= NO_16;
        byte_cnt += R_FDL_ReadMemoryU16 (FCB_REGADD_FLSEDL_U16);
        byte_cnt =  (byte_cnt - start_addr);
        if (FCB_REGPAT_FSSQ_CMD_BC == fcu_cmd_u16)
        {
            /* make byte count from end address */
            byte_cnt += FDL_WRITE_SIZE;
        }
    }

    start_addr -= FDL_DF_BASE_ADD;

    #ifdef PATCH_TO_SIMULATE_ERRORS
        if (R_FDL_TRUE == g_fdl_data_str.simError_enu)
        {
            if (R_FDL_SIM_ERROR_PROTECTION == g_fdl_data_str.simErrorType_enu)
            {
                /* byte_cnt = 0 must raise a protection error */
                byte_cnt = 0;
                g_fdl_data_str.simError_enu = R_FDL_FALSE;
            }
        }
    #endif

    status = R_FDL_ChkAccessBoundaries (start_addr, byte_cnt, access_type_enu);
    /* R_FDL_ChkAccessBoundaries returns PARAMETER error as it is used in R_FDL_Execute for
       parameter check. If we check directly before Flash operation start, we want to return
       R_FDL_ERR_PROTECTION in error case. So we patch the return value here */
    if (R_FDL_ERR_PARAMETER == status)
    {
        R_FDL_FCB_Reset ();
        (void)R_FDL_FCB_SwitchMode (R_FDL_FCB_MODE_USER);
        status = R_FDL_ERR_PROTECTION;
    }

    return (status);
} /* R_FDL_ChkAccessRight */


/***********************************************************************************************************************
 * Function name: R_FDL_FCB_WriteRegFLPMC
 **********************************************************************************************************************/
/**
 * Write the FLPMC register, that is secured by a special sequence. The write must be ckecked in
 * a loop, as a DMA or interrupt handling can lead to an wrong sequence, thus preventing the write.
 * In this case the write must be repeated
 *
 * @param[in]  val_u16 - register target value
 *
 * @return     void
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC void R_FDL_FCB_WriteRegFLPMC (const uint16_t val_u16)
{
    volatile uint16_t regFLPMC;

    do
    {
        R_FDL_WriteMemoryU08 (FCB_REGADD_PFCMD_U08, FCB_REGPAT_PFCMD_U16);
        R_FDL_WriteMemoryU08 (FCB_REGADD_FLPMC_U08, (uint8_t)val_u16);
        R_FDL_WriteMemoryU08 ( FCB_REGADD_FLPMC_U08, (uint8_t)(~val_u16) );
        R_FDL_WriteMemoryU08 (FCB_REGADD_FLPMC_U08, (uint8_t)val_u16);
        regFLPMC = R_FDL_ReadMemoryU08 (FCB_REGADD_FLPMC_U08);
    }
    while (regFLPMC != val_u16);
}


/***********************************************************************************************************************
 * Function name: R_FDL_WaitUs
 **********************************************************************************************************************/
/**
 * Software wait loop.
 * The function wait for x us @ the frequency configured by the parameter.
 * Note: The accuracy is rather low for low frequencies and low wait times because the loop
 *       count is an integer value of tim*freq/8   \n
 *
 * The function uses assembler instructions in order to reduce the dependancy on copiler
 * optimizations
 *
 * @param      tim - us to wait
 * @param      freq - device frequency
 * @return     void
 */
/**********************************************************************************************************************/
/*********************************************************************************************************************
 * MISRA Rule:   MISRA-C 2004 rule 3.1 (QAC message 3206)
 * Reason:       function parameter seems to be not used in the function as no c-code relates on it,
 *               but the assembler code uses the parameter
 * Verification: -
 *********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_GCC
    FDL_STATIC void R_FDL_WaitUs (uint32_t tim_u32, uint32_t freq_u32)
    {
        asm ("MUL r1, r2");

        /* the wait loop shall last 8 clocks */
        asm ("SHAR #3, r2");

        /* wait loop */
        asm ("_R_FDL_WaitUs_Loop:");
        asm ("BZ _R_FDL_WaitUs_End");
        asm ("nop");
        asm ("nop");
        asm ("nop");
        asm ("ADD #-1, r2");
        asm ("BRA.B _R_FDL_WaitUs_Loop");
        asm ("_R_FDL_WaitUs_End:");
    } /* R_FDL_WaitUs */
#elif FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
    FDL_STATIC void R_FDL_WaitUs (uint32_t tim_u32, uint32_t freq_u32)
    {
        /* the wait loop shall last 8 clocks */
        __asm ("MUL r1, r2\n"
               "SHAR #3, r2\n"
               "_R_FDL_WaitUs_Loop:\n"
               "bz.b _R_FDL_WaitUs_End\n"
               "nop\n"
               "nop\n"
               "nop\n"
               "ADD #-1, r2\n"
               "BRA.B _R_FDL_WaitUs_Loop\n"
               "_R_FDL_WaitUs_End:");
    } /* R_FDL_WaitUs */
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
    #pragma inline_asm wait_us_asm
    static void wait_us_asm (uint32_t tim_u32, uint32_t freq_u32)               /* PRQA S 3206 */
    {
        #ifndef ENABLE_QAC_TEST
            MUL r1, r2

            /* the wait loop shall last 8 clocks */
            SHAR #3, r2

            /* wait loop */
        Wait_us_Loop:
            BZ Wait_us_End
            nop
            nop
            nop
            ADD #-1, r2
            BRA.B Wait_us_Loop
        Wait_us_End:
        #endif
    }

    FDL_STATIC void R_FDL_WaitUs (uint32_t tim_u32, uint32_t freq_u32)
    {
        /* wrap over wait_us_asm because that function is inlined and can be used only once */
        wait_us_asm (tim_u32, freq_u32);
    }
#else
    #error "Unknown compiler"
#endif /* if FDL_COMPILER */


/***********************************************************************************************************************
        Public interface functions from now on
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Function name: R_FDL_WriteMemoryU08
 **********************************************************************************************************************/
/**
 * Function to write a 8-bit IO register or memory
 *
 * @param[in]  add_u32 - write destination address
 * @param[in]  val_u8  - 8-bit write data
 * @return     void
 */
/**********************************************************************************************************************/
/***********************************************************************************************************************
* MISRA Rule:   MISRA-C 2004 rule 11.3 (QAC message 0303)
* Reason:       For effective embedded programming, integer to pointer conversions are used
* Verification: The converted addresses are essential for complete code execution. Incorrect
*               conversion would result in test fails.
***********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
void R_FDL_WriteMemoryU08 (const uint32_t add_u32, const uint8_t val_u8)
{
    ( *( (volatile uint8_t *)(add_u32) ) ) = val_u8;                           /* PRQA S 0303 */
}


/***********************************************************************************************************************
 * Function name: R_FDL_ChkAccessBoundaries
 **********************************************************************************************************************/
/**
 * This function checks if the Flash access boundaries are correct. Parameter error if:
 * - end address calc. wrap around
 * - end address overflow
 * - selected block range is not in the correct pool (User/EEL)
 *
 * @param[in]   access_type_enu  - type of FDL access (user or EEL)
 * @param[in]   start_addr_u32 - Start of the block range to access
 * @param[in]   byte_cnt_u32   - Number of Bytes for the access
 * @return      Returns the access check result
 * @li          R_FDL_BUSY (check was OK)
 * @li          R_FDL_ERR_PARAMETER (parameter error detected)
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
r_fdl_status_t R_FDL_ChkAccessBoundaries (const uint32_t start_addr_u32,
                                          const uint32_t byte_cnt_u32,
                                          const r_fdl_accessType_t access_type_enu)
{
    uint32_t       eel_pool_start;
    uint32_t       eel_pool_size;
    uint32_t       eel_pool_end;
    uint32_t       fdl_pool_end;
    uint32_t       end_addr;
    r_fdl_flag_t   in_eel_range;
    r_fdl_status_t status = R_FDL_BUSY;

    eel_pool_start = ( (g_fdl_data_str.RTCfg_pstr)->eelPoolStart_u16 ) << FDL_BLOCK_SIZE_2N;
    eel_pool_size  = ( (g_fdl_data_str.RTCfg_pstr)->eelPoolSize_u16 ) << FDL_BLOCK_SIZE_2N;
    eel_pool_end   = (eel_pool_start + eel_pool_size) - 1u;
    fdl_pool_end   = ( ( (g_fdl_data_str.RTCfg_pstr)->fdlPoolSize_u16 ) << FDL_BLOCK_SIZE_2N ) - 1uL;
    end_addr       = (start_addr_u32 + byte_cnt_u32) - 1uL;
    in_eel_range   = R_FDL_FALSE;

    /* ------------- Judge invalid addresses ------------ */
    if ( (start_addr_u32 > end_addr) ||                                 /* end address calc. wrap around */
         (end_addr >= FDL_DF_SIZE_CHECK_LIMIT) )                        /* end address overflow */
    {
        status = R_FDL_ERR_PARAMETER;
    }
    else
    {
        /* ------------- Judge access boundaries ------------ */
        /* Check EEL range: Are we inside, outside or overlapping
           eel_pool_size > 0: condition that the range is defined at all */
        if (eel_pool_size > 0u)
        {
            if ( (start_addr_u32 >= eel_pool_start) &&
                 (end_addr <= eel_pool_end) )
            {
                in_eel_range = R_FDL_TRUE;
            }
            else
            {
                if ( !( (end_addr < eel_pool_start) || (start_addr_u32 > eel_pool_end) ) )
                {
                    status = R_FDL_ERR_PARAMETER;
                }
            }
        }

        /* As EEL we must remain in the EEL range */
        if (R_FDL_ACCESS_EEL == access_type_enu)
        {
            if (R_FDL_FALSE == in_eel_range)
            {
                status = R_FDL_ERR_PARAMETER;
            }
        }
        /* As user we must remain in the Data Flash but outside EEL range */
        else if (R_FDL_ACCESS_USER == access_type_enu)
        {
            if ( (end_addr > fdl_pool_end) || (R_FDL_TRUE == in_eel_range) )
            {
                status = R_FDL_ERR_PARAMETER;
            }
        }
        else
        {
            status = R_FDL_ERR_PARAMETER;
        }
    }

    return (status);
} /* R_FDL_ChkAccessBoundaries */


/***********************************************************************************************************************
 * Function name: R_FDL_FCB_Reset
 **********************************************************************************************************************/
/**
 * Reset Flash programming sequencer.
 * As the reset does not work on running sequencer and in case of transient conditions, it must
 * be stopped first.
 *
 * @param      void
 * @return     void
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
void R_FDL_FCB_Reset (void)
{
    volatile uint16_t reg_FSSQ;
    volatile uint16_t reg_FSASTH;

    reg_FSSQ = R_FDL_ReadMemoryU08 (FCB_REGADD_FSSQ_U08);

    /* Stop the sequencer if it is busy */
    if ( FCB_REGBIT_FSSQ_SQST == (reg_FSSQ & FCB_REGBIT_FSSQ_SQST) )
    {
        reg_FSSQ |= FCB_REGBIT_FSSQ_FSSTP;
        R_FDL_WriteMemoryU08 (FCB_REGADD_FSSQ_U08, (uint8_t)reg_FSSQ);

        do
        {
            reg_FSASTH = R_FDL_ReadMemoryU08 (FCB_REGADD_FSASTH_U08);
        }
        while ( 0 == (FCB_REGBIT_FSASTH_SQEND & reg_FSASTH) );

        R_FDL_WriteMemoryU08 (FCB_REGADD_FSSQ_U08, FCB_REGPAT_FSSQ_RESET);

        do
        {
            reg_FSASTH = R_FDL_ReadMemoryU08 (FCB_REGADD_FSASTH_U08);
        }
        while ( 0 != (FCB_REGBIT_FSASTH_SQEND & reg_FSASTH) );
    } /* end if ( FCB_REGBIT_FSSQ_SQST == (reg_FSSQ & FCB_REGBIT_FSSQ_SQST) )*/

    /* Issue reset signal on stopped sequencer */
    R_FDL_WriteMemoryU08 (FCB_REGADD_FLRST_U08, FCB_REGBIT_FLRST_FLRST);
    R_FDL_WriteMemoryU08 (FCB_REGADD_FLRST_U08, FCB_REGBIT_FLRST_FLRST_NOT);
} /* R_FDL_FCB_Reset */


/***********************************************************************************************************************
 * Function name: R_FDL_FCB_SwitchMode
 **********************************************************************************************************************/
/**
 * Switch FCB mode to Programming/User mode
 *
 * @param      mode_enu - switch FCB to mode:
 * @li          R_FDL_FCB_MODE_PE (programming) or
 * @li          R_FDL_FCB_MODE_USER (normal)
 * @return     result of the switch operation
 * @li          R_FDL_OK - switch operation executed
 * @li          R_FDL_ERR_PROTECTION - operation rejected because wrong operation mode
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
r_fdl_status_t R_FDL_FCB_SwitchMode (const r_fdl_fcb_mode_t mode_enu)
{
    volatile uint8_t  reg_FSSET;
    r_fdl_status_t    stat = R_FDL_OK;
    uint16_t          cpu_freq = (g_fdl_data_str.RTCfg_pstr)->fdlFrequencyCpuMHz_u16;

    if (R_FDL_FCB_MODE_USER == mode_enu)
    {
        /* disable P/E mode: set FLPMC register = 0x08 */
        R_FDL_FCB_WriteRegFLPMC (FCB_REGBIT_FLPMC_FWEDIS);

        if (SYSTEM_MODE_HIGH_SPEED == SYSTEM_MODE_MD_HI)                            /* PRQA S 0303 */
        {
            /* High speed operating mode */
            R_FDL_WaitUs (WAIT_TMS_TIME_HS_Us, cpu_freq);
        }
        else
        {
            /* Middle speed operating mode */
            R_FDL_WaitUs (WAIT_TMS_TIME_MS_Us, cpu_freq);
        }

        /* transition to read mode */
        R_FDL_WriteMemoryU16 ( FCB_REGADD_FENTRYR_U16, (uint16_t)(FCB_REGVAL_FENTRYR_CMD) );
    } /*end if (R_FDL_FCB_MODE_USER == mode_enu) */
    else
    {
        /* FCB does not work in low speed mode */
        if (0 == SYSTEM_MODE_LO)                                                    /* PRQA S 0303 */
        {
            R_FDL_WriteMemoryU16 ( FCB_REGADD_FENTRYR_U16, (uint16_t)(FCB_REGVAL_FENTRYR_CMD + FCB_REGBIT_FENTRYRD) );

            if (SYSTEM_MODE_HIGH_SPEED == SYSTEM_MODE_MD_HI)                        /* PRQA S 0303 */
            {
                /* High speed operating mode */
                R_FDL_FCB_WriteRegFLPMC (FCB_REGBIT_FLPMC_EEEMD);
                R_FDL_WaitUs (WAIT_TMS_TIME_HS_Us, cpu_freq);
            }
            else
            {
                /* Middle speed operating mode */
                R_FDL_FCB_WriteRegFLPMC (FCB_REGBIT_FLPMC_EEEMD | FCB_REGBIT_FLPMC_LOWSPY);
                R_FDL_WaitUs (WAIT_TMS_TIME_MS_Us, cpu_freq);
            }

            reg_FSSET = R_FDL_ReadMemoryU08 (FCB_REGADD_FSSET_U08);
            reg_FSSET &= (uint8_t)(FCB_REGMASK_FSSET_FSET_NOT);
            reg_FSSET |= (uint8_t)(g_fdl_data_str.fcb_freq_Mhz - 1u);

            R_FDL_WriteMemoryU08 (FCB_REGADD_FSSET_U08, reg_FSSET);
        } /* end if (0 == SYSTEM_MODE_LO)*/
        else
        {
            stat = R_FDL_ERR_PROTECTION;
        }
    } /*if (R_FDL_FCB_MODE_USER == mode_enu) - else*/

    return (stat);
} /* R_FDL_FCB_SwitchMode */


/***********************************************************************************************************************
 * Function name: R_FDL_FCB_GetOPAdd
 **********************************************************************************************************************/
/**
 * Read the current operations address from the FCB.
 *
 * @param      void
 * @return     operation address
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
uint32_t R_FDL_FCB_GetOPAdd (void)
{
    uint32_t add;

    add = R_FDL_ReadMemoryU08 (FCB_REGADD_FLAPH_U08);
    add <<= NO_16;
    add += R_FDL_ReadMemoryU16 (FCB_REGADD_FLAPL_U16);

    return (add);
}


/***********************************************************************************************************************
 * Function name: R_FDL_FCB_GetStat
 **********************************************************************************************************************/
/**
 * Return FCB Flash operation result. In case of BC also the fail address is returned.
 * In case of a preceeding Suspend/Resume or Standby/Wakeup, the FCB might already be resetted.
 * In that case, the backed up result is taken from the FDL variables.
 *
 * @param       void
 * @return      Operation result:
 * @li          R_FDL_ERR_BLANKCHECK
 * @li          R_FDL_ERR_WRITE
 * @li          R_FDL_ERR_ERASE
 * @li          R_FDL_BUSY
 * @li          R_FDL_OK
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
r_fdl_status_t R_FDL_FCB_GetStat (uint32_t * failAdd_pu32)
{
    r_fdl_status_t    stat;
    volatile uint16_t reg_FSASTL;
    uint32_t          add;

    if (R_FDL_BUSY != g_fdl_data_str.fcbStatBackup_enu)
    {
        stat = g_fdl_data_str.fcbStatBackup_enu;
        (*failAdd_pu32) = g_fdl_data_str.fcbAddBackup_u32;
        g_fdl_data_str.fcbStatBackup_enu = R_FDL_BUSY;
    }
    else
    {
        /* Read background operation result */
        reg_FSASTL  = R_FDL_ReadMemoryU08 (FCB_REGADD_FSASTL_U08);
        reg_FSASTL &= (uint16_t)(FCB_REGMASK_FSASTL_ERR);

        #ifdef PATCH_TO_SIMULATE_ERRORS
            if (R_FDL_TRUE == g_fdl_data_str.simError_enu)
            {
                switch (g_fdl_data_str.simErrorType_enu)
                {
                    case R_FDL_SIM_ERROR_ERASE:
                    {
                        reg_FSASTL |= FCB_REGBIT_FSASTL_ERER;
                        g_fdl_data_str.simError_enu = R_FDL_FALSE;
                        break;
                    }

                    case R_FDL_SIM_ERROR_WRITE:
                    {
                        reg_FSASTL |= FCB_REGBIT_FSASTL_WRER;
                        g_fdl_data_str.simError_enu = R_FDL_FALSE;
                        break;
                    }

                    case R_FDL_SIM_ERROR_BLANKCHECK:
                    {
                        reg_FSASTL |= FCB_REGBIT_FSASTL_BLER;
                        g_fdl_data_str.simError_enu = R_FDL_FALSE;
                        break;
                    }

                    default:
                    {
                        /* Do Nothing */
                        break;
                    }
                } /* switch */
            } /* end if (R_FDL_TRUE == g_fdl_data_str.simError_enu)*/

            if (g_fdl_data_str.actFlag_FSASTL_enu == R_FDL_TRUE)
            {
                reg_FSASTL = g_fdl_data_str.reg_FSASTL_u08;
                g_fdl_data_str.actFlag_FSASTL_enu = R_FDL_FALSE;
            }
        #endif /* ifdef PATCH_TO_SIMULATE_ERRORS */


        /* Check if the operation finished with an error */
        if (FCB_REGBITS_FSASTL_FLASHOK == reg_FSASTL)
        {
            /* No error -> return value set to OK */
            stat = R_FDL_OK;
        }
        else
        {
            /* Change status to get the same messages in FW and
               User Status Check mode */
            if (  (FCB_REGBIT_FSASTL_BLER == reg_FSASTL)
               && (R_FDL_CMD_BLANKCHECK == (g_fdl_data_str.fdl_reqInt_pstr)->command_enu) )
            {
                stat = R_FDL_ERR_BLANKCHECK;     /* Blank check error */
            }
            else if (  (FCB_REGBIT_FSASTL_WRER == reg_FSASTL)
                    && (R_FDL_CMD_WRITE == (g_fdl_data_str.fdl_reqInt_pstr)->command_enu) )
            {
                stat = R_FDL_ERR_WRITE;          /* Write error */
            }
            else if (  (FCB_REGBIT_FSASTL_ERER == reg_FSASTL)
                    && (R_FDL_CMD_ERASE == (g_fdl_data_str.fdl_reqInt_pstr)->command_enu) )
            {
                stat = R_FDL_ERR_ERASE;          /* Erase error */
            }
            else
            {
                stat = R_FDL_ERR_INTERNAL;
            }

            if (R_FDL_ERR_BLANKCHECK == stat)
            {
                add = R_FDL_ReadMemoryU16 (FCB_REGADD_FLFADH_U16);
                add <<= NO_16;
                add += R_FDL_ReadMemoryU16 (FCB_REGADD_FLFADL_U16);
                (*failAdd_pu32) = add - FDL_DF_BASE_ADD;
            }
        } /* if (FCB_REGBITS_FSASTL_FLASHOK == reg_FSASTL) - else */

        R_FDL_FCB_Reset ();
    } /* if (R_FDL_BUSY != g_fdl_data_str.fcbStatBackup_enu) - else */

    return (stat);
} /* R_FDL_FCB_GetStat */


/***********************************************************************************************************************
 * Function name: R_FDL_FCB_StartFlashOperation
 *********************************************************************************************************************/
/**
 * This function configures the Flash programming sequencer and starts the operation
 *
 * @param  flash_addr_u32       - Start address (1st block address for erase, 1st address for BC/Write)
 * @param  data_or_end_addr_u32 - End address of the Flash operation on Erase, bCnt
 *                                (last block address+1 for erase, last address+1 for BC) or data value in
 *                                case of write operation
 * @param  fcu_cmd_u16          - Type of command. Erase / BC / Write
 * @param  access_type_enu      - User/EEL access (check of access rights)
 *
 * @return status of the operation to start
 * @li     R_FDL_BUSY - operation started
 * @li     R_FDL_ERR_PROTECTION - operation blocked due to protection
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
r_fdl_status_t R_FDL_FCB_StartFlashOperation (uint32_t flash_addr_u32,
                                              uint32_t data_or_end_addr_u32,
                                              uint16_t fcu_cmd_u16,
                                              const r_fdl_accessType_t access_type_enu)
{
    r_fdl_status_t status;
    uint8_t        data;

    flash_addr_u32 += FDL_DF_BASE_ADD;

    R_FDL_WriteMemoryU16 ( FCB_REGADD_FLAPL_U16, (uint16_t)(flash_addr_u32 & 0xFFFFu) );
    R_FDL_WriteMemoryU08 ( FCB_REGADD_FLAPH_U08, (uint8_t)(flash_addr_u32 >> 16u) );

    if (FCB_REGPAT_FSSQ_CMD_WRT == fcu_cmd_u16)
    {
        /* write */
        R_FDL_WriteMemoryU16 (FCB_REGADD_FLSEDL_U16, 0x0000u);
        R_FDL_WriteMemoryU08 (FCB_REGADD_FLSEDH_U08, 0x00u);

        /* write one byte at a time */
        data = R_FDL_ReadMemoryU08 (data_or_end_addr_u32);
        R_FDL_WriteMemoryU16 (FCB_REGADD_FLWL0_U16, (uint16_t)data);
    }
    else
    {
        /* erase */
        data_or_end_addr_u32 += FDL_DF_BASE_ADD;
        R_FDL_WriteMemoryU16 ( FCB_REGADD_FLSEDL_U16, (uint16_t)(data_or_end_addr_u32 & 0xFFFFu) );
        R_FDL_WriteMemoryU08 ( FCB_REGADD_FLSEDH_U08, (uint8_t)(data_or_end_addr_u32 >> 16u) );
    }

    status = R_FDL_ChkAccessRight (access_type_enu, fcu_cmd_u16);

    if (R_FDL_BUSY == status)
    {
        /* Start FLash operation */
        R_FDL_WriteMemoryU08 (FCB_REGADD_FSSQ_U08, (uint8_t)fcu_cmd_u16);
        fcu_cmd_u16 |= FCB_REGBIT_FSSQ_SQST;
        R_FDL_WriteMemoryU08 (FCB_REGADD_FSSQ_U08, (uint8_t)fcu_cmd_u16);
    }

    return (status);
} /* R_FDL_FCB_StartFlashOperation */


/***********************************************************************************************************************
 * Function name: R_FDL_FCB_ChkReady
 **********************************************************************************************************************/
/**
 * Returns sequencer busy/Ready status \n
 * The sequencer might be busy or not busy (not configured or configured operation already
 * finished)
 *
 * @param   void
 * @return  Status of the sequencer
 * @li      R_FDL_TRUE  - Sequencer is not busy with a Flash operation
 * @li      R_FDL_FALSE - Sequencer is busy with a Flash operation
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
r_fdl_flag_t R_FDL_FCB_ChkReady (void)
{
    r_fdl_flag_t     status;
    volatile uint8_t reg_FSASTH;
    volatile uint8_t reg_FSSQ;

    reg_FSSQ   = R_FDL_ReadMemoryU08 (FCB_REGADD_FSSQ_U08);
    reg_FSASTH = R_FDL_ReadMemoryU08 (FCB_REGADD_FSASTH_U08);

    /* Check if FCB is operating */
    if ( 0 != (reg_FSASTH & FCB_REGBIT_FSASTH_SQEND) )
    {
        /* Reset FSSQ register and wait for reset to finish */
        R_FDL_WriteMemoryU08 (FCB_REGADD_FSSQ_U08, FCB_REGPAT_FSSQ_RESET);

        do
        {
            reg_FSASTH = R_FDL_ReadMemoryU08 (FCB_REGADD_FSASTH_U08);
        }
        while ( 0 != (FCB_REGBIT_FSASTH_SQEND & reg_FSASTH) );

        status = R_FDL_TRUE;
    }
    else if ( 0 == (reg_FSSQ & FCB_REGBIT_FSSQ_SQST) )
    {
        /* FCB is not busy */
        status = R_FDL_TRUE;
    }
    else
    {
        status = R_FDL_FALSE;
    }

    return (status);
} /* R_FDL_FCB_ChkReady */

