#include "indicators.h"
#include "sentence_case.h"
#include "timer.h"
#include "eeconfig.h"
#include "rgb_matrix.h"

static const rgb_t base_color = {.r = 0xFF, .g = 0x5F, .b = 0x40};
static const rgb_t color_red = {.r = 0xFF, .g = 0x00, .b = 0x00};
static const rgb_t color_green = {.r = 0x00, .g = 0xFF, .b = 0x66};
static const rgb_t color_blue = {.r = 0x4D, .g = 0xA6, .b = 0xFF};
static const rgb_t color_orange = {.r = 0xFF, .g = 0xA5, .b = 0x00};
static const rgb_t color_purple = {.r = 0x9B, .g = 0x59, .b = 0xFF};

enum {
    LED_ESC = 21,
    LED_WIN = 77,
    LED_CAPS = 50,
    LED_RSFT = 64,
    LED_ENTER = 62,
    LED_KEY_N = 69,
    LED_KEY_S = 52,
    LED_KEY_E = 46,
};

static const uint8_t f1_to_f12[] = {20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9};
static const uint8_t f1_to_f4[] = {20, 19, 18, 17};
static const uint8_t f5_to_f8[] = {16, 15, 14, 13};
static const uint8_t f9_to_f12[] = {12, 11, 10, 9};
static const uint8_t eeprom_feedback_leds[] = {47, 25, 26, 45, 54, 72, 73, 52};

static bool fn_held = false;
static bool layer2_active = false;
static bool layer3_active = false;
static uint8_t socd_mode_state = 0;

static uint16_t nkro_flash_timer = 0;
static uint8_t nkro_flash_type = 0;
static uint16_t socd_flash_timer = 0;
static uint8_t socd_flash_mode = 0;
static uint16_t dfu_flash_timer = 0;
static uint16_t eeprom_flash_timer = 0;

static inline uint8_t scale_channel(uint8_t value, float brightness) {
    float scaled = value * brightness;
    if (scaled > 255.0f) {
        scaled = 255.0f;
    }
    if (scaled < 0.0f) {
        scaled = 0.0f;
    }
    return (uint8_t)(scaled + 0.5f);
}

static inline void set_indicator_color(uint8_t index, const rgb_t *color) {
    RGB_MATRIX_INDICATOR_SET_COLOR(index, color->r, color->g, color->b);
}

static inline void set_indicator_color_scaled(uint8_t index, const rgb_t *color, float brightness) {
    if (brightness <= 0.0f) {
        return;
    }
    RGB_MATRIX_INDICATOR_SET_COLOR(index,
                                   scale_channel(color->r, brightness),
                                   scale_channel(color->g, brightness),
                                   scale_channel(color->b, brightness));
}

void indicators_set_fn(bool pressed) {
    fn_held = pressed;
}

void indicators_set_layer2(bool pressed) {
    layer2_active = pressed;
}

void indicators_set_layer3(bool pressed) {
    layer3_active = pressed;
}

void indicators_set_socd_mode(uint8_t mode) {
    socd_mode_state = mode;
}

void indicators_trigger_nkro_feedback(bool nkro_enabled) {
    nkro_flash_type = nkro_enabled ? 1 : 2;
    nkro_flash_timer = timer_read();
}

void indicators_trigger_socd_feedback(uint8_t mode) {
    socd_flash_mode = mode + 1;  // store as 1-based so 0 means "no event"
    socd_flash_timer = timer_read();
}

void indicators_trigger_dfu_feedback(void) {
    dfu_flash_timer = timer_read();
}

void indicators_trigger_eeprom_feedback(void) {
    eeprom_flash_timer = timer_read();
}

