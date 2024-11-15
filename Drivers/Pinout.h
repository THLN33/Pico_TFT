#pragma once


#if (LCD_BOARD == LCD_ILI9488_S_BOARD)

#define TFT_RST     8  // Reset pin (could connect to RST pin)
#define TFT_DC      9  // Data Command control pin
#define TFT_SCLK    10
#define TFT_MOSI    11
#define TFT_MISO    12
#define TFT_CS      13  // Chip select control pin
#define TFT_LED     14  // tft backlight

#endif  /* LCD_BOARD == LCD_ILI9488_S_BOARD */


#if (LCD_BOARD == LCD_ILI9488_P_BOARD)

#define TFT_RST     2   // Reset pin (could connect to RST pin)
#define TFT_CS      3   // Chip select control pin
#define TFT_WR      4   // Write strobe control pin
#define TFT_DC      5   // Data Command control pin
#define TFT_D0      6   // Data Bus pin ...
#define TFT_D1      7
#define TFT_D2      8
#define TFT_D3      9
#define TFT_D4      10
#define TFT_D5      11
#define TFT_D6      12
#define TFT_D7      13

#endif  /* LCD_BOARD == LCD_ILI9488_P_BOARD */

#if (LCD_BOARD == LCD_1_28_BOARD)

#define TFT_DC      8
#define TFT_CS	    9
#define TFT_SCLK    10
#define TFT_MOSI    11
#define TFT_RST	    12
#define TFT_LED	    25

#endif  /* LCD_BOARD == LCD_1_28_BOARD */

