
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TFT_WIDTH
  #define TFT_WIDTH  240
#endif
#ifndef TFT_HEIGHT
  #define TFT_HEIGHT 240
#endif


#define TFT_COLOR       TFT_COLOR_16BITS

extern void GC9A01A_Init_Registers(void);
extern void GC9A01A_Rotation(uint8_t r, uint16_t *_width, uint16_t *_height);


#define TFT_Init_Registers() GC9A01A_Init_Registers()
#define TFT_Rotation(m, w, h) GC9A01A_Rotation(m, w, h)

#ifdef __cplusplus
} /*extern "C"*/
#endif