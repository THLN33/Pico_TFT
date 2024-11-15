/*
from : https://github.com/Bodmer/TFT_eSPI
*/


#include "def.h"

#if (LCD_BOARD == LCD_1_28_BOARD)

#include "Generics_Regs.h"
#include "GC9A01_Defines.h"


extern void writecommand(uint8_t c);
extern void writedata(uint8_t d);


// This is the command sequence that initialises the GC9A01 driver

void GC9A01A_Init_Registers(void) {
    writecommand(0xEF);
    writecommand(0xEB);
    writedata(0x14);

    writecommand(0xFE);
    writecommand(0xEF);

    writecommand(0xEB);
    writedata(0x14);

    writecommand(0x84);
    writedata(0x40);

    writecommand(0x85);
    writedata(0xFF);

    writecommand(0x86);
    writedata(0xFF);

    writecommand(0x87);
    writedata(0xFF);

    writecommand(0x88);
    writedata(0x0A);

    writecommand(0x89);
    writedata(0x21);

    writecommand(0x8A);
    writedata(0x00);

    writecommand(0x8B);
    writedata(0x80);

    writecommand(0x8C);
    writedata(0x01);

    writecommand(0x8D);
    writedata(0x01);

    writecommand(0x8E);
    writedata(0xFF);

    writecommand(0x8F);
    writedata(0xFF);

    writecommand(0xB6);
    writedata(0x00);
    writedata(0x20);

    writecommand(0x3A);
    writedata(0x05);

    writecommand(0x90);
    writedata(0x08);
    writedata(0x08);
    writedata(0x08);
    writedata(0x08);

    writecommand(0xBD);
    writedata(0x06);

    writecommand(0xBC);
    writedata(0x00);

    writecommand(0xFF);
    writedata(0x60);
    writedata(0x01);
    writedata(0x04);

    writecommand(0xC3);
    writedata(0x13);
    writecommand(0xC4);
    writedata(0x13);

    writecommand(0xC9);
    writedata(0x22);

    writecommand(0xBE);
    writedata(0x11);

    writecommand(0xE1);
    writedata(0x10);
    writedata(0x0E);

    writecommand(0xDF);
    writedata(0x21);
    writedata(0x0c);
    writedata(0x02);

    writecommand(0xF0);
    writedata(0x45);
    writedata(0x09);
    writedata(0x08);
    writedata(0x08);
    writedata(0x26);
    writedata(0x2A);

    writecommand(0xF1);
    writedata(0x43);
    writedata(0x70);
    writedata(0x72);
    writedata(0x36);
    writedata(0x37);
    writedata(0x6F);

    writecommand(0xF2);
    writedata(0x45);
    writedata(0x09);
    writedata(0x08);
    writedata(0x08);
    writedata(0x26);
    writedata(0x2A);

    writecommand(0xF3);
    writedata(0x43);
    writedata(0x70);
    writedata(0x72);
    writedata(0x36);
    writedata(0x37);
    writedata(0x6F);

    writecommand(0xED);
    writedata(0x1B);
    writedata(0x0B);

    writecommand(0xAE);
    writedata(0x77);

    writecommand(0xCD);
    writedata(0x63);

    writecommand(0x70);
    writedata(0x07);
    writedata(0x07);
    writedata(0x04);
    writedata(0x0E);
    writedata(0x0F);
    writedata(0x09);
    writedata(0x07);
    writedata(0x08);
    writedata(0x03);

    writecommand(0xE8);
    writedata(0x34);

    writecommand(0x62);
    writedata(0x18);
    writedata(0x0D);
    writedata(0x71);
    writedata(0xED);
    writedata(0x70);
    writedata(0x70);
    writedata(0x18);
    writedata(0x0F);
    writedata(0x71);
    writedata(0xEF);
    writedata(0x70);
    writedata(0x70);

    writecommand(0x63);
    writedata(0x18);
    writedata(0x11);
    writedata(0x71);
    writedata(0xF1);
    writedata(0x70);
    writedata(0x70);
    writedata(0x18);
    writedata(0x13);
    writedata(0x71);
    writedata(0xF3);
    writedata(0x70);
    writedata(0x70);

    writecommand(0x64);
    writedata(0x28);
    writedata(0x29);
    writedata(0xF1);
    writedata(0x01);
    writedata(0xF1);
    writedata(0x00);
    writedata(0x07);

    writecommand(0x66);
    writedata(0x3C);
    writedata(0x00);
    writedata(0xCD);
    writedata(0x67);
    writedata(0x45);
    writedata(0x45);
    writedata(0x10);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);

    writecommand(0x67);
    writedata(0x00);
    writedata(0x3C);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0x54);
    writedata(0x10);
    writedata(0x32);
    writedata(0x98);

    writecommand(0x74);
    writedata(0x10);
    writedata(0x85);
    writedata(0x80);
    writedata(0x00);
    writedata(0x00);
    writedata(0x4E);
    writedata(0x00);

    writecommand(0x98);
    writedata(0x3e);
    writedata(0x07);

    writecommand(0x35);
    writecommand(0x21);

    writecommand(0x11);
    sleep_ms(120);
    writecommand(0x29);
    sleep_ms(20);
}


