#pragma once

#include QMK_KEYBOARD_H
#include <stdbool.h>

typedef enum {
    SOCD_MODE_LAST = 0,
    SOCD_MODE_NEUTRAL,
    SOCD_MODE_FIRST,
} socd_mode_t;

void indicators_set_sentence_case(bool enabled);
void indicators_set_winlock(bool enabled);
void indicators_set_socd_mode(socd_mode_t mode, bool trigger_feedback);
void indicators_set_nkro(bool enabled, bool trigger_feedback);
void indicators_trigger_dfu_feedback(void);
void indicators_trigger_eeprom_feedback(void);
void indicators_set_night_hsv(HSV hsv);
void indicators_set_night_enabled(bool enabled);
