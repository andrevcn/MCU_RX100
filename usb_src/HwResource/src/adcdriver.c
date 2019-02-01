/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : adcdriver.c
* Version      : 2.10
* Description  : This is the ADC driver function code.
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/

/******************************************************************************
Section definitions
******************************************************************************/
#ifdef  __RX
#endif


/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"             /* Type define */
#include "r_usb_ckernelid.h"            /* Kernel ID definition */
#include "r_usb_usrconfig.h"            /* System definition */
#include "iodefine.h"                   /* MCU IO definition */
#include "hw_resource.h"


#ifdef USB_ADC_ENABLE
/******************************************************************************
Macro definitions
******************************************************************************/
/* ----- A/D SW Defines -------- */
#define     CHATT_CNT   100
//#define     AD_KEY

/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */

/* functions */
uint16_t    usb_cpu_adc_read(void);
void        usb_cpu_adc_enable(void);


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* variables */
#ifdef AD_KEY
USB_STATIC uint16_t  key_buf;
USB_STATIC uint16_t  key_back;
USB_STATIC uint16_t  key_cnt;
#else
#endif

/* functions */
USB_STATIC uint16_t  usb_cpu_adc_start(void);


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* There is no imported global variables and functions. */



/******************************************************************************
Renesas Abstracted ADC driver function
******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_adc_read
Description     : 
Arguments       : none
Return value    : uint16_t           : ad code
******************************************************************************/
uint16_t usb_cpu_adc_read(void)
{
#ifdef AD_KEY
    uint16_t key_work;

    key_work = usb_cpu_adc_start();
    if(key_buf == key_work)
    {
        if (key_cnt != 0)
        {
            key_cnt--;
            if(key_cnt == 0)
            {
                key_back = key_work;
                return key_work;
            }
        }
    }
    else
    {
        key_buf = key_work;
        key_cnt = CHATT_CNT;
    }
    return key_back;
#else
    return usb_cpu_adc_start();
#endif
} /* eof usb_cpu_adc_read */


/******************************************************************************
Function Name   : usb_cpu_adc_enable
Description     :
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cpu_adc_enable(void)
{
    SYSTEM.MSTPCRA.BIT.MSTPA17  = 0;
    PORT4.PMR.BIT.B0            = 0;
    MPC.PWPR.BIT.B0WI           = 0;
    MPC.PWPR.BIT.PFSWE          = 1;
    MPC.PJ6PFS.BIT.ASEL         = 1;            /* VREFH0 */
    MPC.PJ7PFS.BIT.ASEL         = 1;            /* VREFL0 */
    MPC.P40PFS.BIT.ASEL         = 1;            /* ANN000 */
    PORT4.PMR.BIT.B0            = 1;
    S12AD.ADCER.BIT.ADRFMT      = 0;
    S12AD.ADANSA.BIT.ANSA0      = 1;
} /* eof usb_cpu_adc_enable */


/******************************************************************************
Function Name   : usb_cpu_adc_start
Description     :
Arguments       : none
Return value    : uint8_t            : key state
******************************************************************************/
uint16_t usb_cpu_adc_start(void)
{
    S12AD.ADCSR.BIT.ADST = 1;
    while( S12AD.ADCSR.BIT.ADST );
    return (S12AD.ADDR0);
} /* eof usb_cpu_adc_start */
#endif /* USB_ADC_ENABLE */

/******************************************************************************
End Of File
******************************************************************************/

