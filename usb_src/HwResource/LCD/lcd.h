/***********************************************************************************************************************
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
* http://www.renesas.com/disclaimer
***********************************************************************************************************************/
/* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.   */
/***********************************************************************************************************************
* File Name    : lcd.h
* Version      : 1.00
* Device(s)    : R5F51115AxFM
* Tool-Chain   : CCRX
* Description  : This Header file contains the Macro Definitions & prototypes
*                for the functions used in lcd.c
*******************************************************************************/

/***********************************************************************************************************************
Macro Definitions
***********************************************************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef LCD_H
#define LCD_H

/***********************************************************************************************************************
User Includes (Project Level Includes)
***********************************************************************************************************************/
/* Defines standard variable types used in this file */
//#include <stdint.h>

/***********************************************************************************************************************
*
*  SCREEN
*
*  The screen size is 128 x 64 pixels, with coordinate 0,0 at the bottom left.
*  The display controller is Novatek NT7502.
*
***********************************************************************************************************************/
#define SCREEN_HEIGHT 64            /* 8 lines @ 8 bits = 64. */
#define SCREEN_WIDTH  128           

/* DATA/COMMAND select pin */
#define DATA_CMD_PIN              (PORT4.PODR.BIT.B4)

/* DATA/COMMAND select pin */
#define DATA_CMD_PIN_DIR          (PORT4.PDR.BIT.B4)

/* Pin defines common to both displays */

/* Display enable pin */
#define ENABLE_PIN                (PORTC.PODR.BIT.B7)

/* Display enable pin */
#define ENABLE_PIN_DIR            (PORTC.PDR.BIT.B7)

/* reset pin */
#define RESET_PIN                 (PORTA.PODR.BIT.B3)

/* reset pin pmr bit*/
#define RESET_PIN_PMR             (PORTA.PMR.BIT.B3)

/* Display enable pin */   
#define RESET_PIN_DIR             (PORTA.PDR.BIT.B3)

/* Vbat enable pin */
#define VBAT_ENABLE_PIN           (PORT4.PODR.BIT.B4)

/* Vbat enable pin */
#define VBAT_ENABLE_PIN_DIR       (PORT4.PDR.BIT.B4)

/* Vdd enable pin */
#define VDD_ENABLE_PIN            (PORT4.PODR.BIT.B6)

/* Vdd enable pin */
#define VDD_ENABLE_PIN_DIR        (PORT4.PDR.BIT.B6)

/* Automatic calculation of parameters */
#define FONT_WIDTH                (6)   /* including a space */
#define FONT_HEIGHT               (8)   /* including 1 pixel space */
#define MAX_LINES                 (SCREEN_HEIGHT / FONT_HEIGHT)
#define CHAR_PER_LINE             (SCREEN_WIDTH / FONT_WIDTH)
#define SIZE_OF_DISPLAY_BUFFER    ((SCREEN_HEIGHT * SCREEN_WIDTH) / 8)

#define LEFT_MARGIN 0                   /*  No margin necessary */

#define CR 0x0d
#define LF 0x0a
#define BS 0x08

/***********************************************************************************************************************

  DISPLAY COMMAND SET ST7565R

***********************************************************************************************************************/
#define CMD_DISPLAY_ON            0XAF    /*Turns display ON.                       */
#define CMD_DISPLAY_OFF           0XAE
#define CMD_START_LINE            0X40    /*D5-D0 = start address                   */
#define CMD_PAGE_ADDRESS          0XB0    /*D3-D0 = page address                    */
#define CMD_COL_ADDR_HIGH         0X10    /*D3-D0 = high bits of column address     */
#define CMD_COL_ADDR_LOW          0X00    /*D3-D0 = low bits of column address      */
#define CMD_ADC_SELECT            0XA0
#define CMD_NORMAL_DISPLAY        0XA6
#define CMD_REVERSE_DISPLAY       0XA7
#define CMD_ENTIRE_DISPLAY_ON     0xA5
#define CMD_LCD_BIAS              0XA2
#define CMD_RD_MODIFY_WR          0XE0
#define CMD_RD_MODIFY_WR_END      0XEE
#define CMD_RESET                 0XE2
#define CMD_PWR_CONTROL           0X28
#define CMD_BRIGHTNESS            0X20
#define CMD_CONTRAST              0X81
#define CMD_SLEEP                 0XAC
#define CMD_NORMAL                0XAD
#define CMD_BOOSTER_SELECT        0XF8
#define CMD_NOP                   0XE3

/* delay for delay counter */
#define DELAY_TIMING              (8)

/***********************************************************************************************************************
Function Prototypes
***********************************************************************************************************************/
/* Initialises the debug LCD */
void Init_LCD (void);

/* Display string at specific line of display */
void Display_LCD (uint8_t const position,  uint8_t * const string);

/* Display the string at current cursor position */
void DisplayStr (uint8_t * str);

/* Display the sting at the center of the specified line */
void DisplayCenter (uint8_t const line_num, uint8_t * const str);

/* Clears the display */
void Clear_Display (void);

/* Clear a specified line */
void DisplayClearLine(uint8_t const line_num);

/* Set the current cursor position */
void DisplaySetCursor (uint8_t const x, uint8_t const y);

/* Delay function */
void DisplayDelay (uint32_t const units);

void DisplaySetContrast (uint8_t value);
void DisplaySetBrightness (uint8_t value);


#endif

