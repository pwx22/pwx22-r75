// Copyright 2024 SDK (@sdk66)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "rgb_matrix.h"

#define LED_ENABLE_PIN A5
#define LED_WIN_LOCK_PIN B9
#define LED_MAC_PIN B8

void startup_effect(void);
void loading_effect(void);

void keyboard_pre_init_kb(void) {
    gpio_set_pin_output(LED_ENABLE_PIN);
    gpio_write_pin_high(LED_ENABLE_PIN);
    // Call user initialization function if you have one
    keyboard_pre_init_user();
}
void matrix_init_user(void){
    startup_effect();
}
void startup_effect(void) {
    //to #f001ff (purple-pink)
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SPLASH);
    rgb_matrix_set_color(21, 0xF0, 0x01, 0xFF);
    wait_ms(2500);
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
}

void suspend_power_down_kb(void) {
    gpio_write_pin_low(LED_ENABLE_PIN);
    suspend_power_down_user();
}

void suspend_wakeup_init_kb(void) {
    gpio_write_pin_high(LED_ENABLE_PIN);
    suspend_wakeup_init_user();
}

void housekeeping_task_kb(void) {
    if (keymap_config.no_gui) {
        gpio_write_pin_low(LED_MAC_PIN);      // Turn on Mac LED
        gpio_write_pin_low(LED_WIN_LOCK_PIN);  // Turn on Win Lock LED
    } else {
        gpio_write_pin_high(LED_MAC_PIN);     // Turn off Mac LED
        gpio_write_pin_high(LED_WIN_LOCK_PIN); // Turn off Win Lock LED
    }
}
