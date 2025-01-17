#ifndef AUDIO_VIZ_H
#define AUDIO_VIZ_H

#include "quantum.h"
#include "rgb_matrix.h"

extern bool audio_viz_enabled;

void enable_audio_viz(void);
void disable_audio_viz(void);

void handle_audio_viz(uint8_t led_min, uint8_t led_max);
#endif // AUDIO_VIZ_H
