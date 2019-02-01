#ifdef ENABLE_QAC_TEST
    #pragma PRQA_MESSAGES_OFF 0292
#endif
/***********************************************************************************************************************
* Library       : Data Flash Access Library for Renesas RX111 devices, based on the MF3 Flash technology
*
* File Name     : $Source: r_fdl_user_if.c $
* Lib. Version  : $RX111_FDL_LIB_VERSION_T01: V1.01 $
* Mod. Revision : $Revision: 1.20 $
* Mod. Date     : $Date: 2013/09/03 09:50:52MESZ $
* Device(s)     : MF3 Flash based RX111 microcontroller
* Description   : FDL user interface functions
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
* MISRA Rule:   MISRA-C 2004 rule 11.3 (QAC message 0306)
* Reason:       For effective embedded programming, integer to pointer conversions are used
* Verification: The converted addresses are essential for complete code execution. Incorrect
*               conversion would result in test fails.
***********************************************************************************************************************/

/***********************************************************************************************************************
* MISRA Rule:   MISRA-C 2004 rule 14.1 (QAC message 1503)
* Reason:       This function is not used within the library, it is only used by the user
*               application.
* Verification: This function is part of the library API and therefore essential on the one hand.
*               On the other hand the QAC warning is correct, because this function is never
*               called within the library. To solve this dillemma it is neccessary and suitable
*               to disable the message.
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
/**********************************************************************************************************************
* MISRA Rule:   QAC message 2022
* Reason:       This variable is used for the library management. Initialization will be  made separately in the 
*               function R_FDL_Init.
* Verification: Initialization is made separately in function R_FDL_Init, which should be called before any other 
*               library operation is started.  
**********************************************************************************************************************/
#if   FDL_COMPILER == FDL_COMP_GCC
    #include "rx111.h"
    r_fdl_data_t g_fdl_data_str __attribute__ ((section("R_FDL_DATA")));
#elif FDL_COMPILER == FDL_COMP_GHS
#elif FDL_COMPILER == FDL_COMP_IAR
    #include "iorx111.h"
    #pragma location = "R_FDL_DATA"
    r_fdl_data_t g_fdl_data_str;
#elif FDL_COMPILER == FDL_COMP_REC
    #include "rx111_iodefine_v050.h"
    #pragma section B R_FDL_DATA
    r_fdl_data_t g_fdl_data_str;                                                    /* PRQA S 2022 */
#endif

#if   FDL_COMPILER == FDL_COMP_GCC
#elif FDL_COMPILER == FDL_COMP_GHS
#elif FDL_COMPILER == FDL_COMP_IAR
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section B B
#endif

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
FDL_STATIC void           R_FDL_HandlerEraseSuspendProcessing (void) GCC_SECTION ("R_FDL_TEXT");
FDL_STATIC r_fdl_status_t R_FDL_HandlerEraseResumeProcessing  (void) GCC_SECTION ("R_FDL_TEXT");
FDL_STATIC r_fdl_status_t R_FDL_HandlerMultiOperation (void) GCC_SECTION ("R_FDL_TEXT");
FDL_STATIC r_fdl_status_t R_FDL_WriteOpStart (const r_fdl_request_t * request_pstr) GCC_SECTION ("R_FDL_TEXT");
FDL_STATIC r_fdl_status_t R_FDL_EraseOpStart (const r_fdl_request_t * request_pstr) GCC_SECTION ("R_FDL_TEXT");
FDL_STATIC r_fdl_status_t R_FDL_CheckOpStart (const r_fdl_request_t * request_pstr) GCC_SECTION ("R_FDL_TEXT");
FDL_STATIC void           R_FDL_EndCurrentOp (const r_fdl_status_t retVal_enu,
                                              const uint32_t failAdd_u32) GCC_SECTION ("R_FDL_TEXT");

/***********************************************************************************************************************
Compiler specific section defines
***********************************************************************************************************************/


/***********************************************************************************************************************
 * Function name: R_FDL_HandlerEraseSuspendProcessing
 **********************************************************************************************************************/
/**
 * Part of the R_FDL_Handler function.
 * This function suspends an ongoing Flash erase operation
 * If the FCB is still busy with the the erase operation, the FCB is stopped and the configuration
 * is backed up. If already finished, the operation result is backed up for later use on resume
 *
 * @return      void
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC void R_FDL_HandlerEraseSuspendProcessing (void)
{
    r_fdl_flag_t   rdy;
    r_fdl_status_t stat;
    uint32_t       add;

    /* Check if the FCB is not already ready. If yes backup the result, if no Reset the FCB */
    rdy = R_FDL_FCB_ChkReady ();

    if (R_FDL_TRUE == rdy)
    {
        /* Get operation status after operation end and reset the hardware status */
        stat = R_FDL_FCB_GetStat (&add);
        g_fdl_data_str.spdResStatus_enu  = stat;
        g_fdl_data_str.spdResAddress_u32 = add;
    }
    else
    {
        R_FDL_FCB_Reset ();
        g_fdl_data_str.spdResStatus_enu = R_FDL_BUSY;
    }

    /* Backup multi operation variables for resume */
    g_fdl_data_str.fdl_reqSuspend_pstr  = g_fdl_data_str.fdl_reqInt_pstr;
    g_fdl_data_str.spdMultiOp_str       = g_fdl_data_str.multiOp_str;

    g_fdl_data_str.spdSpdRequest_enu = R_FDL_FALSE;
    g_fdl_data_str.fdlIntStatus_enu  = R_FDL_ISTAT_INIT;
} /* R_FDL_HandlerEraseSuspendProcessing */