static void highlight_f_keys(const uint8_t *leds, uint8_t count, const rgb_t *color) {
    for (uint8_t i = 0; i < count; ++i) {
        set_indicator_color(leds[i], color);
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    (void)led_min;
    (void)led_max;

    bool dfu_flash_on = false;
    if (dfu_flash_timer) {
        uint16_t elapsed = timer_elapsed(dfu_flash_timer);
        if (elapsed >= 600) {
            dfu_flash_timer = 0;
        } else {
            dfu_flash_on = ((elapsed / 150) % 2U) == 0;
        }
    }
    if (dfu_flash_on) {
        for (uint8_t i = led_min; i < led_max; ++i) {
            set_indicator_color(i, &color_red);
        }
        return false;
    }

    float accent_brightness = 1.0f;
    if (layer3_active) {
        accent_brightness = 0.0f;
    } else if (layer2_active) {
        accent_brightness = 0.4f;
    } else if (fn_held) {
        accent_brightness = 0.6f;
    }

    if (!fn_held) {
        if (keymap_config.no_gui) {
            set_indicator_color_scaled(LED_WIN, &color_red, accent_brightness);
        }
        if (is_sentence_case_on()) {
            set_indicator_color_scaled(LED_CAPS, &color_green, accent_brightness);
        }
    }

    if (layer2_active) {
        set_indicator_color(LED_KEY_S, &color_purple);
        if (keymap_config.nkro) {
            set_indicator_color(LED_KEY_N, &color_orange);
        } else {
            set_indicator_color(LED_KEY_N, &base_color);
        }
    }

    if (layer3_active) {
        set_indicator_color(LED_ENTER, &base_color);
        if (fn_held) {
            set_indicator_color(LED_ESC, &color_red);
            set_indicator_color(LED_KEY_E, &color_red);
        }
    }

    if (fn_held) {
        set_indicator_color(LED_RSFT, &color_blue);
        set_indicator_color(LED_ENTER, &color_blue);
        if (!(layer2_active || layer3_active)) {
            if (keymap_config.no_gui) {
                set_indicator_color(LED_WIN, &color_green);
            } else {
                set_indicator_color(LED_WIN, &color_red);
            }
            if (is_sentence_case_on()) {
                set_indicator_color(LED_CAPS, &color_green);
            } else {
                set_indicator_color(LED_CAPS, &color_red);
            }
        }
        if (layer3_active) {
            set_indicator_color(LED_ESC, &color_red);
            set_indicator_color(LED_KEY_E, &color_red);
        }
    }

    bool eeprom_flash_on = false;
    if (eeprom_flash_timer) {
        uint16_t elapsed = timer_elapsed(eeprom_flash_timer);
        if (elapsed >= 600) {
            eeprom_flash_timer = 0;
        } else {
            eeprom_flash_on = ((elapsed / 150) % 2U) == 0;
        }
    }
    if (eeprom_flash_on) {
        for (uint8_t i = 0; i < sizeof(eeprom_feedback_leds); ++i) {
            set_indicator_color(eeprom_feedback_leds[i], &color_red);
        }
    }

    if (nkro_flash_type) {
        uint16_t elapsed = timer_elapsed(nkro_flash_timer);
        if (elapsed >= 320) {
            nkro_flash_type = 0;
        } else if (nkro_flash_type == 1) {
            highlight_f_keys(f1_to_f12, sizeof(f1_to_f12), &color_orange);
        } else {
            highlight_f_keys(f5_to_f8, sizeof(f5_to_f8), &color_orange);
        }
    }

    if (socd_flash_mode) {
        uint16_t elapsed = timer_elapsed(socd_flash_timer);
        if (elapsed >= 480) {
            socd_flash_mode = 0;
        } else {
            switch (socd_flash_mode) {
                case 1:  // Neutralize
                    highlight_f_keys(f5_to_f8, sizeof(f5_to_f8), &color_purple);
                    break;
                case 2:  // Last wins
                    if (elapsed < 240) {
                        highlight_f_keys(f1_to_f4, sizeof(f1_to_f4), &color_purple);
                    } else {
                        highlight_f_keys(f9_to_f12, sizeof(f9_to_f12), &color_purple);
                    }
                    break;
                case 3: {  // First wins
                    highlight_f_keys(f5_to_f8, sizeof(f5_to_f8), &color_purple);
                    if (((elapsed / 120) % 2U) == 0) {
                        highlight_f_keys(f9_to_f12, sizeof(f9_to_f12), &color_purple);
                    }
                    break;
                }
            }
        }
    }

    (void)socd_mode_state;
    return false;
}
