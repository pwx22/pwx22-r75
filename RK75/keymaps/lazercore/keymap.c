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
    RSFT_L2 = SAFE_RANGE,
    RENTER_L3,
};

static bool fn_held = false;
static bool shift_layer_held = false;
static bool enter_layer_held = false;
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
          RENTER_L3,   KC_PGDN,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  RSFT_L2,
          KC_UP,
        KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,                                 KC_RALT,  MO(_FN),
KC_LEFT,  KC_DOWN,  KC_RGHT
        ),

    [_FN] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,   _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,   _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
          _______,   _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
          _______,
        _______,  _______,  _______,                      _______,                                 _______,  _______,
_______,  _______,  _______
        ),

    [_SHIFT] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,   _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,   _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
          _______,   _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
          _______,
        _______,  _______,  _______,                      _______,                                 _______,  _______,
_______,  _______,  _______
        ),

    [_ENTER] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,   _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,   _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
          _______,   _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
          _______,
        _______,  _______,  _______,                      _______,                                 _______,  _______,
_______,  _______,  _______
        ),

    [_EMPTY] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,   _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,   _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
          _______,   _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
          _______,
        _______,  _______,  _______,                      _______,                                 _______,  _______,
_______,  _______,  _______
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

static void set_shift_layer(bool pressed) {
    shift_layer_held = pressed;
    indicators_set_layer2(pressed);
    if (pressed) {
        layer_on(_SHIFT);
        register_code(KC_RSFT);
    } else {
        layer_off(_SHIFT);
        unregister_code(KC_RSFT);
    }
}

static void set_enter_layer(bool pressed) {
    enter_layer_held = pressed;
    indicators_set_layer3(pressed);
    if (pressed) {
        layer_on(_ENTER);
        register_code(KC_ENT);
    } else {
        layer_off(_ENTER);
        unregister_code(KC_ENT);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == MO(_FN)) {
        fn_held = record->event.pressed;
        indicators_set_fn(fn_held);
    }

    switch (keycode) {
        case RSFT_L2:
            if (record->event.pressed) {
                set_shift_layer(true);
            } else {
                set_shift_layer(false);
            }
            return false;
        case RENTER_L3:
            if (record->event.pressed) {
                set_enter_layer(true);
            } else {
                set_enter_layer(false);
            }
            return false;
    }

    if (record->event.pressed) {
        if (fn_held && keycode == KC_LGUI) {
            toggle_winlock();
            return false;
        }
        if (fn_held && keycode == KC_CAPS) {
            sentence_case_toggle();
            return false;
        }
        if (fn_held && shift_layer_held && keycode == KC_N) {
            toggle_nkro();
            return false;
        }
        if (fn_held && shift_layer_held && keycode == KC_S) {
            cycle_socd_mode();
            return false;
        }
        if (fn_held && enter_layer_held && keycode == KC_ESC) {
            arm_dfu_reset();
            return false;
        }
        if (fn_held && enter_layer_held && keycode == KC_E) {
            clear_eeprom_with_feedback();
            return false;
        }
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