/***********************************************************************************************************************
 * Function name: R_FDL_HandlerEraseResumeProcessing
 **********************************************************************************************************************/
/**
 * Part of the R_FDL_Handler function.
 * This function resumes a suspended Flash operation if:
 * - resume request flag set (can only be set if a function was suspended)
 * - No Flash operation is ongoing
 *
 * @param[out]  tmpStat - Status return value. Is modified, if an operation is resumed.
 * @return      handler function flow may continue after this function or handler need to exit:
 * @li          R_FDL_BUSY - operation started
 * @li          R_FDL_ERR_PROTECTION - operation blocked due to wrong access
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC r_fdl_status_t R_FDL_HandlerEraseResumeProcessing (void)
{
    uint32_t           start_addr;
    r_fdl_accessType_t access_type;
    r_fdl_status_t     status = R_FDL_BUSY;

    status = R_FDL_FCB_SwitchMode (R_FDL_FCB_MODE_PE);
    if (R_FDL_OK == status)
    {
        /* Reset the resume request flag */
        g_fdl_data_str.spdResRequest_enu = R_FDL_FALSE;

        /* Restore variables for resume */
        g_fdl_data_str.fdl_reqInt_pstr = g_fdl_data_str.fdl_reqSuspend_pstr;
        g_fdl_data_str.fdl_reqSuspend_pstr = FDL_REQUEST_POINTER_UNDEFINED;

        /* Restore the multi operation data */
        start_addr                                  = (g_fdl_data_str.spdMultiOp_str).srcAdd_u32;
        access_type                                 = (g_fdl_data_str.spdMultiOp_str).accessType_enu;
        g_fdl_data_str.multiOp_str                  = g_fdl_data_str.spdMultiOp_str;

        /* Continue with busy state even if the FCB already finished operation on suspension.
           The ..._GetStat function will use the fcbStatBackup_enu instead FCB HW status */
        g_fdl_data_str.fdlIntStatus_enu              = R_FDL_ISTAT_BUSY;
        (g_fdl_data_str.fdl_reqInt_pstr)->status_enu = R_FDL_BUSY;
        g_fdl_data_str.fcbStatBackup_enu             = g_fdl_data_str.spdResStatus_enu;
        g_fdl_data_str.fcbAddBackup_u32              = g_fdl_data_str.spdResAddress_u32;
        
        /* Restart the block erase operation only if it was not finished at suspension time */
        if (R_FDL_BUSY == g_fdl_data_str.spdResStatus_enu)
        {
            start_addr -= FDL_BLOCK_SIZE;
        }
        else
        {
            (g_fdl_data_str.multiOp_str).srcAdd_u32 += FDL_BLOCK_SIZE;
        }
        
        if (start_addr < (g_fdl_data_str.multiOp_str).srcAddEnd_u32)
        {
            /* multi operation already forwarded the block pointer to the next block */
            status = R_FDL_FCB_StartFlashOperation (start_addr, ((start_addr + FDL_BLOCK_SIZE)  - 1),
                                                    FCB_REGPAT_FSSQ_CMD_ERS, access_type);
        }
    } /* end if (R_FDL_OK == status) */

    return (status);
} /* R_FDL_HandlerEraseResumeProcessing */


/***********************************************************************************************************************
 * Function name: R_FDL_HandlerMultiOperation
 **********************************************************************************************************************/
/**
 * Part of the R_FDL_Handler function.
 * In case of multi-operations (erase multiple blocks or write multiple bytes), after end
 * of a single Flash erase/write Flash operation, the next operation is started.
 *
 * @param[in, out]  tmpStat - current operation status. Used to judge previous operation result
 *                            and is updated in case of multi-operation requiring start of new
 *                            Flash operaiton
 * @return          handler function flow may continue after this function or handler need to exit:
 * @li              R_FDL_BUSY - operation started
 * @li              R_FDL_ERR_PROTECTION - operation blocked due to wrong access
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC r_fdl_status_t R_FDL_HandlerMultiOperation (void)
{
    r_fdl_status_t status = R_FDL_OK;

    /* Check if more data to erase/write:
       - erase or write operation
       - more data to erase/write */
    if ( (R_FDL_CMD_WRITE == (g_fdl_data_str.fdl_reqInt_pstr)->command_enu) ||
         (R_FDL_CMD_ERASE == (g_fdl_data_str.fdl_reqInt_pstr)->command_enu) )
    {
        if ( (g_fdl_data_str.multiOp_str).srcAdd_u32 <= (g_fdl_data_str.multiOp_str).srcAddEnd_u32 )
        {
            if (R_FDL_CMD_WRITE == (g_fdl_data_str.fdl_reqInt_pstr)->command_enu)
            {
                status = R_FDL_FCB_StartFlashOperation ( (g_fdl_data_str.multiOp_str).dstAdd_u32,
                                                         (g_fdl_data_str.multiOp_str).srcAdd_u32,
                                                         FCB_REGPAT_FSSQ_CMD_WRT,
                                                         (g_fdl_data_str.multiOp_str).accessType_enu );

                (g_fdl_data_str.multiOp_str).srcAdd_u32 += FDL_WRITE_SIZE;
                (g_fdl_data_str.multiOp_str).dstAdd_u32 += FDL_WRITE_SIZE;
            }
            else    /* Erase */
            {
                status = R_FDL_FCB_StartFlashOperation ((g_fdl_data_str.multiOp_str).srcAdd_u32,
                                                        ((g_fdl_data_str.multiOp_str).srcAdd_u32 + FDL_BLOCK_SIZE) - 1,
                                                        FCB_REGPAT_FSSQ_CMD_ERS,
                                                        (g_fdl_data_str.multiOp_str).accessType_enu );

                (g_fdl_data_str.multiOp_str).srcAdd_u32 += FDL_BLOCK_SIZE;
            }
        } /*end if ( (g_fdl_data_str.multiOp_str).srcAdd_u32 <= (g_fdl_data_str.multiOp_str).srcAddEnd_u32 ) */
    } /*end if ( (R_FDL_CMD_WRITE == (g_fdl_data_str.fdl_reqInt_pstr)->command_enu) ||
                 (R_FDL_CMD_ERASE == (g_fdl_data_str.fdl_reqInt_pstr)->command_enu) )  */

    return (status);
} /* R_FDL_HandlerMultiOperation */


