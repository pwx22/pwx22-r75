// Copyright 2024 SDK (@sdk66)
// SPDX-License-Identifier: GPL-2.0-or-later
#define LED_MAC_PIN B8
#include QMK_KEYBOARD_H

enum custom_keycodes {
  GAME_MODE = SAFE_RANGE,
};

socd_cleaner_t socd_v = {{KC_W, KC_S}, SOCD_CLEANER_LAST};
socd_cleaner_t socd_h = {{KC_A, KC_D}, SOCD_CLEANER_LAST};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT( /* Base */
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_HOME,   KC_MUTE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_DEL,  
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_PGUP,    
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,   KC_PGDN,          
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LCMD,  KC_LALT,                      KC_SPC,                                 KC_RALT,  MO(1),              KC_LEFT,  KC_DOWN,  KC_RGHT
        ),

    [1] = LAYOUT( /* FN -> RGB */
        _______,  KC_MYCM,  KC_WHOM,  KC_MAIL,  KC_CALC,  KC_MSEL,  KC_MSTP,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,   KC_SCRL,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,   
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_INS,    RGB_MOD,  KC_BRIU,  
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,             _______,  KC_BRID, 
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  MO(2),               RGB_VAI,
        _______,  GU_TOGG,  _______,                      EE_CLR,                                 _______,  _______,            RGB_SPD,  RGB_VAD,  RGB_SPI 
        ),

    [2] = LAYOUT( /* FN -> Bootloader*/
    QK_BOOTLOADER,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  
    _______,  _______,  _______,  _______,  _______,  GAME_MODE,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______, 
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
    _______,  _______,  _______,                      _______,                                 _______,  _______,            _______,  _______,  _______ 
    ),

};
// clang-format on
#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},  // Volume control on layer 0
    [1] = {ENCODER_CCW_CW(KC_MRWD, KC_MFFD)},  // Media control on layer 1
    [2] = {ENCODER_CCW_CW(_______, _______)},  // No encoder on layer 2
};
#endif