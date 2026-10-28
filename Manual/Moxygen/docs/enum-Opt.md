---
title: "Opt"
---
# Opt

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
| `C64_WARP_MODE` | [Warp](enum-Warp.md) activation mode. |
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
| `MON_DOTMASK` | [Dotmask](enum-Dotmask.md) pattern. |
| `MON_DOTMASK_BRIGHTNESS` | [Dotmask](enum-Dotmask.md) brightness. |
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

