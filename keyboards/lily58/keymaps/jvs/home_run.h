/* Home Run Modifiers
 * A simplified home row modifier library for QMK
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
#pragma once

#include QMK_KEYBOARD_H
#include "timer.h"

/* ************************************* *
 *         GLOBAL CONFIGURATION          *
 * ************************************* */

// Time limit for full overlap detection (key fully pressed & released while home-run key held)
#ifndef HOME_RUN_FULL_OVERLAP_LIMIT
#define HOME_RUN_FULL_OVERLAP_LIMIT 150
#endif

// Time limit for partial overlap detection (home-run key held this long = modifier)
#ifndef HOME_RUN_PARTIAL_OVERLAP_LIMIT
#define HOME_RUN_PARTIAL_OVERLAP_LIMIT 250
#endif

// Maximum number of simultaneous home-run keys being tracked
#ifndef HOME_RUN_MAX_ACTIVE
#define HOME_RUN_MAX_ACTIVE 10
#endif

// Maximum number of events to buffer per home-run key
#ifndef HOME_RUN_BUFFER_SIZE
#define HOME_RUN_BUFFER_SIZE 20
#endif

/* ************************************* *
 *             TYPE DEFINITIONS          *
 * ************************************* */

typedef enum {
    HOME_RUN_STATE_UNKNOWN,   // Don't know yet if it's a modifier or normal key
    HOME_RUN_STATE_MODIFIER,  // Determined to be a modifier
    HOME_RUN_STATE_NORMAL     // Determined to be a normal key
} home_run_state_t;

typedef struct {
    uint16_t keycode;
    bool     pressed;
    keypos_t key;
    uint16_t timestamp;  // When this event occurred (from timer_read())
} home_run_buffered_event_t;

typedef struct {
    uint16_t                    keycode;           // The home-run key being tracked
    uint16_t                    press_time;        // When the key was pressed
    home_run_state_t           state;             // Current state
    bool                       active;            // Is this slot active?
    keypos_t                   home_key;           // The home-run key's position
    bool                       requires_opposite_hand; // Only trigger as modifier for opposite hand

    // Track other key presses for overlap detection
    bool                       other_key_pressed;  // Another key was pressed
    uint16_t                   other_keycode;      // The other key's keycode
    keypos_t                   other_key;          // The other key's position
    bool                       other_key_released; // The other key was released

    // Event buffer (now includes the home-run key itself)
    home_run_buffered_event_t  buffer[HOME_RUN_BUFFER_SIZE];
    uint8_t                    buffer_count;
} home_run_tracked_key_t;

/* ************************************* *
 *       USER ACTION DEFINITIONS         *
 * ************************************* */

typedef enum {
    HOME_RUN_ACTION_TAP,      // Key was tapped (normal key press)
    HOME_RUN_ACTION_HOLD,     // Key is being held as modifier
    HOME_RUN_ACTION_RELEASE,  // Key is being released (after acting as modifier)
} home_run_action_t;

// User callback - implement this in your keymap
void on_home_run_action(uint16_t keycode, home_run_action_t action);

// Optional user callback - implement this to enable opposite-hand detection
// Return true if this keycode should only trigger as modifier for opposite-hand keys
__attribute__((weak)) bool home_run_requires_opposite_hand(uint16_t keycode);

/* ************************************* *
 *         INTERNAL STATE                *
 * ************************************* */

// Single tracker - we only track one home-run key at a time
static home_run_tracked_key_t home_run_tracked;

// Global flag to prevent recursive processing during buffer replay
static bool replaying_buffer = false;

/* ************************************* *
 *      MENU LAYER STATE                 *
 * ************************************* */

// Track active menu layer for undo/replace behavior
static uint16_t active_menu_key = 0;      // Which home-run key activated the menu
static uint8_t menu_emit_length = 0;       // Length of last emitted sequence

/* ************************************* *
 *         HELPER FUNCTIONS              *
 * ************************************* */

// Check if two keys are on the same hand (for split keyboards)
static bool same_hand(keypos_t key1, keypos_t key2) {
    // For split keyboards, rows are typically divided between halves
    // Keys on the same half will be on the same side of MATRIX_ROWS/2
    return (key1.row < MATRIX_ROWS / 2) == (key2.row < MATRIX_ROWS / 2);
}

