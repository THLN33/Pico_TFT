#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"


#include "Drivers/Graphics.h"



#ifdef LVGL
#include "lvgl.h"
#include "lv_port_disp.h"

#define DEMO_LVGL
#endif

// https://javl.github.io/image2cpp/


void showPalette_256(void) {

    uint16_t x, x0 = (width()-160) / 2;
    uint16_t y, y0 = (height()-160) / 2;
    uint16_t w = 160;
    uint16_t h = 160;    

    for (uint c=0;c<256;c++) {
        x = x0 + (c % 16) * 10;
        y = y0 + (c / 16) * 10;

        uint32_t color = c;
        DrawRectFilled(x, y, 10, 10, color);
    }
}

void showPalette_65K(void) {

    uint16_t x, x0 = (width()-256) / 2;
    uint16_t y, y0 = (height()-256) / 2;
    uint16_t w = 256;
    uint16_t h = 256;    

    for (uint c=0;c<65536;c++) {
        x = x0 + (c % 256) * 1;
        y = y0 + (c / 256) * 1;

        uint32_t color = c;
        DrawRectFilled(x, y, 1, 1, color);
    }
//#endif
}


#if (MODE_RGB==_MODE_Index_8bits)
#include "image256.h"

/**
 * display image in 256 colors among 65K colors coded with RGB_565
 */
void demo(uint8_t *t) {
    static bool init=false;
    if (!init) {
        Load_Palette(img_Palette, sizeof(img_Palette));
        init = true;
    }
    if (*t<16) {
        showPalette_256();
    } else {
        DrawImage(0, 0, 240, 240, image1);
    }
}
#elif (MODE_RGB== _MODE_RGB332_8bits)
#include "image332.h"

/**
 * display image in 256 colors coded with RGB_332
 */
void demo(uint8_t *t) {
    if (*t<16) {
        showPalette_256();
    } else {
        DrawImage(0, 0, 240, 240, image1);
    }
}

#elif (MODE_RGB== _MODE_RGB565_16bits)
#include "image565.h"

/**
 * display image in 65K colors coded with RGB_565
 */
void demo(uint8_t *t) {
    if (*t<16) {
        //showPalette_256();
        showPalette_65K();
    } else {
        DrawImage(0, 0, 240, 240, image1);
    }
}

#else
#error no demo for this mode!
#endif

#ifdef DEMO_LVGL

void demo_lvgl(void) {

    // Change the active screen's background color
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x00000), LV_PART_MAIN);

    // Change the active screen's text color
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0x808080), LV_PART_MAIN);


    lv_obj_t * label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
                              "and wrap long text automatically.");
    lv_obj_set_width(label1, 180);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);


    lv_obj_t * label2 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_obj_set_width(label2, 200);
    lv_label_set_recolor(label2, true);  
    lv_label_set_text(label2, "It is a circularly scrolling text. #ff0000 red#, #00ff00 green#, #0000ff blue#");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 20);
}
#endif



int main(void) {

    stdio_init_all();

    if (!set_sys_clock_khz(250000, false)) {
        printf("system clock 250 MHz failed\n");
    }

    // Get the processor sys_clk frequency in Hz
    uint32_t freq = clock_get_hz(clk_sys);

    // clk_peri does not have a divider, so input and output frequencies will be the same
    clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    freq,
                    freq);


    printf("System Clock Frequency is %d Hz\n", clock_get_hz(clk_sys));
    printf("USB Clock Frequency is %d Hz\n", clock_get_hz(clk_usb));

    Graphics_Init();
    setRotation(1);

#ifdef DEMO_LVGL
    /* Initialize the graphical unit */
    lv_init();
    lv_port_disp_init();

    demo_lvgl();
#endif

    uint8_t t = 0;

    while (true) {


#ifdef DEMO_LVGL
        lv_task_handler();
        lv_tick_inc(1);
        sleep_ms(1);        
#else
        demo(&t);
      
        sleep_ms(100);
#endif
        t++;
        t &= 0x1F; 
    }

}