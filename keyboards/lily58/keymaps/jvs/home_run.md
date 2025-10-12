# Home Run Modifiers

A simplified home row modifier library for QMK firmware.

## Overview

Home Run Modifiers provides a clean, timing-based approach to home row modifiers. When you press a home row key, the library buffers events until it can determine whether you meant to tap the key normally or hold it as a modifier.

## How It Works

The library uses two time limits to detect modifier intent:

- **FULL_OVERLAP_LIMIT** (default: 150ms)
- **PARTIAL_OVERLAP_LIMIT** (default: 250ms)

When you press a home run key, one of three things happens:

### Case 1: Quick Tap
Key is released before FULL_OVERLAP_LIMIT → **Normal key press**

The key acts as a regular keypress. The buffered events are flushed with the key acting normally.

### Case 2: Long Hold
Key is held longer than PARTIAL_OVERLAP_LIMIT → **Modifier**

Clear modifier intent from the long hold. The key acts as a modifier for all subsequent keypresses until released.

### Case 3: Full Overlap
Another key is fully pressed and released while the home run key is held, and the elapsed time exceeds FULL_OVERLAP_LIMIT → **Modifier**

This detects modifier behavior faster than waiting for PARTIAL_OVERLAP_LIMIT. If you press A, then fully press+release B while A is still down, and it's been longer than FULL_OVERLAP_LIMIT, that's clearly modifier behavior.

### State Transition

Once the library determines the key's role (modifier or normal), it:
1. Flushes the buffered events with the correct interpretation
2. Stops buffering - the key acts in its determined role until released
3. Can be released independently of other held keys

## Installation

1. Copy `home_run.h` to your keymap directory

2. Define your home run keycode range in your `keymap.c`:

```c
enum custom_keycodes {
    CKC_A = SAFE_RANGE,
    CKC_S,
    CKC_D,
    CKC_F,
    CKC_J,
    CKC_K,
    CKC_L,
    CKC_SCLN,
};

#define HOME_RUN_KEYCODES_BEGIN CKC_A
#define HOME_RUN_KEYCODES_END CKC_SCLN
```

3. Include the library and call it from `process_record_user()`:

```c
#include "home_run.h"

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_home_run(keycode, record)) return false;

    // ... rest of your code
    return true;
}
```

4. Implement the action callback to define each key's behavior:

```c
void on_home_run_action(uint16_t keycode, home_run_action_t action) {
    switch (keycode) {
        HOME_RUN_MT(CKC_A, KC_A, KC_LGUI)
        HOME_RUN_MT(CKC_S, KC_S, KC_LALT)
        HOME_RUN_MT(CKC_D, KC_D, KC_LCTL)
        HOME_RUN_MT(CKC_F, KC_F, KC_LSFT)
        HOME_RUN_MT(CKC_J, KC_J, KC_RSFT)
        HOME_RUN_MT(CKC_K, KC_K, KC_RCTL)
        HOME_RUN_MT(CKC_L, KC_L, KC_RALT)
        HOME_RUN_MT(CKC_SCLN, KC_SCLN, KC_RGUI)
    }
}
```

5. Use the home run keycodes in your keymap:

```c
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_TAB,  KC_Q, KC_W, KC_E, KC_R, KC_T,     KC_Y, KC_U, KC_I,    KC_O,   KC_P,    KC_BSPC,
        KC_ESC,  CKC_A, CKC_S, CKC_D, CKC_F, KC_G,  KC_H, CKC_J, CKC_K, CKC_L, CKC_SCLN, KC_QUOT,
        // ...
    ),
};
```

## Configuration

You can customize the timing by defining these before including `home_run.h`:

```c
#define HOME_RUN_FULL_OVERLAP_LIMIT 150   // ms for full overlap detection
#define HOME_RUN_PARTIAL_OVERLAP_LIMIT 250 // ms for long hold detection
#define HOME_RUN_MAX_ACTIVE 10             // max simultaneous home run keys
#define HOME_RUN_BUFFER_SIZE 20            // max buffered events per key
```

## Split Keyboard Support

Home Run Modifiers works correctly with split keyboards. The timing functions (`timer_read()` and `timer_elapsed()`) are designed to work across split halves, and event replay uses QMK's standard `process_record()` mechanism.

## Advanced Usage

For custom behavior, you can implement the action callback manually instead of using the `HOME_RUN_MT` macro:

```c
void on_home_run_action(uint16_t keycode, home_run_action_t action) {
    switch (keycode) {
        case CKC_A:
            switch (action) {
                case HOME_RUN_ACTION_TAP:
                    tap_code16(KC_A);
                    break;
                case HOME_RUN_ACTION_HOLD:
                    register_mods(MOD_BIT(KC_LGUI));
                    break;
                case HOME_RUN_ACTION_RELEASE:
                    unregister_mods(MOD_BIT(KC_LGUI));
                    break;
            }
            break;
        // ... more keys
    }
}
```

This gives you full control over tap behavior, hold behavior, and release behavior for each key.

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
