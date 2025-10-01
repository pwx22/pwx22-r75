// Copyright 2024 SDK (@sdk66)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "action_util.h"
#include "bootloader.h"
#include "deferred_exec.h"
#include "eeconfig.h"
#include "utils/indicators.h"
#include "utils/sentence_case.h"
#include "utils/socd_cleaner.h"

void clear_keyboard_but_mods(void);
#if defined(NKRO_ENABLE)
// Provide weak fallbacks so builds without the upstream NKRO helpers still link.
__attribute__((weak)) void keyboard_nkro_enable(void) {
}

__attribute__((weak)) void keyboard_nkro_disable(void) {
}
#endif

enum custom_keycodes {
    SENT_CASE_TG = SAFE_RANGE,
    WINLOCK_TG,
    SOCD_MODE_TOG,
    NKRO_MODE_TOG,
    DFU_MODE_KEY,
    CLEAR_EEPROM_KEY,
};

static bool winlock_enabled = false;
static bool nkro_enabled = false;
static socd_mode_t socd_mode = SOCD_MODE_LAST;

static deferred_token dfu_token = INVALID_DEFERRED_TOKEN;
static deferred_token eeprom_token = INVALID_DEFERRED_TOKEN;

socd_cleaner_t socd_v = {{KC_W, KC_S}, SOCD_CLEANER_LAST, {false, false}, SOCD_FIRST_NONE};
socd_cleaner_t socd_h = {{KC_A, KC_D}, SOCD_CLEANER_LAST, {false, false}, SOCD_FIRST_NONE};

static void configure_socd_instance(socd_cleaner_t *state, uint8_t resolution) {
    state->resolution = resolution;
    if (resolution != SOCD_CLEANER_FIRST) {
        state->first = SOCD_FIRST_NONE;
        return;
    }

    if (state->held[0] && !state->held[1]) {
        state->first = 0;
    } else if (!state->held[0] && state->held[1]) {
        state->first = 1;
    } else if (!state->held[0] && !state->held[1]) {
        state->first = SOCD_FIRST_NONE;
    }
}

static void set_winlock(bool enabled) {
    winlock_enabled = enabled;
    keymap_config.no_gui = enabled;
    if (enabled) {
        clear_keyboard_but_mods();
    }
    indicators_set_winlock(enabled);
}

static void set_nkro_state(bool enabled, bool trigger_feedback) {
    nkro_enabled = enabled;
#if defined(NKRO_ENABLE)
    if (enabled) {
        keyboard_nkro_enable();
    } else {
        keyboard_nkro_disable();
    }
#endif
    keymap_config.nkro = enabled;
    eeconfig_update_keymap(&keymap_config);
    indicators_set_nkro(enabled, trigger_feedback);
}

static void apply_socd_mode(socd_mode_t mode, bool trigger_feedback) {
    socd_mode = mode;
    uint8_t resolution = SOCD_CLEANER_LAST;
    switch (mode) {
        case SOCD_MODE_LAST:
            resolution = SOCD_CLEANER_LAST;
            break;
        case SOCD_MODE_NEUTRAL:
            resolution = SOCD_CLEANER_NEUTRAL;
            break;
        case SOCD_MODE_FIRST:
            resolution = SOCD_CLEANER_FIRST;
            break;
    }
    configure_socd_instance(&socd_v, resolution);
    configure_socd_instance(&socd_h, resolution);
    indicators_set_socd_mode(mode, trigger_feedback);
}

static uint32_t dfu_deferred_callback(uint32_t trigger_time, void *context) {
    (void)trigger_time;
    (void)context;
    bootloader_jump();
    return 0;
}

static uint32_t eeprom_deferred_callback(uint32_t trigger_time, void *context) {
    (void)trigger_time;
    (void)context;
    eeconfig_init();
    eeconfig_read_keymap(&keymap_config);
    sentence_case_off();
    indicators_set_sentence_case(false);
    set_winlock(false);
    set_nkro_state(keymap_config.nkro, false);
    apply_socd_mode(SOCD_MODE_LAST, false);
    layer_move(0);
    socd_cleaner_enabled = true;
    eeprom_token = INVALID_DEFERRED_TOKEN;
    return 0;
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,
KC_F12,   TO(1),   KC_MUTE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,
KC_EQL,   KC_BSPC,  KC_DEL,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,
KC_RBRC,  KC_BSLS,  KC_PGUP,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
          KC_ENT,   KC_PGDN,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,
          KC_UP,
        KC_LCTL,  KC_LCMD,  KC_LALT,                      KC_SPC,                                 KC_RALT,  MO(3),    KC_LEFT,
        KC_DOWN,  KC_RGHT
    ),
    [1] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,
