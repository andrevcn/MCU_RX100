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
* File Name     : leddriver.c
* Version       : 2.10
* Description   : This is the LED driver function code.
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


#ifdef  USB_LED_ENABLE
/******************************************************************************
Macro definitions
******************************************************************************/
/* There is no macro definition. */
#define LED0_PORT_FUNCTION      PORTB.PDR.BIT.B7
#define LED1_PORT_FUNCTION      PORTA.PDR.BIT.B0
#define LED2_PORT_FUNCTION      PORT5.PDR.BIT.B4
#define LED3_PORT_FUNCTION      PORT1.PDR.BIT.B7

#define LED0_PORT_DATA          PORTB.PODR.BIT.B7
#define LED1_PORT_DATA          PORTA.PODR.BIT.B0
#define LED2_PORT_DATA          PORT5.PODR.BIT.B4
#define LED3_PORT_DATA          PORT1.PODR.BIT.B7

/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */

/* functions */
void        usb_cpu_LedInitial(void);
void        usb_cpu_led_set_bit(uint8_t ledpos, uint8_t data);
void        usb_cpu_led_set_data(uint8_t bitdata);


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* There is no private global variables and functions. */


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* There is no imported global variables and functions. */



/******************************************************************************
Renesas Abstracted LED driver function
******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_LedInitial
Description     : LED Initial
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cpu_LedInitial(void)
{
    /* Initialized port data register(LED is off) */
    LED0_PORT_DATA = PORT_HIGH;         /* LED0 */
    LED1_PORT_DATA = PORT_HIGH;         /* LED1 */
    LED2_PORT_DATA = PORT_HIGH;         /* LED2 */
    LED3_PORT_DATA = PORT_HIGH;         /* LED3 */

    /* Initialized port direction register */
    LED0_PORT_FUNCTION = PORT_OUT_MODE; /* LED0 */
    LED1_PORT_FUNCTION = PORT_OUT_MODE; /* LED1 */
    LED2_PORT_FUNCTION = PORT_OUT_MODE; /* LED2 */
    LED3_PORT_FUNCTION = PORT_OUT_MODE; /* LED3 */

    /* Initialized port data register(LED is off) */
    LED0_PORT_DATA = PORT_HIGH;         /* LED0 */
    LED1_PORT_DATA = PORT_HIGH;         /* LED1 */
    LED2_PORT_DATA = PORT_HIGH;         /* LED2 */
    LED3_PORT_DATA = PORT_HIGH;         /* LED3 */
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_led_set_bit
Description     : LED Bit Set process
Arguments       : uint8_t ledpos       : Led Set Position
                : uint8_t data         : Led On/Off
Return value    : none
******************************************************************************/
void usb_cpu_led_set_bit(uint8_t ledpos, uint8_t data)
{
    switch(ledpos)
    {
        case LED_POS0:
            if (data == LED_OFF)
            {
                LED0_PORT_DATA = PORT_HIGH;
            }
            if (data == LED_ON)
            {
                LED0_PORT_DATA = PORT_LOW;
            }
            break;
        case LED_POS1:
            if (data == LED_OFF)
            {
                LED1_PORT_DATA = PORT_HIGH;
            }
            if (data == LED_ON)
            {
                LED1_PORT_DATA = PORT_LOW;
            }
            break;
        case LED_POS2:
            if (data == LED_OFF)
            {
                LED2_PORT_DATA = PORT_HIGH;
            }
            if (data == LED_ON)
            {
                LED2_PORT_DATA = PORT_LOW;
            }
            break;
        case LED_POS3:
            if (data == LED_OFF)
            {
                LED3_PORT_DATA = PORT_HIGH;
            }
            if (data == LED_ON)
            {
                LED3_PORT_DATA = PORT_LOW;
            }
            break;
        default:
            break;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_led_set_data
Description     : LED Bit Set process
Arguments       : uint8_t bitdata         : xxxx,Led3,Led2,Led1,Led0
Return value    : none
******************************************************************************/
void usb_cpu_led_set_data(uint8_t bitdata)
{
    /* LED0 */
    if((bitdata & LED_POS0) == LED_POS0)
    {
        LED0_PORT_DATA = PORT_LOW;
    }
    else
    {
        LED0_PORT_DATA = PORT_HIGH;
    }
    /* LED1 */
    if((bitdata & LED_POS1) == LED_POS1)
    {
        LED1_PORT_DATA = PORT_LOW;
    }
    else
    {
        LED1_PORT_DATA = PORT_HIGH;
    }
    /* LED2 */
    if((bitdata & LED_POS2) == LED_POS2)
    {
        LED2_PORT_DATA = PORT_LOW;
    }
    else
    {
        LED2_PORT_DATA = PORT_HIGH;
    }
    /* LED3 */
    if((bitdata & LED_POS3) == LED_POS3)
    {
        LED3_PORT_DATA = PORT_LOW;
    }
    else
    {
        LED3_PORT_DATA = PORT_HIGH;
    }
}
/******************************************************************************
End of function
******************************************************************************/

#endif /* USB_LED_ENABLE */

/******************************************************************************
End Of File
******************************************************************************/

