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
* File Name     : main.c
* Version       : 2.10
* Description   : USB Host sample main loop code.
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/


/******************************************************************************
Section definitions
******************************************************************************/
#ifdef  R8CUSB
#pragma section program P_smpl
#pragma section bss B_smpl
#pragma section data R_smpl
#pragma section rom C_smpl
#endif
#ifdef  RL78USB
#endif
#ifdef  RL78USB_SECTION
#pragma section @@CODE   P_SMP
#pragma section @@BASE   T_SMP
#pragma section @@DATA   B_SMP
#endif
#ifdef  __RX
#pragma section _smpl
#endif


/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"             /* Type define */
#include "r_usb_ckernelid.h"            /* Kernel ID definition */
#include "r_usb_usrconfig.h"            /* System definition, USB-FW Library Header */
#include "r_usb_api.h"                  /* USB API public function header */
#include "hw_resource.h"


/******************************************************************************
Macro definitions
******************************************************************************/
/* There is no macro definition. */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* variables */
USB_STATIC volatile uint8_t g_usb_suspend_flag;
USB_STATIC volatile uint8_t g_usb_int_key_flag;

/* functions */


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */

/* functions */
void        main(void);
void        usb_smpl_lcd_string(uint8_t pos, uint8_t *str);
void        usb_smpl_led_position(uint8_t ledpos, uint8_t data);
uint8_t     usb_smpl_key_input(void);
void        usb_smpl_set_suspend_flag(uint8_t data);
uint8_t     usb_smpl_get_suspend_flag(void);
void        usb_smpl_set_intkey(uint8_t data);
uint8_t     usb_smpl_get_intkey(void);


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* variables */

/* functions */
extern void     usb_hsmpl_main_init(void);
extern void     usb_hhid_MainTask(void);
extern void     R_usb_hhid_task(void);

/* Special function drivers */
#ifdef  USB_LCD_ENABLE
extern void     usb_cpu_LcdDisp(uint8_t pos, uint8_t *str);
#endif /* USB_LCD_ENABLE */

#ifdef USB_LED_ENABLE
extern void     usb_cpu_led_set_bit(uint8_t ledpos, uint8_t data);
#endif /* USB_LED_ENABLE */

#ifdef USB_KEY_ENABLE
extern uint8_t  usb_cpu_key_read(void);
#endif
#ifdef USB_ADC_ENABLE
extern uint8_t  usb_cpu_key_read(void);
#endif

/******************************************************************************
Renesas HHID Sample
******************************************************************************/

/******************************************************************************
Function Name   : main
Description     : Main Task
Arguments       : none
Return value    : none
******************************************************************************/
void main(void)
{
    /* Initialized USBIP */
    usb_hsmpl_main_init();
    usb_smpl_lcd_string(LCD_POS_D0, (uint8_t *)"HOST HID");

    /* Sample main loop  */
    while(1)
    {
        if (R_usb_cstd_Scheduler() == USB_FLGSET)
        {
            R_usb_hstd_HcdTask();       /* HCD Task */
            R_usb_hstd_MgrTask();       /* MGR Task */
            R_usb_hhid_task();          /* HID Task */
        }
        usb_hhid_MainTask();            /* HID application */
    }
}   /* eof main() */


/******************************************************************************
Function Name   : usb_smpl_lcd_string
Description     : Displayed string data
Arguments       : uint8_t pos           : Position
                : uint8_t *str          : Write data (string data)
Return value    : none
******************************************************************************/
void usb_smpl_lcd_string(uint8_t pos, uint8_t *str)
{
#ifdef  USB_LCD_ENABLE
    usb_cpu_LcdDisp(pos, str);
#endif
}   /* eof usb_smpl_lcd_string() */


/******************************************************************************
Function Name   : usb_smpl_led_position
Description     : Set LED ON/OFF
Arguments       : uint8_t ledpos        : Led Set Position
                : uint8_t data          : Led On/Off
Return value    : none
******************************************************************************/
void usb_smpl_led_position(uint8_t ledpos, uint8_t data)
{
#ifdef USB_LED_ENABLE
    usb_cpu_led_set_bit(ledpos, data);
#endif
}   /* eof usb_smpl_led_position() */


/******************************************************************************
Function Name   : usb_smpl_key_input
Description     : Read key code
Arguments       : none
Return value    : key code
******************************************************************************/
uint8_t usb_smpl_key_input(void)
{
#ifdef USB_KEY_ENABLE
    return  usb_cpu_key_read();
#else
    return  NO_PUSH;                    /* KEY OFF */
#endif
}   /* eof usb_smpl_key_input() */


/******************************************************************************
Function Name   : usb_smpl_set_suspend_flag
Description     : Set resume interruptt flag
Arguments       : uint8_t data
Return value    : none
******************************************************************************/
void usb_smpl_set_suspend_flag(uint8_t data)
{
    g_usb_suspend_flag  = data;
}   /* eof usb_smpl_set_suspend_flag() */


/******************************************************************************
Function Name   : usb_smpl_get_suspend_flag
Description     : Get resume interruptt flag
Arguments       : none
Return value    : g_usb_suspend_flag
******************************************************************************/
uint8_t usb_smpl_get_suspend_flag(void)
{
    return  g_usb_suspend_flag;
}   /* eof usb_smpl_get_suspend_flag() */


/******************************************************************************
Function Name   : usb_smpl_set_intkey
Description     : Set external key interruptt flag
Arguments       : uint8_t data
Return value    : none
******************************************************************************/
void usb_smpl_set_intkey(uint8_t data)
{
    g_usb_int_key_flag  = data;
}   /* eof usb_smpl_set_intkey() */


/******************************************************************************
Function Name   : usb_smpl_get_intkey
Description     : Get external key interruptt flag
Arguments       : none
Return value    : g_usb_int_key_flag
******************************************************************************/
uint8_t usb_smpl_get_intkey(void)
{
    return  g_usb_int_key_flag;
}   /* eof usb_smpl_get_intkey() */


/******************************************************************************
End Of File
******************************************************************************/
