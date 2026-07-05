#include "lvgl_display.h"
#include "tft_ili9341.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

static const char *TAG = "LVGL_DISPLAY";

static uint8_t *disp_buf1 = NULL;
static uint8_t *disp_buf2 = NULL;

void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint16_t x_start = area->x1;
    uint16_t y_start = area->y1;
    uint16_t x_end = area->x2;
    uint16_t y_end = area->y2;
    
    uint16_t width = x_end - x_start + 1;
    uint16_t height = y_end - y_start + 1;
    
    tft_ili9341_set_window(x_start, y_start, x_end, y_end);
    
    uint32_t total_pixels = (uint32_t)width * height;
    
    const int BUFFER_SIZE = 512;
    uint16_t buffer[BUFFER_SIZE];
    
    uint32_t pixels_written = 0;
    while (pixels_written < total_pixels) {
        uint16_t batch_size = total_pixels - pixels_written;
        if (batch_size > BUFFER_SIZE) {
            batch_size = BUFFER_SIZE;
        }
        
        memcpy(buffer, px_map + pixels_written * 2, batch_size * 2);
        
        tft_write_data((uint8_t *)buffer, batch_size * 2);
        pixels_written += batch_size;
    }
    
    lv_display_flush_ready(disp);
}

lv_display_t *lvgl_display_init(const tft_ili9341_config_t *tft_config)
{
    esp_err_t ret = tft_ili9341_init(tft_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "TFT屏幕初始化失败: %s", esp_err_to_name(ret));
        return NULL;
    }
    
    ESP_LOGI(TAG, "分配LVGL显示缓冲区...");
    
    const size_t buf_size = TFT_WIDTH * 20 * 2;
    
    disp_buf1 = heap_caps_malloc(buf_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    disp_buf2 = heap_caps_malloc(buf_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    
    if (disp_buf1 == NULL || disp_buf2 == NULL) {
        ESP_LOGE(TAG, "分配显示缓冲区失败");
        if (disp_buf1) heap_caps_free(disp_buf1);
        if (disp_buf2) heap_caps_free(disp_buf2);
        return NULL;
    }
    
    ESP_LOGI(TAG, "初始化LVGL...");
    lv_init();
    
    lv_display_t *disp = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
    if (disp == NULL) {
        ESP_LOGE(TAG, "创建LVGL显示失败");
        heap_caps_free(disp_buf1);
        heap_caps_free(disp_buf2);
        return NULL;
    }
    
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565_SWAPPED);
    lv_display_set_flush_cb(disp, lvgl_flush_cb);
    lv_display_set_buffers(disp, 
        disp_buf1, 
        disp_buf2, 
        buf_size, 
        LV_DISPLAY_RENDER_MODE_PARTIAL
    );
    
    ESP_LOGI(TAG, "LVGL显示初始化成功");
    return disp;
}