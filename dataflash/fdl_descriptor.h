/***********************************************************************************************************************
* File Name     : $Source: fdl_descriptor.h $
* Mod. Revision : $Revision: 1.5 $
* Mod. Date     : $Date: 2013/06/25 10:51:28MESZ $
* Device(s)     : MF3 Flash based RX111 microcontroller
* Description   : FDL run-time configuration descriptor variable related defines.
*                 To be configured according to the application needs
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

/***********************************************************************************************************************
* MISRA Rule:   MISRA-C 2004 rule 3.1 (QAC message 0292)
* Reason:       To support automatic insertion of revision, module name etc. by the source
*               revision control system it is necessary to violate the rule, because the
*               system uses non basic characters as placeholders.
* Verification: The placeholders are used in commentars only. Therefore rule violation cannot
*               influency code compilation.
***********************************************************************************************************************/

#ifndef FDL_DESCRIPTOR_H
#define FDL_DESCRIPTOR_H

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/*--------------------------------------------------------------------------------------------
    Important definitions for run-time configuration of the FDL:
    - Frequency and
    - EEL/FDL Pool
  --------------------------------------------------------------------------------------------*/
/* The Flash programming hardware is provided with a clock, derived from the CPU subsystem
 * frequency. The frequency that needs to be specified here is the internal CPU clock: ICLK.
 * Check that the frequency is correct as this has an impact on the programming quality and performance!
 */
#define FDL_CPU_FREQUENCY_MHZ       (32)        /**< ICLK - internal CPU frequency in MHz */

#define FDL_POOL_SIZE               (8u)        /**< Number of Data Flash blocks, accessible by the FAL.
                                                     Typically it is the complete number of available DF blocks */
#define FDL_EEL_POOL_START          (0u)        /**< 1st block of the EEL pool */
#define FDL_EEL_POOL_SIZE           (0u)        /**< no. of block for the EEL pool */

/*================================================================================================
  ================================================================================================
    Even if possible, this section should not be changed by the user
  ================================================================================================
  ================================================================================================*/
/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/
/* Descriptor variable declaration */

#include <r_fdl_types.h>

extern const r_fdl_descriptor_t   g_eel_AppFdlConfig_enu;   /**< FDL descriptor variable */

#endif /* ifndef FDL_DESCRIPTOR_H */

