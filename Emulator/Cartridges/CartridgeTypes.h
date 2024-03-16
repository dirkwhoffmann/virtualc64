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

#include "Aliases.h"
#include "Reflection.hpp"

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
