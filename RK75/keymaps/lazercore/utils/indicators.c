#include "indicators.h"

#include "rgb_matrix.h"
#define LED_INDEX_CAPS 50
#define LED_INDEX_WIN 77
#define LED_INDEX_ENTER 62
#define LED_INDEX_RSFT 64
#define LED_INDEX_S 52
#define LED_INDEX_N 69
#define LED_INDEX_ESC 21
#define LED_INDEX_E 46

static const uint8_t f_keys_1_4[] = {20, 19, 18, 17};
static const uint8_t f_keys_5_8[] = {16, 15, 14, 13};
static const uint8_t f_keys_9_12[] = {12, 11, 10, 9};
static const uint8_t eeprom_feedback_leds[] = {25, 26, 45, 54, 72, 73, 52, 47};

static bool sentence_case_active = false;
static bool winlock_active = false;

static bool socd_feedback_active = false;
static socd_mode_t socd_feedback_mode = SOCD_MODE_LAST;
static uint16_t socd_feedback_timer = 0;

static bool nkro_feedback_active = false;
static bool nkro_feedback_state = false;
static uint16_t nkro_feedback_timer = 0;

static bool dfu_feedback_active = false;
static uint16_t dfu_feedback_timer = 0;

static bool eeprom_feedback_active = false;
static uint16_t eeprom_feedback_timer = 0;

static inline bool led_in_bounds(uint8_t index, uint8_t min, uint8_t max) {
    return index >= min && index < max;
}

static void set_color_if_visible(uint8_t index, uint8_t min, uint8_t max, uint8_t r, uint8_t g, uint8_t b) {
    if (led_in_bounds(index, min, max)) {
        rgb_matrix_set_color(index, r, g, b);
    }
}

static void apply_key_list_color(const uint8_t *indices, uint8_t count, uint8_t min, uint8_t max, uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t i = 0; i < count; i++) {
        set_color_if_visible(indices[i], min, max, r, g, b);
    }
}

void indicators_set_sentence_case(bool enabled) {
    sentence_case_active = enabled;
}

void indicators_set_winlock(bool enabled) {
    winlock_active = enabled;
}

void indicators_set_socd_mode(socd_mode_t mode, bool trigger_feedback) {
    if (trigger_feedback) {
        socd_feedback_active = true;
        socd_feedback_mode = mode;
        socd_feedback_timer = timer_read();
    }
}

void indicators_set_nkro(bool enabled, bool trigger_feedback) {
    if (trigger_feedback) {
        nkro_feedback_active = true;
        nkro_feedback_state = enabled;
        nkro_feedback_timer = timer_read();
    }
}

void indicators_trigger_dfu_feedback(void) {
    dfu_feedback_active = true;
    dfu_feedback_timer = timer_read();
}

