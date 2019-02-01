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
* File Name     : keydriver.c
* Version       : 2.10
* Description   : This is the KEY driver function code.
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


#ifdef  USB_KEY_ENABLE
/******************************************************************************
Macro definitions
******************************************************************************/
#define KEY_ON      1u
#define KEY_OFF     0u
#define CHATT_CNT   20u

#define SW1_PORT_DATA    PORT3.PIDR.BIT.B0
#define SW2_PORT_DATA    PORT3.PIDR.BIT.B1
#define SW3_PORT_DATA    PORTE.PIDR.BIT.B4


/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */

/* functions */
uint8_t usb_cpu_key_read(void);


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* variables */
static uint8_t  key_buf;
static uint8_t  key_cnt;

/* functions */
uint8_t     usb_cpu_sw_data(void);
uint8_t     usb_cpu_sw1_data(void);
uint8_t     usb_cpu_sw2_data(void);
uint8_t     usb_cpu_sw3_data(void);


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* There is no imported global variables and functions. */



/******************************************************************************
Renesas Abstracted KEY driver function
******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_key_read
Description     : 
Arguments       : none
Return value    : uint8_t            : key code
******************************************************************************/
uint8_t usb_cpu_key_read(void)
{
    uint8_t key_work;

    key_work = usb_cpu_sw_data();
    if(key_buf == key_work)
    {
        if (key_cnt != 0)
        {
            key_cnt--;
            if(key_cnt == 0)
            {
                return key_work;
            }
        }
    }
    else
    {
        key_buf = key_work;
        key_cnt = CHATT_CNT;
    }
    return NO_PUSH;
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_sw_data
Description     : 
Arguments       : none
Return value    : uint8_t            : key no
******************************************************************************/
uint8_t usb_cpu_sw_data(void)
{
    uint8_t key_no;
    uint8_t sw1_data;
    uint8_t sw2_data;
    uint8_t sw3_data;

    sw1_data = usb_cpu_sw1_data();
    sw2_data = usb_cpu_sw2_data();
    sw3_data = usb_cpu_sw3_data();

    if(sw1_data == KEY_ON)
    {
        key_no = SW1_PUSH;
    }
    else if(sw2_data == KEY_ON)
    {
        key_no = SW2_PUSH;
    }
    else if(sw3_data == KEY_ON)
    {
        key_no = SW3_PUSH;
    }
    else
    {
        key_no = NO_PUSH;
    }

    return (key_no);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_sw1_data
Description     : 
Arguments       : none
Return value    : uint8_t            : key state
******************************************************************************/
uint8_t usb_cpu_sw1_data(void)
{
    uint8_t sw_data;

    sw_data = SW1_PORT_DATA;
    if(sw_data == 0)
    {
        return KEY_ON;
    }
    else
    {
        return KEY_OFF;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_sw2_data
Description     : 
Arguments       : none
Return value    : uint8_t            : key state
******************************************************************************/
uint8_t usb_cpu_sw2_data(void)
{
    uint8_t sw_data;

    sw_data = SW2_PORT_DATA;
    if(sw_data == 0)
    {
        return KEY_ON;
    }
    else
    {
        return KEY_OFF;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_sw3_data
Description     : 
Arguments       : none
Return value    : uint8_t            : key state
******************************************************************************/
uint8_t usb_cpu_sw3_data(void)
{
    uint8_t sw_data;

    sw_data = SW3_PORT_DATA;
    if(sw_data == 0)
    {
        return KEY_ON;
    }
    else
    {
        return KEY_OFF;
    }
}
/******************************************************************************
End of function
******************************************************************************/

#endif /* USB_KEY_ENABLE */

/******************************************************************************
End Of File
******************************************************************************/

