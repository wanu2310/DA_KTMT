#include "Hardware_Setup.h"
#include "Logcat.h"

ESP_Panel *panel = new ESP_Panel();

void touch_callback(esp_lcd_touch_handle_t tp_handle) {
    bool touched;
    uint16_t touch_x, touch_y;
    uint16_t strength;
    uint8_t touch_num;
    
    // Get touch state
    tp_handle->read_data(tp_handle);
    touched = tp_handle->get_xy(tp_handle, &touch_x, &touch_y, &strength, &touch_num, 10);
    
    if (touched) {
        LogI("Touch", String(touch_x) + ", " + String(touch_y));
    }
}

void setup_touch(ESP_PanelLcdTouch *touch) {
    // Register the callback
    esp_lcd_touch_register_interrupt_callback(touch->getHandle(), touch_callback);
}


void hardware_setup(){
    panel -> init();
    panel -> begin();
    setup_touch(panel -> getLcdTouch());
}