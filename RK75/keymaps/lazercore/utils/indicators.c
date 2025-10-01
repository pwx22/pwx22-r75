#include "indicators.h"

#include "rgb_matrix.h"
#ifndef RGB_MATRIX_DEFAULT_VAL
#    define RGB_MATRIX_DEFAULT_VAL 255
#endif
#define LED_INDEX_CAPS 50
#define LED_INDEX_WIN 77
#define LED_INDEX_ENTER 62
#define LED_INDEX_LAYER_TOGGLE 8
#define LED_INDEX_RSFT 64
#define LED_INDEX_FN 1
#define LED_INDEX_S 52
#define LED_INDEX_N 69
#define LED_INDEX_ESC 21
#define LED_INDEX_E 46
#define LED_INDEX_CUSTOM70 5

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_color_t;

static const rgb_color_t COLOR_OFF = {0x00, 0x00, 0x00};
static const rgb_color_t COLOR_SENTENCE_ON = {0x7E, 0xFF, 0x45};
static const rgb_color_t COLOR_WINLOCK_ON = {0xFF, 0x0E, 0x0E};
static const rgb_color_t COLOR_LAYER1_FN = {0xFF, 0xC4, 0x9D};
static const rgb_color_t COLOR_LAYER1_RSFT = {0x66, 0x4E, 0x3F};
static const rgb_color_t COLOR_LAYER1_ENTER = {0x66, 0x4E, 0x3F};
static const rgb_color_t COLOR_LAYER1_TO2 = {0x0F, 0x5C, 0x16};
static const rgb_color_t COLOR_LAYER1_TOGGLE_OFF = {0xFF, 0x0E, 0x0E};
static const rgb_color_t COLOR_LAYER1_TOGGLE_ON = {0x7E, 0xFF, 0x45};
static const rgb_color_t COLOR_LAYER2_FN = {0x66, 0x4E, 0x3F};
static const rgb_color_t COLOR_LAYER2_RSFT = {0xFF, 0xC4, 0x9D};
static const rgb_color_t COLOR_LAYER2_SOCD = {0xFF, 0x40, 0x0D};
static const rgb_color_t COLOR_LAYER2_NKRO = {0xFF, 0x12, 0x98};
static const rgb_color_t COLOR_LAYER2_TO0 = {0x21, 0x21, 0x80};
static const rgb_color_t COLOR_LAYER3_KEY = {0xFF, 0x00, 0x00};
static const rgb_color_t COLOR_SOCD_INDICATOR_DIM = {0x1A, 0x0D, 0x06};
static const rgb_color_t COLOR_SOCD_INDICATOR_BRIGHT = {0xB3, 0x3C, 0x1B};
static const rgb_color_t COLOR_NKRO_INDICATOR = {0xB2, 0x28, 0x9A};

static const uint8_t f_keys_1_4[] = {20, 19, 18, 17};
static const uint8_t f_keys_5_8[] = {16, 15, 14, 13};
static const uint8_t f_keys_9_12[] = {12, 11, 10, 9};
static const uint8_t eeprom_feedback_leds[] = {25, 26, 45, 54, 72, 73, 52, 47};

#define LED_INDEX_NUM1 23
#define LED_INDEX_NUM2 24
#define LED_INDEX_NUM6 28
#define LED_INDEX_NUM0 32

static bool sentence_case_active = false;
static bool winlock_active = false;

static bool socd_feedback_active = false;
static socd_mode_t socd_feedback_mode = SOCD_MODE_LAST;
static uint16_t socd_feedback_timer = 0;
static socd_mode_t socd_current_mode = SOCD_MODE_LAST;

static bool nkro_feedback_active = false;
static bool nkro_feedback_state = false;
static uint16_t nkro_feedback_timer = 0;
static bool nkro_active = false;

static bool dfu_feedback_active = false;
static uint16_t dfu_feedback_timer = 0;

static bool eeprom_feedback_active = false;
static uint16_t eeprom_feedback_timer = 0;

