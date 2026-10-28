---
generator: doxide
---


# Cmd

**`enum class Cmd : long`**

Emulator command


`NONE`
:   None


`CONFIG`
:   Configure the emulator


`CONFIG_ALL`
:   Configure the emulator


`CONFIG_SCHEME`
:   IConfigure the emulator with a predefined scheme


`HARD_RESET`
:   Perform a hard reset


`SOFT_RESET`
:   Perform a soft reset


`POWER_ON`
:   Switch power on


`POWER_OFF`
:   Switch power off


`RUN`
:   Start emulation


`PAUSE`
:   Pause emulation


`WARP_ON`
:   Switch on warp mode


`WARP_OFF`
:   Switch off warp mode


`TRACK_ON`
:   Switch on track mode


`TRACK_OFF`
:   Switch off track mode


`STEP_INTO`
:   Run the emulator for a single step


`STEP_OVER`
:   Run the emulator for a single step


`STEP_CYCLE`
:   Run the emulator for a single step


`FINISH_LINE`
:   Run the emulator for a single step


`FINISH_FRAME`
:   Run the emulator for a single step


`HALT`
:   Terminate the emulator thread


`ALARM_ABS`
:   Schedule an alarm (absolute cycle)


`ALARM_REL`
:   Schedule an alarm (relative cycle)


`INSPECTION_TARGET`
:   Sets the auto-inspection mask


`SAVE_WORKSPACE`
:   Save the workspace to a folder (path attached)


`SAVE_SNAPSHOT`
:   Save a snapshot to a file (path attached, value = compressor)


`HIBERNATE`
:   Hibernate into an SVM (path attached, value = bit 0: snapshot, bit 1: workspace)


`FLASH`
:   Flash a file into memory (path attached, value = item)


`CPU_BRK`
:   Let the CPU execute a BRK instruction


`CPU_NMI`
:   Emulate an external expansion port NMI


`GUARD_SET_AT`
:   Set a guard


`GUARD_REMOVE_NR`
:   Remove the n-th guard


`GUARD_MOVE_NR`
:   Change the address of a guard


`GUARD_REMOVE_AT`
:   Remove the guard at an address


`GUARD_REMOVE_ALL`
:   Remove all guards


`GUARD_ENABLE_NR`
:   Enable the n-th guard


`GUARD_ENABLE_AT`
:   Enable the guard at an address


`GUARD_ENABLE_ALL`
:   Enable all guards


`GUARD_DISABLE_NR`
:   Disable the n-th guard


`GUARD_DISABLE_AT`
:   Disable the guard at an address


`GUARD_DISABLE_ALL`
:   Disable all guards


`GUARD_TOGGLE_NR`
:   Toggle the n-th guard


`KEY_PRESS`
:   Press a key on the C64 keyboard


`KEY_RELEASE`
:   Release a key on the C64 keyboard


`KEY_RELEASE_ALL`
:   Clear the keyboard matrix


`KEY_TOGGLE`
:   Press or release a key on the C64 keyboard


`KEY_LOCK`
:   Lock a key on the C64 keyboard


`KEY_UNLOCK`
:   Unlock a key on the C64 keyboard


`KEY_UNLOCK_ALL`
:   Unlock all keys on the C64 keyboard


`KEY_ABORT_TYPING`
:   Delete all pending keyboard events


`KEY_AUTOTYPE`
:   Type a string with the auto-typing daemon (text attached)


`MOUSE_MOVE_ABS`
:   Signal a mouse movement (absolute)


`MOUSE_MOVE_REL`
:   Signal a mouse movement (relative)


`MOUSE_BUTTON`
:   Signal a mouse button event


`JOY_EVENT`
:   Signal a joystick button event


`JOY_STATE`
:   Set the complete joystick state (directions and fire button)


`DSK_TOGGLE_WP`
:   Toggle write-protection


`DSK_MODIFIED`
:   Signal that the disk has been saved


`DSK_UNMODIFIED`
:   Signal that the disk needs saving


`DSK_INSERT`
:   Insert a disk (path attached, value = drive, value2 = write protection)


`DATASETTE_PLAY`
:   Press the datasette play key


`DATASETTE_STOP`
:   Press the datasette stop key


`DATASETTE_REWIND`
:   Rewind the tape


`DATASETTE_INSERT`
:   Insert a tape (path attached)


`CRT_ATTACH`
:   Attach a cartridge (path attached, value = reset)


`CRT_BUTTON_PRESS`
:   Press a cartridge button


`CRT_BUTTON_RELEASE`
:   Release a cartridge button


`CRT_SWITCH_LEFT`
:   Pull the cartridge switch left


`CRT_SWITCH_NEUTRAL`
:   Put the cartridge switch in neutral position


`CRT_SWITCH_RIGHT`
:   Pull the cartridge switch right


`RSH_EXECUTE`
:   Execute a script command


`RSH_PRESS_KEY`
:   Type a special key into RetroShell (value = key, value2 = shift)


`RSH_PRESS_CHAR`
:   Type a character into RetroShell (value = character)


`RSH_PRESS_TEXT`
:   Type a string into RetroShell (text attached)


`CALL`
:   Run a function on the emulator thread (value2 = pointer to a std::function<void()> that stays alive until the command has been carried out)


`FOCUS`
:   The emulator windows got or lost focus