/***********************************************************************************************************************
 * Function name: R_FDL_WriteOpStart
 **********************************************************************************************************************/
/**
 * Extract information required for the write operation, do parameter checks and call the function
 * which starts the write operation.
 *
 * @param[in]  request_pstr - pointer to the operation request structure
 *
 * @return     Parameter check result
 * @li         R_FDL_BUSY
 * @li         R_FDL_ERR_PARAMETER
 * @li         R_FDL_ERR_PROTECTION
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC r_fdl_status_t R_FDL_WriteOpStart (const r_fdl_request_t * request_pstr)
{
    r_fdl_status_t     status;
    r_fdl_accessType_t access_type;
    uint32_t           source_addr;
    uint32_t           dest_addr;
    uint32_t           byte_cnt;

    source_addr = request_pstr->dataAdd_u32;
    dest_addr   = request_pstr->idx_u32;
    byte_cnt    = (request_pstr->cnt_u16) * FDL_WRITE_SIZE;
    access_type = request_pstr->accessType_enu;

    /* Prepare for multi-byte write */
    (g_fdl_data_str.multiOp_str).srcAdd_u32     = source_addr + FDL_WRITE_SIZE;     /* Next Source Address */
    (g_fdl_data_str.multiOp_str).srcAddEnd_u32  = (source_addr + byte_cnt) - 1uL;   /* Source End Address  */
    (g_fdl_data_str.multiOp_str).dstAdd_u32     = dest_addr + FDL_WRITE_SIZE;       /* Next Dest. Address  */
    (g_fdl_data_str.multiOp_str).accessType_enu = request_pstr->accessType_enu;

    status = R_FDL_ChkAccessBoundaries (dest_addr, byte_cnt, access_type);
    if (R_FDL_BUSY == status)
    {
        status = R_FDL_FCB_SwitchMode (R_FDL_FCB_MODE_PE);
        if (R_FDL_OK == status)
        {
            status = R_FDL_FCB_StartFlashOperation (dest_addr, source_addr, FCB_REGPAT_FSSQ_CMD_WRT, access_type);
        }
    }

    return (status);
} /* R_FDL_WriteOpStart */


/***********************************************************************************************************************
 * Function name: R_FDL_EraseOpStart
 **********************************************************************************************************************/
/**
 * Extract information required for the Erase operation , do access parameter check
 * and call the function which starts the Erase operation.
 *
 * @param[in]  request_pstr - pointer to the operation request structure
 * @return     Parameter check result
 * @li         R_FDL_BUSY
 * @li         R_FDL_ERR_PARAMETER
 * @li         R_FDL_ERR_PROTECTION
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC r_fdl_status_t R_FDL_EraseOpStart (const r_fdl_request_t * request_pstr)
{
    r_fdl_status_t     status;
    r_fdl_accessType_t access_type;
    uint32_t           start_addr;
    uint32_t           byte_cnt;

    access_type = request_pstr->accessType_enu;
    start_addr  = ( (request_pstr->idx_u32) << FDL_BLOCK_SIZE_2N );
    byte_cnt    = ( (request_pstr->cnt_u16) << FDL_BLOCK_SIZE_2N );

    /* Prepare for multi-block erase */
    (g_fdl_data_str.multiOp_str).srcAdd_u32     = start_addr + FDL_BLOCK_SIZE;   /* Next Erase Address */
    (g_fdl_data_str.multiOp_str).srcAddEnd_u32  = (start_addr + byte_cnt) - 1uL; /* (multi-block)Erase End Address */
    (g_fdl_data_str.multiOp_str).accessType_enu = access_type;

    status = R_FDL_ChkAccessBoundaries (start_addr, byte_cnt, access_type);
    if (R_FDL_BUSY == status)
    {
        status = R_FDL_FCB_SwitchMode (R_FDL_FCB_MODE_PE);
        if (R_FDL_OK == status)
        {
            status = R_FDL_FCB_StartFlashOperation (start_addr, 
                                                    (start_addr + FDL_BLOCK_SIZE) - 1, 
                                                    FCB_REGPAT_FSSQ_CMD_ERS, 
                                                    access_type);
        }
    }

    return (status);
} /* R_FDL_EraseOpStart */


