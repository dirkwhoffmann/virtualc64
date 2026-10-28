---
title: "EventSlot"
---
# EventSlot

```cpp
enum EventSlot
```

Defined in Components/C64Types.h:100

Slots of the event scheduler.

| Value | Description |
|-------|-------------|
| `SLOT_CIA1` | CIA 1 execution. |
| `SLOT_CIA2` | CIA 2 execution. |
| `SLOT_SEC` | Enables secondary slots. |
| `SLOT_SER` | Serial port (IEC bus). |
| `SLOT_DAT` | Datasette execution. |
| `SLOT_TER` | Enables tertiary slots. |
| `SLOT_EXP` | Expansion port. |
| `SLOT_TXD` | Serial data out (RS232). |
| `SLOT_RXD` | Serial data in (RS232). |
| `SLOT_MOT` | Datasette motor. |
| `SLOT_DC8` | Disk change (Drive 8). |
| `SLOT_DC9` | Disk change (Drive 9). |
| `SLOT_SNP` | Snapshots. |
| `SLOT_RSH0` | Retro Shell (GUI). |
| `SLOT_RSH1` | Retro Shell (RSH server). |
| `SLOT_RSH2` | Retro Shell (RPC server). |
| `SLOT_KEY` | Auto-typing. |
| `SLOT_DBG` | Debugging (Regression tester). |
| `SLOT_ALA` | Alarms (set by the GUI). |
| `SLOT_INS` | Handles periodic calls to inspect(). |
| `SLOT_COUNT` |  |

