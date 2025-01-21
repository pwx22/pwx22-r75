#include "type_alchemy.h"
#include "quantum.h"
#include <string.h>
#include <stdlib.h>
#include "send_string.h"

/* Static Variables */
static bool type_alchemy_active = false;
static char typed_buffer[MAX_BUFFER_SIZE]= "" ;
static uint8_t buffer_index = 0;

/* Dynamic array for user mappings */
static WordSymbolMapping mappings[MAX_MAPPING_SIZE];
static size_t mapping_count = 0;

/* Data Structure for Word-to-Symbol Mapping*/
static const WordSymbolMapping default_mappings[] = {
    {"aum", "ॐ"},              // Om symbol
    {"inr", "₹"},              // Indian Rupee symbol
    {"pi", "π"},               // Pi symbol
    {"degree", "°"},           // Degree symbol
    {"micro", "µ"},            // Micro symbol
    {"integral", "∫"},         // Integral symbol
    {"infinity", "∞"},         // Infinity symbol
    {"sigma", "Σ"},            // Summation symbol
    {"delta", "Δ"},            // Delta symbol
    {"theta", "θ"},            // Theta symbol
    {"alpha", "α"},            // Alpha symbol
    {"beta", "β"},             // Beta symbol
    {"gamma", "γ"},            // Gamma symbol
    {"lambda", "λ"},           // Lambda symbol
    {"omega", "Ω"},            // Omega symbol
    {"sqrt", "√"},             // Square root symbol
    {"not_equal", "≠"},        // Not equal to symbol
    {"less_equal", "≤"},       // Less than or equal to symbol
    {"greater_equal", "≥"},    // Greater than or equal to symbol
    {"approx", "≈"},           // Approximately equal to symbol
    {"arrow_right", "→"},      // Right arrow symbol
    {"arrow_left", "←"},       // Left arrow symbol
    {"arrow_up", "↑"},         // Up arrow symbol
    {"arrow_down", "↓"},       // Down arrow symbol
    {"percent", "%"},          // Percent symbol
    {"times", "×"},            // Multiplication symbol
    {"divide", "÷"},           // Division symbol
    {"ellipsis", "…"},         // Ellipsis symbol
    {"caret", "^"},            // Caret symbol
    {"logical_and", "∧"},      // Logical AND symbol
    {"logical_or", "∨"},       // Logical OR symbol
    {"perpendicular", "⊥"},    // Perpendicular symbol
    {"parallel", "∥"},         // Parallel symbol
    // Common Emojis
    {"smile", "😊"},           // Smiling face
    {"heart", "❤️"},           // Heart
    {"thumbs_up", "👍"},       // Thumbs up
    {"fire", "🔥"},            // Fire
};


static const size_t default_mapping_count = sizeof(default_mappings)/sizeof(default_mappings[0]);

/* init func */
void init_type_alchemy(void){
    type_alchemy_active = false;
    buffer_index = 0;
    memset(typed_buffer, 0, MAX_BUFFER_SIZE);

    // Load default mappings
    mapping_count = default_mapping_count;
    for (size_t i = 0; i < default_mapping_count; i++) {
        mappings[i] = default_mappings[i];
    }
}

/* API to add new word to symbol mapping */
bool add_word_symbol_mapping(const char *word, const char *symbol) {
    if (mapping_count >= MAX_MAPPING_SIZE) {
        return false; // Mapping array is full
    }
    mappings[mapping_count].word = word;
    mappings[mapping_count].symbol = symbol;
    mapping_count++;
    return true;
}
/* Toggle feature */
void toggle_type_alchemy(void){
    type_alchemy_active = !type_alchemy_active;
}

/* Check state */
bool is_type_alchemy_on(void){
    return type_alchemy_active;
}

/* Processing key events for type alchemy */
bool process_type_alchemy(uint16_t keycode, char typed_char, bool is_pressed) {
    if (!type_alchemy_active || !is_pressed) {
        return true; // Pass through if the feature is off or the key is released
    }

    // Process alphabetic characters
    if ((typed_char >= 'a' && typed_char <= 'z') || (typed_char >= 'A' && typed_char <= 'Z')) {
        if (buffer_index < sizeof(typed_buffer) - 1) {
            typed_buffer[buffer_index++] = typed_char;
            typed_buffer[buffer_index] = '\0'; // Null-terminate the string
        }

        // Check if the buffer matches any mapping
        for (size_t i = 0; i < mapping_count; i++) {
            if (strcmp(typed_buffer, mappings[i].word) == 0) {
                if (mappings[i].symbol != NULL) {
                    // Remove the typed word by sending backspace for each character
                    for (size_t j = 0; j < strlen(mappings[i].word)-1; j++) {
                        register_code(KC_BACKSPACE); // Send backspace keycode
                    }

                    // Send the mapped symbol
                    send_unicode_string(mappings[i].symbol);
                }

                // Reset the buffer
                buffer_index = 0;
                memset(typed_buffer, 0, sizeof(typed_buffer));
                return false; // Consume the keypress
            }
        }
    } else {
        // Non-alphabetic characters reset the buffer
        buffer_index = 0;
        memset(typed_buffer, 0, sizeof(typed_buffer));
    }

    return true; // Pass through other keys
}