void GC9A01A_Init_Registers_zz(void) {

    writecommand(0xB6);     // Display Function Control 
    writedata(0x00);
    writedata(0x20);

    writecommand(0x3A);     // set Pixel Format Set
#if (TFT_COLOR == TFT_COLOR_1BITS)   
#error 4 bit Colors depth not supported 
    //writedata(0x01);        // 3 bits / pixel
#elif (TFT_COLOR == TFT_COLOR_4BITS)   
//#error 4 bit Colors depth not supported 
#elif (TFT_COLOR == TFT_COLOR_12BITS)    
    writedata(0x03);        // 12 bits / pixel
#elif (TFT_COLOR == TFT_COLOR_16BITS)   
    writedata(0x05);        // 16 bits / pixel 
#elif (TFT_COLOR == TFT_COLOR_18BITS)
    writedata(0x06);        // 18 bits / pixel
#elif (TFT_COLOR == TFT_COLOR_24BITS)
#error 24 bit Colors depth not supported 
#else
#error Colors depth should be defined!
#endif 

#ifdef _Z
    writecommand(0xB2);         // ???
    writedata(0x0C);
    writedata(0x0C);
    writedata(0x00);
    writedata(0x33);
    writedata(0x33);

    writecommand(0xB7);         // ???
    writedata(0x35);

    writecommand(0xBB);         // ???
    writedata(0x35);

    writecommand(0xC0);
    writedata(0x2C);
#endif
    writecommand(0xC2);         // Power Control 1
    writedata(0x00);            // default

    writecommand(0xC3);         // Power Control 2 
    writedata(0x13);

    writecommand(0xC4);         // Power Control 3
    writedata(0x13);

    writecommand(0xC9);         // Power Control 4
    writedata(0x22);

#ifdef _SET_GAMMA
    writecommand(0xF0);     // SET_GAMMA1 
    writedata(0x45);
    writedata(0x09);
    writedata(0x08);
    writedata(0x08);
    writedata(0x26);
    writedata(0x2A);

    writecommand(0xF1);     // SET_GAMMA2 
    writedata(0x43);
    writedata(0x70);
    writedata(0x72);
    writedata(0x36);
    writedata(0x37);
    writedata(0x6F);

    writecommand(0xF2);     // SET_GAMMA3 
    writedata(0x45);
    writedata(0x09);
    writedata(0x08);
    writedata(0x08);
    writedata(0x26);
    writedata(0x2A);

    writecommand(0xF3);     // SET_GAMMA4
    writedata(0x43);
    writedata(0x70);
    writedata(0x72);
    writedata(0x36);
    writedata(0x37);
    writedata(0x6F);
#endif /* _SET_GAMMA */

    writecommand(0xE8);     // Frame Rate
    writedata(0x34);

#ifdef _Z

    writecommand(0xC6);
    writedata(0x0F);

    writecommand(0xD0);
    writedata(0xA4);
    writedata(0xA1);

    writecommand(0xD6);
    writedata(0xA1);

    writecommand(0xE0);
    writedata(0xF0);
    writedata(0x00);
    writedata(0x04);
    writedata(0x04);
    writedata(0x04);
    writedata(0x05);
    writedata(0x29);
    writedata(0x33);
    writedata(0x3E);
    writedata(0x38);
    writedata(0x12);
    writedata(0x12);
    writedata(0x28);
    writedata(0x30);

    writecommand(0xE1);
    writedata(0xF0);
    writedata(0x07);
    writedata(0x0A);
    writedata(0x0D);
    writedata(0x0B);
    writedata(0x07);
    writedata(0x28);
    writedata(0x33);
    writedata(0x3E);
    writedata(0x36);
    writedata(0x14);
    writedata(0x14);
    writedata(0x29);
    writedata(0x32);
#endif

    writecommand(TFT_INVON);

    writecommand(TFT_SLPOUT);
    sleep_ms(120);
    writecommand(TFT_DISPON);

}

