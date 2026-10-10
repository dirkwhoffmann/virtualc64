// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "Roms/RomManager.h"

namespace retro::vault {

// C64 Roms are identified by FNV-1a
void
RomManager::registerC64Roms()
{
    database.insert(database.end(), {

    //
    // Basic ROMs
    //

    {
        .fnv        = 0x20765FEA67A8762D,
        .title      = "Basic Rom",
        .revision   = "V2",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_BASIC
    },{
        .fnv        = 0xDE31E3D41C2E8EBE,
        .title      = "Basic Rom",
        .revision   = "M.E.G.A C64 OpenROM",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::MEGA65,
        .type       = RomType::C64_BASIC
    },

    //
    // Character ROMs
    //

    {
        .fnv        = 0xACC576F7B332AC15,
        .title      = "Character Rom",
        .revision   = "V1",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_CHAR
    },{
        .fnv        = 0x3CA9D37AA3DE0969,
        .title      = "Character Rom",
        .revision   = "Swedish (C2D007)",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_CHAR
    },{
        .fnv        = 0x6E3827A65FFF116F,
        .title      = "Character Rom",
        .revision   = "Swedish",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_CHAR
    },{
        .fnv        = 0x623CDF045B74B691,
        .title      = "Character Rom",
        .revision   = "Spanish",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_CHAR
    },{
        .fnv        = 0x79A236A3B3645231,
        .title      = "Character Rom",
        .revision   = "Danish V3",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_CHAR
    },{
        .fnv        = 0x8550B7ECEEED00C5,
        .title      = "Character Rom",
        .revision   = "Japanese",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_CHAR
    },{
        .fnv        = 0x2D5C504BBC4E5631,
        .title      = "Character Rom",
        .revision   = "Croatian",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_CHAR
    },{
        .fnv        = 0x4D31ECBF4F967DC3,
        .title      = "Character Rom",
        .revision   = "M.E.G.A C64 OpenROM",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::MEGA65,
        .type       = RomType::C64_CHAR
    },{
        .fnv        = 0x564e103e962682dd,
        .title      = "Character Rom",
        .revision   = "PXLfont V2.3",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::MEGA65,
        .type       = RomType::C64_CHAR
    },{
        .fnv        = 0x1130C1CE287876DD,
        .title      = "Patched Character Rom",
        .revision   = "Atari 800 Font",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_CHAR,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x975546A5B6168FFD,
        .title      = "Patched Character Rom",
        .revision   = "MSX Font",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_CHAR,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x7C74107C9365F735,
        .title      = "Patched Character Rom",
        .revision   = "ZX Spectrum Font",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_CHAR,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xAFFE8B0EE2176CBD,
        .title      = "Patched Character Rom",
        .revision   = "Amstrad Font",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_CHAR,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xD14C5BE4FEE17705,
        .title      = "Patched Character Rom",
        .revision   = "Topaz Font",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_CHAR,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xA2C6A6E2C0477981,
        .title      = "Patched Character Rom",
        .revision   = "Topaz Font V2",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_CHAR,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x3BF55C821EE80365,
        .title      = "Patched Character Rom",
        .revision   = "Topaz Font V2 (broken)",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_CHAR,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x19F0DD3F3F9C4FE9,
        .title      = "Patched Character Rom",
        .revision   = "Topaz Font V2",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_CHAR,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xE527AD3E0DDE930D,
        .title      = "Patched Character Rom",
        .revision   = "Teletext Font",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_CHAR,
        .flags      = RomFlags::Patched
    },

    //
    // Kernal ROMs
    //

    {
        .fnv        = 0xFB166E49AF709AB8,
        .title      = "Kernal Rom",
        .revision   = "V1",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0x4232D81CCD24FAAE,
        .title      = "Kernal Rom",
        .revision   = "V2",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0x4AF60EE54BEC9701,
        .title      = "Kernal Rom",
        .revision   = "V3",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0x429EA22675CAB478,
        .title      = "Kernal Rom",
        .revision   = "Danish V3",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0xF695289f3EC48A38,
        .title      = "Kernal Rom",
        .revision   = "Japanese",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0x1124ECFFE9ED2FE9,
        .title      = "Kernal Rom",
        .revision   = "Croatian",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0x8C4548E2202CB366,
        .title      = "Kernal Rom",
        .revision   = "",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0x746EB1BC008B07E1,
        .title      = "Kernal Rom",
        .revision   = "Scandinavian",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0x49A7074F1E6A896F,
        .title      = "Kernal Rom",
        .revision   = "V1",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0xB518628691B2CCA8,
        .title      = "Kernal Rom",
        .revision   = "M.E.G.A C64 OpenROM",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::MEGA65,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0xDE7F07008B787040,
        .title      = "Kernal Rom",
        .revision   = "JiffyDOS",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xA9D2AD1A4E5F782C,
        .title      = "Kernal Rom",
        .revision   = "JiffyDOS",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x750617B8DE6DBA82,
        .title      = "Kernal Rom",
        .revision   = "Turbo Rom V1",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x7E0A124C3F192818,
        .title      = "Kernal Rom",
        .revision   = "Datel Rom V3.2+",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x211EAC45AB03A2CA,
        .title      = "Kernal Rom",
        .revision   = "Exos Rom V3",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xF2A39FF166D338AE,
        .title      = "Kernal Rom",
        .revision   = "Turbo Tape V0.1",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL
    },{
        .fnv        = 0xFD1EBD928024F8D5,
        .title      = "Patched Kernal Rom",
        .revision   = "Turbo 250 V1.0",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x7202DEA530E1C172,
        .title      = "Patched Kernal Rom",
        .revision   = "64'er DOS V3",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x7E3AEFF7886684A2,
        .title      = "Patched Kernal Rom",
        .revision   = "SpeedDOS Plus",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x6E6190177D93D2BB,
        .title      = "Patched Kernal Rom",
        .revision   = "SpeedDOS Plus (TRIAD) V2.7",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x141A584872021C25,
        .title      = "Patched Kernal Rom",
        .revision   = "Dolphin DOS2 (donnchawp) V1.1",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xD135F409F4FA10F2,
        .title      = "Patched Kernal Rom",
        .revision   = "Dolphin DOS V1.0",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x32CAF94AAA196DB6,
        .title      = "Patched Kernal Rom",
        .revision   = "Dolphin DOS V2.0 Rev 1",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xAE3DEC803423CE60,
        .title      = "Patched Kernal Rom",
        .revision   = "Dolphin DOS V2.0 Rev 1 (M.A.)",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x4D3C32F9415972C3,
        .title      = "Patched Kernal Rom",
        .revision   = "Dolphin DOS V2.0 Rev 2",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xA5D930343EE32459,
        .title      = "Patched Kernal Rom",
        .revision   = "Dolphin DOS V2.0 Rev 3",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x7D34E8277F74A321,
        .title      = "Patched Kernal Rom",
        .revision   = "Dolphin DOS V2.0 (SilverDream)",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x877E38DA5DAFEC30,
        .title      = "Patched Kernal Rom",
        .revision   = "Dolphin DOS V3.0",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x2d7ca552f0332aef,
        .title      = "Patched Kernal Rom",
        .revision   = "JiffyDOS Dolphin Mod 2015",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x7D6153CE2E12474F,
        .title      = "Patched Kernal Rom",
        .revision   = "Masterom (Norland) V3.0",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x99692773DB9D7DF4,
        .title      = "Patched Kernal Rom",
        .revision   = "SD2IEC V2.2",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x57C8266769C5B580,
        .title      = "Patched Kernal Rom",
        .revision   = "JaffyDOS V1.3",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_KERNAL,
        .flags      = RomFlags::Patched
    },

    //
    // VC1541 ROM
    //

    // until here

    {
        .fnv        = 0x361A1EC48F04F5A4,
        .title      = "Floppy Drive Firmware",
        .revision   = "MOS 251968-01",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_VC1541
    },{
        .fnv        = 0xB938E2DA07F4FE40,
        .title      = "Floppy Drive Firmware",
        .revision   = "MOS 251968-02",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_VC1541
    },{
        .fnv        = 0x44BBA0EAC5898597,
        .title      = "Floppy Drive Firmware",
        .revision   = "MOS 251968-03",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_VC1541
    },{
        .fnv        = 0xA1D36980A17C8756,
        .title      = "Floppy Drive Firmware",
        .revision   = "MOS 355640-01",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::C64_VC1541
    },{
        .fnv        = 0x47CBA55F16FB3E09,
        .title      = "Patched Drive Firmware",
        .revision   = "Relocation Patch",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x8B2A523E29BED889,
        .title      = "Patched Drive Firmware",
        .revision   = "JiffyDOS Patch",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xF7F4D931219DBB5D,
        .title      = "Patched Drive Firmware",
        .revision   = "JiffyDOS Patch V6.0",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xB4027D6D9D61378A,
        .title      = "Patched Drive Firmware",
        .revision   = "64'er DOS V3",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xC50EAFCBA50C4B63,
        .title      = "Patched Drive Firmware",
        .revision   = "SpeedDOS Plus",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x92ADEBA1BCCD8D31,
        .title      = "Patched Drive Firmware",
        .revision   = "SpeedDOS Plus V2.7 (TRIAD)",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x682B5C77E7535B4A,
        .title      = "Patched Drive Firmware",
        .revision   = "Dolphin DOS2 (donnchawp) V1.1",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x28CD4E47A40C41CA,
        .title      = "Patched Drive Firmware",
        .revision   = "Dolphin DOS V2.0",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x1C1DDD64E02CAD32,
        .title      = "Patched Drive Firmware",
        .revision   = "Dolphin DOS V2.0 (SilverDream)",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0x09D8FBAB61E59FF0,
        .title      = "Patched Drive Firmware",
        .revision   = "Dolphin DOS V3.0",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },{
        .fnv        = 0xF684F72388EE5364,
        .title      = "Patched Drive Firmware",
        .revision   = "Dolphin DOS V3.0 (SilverDream)",
        .platform   = RomPlatform::C64,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::C64_VC1541,
        .flags      = RomFlags::Patched
    },
    });
}

}