void indicators_trigger_eeprom_feedback(void) {
    eeprom_feedback_active = true;
    eeprom_feedback_timer = timer_read();
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (dfu_feedback_active) {
        if (timer_elapsed(dfu_feedback_timer) <= 500) {
            for (uint8_t i = led_min; i < led_max; i++) {
                rgb_matrix_set_color(i, 0xFF, 0x00, 0x00);
            }
            return false;
        }
        dfu_feedback_active = false;
    }

    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    bool layer_is_fn = (layer == 1 || layer == 4);

    if (layer_is_fn) {
        for (uint8_t i = led_min; i < led_max; i++) {
            rgb_matrix_set_color(i, 0, 0, 0);
        }
        set_color_if_visible(LED_INDEX_ENTER, led_min, led_max, 0x4D, 0xA6, 0xFF);
        set_color_if_visible(LED_INDEX_RSFT, led_min, led_max, 0x4D, 0xA6, 0xFF);
        const uint8_t caps_color_g = sentence_case_active ? 0xFF : 0x00;
        const uint8_t caps_color_r = sentence_case_active ? 0x00 : 0xFF;
        const uint8_t caps_color_b = sentence_case_active ? 0x66 : 0x00;
        set_color_if_visible(LED_INDEX_CAPS, led_min, led_max, caps_color_r, caps_color_g, caps_color_b);
        const bool win_active = winlock_active;
        const uint8_t win_r = win_active ? 0x00 : 0xFF;
        const uint8_t win_g = win_active ? 0xFF : 0x00;
        const uint8_t win_b = win_active ? 0x66 : 0x00;
        set_color_if_visible(LED_INDEX_WIN, led_min, led_max, win_r, win_g, win_b);
    } else {
        if (layer == 0) {
            if (sentence_case_active) {
                set_color_if_visible(LED_INDEX_CAPS, led_min, led_max, 0x00, 0xFF, 0x66);
            }
            if (winlock_active) {
                set_color_if_visible(LED_INDEX_WIN, led_min, led_max, 0xFF, 0x00, 0x00);
            }
        }

        if (layer == 2) {
            set_color_if_visible(LED_INDEX_S, led_min, led_max, 0x9B, 0x59, 0xFF);
            set_color_if_visible(LED_INDEX_N, led_min, led_max, 0xFF, 0xA5, 0x00);
        }

        if (layer == 3) {
            set_color_if_visible(LED_INDEX_ESC, led_min, led_max, 0xFF, 0x00, 0x00);
            set_color_if_visible(LED_INDEX_E, led_min, led_max, 0xFF, 0x00, 0x00);
        }
    }

    if (eeprom_feedback_active) {
        if (timer_elapsed(eeprom_feedback_timer) <= 500) {
            apply_key_list_color(eeprom_feedback_leds, ARRAY_SIZE(eeprom_feedback_leds), led_min, led_max, 0xFF, 0x00, 0x00);
        } else {
            eeprom_feedback_active = false;
        }
    }

    if (nkro_feedback_active) {
        if (timer_elapsed(nkro_feedback_timer) <= 1000) {
            if (nkro_feedback_state) {
                apply_key_list_color(f_keys_5_8, ARRAY_SIZE(f_keys_5_8), led_min, led_max, 0xFF, 0xA5, 0x00);
            } else {
                apply_key_list_color(f_keys_1_4, ARRAY_SIZE(f_keys_1_4), led_min, led_max, 0xFF, 0xA5, 0x00);
                apply_key_list_color(f_keys_5_8, ARRAY_SIZE(f_keys_5_8), led_min, led_max, 0xFF, 0xA5, 0x00);
                apply_key_list_color(f_keys_9_12, ARRAY_SIZE(f_keys_9_12), led_min, led_max, 0xFF, 0xA5, 0x00);
            }
        } else {
            nkro_feedback_active = false;
        }
    }

    if (socd_feedback_active) {
        uint16_t elapsed = timer_elapsed(socd_feedback_timer);
        bool keep_active = true;
        switch (socd_feedback_mode) {
            case SOCD_MODE_LAST:
                if (elapsed <= 5000) {
                    apply_key_list_color(f_keys_1_4, ARRAY_SIZE(f_keys_1_4), led_min, led_max, 0x9B, 0x59, 0xFF);
                } else if (elapsed <= 10000) {
                    apply_key_list_color(f_keys_9_12, ARRAY_SIZE(f_keys_9_12), led_min, led_max, 0x9B, 0x59, 0xFF);
                } else {
                    keep_active = false;
                }
                break;
            case SOCD_MODE_NEUTRAL:
                if (elapsed <= 1000) {
                    apply_key_list_color(f_keys_5_8, ARRAY_SIZE(f_keys_5_8), led_min, led_max, 0x9B, 0x59, 0xFF);
                } else {
                    keep_active = false;
                }
                break;
            case SOCD_MODE_FIRST:
                if (elapsed <= 1000) {
                    apply_key_list_color(f_keys_1_4, ARRAY_SIZE(f_keys_1_4), led_min, led_max, 0x9B, 0x59, 0xFF);
                    if (elapsed >= 500) {
                        apply_key_list_color(f_keys_9_12, ARRAY_SIZE(f_keys_9_12), led_min, led_max, 0x9B, 0x59, 0xFF);
                    }
                } else {
                    keep_active = false;
                }
                break;
        }
        if (!keep_active) {
            socd_feedback_active = false;
        }
    }

    return false;
}