/***********************************************************************************************************************
 * Function name: R_FDL_CheckOpStart
 **********************************************************************************************************************/
/**
 * Extract information required for the BC operation , do access parameter check
 * and call the function which starts the BC operation.
 *
 * @param[in]  request_pstr - pointer to the operation request structure
 * @return     Parameter check result
 * @li         R_FDL_BUSY
 * @li         R_FDL_ERR_PARAMETER
 * @li         R_FDL_ERR_PROTECTION
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC r_fdl_status_t R_FDL_CheckOpStart (const r_fdl_request_t * request_pstr)
{
    r_fdl_status_t     status;
    r_fdl_accessType_t access_type;
    uint32_t           start_addr;
    uint32_t           end_addr;
    uint32_t           byte_cnt;
    uint16_t           fcu_cmd;

    access_type = request_pstr->accessType_enu;
    start_addr  = request_pstr->idx_u32;
    byte_cnt    = (request_pstr->cnt_u16) * FDL_WRITE_SIZE;
    end_addr    = (start_addr + byte_cnt) - 1uL;

    fcu_cmd = FCB_REGPAT_FSSQ_CMD_BC;

    status = R_FDL_ChkAccessBoundaries (start_addr, byte_cnt, access_type);
    if (R_FDL_BUSY == status)
    {
        status = R_FDL_FCB_SwitchMode (R_FDL_FCB_MODE_PE);
        if (R_FDL_OK == status)
        {
            status = R_FDL_FCB_StartFlashOperation (start_addr, end_addr, fcu_cmd, access_type);
        }
    }

    return (status);
} /* R_FDL_CheckOpStart */


/***********************************************************************************************************************
 * Function name: R_FDL_EndCurrentOp
 **********************************************************************************************************************/
/**
 * At the end of a requested operation, this function updates the request structure elements and
 * resets the library internal pointer to this structure.
 *
 * @param[in]  retVal_enu  - status return value
 *             failAdd_u32 - address where blankcheck command failed
 * @return     void
 *
 * The function modifies the global structure g_fdl_data_str.fdl_reqInt_pstr to set the operation
 * status
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
FDL_STATIC void R_FDL_EndCurrentOp (const r_fdl_status_t retVal_enu, const uint32_t failAdd_u32)
{
    (g_fdl_data_str.fdl_reqInt_pstr)->status_enu = retVal_enu;

    /* Reset the access type. In case of suspended, the access is reset at real operation end */
    if (R_FDL_SUSPENDED != retVal_enu)
    {
        (g_fdl_data_str.fdl_reqInt_pstr)->accessType_enu = R_FDL_ACCESS_NONE;
    }

    /* check operations need the operations fail address in case of operation fail */
    if (R_FDL_ERR_BLANKCHECK == retVal_enu)
    {
        (g_fdl_data_str.fdl_reqInt_pstr)->idx_u32 = failAdd_u32;
    }

    g_fdl_data_str.fdl_reqInt_pstr  = FDL_REQUEST_POINTER_UNDEFINED;
    g_fdl_data_str.fdlIntStatus_enu = R_FDL_ISTAT_INIT;

    (void)R_FDL_FCB_SwitchMode (R_FDL_FCB_MODE_USER);
} /* R_FDL_EndCurrentOp */


/***********************************************************************************************************************
        Public interface functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Function name: R_FDL_Init
 **********************************************************************************************************************/