// Add an event to the buffer
static void home_run_buffer_add(home_run_tracked_key_t* tracked, uint16_t keycode, bool pressed, keypos_t key, uint16_t timestamp) {
    if (tracked->buffer_count < HOME_RUN_BUFFER_SIZE) {
        tracked->buffer[tracked->buffer_count].keycode = keycode;
        tracked->buffer[tracked->buffer_count].pressed = pressed;
        tracked->buffer[tracked->buffer_count].key = key;
        tracked->buffer[tracked->buffer_count].timestamp = timestamp;
        tracked->buffer_count++;
    }
}

// Helper: Calculate elapsed time between two timestamps, handling timer wraparound
static uint16_t timer_elapsed_safe(uint16_t start, uint16_t end) {
    if (end >= start) {
        return end - start;
    } else {
        // Timer wrapped around
        return (UINT16_MAX - start) + end + 1;
    }
}

// Helper: Try to resolve a home-run key from buffered events
// Returns the resolved state, or UNKNOWN if we can't determine yet
// start_idx: index of the home-run key press in the buffer
// hr_keycode: the home-run keycode to resolve
// hr_timestamp: when the home-run key was pressed
// requires_opposite: does this key require opposite-hand detection?
// hr_key: the home-run key's position
static home_run_state_t home_run_try_resolve_from_buffer(
    home_run_buffered_event_t* buffer,
    uint8_t buffer_count,
    uint8_t start_idx,
    uint16_t hr_keycode,
    uint16_t hr_timestamp,
    bool requires_opposite,
    keypos_t hr_key
) {
    // Look for release of this home-run key
    uint8_t release_idx = 0;
    bool found_release = false;
    for (uint8_t i = start_idx + 1; i < buffer_count; i++) {
        if (buffer[i].keycode == hr_keycode && !buffer[i].pressed) {
            release_idx = i;
            found_release = true;
            break;
        }
    }

    if (found_release) {
        // We have a complete press/release cycle in the buffer
        uint16_t elapsed = timer_elapsed_safe(hr_timestamp, buffer[release_idx].timestamp);

        // Check for same-hand roll (opposite-hand detection)
        if (requires_opposite) {
            for (uint8_t i = start_idx + 1; i < release_idx; i++) {
                if (buffer[i].pressed && buffer[i].keycode != hr_keycode) {
                    if (same_hand(hr_key, buffer[i].key)) {
                        return HOME_RUN_STATE_NORMAL; // Same-hand roll
                    }
                }
            }
        }

        // Check timing: if released quickly, it's a tap
        if (elapsed <= HOME_RUN_FULL_OVERLAP_LIMIT) {
            return HOME_RUN_STATE_NORMAL;
        }

        // Check for full overlap
        bool other_pressed = false;
        bool other_released = false;
        for (uint8_t i = start_idx + 1; i < release_idx; i++) {
            if (buffer[i].keycode != hr_keycode) {
                if (buffer[i].pressed) other_pressed = true;
                if (!buffer[i].pressed) other_released = true;
            }
        }

        if (elapsed > HOME_RUN_FULL_OVERLAP_LIMIT && other_pressed && other_released) {
            return HOME_RUN_STATE_MODIFIER;
        }

        if (elapsed > HOME_RUN_PARTIAL_OVERLAP_LIMIT) {
            return HOME_RUN_STATE_MODIFIER;
        }

        // Default: normal tap
        return HOME_RUN_STATE_NORMAL;
    }

    // No release yet - can we determine from current time?
    uint16_t elapsed = timer_elapsed(hr_timestamp);
    if (elapsed > HOME_RUN_PARTIAL_OVERLAP_LIMIT) {
        return HOME_RUN_STATE_MODIFIER;
    }

    return HOME_RUN_STATE_UNKNOWN; // Still don't know
}

