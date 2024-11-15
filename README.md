# Pico_TFT



This code allows to control TFT screens based on the ILI9488 and GC9A01 chipsets. It implements the low-level layers and uses the hardware capabilities (PIO) of the Rapsberry Pi Pico processors. Connections can be in SPI or 8-bit parallel mode. DMA mode is supported in all color modes.

This code is an extension of the first tests published here: (https://github.com/THLN33/RP_PIO_SPI_16_24).

The configuration selected in the def.h file to choose the different color modes and the Pinout.h file allows the assignment of connections to the screen.

The supported color modes are as follows (see def.h file):
_MODE_Index_8bits, _MODE_RGB332_8bits, _MODE_RGB565_16bits
The 8-bit indexed mode optimizes RAM, the colors are chosen from a palette of 65K colors (identical to RGB565 mode).
In the 8-bit mode, the colors are defined in RGB332.
The colors in 16-bit RGB565 mode (i.e. 5 bits for red, 6 for green and 5 for blue).
The indexed mode requires the use of two additional DMA channels.

I also added the possibility of using the LVGL library (https://github.com/lvgl/lvgl), to do this you just have to uncomment the lines indicated in the CMakeLists.txt file to add LVGL features, and of course include the library code in the ./lvgl/ directory 
