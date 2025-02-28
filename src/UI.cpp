#include "UI.h"
#include "lv_conf.h"
#include "Logcat.h"
#include "spiram.h"
#include "Hardware_Setup.h"
lv_display_t *disp;
static lv_obj_t *label;


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

#define BUFFER_SIZE   (SCREEN_WIDTH * 10)

void btn_event_handler(lv_event_t *e) {
    lv_label_set_text(label, "Button Pressed!");
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

   
    lv_display_flush_ready(disp);
}
// LVGL draw buffer
static lv_draw_buf_t draw_buf;
static lv_color_t *buf1;
static lv_color_t *buf2;

// LVGL flush callback
void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    panel->getLcd()->drawBitmap(area->x1, area->y1, area->x2 + 1, area->y2 + 1, px_map);
    lv_display_flush_ready(disp);
}

void check_memory() {
    if (esp_spiram_get_size() == 0) {
        LogI(__func__, "No PSRAM found");
        ESP_LOGE("PSRAM", "No PSRAM detected! Frame buffer may not fit in RAM.");
    } else {
        LogI(__func__, "PSRAM found");
        ESP_LOGE("PS", "PSRAM detected! Frame buffer will fit in RAM.");
    }
    LogI(__func__, String(esp_get_free_heap_size()));
    ESP_LOGI("MEM", "Free heap: %d bytes", esp_get_free_heap_size());
    ESP_LOGI("MEM", "Largest block in PSRAM: %d bytes", 
             heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
  }

void setup_ui(){
    lv_init();
    // Create LCD Bus

    LogI(__func__, "LCD Bus begin");
    // Allocate memory for LVGL buffers
    buf1 = (lv_color_t *)heap_caps_malloc(SCREEN_WIDTH * 50 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    buf2 = (lv_color_t *)heap_caps_malloc(SCREEN_WIDTH * 50 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    // LVGL display driver setup
    lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_buffers(disp, buf1, buf2, SCREEN_WIDTH * 50, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, lvgl_flush_cb);
    LogI(__func__, "LVGL display driver setup");
    // Create a simple UI

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello, ESP32-S3 with ST7262!");
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    LogI(__func__, "UI created");

    lv_obj_t *btn = lv_btn_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_align(btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    lv_obj_t *labelbtn = lv_label_create(btn); 
    lv_label_set_text(labelbtn, "Click Me");  // Set button text
    lv_obj_center(labelbtn);
}