// Main flush function - processes buffered events with correct interpretation
static void home_run_buffer_flush(home_run_tracked_key_t* tracked) {
    replaying_buffer = true;

    uint8_t i = 0;
    while (i < tracked->buffer_count) {
        home_run_buffered_event_t* event = &tracked->buffer[i];

        // Check if this is the tracked home-run key itself
        if (event->keycode == tracked->keycode) {
            if (tracked->state == HOME_RUN_STATE_NORMAL) {
                // Normal tap: emit on press, skip release
                if (event->pressed) {
                    on_home_run_action(tracked->keycode, HOME_RUN_ACTION_TAP);
                }
                // Skip release (tap already did press+release)
            } else if (tracked->state == HOME_RUN_STATE_MODIFIER) {
                // Modifier: register on press, unregister on release
                if (event->pressed) {
                    on_home_run_action(tracked->keycode, HOME_RUN_ACTION_HOLD);
                } else {
                    on_home_run_action(tracked->keycode, HOME_RUN_ACTION_RELEASE);
                }
            }
            i++;
            continue;
        }

        // Check if this is a nested home-run key
        if (event->pressed && event->keycode >= HOME_RUN_KEYCODES_BEGIN && event->keycode <= HOME_RUN_KEYCODES_END) {
            // Nested home-run key behavior depends on parent state
            if (tracked->state == HOME_RUN_STATE_MODIFIER) {
                // Parent is modifier: nested HR keys process as their tap behavior on the active layer
                // Replay the event so it goes through the layer system
                keyevent_t ke = MAKE_KEYEVENT(event->key.row, event->key.col, event->pressed);
                keyrecord_t record = {.event = ke};
                process_record(&record);

                // Also replay the release
                i++;
                while (i < tracked->buffer_count) {
                    if (tracked->buffer[i].keycode == event->keycode && !tracked->buffer[i].pressed) {
                        keyevent_t ke_release = MAKE_KEYEVENT(tracked->buffer[i].key.row, tracked->buffer[i].key.col, false);
                        keyrecord_t record_release = {.event = ke_release};
                        process_record(&record_release);
                        i++; // Skip past the release
                        break;
                    }
                    i++;
                }
                continue;
            } else if (tracked->state == HOME_RUN_STATE_NORMAL) {
                // Parent is normal: try to resolve this nested HR key
                bool requires_opposite = home_run_requires_opposite_hand && home_run_requires_opposite_hand(event->keycode);
                home_run_state_t nested_state = home_run_try_resolve_from_buffer(
                    tracked->buffer,
                    tracked->buffer_count,
                    i,
                    event->keycode,
                    event->timestamp,
                    requires_opposite,
                    event->key
                );

                if (nested_state == HOME_RUN_STATE_UNKNOWN) {
                    // Can't resolve - resume tracking this nested key
                    tracked->keycode = event->keycode;
                    tracked->press_time = event->timestamp;
                    tracked->home_key = event->key;
                    tracked->requires_opposite_hand = requires_opposite;
                    tracked->state = HOME_RUN_STATE_UNKNOWN;

                    // Remove consumed events from buffer
                    uint8_t remaining = tracked->buffer_count - i;
                    for (uint8_t j = 0; j < remaining; j++) {
                        tracked->buffer[j] = tracked->buffer[i + j];
                    }
                    tracked->buffer_count = remaining;

                    // Reset tracking state
                    tracked->other_key_pressed = false;
                    tracked->other_key_released = false;

                    replaying_buffer = false;
                    return; // Stop flushing, continue tracking
                }

                // Resolved: emit and continue
                if (nested_state == HOME_RUN_STATE_NORMAL) {
                    on_home_run_action(event->keycode, HOME_RUN_ACTION_TAP);
                } else { // MODIFIER
                    on_home_run_action(event->keycode, HOME_RUN_ACTION_HOLD);
                    // Find and emit release
                    uint8_t j = i + 1;
                    while (j < tracked->buffer_count) {
                        if (tracked->buffer[j].keycode == event->keycode && !tracked->buffer[j].pressed) {
                            on_home_run_action(event->keycode, HOME_RUN_ACTION_RELEASE);
                            break;
                        }
                        j++;
                    }
                }
                // Skip to release
                i++;
                while (i < tracked->buffer_count) {
                    if (tracked->buffer[i].keycode == event->keycode && !tracked->buffer[i].pressed) {
                        i++;
                        break;
                    }
                    i++;
                }
                continue;
            }
        }

        // Regular key: replay normally
        keyevent_t ke = MAKE_KEYEVENT(event->key.row, event->key.col, event->pressed);
        keyrecord_t record = {.event = ke};
        process_record(&record);
        i++;
    }

    // All events consumed
    tracked->buffer_count = 0;
    replaying_buffer = false;
}

