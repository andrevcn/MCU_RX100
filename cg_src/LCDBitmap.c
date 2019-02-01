#include "LCDBitmap.h"
#include "HD44780.h"
#include <stdlib.h>

uint8_t bitmap_x;
uint8_t bitmap_y;
uint8_t bitmap[BITMAP_W][BITMAP_H];
uint8_t chr[BITMAP_CHAR][BITMAP_CHAR_H];


void LCDBitmap_Begin(uint8_t x, uint8_t y)
{
  bitmap_x = x;
  bitmap_y = y;

  LCDBitmap_clear();
  LCDBitmap_updateChar();
  LCDBitmap_drawChar();
}

void LCDBitmap_updateChar(void)
{
	for (uint8_t i=0; i<BITMAP_CHAR; i++)
	{
		HD44780_CreateChar(i, chr[i]);
	}

	LCDBitmap_drawChar();
}

void LCDBitmap_drawChar(void)
{
	HD44780_CursorSet(bitmap_x, bitmap_y);

	for (uint8_t i=0; i<BITMAP_CHAR; i++)
	{
		if (i==4)
			HD44780_CursorSet(bitmap_x, bitmap_y+1);

		HD44780_Data(i);
	}
}


void LCDBitmap_begin(void)
{
  LCDBitmap_clear();
  LCDBitmap_updateChar();
  LCDBitmap_drawChar();
}

void LCDBitmap_clear(void)
{
  for (uint8_t y=0; y<BITMAP_H; y++)
  {
	  for (uint8_t x=0; x<BITMAP_W; x++)
	  {
		  bitmap[x][y]=OFF;
	  }
  }

  for (uint8_t c=0; c<BITMAP_CHAR; c++)
  {
	  for (uint8_t a=0; a<BITMAP_CHAR_H; a++)
	  {
		  chr[c][a]=OFF;
	  }
  }

  LCDBitmap_updateChar();
}

void LCDBitmap_clear_text(void)
{
  LCDBitmap_clear();
  LCDBitmap_drawChar();
}

void LCDBitmap_home(void)
{
  	HD44780_CursorSet(0, 0);
}
void LCDBitmap_update(void)
{
  uint8_t shift_x;
  uint8_t shift_y=0;

  for (uint8_t c=0; c<BITMAP_CHAR; c++)
  {
    if (c<4)
		shift_x = c * BITMAP_CHAR_W;
    else
		shift_x = (c-4) * BITMAP_CHAR_W;

	if (c==4)
		shift_y=BITMAP_CHAR_H;

	for (uint8_t a=0; a<BITMAP_CHAR_H; a++)
	{
      chr[c][a]=bitmap[shift_x][shift_y+a]<<4;
      chr[c][a]+=bitmap[shift_x+1][shift_y+a]<<3;
      chr[c][a]+=bitmap[shift_x+2][shift_y+a]<<2;
      chr[c][a]+=bitmap[shift_x+3][shift_y+a]<<1;
      chr[c][a]+=bitmap[shift_x+4][shift_y+a];
    }
  }
  LCDBitmap_updateChar();
}

void LCDBitmap_inverse(void)
{
  for (uint8_t x=0; x<BITMAP_W; x++)
  {
    for (uint8_t y=0; y<BITMAP_H; y++)
	{
		bitmap[x][y]=bitmap[x][y]^1;
	}
  }
  LCDBitmap_update();
}

void LCDBitmap_move(uint8_t x, uint8_t y)
{
  HD44780_Puts(bitmap_x, bitmap_y, "    ");
  HD44780_CursorSet(bitmap_x, bitmap_y+1);
  HD44780_Puts(bitmap_x, bitmap_y+1,"    ");
  bitmap_x = x;
  bitmap_y = y;
  LCDBitmap_drawChar();
}

void LCDBitmap_pixel(uint8_t x, uint8_t y, uint8_t color, uint8_t update)
{
  bitmap[x][y]=color;

  if (update)
	  LCDBitmap_update();
}

void LCDBitmap_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color, uint8_t update) {

  // Vertical line (faster than diagonal line method)
  if (x1==x2)
  {
    if (y1 < y2)
	{
      for (y1=y1; y1<=y2; y1++)
	  {
		  bitmap[x1][y1]=color;
	  }
    }
	else
	{
      for (y2=y2; y2<=y1; y2++)
	  {
		  bitmap[x1][y2]=color;
	  }
    }
  // Horizontal line (faster than diagonal line method)
  }
  else if (y1==y2)
  {
    if (x1 < x2)
	{
      for (x1=x1; x1<=x2; x1++)
	  {
		  bitmap[x1][y1]=color;
	  }
    }
	else
	{
      for (x2=x2; x2<=x1; x2++)
	  {
		  bitmap[x2][y1]=color;
	  }
    }
  // Diagonal line
  }
  else
  {
    uint8_t dx, dy;
    int8_t sx, sy;
    int err, e2;
    dx = abs(x2-x1);
    dy = abs(y2-y1);
    err = dx-dy;

    if (x1 < x2)
		sx = 1;
	else
		sx = -1;

    if (y1 < y2)
		sy = 1;
	else
		sy = -1;

    while (1)
	{
      bitmap[x1][y1]=color;
      if (x1 == x2 && y1 == y2) break;
      e2 = 2*err;

      if (e2 > -dy)
	  {
        err -= dy;
        x1 += sx;
      }

      if (e2 <  dx)
	  {
        err += dx;
        y1 += sy ;
      }
    }
  }
  if (update)
	  LCDBitmap_update();
}

void LCDBitmap_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color, uint8_t update)
{
  LCDBitmap_line(x1, y1, x2, y1, color, NO_UPDATE);
  LCDBitmap_line(x1, y2, x2, y2, color, NO_UPDATE);
  LCDBitmap_line(x1, y1, x1, y2, color, NO_UPDATE);
  LCDBitmap_line(x2, y1, x2, y2, color, NO_UPDATE);

  if (update)
	  LCDBitmap_update();
}

void LCDBitmap_rectFill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color, uint8_t update)
{
	uint8_t y;
	int8_t sx, sy;

	if (x1 < x2)
		sx = 1;
	else
		sx = -1;

	if (y1 < y2)
		sy = 1;
	else
		sy = -1;

	while (1)
	{
		y=y1;
		while (1)
		{
			bitmap[x1][y]=color;

			if (y == y2)
			{
				break;
			}

			y+=sy;
		}

		if (x1 == x2)
		{
			break;
		}

		x1 = x1 + sx;
	}

	if (update)
		LCDBitmap_update();
}

void LCDBitmap_barGraph(uint8_t bars, uint8_t *graph, uint8_t color, uint8_t update)
{
  uint8_t x1, x2, b, alt_color;
  uint8_t bar_W = BITMAP_W/bars;

  if (color == OFF)
	  alt_color=ON;
  else
	  alt_color=OFF;

  for (b=0; b<bars; b++)
  {
    x1 = b*bar_W;
    x2 = (b+1)*bar_W-1;

    if (graph[b]>0)
	{
      LCDBitmap_rectFill(x1, BITMAP_H-1, x2, BITMAP_H-graph[b], color, NO_UPDATE);
    }

    if (graph[b]<BITMAP_H)
	{
      LCDBitmap_rectFill(x1, 0, x2, BITMAP_H-graph[b]-1, alt_color, NO_UPDATE);
    }
  }

  if (update)
	  LCDBitmap_update();

}







