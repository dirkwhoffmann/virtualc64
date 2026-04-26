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

#include "BasicTypes.h"

namespace vc64 {

//
// Enumerations
//

enum class CartridgeType : long
{
    // Cartridges with a defined CRT ID (values must match)
    NORMAL             = 0,
    ACTION_REPLAY      = 1,
    KCS_POWER          = 2,
    FINAL_III          = 3,
    SIMONS_BASIC       = 4,
    OCEAN              = 5,
    EXPERT             = 6,
    FUNPLAY            = 7,
    SUPER_GAMES        = 8,
    ATOMIC_POWER       = 9,
    EPYX_FASTLOAD      = 10,
    WESTERMANN         = 11,
    REX                = 12,
    FINAL_I            = 13,
    MAGIC_FORMEL       = 14,
    GS                 = 15,
    WARPSPEED          = 16,
    DINAMIC            = 17,
    ZAXXON             = 18,
    MAGIC_DESK         = 19,
    SUPER_SNAPSHOT_V5  = 20,
    COMAL80            = 21,
    STRUCTURED_BASIC   = 22,
    ROSS               = 23,
    DELA_EP64          = 24,
    DELA_EP7X8         = 25,
    DELA_EP256         = 26,
    REX_EP256          = 27,
    MIKRO_ASSEMBLER    = 28,
    FINAL_PLUS         = 29,
    ACTION_REPLAY4     = 30,
    STARDOS            = 31,
    EASYFLASH          = 32,
    EASYFLASH_XBANK    = 33,
    CAPTURE            = 34,
    ACTION_REPLAY3     = 35,
    RETRO_REPLAY       = 36,
    MMC64              = 37,
    MMC_REPLAY         = 38,
    IDE64              = 39,
    SUPER_SNAPSHOT     = 40,
    IEEE488            = 41,
    GAME_KILLER        = 42,
    P64                = 43,
    EXOS               = 44,
    FREEZE_FRAME       = 45,
    FREEZE_MACHINE     = 46,
    SNAPSHOT64         = 47,
    SUPER_EXPLODE_V5   = 48,
    MAGIC_VOICE        = 49,
    ACTION_REPLAY2     = 50,
    MACH5              = 51,
    DIASHOW_MAKER      = 52,
    PAGEFOX            = 53,
    KINGSOFT           = 54,
    SILVERROCK_128     = 55,
    FORMEL64           = 56,
    RGCD               = 57,
    RRNETMK3           = 58,
    EASYCALC           = 59,
    GMOD2              = 60,
    MAX_BASIC          = 61,
    GMOD3              = 62,
    ZIPPCODE48         = 63,
    BLACKBOX8          = 64,
    BLACKBOX3          = 65,
    BLACKBOX4          = 66,
    REX_RAMFLOPPY      = 67,
    BISPLUS            = 68,
    SDBOX              = 69,
    MULTIMAX           = 70,
    BLACKBOX9          = 71,
    LT_KERNAL          = 72,
    RAMLINK            = 73,
    HERO               = 74,

    // Cartridges with no CRT ID (values can be arbitrary)
    ISEPIC,
    GEO_RAM,
    REU,
    NONE
};

struct CartridgeTypeEnum : Reflectable<CartridgeTypeEnum, CartridgeType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(CartridgeType::NONE);

