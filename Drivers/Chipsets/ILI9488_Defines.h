
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TFT_WIDTH
  #define TFT_WIDTH  320
#endif
#ifndef TFT_HEIGHT
  #define TFT_HEIGHT 480
#endif

#if (LCD_BOARD == LCD_ILI9488_S_BOARD)
#define TFT_COLOR       TFT_COLOR_18BITS
#elif (LCD_BOARD == LCD_ILI9488_P_BOARD)

#if (MODE_RGB==_MODE_Index_8bits) || (MODE_RGB==_MODE_RGB332_8bits) || (MODE_RGB==_MODE_RGB565_16bits)
#define TFT_COLOR       TFT_COLOR_16BITS
#elif (MODE_RGB==_MODE_RGB666_18bits)
#define TFT_COLOR       TFT_COLOR_24BITS
#endif

#endif

extern void ILI9488_Init_Registers(void);
extern void ILI9488_Rotation(uint8_t r, uint16_t *_width, uint16_t *_height);


#define TFT_Init_Registers() ILI9488_Init_Registers()
#define TFT_Rotation(m, w, h) ILI9488_Rotation(m, w, h)

#ifdef __cplusplus
} /*extern "C"*/
#endif