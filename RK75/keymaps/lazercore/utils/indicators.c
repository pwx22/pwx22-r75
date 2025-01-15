#include "indicators.h"
#include "brightness.h"

bool rgb_matrix_indicators_user(void) {
    uint8_t layer = biton32(layer_state);
    uint8_t currentBrightness = rgblight_get_val();
    if (layer == 1) {
        adjust_global_brightness(currentBrightness/2);
        rgb_matrix_set_color(6, 0xFF, 0xFF, 0xFF); 
        set_brightness_scaled(5, 0xFF, 0xFF, 0xFF, 0.4);

        rgb_matrix_set_color(3, 0xE4, 0x00, 0xFF); 
        rgb_matrix_set_color(63, 0xE4, 0x00, 0xFF); 

        rgb_matrix_set_color(2, 0xFB, 0x98, 0xFF);  
        rgb_matrix_set_color(4, 0xFB, 0x98, 0xFF); 
        rgb_matrix_set_color(64, 0xFF, 0x00, 0x12); 
    }
    adjust_global_brightness(currentBrightness);
    rgb_matrix_sethsv(rgblight_get_hue(), rgblight_get_sat(), currentBrightness);
    if (layer == 2) {
        rgb_matrix_set_color_all(0x10, 0x10, 0x10);  // Full dim background
        rgb_matrix_set_color(21, 0xFF, 0x00, 0x00); // Game Mode color: red (#FF0000)
        rgb_matrix_set_color(55, 0xFF, 0x00, 0x00);  // Bootloader color: red (#FF0000)
    }
    return false;
}
