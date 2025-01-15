#pragma once

#include "rgb_matrix.h"

void set_brightness_scaled(uint8_t k,uint8_t r, uint8_t g, uint8_t b, float brightness_factor);
void adjust_global_brightness(uint8_t brightness);