// This is the command sequence that initialises the GC9A01 driver
void GC9A01A_Init_Registers_zzz(void) {

    writecommand(0xFE);     // Inter Register Enable1
    writecommand(0xEF);     // Inter Register Enable2

#ifdef _Z
    writecommand(0xEB); // ???
    writedata(0x14);

    writecommand(0x84); // ??
    writedata(0x40);

    writecommand(0x85); // ??
    writedata(0xFF);

    writecommand(0x86); //
    writedata(0xFF);

    writecommand(0x87);
    writedata(0xFF);

    writecommand(0x88);
    writedata(0x0A);

    writecommand(0x89);
    writedata(0x21);

    writecommand(0x8A);
    writedata(0x00);

    writecommand(0x8B);
    writedata(0x80);

    writecommand(0x8C);
    writedata(0x01);

    writecommand(0x8D);
    writedata(0x01);

    writecommand(0x8E);
    writedata(0xFF);

    writecommand(0x8F);
    writedata(0xFF);

    writecommand(0x90);
    writedata(0x08);
    writedata(0x08);
    writedata(0x08);
    writedata(0x08);

    writecommand(0xBD);
    writedata(0x06);

    writecommand(0xBC);
    writedata(0x00);    

    writecommand(0xFF);
    writedata(0x60);
    writedata(0x01);
    writedata(0x04);    

        writecommand(0xBE);
    writedata(0x11);

        writecommand(0xE1);
    writedata(0x10);
    writedata(0x0E);

        writecommand(0xDF);
    writedata(0x21);
    writedata(0x0c);
    writedata(0x02);

    writecommand(0xED);
    writedata(0x1B);
    writedata(0x0B);

        writecommand(0xAE);
    writedata(0x77);

        writecommand(0xCD);
    writedata(0x63);

    



    writecommand(0x70);
    writedata(0x07);
    writedata(0x07);
    writedata(0x04);
    writedata(0x0E);
    writedata(0x0F);
    writedata(0x09);
    writedata(0x07);
    writedata(0x08);
    writedata(0x03);

        writecommand(0x62);
    writedata(0x18);
    writedata(0x0D);
    writedata(0x71);
    writedata(0xED);
    writedata(0x70);
    writedata(0x70);
    writedata(0x18);
    writedata(0x0F);
    writedata(0x71);
    writedata(0xEF);
    writedata(0x70);
    writedata(0x70);

    writecommand(0x63);
    writedata(0x18);
    writedata(0x11);
    writedata(0x71);
    writedata(0xF1);
    writedata(0x70);
    writedata(0x70);
    writedata(0x18);
    writedata(0x13);
    writedata(0x71);
    writedata(0xF3);
    writedata(0x70);
    writedata(0x70);

    writecommand(0x64);
    writedata(0x28);
    writedata(0x29);
    writedata(0xF1);
    writedata(0x01);
    writedata(0xF1);
    writedata(0x00);
    writedata(0x07);

    writecommand(0x66);
    writedata(0x3C);
    writedata(0x00);
    writedata(0xCD);
    writedata(0x67);
    writedata(0x45);
    writedata(0x45);
    writedata(0x10);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);

    writecommand(0x67);
    writedata(0x00);
    writedata(0x3C);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0x54);
    writedata(0x10);
    writedata(0x32);
    writedata(0x98);

    writecommand(0x74);
    writedata(0x10);
    writedata(0x85);
    writedata(0x80);
    writedata(0x00);
    writedata(0x00);
    writedata(0x4E);
    writedata(0x00);

    writecommand(0x98);
    writedata(0x3e);
    writedata(0x07);    
