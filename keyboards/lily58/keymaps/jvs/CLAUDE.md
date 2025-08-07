## Context and Guidelines

- I have a lily58 keyboard with ProMicro RP2040.
- I am using a github codespace to compile the configuration.
- Don't try to compile this locally, it won't work.
- Don't install anything on this machine.
- Do not try to flash the keyboard.
- I am able to download a uf2 file from my coderspace, and I am able to flash my keyboard.
- My keyboard does not have RGB lights or rotary encoders.
- Each half of my keyboard has an OLED screen.
- I am using this library to configure home row modifiers: https://github.com/stasmarkin/sm_td

## TODO

### Configure Leader Key

- When the leader key is tapped, it activates a very basic layer.
- This very basic layer closely matches the main default layer, only every key is just a normal basic key.
- When a leader sequence is over, the keyboard returns to its previous layer.
- The point of this scheme is that is avoids compatibility issues with sm_td.
- When the leader key is held, it acts like shift.
- This hold-tap behavior will be handled by custom code.


### Bugfixes

- OS detection doesn't seem to be working on my mac. I see "???" on my right OLED.


### Leader Key Implementation
