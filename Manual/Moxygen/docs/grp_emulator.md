---
title: "Emulator"
---
# Emulator

The emulator as a whole: VirtualC64 and C64API, the configuration options, the execution state, the user defaults, and the types shared by all components.

## Classes

| Name | Description |
|------|-------------|
| [`API`](vc64-API.md#api) | Base class of all APIs |
| [`C64API`](vc64-C64API.md#c64api) | C64 [API](vc64-API.md#api) |
| [`DefaultsAPI`](vc64-DefaultsAPI.md#defaultsapi) | The user's defaults storage |
| [`VirtualC64`](vc64-VirtualC64.md#virtualc64) | Public [API](vc64-API.md#api) |
| [`C64Config`](vc64-C64Config.md#c64config) | Configuration of the C64 component. |
| [`C64Info`](vc64-C64Info.md#c64info) | State of the C64 component. |
| [`EmulatorInfo`](vc64-EmulatorInfo.md#emulatorinfo) | The current emulator state. |
| [`EmulatorMetrics`](vc64-EmulatorMetrics.md#emulatormetrics) | Collected run-time data. |
| [`EventSlotInfo`](vc64-EventSlotInfo.md#eventslotinfo) | State of a single event slot. |
| [`HostConfig`](vc64-HostConfig.md#hostconfig) | Properties of the host machine. |
| [`RomInfo`](vc64-RomInfo.md#rominfo) | Description of an installed ROM. |

## Enumerations

| Name | Description |
|------|-------------|
| [`Class`](#class)  |  |
| [`ConfigScheme`](#configscheme)  | C64 model. |
| [`DebugFlag`](#debugflag)  | Flags for enabling debug output. |
| [`EventID`](#eventid)  | Events that can be scheduled in the event slots. |
| [`EventSlot`](#eventslot)  | Slots of the event scheduler. |
| [`ExecState`](#execstate)  | Execution state. |
| [`Opt`](#opt)  | Configuration option. |
| [`Warp`](#warp)  | [Warp](#warp) mode selection. |

---

### Class

```cpp
enum Class
```

Defined in Infrastructure/CoreComponentTypes.h:23

| Value | Description |
|-------|-------------|
| `Generic` |  |
| `C64` |  |
| `CPU` |  |
| `CIA` |  |
| `Emulator` |  |
| `Host` |  |
| `Memory` |  |
| `PIA` |  |
| `RemoteManager` |  |
| `RetroShell` |  |
| `SID` |  |
| `SIDBridge` |  |
| `TOD` |  |
| `VIA` |  |
| `VICII` |  |
| `AudioPort` |  |
| `ControlPort` |  |
| `ExpansionPort` |  |
| `PowerPort` |  |
| `RS232` |  |
| `SerialPort` |  |
| `UserPort` |  |
| `VideoPort` |  |
| `SampleRateDetector` |  |
| `Cartridge` |  |
| `Datasette` |  |
| `Drive` |  |
| `DriveMemory` |  |
| `Joystick` |  |
| `Keyboard` |  |
| `Monitor` |  |
| `Mouse` |  |
| `Paddle` |  |
| `ParallelCable` |  |

---

### ConfigScheme

```cpp
enum ConfigScheme
```

Defined in Components/C64Types.h:34

C64 model.

| Value | Description |
|-------|-------------|
| `PAL` | C64 ([PAL](vc64-PAL.md#pal-1)). |
| `PAL_II` | C64 II ([PAL](vc64-PAL.md#pal-1)). |
| `PAL_OLD` | Early C64 ([PAL](vc64-PAL.md#pal-1)). |
| `NTSC` | C64 ([NTSC](vc64-NTSC.md#ntsc)). |
| `NTSC_II` | C64 II ([NTSC](vc64-NTSC.md#ntsc)). |
| `NTSC_OLD` | Early C64 ([NTSC](vc64-NTSC.md#ntsc)). |

---

### DebugFlag

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

---

### EventID

```cpp
enum EventID
```

Defined in Components/C64Types.h:205

Events that can be scheduled in the event slots.

| Value | Description |
|-------|-------------|
| `EVENT_NONE` |  |
| `CIA_EXECUTE` |  |
| `CIA_WAKEUP` |  |
| `CIA_EVENT_COUNT` |  |
| `SEC_TRIGGER` |  |
| `SEC_EVENT_COUNT` |  |
| `SER_UPDATE` |  |
| `SER_EVENT_COUNT` |  |
| `DAT_EXECUTE` |  |
| `DAT_EVENT_COUNT` |  |
| `TER_TRIGGER` |  |
| `TER_EVENT_COUNT` |  |
| `EXP_REU_INITIATE` |  |
| `EXP_REU_PREPARE` |  |
| `EXP_REU_STASH` |  |
| `EXP_REU_FETCH` |  |
| `EXP_REU_SWAP` |  |
| `EXP_REU_VERIFY` |  |
| `EXP_REU_AUTOLOAD` |  |
| `EXP_REU_FINALIZE` |  |
| `EXP_EVENT_COUNT` |  |
| `TXD_BIT` |  |
| `TXD_EVENT_COUNT` |  |
| `RXD_BIT` |  |
| `RXD_EVENT_COUT` |  |
| `MOT_START` |  |
| `MOT_STOP` |  |
| `MOT_EVENT_COUNT` |  |
| `DCH_INSERT` |  |
| `DCH_EJECT` |  |
| `DCH_EVENT_COUNT` |  |
| `SNP_TAKE` |  |
| `SNP_EVENT_COUNT` |  |
| `RSH_WAKEUP` |  |
| `RSH_EVENT_COUNT` |  |
| `KEY_AUTO_TYPE` |  |
| `KEY_EVENT_COUNT` |  |
| `DBG_WATCHDOG` |  |
| `DBG_EVENT_COUNT` |  |
| `ALA_TRIGGER` |  |
| `ALA_EVENT_COUNT` |  |
| `INS_INSPECT` |  |
| `INS_EVENT_COUNT` |  |

---

### EventSlot

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

---

### ExecState

```cpp
enum ExecState
```

Defined in Infrastructure/ThreadTypes.h:25

Execution state.

| Value | Description |
|-------|-------------|
| `OFF` | Powered off. |
| `PAUSED` | Powered on, but currently paused. |
| `RUNNING` | Up and running. |
| `HALTED` | Shut down. |

---

### Opt

```cpp
enum Opt
```

Defined in Infrastructure/OptionTypes.h:25

Configuration option.

| Value | Description |
|-------|-------------|
| `HOST_REFRESH_RATE` | Video refresh rate of the host display. |
| `HOST_SAMPLE_RATE` | Host audio sample rate. |
| `HOST_TEX_FORMAT` | Texture format. |
| `HOST_FRAMEBUF_WIDTH` | Current width of the emulator window. |
| `HOST_FRAMEBUF_HEIGHT` | Current height of the emulator window. |
| `C64_WARP_BOOT` | Warp-boot time in seconds. |
| `C64_WARP_MODE` | [Warp](#warp) activation mode. |
| `C64_VSYNC` | Derive the frame rate to the VSYNC signal. |
| `C64_SPEED_BOOST` | Speed adjustment in percent. |
| `C64_RUN_AHEAD` | Number of run-ahead frames. |
| `DASM_NUMBERS` | Disassembler number format. |
| `VICII_REVISION` | Chip revision. |
| `VICII_GRAY_DOT_BUG` | Emulate gray-dot bug. |
| `VICII_POWER_SAVE` | Enable fast-paths. |
| `VICII_HIDE_SPRITES` | Hide some sprites. |
| `VICII_CUT_LAYERS` | Cutout some graphics layers. |
| `VICII_SS_COLLISIONS` | Check for sprite-sprite collisions. |
| `VICII_SB_COLLISIONS` | Check for sprite-background collisions. |
| `XRAY_MODE` | X-Ray mode (off, DMA debugger, layer inspector). |
| `XRAY_OVERLAY` | Overlay DMA texture onto emulator texture. |
| `XRAY_OVERLAY_STYLE` | DMA texture overlay mode. |
| `XRAY_OVERLAY_OPACITY` | DMA texture opacity. |
| `XRAY_DMA_CHANNEL0` | Enable or disable channel 0. |
| `XRAY_DMA_CHANNEL1` | Enable or disable channel 1. |
| `XRAY_DMA_CHANNEL2` | Enable or disable channel 2. |
| `XRAY_DMA_CHANNEL3` | Enable or disable channel 3. |
| `XRAY_DMA_CHANNEL4` | Enable or disable channel 4. |
| `XRAY_DMA_CHANNEL5` | Enable or disable channel 5. |
| `XRAY_COLOR0` | Color for channel 0. |
| `XRAY_COLOR1` | Color for channel 1. |
| `XRAY_COLOR2` | Color for channel 2. |
| `XRAY_COLOR3` | Color for channel 3. |
| `XRAY_COLOR4` | Color for channel 4. |
| `XRAY_COLOR5` | Color for channel 5. |
| `XRAY_COLOR6` | Color for layer 6 (sprite 6). |
| `XRAY_COLOR7` | Color for layer 7 (sprite 7). |
| `XRAY_COLOR8` | Color for layer 8 (border). |
| `XRAY_COLOR9` | Color for layer 9 (foreground graphics). |
| `XRAY_COLOR10` | Color for layer 10 (background graphics). |
| `XRAY_COMPOSITOR` | Who merges the X-Ray texture into the picture. |
| `EXP_REU_SPEED` | Transfer speed of the RAM Extension Unit. |
| `USR_DEVICE` | Device connected to the user port. |
| `VID_WHITE_NOISE` | Generate white-noise when switched off. |
| `MON_PALETTE` | Color palette. |
| `MON_BRIGHTNESS` | Brightness. |
| `MON_CONTRAST` | Contrast. |
| `MON_SATURATION` | Color saturation. |
| `MON_HCENTER` | Horizontal centering. |
| `MON_VCENTER` | Vertical centering. |
| `MON_HZOOM` | Horizontal zoom. |
| `MON_VZOOM` | Vertical zoom. |
| `MON_UPSCALER` | Pixel upscaler. |
| `MON_BLUR` | Blur enable. |
| `MON_BLUR_RADIUS` | Blur radius. |
| `MON_BLOOM` | Bloom enable. |
| `MON_BLOOM_RADIUS` | Bloom radius. |
| `MON_BLOOM_BRIGHTNESS` | Bloom brightness. |
| `MON_BLOOM_WEIGHT` | Bloom weight. |
| `MON_DOTMASK` | [Dotmask](grp_audiovideo.md#dotmask) pattern. |
| `MON_DOTMASK_BRIGHTNESS` | [Dotmask](grp_audiovideo.md#dotmask) brightness. |
| `MON_SCANLINES` | Scanline mode. |
| `MON_SCANLINE_BRIGHTNESS` | Scanline brightness. |
| `MON_SCANLINE_WEIGHT` | Scanline weight. |
| `MON_DISALIGNMENT` | Cathode ray disalignment enable. |
| `MON_DISALIGNMENT_H` | Horizontal cathode ray disalignment. |
| `MON_DISALIGNMENT_V` | Vertical cathode ray disalignment. |
| `POWER_GRID` | Power-grid stability (affects TOD). |
| `GLUE_LOGIC` | VICII glue-logic type. |
| `CIA_REVISION` | Chip revision. |
| `CIA_TIMER_B_BUG` | Emulate timer B bug. |
| `CIA_IDLE_SLEEP` | Enter idle state while not in use. |
| `SID_ENABLE` | Enable or disable SID. |
| `SID_ADDRESS` | Mapping address in memory. |
| `SID_REV` | Chip revision. |
| `SID_FILTER` | Enable or disables the audio filter. |
| `SID_ENGINE` | SID backend (e.g., reSID). |
| `SID_SAMPLING` | Audio sampling mode. |
| `SID_POWER_SAVE` | Enable fast-paths. |
| `AUD_VOL0` | Channel 0 volume. |
| `AUD_VOL1` | Channel 1 volume. |
| `AUD_VOL2` | Channel 2 volume. |
| `AUD_VOL3` | Channel 3 volume. |
| `AUD_PAN0` | Channel 0 panning. |
| `AUD_PAN1` | Channel 1 panning. |
| `AUD_PAN2` | Channel 2 panning. |
| `AUD_PAN3` | Channel 3 panning. |
| `AUD_VOL_L` | Master volume (left channel). |
| `AUD_VOL_R` | Master volume (right channel). |
| `AUD_BUFFER_SIZE` | Size of the audio ringbuffer. |
| `AUD_ASR` | Adaptive Sample Rate. |
| `MEM_INIT_PATTERN` | Ram initialization pattern. |
| `MEM_HEATMAP` | Memory heatmap. |
| `MEM_SAVE_ROMS` | Save Roms in snapshots. |
| `DRV_AUTO_CONFIG` | Auto-configure drives based on the Drive Rom. |
| `DRV_TYPE` | Drive model. |
| `DRV_RAM` | Drive Ram. |
| `DRV_SAVE_ROMS` | Save Roms in snapshots. |
| `DRV_PARCABLE` | Parallel cable type. |
| `DRV_CONNECT` | Connection status. |
| `DRV_POWER_SWITCH` | Power switch (on/off). |
| `DRV_POWER_SAVE` | Enable fast-paths. |
| `DRV_EJECT_DELAY` | Disk ejection delay. |
| `DRV_SWAP_DELAY` | Disk swap delay. |
| `DRV_INSERT_DELAY` | Disk insertion delay. |
| `DRV_PAN` | Pan. |
| `DRV_POWER_VOL` | Volume (power-up sound). |
| `DRV_STEP_VOL` | Volume (head steps). |
| `DRV_INSERT_VOL` | Volume (disk insertion). |
| `DRV_EJECT_VOL` | Volume (disk ejection). |
| `DAT_MODEL` | Datasette model. |
| `DAT_CONNECT` | Connection status. |
| `MOUSE_MODEL` | Mouse model. |
| `MOUSE_SHAKE_DETECT` | Detect a shaking mouse. |
| `MOUSE_VELOCITY` | Mouse velocity. |
| `AUTOFIRE` | Autofire status [on/off]. |
| `AUTOFIRE_BURSTS` | Burst mode (on/off). |
| `AUTOFIRE_BULLETS` | Number of bullets per burst. |
| `AUTOFIRE_DELAY` | Delay between two shots [frames]. |
| `PADDLE_ORIENTATION` | Paddle value mapping scheme. |
| `RS232_DEVICE` | Device connected to the RS232 adapter. |
| `RS232_BAUD` | Transmission rate. |
| `SRV_ENABLE` | Server enable status. |
| `SRV_PORT` | Server port. |
| `SRV_TRANSPORT` | Server transport protocol. |
| `SRV_VERBOSE` | Verbose mode. |
| `DBG_DEBUGCART` | Emulate the VICE debug cartridge. |
| `DBG_WATCHDOG` | Watchdog delay in frames. |
| `COUNT` | Sentinel: the number of options (not an option). |

---

### Warp

```cpp
enum Warp
```

Defined in Components/C64Types.h:70

[Warp](#warp) mode selection.

| Value | Description |
|-------|-------------|
| `AUTO` |  |
| `NEVER` |  |
| `ALWAYS` |  |
