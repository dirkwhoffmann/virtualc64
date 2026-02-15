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
// Type aliases
//

typedef i64 Cycle;


//
// Enumerations
//

/*
enum class Compressor : long
{
    NONE,
    GZIP,
    LZ4,
    RLE2,
    RLE3
};

struct CompressorEnum : Reflection<CompressorEnum, Compressor>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Compressor::RLE3);

    static const char *_key(Compressor value)
    {
        switch (value) {

            case Compressor::NONE:  return "NONE";
            case Compressor::GZIP:  return "GZIP";
            case Compressor::RLE2:  return "RLE2";
            case Compressor::RLE3:  return "RLE3";
            case Compressor::LZ4:   return "LZ4";
        }
        return "???";
    }
    static const char *help(Compressor value)
    {
        switch (value) {

            case Compressor::NONE:  return "No compression";
            case Compressor::GZIP:  return "Gzip compression";
            case Compressor::RLE2:  return "Run-length encoding (2)";
            case Compressor::RLE3:  return "Run-length encoding (3)";
            case Compressor::LZ4:   return "LZ4 compression";
        }
        return "???";
    }
};
*/

/// C64 model
enum class ConfigScheme : long
{
    PAL,      ///< C64 (PAL)
    PAL_II,   ///< C64 II (PAL)
    PAL_OLD,  ///< Early C64 (PAL)
    NTSC,     ///< C64 (NTSC)
    NTSC_II,  ///< C64 II (NTSC)
    NTSC_OLD  ///< Early C64 (NTSC)
};

struct ConfigSchemeEnum : Reflection<ConfigSchemeEnum, ConfigScheme> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(ConfigScheme::NTSC_OLD);

    static const char *_key(ConfigScheme value)
    {
        switch (value) {

            case ConfigScheme::PAL:       return "PAL";
            case ConfigScheme::PAL_II:    return "PAL_II";
            case ConfigScheme::PAL_OLD:   return "PAL_OLD";
            case ConfigScheme::NTSC:      return "NTSC";
            case ConfigScheme::NTSC_II:   return "NTSC_II";
            case ConfigScheme::NTSC_OLD:  return "NTSC_OLD";
        }
        return "???";
    }
    
    static const char *help(ConfigScheme value)
    {
        return "TODO";
    }
};

enum class Warp : long
{
    AUTO,
    NEVER,
    ALWAYS
};

struct WarpEnum : Reflection<WarpEnum, Warp>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Warp::ALWAYS);

    static const char *_key(Warp value)
    {
        switch (value) {

            case Warp::AUTO:     return "AUTO";
            case Warp::NEVER:    return "NEVER";
            case Warp::ALWAYS:   return "ALWAYS";
        }
        return "???";
    }
    
    static const char *help(Warp value)
    {
        return "";
    }
};

enum EventSlot : long
{
    // Primary slots
    SLOT_CIA1,                      // CIA 1 execution
    SLOT_CIA2,                      // CIA 2 execution
    SLOT_SEC,                       // Enables secondary slots

    // Secondary slots
    SLOT_SER,                       // Serial port (IEC bus)
    SLOT_DAT,                       // Datasette execution
    SLOT_TER,                       // Enables tertiary slots

    // Tertiary slots
    SLOT_EXP,                       // Expansion port
    SLOT_TXD,                       // Serial data out (RS232)
    SLOT_RXD,                       // Serial data in (RS232)
    SLOT_MOT,                       // Datasette motor
    SLOT_DC8,                       // Disk change (Drive 8)
    SLOT_DC9,                       // Disk change (Drive 9)
    SLOT_SNP,                       // Snapshots
    SLOT_RSH,                       // Retro Shell
    SLOT_KEY,                       // Auto-typing
    SLOT_SRV,                       // Remote server manager
    SLOT_DBG,                       // Debugging (Regression tester)
    SLOT_ALA,                       // Alarms (set by the GUI)
    SLOT_INS,                       // Handles periodic calls to inspect()

    SLOT_COUNT
};
// typedef SLOT EventSlot;

struct EventSlotEnum : Reflection<EventSlotEnum, EventSlot>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = SLOT_COUNT - 1;

    static long count() { return maxVal; }
    
    static const char *_key(long value)
    {
        switch (value) {

            case SLOT_CIA1:     return "CIA1";
            case SLOT_CIA2:     return "CIA2";
            case SLOT_SEC:      return "SEC";

            case SLOT_SER:      return "SER";
            case SLOT_DAT:      return "DAT";
            case SLOT_TER:      return "TER";

            case SLOT_EXP:      return "EXP";
            case SLOT_TXD:      return "TXD";
            case SLOT_RXD:      return "RXD";
            case SLOT_MOT:      return "MOT";
            case SLOT_DC8:      return "DC8";
            case SLOT_DC9:      return "DC9";
            case SLOT_SNP:      return "SNP";
            case SLOT_RSH:      return "RSH";
            case SLOT_KEY:      return "KEY";
            case SLOT_SRV:      return "SRV";
            case SLOT_DBG:      return "DBG";
            case SLOT_ALA:      return "ALA";
            case SLOT_INS:      return "INS";

            case SLOT_COUNT:    return "???";
        }
        return "???";
    }
    
    static const char *help(EventSlot value)
    {
        switch (value) {

            case SLOT_CIA1:     return "CIA 1";
            case SLOT_CIA2:     return "CIA 2";
            case SLOT_SEC:      return "Next Secondary Event";

            case SLOT_SER:      return "Serial Port";
            case SLOT_DAT:      return "Datasette Data";
            case SLOT_TER:      return "Next Tertiary Event";

            case SLOT_EXP:      return "Expansion Port";
            case SLOT_TXD:      return "RS232 Out";
            case SLOT_RXD:      return "RS232 In";
            case SLOT_MOT:      return "Datasette Motor";
            case SLOT_DC8:      return "Disk Change Drive 8";
            case SLOT_DC9:      return "Disk Change Drive 9";
            case SLOT_SNP:      return "Snapshots";
            case SLOT_RSH:      return "Retro Shell";
            case SLOT_KEY:      return "Auto Typing";
            case SLOT_SRV:      return "Server Daemon";
            case SLOT_DBG:      return "Debugger";
            case SLOT_ALA:      return "Alarms";
            case SLOT_INS:      return "Inspector";

            case SLOT_COUNT:    return "???";
        }
        return "???";
    }
};

