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
/* Copyright (C) _#COPYRIGHTYEAR#_ Renesas Electronics Corporation. All rights reserved.   */
/***********************************************************************************************************************
* File Name     : lcd.c
* Version       : _#VERSION#_
* Device        : _#DEVICE#_
* Tool-Chain    : _#TOOLCHAIN#_
* H/W Platform  : _#HWPLATFORM#_
* Description   : LCD Module utility functions.
*
***********************************************************************************************************************/
/***********************************************************************************************************************
* History       : _#DATE#_  Ver. 1.00 First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* User Includes (Project Level Includes)
***********************************************************************************************************************/
/* Includes assembly level definitions */
#include "r_cg_macrodriver.h"
#include <string.h>
#include "r_cg_iodefine.h"

#include "r_cg_sci.h"
/* LCD controlling function prototypes & macro definitions */
#include "lcd.h"
#include "ascii.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* display buffer */
uint8_t bitmap[SIZE_OF_DISPLAY_BUFFER];
/* LCD cursor co-ordinates */
uint8_t curx = 0;
uint8_t cury = 0;
/* set m_InverseMode to 1 to invert display */
uint8_t m_InverseMode = 0;

/***********************************************************************************************************************
* Local Function Prototypes
***********************************************************************************************************************/
static void init_pmod_lcd (void);

static void charput (uint8_t const val);
static void clr_display (void);
static void update_display (void);
static void display_write_command (uint8_t const  cmd);
static void display_write_data_block (uint8_t * const tx_buf, uint16_t const tx_num);


