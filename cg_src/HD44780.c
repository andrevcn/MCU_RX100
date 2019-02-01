/* Inclusoes de arquivos de cabecalho ----------------------------------------*/
#include "HD44780.h"


extern void     usb_cpu_delay_1us(uint16_t time);

HD44780_Options_t HD44780_Opts;



#define HD44780_RS_LOW              RS_Off()
#define HD44780_RS_HIGH             RS_On()
#define HD44780_E_LOW               EN_Off()
#define HD44780_E_HIGH              EN_On()
#define HD44780_E_BLINK             HD44780_E_HIGH; HD44780_Delay(100); HD44780_E_LOW; HD44780_Delay(100)

void HD44780_Delay(uint32_t x)
{
	usb_cpu_delay_1us(x);
}

void HD44780_Init(uint8_t cols, uint8_t rows)
{
	__delay_ms(45);

	HD44780_Opts.Rows = rows;
	HD44780_Opts.Cols = cols;

	/* Set cursor pointer to beginning for LCD */
	HD44780_Opts.currentX = 0;
	HD44780_Opts.currentY = 0;

	HD44780_Opts.DisplayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;

	if (rows > 1)
	{
		HD44780_Opts.DisplayFunction |= HD44780_2LINE;
	}

	/* Try to set 4bit mode */
	HD44780_Cmd4bit(0x03);
	HD44780_Delay(4500);

	/* Second try */
	HD44780_Cmd4bit(0x03);
	HD44780_Delay(4500);

	/* Third goo! */
	HD44780_Cmd4bit(0x03);
	HD44780_Delay(4500);

	/* Set 4-bit interface */
	HD44780_Cmd4bit(0x02);
	HD44780_Delay(100);

	/* Set # lines, font size, etc. */
	HD44780_Cmd(HD44780_FUNCTIONSET | HD44780_Opts.DisplayFunction);

	/* Turn the display on with no cursor or blinking default */
	HD44780_Opts.DisplayControl = HD44780_DISPLAYON;
	HD44780_DisplayOn();

	/* Clear lcd */
	HD44780_Clear();

	/* Default font directions */
	HD44780_Opts.DisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
	HD44780_Cmd(HD44780_ENTRYMODESET | HD44780_Opts.DisplayMode);

	/* Delay */
	HD44780_Delay(4500);
}

void HD44780_Clear(void)
{
	HD44780_Cmd(HD44780_CLEARDISPLAY);
	HD44780_Delay(3000);
}

void HD44780_Puts(uint8_t x, uint8_t y, char* str)
{
	HD44780_CursorSet(x, y);

	while (*str)
	{
		if (HD44780_Opts.currentX >= HD44780_Opts.Cols)
		{
			HD44780_Opts.currentX = 0;
			HD44780_Opts.currentY++;
			HD44780_CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
		}

		if (*str == '\n')
		{
			HD44780_Opts.currentY++;
			HD44780_CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
		}
		else if (*str == '\r')
		{
			HD44780_CursorSet(0, HD44780_Opts.currentY);
		}
		else
		{
			HD44780_Data(*str);
			HD44780_Opts.currentX++;
		}
		str++;
	}
}

void HD44780_DisplayOn(void)
{
	HD44780_Opts.DisplayControl |= HD44780_DISPLAYON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_DisplayOff(void)
{
	HD44780_Opts.DisplayControl &= ~HD44780_DISPLAYON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_BlinkOn(void)
{
	HD44780_Opts.DisplayControl |= HD44780_BLINKON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_BlinkOff(void)
{
	HD44780_Opts.DisplayControl &= ~HD44780_BLINKON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_CursorOn(void)
{
	HD44780_Opts.DisplayControl |= HD44780_CURSORON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_CursorOff(void)
{
	HD44780_Opts.DisplayControl &= ~HD44780_CURSORON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_ScrollLeft(void)
{
	HD44780_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void HD44780_ScrollRight(void)
{
	HD44780_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

void HD44780_CreateChar(uint8_t location, uint8_t *data)
{
	uint8_t i;

	/* We have 8 locations available for custom characters */
	location &= 0x07;
	HD44780_Cmd(HD44780_SETCGRAMADDR | (location << 3));

	for (i = 0; i < 8; i++)
	{
		HD44780_Data(data[i]);
	}
}

void HD44780_PutCustom(uint8_t x, uint8_t y, uint8_t location)
{
	HD44780_CursorSet(x, y);
	HD44780_Data(location);
}

void HD44780_Cmd(uint8_t cmd)
{
	/* Command mode */
	HD44780_RS_LOW;

	/* High nibble */
	HD44780_Cmd4bit(cmd >> 4);
	/* Low nibble */
	HD44780_Cmd4bit(cmd & 0x0F);
}

void HD44780_Data(uint8_t data)
{
	/* Data mode */
	HD44780_RS_HIGH;

	/* High nibble */
	HD44780_Cmd4bit(data >> 4);
	/* Low nibble */
	HD44780_Cmd4bit(data & 0x0F);
}

void HD44780_Cmd4bit(uint8_t cmd)
{
	/* Set output port */
	if(cmd & 0x08)
		HD44780_D7_On();
	else
		HD44780_D7_Off();

	if(cmd & 0x04)
		HD44780_D6_On();
	else
		HD44780_D6_Off();

	if(cmd & 0x02)
		HD44780_D5_On();
	else
		HD44780_D5_Off();

	if(cmd & 0x01)
		HD44780_D4_On();
	else
		HD44780_D4_Off();

	HD44780_E_BLINK;
}

void HD44780_CursorSet(uint8_t col, uint8_t row)
{
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

	/* Go to beginning */
	if (row >= HD44780_Opts.Rows)
	{
		row = 0;
	}

	/* Set current column and row */
	HD44780_Opts.currentX = col;
	HD44780_Opts.currentY = row;

	/* Set location address */
	HD44780_Cmd(HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}








