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

#ifndef TYPE_ALCHEMY_H
#define TYPE_ALCHEMY_H
#include "quantum.h"
#include <stdbool.h>
#include <stdint.h>

/*Constants defined here*/
#define MAX_BUFFER_SIZE 16
#define MAX_MAPPING_SIZE 64

/* Data structure for the mapping */
typedef struct {
    const char *word;   // Word to match
    const char *symbol; // Corresponding symbol
} WordSymbolMapping;

// Public API Functions
void init_type_alchemy(void);                          // Initialize the type alchemy feature
bool add_word_symbol_mapping(const char *word, const char *symbol); // Add a new mapping
void toggle_type_alchemy(void);                       // Toggle the feature on or off
bool is_type_alchemy_on(void);                    // Check if the feature is active
bool process_type_alchemy(uint16_t keycode, char typed_char, bool is_pressed); // Process key events

#endif // TYPE_ALCHEMY_H