/***********************************************************************************************************************
* Function Name : Init_LCD
* Description   : Initialises the LCD display. 
* Argument      : none
* Return value  : none
***********************************************************************************************************************/
void Init_LCD (void)
{
    /* Start SPI comm channel to LCD Display */
    R_SCI5_Start();

    /* initialise Standard PMOD display */
    init_pmod_lcd();

    /* clear the display before use */
    Clear_Display();
}
/***********************************************************************************************************************
* End of function Init_LCD
***********************************************************************************************************************/
/**********************************************************************************************************************
* Function Name : Display_LCD
* Description   : This function controls LCD writes to the start of the line  of the LCD.
*                 specified by position of the LCD. If position is greater than the
*                 number of lines available then the LCD write will be to the last line.
* Argument      : (uint8_t)position -   Line number of display
*                 (const char *) string -Pointer to data to be written to
*                                display. Last character should be null.
* Return value  : none
***********************************************************************************************************************/
void Display_LCD (uint8_t const position,  uint8_t * const string)
{
    DisplaySetCursor(0, position);
    DisplayStr(string);
}
/***********************************************************************************************************************
* End of function Display_LCD
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : DisplayStr(const uint8_t *string)
* Description   : Displays a string at the current cursor position
* Argument      : char *string - pointer to a string
* Return value  : none
***********************************************************************************************************************/
void DisplayStr (uint8_t * str)
{
    uint16_t i;
    uint16_t size;

    size = (uint16_t)strlen((const char *)str);
    /* load characters into screen bitmap */
    for (i = 0; i < size; i++)
    {
        charput(*str++);
    }
    /* load bitmap into LCD */
    update_display();
}
/***********************************************************************************************************************
* End of function DisplayStr
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : DisplayCenter
* Description   : Display a string in the center of the specified line.
* Argument      : uint8_t line_num - line number (0-7)
*                 pointer to a string.
* Return value  : none
***********************************************************************************************************************/
void DisplayCenter (uint8_t const line_num, uint8_t * const str)
{
    uint8_t spaces;

    DisplaySetCursor(0, line_num);

    spaces = (uint8_t)((CHAR_PER_LINE - strlen((const char *)str)) / 2);
    while (spaces--)
    {
        charput(' ');
    }
    DisplayStr(str);
}
/***********************************************************************************************************************
* End of function DisplayCenter
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : Clear_Display
* Description   : This function clears the screen
* Argument      : none
* Return value  : none
***********************************************************************************************************************/
void Clear_Display (void)
{
   clr_display();
   update_display();
}
/***********************************************************************************************************************
* End of function Clear_Display
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : DisplayClearLine
* Description   : Clear the specified line
* Argument      : uint8_t line numb 0 - 7
* Return value  : none
***********************************************************************************************************************/
void DisplayClearLine (uint8_t const line_num)
{
    uint8_t i;

    if (line_num < MAX_LINES)
    {
        DisplaySetCursor(0, line_num);
        for (i = 0; i < CHAR_PER_LINE; i++)
        {
            charput(' ');
        }   
        update_display();
        DisplaySetCursor(0, line_num);
    }
}
/***********************************************************************************************************************
* End of function DisplayClearLine
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : DisplaySetCursor(uint8_t x, uint8_t y)
* Description   : Set the cursor to the specified position. the position must be
*                 within the limits.
* Argument      : uint8_t x - X position
*                         y - Y position
* Return value  : none
***********************************************************************************************************************/
void DisplaySetCursor (uint8_t const x, uint8_t const y)
{
    if (x < CHAR_PER_LINE)
    {
        curx = x;
    }

    if (y < MAX_LINES)
    {
        cury = y;
    }
}
/***********************************************************************************************************************
* End of function DisplaySetCursor
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : DisplayDelay
* Description   : Delay routine for LCD or any other devices.
* Argument      : (uint32_t) units - time in, approximately, microsec
* Return value  : None
***********************************************************************************************************************/
void DisplayDelay (uint32_t const units)
{
    uint32_t counter = units * DELAY_TIMING;
    
    /* Decrement the counter and wait */
    while (counter--)
    {
        nop();
    }
}
/***********************************************************************************************************************
* End of function DisplayDelay
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : DisplaySetContrast
* Description   : Set the contrast value
* Argument      : uint8_t value - 0 (low) - 0x3F (high)
* Return value  : None
***********************************************************************************************************************/
void DisplaySetContrast (uint8_t value)
{
    value &= 0x3F;
    display_write_command(CMD_CONTRAST);
    display_write_command(value);
}
/***********************************************************************************************************************
* End of function DisplaySetContrast
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : DisplaySetBrightness
* Description   : Set the Brightness level
* Argument      : uint8_t value - 0 (low) - 7 (high)
* Return value  : None
***********************************************************************************************************************/
void DisplaySetBrightness (uint8_t value)
{
    value = (uint8_t)(CMD_BRIGHTNESS | (value & 0x07));
    display_write_command(value);
}
/***********************************************************************************************************************
* End of function DisplaySetBrightness
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : init_pmod_lcd
* Description   : Initialises the LCD display.
* Argument      : none
* Return value  : none
***********************************************************************************************************************/
static void init_pmod_lcd (void)
{
    /* Preset the output ports for i/o pins prior to setting as outputs */
    DATA_CMD_PIN = 0x1;
    ENABLE_PIN  = 0x1;
    RESET_PIN = 0x1;

    /* Configure the all involved pin directions as output */
    DATA_CMD_PIN_DIR = 0x1;
    ENABLE_PIN_DIR  = 0x1;
    RESET_PIN_DIR = 0x1;
    
    /* ensure that the port is i/o not peripheral (IRQ6) */
    RESET_PIN_PMR = 0x0;    

    DisplayDelay(1000);

    /* Clear reset line to reset display */
    RESET_PIN = 0x0;

    /* Wait 1ms */
    DisplayDelay(1000);

    /* Release reset */
    RESET_PIN = 0x1;

    display_write_command(CMD_PWR_CONTROL | 0x07);
    display_write_command(CMD_DISPLAY_ON);
    display_write_command(CMD_NORMAL_DISPLAY);
    display_write_command(CMD_ENTIRE_DISPLAY_ON & 0xFE);
    display_write_command(CMD_START_LINE);
    display_write_command(CMD_ADC_SELECT);
    display_write_command(CMD_NORMAL);
    display_write_command(0x03);

    /* Adjust the contrast value if required. */
    /* By experiment, the best value was found to be 26 */
    DisplaySetContrast(26);

    /* Adjust the brightness level if required. */
    /* By experiment, the best value was found to be 5 */
    DisplaySetBrightness(0x05);
}
/***********************************************************************************************************************
* End of function init_pmod_lcd
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : charput
* Description   : Displays a character at the current cursor position.
*                 Advances the cursor by 1 position.
*                 Cursor wraps to next line at the end of the line.
*                 Cursor wraps to Home position at the end of the display.
*
*                 The following control codes are accepted:
*
*                 LF increments y cursor position
*                 CR resets x cursor to 0.
*                 BS moves x cursor back by 1 place if possible.
* Argument      : none
* Return value  : none
***********************************************************************************************************************/
static void charput (uint8_t const val)
{
    uint8_t * ptr;
    uint8_t   i;
    uint8_t   pattern;
    uint8_t * dst;
    
    switch (val)
    {
        /* Carriage return character */
        case CR:
        {
            curx = 0;
        }    
        break;

        /* Line feed character */
        case LF:
        {
            if (cury < (MAX_LINES - 1))
            {
                cury++;
            }    
            else 
            {
                cury = 0;
            }
        }    
        break;

        /* Backspace character */
        case BS:
        {
            if (curx > 0)
            {
                curx--;
            }
        }
        break;

        /* Normal displayable characters */
        default:
        {
            /* Ensure value is within the ASCII range */
            if ((val >= 0x20) && (val <= 0x7f))
            {
                /* Get pointer to start of location in LCD */
                dst = ((bitmap + SIZE_OF_DISPLAY_BUFFER - SCREEN_WIDTH) 
                      + (curx * FONT_WIDTH) - (cury * SCREEN_WIDTH));
                
                /* Get pointer to start of representation of character in ASCII table */
                ptr = (uint8_t *)&ASCII_TABLE[val - 0x20][0];
                
                /* Copy character over */
                for (i = 0; i < 6; i++)
                {
                    pattern = *ptr++;
                    
                    /* Invert font if required */
                    if (m_InverseMode)
                    {
                        pattern = (uint8_t)(~pattern);
                    }
                    
                    /* Paranoia to ensure that write stays inside bitmap buffer */
                    /* Otherwise operation could be corrupted */
                    if ((dst <= (bitmap + SIZE_OF_DISPLAY_BUFFER)) || (dst >= bitmap))
                    {
                        *dst++ = pattern;
                    }
                }
                
                /* move cursor to next co-ordinate on LCD */
                curx++;

                /* move to next row if reached the end of line */
                if (curx >= CHAR_PER_LINE)
                {
                    curx = 0;
                    if (cury < (MAX_LINES - 1))
                    {
                        cury++;
                    }
                    else 
                    {
                        /* loop back to top line if the last line of the display is reached */
                        cury = 0;
                    }
                }
            }
        }
        break;
    }
}
/***********************************************************************************************************************
* End of function charput
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : clr_display
* Description   : This function clears the screen bitmap array
* Argument      : none
* Return value  : none
***********************************************************************************************************************/
static void clr_display (void)
{
    uint8_t * ptr = bitmap;
    uint32_t  loop;
    for (loop = 0; loop < SIZE_OF_DISPLAY_BUFFER; loop++)
    {
        *ptr++ = 0;
    }
}
/***********************************************************************************************************************
* End of function clr_display
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : update_display
* Description   : This function loads the bitmap array into the appropriate
*                 page in the display.
* Argument      : None
* Return value  : none
***********************************************************************************************************************/
static void update_display (void)
{
    volatile uint8_t loop = 0;
    uint8_t *        bmp_ptr = bitmap;

    /* LCD RAM consists of pages of 128 bytes */
    while (loop < (SIZE_OF_DISPLAY_BUFFER / 128))
    {
       /* set page */
       display_write_command((uint8_t)(0xB0 | loop));
       
       /* start at left */
       display_write_command(0x0 | 0x0);
       display_write_command(0x10 | 0x0);

       /* transmit buffer to display */
       display_write_data_block(bmp_ptr, 128);

       /* point to next page */
       bmp_ptr += 128;
       loop++;
    }
}
/***********************************************************************************************************************
* End of function update_display
***********************************************************************************************************************/


