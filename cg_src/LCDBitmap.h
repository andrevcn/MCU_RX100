#ifndef LCDBitmap_h
#define LCDBitmap_h

#include "r_cg_macrodriver.h"

#ifndef ON
#define ON 1   // Color ON (LCD pixel active)
#endif

#ifndef OFF
#define OFF 0  // Color OFF (LCD pixel inactive)
#endif

#ifndef UPDATE
#define UPDATE 1
#endif

#ifndef NO_UPDATE
#define NO_UPDATE 0
#endif

#define BITMAP_CHAR 8    // Custom characters (don't change)
#define BITMAP_CHAR_W 5  // Character width in pixels (don't change)
#define BITMAP_CHAR_H 8  // Character height in pixels (this is sometimes 7 instead of 8)

// Deprecated commands
#define lineHor(x1, y1, x2, color, update) line(x1, y1, x2, y1, color, update)
#define lineVert(x1, y1, y2, color, update) line(x1, y1, x1, y2, color, update)

#define BITMAP_W BITMAP_CHAR_W * 4 // Bitmap pixels wide (4 character columns wide)
#define BITMAP_H BITMAP_CHAR_H * 2 // Bitmap pixels high (2 character rows high)


void LCDBitmap_Begin(uint8_t x, uint8_t y);
void LCDBitmap_clear(void);
void LCDBitmap_inverse(void);
void LCDBitmap_update(void);
void LCDBitmap_clear_text(void);
void LCDBitmap_home(void);
void LCDBitmap_move(uint8_t x, uint8_t y);
void LCDBitmap_pixel(uint8_t x, uint8_t y, uint8_t color, uint8_t update);
void LCDBitmap_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color, uint8_t update);
void LCDBitmap_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color, uint8_t update);
void LCDBitmap_rectFill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color, uint8_t update);
void LCDBitmap_barGraph(uint8_t bars, uint8_t *graph, uint8_t color, uint8_t update);
void LCDBitmap_updateChar(void);
void LCDBitmap_drawChar(void);


#endif