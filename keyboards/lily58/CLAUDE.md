I have a lily58 keyboard with ProMicro RP2040.
I am using a github codespace to compile the configuration.
I am able to download a uf2 file from my coderspace, and I am able to flash my keyboard.
My keyboard does not have RGB lights or rotary encoders.
Each half of my keyboard has an OLED screen.
I want to make a new configuration for my keyboard.

I want to use this library to configure home row modifiers: https://github.com/stasmarkin/sm_td

I want to call my configuration "jvs".


Here's my current idea for the configuration:


Main Layer:
    ESC, 1, 2, 3, 4, 5,                6, 7, 8, 9, 0, Backspace
    TAB, Q, W, E, R, T,                Y, U, I, O, P, -
    LCTRL, A, S, D, F, G,              H, J, K, L, ;, '
    LAlt, LShift, Z, X, C, V, B,     B, N, M, ,, ., /, RShift
    LGUI, Lower, LShift, Leader, Enter, Space, Raise, Number


Main Layer, Hold for Modifers:
    - Hold D: LCtrl
    - Hold F: LAlt
    - Hold M: RAlt
    - Hold ,: RCtrl


Main Layer Combos:
    J+K = Esc
    D+F = Enter Vim Mode
    Hold Leader, press J -- cycle windows in reverse
    Hold Leader, press K -- cycle windows (alt+tab / command+tab)

Main Layer, Leader Key Sequences:
    am = &
    at = @
    bs = \
    bt = `
    ca = ^
    cl = :
    cm = ,
    da = -
    do = $
    dq = "
    dt = .
    eq = =
    ex = !
    fo = /
    fs = /
    gt = >
    ha = #
    la = <
    lb = [
    lc = {
    lp = (
    ls = [
    lt = <
    pc = %
    pe = %
    pe = .
    pi = |
    pl = +
    pp = |
    pr = %
    qm = ?
    qu = ?
    ra = >
    rb = ]
    rc = }
    rp = )
    rs = ]
    sc = ;
    se = ;
    sq = '
    st = *
    ti = ~
    tl = ~
    un = _
    us = _
    z = 0




Lower Layer:
    NOP, NOP, NOP, NOP, NOP, NOP,                             NOP, {, }, NOP, NOP, Delete
    NOP, NOP, NOP, NOP, NOP, NOP,                             NOP, (, ), NOP, NOP, NOP
    NOP, OSH-LGUI, OSH-LShift, OSH-LCTRL, OSH-LALT,           Left, Down, Up, Right, NOP, NOP
    NOP, NOP, NOP, NOP, NOP, NOP, NOP,                    NOP, NOP, [, ], NOP, NOP, NOP
    NOP, Lower, NOP, NOP,                                 NOP, NOP, NOP, NOP


Raise Layer:
    NOP, NOP, NOP, page up, page down, NOP,                    NOP, NOP, NOP, NOP, NOP, NOP
    NOP, NOP, NOP, home, end, NOP,                      NOP, NOP, NOP, NOP, NOP, NOP
    NOP, NOP, NOP, swap-win-rev, swap-win-fwd, NOP,                      OSH-LALT, OSH-LCTRL, OSH-RShift, OSH-RGUI, NOP
    NOP, NOP, undo, cut, copy, paste,             NOP, NOP, NOP, NOP, NOP, NOP, NOP
    NOP, NOP, NOP, NOP,                 NOP, NOP, Raise, NOP



Number Layer:
    F1, F2, F3, F4, F5, F6,          F7, F8, F9, F10, F11, F12
    NOP, NOP, 1, 2, 3, 0,                      NOP, NOP, NOP, NOP, NOP, NOP
    NOP, NOP, 4, 5, 6, 0,                      OSH-LALT, OSH-LCTRL, OSH-RShift, OSH-RGUI, NOP
    NOP, NOP, 7, 8, 9, 0,             NOP, NOP, NOP, NOP, NOP, NOP, NOP
    NOP, 0, ., Backspace,                 NOP, NOP, Main, NOP



Vim Mode:
- This mode emulates Vim Normal Mode
- ESC returns to the main layer



Issues:
- The right-half doesn't seem to go to sleep.
- "f:" doesn't seem to search for the next ":" character.
