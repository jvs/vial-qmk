#include QMK_KEYBOARD_H

enum layer_number {
    _MAIN = 0,
    _LOWER,
    _RAISE,
    _MISC,
    _SYMBOL,
    _COMPARE,
    _MATH,
    _LAYER_F,
    _LAYER_D,
};

enum custom_keycodes {
    // Follower key for text expansion
    FOLLOWER_KEY = SAFE_RANGE,

    // Custom window switching
    WIN_SWITCH,

    OS_CYCLE,

    // OS-aware clipboard operations
    OS_UNDO,
    OS_CUT,
    OS_COPY,
    OS_PASTE,

    // Comparison operator keycodes (menu layer with smart spacing)
    CMP_EQ,      // " == "
    CMP_NE,      // " != "
    CMP_LT,      // " < "
    CMP_GT,      // " > "
    CMP_LE,      // " <= "
    CMP_GE,      // " >= "
    CMP_IN,      // " in "
    CMP_NIN,     // " not in "
    CMP_IS,      // " is "
    CMP_ISN,     // " is not "
    CMP_AND,     // " and "
    CMP_OR,      // " or "
    CMP_ASSIGN,  // " = "

    // Math operator keycodes (menu layer)
    MATH_ADD,    // " + "
    MATH_SUB,    // " - "
    MATH_MUL,    // " * "
    MATH_DIV,    // " / "
    MATH_FDIV,   // " // "
    MATH_MOD,    // " % "
    MATH_POW,    // "**" (no spaces)
    MATH_ADDEQ,  // " += "
    MATH_SUBEQ,  // " -= "
    MATH_MULEQ,  // " *= "
    MATH_DIVEQ,  // " /= "

    // home_run keycodes after our custom ones
    HOME_RUN_KEYCODES_BEGIN,
    HR_A,    // A with LShift hold (opposite-hand)
    HR_SCLN, // ; with RShift hold (opposite-hand)
    HR_X,    // X with LCtrl hold (opposite-hand)
    HR_COMM, // , with RCtrl hold (opposite-hand)
    HR_C,    // C with LAlt hold (opposite-hand)
    HR_M,    // M with LAlt hold (opposite-hand)
    HR_SLSH, // / with RGui hold (opposite-hand)
    HR_F,    // F with _LAYER_F menu layer (opposite-hand)
    HR_D,    // D with _LAYER_D menu layer (opposite-hand)
    HOME_RUN_KEYCODES_END,
};

#include "home_run.h"
#include "follower.h"

