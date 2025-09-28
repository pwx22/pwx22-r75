// Copyright 2024 SDK (@sdk66)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "utils/indicators.h"
#include "utils/sentence_case.h"
#include "utils/socd_cleaner.h"
#include "eeconfig.h"
#include "timer.h"

enum layer_names {
    _BASE,
    _FN,
    _SHIFT,
    _ENTER,
    _EMPTY,
};

enum custom_keycodes {
    WINLOCK_TG = SAFE_RANGE,
    SENTENCE_CASE_TG,
    SOCD_TOG,
    NKRO_TOG,
    DFU_MODE,
    EEPROM_CLEAR,
};

static bool fn_held = false;
static bool shift_layer_active = false;
static bool enter_layer_active = false;
static bool dfu_pending = false;
static uint16_t dfu_request_timer = 0;
static uint8_t socd_mode = 0;  // 0: Neutralize, 1: Last wins, 2: First wins

socd_cleaner_t socd_v = {{KC_W, KC_S}, SOCD_CLEANER_NEUTRAL};
socd_cleaner_t socd_h = {{KC_A, KC_D}, SOCD_CLEANER_NEUTRAL};

static void update_socd_resolution(uint8_t mode) {
    socd_mode = mode;
    switch (socd_mode) {
        case 0:
            socd_v.resolution = SOCD_CLEANER_NEUTRAL;
            socd_h.resolution = SOCD_CLEANER_NEUTRAL;
            break;
        case 1:
            socd_v.resolution = SOCD_CLEANER_LAST;
            socd_h.resolution = SOCD_CLEANER_LAST;
            break;
        default:
            socd_v.resolution = SOCD_CLEANER_0_WINS;
            socd_h.resolution = SOCD_CLEANER_0_WINS;
            break;
    }
    indicators_set_socd_mode(socd_mode);
}

static void toggle_winlock(void) {
    keymap_config.no_gui = !keymap_config.no_gui;
    eeconfig_update_keymap(&keymap_config);
    clear_keyboard();
}

static void toggle_nkro(void) {
    keymap_config.nkro = !keymap_config.nkro;
    eeconfig_update_keymap(&keymap_config);
    clear_keyboard();
    indicators_trigger_nkro_feedback(keymap_config.nkro);
}

static void cycle_socd_mode(void) {
    uint8_t next_mode = (socd_mode + 1) % 3;
    update_socd_resolution(next_mode);
    indicators_trigger_socd_feedback(next_mode);
}

static void arm_dfu_reset(void) {
    if (!dfu_pending) {
        dfu_pending = true;
        dfu_request_timer = timer_read();
        indicators_trigger_dfu_feedback();
    }
}

static void clear_eeprom_with_feedback(void) {
    indicators_trigger_eeprom_feedback();
    eeconfig_init();
    keymap_config.raw = 0;
    update_socd_resolution(socd_mode);
    clear_keyboard();
}

void keyboard_post_init_user(void) {
    update_socd_resolution(socd_mode);
    sentence_case_off();
    layer_state_set_user(layer_state);
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,
KC_F12,   KC_HOME,   KC_MUTE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,
KC_EQL,   KC_BSPC,  KC_DEL,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,
KC_RBRC,  KC_BSLS,  KC_PGUP,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
          KC_ENT,   KC_PGDN,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,
          KC_UP,
        KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,                                 KC_RALT,  MO(_FN),
KC_LEFT,  KC_DOWN,  KC_RGHT
        ),

    [_FN] = LAYOUT( 
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  
        SENTENCE_CASE_TG,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            MO(_ENTER),  _______, 
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  MO(_SHIFT),            _______,
        _______,  WINLOCK_TG,  _______,                      _______,                                 _______,  _______,            _______,  _______,  _______ 
        ),

    [_SHIFT] = LAYOUT( 
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  
        _______,  _______,  SOCD_TOG,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______, 
        _______,  _______,  _______,  _______,  _______,  _______,  NKRO_TOG,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,                      _______,                                 _______,  _______,            _______,  _______,  _______ 
        ),

    [_ENTER] = LAYOUT( 
        DFU_MODE,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   
        _______,  _______,  _______,  EEPROM_CLEAR,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______, 
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,                      _______,                                 _______,  _______,            _______,  _______,  _______ 
        ),

    [_EMPTY] = LAYOUT( 
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______, 
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,                      _______,                                 _______,  _______,            _______,  _______,  _______ 
        ),
};
// clang-format on

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE]  = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_FN]    = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_SHIFT] = {ENCODER_CCW_CW(KC_MRWD, KC_MFFD)},
    [_ENTER] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_EMPTY] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
};
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
    fn_held             = layer_state_cmp(state, 1UL << _FN);
    shift_layer_active  = layer_state_cmp(state, 1UL << _SHIFT);
    enter_layer_active  = layer_state_cmp(state, 1UL << _ENTER);

    indicators_set_fn(fn_held);
    indicators_set_layer2(shift_layer_active);
    indicators_set_layer3(enter_layer_active);

    return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case WINLOCK_TG:
            if (record->event.pressed) {
                toggle_winlock();
            }
            return false;
        case SENTENCE_CASE_TG:
            if (record->event.pressed) {
                sentence_case_toggle();
            }
            return false;
        case SOCD_TOG:
            if (record->event.pressed) {
                cycle_socd_mode();
            }
            return false;
        case NKRO_TOG:
            if (record->event.pressed) {
                toggle_nkro();
            }
            return false;
        case DFU_MODE:
            if (record->event.pressed) {
                arm_dfu_reset();
            }
            return false;
        case EEPROM_CLEAR:
            if (record->event.pressed) {
                clear_eeprom_with_feedback();
            }
            return false;
    }

    if (!process_sentence_case(keycode, record)) {
        return false;
    }
    if (!process_socd_cleaner(keycode, record, &socd_v)) {
        return false;
    }
    if (!process_socd_cleaner(keycode, record, &socd_h)) {
        return false;
    }

    return true;
}

void matrix_scan_user(void) {
    sentence_case_task();
    if (dfu_pending && timer_elapsed(dfu_request_timer) > 600) {
        dfu_pending = false;
        reset_keyboard();
    }
}
