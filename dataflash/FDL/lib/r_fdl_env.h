#ifdef ENABLE_QAC_TEST
    #pragma PRQA_MESSAGES_OFF 0292
#endif
/***********************************************************************************************************************
* Library       : Data Flash Access Library for Renesas RX111 devices, based on the MF3 Flash technology
*
* File Name     : $Source: r_fdl_env.h $
* Lib. Version  : $RX111_FDL_LIB_VERSION_T01: V1.01 $
* Mod. Revision : $Revision: 1.9 $
* Mod. Date     : $Date: 2013/06/25 10:52:13MESZ $
* Device(s)     : MF3 Flash based RX111 microcontroller
* Description   : Flash programming hardware related definitions
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

#ifndef R_FDL_ENV_H
#define R_FDL_ENV_H

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/* FCB definitions */
#define FLASHREG_BASE_A             (0x007FC000uL)
#define FLASHREG_BASE_B             (0x007FFF00uL)

#define FCB_REGADD_FLPMC_U08        (FLASHREG_BASE_B + 0x80uL)
#define FCB_REGADD_FLAPL_U16        (FLASHREG_BASE_B + 0x82uL)
#define FCB_REGADD_FLAPH_U08        (FLASHREG_BASE_B + 0x84uL)
#define FCB_REGADD_FSSQ_U08         (FLASHREG_BASE_B + 0x85uL)
#define FCB_REGADD_FLSEDL_U16       (FLASHREG_BASE_B + 0x86uL)
#define FCB_REGADD_FLSEDH_U08       (FLASHREG_BASE_B + 0x88uL)
#define FCB_REGADD_FLRST_U08        (FLASHREG_BASE_B + 0x89uL)
#define FCB_REGADD_FSASTL_U08       (FLASHREG_BASE_B + 0x8AuL)
#define FCB_REGADD_FSASTH_U08       (FLASHREG_BASE_B + 0x8BuL)
#define FCB_REGADD_FLWL0_U16        (FLASHREG_BASE_B + 0x8CuL)
#define FCB_REGADD_FLWH0_U16        (FLASHREG_BASE_B + 0x8EuL)
#define FCB_REGADD_FENTRYR_U16      (FLASHREG_BASE_B + 0xB2uL)

#define FCB_REGADD_PFCMD_U08        (FLASHREG_BASE_A + 0xC0uL)
#define FCB_REGADD_FSSET_U08        (FLASHREG_BASE_A + 0xB6uL)
#define FCB_REGADD_FLFADL_U16       (FLASHREG_BASE_A + 0xB8uL)
#define FCB_REGADD_FLFADH_U16       (FLASHREG_BASE_A + 0xBAuL)

#define FCB_REGBIT_FLRST_FLRST      (0x01u)
#define FCB_REGBIT_FLRST_FLRST_NOT  (0xFEu)     /* avoid QAC msg. */
#define FCB_REGBIT_FSASTH_SQEND     (0x40u)
#define FCB_REGMASK_FSASTL_ERR      (0x1Fu)
#define FCB_REGBIT_FSASTL_ERER      (0x01u)
#define FCB_REGBIT_FSASTL_WRER      (0x02u)
#define FCB_REGBIT_FSASTL_BLER      (0x08u)
#define FCB_REGBIT_FLPMC_CWEE       (0x80u)
#define FCB_REGBIT_FLPMC_FLSPM      (0x02u)
#define FCB_REGBITS_FSASTL_FLASHOK  (0x00u)
#define FCB_REGMASK_FSSET_FSET      (0x1Fu)
#define FCB_REGMASK_FSSET_FSET_NOT  (0xE0u)
#define FCB_REGBIT_FLPMC_EEEMD      (0x10u)
#define FCB_REGBIT_FLPMC_LOWSPY     (0x40u)
#define FCB_REGBIT_FLPMC_FWEDIS     (0x08u)
#define FCB_REGPAT_FLPMC_RESET      (0x08u)
#define FCB_REGMASK_FLPMC_STAT      (0x1Fu)
#define FCB_REGPAT_PFCMD_U16        (0xA5u)
#define FCB_REGPAT_FSSQ_RESET       (0x00u)
#define FCB_REGPAT_FSSQ_CMD_WRT     (0x01u)
#define FCB_REGPAT_FSSQ_CMD_BC      (0x0Bu)  /* BC mode + MRG12 level*/
#define FCB_REGPAT_FSSQ_CMD_ERS     (0x04u)
#define FCB_REGBIT_FSSQ_FSSTP       (0x40u)
#define FCB_REGBIT_FSSQ_SQST        (0x80u)
#define FCB_REGBIT_FENTRYRD         (0x0080u)
#define FCB_REGBIT_FENTRYRI         (0x0001u)

#define FCB_REGVAL_FENTRYR_CMD      (0xAA00)    /* mode switching command P/E <-> read */

/* Other Data Flash related defines */
#define FDL_BLOCK_SIZE              (0x0400u)                   /* EEP Flash block size in bytes */
#define FDL_BLOCK_SIZE_2N           (0x000Au)                   /* EEP block size as 2^N Bytes */
#define FDL_WRITE_SIZE              (0x00000001uL)              /* Write granularity */
#define FDL_DF_SIZE_CHECK_LIMIT     (0x00100000uL)              /* Check limit for DF Size */
#define FDL_DF_BASE_ADD             (0x000F1000)                /* base adress for write/erase operations */
#define FDL_DF_BASE_READ_ADD        (0x00100000)                /* base adress for read operation */

/* clock limits */
#define FDL_FCBCLK_MIN_MHz          (1)     /* absolute minimal frequency required for FCB */
#define FDL_FCBCLK_MAX_HIV_MHz      (32)    /* max high speed mode */
#define FDL_FCBCLK_MAX_MDV_MHz      (12)    /* max middle speed mode */

/* clock operating modes */
#define SYSTEM_MODE_HIGH_SPEED      (0)
#define SYSTEM_MODE_MEDIUM_SPEED    (2)
#define SYSTEM_MODE_MD_HI           (SYSTEM.OPCCR.BIT.OPCM)
#define SYSTEM_MODE_LO              (SYSTEM.SOPCCR.BIT.SOPCM)

/* clock prescallers for CPU (ICLK) and FlashIF (FCLK) */
#define PRESCALER_ICK               (SYSTEM.SCKCR.BIT.ICK)
#define PRESCALER_FCK               (SYSTEM.SCKCR.BIT.FCK)

/* Product info register */
#define DF_SIZE_BITS                ( ( (*(volatile uint16_t *)0x007FFFB6) & 0x0018 ) >> 3 )

/* FCB (de)activation times */
#define WAIT_TDIS_TIME_Us            (2)
#define WAIT_TMS_TIME_HS_Us          (5)
#define WAIT_TMS_TIME_MS_Us          (3)

/* QAC stuff */
#define NO_16                        (16u)

#endif /* ifndef R_FDL_ENV_H */