// Follower expansions - MUST be sorted longest input first for correct matching!
const follower_expansion_t follower_expansions[] = {
    // Length 13
    FOLLOWER_EXPANSION("gap", "git add -p\\n"),

    // Length 9
    FOLLOWER_EXPANSION("semicolon", ";"),

    // Length 8
    FOLLOWER_EXPANSION("lsquare", "["),
    FOLLOWER_EXPANSION("rsquare", "]"),

    // Length 6
    FOLLOWER_EXPANSION("bslash", "\\"),
    FOLLOWER_EXPANSION("fslash", "/"),
    FOLLOWER_EXPANSION("dollar", "$"),
    FOLLOWER_EXPANSION("single", "'"),
    FOLLOWER_EXPANSION("double", "\""),

    // Length 5
    FOLLOWER_EXPANSION("three", "3"),
    FOLLOWER_EXPANSION("seven", "7"),
    FOLLOWER_EXPANSION("eight", "8"),
    FOLLOWER_EXPANSION("colon", ":"),
    FOLLOWER_EXPANSION("comma", ","),
    FOLLOWER_EXPANSION("tilde", "~"),
    FOLLOWER_EXPANSION("minus", "-"),
    FOLLOWER_EXPANSION("under", "_"),
    FOLLOWER_EXPANSION("caret", "^"),
    FOLLOWER_EXPANSION("lcurl", "{"),
    FOLLOWER_EXPANSION("rcurl", "}"),

    // Length 4
    FOLLOWER_EXPANSION("four", "4"),
    FOLLOWER_EXPANSION("five", "5"),
    FOLLOWER_EXPANSION("nine", "9"),
    FOLLOWER_EXPANSION("zero", "0"),
    FOLLOWER_EXPANSION("eqeq", "=="),
    FOLLOWER_EXPANSION("plus", "+"),
    FOLLOWER_EXPANSION("dash", "-"),
    FOLLOWER_EXPANSION("star", "*"),
    FOLLOWER_EXPANSION("mult", "*"),
    FOLLOWER_EXPANSION("pipe", "|"),
    FOLLOWER_EXPANSION("bang", "!"),
    FOLLOWER_EXPANSION("hash", "#"),
    FOLLOWER_EXPANSION("pcnt", "%"),
    FOLLOWER_EXPANSION("tick", "`"),
    FOLLOWER_EXPANSION("semi", ";"),

    // Length 3
    FOLLOWER_EXPANSION("one", "1"),
    FOLLOWER_EXPANSION("two", "2"),
    FOLLOWER_EXPANSION("six", "6"),
    FOLLOWER_EXPANSION("dot", "."),
    FOLLOWER_EXPANSION("ttt", "```"),
    FOLLOWER_EXPANSION("lsq", "["),
    FOLLOWER_EXPANSION("rsq", "]"),
    FOLLOWER_EXPANSION("til", "~"),
    FOLLOWER_EXPANSION("dub", "\""),
    FOLLOWER_EXPANSION("col", ":"),
    FOLLOWER_EXPANSION("com", ","),
    FOLLOWER_EXPANSION("dol", "$"),
    FOLLOWER_EXPANSION("per", "%"),
    FOLLOWER_EXPANSION("amp", "&"),
    FOLLOWER_EXPANSION("pip", "|"),
    FOLLOWER_EXPANSION("sem", ";"),

    // Length 2
    FOLLOWER_EXPANSION("ne", "!="),
    FOLLOWER_EXPANSION("ge", ">="),
    FOLLOWER_EXPANSION("le", "<="),
    FOLLOWER_EXPANSION("ee", "=="),
    FOLLOWER_EXPANSION("pl", "+"),
    FOLLOWER_EXPANSION("mi", "-"),
    FOLLOWER_EXPANSION("mn", "-"),
    FOLLOWER_EXPANSION("da", "-"),
    FOLLOWER_EXPANSION("un", "_"),
    FOLLOWER_EXPANSION("us", "_"),
    FOLLOWER_EXPANSION("ca", "^"),
    FOLLOWER_EXPANSION("am", "&"),
    FOLLOWER_EXPANSION("mu", "*"),
    FOLLOWER_EXPANSION("st", "*"),
    FOLLOWER_EXPANSION("lp", "("),
    FOLLOWER_EXPANSION("rp", ")"),
    FOLLOWER_EXPANSION("pi", "|"),
    FOLLOWER_EXPANSION("bs", "\\"),
    FOLLOWER_EXPANSION("fs", "/"),
    FOLLOWER_EXPANSION("ex", "!"),
    FOLLOWER_EXPANSION("at", "@"),
    FOLLOWER_EXPANSION("hs", "#"),
    FOLLOWER_EXPANSION("ha", "#"),
    FOLLOWER_EXPANSION("do", "$"),
    FOLLOWER_EXPANSION("pc", "%"),
    FOLLOWER_EXPANSION("eq", "="),
    FOLLOWER_EXPANSION("lc", "{"),
    FOLLOWER_EXPANSION("rc", "}"),
    FOLLOWER_EXPANSION("ls", "["),
    FOLLOWER_EXPANSION("rs", "]"),
    FOLLOWER_EXPANSION("sc", ";"),
    FOLLOWER_EXPANSION("sq", "'"),
    FOLLOWER_EXPANSION("dq", "\""),
    FOLLOWER_EXPANSION("lt", "<"),
    FOLLOWER_EXPANSION("gt", ">"),
    FOLLOWER_EXPANSION("bt", "`"),
};
#define NUM_FOLLOWER_EXPANSIONS (sizeof(follower_expansions) / sizeof(follower_expansion_t))

// Shorter aliases for readability
#define XX      KC_NO    // Disabled key
#define __      KC_TRNS  // Transparent (falls through to lower layer)

// Layer keys.
#define L_SYM   MO(_SYMBOL)
#define L_LOW   MO(_LOWER)
#define L_RAS   MO(_RAISE)
#define L_MISC  MO(_MISC)


