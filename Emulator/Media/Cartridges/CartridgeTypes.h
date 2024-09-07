// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------
/// @file

#pragma once

#include "Reflection.h"

namespace vc64 {

//
// Enumerations
//

enum_long(CRT_TYPE)
{
    // Cartridges with a defined CRT ID (values must match)
    CRT_NORMAL             = 0,
    CRT_ACTION_REPLAY      = 1,
    CRT_KCS_POWER          = 2,
    CRT_FINAL_III          = 3,
    CRT_SIMONS_BASIC       = 4,
    CRT_OCEAN              = 5,
    CRT_EXPERT             = 6,
    CRT_FUNPLAY            = 7,
    CRT_SUPER_GAMES        = 8,
    CRT_ATOMIC_POWER       = 9,
    CRT_EPYX_FASTLOAD      = 10,
    CRT_WESTERMANN         = 11,
    CRT_REX                = 12,
    CRT_FINAL_I            = 13,
    CRT_MAGIC_FORMEL       = 14,
    CRT_GS                 = 15,
    CRT_WARPSPEED          = 16,
    CRT_DINAMIC            = 17,
    CRT_ZAXXON             = 18,
    CRT_MAGIC_DESK         = 19,
    CRT_SUPER_SNAPSHOT_V5  = 20,
    CRT_COMAL80            = 21,
    CRT_STRUCTURED_BASIC   = 22,
    CRT_ROSS               = 23,
    CRT_DELA_EP64          = 24,
    CRT_DELA_EP7X8         = 25,
    CRT_DELA_EP256         = 26,
    CRT_REX_EP256          = 27,
    CRT_MIKRO_ASSEMBLER    = 28,
    CRT_FINAL_PLUS         = 29,
    CRT_ACTION_REPLAY4     = 30,
    CRT_STARDOS            = 31,
    CRT_EASYFLASH          = 32,
    CRT_EASYFLASH_XBANK    = 33,
    CRT_CAPTURE            = 34,
    CRT_ACTION_REPLAY3     = 35,
    CRT_RETRO_REPLAY       = 36,
    CRT_MMC64              = 37,
    CRT_MMC_REPLAY         = 38,
    CRT_IDE64              = 39,
    CRT_SUPER_SNAPSHOT     = 40,
    CRT_IEEE488            = 41,
    CRT_GAME_KILLER        = 42,
    CRT_P64                = 43,
    CRT_EXOS               = 44,
    CRT_FREEZE_FRAME       = 45,
    CRT_FREEZE_MACHINE     = 46,
    CRT_SNAPSHOT64         = 47,
    CRT_SUPER_EXPLODE_V5   = 48,
    CRT_MAGIC_VOICE        = 49,
    CRT_ACTION_REPLAY2     = 50,
    CRT_MACH5              = 51,
    CRT_DIASHOW_MAKER      = 52,
    CRT_PAGEFOX            = 53,
    CRT_KINGSOFT           = 54,
    CRT_SILVERROCK_128     = 55,
    CRT_FORMEL64           = 56,
    CRT_RGCD               = 57,
    CRT_RRNETMK3           = 58,
    CRT_EASYCALC           = 59,
    CRT_GMOD2              = 60,
    CRT_MAX_BASIC          = 61,
    CRT_GMOD3              = 62,
    CRT_ZIPPCODE48         = 63,
    CRT_BLACKBOX8          = 64,
    CRT_BLACKBOX3          = 65,
    CRT_BLACKBOX4          = 66,
    CRT_REX_RAMFLOPPY      = 67,
    CRT_BISPLUS            = 68,
    CRT_SDBOX              = 69,
    CRT_MULTIMAX           = 70,
    CRT_BLACKBOX9          = 71,
    CRT_LT_KERNAL          = 72,
    CRT_RAMLINK            = 73,
    CRT_HERO               = 74,

    // Cartridges with no CRT ID (values can be arbitrary)
    CRT_ISEPIC,
    CRT_GEO_RAM,
    CRT_REU,
    CRT_NONE
};
typedef CRT_TYPE CartridgeType;

struct CartridgeTypeEnum : util::Reflection<CartridgeTypeEnum, CartridgeType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CRT_NONE;

