// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Reflectable.h"

namespace retro::vault {

using utl::isize;
using utl::u8;
using utl::u16;
using utl::u32;
using utl::u64;


//
// Constants
//

/* Checksums of the Amiga Roms known to RomManager. Amiga Roms are identified
 * by CRC32; C64 Roms are identified by FNV-1a and are spelled out inline in the
 * database, as they have no established names to give them here.
 */
constexpr u32 CRC32_MISSING                  = 0x00000000;
constexpr u32 CRC32_BOOT_A1000_8K            = 0x62F11C04;
constexpr u32 CRC32_BOOT_A1000_64K           = 0x0B1AD2D0;

constexpr u32 CRC32_KICK07_27_003_BETA       = 0x428A9A4B;

constexpr u32 CRC32_KICK10_30_NTSC           = 0x299790FF;

constexpr u32 CRC32_KICK11_31_034_NTSC       = 0xD060572A;
constexpr u32 CRC32_KICK11_32_034_PAL        = 0xEC86DAE2;

constexpr u32 CRC32_KICK12_33_166            = 0x9ED783D0;
constexpr u32 CRC32_KICK12_33_180            = 0xA6CE1636;
constexpr u32 CRC32_KICK12_33_180_MRAS       = 0xF80F0FC5;
constexpr u32 CRC32_KICK12_33_180_G11R       = 0x85067666;
constexpr u32 CRC32_KICK121_34_004           = 0xDB4C8033;

constexpr u32 CRC32_KICK13_34_005_A500       = 0xC4F0F55F;
constexpr u32 CRC32_KICK13_34_005_A3000      = 0xE0F37258;
constexpr u32 CRC32_KICK13_34_005_G12R       = 0x74680D37;

constexpr u32 CRC32_KICK14_36_015            = 0xA3BA6116;
constexpr u32 CRC32_KICK14_36_002            = 0x39779507;

constexpr u32 CRC32_KICK20_36_028            = 0xB4113910;
constexpr u32 CRC32_KICK201_36_143           = 0xB333D3C6;
constexpr u32 CRC32_KICK202_36_207_A3000     = 0x9A15519D;
constexpr u32 CRC32_KICK204_37_175_A500      = 0xC3BDB240;
constexpr u32 CRC32_KICK204_37_175_A3000     = 0x234A7233;
constexpr u32 CRC32_KICK20x_37_210_A600      = 0x4C0C1603;
constexpr u32 CRC32_KICK205_37_299_A600      = 0x83028FB5;
constexpr u32 CRC32_KICK205_37_300_A600HD    = 0x64466C2A;
constexpr u32 CRC32_KICK205_37_350_A600HD    = 0x43B0DF7B;

constexpr u32 CRC32_KICK30_39_106_A1200      = 0x6C9B07D2;
constexpr u32 CRC32_KICK30_39_106_A4000      = 0x9E6AC152;

constexpr u32 CRC32_KICK31_40_055_A3000      = 0x3ADDF0D6;
constexpr u32 CRC32_KICK31_40_060_CD32       = 0x1E62D4A5;
constexpr u32 CRC32_KICK31_40_063_A500       = 0xFC24AE0D;
constexpr u32 CRC32_KICK31_40_063_A500_R     = 0x88136CA9;
constexpr u32 CRC32_KICK31_40_068_A1200      = 0x1483A091;
constexpr u32 CRC32_KICK31_40_068_A3000      = 0xEFB239CC;
constexpr u32 CRC32_KICK31_40_068_A4000      = 0xD6BAE334;
constexpr u32 CRC32_KICK31_40_069_A1200      = 0x1D9AA278;
constexpr u32 CRC32_KICK31_40_070_A4000T     = 0x75932C3A;

constexpr u32 CRC32_KICK35_40_071_A4000      = 0xE75A53F4;

constexpr u32 CRC32_CDTV_EXT_ROM_1_0         = 0x42BAA124;
constexpr u32 CRC32_CDTV_EXT_ROM_2_3         = 0x30B54232;
constexpr u32 CRC32_CDTV_EXT_ROM_2_7         = 0xCEAE68D2;

constexpr u32 CRC32_HYP314_46_143_A500       = 0x568F8786;
constexpr u32 CRC32_HYP314_46_143_A1200      = 0xF17FA97F;
constexpr u32 CRC32_HYP314_46_143_A2000      = 0xC25939AC;
constexpr u32 CRC32_HYP314_46_143_A3000      = 0x50C3529C;
constexpr u32 CRC32_HYP314_46_143_A4000      = 0xD47E18FD;
constexpr u32 CRC32_HYP314_46_143_A4000T     = 0x75A2B2A5;
constexpr u32 CRC32_HYP320_47_96_A500        = 0x8173D7B6;
constexpr u32 CRC32_HYP320_47_96_A1200       = 0xBD1FF75E;
constexpr u32 CRC32_HYP320_47_96_A3000       = 0xF3AF46CC;
constexpr u32 CRC32_HYP320_47_96_A4000       = 0x9BB8FC93;
constexpr u32 CRC32_HYP320_47_96_A4000T      = 0x9188A509;
constexpr u32 CRC32_HYP321_47_102_A500       = 0x4F078456;
constexpr u32 CRC32_HYP321_47_102_A1200      = 0x2B653371;
constexpr u32 CRC32_HYP321_47_102_A3000      = 0x0078F607;
constexpr u32 CRC32_HYP321_47_102_A4000      = 0xF3CED3B8;
constexpr u32 CRC32_HYP321_47_102_A4000T     = 0xAF3452EC;
constexpr u32 CRC32_HYP322_47_111_A500       = 0xE4458462;
constexpr u32 CRC32_HYP322_47_111_A1200      = 0x5C40328A;
constexpr u32 CRC32_HYP322_47_111_A3000      = 0x46335B57;
constexpr u32 CRC32_HYP322_47_111_A4000      = 0x4BEA9798;
constexpr u32 CRC32_HYP322_47_111_A4000T     = 0x36BBCD8A;
constexpr u32 CRC32_HYP323_47_115_A500       = 0xE1F50B0B;
constexpr u32 CRC32_HYP323_47_115_A1200      = 0xB18D3B67;
constexpr u32 CRC32_HYP323_47_115_A3000      = 0x74C0B23F;
constexpr u32 CRC32_HYP323_47_115_A4000      = 0xB6A4698E;
constexpr u32 CRC32_HYP323_47_115_A4000T     = 0x588A5E6D;

constexpr u32 CRC32_AROS_54705               = 0x9CE0F009;
constexpr u32 CRC32_AROS_54705_EXT           = 0xE2C7F70A;
constexpr u32 CRC32_AROS_55696               = 0x3F4FCC0A;
constexpr u32 CRC32_AROS_55696_EXT           = 0xF2E52B07;
constexpr u32 CRC32_AROS_1ED13DE6E3          = 0x4CE7C8D6;
constexpr u32 CRC32_AROS_1ED13DE6E3_EXT      = 0xF2A9CDC5;
constexpr u32 CRC32_AROS_20250219            = 0xA3232963;
constexpr u32 CRC32_AROS_20250219_EXT        = 0x9E4253EC;
constexpr u32 CRC32_AROS_20260820            = 0x7ae94477;
constexpr u32 CRC32_AROS_20260820_EXT        = 0x6f9a4ad2;

constexpr u32 CRC32_EMUTOS13                 = 0x7123C1C1;
constexpr u32 CRC32_EMUTOS14                 = 0x9DBDC96A;

constexpr u32 CRC32_DIAG11                   = 0x4C4B5C05;
constexpr u32 CRC32_DIAG12                   = 0x771CD0EA;
constexpr u32 CRC32_DIAG121                  = 0x850209CD;
constexpr u32 CRC32_DIAG13                   = 0x55E2E127;
constexpr u32 CRC32_DIAG20                   = 0x157CE67A;
constexpr u32 CRC32_LOGICA20                 = 0x8484F426;

constexpr u32 CRC32_CPUBLTRO_F8_0_3_2        = 0x169C8FCE;
constexpr u32 CRC32_CPUBLTRO_FC_0_3_2        = 0x86CB5B1B;


//
// Enumerations
//

/* Rom types. Only MEGA65 is C64-specific and only the AMIGA_ entries are
 * Amiga-specific; a Rom of any type is described by the same RomTraits.
 *
 * UNKNOWN is the zero value, so a RomTraits that leaves 'type' out (e.g. a
 * fallback for an unrecognized Rom) gets it by default.
 *
 * AMIGA_KICKSTART is the standard type for Amiga Roms; AMIGA_BOOT marks the
 * two A1000 boot Roms, which used to carry a dedicated 'boot' flag; AMIGA_EXTROM
 * marks a Kickstart extension Rom (loaded alongside a Kickstart, not in place
 * of it) -- currently the five AROS extension Roms. The CDTV Extended Rom is
 * also an extension Rom in this sense but is still tagged AMIGA_KICKSTART.
 * AMIGA_DEMO marks a chipset test/demo tool rather than a real Kickstart --
 * currently the two OCS CPUBLTRO Roms, formerly told apart by RomVendor::DEMO.
 * AMIGA_DIAG marks a diagnostic Rom -- currently the five Amiga DiagROM
 * versions, formerly told apart by RomVendor::DIAG.
 */
enum class RomType : long
{
    UNKNOWN,
    C64_BASIC,
    C64_CHAR,
    C64_KERNAL,
    C64_VC1541,
    AMIGA_KICKSTART,
    AMIGA_BOOT,
    AMIGA_EXTROM,
    AMIGA_DEMO,
    AMIGA_DIAG
};

struct RomTypeEnum : utl::Reflectable<RomTypeEnum, RomType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(RomType::AMIGA_DIAG);

