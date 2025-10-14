# Home Run Modifiers - Specification

## Overview

Home Run Modifiers is a system for implementing home row modifiers with dual behavior: each key can act as either a normal key (TAP) or a modifier/layer key (HOLD), depending on timing and context.

The core challenge: When a home-run key is pressed, the system doesn't immediately know whether the user intends to tap it or hold it. The system must buffer events until it can determine the correct interpretation, then replay those events with the proper meaning applied.

## Core Algorithm

### Data Structures

**Single Event Buffer:**
- Contains a sequence of keyboard events (keypresses and releases)
- Each event includes: keycode, pressed/released state, key position, timestamp
- The first event in the buffer is always a home-run key press

**Tracking State:**
- `active`: Is tracking currently active?
- `keycode`: Which home-run key we're tracking
- `press_time`: When the home-run key was originally pressed
- `state`: UNKNOWN, NORMAL (tap), or MODIFIER (hold)
- Buffer of events
- Timing tracking for overlap detection

### Main Loop

```
1. If not tracking:
   - If home-run key pressed → start tracking, buffer the event
   - Otherwise → process event normally

2. If tracking:
   a. If this is the release of the tracked home-run key:
      - If state is UNKNOWN:
        * Check elapsed time and apply determination rules (timing/overlap)
        * Resolve as NORMAL or MODIFIER based on timing
        * Buffer the release event
      - Flush buffer with determined interpretation
      - Clear tracking

   b. If state is MODIFIER (already determined):
      - If this is another home-run key → interpret as TAP, process the TAP with the modifier active
      - If this is a regular key → process normally with modifier active

   c. If state is UNKNOWN (still deciding):
      - Buffer this event
      - Check if we can determine state now (timing/overlap rules)
      - If state determined → flush buffer
```

### Determining State

The system determines state based on:

1. **Long hold:** If home-run key held > 250ms → MODIFIER
2. **Full overlap:** If another key is fully pressed and released while home-run key held, and elapsed time > 150ms → MODIFIER
3. **Quick release:** If home-run key released before timing thresholds → NORMAL
4. **Same-hand roll (opposite-hand mode only):** If same-hand key pressed → NORMAL immediately

### Buffer Flush Algorithm

**When state is NORMAL (tap):**

```
For each event in buffer:
  If event is the tracked home-run key press:
    - Emit TAP action

  Else if event is the tracked home-run key release:
    - Skip (TAP action already emitted press+release)

  Else if event is another home-run key press:
    - Try to resolve this nested home-run key from remaining buffer
    - If cannot resolve (UNKNOWN):
      * Update tracking to track this new home-run key
      * Use the ORIGINAL timestamp from this event
      * Shift buffer to start at this event
      * Return to main loop (RECUR)
    - If resolved as NORMAL:
      * Emit TAP action
      * Skip to its release in buffer
    - If resolved as MODIFIER:
      * Emit HOLD action
      * Find and emit RELEASE action
      * Skip to its release in buffer

  Else (regular key):
    - Replay event normally
```

**When state is MODIFIER (hold):**

```
For each event in buffer:
  If event is the tracked home-run key press:
    - Emit HOLD action (activate modifier/layer)

  Else if event is the tracked home-run key release:
    - Emit RELEASE action (deactivate modifier/layer)

  Else if event is another home-run key:
    - Emit as TAP (nested home-run keys are always taps under modifier)
    - Skip to its release

  Else (regular key):
    - Replay event normally
```

**Key insight:** The recursion happens when flushing with NORMAL state and encountering an unresolved nested home-run key. At that point, we update the tracker to track the nested key (preserving its original timestamp) and continue buffering from that point.

## State Transition Example

**Scenario: Press C (HR), press M (HR), release C, hold M, press J**