    static const char *prefix() { return "CRT"; }
    static const char *_key(long value)
    {
        switch (value) {

            case CRT_NORMAL:             return "NORMAL";
            case CRT_ACTION_REPLAY:      return "ACTION_REPLAY";
            case CRT_KCS_POWER:          return "KCS_POWER";
            case CRT_FINAL_III:          return "FINAL_III";
            case CRT_SIMONS_BASIC:       return "SIMONS_BASIC";
            case CRT_OCEAN:              return "OCEAN";
            case CRT_EXPERT:             return "EXPERT";
            case CRT_FUNPLAY:            return "FUNPLAY";
            case CRT_SUPER_GAMES:        return "SUPER_GAMES";
            case CRT_ATOMIC_POWER:       return "ATOMIC_POWER";
            case CRT_EPYX_FASTLOAD:      return "EPYX_FASTLOAD";
            case CRT_WESTERMANN:         return "WESTERMANN";
            case CRT_REX:                return "REX";
            case CRT_FINAL_I:            return "FINAL_I";
            case CRT_MAGIC_FORMEL:       return "MAGIC_FORMEL";
            case CRT_GS:                 return "GS";
            case CRT_WARPSPEED:          return "WARPSPEED";
            case CRT_DINAMIC:            return "DINAMIC";
            case CRT_ZAXXON:             return "ZAXXON";
            case CRT_MAGIC_DESK:         return "MAGIC_DESK";
            case CRT_SUPER_SNAPSHOT_V5:  return "SUPER_SNAPSHOT_V5";
            case CRT_COMAL80:            return "COMAL80";
            case CRT_STRUCTURED_BASIC:   return "STRUCTURED_BASIC";
            case CRT_ROSS:               return "ROSS";
            case CRT_DELA_EP64:          return "DELA_EP64";
            case CRT_DELA_EP7X8:         return "DELA_EP7X8";
            case CRT_DELA_EP256:         return "DELA_EP256";
            case CRT_REX_EP256:          return "REX_EP256";
            case CRT_MIKRO_ASSEMBLER:    return "MIKRO_ASSEMBLER";
            case CRT_FINAL_PLUS:         return "FINAL_PLUS";
            case CRT_ACTION_REPLAY4:     return "ACTION_REPLAY4";
            case CRT_STARDOS:            return "STARDOS";
            case CRT_EASYFLASH:          return "EASYFLASH";
            case CRT_EASYFLASH_XBANK:    return "EASYFLASH_XBANK";
            case CRT_CAPTURE:            return "CAPTURE";
            case CRT_ACTION_REPLAY3:     return "REAACTION_REPLAY3D";
            case CRT_RETRO_REPLAY:       return "RETRO_REPLAY";
            case CRT_MMC64:              return "MMC64";
            case CRT_MMC_REPLAY:         return "MMC_REPLAY";
            case CRT_IDE64:              return "IDE64";
            case CRT_SUPER_SNAPSHOT:     return "SUPER_SNAPSHOT";
            case CRT_IEEE488:            return "IEEE488";
            case CRT_GAME_KILLER:        return "GAME_KILLER";
            case CRT_P64:                return "P64";
            case CRT_EXOS:               return "EXOS";
            case CRT_FREEZE_FRAME:       return "FREEZE_FRAME";
            case CRT_FREEZE_MACHINE:     return "FREEZE_MACHINE";
            case CRT_SNAPSHOT64:         return "SNAPSHOT64";
            case CRT_SUPER_EXPLODE_V5:   return "SUPER_EXPLODE_V5";
            case CRT_MAGIC_VOICE:        return "MAGIC_VOICE";
            case CRT_ACTION_REPLAY2:     return "ACTION_REPLAY2";
            case CRT_MACH5:              return "MACH5";
            case CRT_DIASHOW_MAKER:      return "DIASHOW_MAKER";
            case CRT_PAGEFOX:            return "PAGEFOX";
            case CRT_KINGSOFT:           return "KINGSOFT";
            case CRT_SILVERROCK_128:     return "SILVERROCK_128";
            case CRT_FORMEL64:           return "FORMEL64";
            case CRT_RGCD:               return "RGCD";
            case CRT_RRNETMK3:           return "RRNETMK3";
            case CRT_EASYCALC:           return "EASYCALC";
            case CRT_GMOD2:              return "GMOD2";
            case CRT_MAX_BASIC:          return "MAX_BASIC";
            case CRT_GMOD3:              return "GMOD3";
            case CRT_ZIPPCODE48:         return "ZIPPCODE48";
            case CRT_BLACKBOX8:          return "BLACKBOX8";
            case CRT_BLACKBOX3:          return "BLACKBOX3";
            case CRT_BLACKBOX4:          return "BLACKBOX4";
            case CRT_REX_RAMFLOPPY:      return "REX_RAMFLOPPY";
            case CRT_BISPLUS:            return "BISPLUS";
            case CRT_SDBOX:              return "SDBOX";
            case CRT_MULTIMAX:           return "MULTIMAX";
            case CRT_BLACKBOX9:          return "BLACKBOX9";
            case CRT_LT_KERNAL:          return "LT_KERNAL";
            case CRT_RAMLINK:            return "RAMLINK";
            case CRT_HERO:               return "HERO";
            case CRT_ISEPIC:             return "ISEPIC";
            case CRT_GEO_RAM:            return "GEO_RAM";
            case CRT_REU:                return "REU";
            case CRT_NONE:               return "NONE";
        }
        return "???";
    }
};