    static const char *_key(RomType value)
    {
        switch (value) {

            case RomType::UNKNOWN:         return "UNKNOWN";
            case RomType::C64_BASIC:       return "C64_BASIC";
            case RomType::C64_CHAR:        return "C64_CHAR";
            case RomType::C64_KERNAL:      return "C64_KERNAL";
            case RomType::C64_VC1541:      return "C64_VC1541";
            case RomType::AMIGA_KICKSTART: return "AMIGA_KICKSTART";
            case RomType::AMIGA_BOOT:      return "AMIGA_BOOT";
            case RomType::AMIGA_EXTROM:    return "AMIGA_EXTROM";
            case RomType::AMIGA_DEMO:      return "AMIGA_DEMO";
            case RomType::AMIGA_DIAG:      return "AMIGA_DIAG";
        }
        return "???";
    }

    static const char *help(RomType value)
    {
        switch (value) {

            case RomType::UNKNOWN:         return "Unknown ROM";
            case RomType::C64_BASIC:       return "Basic ROM";
            case RomType::C64_CHAR:        return "Character ROM";
            case RomType::C64_KERNAL:      return "Kernal ROM";
            case RomType::C64_VC1541:      return "Floppy Drive ROM";
            case RomType::AMIGA_KICKSTART: return "Kickstart ROM";
            case RomType::AMIGA_BOOT:      return "Boot ROM";
            case RomType::AMIGA_EXTROM:    return "Kickstart Extension ROM";
            case RomType::AMIGA_DEMO:      return "Demo/Test ROM";
            case RomType::AMIGA_DIAG:      return "Diagnostic ROM";
        }
        return "";
    }
};