1. Press C → Start tracking C (UNKNOWN), buffer [C-press@0ms]
2. Press M → Buffer [C-press@0ms, M-press@10ms]
3. Release C@100ms → State UNKNOWN, resolve as NORMAL, flush:
   - Emit C as TAP
   - Encounter M-press, try to resolve from buffer
   - M has no release yet → UNKNOWN
   - **Update tracker:** keycode=M, press_time=10ms, state=UNKNOWN
   - **Shift buffer:** [M-press@10ms, C-release@100ms]
   - Return to main loop (now tracking M)
4. Continue buffering new events into M's buffer
5. Press J@200ms (190ms after M pressed) → Buffer, triggers timing check
6. M held > 150ms with J pressed → M resolves as MODIFIER
7. Flush M's buffer as MODIFIER: activate M's layer, J processes on layer

## Extensions

### Opposite-Hand Detection

**Purpose:** Eliminate latency for same-hand typing rolls while requiring opposite-hand activation for modifier behavior.

**Mechanism:**
- Each home-run key can be marked as "requires opposite hand"
- Uses matrix row position to determine which hand a key is on
- Split keyboards: rows 0-3 are left hand, rows 4-7 are right hand

**Behavior:**
When tracking a home-run key with opposite-hand requirement:
- If a same-hand key is pressed → immediately resolve as NORMAL, flush buffer
- If an opposite-hand key is pressed → use normal timing rules

**Use case:** Design layers where left-hand keys activate layers and right-hand keys contain layer content (or vice versa). This eliminates all buffering latency for fast same-hand typing.

### Menu Layers

**Purpose:** Allow selecting from a menu of options where selecting a new option automatically replaces the previous selection.

**State Tracking:**
- `active_menu_key`: Which home-run key activated the current menu (0 if none)
- `menu_emit_length`: Length in characters of the last emitted sequence

**Behavior:**

When a menu layer is active (home-run key acting as MODIFIER with menu flag):
1. User presses option key → emit string (e.g., " == ")
2. User presses different option key:
   - Emit backspaces (count = `menu_emit_length`)
   - Emit new string (e.g., " != ")
   - Update `menu_emit_length` to new string length

When menu layer is released:
- Clear `active_menu_key` and `menu_emit_length`

**Example:**
- Hold C (activates _COMPARE layer)
- Press J → emits " == ", menu_emit_length=4
- Press K → emits 4 backspaces, then " != ", menu_emit_length=4
- Press M → emits 4 backspaces, then " <= ", menu_emit_length=4
- Release C → layer deactivates, menu state cleared

### Timing Constants

- **FULL_OVERLAP_LIMIT:** 150ms
  - If another key is fully pressed+released while HR key held, and elapsed > 150ms → MODIFIER
- **PARTIAL_OVERLAP_LIMIT:** 250ms
  - If HR key held > 250ms → MODIFIER
- **Quick tap detection:** If HR key released before FULL_OVERLAP_LIMIT → usually NORMAL

### One-Shot Layer Mode

Similar to momentary layer mode, but with "sticky" behavior:
- **Tap:** Sends the key normally
- **Hold briefly then release:** Layer activates for next keypress only, then deactivates
- **Hold and press other keys:** Layer stays active as long as HR key is held

## Critical Design Decisions

1. **Single buffer:** Only one home-run key is tracked at a time. When flushing discovers another home-run key, tracking switches to that key rather than tracking both simultaneously.

2. **Timestamp preservation:** When switching tracking to a nested home-run key, preserve the original timestamp of when that key was pressed. This ensures correct timing calculations.

3. **No nesting in MODIFIER state:** When a home-run key is acting as MODIFIER, all other home-run keys encountered are immediately emitted as TAPs. This prevents complex nested modifier states.

4. **Buffer or process, never both:** An event is either buffered (when state is UNKNOWN) or processed immediately (when state is MODIFIER or when not tracking). Never buffer an event then also process it.

5. **Replay during recursion:** When resolving a nested home-run key from the buffer during flush, the system must replay any events that occurred between the nested key's press and release.
