#include "quantum.h"
#include "rgb_matrix.h"
#include "via.h"
#include "print.h"

bool audio_viz_enabled = false;
uint8_t audio_viz_data[32]; 

#ifdef RAW_ENABLE
bool via_command_kb(uint8_t *data, uint8_t length) {
    // Copy Audio Data when on Layer3 [audio visualiser layer]
    if (get_highest_layer(layer_state | default_layer_state) == 3) {
        memcpy(audio_viz_data, data, length);
        matrix_scan();
        return true;
    } else {
        return false;
    }
}
#   if !defined(VIA_ENABLE)
// Weak override for raw_hid_receive_user, allowing keymap override
__attribute__((weak)) bool raw_hid_receive_user(uint8_t *data, uint8_t length) {
    return false;
}

// Simple implementation of raw_hid_receive with user-level override
void raw_hid_receive(uint8_t *data, uint8_t length) {
    // Check if keymap-level user function is implemented
    if (raw_hid_receive_user(data, length)) return;
    
    // If VIA is enabled and the custom function doesn't handle it, handle it here
    if (via_command_kb(data, length)) return;

    // If neither handler takes care of the data, respond with default action
    // You may want to implement a default behavior here (e.g., default raw HID handling)
    data[0] = 0;  // Clear the first byte
    replaced_hid_send(data, length); // Send the data back (or handle as necessary)
}
#   endif
#endif


void enable_audio_viz(void) {
    audio_viz_enabled = true;
    rgb_matrix_set_color(21, 255, 0, 0); // Red for the indicator LED
    layer_clear();
    layer_on(3);
}

void disable_audio_viz(void) {
    audio_viz_enabled = false;
    layer_clear();
    layer_on(0);
}

void handle_audio_viz(uint8_t led_min, uint8_t led_max) {
    const uint8_t col_groups[6][2] = {
        {0, 2},  // Band 1: columns 0, 1, 2
        {3, 4},  // Band 2: columns 3, 4
        {5, 7},  // Band 3: columns 5, 6, 7
        {8, 9},  // Band 4: columns 8, 9
        {10, 11}, // Band 5: columns 10, 11
        {12, 14}  // Band 6: columns 12, 13, 14
    };

    for (uint8_t band = 0; band < 6; band++) {
        uint8_t leds_to_light = audio_viz_data[band];

        // Get column range for this band
        uint8_t col_start = col_groups[band][0];
        uint8_t col_end = col_groups[band][1];

        // Light up LEDs for each column in the band
        for (uint8_t col = col_start; col <= col_end; col++) {
            for (uint8_t row = 5; row >= 6 - leds_to_light; row--) { 
                uint8_t led_index = g_led_config.matrix_co[row][col];
                if (row == 5) {
                    rgb_matrix_set_color(led_index, 128, 0, 128); // Deep Purple
                } else if (row == 4) {
                    rgb_matrix_set_color(led_index, 0, 0, 255); // Electric Blue
                } else if (row == 3) {
                    rgb_matrix_set_color(led_index, 255, 0, 255); // Magenta
                } else if (row == 2) {
                    rgb_matrix_set_color(led_index, 220, 20, 60); // Crimson
                } else if (row == 1) {
                    rgb_matrix_set_color(led_index, 153, 50, 204); // Electric Purple
                }


            }
            // Turn off remaining LEDs
            for (uint8_t row = 6 - leds_to_light - 1; row < 6; row--) {
                uint8_t led_index = g_led_config.matrix_co[row][col];
                rgb_matrix_set_color(led_index, 0, 0, 0); // Turn off
            }
        }
    }
}
