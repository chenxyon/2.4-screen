set(COMPONENT_REQUIRES fatfs)

idf_component_register(SRCS "main.c" "lv_font_chinese_longyin.c" "w25q128.c"
                       INCLUDE_DIRS "."
                       REQUIRES "esp_lcd" "driver" "version_manager" "lvgl" "spi_flash" "app_update")

fatfs_create_rawflash_image(internal_storage ${CMAKE_CURRENT_SOURCE_DIR}/../fatfs_image FLASH_IN_PROJECT)