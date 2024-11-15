/*
from : https://github.com/Bodmer/TFT_eSPI
*/


#include "def.h"

#if (LCD_BOARD == LCD_ILI9488_S_BOARD) || (LCD_BOARD == LCD_ILI9488_P_BOARD)

#include "Generics_Regs.h"
#include "ILI9488_Defines.h"


extern void writecommand(uint8_t c);
extern void writedata(uint8_t d);




void ILI9488_Init_Registers(void) {
    // Configure ILI9488 display
    writecommand(0xE0);     // Positive Gamma Control
    writedata(0x00);        // 00
    writedata(0x03);        // 07
    writedata(0x09);        // 10
    writedata(0x08);        // 09
    writedata(0x16);        // 17
    writedata(0x0A);        // 0B
    writedata(0x3F);        // 41
    writedata(0x78);        // 89
    writedata(0x4C);        // 4B
    writedata(0x09);        // 0A
    writedata(0x0A);        // 0C
    writedata(0x08);        // 0E
    writedata(0x16);        // 18
    writedata(0x1A);        // 1B
    writedata(0x0F);        // 0F

    writecommand(0XE1);     // Negative Gamma Control
    writedata(0x00);        // 00
    writedata(0x16);        // 17
    writedata(0x19);        // 1A
    writedata(0x03);        // 04
    writedata(0x0F);        // 0E
    writedata(0x05);        // 06
    writedata(0x32);        // 2F
    writedata(0x45);        // 45
    writedata(0x46);        // 43
    writedata(0x04);        // 02
    writedata(0x0E);        // 0A
    writedata(0x0D);        // 09
    writedata(0x35);        // 32
    writedata(0x37);        // 36
    writedata(0x0F);        // 0F

    writecommand(0XC0);     // Power Control 1
    writedata(0x17);        // 11
    writedata(0x15);        // 09

    writecommand(0xC1);     // Power Control 2
    writedata(0x41);

    writecommand(0xC5);     // VCOM Control
    writedata(0x00);        // 00
    writedata(0x12);        // 28
    writedata(0x80);        // 80

    writecommand(TFT_MADCTL); // Memory Access Control
    //writedata(0x48);          // MX, BGR
    writedata(0x08);


    writecommand(0x3A);     // set Pixel Format Set
#if (LCD_BOARD == LCD_ILI9488_S_BOARD)

#if (TFT_COLOR == TFT_COLOR_1BITS)  
    writedata(0x01);        // 3 bits / pixel
#elif (TFT_COLOR == TFT_COLOR_4BITS)   
#error 4 bits Colors depth not supported 
#elif (TFT_COLOR == TFT_COLOR_12BITS)    
#error 12 bits Colors depth not supported 
    writedata(0x03);        // 12 bits / pixel
#elif (TFT_COLOR == TFT_COLOR_16BITS)   
    writedata(0x05);        // 16 bits / pixel 
#elif (TFT_COLOR == TFT_COLOR_18BITS)
    writedata(0x66);        // 18 bits / pixel
#elif (TFT_COLOR == TFT_COLOR_24BITS)
    writedata(0x77);        // 24 bits / pixel
#else
#error Colors depth should be defined!
#endif    
#elif (LCD_BOARD == LCD_ILI9488_P_BOARD)

#if (TFT_COLOR == TFT_COLOR_16BITS)   
    writedata(0x65);        // 16 bits / pixel 
#elif (TFT_COLOR == TFT_COLOR_18BITS)
    writedata(0x66);        // 18 bits / pixel
#elif (TFT_COLOR == TFT_COLOR_24BITS)
    writedata(0x77);        // 24 bits / pixel
#else
#error Colors depth should be defined!
#endif    

#endif
/*
    writecommand(0x3A); // Pixel Interface Format
#if defined (TFT_PARALLEL_8_BIT) || defined (TFT_PARALLEL_16_BIT) || defined (RPI_DISPLAY_TYPE)
    //writedata(0x55);  // 16-bit colour for parallel
#else
    //writedata(0x66);  // 18-bit colour for SPI
#endif
writedata(0x66); 
*/

    writecommand(0xB0); // Interface Mode Control
    writedata(0x00);

    writecommand(0xB1); // Frame Rate Control
    //writedata(0xA0);
    writedata(0xB0);        // *
    writedata(0x11);        // *

    writecommand(0xB4);     // Display Inversion Control
    writedata(0x02);

    writecommand(0xB6);     // Display Function Control
    writedata(0x02);
    writedata(0x02);
    writedata(0x3B);

    writecommand(0xB7);     // Entry Mode Set
    writedata(0xC6);

	writecommand(0xE9);     // Set Image Function
	writedata(0x00);

    writecommand(0xF7);     // Adjust Control 3
    writedata(0xA9);
    writedata(0x51);
    writedata(0x2C);
    writedata(0x82);    

    writecommand(TFT_SLPOUT);  //Exit Sleep
    sleep_ms(120);

    writecommand(TFT_DISPON);  //Display on
    sleep_ms(25);

    // End of ILI9488 display configuration
}





void ILI9488_Rotation(uint8_t r, uint16_t *_width, uint16_t *_height) {

    writecommand(TFT_MADCTL);
    switch (r) {
    case 0: // Portrait
        writedata(TFT_MAD_BGR);
        writedata(TFT_MAD_MX | TFT_MAD_BGR);
        *_width  = TFT_WIDTH;
        *_height = TFT_HEIGHT;   
        break;
    case 1: // Landscape (Portrait + 90)
        writedata(TFT_MAD_MV | TFT_MAD_BGR);
        *_width  = TFT_HEIGHT;
        *_height = TFT_WIDTH;
        break;
    case 2: // Inverter portrait
        writedata(TFT_MAD_MY | TFT_MAD_BGR);
        *_width  = TFT_WIDTH;
        *_height = TFT_HEIGHT;
        break;
    case 3: // Inverted landscape
        writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_MV | TFT_MAD_BGR);
        *_width  = TFT_HEIGHT;
        *_height = TFT_WIDTH;
        break;
    }
}


#endif /* LCD_BOARD == LCD_ILI9488_BOARD */