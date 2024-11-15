
#pragma once

#ifdef LVGL

#ifdef __cplusplus
extern "C" {
#endif


#include "lvgl/lvgl.h"

#include "def.h"

#include "Drivers\Chipset.h"
#include "Drivers\Interface.h"
#include "Drivers\Graphics.h"

#define MY_DISP_HOR_RES     TFT_HEIGHT
#define MY_DISP_VER_RES     TFT_WIDTH  

#if (MODE_RGB== _MODE_RGB332_8bits) || (MODE_RGB==_MODE_Index_8bits)
#define BYTE_PER_PIXEL 1
#warning set LV_COLOR_DEPTH 8 in lv_conf.h
#else
#define BYTE_PER_PIXEL 2
#warning set LV_COLOR_DEPTH 16 in lv_conf.h
#endif






/**********************
 * GLOBAL PROTOTYPES
 **********************/
/* Initialize low level display driver */
void lv_port_disp_init(void);

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void);

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LVGL */