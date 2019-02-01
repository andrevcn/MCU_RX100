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
* File Name     : lcddriver.c
* Version       : 2.10
* Description   : This is the LCD driver function code.
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/

/******************************************************************************
Section definitions
******************************************************************************/
#ifdef  __RX
#endif


/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "r_usb_ctypedef.h"             /* Type define */
#include "r_usb_ckernelid.h"            /* Kernel ID definition */
#include "r_usb_usrconfig.h"            /* System definition */
#include "iodefine.h"                   /* MCU IO definition */
#include "hw_resource.h"
#include "r_usb_cdefusbip.h"            /* USB-FW Library Header */
#include "r_usb_api.h"                  /* USB API public function header */


#ifdef  USB_LCD_ENABLE
/*****************************************************************************
Macro definitions
******************************************************************************/


/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern void R_SCI5_Create(void);

/* Initialises the debug LCD */
extern void Init_LCD (void);

/* Set the current cursor position */
extern void DisplaySetCursor (uint8_t const x, uint8_t const y);

/* Display the string at current cursor position */
extern void DisplayStr (uint8_t * str);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */

/* functions */
void        usb_cpu_LcdInitial(void);
void        usb_cpu_LcdDisp(uint8_t pos, uint8_t *str);
void        usb_cpu_LcdContDisp(uint8_t pos, uint8_t data);


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* variables */

/* functions */


/******************************************************************************
Renesas Abstracted LCD functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_LcdInitial
Description     : LCD Initial
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cpu_LcdInitial(void)
{
    /* Enable writing to registers related to operating modes, LPC, CGC and software reset */
    SYSTEM.PRCR.WORD = 0xA507U;         /* Protect off */

    /* Enable writing to MPC pin function control registers */
    MPC.PWPR.BIT.B0WI = 0U;             /* Enable PFSWE-bit */
    MPC.PWPR.BIT.PFSWE = 1U;            /* Enable PFS-Register */

    R_SCI5_Create();    /* Initializes SCI5 */
    Init_LCD();     /* Initialises the LCD display.  */
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_LcdDisp
Description     : LCD display string data
Arguments       : uint8_t pos           : Position
                : uint8_t *str          : Write data (string data)
Return value    : none
******************************************************************************/
void usb_cpu_LcdDisp(uint8_t pos, uint8_t *str)
{
    if( pos < 8 )
    {
        DisplaySetCursor(pos, 0);
    }
    else
    {
        DisplaySetCursor( ( pos - 8), 1 );
    }
    DisplayStr(str);
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_LcdContDisp
Description     : LCD display numeric data
Arguments       : uint8_t pos           : Position
                : uint8_t data          : Write data (string data)
Return value    : none
******************************************************************************/
void usb_cpu_LcdContDisp(uint8_t pos, uint8_t data)
{
    uint8_t buf[3];

    buf[0] = (data / 10) | 0x30;
    buf[1] = (data % 10) | 0x30;
    buf[2] = 0x00;
#ifdef USB_LCD_ENABLE
    usb_cpu_LcdDisp(pos, buf);
#endif
}
/******************************************************************************
End of function
******************************************************************************/


#endif /* USB_LCD_ENABLE */

/******************************************************************************
End Of File
******************************************************************************/

