#ifndef LVGL_DISPLAY_H
#define LVGL_DISPLAY_H

#include "lv_conf.h"
#include "lvgl.h"
#include "tft_ili9341.h"

#ifdef __cplusplus
extern "C" {
#endif

lv_display_t *lvgl_display_init(const tft_ili9341_config_t *tft_config);

#ifdef __cplusplus
}
#endif

#endif