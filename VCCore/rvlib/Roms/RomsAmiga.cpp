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

// Amiga Roms are identified by CRC32
void
RomManager::registerAmigaRoms()
{
    database.insert(database.end(), {

    {
        .crc        = CRC32_BOOT_A1000_8K,
        .title      = "Amiga 1000 Boot Rom",
        .revision   = "8K",
        .released   = { 1985, 0 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_BOOT
    },{
        .crc        = CRC32_BOOT_A1000_64K,
        .title      = "Amiga 1000 Boot Rom",
        .revision   = "64KB",
        .released   = { 1985, 0 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_BOOT
    },{
        .crc        = CRC32_KICK07_27_003_BETA,
        .title      = "Kickstart 0.7 Beta",
        .revision   = "Rev 27.003 NTSC",
        .released   = { 1985, 7 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK10_30_NTSC,
        .title      = "Kickstart 1.0",
        .revision   = "Rev 30.000 NTSC",
        .released   = { 1985, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK11_31_034_NTSC,
        .title      = "Kickstart 1.1",
        .revision   = "Rev 31.034 NTSC",
        .released   = { 1985, 11 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK11_32_034_PAL,
        .title      = "Kickstart 1.1",
        .revision   = "Rev 32.034 PAL",
        .released   = { 1986, 2 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK12_33_166,
        .title      = "Kickstart 1.2",
        .revision   = "Rev 33.166",
        .released   = { 1986, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK12_33_180,
        .title      = "Kickstart 1.2",
        .revision   = "Rev 33.180",
        .released   = { 1986, 10 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK12_33_180_MRAS,
        .title      = "Kickstart 1.2",
        .revision   = "Rev 33.180 (MRAS patch)",
        .released   = { 2022, 0 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK12_33_180_G11R,
        .title      = "Kickstart 1.2",
        .revision   = "Rev 33.180 (Guardian patch)",
        .released   = { 1988, 0 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART,
        .flags      = RomFlags::Patched
    },{
        .crc        = CRC32_KICK121_34_004,
        .title      = "Kickstart 1.2",
        .revision   = "Rev 34.004",
        .released   = { 1986, 10 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK13_34_005_A500,
        .title      = "Kickstart 1.3",
        .revision   = "Rev 34.005",
        .released   = { 1987, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK13_34_005_A3000,
        .title      = "Kickstart 1.3",
        .revision   = "Rev 34.005",
        .released   = { 1987, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK13_34_005_G12R,
        .title      = "Kickstart 1.3",
        .revision   = "Rev 34.005 (Guardian patch)",
        .released   = { 1988, 0 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART,
        .flags      = RomFlags::Patched
    },{
        .crc        = CRC32_KICK14_36_015,
        .title      = "Kickstart 1.4 Alpha 15",
        .revision   = "Rev 36.015",
        .released   = { 1989, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK14_36_002,
        .title      = "Kickstart 1.4 Alpha 18",
        .revision   = "Rev 36.020",
        .released   = { 1989, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK20_36_028,
        .title      = "Kickstart 2.0",
        .revision   = "Rev 36.028",
        .released   = { 1990, 3 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK201_36_143,
        .title      = "Kickstart 2.01",
        .revision   = "Rev 36.143",
        .released   = { 1990, 8 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK202_36_207_A3000,
        .title      = "Kickstart 2.02",
        .revision   = "Rev 36.207",
        .released   = { 1990, 10 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK204_37_175_A500,
        .title      = "Kickstart 2.04",
        .revision   = "Rev 37.175",
        .released   = { 1991, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK204_37_175_A3000,
        .title      = "Kickstart 2.04",
        .revision   = "Rev 37.175",
        .released   = { 1991, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK20x_37_210_A600,
        .title      = "Kickstart 2.0?",
        .revision   = "Rev 37.210",
        .released   = { 1991, 11 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK205_37_299_A600,
        .title      = "Kickstart 2.05",
        .revision   = "Rev 37.299",
        .released   = { 1991, 11 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK205_37_300_A600HD,
        .title      = "Kickstart 2.05",
        .revision   = "Rev 37.300",
        .released   = { 1991, 11 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK205_37_350_A600HD,
        .title      = "Kickstart 2.05",
        .revision   = "Rev 37.350",
        .released   = { 1992, 4 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK30_39_106_A1200,
        .title      = "Kickstart 3.0",
        .revision   = "Rev 39.106",
        .released   = { 1992, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK30_39_106_A4000,
        .title      = "Kickstart 3.0",
        .revision   = "Rev 39.106",
        .released   = { 1992, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK31_40_055_A3000,
        .title      = "Kickstart 3.1",
        .revision   = "Rev 40.055",
        .released   = { 1993, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK31_40_060_CD32,
        .title      = "Kickstart 3.1",
        .revision   = "Rev 40.060",
        .released   = { 1993, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK31_40_063_A500,
        .title      = "Kickstart 3.1",
        .revision   = "Rev 40.063",
        .released   = { 1993, 7 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK31_40_063_A500_R,
        .title      = "Kickstart 3.1",
        .revision   = "Rev 40.063 (ReKick image)",
        .released   = { 0, 0 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK31_40_068_A1200,
        .title      = "Kickstart 3.1",
        .revision   = "Rev 40.068",
        .released   = { 1993, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK31_40_068_A3000,
        .title      = "Kickstart 3.1",
        .revision   = "Rev 40.068",
        .released   = { 1993, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK31_40_068_A4000,
        .title      = "Kickstart 3.1",
        .revision   = "Rev 40.068",
        .released   = { 1993, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK31_40_069_A1200,
        .title      = "Kickstart 3.1",
        .revision   = "Rev 40.069 Beta",
        .released   = { 1993, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK31_40_070_A4000T,
        .title      = "Kickstart 3.1",
        .revision   = "Rev 40.070",
        .released   = { 1994, 2 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_KICK35_40_071_A4000,
        .title      = "Fake Kickstart 3.5",
        .revision   = "Rev 40.071",
        .released   = { 0, 0 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_KICKSTART,
        .flags      = RomFlags::Patched
    },{
        .crc        = CRC32_CDTV_EXT_ROM_1_0,
        .title      = "CDTV Extended ROM",
        .revision   = "Rev 1.0",
        .released   = { 1991, 3 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_EXTROM
    },{
        .crc        = CRC32_CDTV_EXT_ROM_2_3,
        .title      = "CDTV Extended ROM",
        .revision   = "Rev 2.3",
        .released   = { 1992, 0 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_EXTROM
    },{
        .crc        = CRC32_CDTV_EXT_ROM_2_7,
        .title      = "CDTV Extended ROM",
        .revision   = "Rev 2.7",
        .released   = { 1992, 0 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::COMMODORE,
        .type       = RomType::AMIGA_EXTROM
    },{
        .crc        = CRC32_HYP314_46_143_A500,
        .title      = "Kickstart 3.1.4 (Hyperion)",
        .revision   = "Rev 46.143",
        .released   = { 2018, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP314_46_143_A1200,
        .title      = "Kickstart 3.1.4 (Hyperion)",
        .revision   = "Rev 46.143",
        .released   = { 2018, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP314_46_143_A2000,
        .title      = "Kickstart 3.1.4 (Hyperion)",
        .revision   = "Rev 46.143",
        .released   = { 2018, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP314_46_143_A3000,
        .title      = "Kickstart 3.1.4 (Hyperion)",
        .revision   = "Rev 46.143",
        .released   = { 2018, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP314_46_143_A4000,
        .title      = "Kickstart 3.1.4 (Hyperion)",
        .revision   = "Rev 46.143",
        .released   = { 2018, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP314_46_143_A4000T,
        .title      = "Kickstart 3.1.4 (Hyperion)",
        .revision   = "Rev 46.143",
        .released   = { 2021, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP320_47_96_A500,
        .title      = "Kickstart 3.2 (Hyperion)",
        .revision   = "Rev 47.96",
        .released   = { 2021, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP320_47_96_A1200,
        .title      = "Kickstart 3.2 (Hyperion)",
        .revision   = "Rev 47.96",
        .released   = { 2021, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP320_47_96_A3000,
        .title      = "Kickstart 3.2 (Hyperion)",
        .revision   = "Rev 47.96",
        .released   = { 2021, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP320_47_96_A4000,
        .title      = "Kickstart 3.2 (Hyperion)",
        .revision   = "Rev 47.96",
        .released   = { 2021, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP320_47_96_A4000T,
        .title      = "Kickstart 3.2 (Hyperion)",
        .revision   = "Rev 47.96",
        .released   = { 2021, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP321_47_102_A500,
        .title      = "Kickstart 3.2.1 (Hyperion)",
        .revision   = "Rev 47.102",
        .released   = { 2021, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP321_47_102_A1200,
        .title      = "Kickstart 3.2.1 (Hyperion)",
        .revision   = "Rev 47.102",
        .released   = { 2021, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP321_47_102_A3000,
        .title      = "Kickstart 3.2.1 (Hyperion)",
        .revision   = "Rev 47.102",
        .released   = { 2021, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP321_47_102_A4000,
        .title      = "Kickstart 3.2.1 (Hyperion)",
        .revision   = "Rev 47.102",
        .released   = { 2021, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP321_47_102_A4000T,
        .title      = "Kickstart 3.2.1 (Hyperion)",
        .revision   = "Rev 47.102",
        .released   = { 2021, 12 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP322_47_111_A500,
        .title      = "Kickstart 3.2.2 (Hyperion)",
        .revision   = "Rev 47.111",
        .released   = { 2023, 3 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP322_47_111_A1200,
        .title      = "Kickstart 3.2.2 (Hyperion)",
        .revision   = "Rev 47.111",
        .released   = { 2023, 3 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP322_47_111_A3000,
        .title      = "Kickstart 3.2.2 (Hyperion)",
        .revision   = "Rev 47.111",
        .released   = { 2023, 3 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP322_47_111_A4000,
        .title      = "Kickstart 3.2.2 (Hyperion)",
        .revision   = "Rev 47.111",
        .released   = { 2023, 3 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP322_47_111_A4000T,
        .title      = "Kickstart 3.2.2 (Hyperion)",
        .revision   = "Rev 47.111",
        .released   = { 2023, 3 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP323_47_115_A500,
        .title      = "Kickstart 3.2.3 (Hyperion)",
        .revision   = "Rev 47.115",
        .released   = { 2025, 4 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP323_47_115_A1200,
        .title      = "Kickstart 3.2.3 (Hyperion)",
        .revision   = "Rev 47.115",
        .released   = { 2025, 4 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP323_47_115_A3000,
        .title      = "Kickstart 3.2.3 (Hyperion)",
        .revision   = "Rev 47.115",
        .released   = { 2025, 4 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP323_47_115_A4000,
        .title      = "Kickstart 3.2.3 (Hyperion)",
        .revision   = "Rev 47.115",
        .released   = { 2025, 4 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_HYP323_47_115_A4000T,
        .title      = "Kickstart 3.2.3 (Hyperion)",
        .revision   = "Rev 47.115",
        .released   = { 2025, 4 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::HYPERION,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_AROS_54705,
        .title      = "AROS Kickstart replacement",
        .revision   = "SVN 54705 (UAE version)",
        .released   = { 2017, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::AROS,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_AROS_54705_EXT,
        .title      = "AROS Kickstart extension",
        .revision   = "SVN 54705 (UAE version)",
        .released   = { 2017, 5 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::AROS,
        .type       = RomType::AMIGA_EXTROM
    },{
        .crc        = CRC32_AROS_55696,
        .title      = "AROS Kickstart replacement",
        .revision   = "SVN 55696 (SAE version)",
        .released   = { 2019, 2 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::AROS,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_AROS_55696_EXT,
        .title      = "AROS Kickstart extension",
        .revision   = "SVN 55696 (SAE version)",
        .released   = { 2019, 2 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::AROS,
        .type       = RomType::AMIGA_EXTROM
    },{
        .crc        = CRC32_AROS_1ED13DE6E3,
        .title      = "AROS Kickstart replacement",
        .revision   = "Version 1ed13de6e3",
        .released   = { 2021, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::AROS,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_AROS_1ED13DE6E3_EXT,
        .title      = "AROS Kickstart extension",
        .revision   = "Version 1ed13de6e3",
        .released   = { 2021, 9 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::AROS,
        .type       = RomType::AMIGA_EXTROM
    },{
        .crc        = CRC32_AROS_20250219,
        .title      = "AROS Kickstart replacement",
        .revision   = "Version 20250219",
        .released   = { 2025, 2 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::AROS,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_AROS_20250219_EXT,
        .title      = "AROS Kickstart extension",
        .revision   = "Version 20250219",
        .released   = { 2025, 2 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::AROS,
        .type       = RomType::AMIGA_EXTROM
    },{
        .crc        = CRC32_AROS_20260820,
        .title      = "AROS Kickstart replacement",
        .revision   = "Version 20260820",
        .released   = { 2026, 8 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::AROS,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_AROS_20260820_EXT,
        .title      = "AROS Kickstart extension",
        .revision   = "Version 20260820",
        .released   = { 2026, 8 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::AROS,
        .type       = RomType::AMIGA_EXTROM
    },{
        .crc        = CRC32_EMUTOS13,
        .title      = "EmuTOS Kickstart replacement",
        .revision   = "Version 1.3",
        .released   = { 2024, 3 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::EMUTOS,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_EMUTOS14,
        .title      = "EmuTOS Kickstart replacement",
        .revision   = "Version 1.4",
        .released   = { 2025, 6 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::EMUTOS,
        .type       = RomType::AMIGA_KICKSTART
    },{
        .crc        = CRC32_DIAG11,
        .title      = "Amiga DiagROM",
        .revision   = "Version 1.1",
        .released   = { 2018, 10 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::AMIGA_DIAG
    },{
        .crc        = CRC32_DIAG12,
        .title      = "Amiga DiagROM",
        .revision   = "Version 1.2",
        .released   = { 2019, 8 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::AMIGA_DIAG
    },{
        .crc        = CRC32_DIAG121,
        .title      = "Amiga DiagROM",
        .revision   = "Version 1.2.1",
        .released   = { 2020, 7 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::AMIGA_DIAG
    },{
        .crc        = CRC32_DIAG13,
        .title      = "Amiga DiagROM",
        .revision   = "Version 1.3",
        .released   = { 2023, 4 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::AMIGA_DIAG
    },{
        .crc        = CRC32_DIAG20,
        .title      = "Amiga DiagROM",
        .revision   = "Version 2.0",
        .released   = { 2026, 3 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::AMIGA_DIAG
    },{
        .crc        = CRC32_CPUBLTRO_F8_0_3_2,
        .title      = "OCS CPUBLTRO",
        .revision   = "Version 0.3.2 (512 KB)",
        .released   = { 2024, 11 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::AMIGA_DEMO
    },{
        .crc        = CRC32_CPUBLTRO_FC_0_3_2,
        .title      = "OCS CPUBLTRO",
        .revision   = "Version 0.3.2 (256 KB)",
        .released   = { 2024, 11 },
        .platform   = RomPlatform::Amiga,
        .vendor     = RomVendor::OTHER,
        .type       = RomType::AMIGA_DEMO
    }
    });
}

}
