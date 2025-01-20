/* 
 * Copyright 2025 Lazercore 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#ifndef AUDIO_VIZ_H
#define AUDIO_VIZ_H

#include "quantum.h"
#include "rgb_matrix.h"
#include "via.h"

extern bool audio_viz_enabled;

void enable_audio_viz(void);
void disable_audio_viz(void);

void handle_audio_viz(uint8_t led_min, uint8_t led_max);
#endif // AUDIO_VIZ_H