// One-shot modifiers.
#define OM_LGUI  OSM(MOD_LGUI)
#define OM_LSFT  OSM(MOD_LSFT)
#define OM_LCTL  OSM(MOD_LCTL)
#define OM_LALT  OSM(MOD_LALT)


// Simple OS toggle system
typedef enum {
    OS_MAC = 0,
    OS_WIN,
} os_mode_t;

static os_mode_t current_os = OS_MAC;  // Default to Mac

// Toggle between Mac and Windows
static void toggle_os(void) {
    current_os = (current_os == OS_MAC) ? OS_WIN : OS_MAC;
}

// Combos
enum combo_events {
    UI_BSPC,      // Right hand: U+I for backspace
};

const uint16_t PROGMEM ui_combo[] = {KC_U, KC_I, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
    [UI_BSPC] = COMBO(ui_combo, KC_BSPC),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* MAIN
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | ESC  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |  \|  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |LCtrl | A/LS |   S  | D/LD | F/LF |   G  |-------.    ,-------|   H  |   J  |   K  |   L  | ;/RS |  '   |
 * |------+------+------+------+------+------|   B   |    |    B  |------+------+------+------+------+------|
 * | LGUI | LAlt |   Z  | X/LC | C/LA |   V  |-------|    |-------|   N  | M/LA |,/RC  |   .  | //RG |RShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |Symbol|Lower |Leader| /Follower      \  Tab \  |Space |Raise |Number|
 *                   |      |      |Shift |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */

//        |        |        |        |        |        |                |        |        |        |        |        |        |
[_MAIN] = LAYOUT(
  KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSLS,
  KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS,
  KC_LCTL, HR_A,    KC_S,    HR_D,    HR_F,    KC_G,                     KC_H,    KC_J,    KC_K,    KC_L,    HR_SCLN, KC_QUOT,
  KC_LGUI, KC_LALT, KC_Z,    HR_X,    HR_C,    KC_V,  KC_B,       KC_B,  KC_N,    HR_M,    HR_COMM, KC_DOT,  HR_SLSH, KC_RSFT,
                        L_SYM,    L_LOW,   KC_LSFT,   FOLLOWER_KEY,  KC_TAB,  KC_SPC,  L_RAS,   L_MISC
),

[_LOWER] = LAYOUT(
  XX,      XX,      XX,      XX,      XX,      XX,                       XX,      KC_LCBR, KC_RCBR, XX,      XX,      KC_DEL,
  XX,      XX,      XX,      XX,      XX,      KC_TILD,                  KC_TAB,  KC_PGUP, KC_PGDN, XX,      XX,      XX,
  XX,      OM_LGUI, OM_LSFT, OM_LCTL, OM_LALT, XX,                       KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XX,      XX,
  XX,      XX,      OS_UNDO, OS_CUT,  OS_COPY, OS_PASTE, XX,    KC_CAPS, XX,      KC_HOME, KC_END,  KC_RBRC, XX,      XX,
                        XX,      __,      XX,       __,             XX,  KC_UNDS, XX,      XX
),

[_RAISE] = LAYOUT(
  XX,      XX,      XX,      XX,      XX,      XX,                            XX,      XX,      XX,      XX,      XX,      XX,
  XX,      KC_LBRC, KC_RBRC, KC_LCBR, KC_RCBR, A(KC_T),                       XX,      XX,      XX,      XX,      XX,      XX,
  XX,      KC_PLUS, KC_EQL,  G(S(KC_TAB)), G(KC_TAB), XX,                     OSM(MOD_LALT), OSM(MOD_LCTL), OSM(MOD_RSFT), OSM(MOD_RGUI), XX, XX,
  XX,      XX,      OS_UNDO, OS_CUT,  OS_COPY, OS_PASTE, XX,      XX,        XX,      XX,      XX,      XX,      XX,      XX,
                             XX,      XX,      XX,      __,            XX,      XX,      __,      XX
),

// NUMBER - Function keys and numpad layout with one-shot mods
[_MISC] = LAYOUT(
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                      KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  OS_CYCLE,
  XX,      XX,      KC_3,    KC_2,    KC_1,    KC_0,                       XX,      XX,      XX,      XX,      XX,      XX,
  XX,      XX,      KC_6,    KC_5,    KC_4,    KC_0,                       OSM(MOD_LALT), OSM(MOD_LCTL), OSM(MOD_RSFT), OSM(MOD_RGUI), XX, XX,
  XX,      XX,      KC_9,    KC_8,    KC_7,    KC_0, XX,          XX,      XX,      XX,      XX,      XX,      XX,      XX,
                             XX,      KC_0,    KC_DOT, KC_BSPC,      XX,      XX,      TO(_MAIN), __
),

// SYMBOL - Symbol layer with window switching
[_SYMBOL] = LAYOUT(
  WIN_SWITCH, XX,      XX,      XX,      XX,      XX,                            XX,      XX,      XX,      XX,      XX,      XX,
  XX,      KC_QUES, XX,      KC_EQL,  XX,      KC_TILD,                      KC_GRV,  KC_LCBR, KC_RCBR, XX,      XX,      XX,
  KC_AMPR, KC_AT,   KC_BSLS, KC_DLR,  KC_SLSH, KC_GRV,                       KC_TILD, KC_LPRN, KC_RPRN, KC_PLUS, KC_EQL,  KC_QUOT,
  XX,      XX,      KC_0,    KC_EXLM, KC_CIRC, KC_PERC, KC_LBRC, KC_RBRC,  KC_CIRC, KC_LBRC, KC_RBRC, KC_DOT,  XX,      XX,
                             XX,      KC_LCBR, KC_RCBR, __,            KC_DQUO, KC_GT,   KC_SCLN, XX
),

// COMPARE - Python comparison operators with smart spacing (activated by holding C)
[_COMPARE] = LAYOUT(
  XX,      XX,      XX,      XX,      XX,      XX,                            XX,      CMP_GE,  CMP_IS,  CMP_OR,  CMP_NIN, XX,
  XX,      XX,      XX,      XX,      XX,      XX,                            XX,      XX,      XX,      XX,      XX,      XX,
  XX,      XX,      XX,      XX,      XX,      XX,                            CMP_ASSIGN, CMP_EQ, CMP_NE, CMP_IN,  CMP_AND, XX,
  XX,      XX,      XX,      XX,      XX,      XX,      XX,      XX,         XX,      CMP_LE,  CMP_LT,  CMP_GT,  CMP_ISN, XX,
                             XX,      XX,      XX,      __,            XX,      XX,      XX,      XX
),

// MATH - Python math operators (activated by holding X)
[_MATH] = LAYOUT(
  XX,      XX,      XX,      XX,      XX,      XX,                            XX,      MATH_ADDEQ, MATH_SUBEQ, MATH_MULEQ, MATH_DIVEQ, XX,
  XX,      XX,      XX,      XX,      XX,      XX,                            XX,      XX,         XX,         XX,         XX,         XX,
  XX,      XX,      XX,      XX,      XX,      XX,                            MATH_ADD, MATH_SUB, MATH_MUL,   MATH_DIV,   XX,         XX,
  XX,      XX,      XX,      XX,      XX,      XX,      XX,      XX,         XX,      MATH_FDIV,  MATH_MOD,   MATH_POW,   XX,         XX,
                             XX,      XX,      XX,      __,            XX,      XX,      XX,        XX
),

// LAYER_F - Symbol layer (activated by holding F)
[_LAYER_F] = LAYOUT(
  XX,      XX,      XX,      XX,      XX,      XX,                            XX,      XX,      XX,      XX,      XX,      XX,
  XX,      XX,      XX,      XX,      XX,      XX,                            KC_GRV,  KC_TILD, KC_ASTR, KC_PERC, KC_PLUS, XX,
  XX,      XX,      XX,      XX,      XX,      XX,                            KC_BSPC, KC_ESC,  KC_ENT,  KC_DEL,  KC_QUOT, XX,
  XX,      XX,      XX,      XX,      XX,      XX,      XX,      XX,         KC_EQL,  KC_LCBR, KC_RCBR, KC_CIRC, KC_DLR,  XX,
                             XX,      XX,      XX,      __,            XX,      XX,      XX,      XX
),

// LAYER_D - Bracket/comparison layer (activated by holding D)
[_LAYER_D] = LAYOUT(
  XX,      XX,      XX,      XX,      XX,      XX,                            XX,      XX,      XX,      XX,      XX,      XX,
  XX,      XX,      XX,      XX,      XX,      XX,                            KC_GT,   KC_LCBR, KC_RCBR, XX,      XX,      XX,
  XX,      XX,      XX,      XX,      XX,      XX,                            KC_EQL,  KC_LPRN, KC_RPRN, XX,      XX,      XX,
  XX,      XX,      XX,      XX,      XX,      XX,      XX,      XX,         KC_LT,   KC_LBRC, KC_RBRC, XX,      XX,      XX,
                             XX,      XX,      XX,      __,            XX,      XX,      XX,      XX
),

};

