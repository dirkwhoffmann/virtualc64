---
title: "Msg"
---
# Msg

```cpp
enum Msg
```

Defined in Infrastructure/MsgQueueTypes.h:26

Emulator message.

| Value | Description |
|-------|-------------|
| `NONE` | Unclassified. |
| `CONFIG` | A configuration request has been carried out (the exception is attached if it failed). |
| `POWER` | The emulator has been powered on or off. |
| `RUN` | The emulator has entered [ExecState::RUNNING](/reference/enum-ExecState.md). |
| `PAUSE` | The emulator has entered [ExecState::PAUSED](/reference/enum-ExecState.md). |
| `STEP` | The emulator has stepped a single instruction. |
| `EOL_REACHED` | The emulator has been paused at the end of the current line. |
| `EOF_REACHED` | The emulator has been paused at the end of the current frame. |
| `RESET` | The emulator has been reset. |
| `SHUTDOWN` | The emulator has been shut down. |
| `ABORT` | The emulator requests the GUI to quit the app. |
| `WARP` | The emulator has entered or exited warp mode. |
| `TRACK` | The emulator has entered or exited track mode. |
| `MUTE` | The emulator has been muted or unmuted. |
| `EASTER_EGG` | The easter egg was triggered. |
| `RSH_CLOSE` | RetroShell has been closed. |
| `RSH_UPDATE` | RetroShell has generated new output. |
| `RSH_SWITCH` | RetroShell has switched to another console. |
| `RSH_DEBUGGER` | DEPRECATED: The RetroShell debugger has been opend or closed. |
| `RSH_WAIT` | Execution has peen postponed due to a wait command. |
| `RSH_ERROR` | [Command](/reference/vc64-Command.md#command) execution has been aborted due to an error. |
| `RSH_DONE` | All pending commands have been executed. |
| `GUARD_UPDATED` | A guard list has been modified (value = GuardType). |
| `BREAKPOINT_REACHED` | A breakpoint has been reached. |
| `WATCHPOINT_REACHED` | A watchpoint has been reached. |
| `CPU_JUMPED` | The program counter has been modified manually. |
| `CPU_JAMMED` | The CPU has halted due to an illegal instruction. |
| `VIDEO_FORMAT` | The video format has changed (value = 0 for [PAL](/reference/vc64-PAL.md#pal-1), 1 for [NTSC](/reference/vc64-NTSC.md#ntsc)). |
| `SER_BUSY` | The IEC bus is transferring data. |
| `SER_IDLE` | The IEC bus has returned to idle state. |
| `MON_SETTING` | The monitor configuration has changed. |
| `DRIVE_CONNECT` | A floppy drive has been connected or disconnected. |
| `DRIVE_POWER` | The emulator has been powered on. |
| `DRIVE_POWER_SAVE` | Fast paths have been enabled or disabled. |
| `DRIVE_READ` | The floopy drive has switched to read mode. |
| `DRIVE_WRITE` | The floppy drive has switched to write mode. |
| `DRIVE_LED` | The green drive LED has switched on or off. |
| `DRIVE_MOTOR` | The disk has started or stopped spinning. |
| `DRIVE_STEP` | The drive head has stepped. |
| `DISK_INSERT` | A disk has been inserted (carries the exception if it failed). |
| `DISK_EJECT` | A disk has been ejected. |
| `DISK_PROTECTED` | The write protection status has changed. |
| `DISK_MODIFIED` | The disk needs saving. |
| `FILE_FLASHED` | A file has been flashed into memory (carries the exception if it failed). |
| `VC1530_CONNECT` | A datasette has been connected or disconnected. |
| `VC1530_TAPE` | A tape has been inserted or ejected (carries the exception if inserting failed). |
| `VC1530_PLAY` | The play key has been pressed. |
| `VC1530_MOTOR` | The drive motor has started or stopped. |
| `VC1530_COUNTER` | The tape counter has stepped. |
| `CRT_ATTACHED` | A cartridge has been plugged into the expansion port (carries the exception if it failed). |
| `KB_PRESS` | A key has been pressed. |
| `KB_RELEASE` | A key has been released. |
| `KB_LOCK` | A key has been locked. |
| `KB_UNLOCK` | A key has been unlocked. |
| `SHAKING` | A shaking mouse has been detected. |
| `ROM_LOADED` | A ROM has been loaded (value = RomType, or -1 if unknown; carries the exception if it failed). |
| `SNAPSHOT_TAKEN` | A snapshot has been taken. |
| `SNAPSHOT_RESTORED` | A snapshot has been restored (carries the exception if it failed). |
| `SNAPSHOT_SAVED` | A snapshot has been saved (carries the exception if it failed). |
| `WORKSPACE_LOADED` | A workspace has been loaded. |
| `WORKSPACE_SAVED` | A workspace has been saved (carries the exception if it failed). |
| `HIBERNATED` | The machine has been hibernated (carries the exception if it failed). |
| `QUEUE_FULL` | The command queue is full and commands are dropped. |
| `XRAY` | The X-Ray debugger has been started or stopped. |
| `ALARM` | A user-set alarm event has fired. |
| `RS232` | RS232 activity (DEPRECATED). |
| `RS232_IN` | RS232 adapter has received data. |
| `RS232_OUT` | RS232 adapter has sent data. |
| `SRV_STATE` | A remote server has switched state. |
| `SRV_RECEIVE` | A remote server has received a packet (value = ServerType, value2 = sequence number). |
| `SRV_SEND` | A remote server has sent a packet (value = ServerType, value2 = sequence number). |

