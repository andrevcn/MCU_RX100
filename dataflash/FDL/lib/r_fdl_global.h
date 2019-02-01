#ifdef ENABLE_QAC_TEST
    #pragma PRQA_MESSAGES_OFF 0292
#endif
/***********************************************************************************************************************
* Library       : Data Flash Access Library for Renesas RX111 devices, based on the MF3 Flash technology
*
* File Name     : $Source: r_fdl_global.h $
* Lib. Version  : $RX111_FDL_LIB_VERSION_T01: V1.01 $
* Mod. Revision : $Revision: 1.17 $
* Mod. Date     : $Date: 2013/09/03 14:06:11MESZ $
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

#ifndef R_FDL_GLOBAL_H
#define R_FDL_GLOBAL_H


/* Library version string */
#ifndef PRECOMPILED
    #define R_FDL_LIBRARY_VERSION           "DX100T01xxxxxxV101"
#else
    #ifndef PRECOMPILED_LIB_VER
        #error "PRECOMPILED_LIB_VER is not defined. Aborting build."
    #endif

    #define STRINGIZE(str)                  #str
    #define STRINGIZE_VALUE_OF(x)           STRINGIZE(x)

    #define R_FDL_LIBRARY_VERSION           STRINGIZE_VALUE_OF(PRECOMPILED_LIB_VER)
#endif

/* Static functions definition */
#ifndef FDL_STATIC
/* GCC does not place static functions into sections specified by __attribute__ suffix */
    #ifndef __GNUC__
        #define FDL_STATIC                      static
    #else
        #define FDL_STATIC
    #endif
#endif

/* Reset value of request pointers */
#define FDL_REQUEST_POINTER_UNDEFINED           (r_fdl_request_t *)(0x00000000uL)
#define FDL_DESCRIPTOR_POINTER_UNDEFINED     (r_fdl_descriptor_t *)(0x00000000uL)


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Library internal enumerations definitions */

/**
 * @enum r_fdl_flag_t
 * standard FDL internal flag definition
 */
typedef enum R_FDL_FLAG_T
{
    R_FDL_FALSE,                                         /**< Flag false */
    R_FDL_TRUE                                           /**< Flag true */
} r_fdl_flag_t;


typedef enum R_FDL_FCB_MODE_T
{
    R_FDL_FCB_MODE_USER,
    R_FDL_FCB_MODE_PE
} r_fdl_fcb_mode_t;

/**
 * @enum r_fdl_int_status_t
 * definition of the FDL operational status
 */
typedef enum R_FDL_INT_STATUS_T
{
    R_FDL_ISTAT_NOINIT          = 0x00000000uL,          /**< Pattern for uninitialized library (0x00000000) */
    R_FDL_ISTAT_INIT            = 0x55555555uL,          /**< Pattern for initialized/idle library */
    R_FDL_ISTAT_BUSY            = 0x55555556uL,          /**< Pattern for a busy library*/
    R_FDL_ISTAT_STANDBYPROC     = 0x55555557uL,          /**< Pattern for library in standby processing */
    R_FDL_ISTAT_STANDBY         = 0x55555558uL           /**< Pattern for library in standby */
} r_fdl_int_status_t;


#ifdef PATCH_TO_SIMULATE_ERRORS
/* Type definition for simulated errors */
typedef enum R_FDL_SIM_ERROR_TYPE_T
{
    R_FDL_SIM_ERROR_ERASE,                               /* Simulate erase error */
    R_FDL_SIM_ERROR_WRITE,                               /* Simulate write error */
    R_FDL_SIM_ERROR_BITCHECK,                            /* Simulate bit check error */
    R_FDL_SIM_ERROR_PROTECTION,                          /* Simulate protection error */
    R_FDL_SIM_FCB_TIMEOUT,                               /* Simulate FCB timeout (FRDY=0) */
    R_FDL_SIM_ERROR_FATALERROR,                          /* Simulate fatal FCB error */
    R_FDL_SIM_ERROR_CHECKSUM,                            /* Simulate checksum error */
    R_FDL_SIM_ERROR_BLANKCHECK,                          /* Simulate blankcheck error */
    R_FDL_SIM_ERROR_INT_ERROR                            /* Simulate internal error during blankcheck */
} r_fdl_sim_error_type_t;

#endif /* ifdef PATCH_TO_SIMULATE_ERRORS */

/* Data Type definitions */
/**
 * @struct r_fdl_multiOp_t
 * structure containing the data required for multi operations (erase multiple blocks, write
 * multiple half words data)
 */
typedef struct R_FDL_MULTIOP_T
{
    uint32_t            srcAdd_u32;                      /**< source data pointer */
    uint32_t            dstAdd_u32;                      /**< destination data pointer */
    uint32_t            srcAddEnd_u32;                   /**< last source address to write */
    r_fdl_accessType_t  accessType_enu;                  /**< access type*/
} r_fdl_multiOp_t;


/**
 * @struct r_fdl_data_t
 * this structure contains all FDL internal data
 */
