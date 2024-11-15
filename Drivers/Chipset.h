#pragma once


//#include "def.h"

#if (LCD_BOARD == LCD_ILI9488_S_BOARD) || (LCD_BOARD == LCD_ILI9488_P_BOARD)
#include "Chipsets\ILI9488_Defines.h"
#elif (LCD_BOARD == LCD_1_28_BOARD )
#include "Chipsets\GC9A01_Defines.h"
#else
#error LCD_BOARD should be defined!
#endif