#endif



    writecommand(0x3A);     // set Pixel Format Set
#if (TFT_COLOR == TFT_COLOR_1BITS)   
#error 4 bit Colors depth not supported 
    //writedata(0x01);        // 3 bits / pixel
#elif (TFT_COLOR == TFT_COLOR_4BITS)   
//#error 4 bit Colors depth not supported 
#elif (TFT_COLOR == TFT_COLOR_12BITS)    
    writedata(0x03);        // 12 bits / pixel
#elif (TFT_COLOR == TFT_COLOR_16BITS)   
    writedata(0x65);        // 16 bits / pixel 
#elif (TFT_COLOR == TFT_COLOR_18BITS)
    writedata(0x66);        // 18 bits / pixel
#elif (TFT_COLOR == TFT_COLOR_24BITS)
#error 24 bit Colors depth not supported 
#else
#error Colors depth should be defined!
#endif    

    writecommand(0xC3);     // Power Control 2 
    writedata(0x13);

    writecommand(0xC4);     // Power Control 3
    writedata(0x13);

    writecommand(0xC9);     // Power Control 4
    writedata(0x22);

    writecommand(0xF0);     // SET_GAMMA1 
    writedata(0x45);
    writedata(0x09);
    writedata(0x08);
    writedata(0x08);
    writedata(0x26);
    writedata(0x2A);

    writecommand(0xF1);     // SET_GAMMA2 
    writedata(0x43);
    writedata(0x70);
    writedata(0x72);
    writedata(0x36);
    writedata(0x37);
    writedata(0x6F);

    writecommand(0xF2);     // SET_GAMMA3 
    writedata(0x45);
    writedata(0x09);
    writedata(0x08);
    writedata(0x08);
    writedata(0x26);
    writedata(0x2A);

    writecommand(0xF3);     // SET_GAMMA4
    writedata(0x43);
    writedata(0x70);
    writedata(0x72);
    writedata(0x36);
    writedata(0x37);
    writedata(0x6F);

    writecommand(0xE8);     // Frame Rate
    writedata(0x34);


    writecommand(0x35);     // Tearing Effect Line ON
    //..

    writecommand(TFT_INVON);

    writecommand(TFT_SLPOUT);
    sleep_ms(120);
    writecommand(TFT_DISPON);
    sleep_ms(20);
}


void GC9A01A_Rotation(uint8_t r, uint16_t *_width, uint16_t *_height) {
/*
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
*/

    writecommand(TFT_MADCTL);
    switch (r) {
    case 0: // Portrait
        writedata(TFT_MAD_BGR);
        *_width  = TFT_WIDTH;
        *_height = TFT_HEIGHT;   
        break;
    case 1: // Landscape (Portrait + 90)
        writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR);
        *_width  = TFT_HEIGHT;
        *_height = TFT_WIDTH;
        break;
    case 2: // Inverter portrait
        writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_BGR);
        *_width  = TFT_WIDTH;
        *_height = TFT_HEIGHT;
        break;
    case 3: // Inverted landscape
        writedata(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_BGR);
        *_width  = TFT_HEIGHT;
        *_height = TFT_WIDTH;
        break;
    }
}

#endif /* LCD_BOARD == LCD_1_28_BOARD */