enum_long(FLASH_STATE)
{
    FLASH_READ,
    FLASH_MAGIC_1,
    FLASH_MAGIC_2,
    FLASH_AUTOSELECT,
    FLASH_BYTE_PROGRAM,
    FLASH_BYTE_PROGRAM_ERROR,
    FLASH_ERASE_MAGIC_1,
    FLASH_ERASE_MAGIC_2,
    FLASH_ERASE_SELECT,
    FLASH_CHIP_ERASE,
    FLASH_SECTOR_ERASE,
    FLASH_SECTOR_ERASE_TIMEOUT,
    FLASH_SECTOR_ERASE_SUSPEND
};
typedef FLASH_STATE FlashState;

struct FlashStateEnum : util::Reflection<FlashStateEnum, FlashState> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = FLASH_SECTOR_ERASE_SUSPEND;

    static const char *prefix() { return "FLASH"; }
    static const char *_key(long value)
    {
        switch (value) {

            case FLASH_READ:                 return "READ";
            case FLASH_MAGIC_1:              return "MAGIC_1";
            case FLASH_MAGIC_2:              return "MAGIC_2";
            case FLASH_AUTOSELECT:           return "AUTOSELECT";
            case FLASH_BYTE_PROGRAM:         return "BYTE_PROGRAM";
            case FLASH_BYTE_PROGRAM_ERROR:   return "BYTE_PROGRAM_ERROR";
            case FLASH_ERASE_MAGIC_1:        return "ERASE_MAGIC_1";
            case FLASH_ERASE_MAGIC_2:        return "ERASE_MAGIC_2";
            case FLASH_ERASE_SELECT:         return "ERASE_SELECT";
            case FLASH_CHIP_ERASE:           return "CHIP_ERASE";
            case FLASH_SECTOR_ERASE:         return "SECTOR_ERASE";
            case FLASH_SECTOR_ERASE_TIMEOUT: return "SECTOR_ERASE_TIMEOUT";
            case FLASH_SECTOR_ERASE_SUSPEND: return "SECTOR_ERASE_SUSPEND";
        }
        return "???";
    }
};

//
// Structures
//

typedef struct
{
    CartridgeType type;
    const char *title;

    isize memory;
    bool  battery;

    isize buttons;
    const char *button1;
    const char *button2;

    isize switches;
    const char *switchLeft;
    const char *switchNeutral;
    const char *switchRight;

    isize leds;

    bool needsExecution;
}
CartridgeTraits;

typedef struct
{
    CartridgeType type; // DEPRECATED
    bool supported;
    bool gameLineInCrtFile;
    bool exromLineInCrtFile;
    isize numPackets;
    isize switchPos;
    bool led;
}
CartridgeInfo;

typedef struct
{
    u16 size;
    u16 loadAddress;
}
CartridgeRomInfo;

}