// Check if we can determine the state yet
static bool home_run_check_state(home_run_tracked_key_t* tracked) {
    if (tracked->state != HOME_RUN_STATE_UNKNOWN) {
        return true; // Already determined
    }

    uint16_t elapsed = timer_elapsed(tracked->press_time);

    // Case 2: Held longer than PARTIAL_OVERLAP_LIMIT
    if (elapsed > HOME_RUN_PARTIAL_OVERLAP_LIMIT) {
        tracked->state = HOME_RUN_STATE_MODIFIER;
        return true;
    }

    // Case 3: Full overlap - another key pressed and released while held, and past FULL_OVERLAP_LIMIT
    if (elapsed > HOME_RUN_FULL_OVERLAP_LIMIT &&
        tracked->other_key_pressed &&
        tracked->other_key_released) {
        tracked->state = HOME_RUN_STATE_MODIFIER;
        return true;
    }

    return false; // Still unknown
}

// Transition to a determined state and flush buffer
static void home_run_finalize_state(home_run_tracked_key_t* tracked) {
    // Flush buffer with the resolved interpretation
    home_run_buffer_flush(tracked);
}

// Clear a tracked key
static void home_run_clear_tracked(home_run_tracked_key_t* tracked) {
    tracked->active = false;
    tracked->keycode = 0;
    tracked->state = HOME_RUN_STATE_UNKNOWN;
    tracked->buffer_count = 0;
    tracked->other_key_pressed = false;
    tracked->other_key_released = false;
}

/* ************************************* *
 *      MENU LAYER FUNCTIONS             *
 * ************************************* */

// Emit a string for a menu layer item with automatic undo/replace
// Returns false to indicate keycode was handled
static bool menu_emit(uint16_t menu_key, const char* str) {
    uint8_t len = 0;

    // Calculate string length
    while (str[len] != '\0') len++;

    // If this menu is active and we previously emitted something, backspace it first
    if (active_menu_key == menu_key && menu_emit_length > 0) {
        for (uint8_t i = 0; i < menu_emit_length; i++) {
            tap_code(KC_BSPC);
        }
    }

    // Set this as the active menu
    active_menu_key = menu_key;
    menu_emit_length = len;

    // Emit the string
    send_string(str);

    return false;
}

// Clear menu state when menu key is released
static void clear_menu_state(uint16_t menu_key) {
    if (active_menu_key == menu_key) {
        active_menu_key = 0;
        menu_emit_length = 0;
    }
}

/* ************************************* *
 *      ENTRY POINT IMPLEMENTATION       *
 * ************************************* */

