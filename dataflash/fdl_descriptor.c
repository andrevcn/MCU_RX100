/***********************************************************************************************************************
* File Name     : $Source: fdl_descriptor.c $
* Mod. Revision : $Revision: 1.4 $
* Mod. Date     : $Date: 2013/06/25 10:49:18MESZ $
* Device(s)     : MF3 Flash based RX111 microcontroller
* Description   : descriptor variable definition
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
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_fdl_types.h"
#include "r_fdl.h"
#include "fdl_descriptor.h"

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
const r_fdl_descriptor_t g_eel_AppFdlConfig_enu =
{
    FDL_CPU_FREQUENCY_MHZ,      /**< ICLK - internal CPU frequency in MHz */
    FDL_POOL_SIZE,              /**< Number of Data Flash blocks, accessible by the FAL.
                                     Typically it is the complete number of available DF blocks */
    FDL_EEL_POOL_START,         /**< 1st block of the EEL pool */
    FDL_EEL_POOL_SIZE           /**< Number of blocks in the EEL pool */
};

