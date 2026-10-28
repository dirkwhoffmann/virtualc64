---
title: "DebugFlag"
---
# DebugFlag

```cpp
enum DebugFlag
```

Defined in Infrastructure/EmulatorTypes.h:26

Flags for enabling debug output.

| Value | Description |
|-------|-------------|
| `XFILES` | Report paranormal activity. |
| `CNF_DEBUG` | Debug configuration code. |
| `DEF_DEBUG` | Debug the user defaults storage. |
| `RUN_DEBUG` | Debug the run loop and component states. |
| `TIM_DEBUG` | Debug thread synchronization. |
| `WARP_DEBUG` | Debug warp mode. |
| `CMD_DEBUG` | Debug the command queue. |
| `MSG_DEBUG` | Debug the message queue. |
| `SNP_DEBUG` | Debug snapshots. |
| `RUA_DEBUG` | Inform about run-ahead activity. |
| `RUA_CHECKSUM` | Verify the integrity of the run-ahead instance. |
| `RUA_ON_STEROIDS` | Update the run-ahead instance in every frame. |
| `CPU_DEBUG` | Debug the CPU. |
| `IRQ_DEBUG` | Debug interrupts. |
| `MEM_DEBUG` | Debug memory accesses. |
| `CIA_DEBUG` | Debug the Complex Interface Adapters. |
| `CIAREG_DEBUG` | Debug the CIA register accesses. |
| `VICII_DEBUG` | Debug the Video Interface Controller. |
| `VICII_REG_DEBUG` | Debug VICII register accesses. |
| `VICII_SAFE_MODE` | Disable performance optimizations. |
| `VICII_STATS` | Collect statistical information. |
| `RASTERIRQ_DEBUG` | Debug raster interrupts. |
| `SID_DEBUG` | Debug the Sound Interface Device. |
| `SID_EXEC` | Debug SID execution. |
| `SIDREG_DEBUG` | Debug SID register accesses. |
| `AUD_DEBUG` | Debug audio parameters. |
| `AUDBUF_DEBUG` | Debug the audio buffers. |
| `AUDVOL_DEBUG` | Debug the audio volumes. |
| `VIA_DEBUG` | Debug the Versatile Interface Adapter. |
| `PIA_DEBUG` | Debug the Peripheral Interface Adapter. |
| `SER_DEBUG` | Debug the serial port (IEC bus). |
| `DSK_DEBUG` | Debug the disk controller execution. |
| `DSKCHG_DEBUG` | Debug the disk change procedure. |
| `GCR_DEBUG` | Debug the GCR encoder and decoder. |
| `FS_DEBUG` | Debug files system related code. |
| `PAR_DEBUG` | Debug the parallel drive cable. |
| `CRT_DEBUG` | Debug cartridges. |
| `FILE_DEBUG` | Debug the handling of media files. |
| `JOY_DEBUG` | Debug the joysticks. |
| `DRV_DEBUG` | Debug the floppy drives. |
| `TAP_DEBUG` | Debug the datasette. |
| `KBD_DEBUG` | Debug the keyboard. |
| `PRT_DEBUG` | Debug the control port and connected devices. |
| `EXP_DEBUG` | Debug the expansion port. |
| `USR_DEBUG` | User port. |
| `REU_DEBUG` | Debug the REU memory expansion. |
| `RSH_DEBUG` | Retro shell. |
| `SCK_DEBUG` | Debug the socket interface. |
| `SRV_DEBUG` | Debug the remote servers. |
| `GDB_DEBUG` | Debug the GDB server. |
| `DAP_DEBUG` | Debug the DAP server. |
| `FORCE_LAUNCH_ERROR` | Forced error condition. |
| `FORCE_ROM_MISSING` |  |
| `FORCE_MEGA64_MISMATCH` |  |
| `FORCE_SNAP_TOO_OLD` |  |
| `FORCE_SNAP_TOO_NEW` |  |
| `FORCE_SNAP_IS_BETA` |  |
| `FORCE_SNAP_CORRUPTED` |  |
| `FORCE_CRT_UNKNOWN` |  |
| `FORCE_CRT_UNSUPPORTED` |  |