/* Rom vendors. UNKNOWN is the zero value, so a database entry that leaves
 * 'vendor' out gets it by default -- name the vendor explicitly wherever it
 * is known. Demo/test Roms are told apart by RomType::AMIGA_DEMO and
 * diagnostic Roms by RomType::AMIGA_DIAG, not by a vendor of their own --
 * they use whichever real vendor applies (OTHER for homebrew ones without a
 * formal vendor).
 */
enum class RomVendor
{
    UNKNOWN,
    COMMODORE,
    MEGA65,
    AROS,
    HYPERION,
    EMUTOS,
    OTHER
};

struct RomVendorEnum : utl::Reflectable<RomVendorEnum, RomVendor> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(RomVendor::OTHER);

    static const char *_key(RomVendor value)
    {
        switch (value) {

            case RomVendor::UNKNOWN:    return "UNKNOWN";
            case RomVendor::COMMODORE:  return "COMMODORE";
            case RomVendor::MEGA65:     return "MEGA65";
            case RomVendor::AROS:       return "AROS";
            case RomVendor::HYPERION:   return "HYPERION";
            case RomVendor::EMUTOS:     return "EMUTOS";
            case RomVendor::OTHER:      return "OTHER";
        }
        return "???";
    }

    static const char *help(RomVendor value)
    {
        return "";
    }
};


//
// Structures
//

/* A Rom's release date, precise to the month at best. A database entry for
 * which only the year is known leaves 'month' at 0; one for which no date is
 * known at all leaves both fields at 0.
 */
typedef struct {

    u16 year;
    u8 month;
}
RomDate;

// Formats a RomDate for display, e.g. "April 2025", "2022", or "" if unknown
string RomDateToString(RomDate date);

/* The machine a Rom belongs to. Unlike the former RomModel bitmask, this does
 * not distinguish individual hardware models (SX-64, A1200, ...) -- RomType
 * already says what kind of Rom it is (Kickstart, boot Rom, drive firmware,
 * ...), and the C64/Amiga split is all that's left worth keeping structured.
 * AtariST is a placeholder -- no known Rom carries it yet.
 */
enum class RomPlatform
{
    C64,
    Amiga,
    AtariST
};

struct RomPlatformEnum : utl::Reflectable<RomPlatformEnum, RomPlatform> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(RomPlatform::AtariST);

    static const char *_key(RomPlatform value)
    {
        switch (value) {

            case RomPlatform::C64:     return "C64";
            case RomPlatform::Amiga:   return "AMIGA";
            case RomPlatform::AtariST: return "ATARIST";
        }
        return "???";
    }

    static const char *help(RomPlatform value)
    {
        switch (value) {

            case RomPlatform::C64:     return "Commodore 64";
            case RomPlatform::Amiga:   return "Amiga";
            case RomPlatform::AtariST: return "Atari ST";
        }
        return "";
    }
};

/* Properties a Rom can have, one bit each. A database entry ORs together
 * every flag that applies; 0 means none of them do.
 */
namespace RomFlags {

constexpr u32 Verified   = 1u << 0;   // Dump checked against a trusted source
constexpr u32 Corrupted  = 1u << 1;   // Dump is known to be damaged
constexpr u32 Patched    = 1u << 2;   // Modified from the original Rom
constexpr u32 Relocated  = 1u << 3;   // Built to run from a non-standard address

}

/* Describes a single known Rom. Cores fill in whichever hash they identify
 * Roms by -- the C64 matches on 'fnv', the Amiga on 'crc' -- and leave the
 * other at zero.
 */
typedef struct {

    u64 fnv;
    u32 crc;

    const char *title;
    const char *revision;
    RomDate released;
    RomPlatform platform;

    RomVendor vendor;
    RomType type;
    u32 flags;
}
RomTraits;

}