static bool night_mode_enabled = false;
static HSV night_mode_hsv = {.h = 16, .s = 165, .v = 26};

static rgb_color_t hsv_to_rgb_custom(uint8_t h, uint8_t s, uint8_t v) {
    rgb_color_t rgb = {v, v, v};
    if (s == 0 || v == 0) {
        rgb.r = v;
        rgb.g = v;
        rgb.b = v;
        return rgb;
    }

    uint8_t region = h / 43;
    uint16_t remainder = (h - (region * 43)) * 6;

    uint8_t p = (uint16_t)v * (255 - s) / 255;
    uint8_t q = (uint16_t)v * (255 - ((uint16_t)s * remainder / 255)) / 255;
    uint8_t t = (uint16_t)v * (255 - ((uint16_t)s * (255 - remainder) / 255)) / 255;

    switch (region) {
        case 0:
            rgb.r = v;
            rgb.g = t;
            rgb.b = p;
            break;
        case 1:
            rgb.r = q;
            rgb.g = v;
            rgb.b = p;
            break;
        case 2:
            rgb.r = p;
            rgb.g = v;
            rgb.b = t;
            break;
        case 3:
            rgb.r = p;
            rgb.g = q;
            rgb.b = v;
            break;
        case 4:
            rgb.r = t;
            rgb.g = p;
            rgb.b = v;
            break;
        default:
            rgb.r = v;
            rgb.g = p;
            rgb.b = q;
            break;
    }

    return rgb;
}

static inline bool led_in_bounds(uint8_t index, uint8_t min, uint8_t max) {
    return index >= min && index < max;
}

static rgb_color_t scale_for_brightness(rgb_color_t color) {
    uint16_t value = night_mode_enabled ? night_mode_hsv.v : rgb_matrix_config.hsv.v;
    uint16_t base = RGB_MATRIX_DEFAULT_VAL;
    if (base == 0) {
        base = 255;
    }

    uint16_t scaled = (uint16_t)color.r * value / base;
    color.r = scaled > 255 ? 255 : (uint8_t)scaled;
    scaled = (uint16_t)color.g * value / base;
    color.g = scaled > 255 ? 255 : (uint8_t)scaled;
    scaled = (uint16_t)color.b * value / base;
    color.b = scaled > 255 ? 255 : (uint8_t)scaled;
    return color;
}

static void set_color_internal(uint8_t index, uint8_t min, uint8_t max, const rgb_color_t *color, bool apply_brightness) {
    if (!led_in_bounds(index, min, max)) {
        return;
    }
    rgb_color_t out = apply_brightness ? scale_for_brightness(*color) : *color;
    rgb_matrix_set_color(index, out.r, out.g, out.b);
}

static void set_color_rgb(uint8_t index, uint8_t min, uint8_t max, const rgb_color_t *color) {
    set_color_internal(index, min, max, color, true);
}

static void set_color_raw(uint8_t index, uint8_t min, uint8_t max, const rgb_color_t *color) {
    set_color_internal(index, min, max, color, false);
}

static void fill_range_internal(uint8_t min, uint8_t max, const rgb_color_t *color, bool apply_brightness) {
    rgb_color_t out = apply_brightness ? scale_for_brightness(*color) : *color;
    for (uint8_t i = min; i < max; i++) {
        rgb_matrix_set_color(i, out.r, out.g, out.b);
    }
}

static void fill_range_with_color(uint8_t min, uint8_t max, const rgb_color_t *color) {
    fill_range_internal(min, max, color, true);
}

static void fill_range_with_raw_color(uint8_t min, uint8_t max, const rgb_color_t *color) {
    fill_range_internal(min, max, color, false);
}

static void apply_key_list_rgb(const uint8_t *indices, uint8_t count, uint8_t min, uint8_t max, const rgb_color_t *color) {
    rgb_color_t out = scale_for_brightness(*color);
    for (uint8_t i = 0; i < count; i++) {
        set_color_internal(indices[i], min, max, &out, false);
    }
}