// Home Run Modifiers configuration
void on_home_run_action(uint16_t keycode, home_run_action_t action) {
    // Track taps for follower system
    if (action == HOME_RUN_ACTION_TAP) {
        switch (keycode) {
            case HR_A: follower_track_tap(KC_A); break;
            case HR_SCLN: follower_track_tap(KC_SCLN); break;
            case HR_X: follower_track_tap(KC_X); break;
            case HR_COMM: follower_track_tap(KC_COMM); break;
            case HR_C: follower_track_tap(KC_C); break;
            case HR_M: follower_track_tap(KC_M); break;
            case HR_SLSH: follower_track_tap(KC_SLSH); break;
            case HR_F: follower_track_tap(KC_F); break;
            case HR_D: follower_track_tap(KC_D); break;
        }
    }

    switch (keycode) {
        HOME_RUN_OPPOSITE_MT(HR_A, KC_A, KC_LSFT)
        HOME_RUN_OPPOSITE_MT(HR_SCLN, KC_SCLN, KC_RSFT)
        HOME_RUN_OPPOSITE_MT(HR_X, KC_X, KC_LCTL)
        HOME_RUN_OPPOSITE_MT(HR_COMM, KC_COMM, KC_RCTL)
        HOME_RUN_OPPOSITE_MT(HR_C, KC_C, KC_LALT)
        HOME_RUN_OPPOSITE_MT(HR_M, KC_M, KC_LALT)
        HOME_RUN_OPPOSITE_MT(HR_SLSH, KC_SLSH, KC_RGUI)
        HOME_RUN_OPPOSITE_MENU_ML(HR_F, KC_F, _LAYER_F)
        HOME_RUN_OPPOSITE_MENU_ML(HR_D, KC_D, _LAYER_D)
    }
}

