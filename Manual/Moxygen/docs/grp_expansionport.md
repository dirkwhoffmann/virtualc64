---
title: "Expansion port"
---
# Expansion port

The expansion port, cartridges, REU, and GeoRAM.

## Classes

| Name | Description |
|------|-------------|
| [`ExpansionPortAPI`](vc64-ExpansionPortAPI.md#expansionportapi) | Expansion Port Public [API](vc64-API.md#api) |
| [`CartridgeInfo`](vc64-CartridgeInfo.md#cartridgeinfo) | State of an attached cartridge. |
| [`CartridgeRomInfo`](vc64-CartridgeRomInfo.md#cartridgerominfo) | Description of a cartridge ROM chip. |
| [`CartridgeTraits`](vc64-CartridgeTraits.md#cartridgetraits) | Properties of a cartridge type. |
| [`ExpansionPortConfig`](vc64-ExpansionPortConfig.md#expansionportconfig) | Configuration of the expansion port. |

## Enumerations

| Name | Description |
|------|-------------|
| [`CRTMode`](#crtmode)  | Cartridge mode. |
| [`CartridgeType`](#cartridgetype)  | Supported cartridge types. |
| [`FlashState`](#flashstate)  | State of a flash ROM chip. |

---

### CRTMode

```cpp
enum CRTMode
```

Defined in Ports/ExpansionPortTypes.h:25

Cartridge mode.

| Value | Description |
|-------|-------------|
| `MODE16K` |  |
| `MODE8K` | 16KB cartridge |
| `ULTIMAX` | 8KB cartridge |
| `OFF` | Ultimax cartridge. |

---

### CartridgeType

```cpp
enum CartridgeType
```

Defined in Media/Cartridges/CartridgeTypes.h:25

Supported cartridge types.

| Value | Description |
|-------|-------------|
| `NORMAL` |  |
| `ACTION_REPLAY` |  |
| `KCS_POWER` |  |
| `FINAL_III` |  |
| `SIMONS_BASIC` |  |
| `OCEAN` |  |
| `EXPERT` |  |
| `FUNPLAY` |  |
| `SUPER_GAMES` |  |
| `ATOMIC_POWER` |  |
| `EPYX_FASTLOAD` |  |
| `WESTERMANN` |  |
| `REX` |  |
| `FINAL_I` |  |
| `MAGIC_FORMEL` |  |
| `GS` |  |
| `WARPSPEED` |  |
| `DINAMIC` |  |
| `ZAXXON` |  |
| `MAGIC_DESK` |  |
| `SUPER_SNAPSHOT_V5` |  |
| `COMAL80` |  |
| `STRUCTURED_BASIC` |  |
| `ROSS` |  |
| `DELA_EP64` |  |
| `DELA_EP7X8` |  |
| `DELA_EP256` |  |
| `REX_EP256` |  |
| `MIKRO_ASSEMBLER` |  |
| `FINAL_PLUS` |  |
| `ACTION_REPLAY4` |  |
| `STARDOS` |  |
| `EASYFLASH` |  |
| `EASYFLASH_XBANK` |  |
| `CAPTURE` |  |
| `ACTION_REPLAY3` |  |
| `RETRO_REPLAY` |  |
| `MMC64` |  |
| `MMC_REPLAY` |  |
| `IDE64` |  |
| `SUPER_SNAPSHOT` |  |
| `IEEE488` |  |
| `GAME_KILLER` |  |
| `P64` |  |
| `EXOS` |  |
| `FREEZE_FRAME` |  |
| `FREEZE_MACHINE` |  |
| `SNAPSHOT64` |  |
| `SUPER_EXPLODE_V5` |  |
| `MAGIC_VOICE` |  |
| `ACTION_REPLAY2` |  |
| `MACH5` |  |
| `DIASHOW_MAKER` |  |
| `PAGEFOX` |  |
| `KINGSOFT` |  |
| `SILVERROCK_128` |  |
| `FORMEL64` |  |
| `RGCD` |  |
| `RRNETMK3` |  |
| `EASYCALC` |  |
| `GMOD2` |  |
| `MAX_BASIC` |  |
| `GMOD3` |  |
| `ZIPPCODE48` |  |
| `BLACKBOX8` |  |
| `BLACKBOX3` |  |
| `BLACKBOX4` |  |
| `REX_RAMFLOPPY` |  |
| `BISPLUS` |  |
| `SDBOX` |  |
| `MULTIMAX` |  |
| `BLACKBOX9` |  |
| `LT_KERNAL` |  |
| `RAMLINK` |  |
| `HERO` |  |
| `ISEPIC` |  |
| `GEO_RAM` |  |
| `REU` |  |
| `NONE` |  |

---

### FlashState

```cpp
enum FlashState
```

Defined in Media/Cartridges/CartridgeTypes.h:210

State of a flash ROM chip.

| Value | Description |
|-------|-------------|
| `READ` |  |
| `MAGIC_1` |  |
| `MAGIC_2` |  |
| `AUTOSELECT` |  |
| `BYTE_PROGRAM` |  |
| `BYTE_PROGRAM_ERROR` |  |
| `ERASE_MAGIC_1` |  |
| `ERASE_MAGIC_2` |  |
| `ERASE_SELECT` |  |
| `CHIP_ERASE` |  |
| `SECTOR_ERASE` |  |
| `SECTOR_ERASE_TIMEOUT` |  |
| `SECTOR_ERASE_SUSPEND` |  |
