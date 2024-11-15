
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//#include "TFT_Drivers\portable.h"
#include "def.h"

/*
typedef enum {
    MODE_UNDEFINED = 0,
    MODE_RGB332_8bits = _MODE_RGB332_8bits,
    MODE_RGB332_256_Color = MODE_RGB332_8bits,
    MODE_RGB565_16bits = _MODE_RGB565_16bits,
    MODE_RGB332_65K_color = MODE_RGB565_16bits,
    MODE_RGB666_18bits = _MODE_RGB666_18bits,      
    MODE_RGB666_262K_color = MODE_RGB666_18bits,     
    MODE_RGB666_24bits = _MODE_RGB666_24bits,      
    MODE_RGB666_16M_color = MODE_RGB666_24bits,  
} Mode_LCD_def;
*/


int16_t width(void);
int16_t height(void);

void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void setPixel(uint16_t x, uint16_t y, uint32_t color);
void pushBlock(uint32_t color, uint32_t len);
void pushImage(const void *image, uint32_t len);

uint8_t Graphics_Init(void);

void setRotation(uint8_t m);

void DrawRectFilled(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
void DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const void *image);
void DrawPixel(uint16_t x, uint16_t y, uint32_t color);

void DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint32_t color);
void DrawCircle2(uint16_t x0, uint16_t y0, uint16_t r1, uint16_t r2, uint32_t color);

void circle2(int xc, int yc, int inner, int outer, int colour);


int8_t Load_Palette(const uint8_t *_palette, uint32_t len);

#ifdef __cplusplus
} /*extern "C"*/
#endif