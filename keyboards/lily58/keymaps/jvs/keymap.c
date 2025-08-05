#include QMK_KEYBOARD_H

enum layer_number {
    _MAIN = 0,
    _LOWER,
    _RAISE,
    _NUMBER,
    _VIM,
    _SYMBOL,
};

enum custom_keycodes {
    SMTD_KEYCODES_BEGIN = SAFE_RANGE,
    CKC_D,   // D with LCtrl hold
    CKC_F,   // F with LAlt hold
    CKC_M,   // M with RAlt hold
    CKC_COMM, // , with RCtrl hold
    SMTD_KEYCODES_END,


    // Vim mode keys
    VIM_H,
    VIM_J,
    VIM_K,
    VIM_L,
    VIM_X,
    VIM_U,
    VIM_DD,
    VIM_0,
    VIM_DLR,
    VIM_B,
    VIM_V,
};

#include "sm_td.h"

// Vim mode tracking
typedef enum {
    VIM_NORMAL,
    VIM_VISUAL,
} vim_mode_t;

static vim_mode_t current_vim_mode = VIM_NORMAL;


#define KEYMAP_VERSION 8

// Combos
enum combo_events {
    JK_ESC,
    DF_VIM,
    VIM_JK_ESC,
};

const uint16_t PROGMEM jk_combo[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM df_combo[] = {CKC_D, CKC_F, COMBO_END};
const uint16_t PROGMEM vim_jk_combo[] = {VIM_J, VIM_K, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
    [JK_ESC] = COMBO(jk_combo, KC_ESC),
    [DF_VIM] = COMBO(df_combo, TO(_VIM)),
    [VIM_JK_ESC] = COMBO(vim_jk_combo, TO(_MAIN)),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* MAIN
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |  `~  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |  \|  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |LCtrl |   A  |   S  | D/LC | F/LA |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |  '   |
 * |------+------+------+------+------+------|   B   |    |    B  |------+------+------+------+------+------|
 * | LGUI |LShift|   Z  |   X  |   C  |   V  |-------|    |-------|   N  | M/RA |,/RCtl|   .  |   /  |RShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LGUI |Lower |LShift| /Leader /       \Enter \  |Space |Raise |Number|
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */

[_MAIN] = LAYOUT(
  KC_GRV,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
  KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
  KC_LCTL,  KC_A,   KC_S,    CKC_D,   CKC_F,   KC_G,                      KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  KC_LGUI,  KC_LSFT,KC_Z,    KC_X,    KC_C,    KC_V, KC_B,         KC_B,  KC_N,    CKC_M,   CKC_COMM,KC_DOT,  KC_SLSH, KC_RSFT,
                        MO(_SYMBOL), MO(_LOWER), KC_LSFT, KC_BSPC, KC_ENT, KC_SPC, MO(_RAISE), MO(_NUMBER)
),

/* LOWER
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |   {  |   }  |      |      | Del  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |                    |      |   (  |   )  |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |OSM_GUI|OSM_SH|OSM_CT|OSM_AL|      |-------.    ,-------|Left  | Down |  Up  |Right |      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      |   [  |   ]  |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |Lower |      | /       /       \      \  |      |      |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_LOWER] = LAYOUT(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, KC_LCBR, KC_RCBR, XXXXXXX, XXXXXXX, KC_DEL,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, KC_LPRN, KC_RPRN, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, OSM(MOD_LGUI), OSM(MOD_LSFT), OSM(MOD_LCTL), OSM(MOD_LALT), XXXXXXX,  KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_LBRC, KC_RBRC, XXXXXXX, XXXXXXX,
                             XXXXXXX, _______, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX
),

/* RAISE
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      | PgUp | PgDn |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      | Home | End  |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |C+S+Tab|C+Tab|      |-------.    ,-------|OSM_AL|OSM_CT|OSM_SH|OSM_GU|      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      | Undo | Cut  | Copy |Paste |-------|    |-------|      |      |      |      |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /       /       \      \  |      |Raise |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */

[_RAISE] = LAYOUT(
  XXXXXXX, XXXXXXX, XXXXXXX, KC_PGUP, KC_PGDN, XXXXXXX,                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, KC_HOME, KC_END,  XXXXXXX,                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, G(S(KC_TAB)), G(KC_TAB), XXXXXXX,               OSM(MOD_LALT), OSM(MOD_LCTL), OSM(MOD_RSFT), OSM(MOD_RGUI), XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, G(KC_Z), G(KC_X), G(KC_C), G(KC_V), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                             XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, _______, XXXXXXX
),

/* NUMBER
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |                    |  F7  |  F8  |  F9  | F10  | F11  | F12  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |   3  |   2  |   1  |   0  |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |   6  |   5  |   4  |   0  |-------.    ,-------|OSM_AL|OSM_CT|OSM_SH|OSM_GU|      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |   9  |   8  |   7  |   0  |-------|    |-------|      |      |      |      |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |   0  |   .  | / Bksp  /       \      \  |      |TO(0) |Number|
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_NUMBER] = LAYOUT(
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  XXXXXXX, XXXXXXX, KC_3,    KC_2,    KC_1,    KC_0,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, KC_6,    KC_5,    KC_4,    KC_0,                      OSM(MOD_LALT), OSM(MOD_LCTL), OSM(MOD_RSFT), OSM(MOD_RGUI), XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, KC_9,    KC_8,    KC_7,    KC_0, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                             XXXXXXX, KC_0, KC_DOT, KC_BSPC, XXXXXXX,  XXXXXXX, TO(_MAIN), _______
),

/* VIM
 * Vim-like navigation and commands
 * ESC returns to main layer, v enables shift mode
 */
[_VIM] = LAYOUT(
  TO(_MAIN), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, VIM_0,   XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     XXXXXXX, VIM_U,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, VIM_DD,  XXXXXXX, XXXXXXX,                     VIM_H,   VIM_J,   VIM_K,   VIM_L,   XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, VIM_X,   XXXXXXX, VIM_V, XXXXXXX, XXXXXXX, VIM_B,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, VIM_DLR,
                             XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX
),

/* SYMBOL
 * Symbol layer - replaces leader key sequences
 * Based on original leader mappings: am=&, at=@, bs=\, bt=`, etc.
 */
[_SYMBOL] = LAYOUT(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_0,    XXXXXXX,
  XXXXXXX, KC_QUES, XXXXXXX, KC_EQL,  XXXXXXX, KC_TILD,                     XXXXXXX, KC_UNDS, KC_PIPE, XXXXXXX, KC_PLUS, XXXXXXX,
  KC_AMPR, KC_AT,   KC_BSLS, KC_DLR,  KC_SLSH, KC_GRV,                     KC_HASH, KC_LPRN, KC_RPRN, KC_LT,   KC_COLN, KC_QUOT,
  XXXXXXX, XXXXXXX, KC_0,    KC_EXLM, KC_CIRC, KC_PERC, KC_LBRC, KC_RBRC, KC_MINS, KC_ASTR, KC_COMM, KC_DOT,  XXXXXXX, XXXXXXX,
                             XXXXXXX, KC_LCBR, KC_RCBR, _______, KC_DQUO,  KC_GT,   KC_SCLN, XXXXXXX
),
};

// SM Tap Dance configuration
void on_smtd_action(uint16_t keycode, smtd_action action, uint8_t tap_count) {
    switch (keycode) {
        SMTD_MT(CKC_D, KC_D, KC_LCTL)
        SMTD_MT(CKC_F, KC_F, KC_LALT)
        SMTD_MT(CKC_M, KC_M, KC_RALT)
        SMTD_MT(CKC_COMM, KC_COMM, KC_RCTL)
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_smtd(keycode, record)) {
        return false;
    }

    if (record->event.pressed) {
        switch (keycode) {
            // Vim mode keys
            case VIM_H:
                register_code(KC_LEFT);
                break;
            case VIM_J:
                register_code(KC_DOWN);
                break;
            case VIM_K:
                register_code(KC_UP);
                break;
            case VIM_L:
                register_code(KC_RGHT);
                break;
            case VIM_X:
                register_code(KC_DEL);
                break;
            case VIM_U:
                register_code(KC_LCTL);
                register_code(KC_Z);
                break;
            case VIM_0:
                register_code(KC_HOME);
                break;
            case VIM_DLR:
                register_code(KC_END);
                break;
            case VIM_B:
                register_code(KC_LCTL);
                register_code(KC_LEFT);
                break;
            case VIM_V:
                if (current_vim_mode == VIM_NORMAL) {
                    current_vim_mode = VIM_VISUAL;
                    register_code(KC_LSFT);
                } else {
                    current_vim_mode = VIM_NORMAL;
                    unregister_code(KC_LSFT);
                }
                break;
            case VIM_DD:
                // Delete line: Home, Shift+End, Delete
                register_code(KC_HOME);
                unregister_code(KC_HOME);
                register_code(KC_LSFT);
                register_code(KC_END);
                unregister_code(KC_END);
                unregister_code(KC_LSFT);
                register_code(KC_DEL);
                break;
        }
    } else {
        switch (keycode) {
            case VIM_H:
                unregister_code(KC_LEFT);
                break;
            case VIM_J:
                unregister_code(KC_DOWN);
                break;
            case VIM_K:
                unregister_code(KC_UP);
                break;
            case VIM_L:
                unregister_code(KC_RGHT);
                break;
            case VIM_X:
                unregister_code(KC_DEL);
                break;
            case VIM_U:
                unregister_code(KC_Z);
                unregister_code(KC_LCTL);
                break;
            case VIM_0:
                unregister_code(KC_HOME);
                break;
            case VIM_DLR:
                unregister_code(KC_END);
                break;
            case VIM_B:
                unregister_code(KC_LEFT);
                unregister_code(KC_LCTL);
                break;
            case VIM_V:
                // Mode change handled in press
                break;
            case VIM_DD:
                unregister_code(KC_DEL);
                break;
        }
    }
    return true;
}


layer_state_t layer_state_set_user(layer_state_t state) {
    // Reset vim mode when leaving vim layer
    if (!layer_state_cmp(state, _VIM) && current_vim_mode == VIM_VISUAL) {
        current_vim_mode = VIM_NORMAL;
        unregister_code(KC_LSFT);
    }
    // Remove tri-layer to allow direct NUMBER layer access
    return state;
}

//SSD1306 OLED update loop, make sure to enable OLED_ENABLE=yes in rules.mk
#ifdef OLED_ENABLE

// Display tracking variables
static uint8_t last_left_layer = 255;
static uint8_t last_right_layer = 255;
static vim_mode_t last_vim_mode = VIM_NORMAL;

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    // Rotate 270 degrees (or -90 degrees) for proper tall/skinny orientation
    return OLED_ROTATION_270;
}

