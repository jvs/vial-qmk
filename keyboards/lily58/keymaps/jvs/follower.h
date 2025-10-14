#pragma once

#include "quantum.h"
#include <string.h>

// Configuration
#ifndef FOLLOWER_BUFFER_SIZE
#define FOLLOWER_BUFFER_SIZE 15
#endif

#ifndef FOLLOWER_MAX_OUTPUT
#define FOLLOWER_MAX_OUTPUT 64
#endif

// Expansion definition
typedef struct {
    const char* input;
    const char* output;
    int8_t cursor_offset;  // 0 = end of output, negative = back from end
} follower_expansion_t;

// Macros for defining expansions
#define FOLLOWER_EXPANSION(in, out) {in, out, 0}
#define FOLLOWER_EXPANSION_CURSOR(in, out, offset) {in, out, offset}

// Internal state
static struct {
    char buffer[FOLLOWER_BUFFER_SIZE];
    uint8_t length;  // Current number of chars in buffer
} follower_state = {0};

// Helper: Check if buffer ends with target string
static bool follower_ends_with(const char* target) {
    size_t target_len = strlen(target);
    if (target_len > follower_state.length) {
        return false;
    }

    // Compare end of buffer with target
    size_t start = follower_state.length - target_len;
    return memcmp(follower_state.buffer + start, target, target_len) == 0;
}

// Helper: Add character to buffer
static void follower_add_char(char c) {
    if (follower_state.length < FOLLOWER_BUFFER_SIZE) {
        follower_state.buffer[follower_state.length++] = c;
    } else {
        // Shift buffer left and add to end (rolling window)
        memmove(follower_state.buffer, follower_state.buffer + 1, FOLLOWER_BUFFER_SIZE - 1);
        follower_state.buffer[FOLLOWER_BUFFER_SIZE - 1] = c;
    }
}

// Helper: Remove last character (backspace)
static void follower_remove_char(void) {
    if (follower_state.length > 0) {
        follower_state.length--;
    }
}

// Helper: Convert QMK keycode to character
static bool keycode_to_char(uint16_t keycode, bool shifted, char* out) {
    // Letters
    if (keycode >= KC_A && keycode <= KC_Z) {
        *out = shifted ? ('A' + (keycode - KC_A)) : ('a' + (keycode - KC_A));
        return true;
    }

    // Numbers and symbols
    if (keycode >= KC_1 && keycode <= KC_0) {
        if (shifted) {
            const char symbols[] = "!@#$%^&*()";
            *out = symbols[keycode - KC_1];
        } else {
            *out = (keycode == KC_0) ? '0' : ('1' + (keycode - KC_1));
        }
        return true;
    }

    // Common symbols (using QMK keycode names)
    switch (keycode) {
        case KC_SPC:  *out = ' '; return true;
        case KC_MINS: *out = shifted ? '_' : '-'; return true;
        case KC_EQL:  *out = shifted ? '+' : '='; return true;
        case KC_LBRC: *out = shifted ? '{' : '['; return true;
        case KC_RBRC: *out = shifted ? '}' : ']'; return true;
        case KC_BSLS: *out = shifted ? '|' : '\\'; return true;
        case KC_SCLN: *out = shifted ? ':' : ';'; return true;
        case KC_QUOT: *out = shifted ? '"' : '\''; return true;
        case KC_GRV:  *out = shifted ? '~' : '`'; return true;
        case KC_COMM: *out = shifted ? '<' : ','; return true;
        case KC_DOT:  *out = shifted ? '>' : '.'; return true;
        case KC_SLSH: *out = shifted ? '?' : '/'; return true;
    }

    return false;
}

// Track keypresses - call this from process_record_user before other processing
void follower_track_key(uint16_t keycode, keyrecord_t* record) {
    if (!record->event.pressed) {
        return;  // Only track on key press
    }

    // Handle backspace
    if (keycode == KC_BSPC) {
        follower_remove_char();
        return;
    }

    // Try to convert keycode to character
    bool shifted = (get_mods() & MOD_MASK_SHIFT) != 0;
    char c;
    if (keycode_to_char(keycode, shifted, &c)) {
        follower_add_char(c);
    }
}

// Process follower key press - returns true if expansion was performed
// NOTE: Assumes expansions array is sorted longest-input-first!
bool follower_process(const follower_expansion_t* expansions, size_t num_expansions) {
    // Find first (longest) matching expansion
    int best_match = -1;

    for (size_t i = 0; i < num_expansions; i++) {
        if (follower_ends_with(expansions[i].input)) {
            best_match = i;
            break;  // Found match, and it's the longest due to sorted array
        }
    }

    if (best_match == -1) {
        return false;  // No match found
    }

    // Execute expansion
    const follower_expansion_t* exp = &expansions[best_match];
    size_t input_len = strlen(exp->input);

    // Emit backspaces to delete input
    for (size_t i = 0; i < input_len; i++) {
        tap_code(KC_BSPC);
    }

    // Type output string, handling special escape sequences
    const char* output_str = exp->output;
    bool ends_with_enter = false;

    // Check if output ends with "\n" (literal backslash-n)
    size_t len = strlen(output_str);
    if (len >= 2 && output_str[len-2] == '\\' && output_str[len-1] == 'n') {
        ends_with_enter = true;
        // Send string without the "\n"
        char temp[FOLLOWER_MAX_OUTPUT];
        strncpy(temp, output_str, len - 2);
        temp[len - 2] = '\0';
        send_string(temp);
    } else {
        send_string(output_str);
    }

    // Handle special key at end
    if (ends_with_enter) {
        tap_code(KC_ENTER);
    }

    // Handle cursor positioning if specified
    if (exp->cursor_offset < 0) {
        for (int i = 0; i < -exp->cursor_offset; i++) {
            tap_code(KC_LEFT);
        }
    }

    // Update buffer: remove input length, add output characters (excluding \n)
    if (follower_state.length >= input_len) {
        follower_state.length -= input_len;
    } else {
        follower_state.length = 0;
    }

    for (const char* p = output_str; *p; p++) {
        // Stop before "\n" escape sequence
        if (*p == '\\' && *(p+1) == 'n') {
            break;
        }
        follower_add_char(*p);
    }

    return true;
}

// Optional: Clear buffer (useful for debugging or manual reset)
void follower_clear(void) {
    follower_state.length = 0;
}

// Optional: Get current buffer contents (for debugging/display)
const char* follower_get_buffer(uint8_t* length) {
    if (length) {
        *length = follower_state.length;
    }
    return follower_state.buffer;
}
