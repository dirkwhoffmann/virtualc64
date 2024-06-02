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

#define BASIC       ROM_TYPE_BASIC
#define KERNAL      ROM_TYPE_KERNAL
#define CHAR        ROM_TYPE_CHAR
#define VC1541      ROM_TYPE_VC1541

#define COMMODORE   ROM_VENDOR_COMMODORE
#define MEGA65      ROM_VENDOR_MEGA65
#define OTHER       ROM_VENDOR_OTHER

static std::vector<RomTraits> roms = {

    //
    // Basic ROMs
    //

    {
        .fnv        = 0x20765FEA67A8762D,
        .title      = "Basic Rom",
        .subtitle   = "Generic C64",
        .revision   = "V2",
        .vendor     = COMMODORE,
        .type       = BASIC
    },

    //
    // Character ROMs
    //

    {
        .fnv        = 0xACC576F7B332AC15,
        .title      = "Character Rom",
        .subtitle   = "Generic C64",
        .revision   = "V1",
        .vendor     = COMMODORE,
        .type       = CHAR
    },{
        .fnv        = 0x3CA9D37AA3DE0969,
        .title      = "Character Rom",
        .subtitle   = "Swedish C64 (C2D007)",
        .revision   = "",
        .vendor     = COMMODORE,
        .type       = CHAR
    },{
        .fnv        = 0x6E3827A65FFF116F,
        .title      = "Character Rom",
        .subtitle   = "Swedish C64",
        .revision   = "",
        .vendor     = COMMODORE,
        .type       = CHAR
    },{
        .fnv        = 0x623CDF045B74B691,
        .title      = "Character Rom",
        .subtitle   = "Spanish C64C",
        .revision   = "",
        .vendor     = COMMODORE,
        .type       = CHAR
    },{
        .fnv        = 0x79A236A3B3645231,
        .title      = "Character Rom",
        .subtitle   = "Danish C64",
        .revision   = "V3",
        .vendor     = COMMODORE,
        .type       = CHAR
    },{
        .fnv        = 0x4D31ECBF4F967DC3,
        .title      = "Character Rom",
        .subtitle   = "M.E.G.A C64 OpenROM",
        .revision   = "",
        .vendor     = MEGA65,
        .type       = CHAR
    },{
        .fnv        = 0x564e103e962682dd,
        .title      = "Character Rom",
        .subtitle   = "PXLfont",
        .revision   = "V2.3",
        .vendor     = MEGA65,
        .type       = CHAR
    },{
        .fnv        = 0x1130C1CE287876DD,
        .title      = "Patched Character Rom",
        .subtitle   = "Atari 800 Font",
        .revision   = "",
        .vendor     = OTHER,
        .type       = CHAR,
        .patched    = true
    },{
        .fnv        = 0x975546A5B6168FFD,
        .title      = "Patched Character Rom",
        .subtitle   = "MSX Font",
        .revision   = "",
        .vendor     = OTHER,
        .type       = CHAR,
        .patched    = true
    },{
        .fnv        = 0x7C74107C9365F735,
        .title      = "Patched Character Rom",
        .subtitle   = "ZX Spectrum Font",
        .revision   = "",
        .vendor     = OTHER,
        .type       = CHAR,
        .patched    = true
    },{
        .fnv        = 0xAFFE8B0EE2176CBD,
        .title      = "Patched Character Rom",
        .subtitle   = "Amstrad Font",
        .revision   = "",
        .vendor     = OTHER,
        .type       = CHAR,
        .patched    = true
    },{
        .fnv        = 0xD14C5BE4FEE17705,
        .title      = "Patched Character Rom",
        .subtitle   = "Topaz Font",
        .revision   = "",
        .vendor     = OTHER,
        .type       = CHAR,
        .patched    = true
    },{
        .fnv        = 0xA2C6A6E2C0477981,
        .title      = "Patched Character Rom",
        .subtitle   = "Topaz Font",
        .revision   = "V2",
        .vendor     = OTHER,
        .type       = CHAR,
        .patched    = true
    },{
        .fnv        = 0x3BF55C821EE80365,
        .title      = "Patched Character Rom",
        .subtitle   = "Topaz Font",
        .revision   = "V2 (broken)",
        .vendor     = OTHER,
        .type       = CHAR,
        .patched    = true
    },{
        .fnv        = 0x19F0DD3F3F9C4FE9,
        .title      = "Patched Character Rom",
        .subtitle   = "Topaz Font",
        .revision   = "V2",
        .vendor     = OTHER,
        .type       = CHAR,
        .patched    = true
    },{
        .fnv        = 0xE527AD3E0DDE930D,
        .title      = "Patched Character Rom",
        .subtitle   = "Teletext Font",
        .revision   = "",
        .vendor     = OTHER,
        .type       = CHAR,
        .patched    = true
    },

    //
    // Kernal ROMs
    //

    {
        .fnv        = 0xFB166E49AF709AB8,
        .title      = "Kernal Rom",
        .subtitle   = "Generic C64",
        .revision   = "V1",
        .vendor     = COMMODORE,
        .type       = KERNAL
    },{
        .fnv        = 0x4232D81CCD24FAAE,
        .title      = "Kernal Rom",
        .subtitle   = "Generic C64",
        .revision   = "V2",
        .vendor     = COMMODORE,
        .type       = KERNAL
    },{
        .fnv        = 0x4AF60EE54BEC9701,
        .title      = "Kernal Rom",
        .subtitle   = "Generic C64",
        .revision   = "V3",
        .vendor     = COMMODORE,
        .type       = KERNAL
    },{
        .fnv        = 0x429EA22675CAB478,
        .title      = "Kernal Rom",
        .subtitle   = "Danish C64",
        .revision   = "V3",
        .vendor     = COMMODORE,
        .type       = KERNAL
    },{
        .fnv        = 0x8C4548E2202CB366,
        .title      = "Kernal Rom",
        .subtitle   = "Generic SX64",
        .revision   = "",
        .vendor     = COMMODORE,
        .type       = KERNAL
    },{
        .fnv        = 0x746EB1BC008B07E1,
        .title      = "Kernal Rom",
        .subtitle   = "Scandinavian SX64",
        .revision   = "",
        .vendor     = COMMODORE,
        .type       = KERNAL
    },{
        .fnv        = 0x49A7074F1E6A896F,
        .title      = "Kernal Rom",
        .subtitle   = "PET64",
        .revision   = "V1",
        .vendor     = COMMODORE,
        .type       = KERNAL
    },{
        .fnv        = 0xDE7F07008B787040,
        .title      = "Kernal Rom",
        .subtitle   = "JiffyDOS",
        .revision   = "",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0xA9D2AD1A4E5F782C,
        .title      = "Kernal Rom",
        .subtitle   = "JiffyDOS (SX64)",
        .revision   = "",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x750617B8DE6DBA82,
        .title      = "Kernal Rom",
        .subtitle   = "Turbo Rom",
        .revision   = "V1",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x7E0A124C3F192818,
        .title      = "Kernal Rom",
        .subtitle   = "Datel Rom",
        .revision   = "V3.2+",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x211EAC45AB03A2CA,
        .title      = "Kernal Rom",
        .subtitle   = "Exos Rom",
        .revision   = "V3",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0xF2A39FF166D338AE,
        .title      = "Kernal Rom",
        .subtitle   = "Turbo Tape",
        .revision   = "V0.1",
        .vendor     = OTHER,
        .type       = KERNAL
    },{
        .fnv        = 0xFD1EBD928024F8D5,
        .title      = "Patched Kernal Rom",
        .subtitle   = "Turbo 250",
        .revision   = "V1.0",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x7202DEA530E1C172,
        .title      = "Patched Kernal Rom",
        .subtitle   = "64'er DOS",
        .revision   = "V3",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x7E3AEFF7886684A2,
        .title      = "Patched Kernal Rom",
        .subtitle   = "SpeedDOS Plus",
        .revision   = "",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x6E6190177D93D2BB,
        .title      = "Patched Kernal Rom",
        .subtitle   = "SpeedDOS Plus (TRIAD)",
        .revision   = "V2.7",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0xD135F409F4FA10F2,
        .title      = "Patched Kernal Rom",
        .subtitle   = "Dolphin DOS",
        .revision   = "V1.0",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x32CAF94AAA196DB6,
        .title      = "Patched Kernal Rom",
        .subtitle   = "Dolphin DOS",
        .revision   = "V2.0 Rev 1",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0xAE3DEC803423CE60,
        .title      = "Patched Kernal Rom",
        .subtitle   = "Dolphin DOS",
        .revision   = "V2.0 Rev 1 (M.A.)",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x4D3C32F9415972C3,
        .title      = "Patched Kernal Rom",
        .subtitle   = "Dolphin DOS",
        .revision   = "V2.0 Rev 2",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0xA5D930343EE32459,
        .title      = "Patched Kernal Rom",
        .subtitle   = "Dolphin DOS",
        .revision   = "V2.0 Rev 3",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x7D34E8277F74A321,
        .title      = "Patched Kernal Rom",
        .subtitle   = "Dolphin DOS",
        .revision   = "V2.0 (SilverDream)",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x877E38DA5DAFEC30,
        .title      = "Patched Kernal Rom",
        .subtitle   = "Dolphin DOS",
        .revision   = "V3.0",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x2d7ca552f0332aef,
        .title      = "Patched Kernal Rom",
        .subtitle   = "JiffyDOS Dolphin Mod",
        .revision   = "2015",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x7D6153CE2E12474F,
        .title      = "Patched Kernal Rom",
        .subtitle   = "Masterom (Norland)",
        .revision   = "V3.0",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x99692773DB9D7DF4,
        .title      = "Patched Kernal Rom",
        .subtitle   = "SD2IEC",
        .revision   = "V2.2",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },{
        .fnv        = 0x57C8266769C5B580,
        .title      = "Patched Kernal Rom",
        .subtitle   = "JaffyDOS",
        .revision   = "V1.3",
        .vendor     = OTHER,
        .type       = KERNAL,
        .patched    = true
    },

    //
    // VC1541 ROM
    //

    // until here

    {
        .fnv        = 0x361A1EC48F04F5A4,
        .title      = "Floppy Drive Firmware",
        .subtitle   = "MOS 251968-01",
        .revision   = "",
        .vendor     = COMMODORE,
        .type       = VC1541
    },{
        .fnv        = 0xB938E2DA07F4FE40,
        .title      = "Floppy Drive Firmware",
        .subtitle   = "MOS 251968-02",
        .revision   = "",
        .vendor     = COMMODORE,
        .type       = VC1541
    },{
        .fnv        = 0x44BBA0EAC5898597,
        .title      = "Floppy Drive Firmware",
        .subtitle   = "MOS 251968-03",
        .revision   = "",
        .vendor     = COMMODORE,
        .type       = VC1541
    },{
        .fnv        = 0xA1D36980A17C8756,
        .title      = "Floppy Drive Firmware",
        .subtitle   = "MOS 355640-01",
        .revision   = "",
        .vendor     = COMMODORE,
        .type       = VC1541
    },{
        .fnv        = 0x47CBA55F16FB3E09,
        .title      = "Patched Drive Firmware",
        .subtitle   = "Relocation Patch",
        .revision   = "",
        .vendor     = OTHER,
        .type       = VC1541,
        .patched    = true
    },{
        .fnv        = 0x8B2A523E29BED889,
        .title      = "Patched Drive Firmware",
        .subtitle   = "JiffyDOS Patch",
        .revision   = "",
        .vendor     = OTHER,
        .type       = VC1541,
        .patched    = true
    },{
        .fnv        = 0xF7F4D931219DBB5D,
        .title      = "Patched Drive Firmware",
        .subtitle   = "JiffyDOS Patch",
        .revision   = "V6.0",
        .vendor     = OTHER,
        .type       = VC1541,
        .patched    = true
    },{
        .fnv        = 0xB4027D6D9D61378A,
        .title      = "Patched Drive Firmware",
        .subtitle   = "64'er DOS",
        .revision   = "V3",
        .vendor     = OTHER,
        .type       = VC1541,
        .patched    = true
    },{
        .fnv        = 0xC50EAFCBA50C4B63,
        .title      = "Patched Drive Firmware",
        .subtitle   = "SpeedDOS Plus",
        .revision   = "",
        .vendor     = OTHER,
        .type       = VC1541,
        .patched    = true
    },{
        .fnv        = 0x92ADEBA1BCCD8D31,
        .title      = "Patched Drive Firmware",
        .subtitle   = "SpeedDOS Plus",
        .revision   = "V2.7 (TRIAD)",
        .vendor     = OTHER,
        .type       = VC1541,
        .patched    = true
    },{
        .fnv        = 0x28CD4E47A40C41CA,
        .title      = "Patched Drive Firmware",
        .subtitle   = "Dolphin DOS",
        .revision   = "V2.0",
        .vendor     = OTHER,
        .type       = VC1541,
        .patched    = true
    },{
        .fnv        = 0x1C1DDD64E02CAD32,
        .title      = "Patched Drive Firmware",
        .subtitle   = "Dolphin DOS",
        .revision   = "V2.0 (SilverDream)",
        .vendor     = OTHER,
        .type       = VC1541,
        .patched    = true
    },{
        .fnv        = 0x09D8FBAB61E59FF0,
        .title      = "Patched Drive Firmware",
        .subtitle   = "Dolphin DOS",
        .revision   = "V3.0",
        .vendor     = OTHER,
        .type       = VC1541,
        .patched    = true
    },{
        .fnv        = 0xF684F72388EE5364,
        .title      = "Patched Drive Firmware",
        .subtitle   = "Dolphin DOS",
        .revision   = "V3.0 (SilverDream)",
        .vendor     = OTHER,
        .type       = VC1541,
        .patched    = true
    }
};

#undef BASIC
#undef KERNAL
#undef CHAR
#undef VC1541

#undef COMMODORE
#undef MEGA65
#undef OTHER