/**
 * User interface function:
 *
 * This function initializes the FDL. It must be called before any execution of a FDL function
 * with a properly set descriptor.
 *
 * In error case the library is set to not initialized (R_FDL_ISTAT_NOINIT)
 *
 * @param[in]  descriptor_pstr - Pointer to the FDL descriptor structure
 * @return     Initialization status when returned from function call:
 * @li         R_FDL_OK - Initialization was successful
 * @li         R_FDL_ERR_PARAMETER - FDL configuration parameter error or
 *                                   parameters are not properly set for speed operating mode
 * @li         R_FDL_ERR_PROTECTION - no data flash on the device or low speed operation mode
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
r_fdl_status_t R_FDL_Init (const r_fdl_descriptor_t * descriptor_pstr)              /* PRQA S 1503 */
{
    uint16_t fclk_freq;
    uint16_t df_size;
    uint16_t block_end;
    uint32_t i;
    uint32_t fdl_addr;

    /* Initially clear the complete fdl_data structure */
    fdl_addr = (uint32_t)(&g_fdl_data_str);                                         /* PRQA S 0306 */

    for (i = 0; i < sizeof (g_fdl_data_str); i++)
    {
        R_FDL_WriteMemoryU08 (fdl_addr, 0u);
        fdl_addr++;
    }

    g_fdl_data_str.fdlIntStatus_enu = R_FDL_ISTAT_NOINIT;

    /* Parameter independent initializations */
    g_fdl_data_str.fdl_reqSuspend_pstr = FDL_REQUEST_POINTER_UNDEFINED;
    g_fdl_data_str.fdl_reqInt_pstr     = FDL_REQUEST_POINTER_UNDEFINED;
    g_fdl_data_str.spdSpdRequest_enu   = R_FDL_FALSE;
    g_fdl_data_str.spdResRequest_enu   = R_FDL_FALSE;
    g_fdl_data_str.fcbStatBackup_enu   = R_FDL_BUSY;

    /* Reset debug patch */
    #ifdef PATCH_TO_SIMULATE_ERRORS
        g_fdl_data_str.simError_enu    = R_FDL_FALSE;
    #endif

    /* Initializations with parameter check */
    if (FDL_DESCRIPTOR_POINTER_UNDEFINED != descriptor_pstr)
    {
        /* Set pointer to FDL static configuration */
        g_fdl_data_str.RTCfg_pstr = descriptor_pstr;
    }
    else
    {
        return (R_FDL_ERR_PARAMETER);
    }

    /* find Data Flash size */
    df_size = DF_SIZE_BITS;                                                         /* PRQA S 0303 */
    if (0 != df_size)
    {
        /* 4, 8 or 16 KB are valid */
        df_size = (uint16_t)( 4 << (df_size - 1) );
    }
    else
    {
        return (R_FDL_ERR_PROTECTION);
    }

    /* check FDL pool not to exceed physically available Data Flash */
    if ( ( (g_fdl_data_str.RTCfg_pstr)->fdlPoolSize_u16 ) > df_size )
    {
        /* data flash size verification failed */
        return (R_FDL_ERR_PARAMETER);
    }

    /* Check EEL Flash window boundaries against Flash size */
    if (0u != (g_fdl_data_str.RTCfg_pstr)->fdlPoolSize_u16)
    {
        block_end = ( (g_fdl_data_str.RTCfg_pstr)->eelPoolStart_u16 ) +
                    ( (g_fdl_data_str.RTCfg_pstr)->eelPoolSize_u16 );

        if ( block_end >= ( (g_fdl_data_str.RTCfg_pstr)->fdlPoolSize_u16 + 1u ) )
        {
            return (R_FDL_ERR_PARAMETER);
        }
    }
    else
    {
        return (R_FDL_ERR_PARAMETER);
    }

    /* calculate FCK from given ICK */
    fclk_freq = (g_fdl_data_str.RTCfg_pstr)->fdlFrequencyCpuMHz_u16;
    fclk_freq <<= (PRESCALER_ICK);                                                  /* PRQA S 0303 */
    fclk_freq >>= (PRESCALER_FCK);                                                  /* PRQA S 0303 */
    if (0 != SYSTEM_MODE_LO)                                                        /* PRQA S 0303 */
    {
        /* FDL does not run in low speed power mode */
        return (R_FDL_ERR_PROTECTION);
    }
    else /* SYSTEM_MODE_LO = 0 */
    {
        /* medium of high speed power modes */
        if (fclk_freq >= FDL_FCBCLK_MIN_MHz)
        {
            if (SYSTEM_MODE_HIGH_SPEED == SYSTEM_MODE_MD_HI)                        /* PRQA S 0303 */
            {
                /* high speed */
                if (fclk_freq > FDL_FCBCLK_MAX_HIV_MHz)
                {
                    return (R_FDL_ERR_PARAMETER);
                }
            }
            else if (SYSTEM_MODE_MEDIUM_SPEED == SYSTEM_MODE_MD_HI)                 /* PRQA S 0303 */
            {
                /* medium speed */
                if (fclk_freq > FDL_FCBCLK_MAX_MDV_MHz)
                {
                    return (R_FDL_ERR_PARAMETER);
                }
            }
            else
            {
                /* operating mode bits (OPCM and SOPCM) are not set properly */
                return (R_FDL_ERR_PARAMETER);
            }
        } /* end if (fclk_freq >= FDL_FCBCLK_MIN_MHz) */
        else
        {
            return (R_FDL_ERR_PARAMETER);
        }
    } /* if (0 != SYSTEM_MODE_LO) - else */

    /* replace CPU freq with FCB frequency. CPU freq is not needed anymore */
    g_fdl_data_str.fcb_freq_Mhz = (uint8_t)fclk_freq;

    /* Reset the Flash environment for the case of EEPROM emulation after a "warm Reset" */
    R_FDL_FCB_Reset ();

    /* Set library init status  */
    g_fdl_data_str.fdlIntStatus_enu = R_FDL_ISTAT_INIT;

    return (R_FDL_OK);
} /* R_FDL_Init */


/***********************************************************************************************************************
 * Function name: R_FDL_Execute
 **********************************************************************************************************************/
