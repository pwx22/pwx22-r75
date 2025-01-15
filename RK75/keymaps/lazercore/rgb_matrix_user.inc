// Step 1: Declare the custom effect
RGB_MATRIX_EFFECT(game_mode_effect)

// Step 2: Define the effect logic inside the `RGB_MATRIX_CUSTOM_EFFECT_IMPLS` block
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// Custom RGB Matrix Game Mode Effect
static bool game_mode_effect(effect_params_t* params) {
  // Use limits to ensure we work within the bounds of the available LED matrix
  RGB_MATRIX_USE_LIMITS(led_min, led_max);

  // Set background color to #0432b1 for all keys (light pink)
  for (uint8_t i = led_min; i < led_max; i++) {
    rgb_matrix_set_color(i, 0x10, 0x10, 0x10);  // Set background for all keys
  }
  
  // Set full brightness for WASD, Space, and Ctrl keys
  rgb_matrix_set_color(47, 0xFF, 0x00, 0x00); // W
  rgb_matrix_set_color(51, 0xFF, 0x00, 0x00); // A
  rgb_matrix_set_color(52, 0xFF, 0x00, 0x00); // S
  rgb_matrix_set_color(53, 0xFF, 0x00, 0x00); // D
  // Set full brightness for 1, 2, 3, and 4 keys


  // Check if the effect has finished
  return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