bool process_home_run(uint16_t keycode, keyrecord_t* record) {
    // Don't process if we're replaying buffered events
    if (replaying_buffer) {
        return true;
    }

    // If we're already tracking something, handle it
    if (home_run_tracked.active) {
        // Check if this is the release of the tracked home-run key
        if (!record->event.pressed && keycode == home_run_tracked.keycode) {
            if (home_run_tracked.state == HOME_RUN_STATE_UNKNOWN) {
                // Buffer the release FIRST
                home_run_buffer_add(&home_run_tracked, keycode, record->event.pressed, record->event.key, timer_read());

                // NOW check timing to determine state
                home_run_check_state(&home_run_tracked);

                // If still unknown after timing check, it's a normal tap
                if (home_run_tracked.state == HOME_RUN_STATE_UNKNOWN) {
                    home_run_tracked.state = HOME_RUN_STATE_NORMAL;
                }

                // Flush buffer with appropriate interpretation
                home_run_finalize_state(&home_run_tracked);

                // Clean up
                home_run_clear_tracked(&home_run_tracked);
            } else if (home_run_tracked.state == HOME_RUN_STATE_MODIFIER) {
                // Already determined as modifier, just release it
                on_home_run_action(home_run_tracked.keycode, HOME_RUN_ACTION_RELEASE);

                // Clean up
                home_run_clear_tracked(&home_run_tracked);
            }

            return false; // Handled
        }

        // If state is already determined as MODIFIER
        if (home_run_tracked.state == HOME_RUN_STATE_MODIFIER) {
            // If this is another home-run key, let it process through the layer
            // (it will act as its tap behavior on the active layer)
            if (keycode >= HOME_RUN_KEYCODES_BEGIN && keycode <= HOME_RUN_KEYCODES_END) {
                return true; // Let it process on the active layer
            }
            // Regular keys process normally with modifier active
            return true;
        }

        // State is UNKNOWN - ALWAYS buffer event FIRST
        home_run_buffer_add(&home_run_tracked, keycode, record->event.pressed, record->event.key, timer_read());

        // Track overlap for timing detection
        if (record->event.pressed) {
            if (!home_run_tracked.other_key_pressed) {
                home_run_tracked.other_key_pressed = true;
                home_run_tracked.other_keycode = keycode;
                home_run_tracked.other_key = record->event.key;
            }
        } else {
            // Key released - check if it's the other key we're tracking
            if (home_run_tracked.other_key_pressed &&
                home_run_tracked.other_key.row == record->event.key.row &&
                home_run_tracked.other_key.col == record->event.key.col) {
                home_run_tracked.other_key_released = true;
            }
        }

        // NOW check if we can interpret the HR key
        // Check timing first
        if (home_run_check_state(&home_run_tracked)) {
            home_run_finalize_state(&home_run_tracked);

            // If it was determined as MODIFIER, keep tracking (don't clear)
            // so we can properly release it later
            if (home_run_tracked.state == HOME_RUN_STATE_NORMAL) {
                home_run_clear_tracked(&home_run_tracked);
            }
            return false;
        }

        // Check for same-hand roll (only for opposite-hand HR keys)
        if (record->event.pressed &&
            home_run_tracked.requires_opposite_hand &&
            same_hand(home_run_tracked.home_key, record->event.key)) {
            // Immediately resolve as normal key
            home_run_tracked.state = HOME_RUN_STATE_NORMAL;
            home_run_finalize_state(&home_run_tracked);

            // Check if flush switched tracking to a nested key
            if (!home_run_tracked.active) {
                // Fully resolved and cleared
                return false;
            }
            // Still tracking nested key - fall through
        }

        return false; // Event is buffered, don't process normally
    }

    // Not currently tracking anything - check if this is a new home-run key press
    if (record->event.pressed && keycode >= HOME_RUN_KEYCODES_BEGIN && keycode <= HOME_RUN_KEYCODES_END) {
        // Start tracking this home-run key
        uint16_t now = timer_read();
        home_run_tracked.active = true;
        home_run_tracked.keycode = keycode;
        home_run_tracked.press_time = now;
        home_run_tracked.state = HOME_RUN_STATE_UNKNOWN;
        home_run_tracked.buffer_count = 0;
        home_run_tracked.other_key_pressed = false;
        home_run_tracked.other_key_released = false;
        home_run_tracked.home_key = record->event.key;
        home_run_tracked.requires_opposite_hand = home_run_requires_opposite_hand && home_run_requires_opposite_hand(keycode);

        // Buffer the home-run key press itself
        home_run_buffer_add(&home_run_tracked, keycode, record->event.pressed, record->event.key, now);

        return false; // Handled, don't process normally
    }

    return true; // Not a home-run event, continue normal processing
}

/* ************************************* *
 *         CUSTOMIZATION MACROS          *
 * ************************************* */

// Helper macro to define a home-run mod-tap key
// Usage: HOME_RUN_MT(keycode, tap_key, mod)
#define HOME_RUN_MT(hr_keycode, tap_key, mod)              \
    case hr_keycode: {                                      \
        switch (action) {                                   \
            case HOME_RUN_ACTION_TAP:                       \
                tap_code16(tap_key);                        \
                break;                                      \
            case HOME_RUN_ACTION_HOLD:                      \
                register_mods(MOD_BIT(mod));                \
                break;                                      \
            case HOME_RUN_ACTION_RELEASE:                   \
                unregister_mods(MOD_BIT(mod));              \
                break;                                      \
        }                                                   \
        break;                                              \
    }

// Helper macro to define a home-run one-shot layer key
// Usage: HOME_RUN_OSL(keycode, tap_key, layer)
// When tapped: sends tap_key normally
// When held: activates one-shot layer (stays active for one key, or until released if held)
#define HOME_RUN_OSL(hr_keycode, tap_key, layer)           \
    case hr_keycode: {                                      \
        switch (action) {                                   \
            case HOME_RUN_ACTION_TAP:                       \
                tap_code16(tap_key);                        \
                break;                                      \
            case HOME_RUN_ACTION_HOLD:                      \
                set_oneshot_layer(layer, ONESHOT_START);    \
                break;                                      \
            case HOME_RUN_ACTION_RELEASE:                   \
                reset_oneshot_layer();                      \
                break;                                      \
        }                                                   \
        break;                                              \
    }