/**
 * User interface function:
 * This function initiates a Flash operation (Erase/Write/...).
 * After initiation, the function R_FDL_Handler needs to be called regularely to finish the operation.
 *
 * @param[in, out] request_pstr   Pointer to the operation request structure.
 *                                - Structure input elements: \n
 *                                  command_enu, dataAdd_u32, idx_u32, cnt_u16, accessType_enu
 *                                - Structure output element: \n
 *                                  status_enu
 * @return      void
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
void  R_FDL_Execute (r_fdl_request_t * request_pstr)                           /* PRQA S 1503 */
{
    r_fdl_status_t fdl_req_status = R_FDL_ERR_REJECTED;

    /* New commands accepted if the library is idle and a suspended operation shall not be resumed */
    if ( (R_FDL_ISTAT_INIT == g_fdl_data_str.fdlIntStatus_enu) &&
         (R_FDL_FALSE      == g_fdl_data_str.spdResRequest_enu) )
    {
        /* Currently no operation ongoing */
        switch (request_pstr->command_enu)
        {
            case R_FDL_CMD_ERASE:
            {
                fdl_req_status = R_FDL_EraseOpStart (request_pstr);
                break;
            }

            case R_FDL_CMD_BLANKCHECK:
            {
                fdl_req_status = R_FDL_CheckOpStart (request_pstr);
                break;
            }

            case R_FDL_CMD_WRITE:
            {
                fdl_req_status = R_FDL_WriteOpStart (request_pstr);
                break;
            }

            default:
            {
                fdl_req_status = R_FDL_ERR_COMMAND;
                break;
            }
        } /* switch */

    } /* end if ( (R_FDL_ISTAT_INIT == g_fdl_data_str.fdlIntStatus_enu) &&
                  (R_FDL_FALSE      == g_fdl_data_str.spdResRequest_enu) )  */

    if (R_FDL_BUSY == fdl_req_status)
    {
        /* set internal pointer to the request structure, required by the handler */
        g_fdl_data_str.fdl_reqInt_pstr  = request_pstr;
        g_fdl_data_str.fdlIntStatus_enu = R_FDL_ISTAT_BUSY;
    }
    else
    {
        /* At operations end (In error case or if the operation requires no handler calls), reset
           the access type */
        request_pstr->accessType_enu = R_FDL_ACCESS_NONE;
    }

    /* Set the return state */
    request_pstr->status_enu = fdl_req_status;
} /* R_FDL_Execute */


/***********************************************************************************************************************
 * Function name: R_FDL_Handler
 **********************************************************************************************************************/
/**
 * User interface function:
 * This function forwards an initiated Flash operation until it is finished. At operation end the
 * result is set in the request structure.
 * After initiation by R_FDL_Execute, the function R_FDL_Handler need to be called regularly to finish
 * the operation.
 *
 * @param          void
 * @return         void
 *
 * The function returns the operation result in the status variable, passed to
 * the R_FDL_Execute variable (request.status_enu)
 */
/**********************************************************************************************************************/

/***********************************************************************************************************************
* MISRA Rule:   MISRA-C 2004 rule 1.2 (QAC message 0505)
* Reason:       QAC claims referencing a possibly undefined pointer.
* Verification: Reviewed, that this is not possible. The pointer is always defined
***********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
void R_FDL_Handler (void)                                                      /* PRQA S 1503 */
{
    uint32_t       fail_addr = 0;
    r_fdl_flag_t   rdy;
    r_fdl_status_t stat = R_FDL_BUSY;

    /* Check if the handler has something to do */
    if ( (R_FDL_ISTAT_BUSY == g_fdl_data_str.fdlIntStatus_enu) ||
         ( (R_FDL_ISTAT_INIT == g_fdl_data_str.fdlIntStatus_enu) &&
           (R_FDL_TRUE == g_fdl_data_str.spdResRequest_enu) ) )
    {

        /* Suspend / Resume management */
        if (R_FDL_TRUE == g_fdl_data_str.spdSpdRequest_enu)
        {
            /* Erase operation is suspended ... now we are idle */
            R_FDL_HandlerEraseSuspendProcessing ();
            stat = R_FDL_SUSPENDED;
        }
        else if (R_FDL_TRUE == g_fdl_data_str.spdResRequest_enu)
        {
            /* Erase processing is resumed or FCB result backup is set */
            stat = R_FDL_HandlerEraseResumeProcessing ();
        }
        /* Normal operation */
        else
        {
            rdy = R_FDL_FCB_ChkReady ();
            if (R_FDL_TRUE == rdy)
            {
                stat = R_FDL_FCB_GetStat (&fail_addr);
                if (R_FDL_OK == stat)
                {
                    /* Multi operation (Write/Erase) state machine */
                    stat = R_FDL_HandlerMultiOperation ();
                }
            }
        }

        /* Normal operation end */
        if (R_FDL_BUSY != stat)
        {
            R_FDL_EndCurrentOp (stat, fail_addr);
        }
    } /* end check if handler has something to do */
#ifdef PATCH_TO_SIMULATE_ERRORS
    else
    {
        g_fdl_data_str.simErrorCnt_u32++;
    }
#endif

    
} /* R_FDL_Handler */


/***********************************************************************************************************************
 * Function name: R_FDL_EraseSuspendRequest
 **********************************************************************************************************************/
/**
 * User interface function:
 * Sets the suspend request flag for the an ongoing erase operation
 *
 * Note:
 * The operation is not immediately suspended. Suspend handling is controlled by R_FDL_Handler.
 *
 * Suspend request handling:
 * - Reject if
 *     - we have no active Flash erase operation
 *     - we have already a Flash operation suspended
 * - Reset resume request if set (then we have a not yet handled resume request)
 * - Set suspend request if we have an operation ongoing and we are not already in suspend
 *   processing
 *
 * @param      void
 * @return     result of the suspend request
 * @li         R_FDL_OK           - Request accepted
 * @li         R_FDL_ERR_REJECTED - Request rejected
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
r_fdl_status_t R_FDL_EraseSuspendRequest (void)                                /* PRQA S 1503 */
{
    r_fdl_status_t status = R_FDL_ERR_REJECTED;

    /* library internal state "resume request" (idle & resume request set) */
    if ( (R_FDL_ISTAT_INIT == g_fdl_data_str.fdlIntStatus_enu) &&
         (R_FDL_TRUE       == g_fdl_data_str.spdResRequest_enu) )
    {
        g_fdl_data_str.spdResRequest_enu = R_FDL_FALSE;
        status = R_FDL_OK;
    }
    else
    {
        /* library internal state "busy" (busy & no suspended erase operation) */
        if ( (R_FDL_ISTAT_BUSY              == g_fdl_data_str.fdlIntStatus_enu)    &&
             (FDL_REQUEST_POINTER_UNDEFINED == g_fdl_data_str.fdl_reqSuspend_pstr) && 
             (R_FDL_TRUE != g_fdl_data_str.spdSpdRequest_enu)                         )
        {
            if (R_FDL_CMD_ERASE == (g_fdl_data_str.fdl_reqInt_pstr)->command_enu)
            {
                g_fdl_data_str.spdSpdRequest_enu = R_FDL_TRUE;
                status = R_FDL_OK;
            }
        }
    }

    return (status);
} /* R_FDL_EraseSuspendRequest */