void indicators_set_sentence_case(bool enabled) {
    sentence_case_active = enabled;
}

void indicators_set_winlock(bool enabled) {
    winlock_active = enabled;
}

void indicators_set_socd_mode(socd_mode_t mode, bool trigger_feedback) {
    socd_current_mode = mode;
    if (trigger_feedback) {
        socd_feedback_active = true;
        socd_feedback_mode = mode;
        socd_feedback_timer = timer_read();
    }
}

void indicators_set_nkro(bool enabled, bool trigger_feedback) {
    nkro_active = enabled;
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

void indicators_set_night_hsv(HSV hsv) {
    night_mode_hsv = hsv;
}

void indicators_set_night_enabled(bool enabled) {
    night_mode_enabled = enabled;
}

bool indicators_is_night_enabled(void) {
    return night_mode_enabled;
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
    bool layer_is_base = (layer == 0 || layer == 1 || layer == 2);
    bool layer_is_fn = (layer == 3);
    bool layer_is_socd = (layer == 4);
    bool layer_is_system = (layer == 5);

    HSV active_hsv = night_mode_enabled ? night_mode_hsv : rgb_matrix_config.hsv;
    rgb_color_t via_color = hsv_to_rgb_custom(active_hsv.h, active_hsv.s, active_hsv.v);

    if (layer_is_base) {
        fill_range_with_raw_color(led_min, led_max, &via_color);
    } else {
        fill_range_with_color(led_min, led_max, &COLOR_OFF);
    }

    if (layer_is_fn) {
        set_color_raw(LED_INDEX_FN, led_min, led_max, &COLOR_LAYER1_FN);
        set_color_raw(LED_INDEX_RSFT, led_min, led_max, &COLOR_LAYER1_RSFT);
        set_color_raw(LED_INDEX_ENTER, led_min, led_max, &COLOR_LAYER1_ENTER);
        const rgb_color_t *caps_color = sentence_case_active ? &COLOR_LAYER1_TOGGLE_ON : &COLOR_LAYER1_TOGGLE_OFF;
        set_color_raw(LED_INDEX_CAPS, led_min, led_max, caps_color);
        const rgb_color_t *win_color = winlock_active ? &COLOR_LAYER1_TOGGLE_ON : &COLOR_LAYER1_TOGGLE_OFF;
        set_color_raw(LED_INDEX_WIN, led_min, led_max, win_color);
        set_color_internal(LED_INDEX_CUSTOM70, led_min, led_max, &via_color, false);
    } else if (layer_is_socd) {
        set_color_raw(LED_INDEX_FN, led_min, led_max, &COLOR_LAYER2_FN);
        set_color_raw(LED_INDEX_RSFT, led_min, led_max, &COLOR_LAYER2_RSFT);
        set_color_raw(LED_INDEX_S, led_min, led_max, &COLOR_LAYER2_SOCD);
        set_color_raw(LED_INDEX_N, led_min, led_max, &COLOR_LAYER2_NKRO);
        const rgb_color_t *digit1_color = &COLOR_SOCD_INDICATOR_DIM;
        const rgb_color_t *digit2_color = &COLOR_SOCD_INDICATOR_DIM;
        switch (socd_current_mode) {
            case SOCD_MODE_FIRST:
                digit1_color = &COLOR_SOCD_INDICATOR_BRIGHT;
                break;
            case SOCD_MODE_LAST:
                digit2_color = &COLOR_SOCD_INDICATOR_BRIGHT;
                break;
            case SOCD_MODE_NEUTRAL:
            default:
                break;
        }
        set_color_raw(LED_INDEX_NUM1, led_min, led_max, digit1_color);
        set_color_raw(LED_INDEX_NUM2, led_min, led_max, digit2_color);
        if (nkro_active) {
            set_color_raw(LED_INDEX_NUM0, led_min, led_max, &COLOR_NKRO_INDICATOR);
        } else {
            set_color_raw(LED_INDEX_NUM6, led_min, led_max, &COLOR_NKRO_INDICATOR);
        }
    } else if (layer_is_system) {
        set_color_raw(LED_INDEX_FN, led_min, led_max, &COLOR_LAYER3_KEY);
        set_color_raw(LED_INDEX_ENTER, led_min, led_max, &COLOR_LAYER3_KEY);
        set_color_raw(LED_INDEX_ESC, led_min, led_max, &COLOR_LAYER3_KEY);
        set_color_raw(LED_INDEX_E, led_min, led_max, &COLOR_LAYER3_KEY);
    }

    if (layer_is_base) {
        if (layer == 1) {
            set_color_rgb(LED_INDEX_LAYER_TOGGLE, led_min, led_max, &COLOR_LAYER1_TO2);
        } else if (layer == 2) {
            set_color_rgb(LED_INDEX_LAYER_TOGGLE, led_min, led_max, &COLOR_LAYER2_TO0);
        }
        if (sentence_case_active) {
            set_color_rgb(LED_INDEX_CAPS, led_min, led_max, &COLOR_SENTENCE_ON);
        }
        if (winlock_active) {
            set_color_rgb(LED_INDEX_WIN, led_min, led_max, &COLOR_WINLOCK_ON);
        }
    }

    if (eeprom_feedback_active) {
        if (timer_elapsed(eeprom_feedback_timer) <= 500) {
            apply_key_list_rgb(eeprom_feedback_leds, ARRAY_SIZE(eeprom_feedback_leds), led_min, led_max, &COLOR_LAYER3_KEY);
        } else {
            eeprom_feedback_active = false;
        }
    }

    if (nkro_feedback_active) {
        if (timer_elapsed(nkro_feedback_timer) <= 1000) {
            if (nkro_feedback_state) {
                apply_key_list_rgb(f_keys_1_4, ARRAY_SIZE(f_keys_1_4), led_min, led_max, &COLOR_LAYER2_NKRO);
                apply_key_list_rgb(f_keys_5_8, ARRAY_SIZE(f_keys_5_8), led_min, led_max, &COLOR_LAYER2_NKRO);
                apply_key_list_rgb(f_keys_9_12, ARRAY_SIZE(f_keys_9_12), led_min, led_max, &COLOR_LAYER2_NKRO);
            } else {
                apply_key_list_rgb(f_keys_5_8, ARRAY_SIZE(f_keys_5_8), led_min, led_max, &COLOR_LAYER2_NKRO);
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
                if (elapsed < 500) {
                    apply_key_list_rgb(f_keys_1_4, ARRAY_SIZE(f_keys_1_4), led_min, led_max, &COLOR_LAYER2_SOCD);
                } else if (elapsed < 1000) {
                    apply_key_list_rgb(f_keys_9_12, ARRAY_SIZE(f_keys_9_12), led_min, led_max, &COLOR_LAYER2_SOCD);
                } else {
                    keep_active = false;
                }
                break;
            case SOCD_MODE_NEUTRAL:
                if (elapsed < 1000) {
                    apply_key_list_rgb(f_keys_5_8, ARRAY_SIZE(f_keys_5_8), led_min, led_max, &COLOR_LAYER2_SOCD);
                } else {
                    keep_active = false;
                }
                break;
            case SOCD_MODE_FIRST:
                if (elapsed < 1000) {
                    apply_key_list_rgb(f_keys_1_4, ARRAY_SIZE(f_keys_1_4), led_min, led_max, &COLOR_LAYER2_SOCD);
                    if ((elapsed >= 250 && elapsed < 500) || (elapsed >= 750 && elapsed < 1000)) {
                        apply_key_list_rgb(f_keys_9_12, ARRAY_SIZE(f_keys_9_12), led_min, led_max, &COLOR_LAYER2_SOCD);
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
