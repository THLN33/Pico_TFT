#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "pico/stdlib.h"


#define LOW         0
#define HIGH        1


#define TFT_COLOR_1BITS     1
#define TFT_COLOR_4BITS     4
#define TFT_COLOR_12BITS    12
#define TFT_COLOR_16BITS    16
#define TFT_COLOR_18BITS    18
#define TFT_COLOR_24BITS    24

#define _MODE_Index_8bits       7
#define _MODE_RGB332_8bits      8
#define _MODE_RGB565_16bits     16
#define _MODE_RGB666_18bits     18
#define _MODE_RGB666_24bits     24

#define NO_ERR                  0
#define ERR_PIO_NO_AVAILABLE    -1

#define LCD_1_28_BOARD              0x0000C9A1
#define LCD_ILI9488_S_BOARD         0x00009488
#define LCD_ILI9488_P_BOARD         0x10009488

#define INTERFACE_LCD_SPI                       1
#define INTERFACE_LCD_SERIAL_PIO                2
#define INTERFACE_LCD_PARALLEL_PIO              4


#ifdef __cplusplus
} /*extern "C"*/
#endif