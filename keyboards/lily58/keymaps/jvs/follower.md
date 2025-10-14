# Follower Key

A retroactive text expansion system for QMK keyboards.

## Overview

The Follower Key allows you to type text naturally, then trigger expansions after the fact. Unlike traditional text expansion (where you type a trigger and it expands immediately), the Follower system:

1. Tracks your recent keystrokes in a buffer
2. Waits for you to press the follower key
3. Checks if your recent typing matches any expansion
4. Replaces the match with the expanded text

## How It Works

### Character Buffer

The system maintains a rolling buffer of your last N characters (default: 15). As you type:
- Normal characters are added to the buffer
- Backspace removes the most recent character
- The buffer is layer-agnostic (tracks all typing)

### Expansions

Expansions are defined as input→output pairs:
```c
FOLLOWER_EXPANSION("gap", "git add -p")
FOLLOWER_EXPANSION("gst", "git status")
FOLLOWER_EXPANSION("gcm", "git commit -m \"\"", -1)  // Optional cursor offset
```

### Matching Logic

When you press the follower key:
1. Check buffer against all expansions, longest input first
2. Find the first expansion where the buffer ends with the input text
3. Emit backspaces to delete the input (length of input string)
4. Type out the output string
5. If cursor offset is specified, emit left arrow keys to position cursor

## Example Usage

```
You type: gap
You press: follower key
Result:   git add -p
          (3 backspaces delete "gap", then types "git add -p")
```

```
You type: gcm
You press: follower key
Result:   git commit -m "█"
          (cursor positioned between quotes)
```

## Configuration

```c
// In your keymap.c or config
#define FOLLOWER_BUFFER_SIZE 15
#define FOLLOWER_MAX_OUTPUT 64
```

## Limitations (v1)

- Only tracks printable characters and backspace
- Doesn't track cursor movement (arrows, home/end, etc.)
- Output is plain text only (no special keys beyond cursor positioning)
- Case sensitive matching

## Future Enhancements

- Track cursor position with arrow keys, delete, home/end
- Support modifier keys in output (Ctrl+X, etc.)
- Multiple follower keys with different expansion sets
- Case-insensitive matching option
- Fuzzy matching