// Enable opposite-hand detection for all home run modifiers
bool home_run_requires_opposite_hand(uint16_t keycode) {
    switch (keycode) {
        case HR_A:
        case HR_SCLN:
        case HR_X:
        case HR_COMM:
        case HR_C:
        case HR_M:
        case HR_SLSH:
        case HR_F:
        case HR_D:
            return true;
        default:
            return false;
    }
}

// Helper for follower to check if keycode is a home run key
static bool is_home_run_keycode(uint16_t keycode) {
    return (keycode >= HOME_RUN_KEYCODES_BEGIN && keycode < HOME_RUN_KEYCODES_END);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Track all keypresses for follower system (home run keys tracked separately)
    follower_track_key(keycode, record, is_home_run_keycode);

    if (!process_home_run(keycode, record)) {
        return false;
    }

    if (record->event.pressed) {
        switch (keycode) {
            case FOLLOWER_KEY:
                follower_process(follower_expansions, NUM_FOLLOWER_EXPANSIONS);
                return false;
            // OS-aware window switching
            case WIN_SWITCH:
                if (current_os == OS_MAC) {
                    // Mac: Cmd+Tab for app switching
                    register_code(KC_LGUI);
                    register_code(KC_TAB);
                } else {
                    // Windows: Alt+Tab for app switching
                    register_code(KC_LALT);
                    register_code(KC_TAB);
                }
                break;

            case OS_CYCLE:
                toggle_os();
                break;

            // OS-aware clipboard operations
            case OS_UNDO:
                if (current_os == OS_MAC) {
                    register_code(KC_LGUI);
                    register_code(KC_Z);
                } else {
                    register_code(KC_LCTL);
                    register_code(KC_Z);
                }
                break;

            case OS_CUT:
                if (current_os == OS_MAC) {
                    register_code(KC_LGUI);
                    register_code(KC_X);
                } else {
                    register_code(KC_LCTL);
                    register_code(KC_X);
                }
                break;

            case OS_COPY:
                if (current_os == OS_MAC) {
                    register_code(KC_LGUI);
                    register_code(KC_C);
                } else {
                    register_code(KC_LCTL);
                    register_code(KC_C);
                }
                break;

            case OS_PASTE:
                if (current_os == OS_MAC) {
                    register_code(KC_LGUI);
                    register_code(KC_V);
                } else {
                    register_code(KC_LCTL);
                    register_code(KC_V);
                }
                break;

            // Comparison operators (menu layer with auto-undo)
            case CMP_EQ:
                return menu_emit(HR_C, " == ");
            case CMP_NE:
                return menu_emit(HR_C, " != ");
            case CMP_LT:
                return menu_emit(HR_C, " < ");
            case CMP_GT:
                return menu_emit(HR_C, " > ");
            case CMP_LE:
                return menu_emit(HR_C, " <= ");
            case CMP_GE:
                return menu_emit(HR_C, " >= ");
            case CMP_IN:
                return menu_emit(HR_C, " in ");
            case CMP_NIN:
                return menu_emit(HR_C, " not in ");
            case CMP_IS:
                return menu_emit(HR_C, " is ");
            case CMP_ISN:
                return menu_emit(HR_C, " is not ");
            case CMP_AND:
                return menu_emit(HR_C, " and ");
            case CMP_OR:
                return menu_emit(HR_C, " or ");
            case CMP_ASSIGN:
                return menu_emit(HR_C, " = ");

            // Math operators (menu layer with auto-undo)
            case MATH_ADD:
                return menu_emit(HR_X, " + ");
            case MATH_SUB:
                return menu_emit(HR_X, " - ");
            case MATH_MUL:
                return menu_emit(HR_X, " * ");
            case MATH_DIV:
                return menu_emit(HR_X, " / ");
            case MATH_FDIV:
                return menu_emit(HR_X, " // ");
            case MATH_MOD:
                return menu_emit(HR_X, " % ");
            case MATH_POW:
                return menu_emit(HR_X, "**");  // No spaces for power operator
            case MATH_ADDEQ:
                return menu_emit(HR_X, " += ");
            case MATH_SUBEQ:
                return menu_emit(HR_X, " -= ");
            case MATH_MULEQ:
                return menu_emit(HR_X, " *= ");
            case MATH_DIVEQ:
                return menu_emit(HR_X, " /= ");
        }
    } else {
        switch (keycode) {
            // OS-aware clipboard operations release
            case OS_UNDO:
                if (current_os == OS_MAC) {
                    unregister_code(KC_Z);
                    unregister_code(KC_LGUI);
                } else {
                    unregister_code(KC_Z);
                    unregister_code(KC_LCTL);
                }
                break;

            case OS_CUT:
                if (current_os == OS_MAC) {
                    unregister_code(KC_X);
                    unregister_code(KC_LGUI);
                } else {
                    unregister_code(KC_X);
                    unregister_code(KC_LCTL);
                }
                break;

            case OS_COPY:
                if (current_os == OS_MAC) {
                    unregister_code(KC_C);
                    unregister_code(KC_LGUI);
                } else {
                    unregister_code(KC_C);
                    unregister_code(KC_LCTL);
                }
                break;

            case OS_PASTE:
                if (current_os == OS_MAC) {
                    unregister_code(KC_V);
                    unregister_code(KC_LGUI);
                } else {
                    unregister_code(KC_V);
                    unregister_code(KC_LCTL);
                }
                break;

            case WIN_SWITCH:
                if (current_os == OS_MAC) {
                    // Mac/iOS: Release Cmd+Tab
                    unregister_code(KC_TAB);
                    unregister_code(KC_LGUI);
                } else {
                    // Windows/Linux: Release Alt+Tab
                    unregister_code(KC_TAB);
                    unregister_code(KC_LALT);
                }
                break;
        }
    }

    return true;
}

void matrix_scan_user(void) {
}
