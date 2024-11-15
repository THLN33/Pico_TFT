#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//#include "portable.h"
#include "def.h"

#if (INTERFACE_LCD == INTERFACE_LCD_SPI)

#define SPI_FREQUENCY   (125*1000*1000UL)  
//#define SPI_FREQUENCY   (62*5000*1000UL) 

#elif (INTERFACE_LCD == INTERFACE_LCD_SERIAL_PIO)

#define PIO_FREQUENCY   (125*1000*1000UL)
//#define PIO_FREQUENCY   (62*5000*1000UL)

#elif (INTERFACE_LCD == INTERFACE_LCD_PARALLEL_PIO)

#define PIO_FREQUENCY   (125*1000*1000UL)
#endif


void begin_tft_write(void);
void end_tft_write(void);
void writecommand(uint8_t c);
void writedata(uint8_t d);

uint8_t TFT_Hardware_Init(void);
void TFT_Blacklight(bool enable);

#ifdef __cplusplus
} /*extern "C"*/
#endif

