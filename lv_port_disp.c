#include "lv_port_disp.h"
#include "lv_conf.h"


#ifdef LVGL

volatile bool disp_flush_enabled = true;

/*Initialize your display and the required peripherals.*/
static void disp_init(void) {
    // Already initialized int main fonction
}




/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void) {
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void) {
    disp_flush_enabled = false;
}


/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map) {
    if (disp_flush_enabled) {
        begin_tft_write();

        uint32_t len = lv_area_get_width(area) * lv_area_get_height(area);

        setWindow(area->x1, area->y1, area->x2, area->y2);        

        pushImage(px_map, len);

        end_tft_write();
    }
    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp_drv);
}


LV_ATTRIBUTE_MEM_ALIGN
static uint8_t buf_1_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];            /*A buffer for 10 rows*/



void lv_port_disp_init(void) {
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);

    /* Example 1
     * One buffer for partial rendering*/
    lv_display_set_buffers(disp, buf_1_1, NULL, sizeof(buf_1_1), LV_DISPLAY_RENDER_MODE_PARTIAL);
}

#endif /* LVGL */