    static const char *_key(CartridgeType value)
    {
        switch (value) {

            case CartridgeType::NORMAL:             return "NORMAL";
            case CartridgeType::ACTION_REPLAY:      return "ACTION_REPLAY";
            case CartridgeType::KCS_POWER:          return "KCS_POWER";
            case CartridgeType::FINAL_III:          return "FINAL_III";
            case CartridgeType::SIMONS_BASIC:       return "SIMONS_BASIC";
            case CartridgeType::OCEAN:              return "OCEAN";
            case CartridgeType::EXPERT:             return "EXPERT";
            case CartridgeType::FUNPLAY:            return "FUNPLAY";
            case CartridgeType::SUPER_GAMES:        return "SUPER_GAMES";
            case CartridgeType::ATOMIC_POWER:       return "ATOMIC_POWER";
            case CartridgeType::EPYX_FASTLOAD:      return "EPYX_FASTLOAD";
            case CartridgeType::WESTERMANN:         return "WESTERMANN";
            case CartridgeType::REX:                return "REX";
            case CartridgeType::FINAL_I:            return "FINAL_I";
            case CartridgeType::MAGIC_FORMEL:       return "MAGIC_FORMEL";
            case CartridgeType::GS:                 return "GS";
            case CartridgeType::WARPSPEED:          return "WARPSPEED";
            case CartridgeType::DINAMIC:            return "DINAMIC";
            case CartridgeType::ZAXXON:             return "ZAXXON";
            case CartridgeType::MAGIC_DESK:         return "MAGIC_DESK";
            case CartridgeType::SUPER_SNAPSHOT_V5:  return "SUPER_SNAPSHOT_V5";
            case CartridgeType::COMAL80:            return "COMAL80";
            case CartridgeType::STRUCTURED_BASIC:   return "STRUCTURED_BASIC";
            case CartridgeType::ROSS:               return "ROSS";
            case CartridgeType::DELA_EP64:          return "DELA_EP64";
            case CartridgeType::DELA_EP7X8:         return "DELA_EP7X8";
            case CartridgeType::DELA_EP256:         return "DELA_EP256";
            case CartridgeType::REX_EP256:          return "REX_EP256";
            case CartridgeType::MIKRO_ASSEMBLER:    return "MIKRO_ASSEMBLER";
            case CartridgeType::FINAL_PLUS:         return "FINAL_PLUS";
            case CartridgeType::ACTION_REPLAY4:     return "ACTION_REPLAY4";
            case CartridgeType::STARDOS:            return "STARDOS";
            case CartridgeType::EASYFLASH:          return "EASYFLASH";
            case CartridgeType::EASYFLASH_XBANK:    return "EASYFLASH_XBANK";
            case CartridgeType::CAPTURE:            return "CAPTURE";
            case CartridgeType::ACTION_REPLAY3:     return "REAACTION_REPLAY3D";
            case CartridgeType::RETRO_REPLAY:       return "RETRO_REPLAY";
            case CartridgeType::MMC64:              return "MMC64";
            case CartridgeType::MMC_REPLAY:         return "MMC_REPLAY";
            case CartridgeType::IDE64:              return "IDE64";
            case CartridgeType::SUPER_SNAPSHOT:     return "SUPER_SNAPSHOT";
            case CartridgeType::IEEE488:            return "IEEE488";
            case CartridgeType::GAME_KILLER:        return "GAME_KILLER";
            case CartridgeType::P64:                return "P64";
            case CartridgeType::EXOS:               return "EXOS";
            case CartridgeType::FREEZE_FRAME:       return "FREEZE_FRAME";
            case CartridgeType::FREEZE_MACHINE:     return "FREEZE_MACHINE";
            case CartridgeType::SNAPSHOT64:         return "SNAPSHOT64";
            case CartridgeType::SUPER_EXPLODE_V5:   return "SUPER_EXPLODE_V5";
            case CartridgeType::MAGIC_VOICE:        return "MAGIC_VOICE";
            case CartridgeType::ACTION_REPLAY2:     return "ACTION_REPLAY2";
            case CartridgeType::MACH5:              return "MACH5";
            case CartridgeType::DIASHOW_MAKER:      return "DIASHOW_MAKER";
            case CartridgeType::PAGEFOX:            return "PAGEFOX";
            case CartridgeType::KINGSOFT:           return "KINGSOFT";
            case CartridgeType::SILVERROCK_128:     return "SILVERROCK_128";
            case CartridgeType::FORMEL64:           return "FORMEL64";
            case CartridgeType::RGCD:               return "RGCD";
            case CartridgeType::RRNETMK3:           return "RRNETMK3";
            case CartridgeType::EASYCALC:           return "EASYCALC";
            case CartridgeType::GMOD2:              return "GMOD2";
            case CartridgeType::MAX_BASIC:          return "MAX_BASIC";
            case CartridgeType::GMOD3:              return "GMOD3";
            case CartridgeType::ZIPPCODE48:         return "ZIPPCODE48";
            case CartridgeType::BLACKBOX8:          return "BLACKBOX8";
            case CartridgeType::BLACKBOX3:          return "BLACKBOX3";
            case CartridgeType::BLACKBOX4:          return "BLACKBOX4";
            case CartridgeType::REX_RAMFLOPPY:      return "REX_RAMFLOPPY";
            case CartridgeType::BISPLUS:            return "BISPLUS";
            case CartridgeType::SDBOX:              return "SDBOX";
            case CartridgeType::MULTIMAX:           return "MULTIMAX";
            case CartridgeType::BLACKBOX9:          return "BLACKBOX9";
            case CartridgeType::LT_KERNAL:          return "LT_KERNAL";
            case CartridgeType::RAMLINK:            return "RAMLINK";
            case CartridgeType::HERO:               return "HERO";
            case CartridgeType::ISEPIC:             return "ISEPIC";
            case CartridgeType::GEO_RAM:            return "GEO_RAM";
            case CartridgeType::REU:                return "REU";
            case CartridgeType::NONE:               return "NONE";
        }
        return "???";
    }
    
    static const char *help(CartridgeType value)
    {
        return "";
    }
};

enum class FlashState : long
{
    READ,
    MAGIC_1,
    MAGIC_2,
    AUTOSELECT,
    BYTE_PROGRAM,
    BYTE_PROGRAM_ERROR,
    ERASE_MAGIC_1,
    ERASE_MAGIC_2,
    ERASE_SELECT,
    CHIP_ERASE,
    SECTOR_ERASE,
    SECTOR_ERASE_TIMEOUT,
    SECTOR_ERASE_SUSPEND
};

struct FlashStateEnum : Reflectable<FlashStateEnum, FlashState> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FlashState::SECTOR_ERASE_SUSPEND);

    static const char *_key(FlashState value)
    {
        switch (value) {

            case FlashState::READ:                 return "READ";
            case FlashState::MAGIC_1:              return "MAGIC_1";
            case FlashState::MAGIC_2:              return "MAGIC_2";
            case FlashState::AUTOSELECT:           return "AUTOSELECT";
            case FlashState::BYTE_PROGRAM:         return "BYTE_PROGRAM";
            case FlashState::BYTE_PROGRAM_ERROR:   return "BYTE_PROGRAM_ERROR";
            case FlashState::ERASE_MAGIC_1:        return "ERASE_MAGIC_1";
            case FlashState::ERASE_MAGIC_2:        return "ERASE_MAGIC_2";
            case FlashState::ERASE_SELECT:         return "ERASE_SELECT";
            case FlashState::CHIP_ERASE:           return "CHIP_ERASE";
            case FlashState::SECTOR_ERASE:         return "SECTOR_ERASE";
            case FlashState::SECTOR_ERASE_TIMEOUT: return "SECTOR_ERASE_TIMEOUT";
            case FlashState::SECTOR_ERASE_SUSPEND: return "SECTOR_ERASE_SUSPEND";
        }
        return "???";
    }
    
    static const char *help(FlashState value)
    {
        return "";
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
