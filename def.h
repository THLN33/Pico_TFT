#pragma once

#include "Drivers/portable.h"

#define MODE_RGB  _MODE_Index_8bits
//#define MODE_RGB  _MODE_RGB332_8bits
//#define MODE_RGB  _MODE_RGB565_16bits
//#define MODE_RGB  _MODE_RGB666_18bits /* not implemented */


#define LCD_BOARD               LCD_ILI9488_P_BOARD


#if (LCD_BOARD == LCD_1_28_BOARD)
#define INTERFACE_LCD           INTERFACE_LCD_SERIAL_PIO
#elif (LCD_BOARD == LCD_ILI9488_S_BOARD)
#define INTERFACE_LCD           INTERFACE_LCD_SERIAL_PIO
#elif (LCD_BOARD == LCD_ILI9488_P_BOARD)
#define INTERFACE_LCD           INTERFACE_LCD_PARALLEL_PIO
#else
#error You must choose a board!
#endif







