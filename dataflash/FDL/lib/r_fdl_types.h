#ifdef ENABLE_QAC_TEST
    #pragma PRQA_MESSAGES_OFF 0292
#endif
/***********************************************************************************************************************
* Library       : Data Flash Access Library for Renesas RX111 devices, based on the MF3 Flash technology
*
* File Name     : $Source: r_fdl_types.h $
* Lib. Version  : $RX111_FDL_LIB_VERSION_T01: V1.01 $
* Mod. Revision : $Revision: 1.6 $
* Mod. Date     : $Date: 2013/06/25 10:52:13MESZ $
* Device(s)     : MF3 Flash based RX111 microcontroller
* Description   : User interface type definitions
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

#ifndef R_FDL_TYPES_H
#define R_FDL_TYPES_H

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
    Enumerations / Structures - Configuration
***********************************************************************************************************************/

#if (defined __IAR_SYSTEMS_ICC__)
    #pragma pack (push)
#elif (defined __RX200)
    #pragma unpack
#endif

/**
 * @struct r_fdl_descriptor_t
 * FDL descriptor variable definition.
 */
typedef struct R_FDL_DESCRIPTOR_T
{
    uint16_t    fdlFrequencyCpuMHz_u16;   /**< Flash hardware frequency in MHz */
    uint16_t    fdlPoolSize_u16;          /**< FDL pool size in no. of blocks */
    uint16_t    eelPoolStart_u16;         /**< 1st block of the EEL pool */
    uint16_t    eelPoolSize_u16;          /**< Last block of the EEL pool */
} r_fdl_descriptor_t;


/***********************************************************************************************************************
    Enumerations / Structures - Operation
***********************************************************************************************************************/
/**
 * @enum r_fdl_status_t
 * FDL status return values
 */
typedef enum R_FDL_STATUS_T
{
    R_FDL_OK,                             /**< Operation terminated successfully */
    R_FDL_BUSY,                           /**< Operation is still ongoing */
    R_FDL_SUSPENDED,                      /**< Flash operation is suspended */
    R_FDL_ERR_PARAMETER,                  /**< A parameter of the FDL function call was wrong */
    R_FDL_ERR_PROTECTION,                 /**< Operation blocked due to wrong parameters */
    R_FDL_ERR_REJECTED,                   /**< Flow error, e.g. another operation is still busy */
    R_FDL_ERR_WRITE,                      /**< Flash write error */
    R_FDL_ERR_ERASE,                      /**< Flash erase error */
    R_FDL_ERR_BLANKCHECK,                 /**< Flash blank check error */
    R_FDL_ERR_COMMAND,                    /**< Unknown command */
    R_FDL_ERR_INTERNAL                    /**< Library internal error */
} r_fdl_status_t;


/**
 * @enum r_fdl_accessType_t
 * FDL operations originator defines.
 * Set by the user when starting an FDL operation.
 * Reset to ..._NONE by the library on operation end.
 */
typedef enum R_FDL_ACCESS_TYPE_T
{
    R_FDL_ACCESS_NONE,                    /**< FDL internal value. Not used by the application */
    R_FDL_ACCESS_USER,                    /**< User application wants to execute an FDL operation */
    R_FDL_ACCESS_EEL                      /**< The EEL wants to execute an FDL operation */
} r_fdl_accessType_t;


/**
 * @enum r_fdl_command_t
 * FDL operation commands
 */
typedef enum R_FDL_COMMAND_T
{
    R_FDL_CMD_ERASE,                      /**< Flash block erase (Multiple blocks) */
    R_FDL_CMD_WRITE,                      /**< Flash write (Multiple bytes) */
    R_FDL_CMD_BLANKCHECK                  /**< Flash blank check command (Multiple bytes) */
} r_fdl_command_t;


/**
 * @struct r_fdl_request_t
 * FDL operations request structure, required for R_FDL_Execute
 */
typedef volatile struct R_FDL_REQUEST_T
{
    r_fdl_command_t     command_enu;      /**< FDL operation, e.g. R_FDL_CMD_ERASE */
    uint32_t            dataAdd_u32;      /**< Source data address when performing a Write operation */
    uint32_t            idx_u32;          /**< Write operation: start address to write to \n
                                               Erase operation: block number of the first block to erase \n
                                               Blank Check operation: address of the first byte to check
                                               Internal Verify: address of the first byte to verify */
    uint16_t            cnt_u16;          /**< Write operation: number of byets to be written \n
                                               Erase operation: number of blocks to be erased \n
                                               Blank Check operation: number of byets to check \n
                                               Internal Verify: number of bytes to check */
    r_fdl_accessType_t  accessType_enu;   /**< Data Flash access originator: \n
                                               R_FDL_ACCESS_USER: User code \n
                                               R_FDL_ACCESS_EEL:  EEL library */
    r_fdl_status_t      status_enu;       /**< status return value of the FDL */
} r_fdl_request_t;

#if (defined __IAR_SYSTEMS_ICC__)
    #pragma pack (pop)
#elif (defined __RX200)
    #pragma packoption
#endif

#endif /* ifndef R_FDL_TYPES_H */

