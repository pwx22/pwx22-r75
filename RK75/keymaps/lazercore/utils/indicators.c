#include "indicators.h"
#include "sentence_case.h"
#include "type_alchemy.h"
#ifdef AUTOCORRECT_ENABLE
#    include "autocorrect.h"
#endif

#define LAZERCORE_LED_CAPS 50
#define LAZERCORE_LED_ENTER 62
#define LAZERCORE_LED_RSFT 64
#define LAZERCORE_LED_LGUI 77
#define LAZERCORE_LED_TYPE_ALCHEMY 20
#define LAZERCORE_LED_UNICODE 19
#define LAZERCORE_LED_AUTOCORRECT 18

#define LAZERCORE_FROW_LED_COUNT 14

static const uint8_t lazercore_frow_leds[LAZERCORE_FROW_LED_COUNT] = {
    21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8,
};

static bool lazercore_is_frow_led(uint8_t led_index) {
    for (uint8_t i = 0; i < LAZERCORE_FROW_LED_COUNT; ++i) {
        if (lazercore_frow_leds[i] == led_index) {
            return true;
        }
    }
    return false;
}

static rgb_t lazercore_scale_hsv(hsv_t hsv, uint8_t percentage) {
    hsv_t scaled = hsv;
    scaled.v = (uint8_t)(((uint16_t)scaled.v * percentage) / 100);
    return hsv_to_rgb(scaled);
}

static rgb_t lazercore_make_rgb(uint8_t r, uint8_t g, uint8_t b) {
    rgb_t color = {r, g, b};
    return color;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    bool fn_active = layer_state_is(1);

    hsv_t base_hsv = rgb_matrix_get_hsv();
    rgb_t dim40 = lazercore_scale_hsv(base_hsv, 40);
    rgb_t dim60 = lazercore_scale_hsv(base_hsv, 60);

    rgb_t global_flash_color;
    bool global_flash_active = lazercore_rgb_get_global_flash(&global_flash_color);

    for (uint8_t led = led_min; led < led_max; ++led) {
        rgb_t color = lazercore_is_frow_led(led) ? dim60 : dim40;

        if (!global_flash_active) {
            rgb_t flash_color;
            if (lazercore_rgb_get_frow_flash(led, &flash_color)) {
                color = flash_color;
            }

            switch (led) {
                case LAZERCORE_LED_TYPE_ALCHEMY:
                    if (is_type_alchemy_on()) {
                        color = lazercore_make_rgb(0xD6, 0x5F, 0xFF);
                    } else {
                        color = dim60;
                    }
                    break;
                case LAZERCORE_LED_UNICODE: {
                    uint8_t unicode_mode = get_unicode_input_mode();
                    if (unicode_mode == 4) {  // WinCompose
                        color = lazercore_make_rgb(0x20, 0xC0, 0xFF);
                    } else if (unicode_mode == 1) {  // Linux
                        color = lazercore_make_rgb(0xFF, 0x40, 0x40);
                    } else if (unicode_mode == 0) {  // macOS
                        color = lazercore_make_rgb(0x40, 0x40, 0xFF);
                    } else {
                        color = dim60;
                    }
                    break;
                }
#ifdef AUTOCORRECT_ENABLE
                case LAZERCORE_LED_AUTOCORRECT:
                    if (autocorrect_is_enabled()) {
                        color = lazercore_make_rgb(0x40, 0xFF, 0x80);
                    }
                    break;
#endif
                case LAZERCORE_LED_CAPS:
                    if (is_sentence_case_on()) {
                        color = lazercore_make_rgb(0xFF, 0xC4, 0x60);
                    }
                    break;
                case LAZERCORE_LED_LGUI:
                    color = keymap_config.no_gui ? lazercore_make_rgb(0xFF, 0x40, 0x40)
                                                 : lazercore_make_rgb(0x40, 0xCC, 0xFF);
                    break;
                default:
                    break;
            }

            if (fn_active && (led == LAZERCORE_LED_ENTER || led == LAZERCORE_LED_RSFT)) {
                color = lazercore_make_rgb(0x40, 0x80, 0xFF);
            }
        } else {
            color = global_flash_color;
        }

        rgb_matrix_set_color(led, color.r, color.g, color.b);
    }

    if (layer == 3) {
        handle_audio_viz(led_min, led_max);
    }

    return false;
}