KC_F12,   TO(2),   KC_MUTE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,
KC_EQL,   KC_BSPC,  KC_DEL,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,
KC_RBRC,  KC_BSLS,  KC_PGUP,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
          KC_ENT,   KC_PGDN,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,
          KC_UP,
        KC_LCTL,  KC_LCMD,  KC_LALT,                      KC_SPC,                                 KC_RALT,  MO(3),    KC_LEFT,
        KC_DOWN,  KC_RGHT
    ),
    [2] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,
KC_F12,   TO(0),   KC_MUTE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,
KC_EQL,   KC_BSPC,  KC_DEL,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,
KC_RBRC,  KC_BSLS,  KC_PGUP,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
          KC_ENT,   KC_PGDN,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,
          KC_UP,
        KC_LCTL,  KC_LCMD,  KC_LALT,                      KC_SPC,                                 KC_RALT,  MO(3),    KC_LEFT,
        KC_DOWN,  KC_RGHT
    ),
    [3] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        SENT_CASE_TG, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    MO(5),   _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  MO(4),
          _______,
        _______,  WINLOCK_TG, _______,                      _______,                               _______,  _______,  _______,
        _______,  _______
    ),
    [4] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        _______,  _______,  SOCD_MODE_TOG, _______, _______, _______, _______, _______, _______, _______, _______, _______,
       _______, _______,
        _______,  _______,  _______,  _______,  _______,  _______,  NKRO_MODE_TOG, _______, _______, _______, _______, _______,
         _______,
        _______,  _______,  _______,                      _______,                               _______,  _______,  _______,
        _______,  _______
    ),
    [5] = LAYOUT(
        DFU_MODE_KEY, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
 _______, _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
_______,  _______,  _______,
        _______,  _______,  _______,  CLEAR_EEPROM_KEY, _______, _______, _______, _______, _______, _______, _______, _______,
_______, _______, _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
          _______, _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
          _______,
        _______,  _______,  _______,                      _______,                               _______,  _______,  _______,
        _______,  _______
    ),
};
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [1] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [2] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [3] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [4] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [5] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
};
#endif
// clang-format on

void keyboard_post_init_user(void) {
    sentence_case_off();
    indicators_set_sentence_case(false);
    set_winlock(false);
    set_nkro_state(keymap_config.nkro, false);
    apply_socd_mode(SOCD_MODE_LAST, false);
    socd_cleaner_enabled = true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_socd_cleaner(keycode, record, &socd_v)) {
        return false;
    }
    if (!process_socd_cleaner(keycode, record, &socd_h)) {
        return false;
    }
    if (!process_sentence_case(keycode, record)) {
        return false;
    }

    if (winlock_enabled && record->event.pressed && (keycode == KC_LGUI || keycode == KC_RGUI)) {
        return false;
    }

    switch (keycode) {
        case SENT_CASE_TG:
            if (record->event.pressed) {
                sentence_case_toggle();
                indicators_set_sentence_case(is_sentence_case_on());
            }
            return false;
        case WINLOCK_TG:
            if (record->event.pressed) {
                set_winlock(!winlock_enabled);
            }
            return false;
        case SOCD_MODE_TOG:
            if (record->event.pressed) {
                socd_mode_t next_mode = (socd_mode == SOCD_MODE_FIRST) ? SOCD_MODE_LAST : (socd_mode_t)(socd_mode + 1);
                apply_socd_mode(next_mode, true);
            }
            return false;
        case NKRO_MODE_TOG:
            if (record->event.pressed) {
                set_nkro_state(!nkro_enabled, true);
            }
            return false;
        case DFU_MODE_KEY:
            if (record->event.pressed) {
                indicators_trigger_dfu_feedback();
                if (dfu_token != INVALID_DEFERRED_TOKEN) {
                    cancel_deferred_exec(dfu_token);
                }
                dfu_token = defer_exec(500, dfu_deferred_callback, NULL);
            }
            return false;
        case CLEAR_EEPROM_KEY:
            if (record->event.pressed) {
                indicators_trigger_eeprom_feedback();
                if (eeprom_token != INVALID_DEFERRED_TOKEN) {
                    cancel_deferred_exec(eeprom_token);
                }
                eeprom_token = defer_exec(500, eeprom_deferred_callback, NULL);
            }
            return false;
    }

    return true;
}