/***********************************************************************************************************************
 * Function name: R_FDL_EraseResumeRequest
 **********************************************************************************************************************/
/**
 * User interface function:
 * Sets the resume request flag on a suspended erase operation
 *
 * Note:
 * The operation is not immediately resumed. Resume handling is controlled by R_FDL_Handler.
 *
 * Resume request handling:
 * - Reset suspend request if set (then we have a not yet handled suspend request)
 * - Set resume request if we have a suspended operation ( fdl_reqSuspend_pstr defined )
 *   and we have not already a resume request
 * - Reject all other conditions
 *
 * @param      void
 * @return     result of the resume request
 * @li         R_FDL_OK           - Request accepted
 * @li         R_FDL_ERR_REJECTED - Request rejected
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
r_fdl_status_t R_FDL_EraseResumeRequest (void)                                 /* PRQA S 1503 */
{
    r_fdl_status_t status = R_FDL_ERR_REJECTED;

    /* library internal state "suspend request" (busy & suspend request set) */
    if ( (R_FDL_ISTAT_BUSY == g_fdl_data_str.fdlIntStatus_enu) &&
         (R_FDL_TRUE       == g_fdl_data_str.spdSpdRequest_enu) )
    {
        g_fdl_data_str.spdSpdRequest_enu = R_FDL_FALSE;
        status = R_FDL_OK;
    }
    else
    {
        /* library internal state "erase suspended" (idle & suspendedn & no resume request set) */
        if ( (R_FDL_ISTAT_INIT == g_fdl_data_str.fdlIntStatus_enu) &&
             ( (FDL_REQUEST_POINTER_UNDEFINED != g_fdl_data_str.fdl_reqSuspend_pstr) &&
               (R_FDL_FALSE                   == g_fdl_data_str.spdResRequest_enu) ) )
        {
            g_fdl_data_str.spdResRequest_enu = R_FDL_TRUE;
            status = R_FDL_OK;
        }
    }

    return (status);
} /* R_FDL_EraseResumeRequest */


/***********************************************************************************************************************
 * Function name: R_FDL_StandBy
 **********************************************************************************************************************/
/**
 * User interface function:
 * Prepares the HW for fast standby entry:
 * - on erase --> stop operation and backup FCB configuration for later restart on R_FDL_WakeUp
 * - other operations are not suspendable, here the function need to be called until the FCB
 *   finished the operation
 *
 * This function can be called asynchronous (e.g. interrupt function) and so need to take
 * care of the current hardware status in order to restore it later on during wakeup
 *
 * Note:
 * the function need to be called continuously until the result is != R_FDL_BUSY
 *
 * @param      void
 * @return     result of the standby operation
 * @li         R_FDL_OK           - Flash hardware is in standby or ready (no more Flash operation)
 * @li         R_FDL_BUSY         - Flash hardware is still busy. Continue by calling the R_FDL_Handler function
 * @li         R_FDL_ERR_REJECTED - Rejected, as the library is not initialized
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
r_fdl_status_t R_FDL_StandBy (void)                                            /* PRQA S 1503 */
{
    r_fdl_status_t status;
    r_fdl_flag_t   rdy;

    if (R_FDL_ISTAT_INIT == g_fdl_data_str.fdlIntStatus_enu)
    {
        g_fdl_data_str.stdByIntStatus_enu = g_fdl_data_str.fdlIntStatus_enu;
        g_fdl_data_str.fdlIntStatus_enu   = R_FDL_ISTAT_STANDBY;
        status = R_FDL_OK;
    }
    else
    {
        if (R_FDL_ISTAT_BUSY == g_fdl_data_str.fdlIntStatus_enu)
        {
            /* Set to standby processing */
            g_fdl_data_str.stdByIntStatus_enu = g_fdl_data_str.fdlIntStatus_enu;
            g_fdl_data_str.fdlIntStatus_enu   = R_FDL_ISTAT_STANDBYPROC;
        }

        if (R_FDL_ISTAT_STANDBYPROC == g_fdl_data_str.fdlIntStatus_enu)
        {
            g_fdl_data_str.stdByEraseStopped_enu = R_FDL_FALSE;
            status = R_FDL_BUSY;

            /* stop an ongoing erase and wait for end of any other operation */
            rdy = R_FDL_FCB_ChkReady ();
            if (R_FDL_FALSE == rdy)
            {
                /* hardware is still busy and does an erase */
                if (R_FDL_CMD_ERASE == (g_fdl_data_str.fdl_reqInt_pstr)->command_enu)
                {
                    g_fdl_data_str.stdByEraseAdd_u32  = R_FDL_FCB_GetOPAdd ();
                    g_fdl_data_str.stdByEraseAdd_u32 -= FDL_DF_BASE_ADD;
                    g_fdl_data_str.stdByEraseStopped_enu = R_FDL_TRUE;

                    R_FDL_FCB_Reset ();
                    (void)R_FDL_FCB_SwitchMode (R_FDL_FCB_MODE_USER);

                    g_fdl_data_str.fdlIntStatus_enu = R_FDL_ISTAT_STANDBY;
                    status = R_FDL_OK;
                }
            } /* end if (R_FDL_FALSE == rdy) */
            else
            {
                (void)R_FDL_FCB_SwitchMode (R_FDL_FCB_MODE_USER);
                g_fdl_data_str.fdlIntStatus_enu = R_FDL_ISTAT_STANDBY;
                status = R_FDL_OK;
            }
            
            #ifdef PATCH_TO_SIMULATE_ERRORS
                g_fdl_data_str.sequencerReady_enu = rdy;
            #endif
        } /* end if (R_FDL_ISTAT_STANDBYPROC == g_fdl_data_str.fdlIntStatus_enu) */
        else
        {
            /* internal status is not initialized */
            status = R_FDL_ERR_REJECTED;
        }
    } /* if (R_FDL_ISTAT_INIT == g_fdl_data_str.fdlIntStatus_enu) - else */

    return (status);
} /* R_FDL_StandBy */