// Large letter bitmaps for 270-degree rotated display
// Each letter is designed for vertical display and made bigger/bolder
static const char PROGMEM large_M[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0x1F, 0x3E, 0x7C, 0xF8, 0x7C,
    0x3E, 0x1F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const char PROGMEM large_L[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const char PROGMEM large_R[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0x0E, 0x0E, 0x0E, 0x1E, 0x3C,
    0x78, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0x0E, 0x0E, 0x0E, 0x1E, 0x3C,
    0x78, 0xF0, 0xE1, 0xC3, 0x87, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const char PROGMEM large_N[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0x0E, 0x1C, 0x38, 0x70, 0xE0,
    0xC0, 0x80, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x03, 0x07, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const char PROGMEM large_S[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xE0, 0xF0, 0x1C, 0x0E, 0x0E, 0x0E, 0x1C,
    0x18, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x03, 0x07, 0x0E, 0x0E, 0x0E, 0x1C,
    0xF8, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Vertical text bitmaps - each shows the full word vertically
static const char PROGMEM vertical_JVS[] = {
    // J at top
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // V in middle
    0x1F, 0x1F, 0x60, 0x80, 0x80, 0x60, 0x1F, 0x1F,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // S at bottom
    0x00, 0x00, 0x06, 0x0F, 0x19, 0x19, 0x19, 0x10
};

static const char PROGMEM vertical_VIM[] = {
    // V at top
    0x07, 0x1F, 0x78, 0xE0, 0xE0, 0x78, 0x1F, 0x07,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // I in middle
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // M at bottom
    0x00, 0xFF, 0xFF, 0x06, 0x0C, 0x06, 0xFF, 0xFF
};

void render_large_letter(const char* letter_bitmap) {
    oled_write_raw_P(letter_bitmap, 64);
}

void render_simple_vertical_text(const char* text) {
    for (int i = 0; text[i] != '\0'; i++) {
        oled_set_cursor(0, i);
        oled_write_char(text[i], false);
    }
}

void render_left_display(void) {
    uint8_t current_layer = get_highest_layer(layer_state);

    // Only redraw if layer changed
    if (current_layer != last_left_layer) {
        last_left_layer = current_layer;
        oled_clear();

        switch (current_layer) {
            case _MAIN:
                render_large_letter(large_M);
                break;
            case _LOWER:
                render_large_letter(large_L);
                break;
            case _RAISE:
                render_large_letter(large_R);
                break;
            case _NUMBER:
                render_large_letter(large_N);
                break;
            case _VIM:
                render_simple_vertical_text("VIM");
                break;
            case _SYMBOL:
                render_large_letter(large_S);
                break;
            default:
                render_large_letter(large_M);
                break;
        }
    }
}

void render_right_display(void) {
    uint8_t current_layer = get_highest_layer(layer_state);

    // Only redraw if layer or vim mode changed
    bool needs_redraw = (current_layer != last_right_layer) ||
                       (current_layer == _VIM && current_vim_mode != last_vim_mode);

    if (needs_redraw) {
        last_right_layer = current_layer;
        last_vim_mode = current_vim_mode;
        oled_clear();

        if (current_layer == _VIM) {
            // Show vim mode
            switch (current_vim_mode) {
                case VIM_NORMAL:
                    render_simple_vertical_text("N");
                    break;
                case VIM_VISUAL:
                    render_simple_vertical_text("V");
                    break;
            }
        } else if (current_layer == _SYMBOL) {
            // Show symbol layer active
            render_simple_vertical_text("S");
        } else {
            // Show version
            oled_write_P(PSTR("v"), false);
            oled_write(get_u8_str(KEYMAP_VERSION, ' '), false);
        }
    }
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        render_left_display();
    } else {
        render_right_display();
    }
    return false;
}

// OLED timeout functions removed - using automatic timeout instead
#endif // OLED_ENABLE