// Helper macro to define a home-run momentary layer key
// Usage: HOME_RUN_ML(keycode, tap_key, layer)
// When tapped: sends tap_key normally
// When held: activates layer (deactivates immediately on release)
#define HOME_RUN_ML(hr_keycode, tap_key, layer)            \
    case hr_keycode: {                                      \
        switch (action) {                                   \
            case HOME_RUN_ACTION_TAP:                       \
                tap_code16(tap_key);                        \
                break;                                      \
            case HOME_RUN_ACTION_HOLD:                      \
                layer_on(layer);                            \
                break;                                      \
            case HOME_RUN_ACTION_RELEASE:                   \
                layer_off(layer);                           \
                break;                                      \
        }                                                   \
        break;                                              \
    }

/* ************************************* *
 *    OPPOSITE-HAND DETECTION MACROS     *
 * ************************************* */

// These macros are identical to their non-OPPOSITE counterparts in functionality.
// Use them to document that a key should require opposite-hand activation.
// You must implement home_run_requires_opposite_hand() to return true for these keycodes.

// Opposite-hand mod-tap: only triggers as modifier for opposite-hand keys
// Usage: HOME_RUN_OPPOSITE_MT(keycode, tap_key, mod)
#define HOME_RUN_OPPOSITE_MT(hr_keycode, tap_key, mod) HOME_RUN_MT(hr_keycode, tap_key, mod)

// Opposite-hand one-shot layer: only triggers as layer for opposite-hand keys
// Usage: HOME_RUN_OPPOSITE_OSL(keycode, tap_key, layer)
#define HOME_RUN_OPPOSITE_OSL(hr_keycode, tap_key, layer) HOME_RUN_OSL(hr_keycode, tap_key, layer)

// Opposite-hand momentary layer: only triggers as layer for opposite-hand keys
// Usage: HOME_RUN_OPPOSITE_ML(keycode, tap_key, layer)
#define HOME_RUN_OPPOSITE_ML(hr_keycode, tap_key, layer) HOME_RUN_ML(hr_keycode, tap_key, layer)

// Opposite-hand menu layer: like OPPOSITE_ML but with menu selection behavior
// When held, allows selecting items from a menu - pressing a new menu item
// backspaces the previous selection and emits the new one
// Usage: HOME_RUN_OPPOSITE_MENU_ML(keycode, tap_key, layer)
#define HOME_RUN_OPPOSITE_MENU_ML(hr_keycode, tap_key, layer)  \
    case hr_keycode: {                                          \
        switch (action) {                                       \
            case HOME_RUN_ACTION_TAP:                           \
                tap_code16(tap_key);                            \
                break;                                          \
            case HOME_RUN_ACTION_HOLD:                          \
                layer_on(layer);                                \
                break;                                          \
            case HOME_RUN_ACTION_RELEASE:                       \
                layer_off(layer);                               \
                clear_menu_state(hr_keycode);                   \
                break;                                          \
        }                                                       \
        break;                                                  \
    }

/* ************************************* *
 *          USAGE INSTRUCTIONS           *
 * ************************************* */

/*
 * To use this library:
 *
 * 1. Define your home-run keycode range in your keymap.c:
 *    #define HOME_RUN_KEYCODES_BEGIN CKC_A
 *    #define HOME_RUN_KEYCODES_END CKC_SCLN
 *
 * 2. Add process_home_run() to your process_record_user():
 *    bool process_record_user(uint16_t keycode, keyrecord_t *record) {
 *        if (!process_home_run(keycode, record)) return false;
 *        // ... rest of your code
 *    }
 *
 * 3. Implement on_home_run_action() to define behavior:
 *    void on_home_run_action(uint16_t keycode, home_run_action_t action) {
 *        switch (keycode) {
 *            HOME_RUN_MT(CKC_A, KC_A, KC_LGUI)
 *            HOME_RUN_MT(CKC_S, KC_S, KC_LALT)
 *            HOME_RUN_MT(CKC_D, KC_D, KC_LCTL)
 *            HOME_RUN_MT(CKC_F, KC_F, KC_LSFT)
 *        }
 *    }
 *
 * 4. Use the home-run keycodes in your keymap
 */
