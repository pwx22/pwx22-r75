#include "brightness.h"

void set_brightness_scaled(uint8_t k, uint8_t r, uint8_t g, uint8_t b, float brightness_factor) {
    uint8_t scaled_r = (uint8_t)(r * brightness_factor);
    uint8_t scaled_g = (uint8_t)(g * brightness_factor);
    uint8_t scaled_b = (uint8_t)(b * brightness_factor);
    rgb_matrix_set_color(k, scaled_r, scaled_g, scaled_b);
}

void adjust_global_brightness(uint8_t brightness) {
    rgb_matrix_sethsv(rgb_matrix_config.hsv.h, rgb_matrix_config.hsv.s, brightness);
    if (!rgb_matrix_is_enabled()) {
        rgb_matrix_enable();
    }
}