/***********************************************************************************************************************
* Function Name : display_write_command
* Description   : Encapsulation of autocode SPI5 send, sends 1 cmd byte.
* Argument      :
* Return value  : None
***********************************************************************************************************************/
static void display_write_command (uint8_t const cmd)
{
    /* data cmd pin low to indicate a command */
    DATA_CMD_PIN = 0x0;
    
    /* assert chip select */
    ENABLE_PIN  = 0x0;
    
    /*send command */
    sendSPI5((uint8_t *)&cmd, 1);
    
    /* poll for tx complete */
    while (sci5_tx_is_busy())
    {
        /* do nothing */
    }
    
    /* de-assert chip select */
    ENABLE_PIN  = 0x1;
    
    /* data cmd pin high to signify data */
    DATA_CMD_PIN = 0x1;
}
/***********************************************************************************************************************
* End of function display_write_command
***********************************************************************************************************************/


/***********************************************************************************************************************
* Function Name : display_write_data_block
* Description   : Encapsulation of autocode SPI5 send, controlling CS line & waiting for send to finish
* Argument      :
* Return value  : None
***********************************************************************************************************************/
static void display_write_data_block (uint8_t * const tx_buf, uint16_t const tx_num)
{
    /* data cmd pin high to signify data */
    DATA_CMD_PIN = 0x1;
    
    /* assert chip select */
    ENABLE_PIN  = 0x0;
    
    /* send tx_num bytes */
    sendSPI5(tx_buf, tx_num);
    
    /* poll for tx complete */
    while (sci5_tx_is_busy())
    {
        /* do nothing */
    }
    /* de-assert chip select */
    ENABLE_PIN  = 0x1;
    
    /* data cmd pin high to signify data */
    DATA_CMD_PIN = 0x1;
}
/***********************************************************************************************************************
* End of function display_write_data_block
***********************************************************************************************************************/


