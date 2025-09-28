#pragma once

#include QMK_KEYBOARD_H
#include "rgb_matrix.h"

void indicators_set_fn(bool pressed);
void indicators_set_layer2(bool pressed);
void indicators_set_layer3(bool pressed);
void indicators_set_socd_mode(uint8_t mode);
void indicators_trigger_nkro_feedback(bool nkro_enabled);
void indicators_trigger_socd_feedback(uint8_t mode);
void indicators_trigger_dfu_feedback(void);
void indicators_trigger_eeprom_feedback(void);