typedef struct R_FDL_DATA_T
{
    const r_fdl_descriptor_t *  RTCfg_pstr;             /**< pointer to descriptor structure,
                                                            initialized by R_FDL_ISTAT_INIT */
    uint8_t                     fcb_freq_Mhz;           /**< Flash Interface frequency, computed internally */
    r_fdl_multiOp_t             multiOp_str;            /**< multi operation (erase/write) variables.
                                                             Set when a multi operation is started,
                                                             checked/updated when a single erase/write is finished */
    r_fdl_multiOp_t     spdMultiOp_str;                 /**< multi operation variables backup for FDL suspend */
    r_fdl_request_t *   fdl_reqSuspend_pstr;            /**< request structure pointer backup buffer
                                                            for suspend/resume */
    r_fdl_request_t *   fdl_reqInt_pstr;                /**< request structure pointer for current operation */
    r_fdl_status_t      spdResStatus_enu;               /**< result backup buffer for suspend/resume */
    uint32_t            spdResAddress_u32;
    r_fdl_flag_t        spdSpdRequest_enu;              /**< suspend rq. indication to the handler*/
    r_fdl_flag_t        spdResRequest_enu;              /**< resume req. indication to the handler*/
    uint32_t            stdByEraseAdd_u32;              /**< standby info backup: erase address */
    r_fdl_flag_t        stdByEraseStopped_enu;          /**< standby info backup: erase was stopped */
    r_fdl_int_status_t  stdByIntStatus_enu;             /**< standby info backup: library status */
    r_fdl_int_status_t  fdlIntStatus_enu;               /**< FDL operational status variable */
    r_fdl_status_t      fcbStatBackup_enu;
    uint32_t            fcbAddBackup_u32;

#ifdef PATCH_TO_SIMULATE_ERRORS
    r_fdl_flag_t                simError_enu;           /**< flag indicating if an error value is simulated */
    r_fdl_flag_t                actFlag_FSASTL_enu;     /**< flag indicating if an error value is simulated on 
                                                             FSASTL register */
    r_fdl_flag_t                sequencerReady_enu;     /**< flag indicating if sequencer is busy or not */                                    
    r_fdl_sim_error_type_t      simErrorType_enu;       /**< Simulated error type */
    uint32_t                    simErrorVal_u32;        /**< Error target value to be set */
    uint32_t                    simErrorCntSet_u32;     /**< Set value for cnt until error is generated */
    uint32_t                    simErrorCnt_u32;        /**< Counter for generation of the error */
    uint8_t                     reg_FSASTL_u08;             /**< Patched value for register FSASTL */
#endif
} r_fdl_data_t;


/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/
#if   FDL_COMPILER == FDL_COMP_GCC
    extern r_fdl_data_t   g_fdl_data_str __attribute__ ((section("R_FDL_DATA")));
#elif FDL_COMPILER == FDL_COMP_GHS
#elif FDL_COMPILER == FDL_COMP_IAR
    #pragma section = "R_FDL_DATA"
    extern r_fdl_data_t g_fdl_data_str;
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section B R_FDL_DATA
    extern r_fdl_data_t   g_fdl_data_str;         /**< fdl global variable structure */
#else
    #error "Unknown compiler"
#endif

#if   FDL_COMPILER == FDL_COMP_GCC
#elif FDL_COMPILER == FDL_COMP_GHS
#elif FDL_COMPILER == FDL_COMP_IAR
    #pragma section = "D"
#elif FDL_COMPILER == FDL_COMP_REC
    #pragma section B B
#endif

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
uint32_t        R_FDL_FCB_GetOPAdd (void) GCC_SECTION ("R_FDL_TEXT");
void            R_FDL_FCB_Reset    (void) GCC_SECTION ("R_FDL_TEXT");
r_fdl_flag_t    R_FDL_FCB_Suspend  (void) GCC_SECTION ("R_FDL_TEXT");
void            R_FDL_FCB_Resume   (void) GCC_SECTION ("R_FDL_TEXT");
r_fdl_flag_t    R_FDL_FCB_ChkReady (void) GCC_SECTION ("R_FDL_TEXT");
r_fdl_status_t  R_FDL_FCB_GetStat  (uint32_t * failAdd_pu32) GCC_SECTION ("R_FDL_TEXT");
r_fdl_status_t  R_FDL_FCB_SetFrequency (const uint16_t cpu_frequency_Mhz_u16) GCC_SECTION ("R_FDL_TEXT");
r_fdl_status_t  R_FDL_FCB_StartFlashOperation (uint32_t flash_addr_u32,
                                               uint32_t data_or_end_addr_u32,
                                               uint16_t fcu_cmd_u16,
                                               const r_fdl_accessType_t access_type_enu) GCC_SECTION ("R_FDL_TEXT");
r_fdl_status_t  R_FDL_ChkAccessBoundaries   (const uint32_t start_addr_u32,
                                             const uint32_t byte_cnt_u32,
                                             const r_fdl_accessType_t access_type_enu) GCC_SECTION ("R_FDL_TEXT");
r_fdl_status_t  R_FDL_FCB_SwitchMode (const r_fdl_fcb_mode_t mode_enu) GCC_SECTION ("R_FDL_TEXT");
void            R_FDL_WriteMemoryU08 (const uint32_t add_u32, const uint8_t val_u8) GCC_SECTION ("R_FDL_TEXT");

#endif /* ifndef R_FDL_GLOBAL_H */

