// game_mode.c
#include "game_mode.h"
#include "quantum.h"

uint8_t previous_rgb_matrix_mode;

bool game_mode_enabled = false;

void enable_game_mode(void) {
    game_mode_enabled = true;
    layer_clear();
    layer_on(0);
    previous_rgb_matrix_mode = rgb_matrix_get_mode();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
}

void disable_game_mode(void) {
    game_mode_enabled = false;
    layer_clear();
    layer_on(0);
    rgb_matrix_mode(previous_rgb_matrix_mode);
}
