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

    // Track other key presses for overlap detection
    bool                       other_key_pressed;  // Another key was pressed
    uint16_t                   other_keycode;      // The other key's keycode
    keypos_t                   other_key;          // The other key's position
    bool                       other_key_released; // The other key was released

    // Event buffer
    home_run_buffered_event_t  buffer[HOME_RUN_BUFFER_SIZE];
    uint8_t                    buffer_count;

    // Flag to prevent recursive processing
    bool                       replaying;
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

/* ************************************* *
 *         INTERNAL STATE                *
 * ************************************* */

static home_run_tracked_key_t home_run_tracked[HOME_RUN_MAX_ACTIVE];

/* ************************************* *
 *         HELPER FUNCTIONS              *
 * ************************************* */

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

// Replay buffered events
static void home_run_buffer_flush(home_run_tracked_key_t* tracked) {
    tracked->replaying = true;

    for (uint8_t i = 0; i < tracked->buffer_count; i++) {
        home_run_buffered_event_t* event = &tracked->buffer[i];

        keyevent_t ke = MAKE_KEYEVENT(event->key.row, event->key.col, event->pressed);
        keyrecord_t record = {.event = ke};

        process_record(&record);
    }

    tracked->buffer_count = 0;
    tracked->replaying = false;
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
    if (tracked->state == HOME_RUN_STATE_MODIFIER) {
        // Call user's hold action before flushing buffer
        on_home_run_action(tracked->keycode, HOME_RUN_ACTION_HOLD);
    }

    // Flush the buffer
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
    tracked->replaying = false;
}

/* ************************************* *
 *      ENTRY POINT IMPLEMENTATION       *
 * ************************************* */

bool process_home_run(uint16_t keycode, keyrecord_t* record) {
    // Don't process if we're replaying buffered events
    for (uint8_t i = 0; i < HOME_RUN_MAX_ACTIVE; i++) {
        if (home_run_tracked[i].active && home_run_tracked[i].replaying) {
            return true;
        }
    }

    // Check if this is a tracked home-run key
    home_run_tracked_key_t* tracked = home_run_find_tracked(keycode);

    if (tracked) {
        // This is a home-run key we're tracking
        if (!record->event.pressed) {
            // Home-run key released

            if (tracked->state == HOME_RUN_STATE_UNKNOWN) {
                // Case 1: Released before limits - it's a normal key
                tracked->state = HOME_RUN_STATE_NORMAL;

                // Flush buffer (normal tap)
                home_run_buffer_flush(tracked);

                // Call user's tap action
                on_home_run_action(tracked->keycode, HOME_RUN_ACTION_TAP);

                // Clean up
                home_run_clear_tracked(tracked);

            } else if (tracked->state == HOME_RUN_STATE_MODIFIER) {
                // Release the modifier
                on_home_run_action(tracked->keycode, HOME_RUN_ACTION_RELEASE);

                // Clean up
                home_run_clear_tracked(tracked);
            }
        }

        return false; // Handled
    }

    // Check if any home-run key is being tracked (we may need to buffer this event)
    bool any_tracking = false;
    home_run_tracked_key_t* tracking_unknown = NULL;

    for (uint8_t i = 0; i < HOME_RUN_MAX_ACTIVE; i++) {
        if (home_run_tracked[i].active) {
            any_tracking = true;
            if (home_run_tracked[i].state == HOME_RUN_STATE_UNKNOWN) {
                tracking_unknown = &home_run_tracked[i];
                break;
            }
        }
    }

    if (tracking_unknown) {
        // We're tracking a home-run key in unknown state
        // Buffer this event
        home_run_buffer_add(tracking_unknown, keycode, record->event.pressed, record->event.key);

        // Track other key presses for overlap detection
        if (record->event.pressed) {
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
        }

        return false; // Don't process this event yet (it's buffered)
    }

    // Check if this is a press of a new home-run key (user must define the range)
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
            slot->replaying = false;
        }

        return false; // Handled
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
