#include QMK_KEYBOARD_H

enum layer_number {
    _MAIN = 0,
    _LOWER,
    _RAISE,
    _NUMBER,
    _VIM,
};

enum custom_keycodes {
    SMTD_KEYCODES_BEGIN = SAFE_RANGE,
    CKC_D,   // D with LCtrl hold
    CKC_F,   // F with LAlt hold
    CKC_M,   // M with RAlt hold
    CKC_COMM, // , with RCtrl hold
    SMTD_KEYCODES_END,
    
    // Leader sequences
    LD_AM = SMTD_KEYCODES_END,
    LD_AT,
    LD_BS,
    LD_BT,
    LD_CA,
    LD_CL,
    LD_CM,
    LD_DA,
    LD_DO,
    LD_DQ,
    LD_DT,
    LD_EQ,
    LD_EX,
    LD_FO,
    LD_FS,
    LD_GT,
    LD_HA,
    LD_LA,
    LD_LB,
    LD_LC,
    LD_LP,
    LD_LS,
    LD_LT,
    LD_PC,
    LD_PE,
    LD_PI,
    LD_PL,
    LD_PP,
    LD_PR,
    LD_QM,
    LD_QU,
    LD_RA,
    LD_RB,
    LD_RC,
    LD_RP,
    LD_RS,
    LD_SC,
    LD_SE,
    LD_SQ,
    LD_ST,
    LD_TI,
    LD_TL,
    LD_UN,
    LD_US,
    LD_Z,
    
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

// Combos
enum combo_events {
    JK_ESC,
    DF_VIM,
};

const uint16_t PROGMEM jk_combo[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM df_combo[] = {CKC_D, CKC_F, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
    [JK_ESC] = COMBO(jk_combo, KC_ESC),
    [DF_VIM] = COMBO(df_combo, TO(_VIM)),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* MAIN
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | ESC  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  | Bksp |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |LCtrl |   A  |   S  | D/LC | F/LA |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |  '   |
 * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
 * | LAlt |LShift|   Z  |   X  |   C  |   V  |-------|    |-------|   B  |   N  | M/RA |,/RCtl|   .  |  /   |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LGUI |Lower |LShift| /Leader /       \Enter \  |Space |Raise |Number|
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */

[_MAIN] = LAYOUT(
  KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS,
  KC_LCTL,  KC_A,   KC_S,    CKC_D,   CKC_F,   KC_G,                     KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  KC_LALT,  KC_LSFT,KC_Z,    KC_X,    KC_C,    KC_V, KC_LBRC,  KC_RBRC,  KC_B,    KC_N,    CKC_M,   CKC_COMM,KC_DOT,  KC_SLSH,
                        KC_LGUI, MO(_LOWER), KC_LSFT, QK_LEAD, KC_ENT, KC_SPC, MO(_RAISE), MO(_NUMBER)
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
  XXXXXXX, XXXXXXX, XXXXXXX, C(S(KC_TAB)), C(KC_TAB), XXXXXXX,               OSM(MOD_LALT), OSM(MOD_LCTL), OSM(MOD_RSFT), OSM(MOD_RGUI), XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, C(KC_Z), C(KC_X), C(KC_C), C(KC_V), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                             XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, _______, XXXXXXX
),

/* NUMBER
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |                    |  F7  |  F8  |  F9  | F10  | F11  | F12  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |   1  |   2  |   3  |   0  |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |   4  |   5  |   6  |   0  |-------.    ,-------|OSM_AL|OSM_CT|OSM_SH|OSM_GU|      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |   7  |   8  |   9  |   0  |-------|    |-------|      |      |      |      |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |   0  |   .  | / Bksp  /       \      \  |      |TO(0) |Number|
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_NUMBER] = LAYOUT(
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  XXXXXXX, XXXXXXX, KC_1,    KC_2,    KC_3,    KC_0,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, KC_4,    KC_5,    KC_6,    KC_0,                      OSM(MOD_LALT), OSM(MOD_LCTL), OSM(MOD_RSFT), OSM(MOD_RGUI), XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, KC_7,    KC_8,    KC_9,    KC_0, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
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

// Leader key sequences
void leader_start_user(void) {
    // Leader key started
}

void leader_end_user(void) {
    // Check for window cycling sequences first
    if (leader_sequence_two_keys(KC_J, KC_J)) {
        // Cycle windows in reverse (Shift+Alt+Tab or Shift+Cmd+Tab)
        register_code(KC_LSFT);
        register_code(KC_LALT);
        register_code(KC_TAB);
        unregister_code(KC_TAB);
        unregister_code(KC_LALT);
        unregister_code(KC_LSFT);
    } else if (leader_sequence_two_keys(KC_K, KC_K)) {
        // Cycle windows forward (Alt+Tab or Cmd+Tab)
        register_code(KC_LALT);
        register_code(KC_TAB);
        unregister_code(KC_TAB);
        unregister_code(KC_LALT);
    }
    
    // Symbol sequences
    if (leader_sequence_two_keys(KC_A, KC_M)) {
        SEND_STRING("&");
    } else if (leader_sequence_two_keys(KC_A, KC_T)) {
        SEND_STRING("@");
    } else if (leader_sequence_two_keys(KC_B, KC_S)) {
        SEND_STRING("\\");
    } else if (leader_sequence_two_keys(KC_B, KC_T)) {
        SEND_STRING("`");
    } else if (leader_sequence_two_keys(KC_C, KC_A)) {
        SEND_STRING("^");
    } else if (leader_sequence_two_keys(KC_C, KC_L)) {
        SEND_STRING(":");
    } else if (leader_sequence_two_keys(KC_C, KC_M)) {
        SEND_STRING(",");
    } else if (leader_sequence_two_keys(KC_D, KC_A)) {
        SEND_STRING("-");
    } else if (leader_sequence_two_keys(KC_D, KC_O)) {
        SEND_STRING("$");
    } else if (leader_sequence_two_keys(KC_D, KC_Q)) {
        SEND_STRING("\"");
    } else if (leader_sequence_two_keys(KC_D, KC_T)) {
        SEND_STRING(".");
    } else if (leader_sequence_two_keys(KC_E, KC_Q)) {
        SEND_STRING("=");
    } else if (leader_sequence_two_keys(KC_E, KC_X)) {
        SEND_STRING("!");
    } else if (leader_sequence_two_keys(KC_F, KC_O)) {
        SEND_STRING("/");
    } else if (leader_sequence_two_keys(KC_F, KC_S)) {
        SEND_STRING("/");
    } else if (leader_sequence_two_keys(KC_G, KC_T)) {
        SEND_STRING(">");
    } else if (leader_sequence_two_keys(KC_H, KC_A)) {
        SEND_STRING("#");
    } else if (leader_sequence_two_keys(KC_L, KC_A)) {
        SEND_STRING("<");
    } else if (leader_sequence_two_keys(KC_L, KC_B)) {
        SEND_STRING("[");
    } else if (leader_sequence_two_keys(KC_L, KC_C)) {
        SEND_STRING("{");
    } else if (leader_sequence_two_keys(KC_L, KC_P)) {
        SEND_STRING("(");
    } else if (leader_sequence_two_keys(KC_L, KC_S)) {
        SEND_STRING("[");
    } else if (leader_sequence_two_keys(KC_L, KC_T)) {
        SEND_STRING("<");
    } else if (leader_sequence_two_keys(KC_P, KC_C)) {
        SEND_STRING("%");
    } else if (leader_sequence_two_keys(KC_P, KC_E)) {
        SEND_STRING("%");
    } else if (leader_sequence_two_keys(KC_P, KC_I)) {
        SEND_STRING("|");
    } else if (leader_sequence_two_keys(KC_P, KC_L)) {
        SEND_STRING("+");
    } else if (leader_sequence_two_keys(KC_P, KC_P)) {
        SEND_STRING("|");
    } else if (leader_sequence_two_keys(KC_P, KC_R)) {
        SEND_STRING("%");
    } else if (leader_sequence_two_keys(KC_Q, KC_M)) {
        SEND_STRING("?");
    } else if (leader_sequence_two_keys(KC_Q, KC_U)) {
        SEND_STRING("?");
    } else if (leader_sequence_two_keys(KC_R, KC_A)) {
        SEND_STRING(">");
    } else if (leader_sequence_two_keys(KC_R, KC_B)) {
        SEND_STRING("]");
    } else if (leader_sequence_two_keys(KC_R, KC_C)) {
        SEND_STRING("}");
    } else if (leader_sequence_two_keys(KC_R, KC_P)) {
        SEND_STRING(")");
    } else if (leader_sequence_two_keys(KC_R, KC_S)) {
        SEND_STRING("]");
    } else if (leader_sequence_two_keys(KC_S, KC_C)) {
        SEND_STRING(";");
    } else if (leader_sequence_two_keys(KC_S, KC_E)) {
        SEND_STRING(";");
    } else if (leader_sequence_two_keys(KC_S, KC_Q)) {
        SEND_STRING("'");
    } else if (leader_sequence_two_keys(KC_S, KC_T)) {
        SEND_STRING("*");
    } else if (leader_sequence_two_keys(KC_T, KC_I)) {
        SEND_STRING("~");
    } else if (leader_sequence_two_keys(KC_T, KC_L)) {
        SEND_STRING("~");
    } else if (leader_sequence_two_keys(KC_U, KC_N)) {
        SEND_STRING("_");
    } else if (leader_sequence_two_keys(KC_U, KC_S)) {
        SEND_STRING("_");
    } else if (leader_sequence_one_key(KC_Z)) {
        SEND_STRING("0");
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // Reset vim mode when leaving vim layer
    if (!layer_state_cmp(state, _VIM) && current_vim_mode == VIM_VISUAL) {
        current_vim_mode = VIM_NORMAL;
        unregister_code(KC_LSFT);
    }
    return update_tri_layer_state(state, _LOWER, _RAISE, _NUMBER);
}

//SSD1306 OLED update loop, make sure to enable OLED_ENABLE=yes in rules.mk
#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    // Rotate 90 degrees clockwise for tall/skinny orientation
    return OLED_ROTATION_90;
}

// Large letter bitmaps for 90-degree rotated display
// Each letter is designed for vertical display
static const char PROGMEM large_M[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFE, 0xFE, 0x0E, 0x1C, 0x38, 0x70, 0x38,
    0x1C, 0x0E, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const char PROGMEM large_L[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFE, 0xFE, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const char PROGMEM large_R[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFE, 0xFE, 0x06, 0x06, 0x06, 0x06, 0x0E,
    0x1C, 0x38, 0x70, 0xE0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFF, 0xFF, 0x06, 0x06, 0x06, 0x06, 0x0E,
    0x1C, 0x38, 0x70, 0xE1, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const char PROGMEM large_N[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFE, 0xFE, 0x06, 0x0C, 0x18, 0x30, 0x60,
    0xC0, 0x80, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x03, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Vertical text functions
void render_large_letter(const char* letter_bitmap) {
    oled_write_raw_P(letter_bitmap, 64);
}

void render_vertical_text(const char* text) {
    oled_clear();
    for (int i = 0; text[i] != '\0'; i++) {
        oled_set_cursor(0, i * 2);
        oled_write_char(text[i], false);
    }
}

void render_left_display(void) {
    uint8_t current_layer = get_highest_layer(layer_state);
    
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
            render_vertical_text("VIM");
            break;
        default:
            render_large_letter(large_M);
            break;
    }
}

void render_right_display(void) {
    uint8_t current_layer = get_highest_layer(layer_state);
    
    if (current_layer == _VIM) {
        // Show vim mode
        switch (current_vim_mode) {
            case VIM_NORMAL:
                render_vertical_text("N");
                break;
            case VIM_VISUAL:
                render_vertical_text("V");
                break;
        }
    } else {
        // Show JVS
        render_vertical_text("JVS");
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
#endif // OLED_ENABLE