enum EventID : u8 // _i8(EventID)
{
    EVENT_NONE          = 0,

    //
    // Events in the primary event table
    //

    // CIA slots
    CIA_EXECUTE         = 1,
    CIA_WAKEUP,
    CIA_EVENT_COUNT,

    // SEC slot
    SEC_TRIGGER         = 1,
    SEC_EVENT_COUNT,

    //
    // Events in secondary event table
    //

    // SER slot
    SER_UPDATE          = 1,
    SER_EVENT_COUNT,

    // DAT slot
    DAT_EXECUTE         = 1,
    DAT_EVENT_COUNT,

    // SEC slot
    TER_TRIGGER         = 1,
    TER_EVENT_COUNT,

    //
    // Events in tertiary event table
    //

    // Expansion port
    EXP_REU_INITIATE    = 1,
    EXP_REU_PREPARE,
    EXP_REU_STASH,
    EXP_REU_FETCH,
    EXP_REU_SWAP,
    EXP_REU_VERIFY,
    EXP_REU_AUTOLOAD,
    EXP_REU_FINALIZE,
    EXP_EVENT_COUNT,

    // Serial data out (RS232)
    TXD_BIT             = 1,
    TXD_EVENT_COUNT,

    // Serial data in (RS232)
    RXD_BIT             = 1,
    RXD_EVENT_COUT,

    // Datasette motor
    MOT_START           = 1,
    MOT_STOP,
    MOT_EVENT_COUNT,

    // Disk change
    DCH_INSERT          = 1,
    DCH_EJECT,
    DCH_EVENT_COUNT,

    // Snapshots
    SNP_TAKE             = 1,
    SNP_EVENT_COUNT,

    // Retro shell
    RSH_WAKEUP          = 1,
    RSH_EVENT_COUNT,

    // Auto typing
    KEY_AUTO_TYPE       = 1,
    KEY_EVENT_COUNT,

    // Remote server manager
    SRV_LAUNCH_DAEMON   = 1,
    SRV_EVENT_COUNT,

    // Alarm event slot
    DBG_WATCHDOG        = 1,
    DBG_EVENT_COUNT,

    // Alarm event slot
    ALA_TRIGGER         = 1,
    ALA_EVENT_COUNT,

    // Inspector slot
    INS_INSPECT         = 1,
    INS_EVENT_COUNT
};


//
// Structures
//

typedef struct
{
    //! After a reset, the emulator runs in warp mode for this amout of seconds
    isize warpBoot;
    
    //! Warp mode
    Warp warpMode;
    
    //! Emulator speed in percent (100 is native speed)
    isize speedBoost;
    
    //! Vertical Synchronization
    bool vsync;
    
    //! Number of run-ahead frames (0 = run-ahead is disabled)
    isize runAhead;
}
C64Config;

typedef struct
{
    u32 crc32;
    u64 fnv64;

    const char *title;
    const char *subtitle;
    const char *revision;

    bool isCommodoreRom;
    bool isPatchedRom;
    bool isMega65Rom;
}
RomInfo;

typedef struct
{
    EventSlot slot;
    EventID eventId;
    const char *eventName;

    // Trigger cycle of the event
    Cycle trigger;
    Cycle triggerRel;

    // Trigger frame relative to the current frame
    long frameRel;

    // The trigger cycle translated to a beam position.
    long vpos;
    long hpos;
}
EventSlotInfo;

typedef struct
{
    Cycle cpuProgress;
    Cycle cia1Progress;
    Cycle cia2Progress;
    i64 frame;
    long vpos;
    long hpos;

    // Events
    EventSlotInfo slotInfo[SLOT_COUNT];
}
C64Info;

typedef u32 RunLoopFlags;

namespace RL {

constexpr u32 STOP          = (1 << 0);
constexpr u32 WARP_ON       = (1 << 1);
constexpr u32 WARP_OFF      = (1 << 2);
constexpr u32 BREAKPOINT    = (1 << 3);
constexpr u32 WATCHPOINT    = (1 << 4);
constexpr u32 CPU_JAM       = (1 << 5);
constexpr u32 SINGLE_STEP   = (1 << 6);
constexpr u32 SINGLE_CYCLE  = (1 << 7);
constexpr u32 FINISH_LINE   = (1 << 8);
constexpr u32 FINISH_FRAME  = (1 << 9);
}

}
