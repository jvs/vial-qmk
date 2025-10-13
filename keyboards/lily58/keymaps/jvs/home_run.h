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

static home_run_tracked_key_t home_run_tracked[HOME_RUN_MAX_ACTIVE];

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

// Find a tracked key by keycode
static home_run_tracked_key_t* home_run_find_tracked(uint16_t keycode) {
    for (uint8_t i = 0; i < HOME_RUN_MAX_ACTIVE; i++) {
        if (home_run_tracked[i].active && home_run_tracked[i].keycode == keycode) {
            return &home_run_tracked[i];
        }
    }
    return NULL;
}

// Find an inactive slot for tracking a new key
static home_run_tracked_key_t* home_run_get_inactive_slot(void) {
    for (uint8_t i = 0; i < HOME_RUN_MAX_ACTIVE; i++) {
        if (!home_run_tracked[i].active) {
            return &home_run_tracked[i];
        }
    }
    return NULL;
}

// Add an event to the buffer
static void home_run_buffer_add(home_run_tracked_key_t* tracked, uint16_t keycode, bool pressed, keypos_t key) {
    if (tracked->buffer_count < HOME_RUN_BUFFER_SIZE) {
        tracked->buffer[tracked->buffer_count].keycode = keycode;
        tracked->buffer[tracked->buffer_count].pressed = pressed;
        tracked->buffer[tracked->buffer_count].key = key;
        tracked->buffer_count++;
    }
}

// Replay buffered events with proper interpretation of home-run key
static void home_run_buffer_flush(home_run_tracked_key_t* tracked) {
    replaying_buffer = true;

    for (uint8_t i = 0; i < tracked->buffer_count; i++) {
        home_run_buffered_event_t* event = &tracked->buffer[i];

        // Check if this is the home-run key itself
        if (event->keycode == tracked->keycode) {
            // Apply resolved interpretation
            if (tracked->state == HOME_RUN_STATE_NORMAL) {
                // Normal tap: emit the key on press, skip release
                if (event->pressed) {
                    on_home_run_action(tracked->keycode, HOME_RUN_ACTION_TAP);
                }
                // Skip release event (tap_code16 already did press+release)
            } else if (tracked->state == HOME_RUN_STATE_MODIFIER) {
                // Modifier: register on press, unregister on release
                if (event->pressed) {
                    on_home_run_action(tracked->keycode, HOME_RUN_ACTION_HOLD);
                } else {
                    on_home_run_action(tracked->keycode, HOME_RUN_ACTION_RELEASE);
                }
            }
        } else {
            // Regular key: replay the event normally
            keyevent_t ke = MAKE_KEYEVENT(event->key.row, event->key.col, event->pressed);
            keyrecord_t record = {.event = ke};
            process_record(&record);
        }
    }

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

    // Check if this is a new home-run key press
    if (record->event.pressed && keycode >= HOME_RUN_KEYCODES_BEGIN && keycode <= HOME_RUN_KEYCODES_END) {
        // Start tracking this home-run key
        home_run_tracked_key_t* slot = home_run_get_inactive_slot();
        if (slot) {
            slot->active = true;
            slot->keycode = keycode;
            slot->press_time = timer_read();
            slot->state = HOME_RUN_STATE_UNKNOWN;
            slot->buffer_count = 0;
            slot->other_key_pressed = false;
            slot->other_key_released = false;
            slot->home_key = record->event.key;
            slot->requires_opposite_hand = home_run_requires_opposite_hand && home_run_requires_opposite_hand(keycode);

            // Buffer the home-run key press itself
            home_run_buffer_add(slot, keycode, record->event.pressed, record->event.key);
        }

        return false; // Handled, don't process normally
    }

    // Check if this is a tracked home-run key release
    home_run_tracked_key_t* tracked = home_run_find_tracked(keycode);
    if (tracked && !record->event.pressed) {
        if (tracked->state == HOME_RUN_STATE_UNKNOWN) {
            // Released before any decision was made - it's a normal tap
            tracked->state = HOME_RUN_STATE_NORMAL;

            // Buffer the release
            home_run_buffer_add(tracked, keycode, record->event.pressed, record->event.key);

            // Flush buffer with appropriate interpretation
            home_run_finalize_state(tracked);
        } else if (tracked->state == HOME_RUN_STATE_MODIFIER) {
            // Already determined as modifier, just release it
            on_home_run_action(tracked->keycode, HOME_RUN_ACTION_RELEASE);
        }

        // Clean up
        home_run_clear_tracked(tracked);

        return false; // Handled
    }

    // Check if any home-run key is being tracked
    home_run_tracked_key_t* tracking_unknown = NULL;
    home_run_tracked_key_t* tracking_modifier = NULL;

    for (uint8_t i = 0; i < HOME_RUN_MAX_ACTIVE; i++) {
        if (home_run_tracked[i].active) {
            if (home_run_tracked[i].state == HOME_RUN_STATE_UNKNOWN) {
                tracking_unknown = &home_run_tracked[i];
                break;
            } else if (home_run_tracked[i].state == HOME_RUN_STATE_MODIFIER) {
                tracking_modifier = &home_run_tracked[i];
                // Don't break - still check for unknown states which take priority
            }
        }
    }

    if (tracking_unknown) {
        // Buffer this event (state still unknown)
        home_run_buffer_add(tracking_unknown, keycode, record->event.pressed, record->event.key);

        // Track other key presses for overlap detection
        if (record->event.pressed) {
            // Check for same-hand roll
            if (tracking_unknown->requires_opposite_hand && same_hand(tracking_unknown->home_key, record->event.key)) {
                // Immediately resolve as normal key
                tracking_unknown->state = HOME_RUN_STATE_NORMAL;
                home_run_finalize_state(tracking_unknown);
                home_run_clear_tracked(tracking_unknown);
                return false; // Event was buffered and flushed
            }

            if (!tracking_unknown->other_key_pressed) {
                tracking_unknown->other_key_pressed = true;
                tracking_unknown->other_keycode = keycode;
                tracking_unknown->other_key = record->event.key;
            }
        } else {
            // Key released - check if it's the other key we're tracking
            if (tracking_unknown->other_key_pressed &&
                tracking_unknown->other_key.row == record->event.key.row &&
                tracking_unknown->other_key.col == record->event.key.col) {
                tracking_unknown->other_key_released = true;
            }
        }

        // Check if we can determine the state now
        if (home_run_check_state(tracking_unknown)) {
            home_run_finalize_state(tracking_unknown);

            // If it was determined as MODIFIER, keep tracking (don't clear)
            // so we can properly release it later
            if (tracking_unknown->state == HOME_RUN_STATE_NORMAL) {
                home_run_clear_tracked(tracking_unknown);
            }
            // If MODIFIER, tracking continues until key is released
        }

        return false; // Event is buffered, don't process normally
    }

    // If we're tracking a modifier (already determined), let events process normally
    if (tracking_modifier) {
        return true; // Let the event process with modifier active
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
