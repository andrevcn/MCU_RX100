#ifdef ENABLE_QAC_TEST
    #pragma PRQA_MESSAGES_OFF 0292
#endif
/***********************************************************************************************************************
* Library       : Data Flash Access Library for Renesas RX111 devices, based on the MF3 Flash technology
*
* File Name     : $Source: r_fdl.h $
* Lib. Version  : $RX111_FDL_LIB_VERSION_T01: V1.01 $
* Mod. Revision : $Revision: 1.4 $
* Mod. Date     : $Date: 2013/06/25 10:52:13MESZ $
* Device(s)     : MF3 Flash based RX111 microcontroller
* Description   : User interface function prototypes
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

#ifndef R_FDL_H
#define R_FDL_H


/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/
/* Global compiler definition */
#define EEL_COMP_GCC            (1)
#define EEL_COMP_GHS            (2)
#define EEL_COMP_IAR            (3)
#define EEL_COMP_REC            (4)

#define FDL_COMP_GCC            (1)
#define FDL_COMP_GHS            (2)
#define FDL_COMP_IAR            (3)
#define FDL_COMP_REC            (4)

#if (defined __GNUC__)
    #define EEL_COMPILER EEL_COMP_GCC
    #define FDL_COMPILER FDL_COMP_GCC
    #define GCC_SECTION(s)              __attribute__((section(s)))
    #define GCC_ATTRIB_NOINLINE         __attribute__((noinline))

#elif (defined __ghs__)
    #define EEL_COMPILER EEL_COMP_GHS
    #define FDL_COMPILER FDL_COMP_GHS
    #define GCC_SECTION(s)
    #define GCC_ATTRIB_NOINLINE

#elif (defined __IAR_SYSTEMS_ASM__) || (defined __IAR_SYSTEMS_ICC__)
    #define EEL_COMPILER EEL_COMP_IAR
    #define FDL_COMPILER FDL_COMP_IAR
    #define GCC_SECTION(s)
    #define GCC_ATTRIB_NOINLINE

#elif (defined __v850__) || (defined __RX200)
    #define EEL_COMPILER EEL_COMP_REC
    #define FDL_COMPILER FDL_COMP_REC
    #define GCC_SECTION(s)
    #define GCC_ATTRIB_NOINLINE

#else
    #error "Unknown compiler"
#endif /* if (defined __GNUC__) */


/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
/* FDL initialization function */
r_fdl_status_t  R_FDL_Init (const r_fdl_descriptor_t * descriptor_pstr) GCC_SECTION ("R_FDL_TEXT");

/* Flash operation initiation */
void            R_FDL_Execute (r_fdl_request_t * request_pstr) GCC_SECTION ("R_FDL_TEXT");

/* Flash operation forwarding and status check */
void            R_FDL_Handler (void) GCC_SECTION ("R_FDL_TEXT");

/* Flash operation suspend request */
r_fdl_status_t  R_FDL_EraseSuspendRequest (void) GCC_SECTION ("R_FDL_TEXT");

/* Flash operation resume request after suspend */
r_fdl_status_t  R_FDL_EraseResumeRequest (void) GCC_SECTION ("R_FDL_TEXT");

/* Asynchronous function to prepare FDL for device power save modes */
r_fdl_status_t  R_FDL_StandBy (void) GCC_SECTION ("R_FDL_TEXT");

/* Asynchronous function to resume FDL after leaving device power save modes */
r_fdl_status_t  R_FDL_WakeUp (void) GCC_SECTION ("R_FDL_TEXT");

/* Return of FDL library version string */
const uint8_t * R_FDL_GetVersionString (void) GCC_SECTION ("R_FDL_TEXT");

#endif /* ifndef R_FDL_H */

