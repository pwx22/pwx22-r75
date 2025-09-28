#pragma once

#include QMK_KEYBOARD_H
#include "rgb_matrix.h"
#include "audio_viz.h"

void lazercore_rgb_note_frow_press(uint8_t row, uint8_t col);
void lazercore_rgb_flash_nkro(void);
void lazercore_rgb_flash_socd(void);
void lazercore_rgb_flash_dfu(void);
void lazercore_rgb_flash_eeprom(void);
bool lazercore_rgb_get_frow_flash(uint8_t led, rgb_t *out);
bool lazercore_rgb_get_global_flash(rgb_t *out);

