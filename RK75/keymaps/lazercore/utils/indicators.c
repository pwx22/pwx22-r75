#include "indicators.h"
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);

    for (uint8_t i = led_min; i < led_max; i++) {
        hsv_t global_hsv = rgb_matrix_get_hsv();
        if (layer == 1) {
            if (i == 6) {
                rgb_matrix_set_color(i, 0xFF, 0xFF, 0xFF);
            } else if (i == 3 || i == 63) {  // Keys 3, 63: Purple
                rgb_matrix_set_color(i, 0xE4, 0x00, 0xFF);
            } else if (i == 2 || i == 4) {  // Keys 2, 4: Pink
                rgb_matrix_set_color(i, 0xFB, 0x98, 0xFF);
            } else if (i == 64) {  // Key 64: Red
                rgb_matrix_set_color(i, 0xFF, 0x00, 0x12);
            } else{
                hsv_t dimmed_hsv = global_hsv;
                dimmed_hsv.v = dimmed_hsv.v / 2;
                rgb_t rgb = hsv_to_rgb(dimmed_hsv);
                rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
            }
        }

        if (layer == 2) {
            if (i == 21 || i == 55) {  // Keys 21, 55: Bright Red
                rgb_matrix_set_color(i, 0xFF, 0x00, 0x00);
            }else{
                hsv_t dimmed_hsv = global_hsv;
                dimmed_hsv.v = dimmed_hsv.v / 3;
                rgb_t rgb = hsv_to_rgb(dimmed_hsv);
                rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
            }
        }
    }
    if(layer==3){
        handle_audio_viz(led_min, led_max);
    }
     
    return false;
}