/***********************************************************************************************************************
 * Function name: R_FDL_WakeUp
 **********************************************************************************************************************/
/**
 * User interface function:
 * Wake up the hardware from standby.
 * Depending on the FCB status on standby:
 * - Stopped Erase operation is restarted
 * - On finished write/blankcheck operation just return to programming mode
 * - On no operation ongoing... do nothing
 *
 * @param      void
 * @return     result of the initialization check
 * @li         R_FDL_OK           - Wakeup succeeded
 * @li         R_FDL_ERR_REJECTED - Rejected, as the library is not initialized
 * @li         R_FDL_ERR_PROTECTION - Protection error on erase restart
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
#endif
r_fdl_status_t R_FDL_WakeUp (void)                                             /* PRQA S 1503 */
{
    r_fdl_status_t status;
    uint32_t       erase_addr;

    /* We can only wakeup if we have been in standby before */
    if (R_FDL_ISTAT_STANDBY == g_fdl_data_str.fdlIntStatus_enu)
    {
        status = R_FDL_OK;
        g_fdl_data_str.fdlIntStatus_enu = g_fdl_data_str.stdByIntStatus_enu;

        /* If we were in PE mode @ standby, we need to return to that mode at first */
        if (R_FDL_ISTAT_BUSY == g_fdl_data_str.stdByIntStatus_enu)
        {
            status = R_FDL_FCB_SwitchMode (R_FDL_FCB_MODE_PE);
        }

        if (R_FDL_OK == status)
        {
            /* If we had a not finished erase @ standby, we will restart it */
            if (R_FDL_TRUE == g_fdl_data_str.stdByEraseStopped_enu)
            {
                erase_addr = g_fdl_data_str.stdByEraseAdd_u32;
                status = R_FDL_FCB_StartFlashOperation ( erase_addr, 
                                                         ((erase_addr + FDL_BLOCK_SIZE) - 1), 
                                                         FCB_REGPAT_FSSQ_CMD_ERS,
                                                         (g_fdl_data_str.fdl_reqInt_pstr)->accessType_enu );
                if (R_FDL_BUSY == status) /* Patch return value to function result OK */
                {
                    status = R_FDL_OK;
                }
            }
        }
    } /* end if (R_FDL_ISTAT_STANDBY == g_fdl_data_str.fdlIntStatus_enu) */
    else
    {
        status = R_FDL_ERR_REJECTED;
    }

    return (status);
} /* R_FDL_WakeUp */


/***********************************************************************************************************************
 * Function name: R_FDL_GetVersionString
 **********************************************************************************************************************/
/**
 * User interface function:
 * Returns the pointer to the library version string, stores in the library code section.
 *
 * @param      void
 * @return     pointer to the version string in Flash
 */
/**********************************************************************************************************************/
#if FDL_COMPILER == FDL_COMP_IAR
    #pragma location = "R_FDL_TEXT"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section P R_FDL_TEXT
    #pragma section C R_FDL_CONST
#endif
const uint8_t *R_FDL_GetVersionString (void)                                  /* PRQA S 1503 */
{
    #if FDL_COMPILER == FDL_COMP_GCC
        static const uint8_t fdl_ver_str[] GCC_SECTION ("R_FDL_CONST") = R_FDL_LIBRARY_VERSION;

    #elif FDL_COMPILER == FDL_COMP_IAR
        static const uint8_t fdl_ver_str[] @ "R_FDL_CONST" = R_FDL_LIBRARY_VERSION;

    #elif FDL_COMPILER == FDL_COMP_REC
        static const uint8_t fdl_ver_str[] = R_FDL_LIBRARY_VERSION;

    #endif

    return (&fdl_ver_str[0]);
}

