// game_mode.c
#include "game_mode.h"
#include "quantum.h"

bool game_mode_enabled = false;

void enable_game_mode(void) {
    game_mode_enabled = true;
    layer_clear();
    layer_on(0);
    rgb_matrix_mode(RGB_MATRIX_CUSTOM_game_mode_effect);
}

void disable_game_mode(void) {
    game_mode_enabled = false;
    layer_clear();
    layer_on(0);
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
}
