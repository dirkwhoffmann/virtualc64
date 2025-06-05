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

#include "Infrastructure/ThreadTypes.h"
#include "Components/C64Types.h"

namespace vc64 {

//
// Enumerations
//

enum class DebugFlag
{
    // General
    XFILES,            ///< Report paranormal activity
    CNF_DEBUG,         ///< Debug configuration code
    DEF_DEBUG,         ///< Debug the user defaults storage

    // Emulator
    RUN_DEBUG,         ///< Debug the run loop and component states
    TIM_DEBUG,         ///< Debug thread synchronization
    WARP_DEBUG,        ///< Debug warp mode
    CMD_DEBUG,         ///< Debug the command queue
    MSG_DEBUG,         ///< Debug the message queue
    SNP_DEBUG,         ///< Debug snapshots

    // Run ahead
    RUA_DEBUG,         ///< Inform about run-ahead activity
    RUA_CHECKSUM,      ///< Verify the integrity of the run-ahead instance
    RUA_ON_STEROIDS,   ///< Update the run-ahead instance in every frame

    // CPU
    CPU_DEBUG,         ///< Debug the CPU
    IRQ_DEBUG,         ///< Debug interrupts

    // Memory
    MEM_DEBUG,         ///< Debug memory accesses

    // CIAs
    CIA_DEBUG,         ///< Debug the Complex Interface Adapters
    CIAREG_DEBUG,      ///< Debug the CIA register accesses

    // VICII
    VICII_DEBUG,       ///< Debug the Video Interface Controller
    VICII_REG_DEBUG,   ///< Debug VICII register accesses
    VICII_SAFE_MODE,   ///< Disable performance optimizations
    VICII_STATS,       ///< Collect statistical information
    RASTERIRQ_DEBUG,   ///< Debug raster interrupts

    // SID
    SID_DEBUG,         ///< Debug the Sound Interface Device
    SID_EXEC,          ///< Debug SID execution
    SIDREG_DEBUG,      ///< Debug SID register accesses
    AUDBUF_DEBUG,      ///< Debug the audio buffers
    AUDVOL_DEBUG,      ///< Debug the audio volumes

    // Drive
    VIA_DEBUG,         ///< Debug the Versatile Interface Adapter
    PIA_DEBUG,         ///< Debug the Peripheral Interface Adapter
    SER_DEBUG,         ///< Debug the serial port (IEC bus)
    DSK_DEBUG,         ///< Debug the disk controller execution
    DSKCHG_DEBUG,      ///< Debug the disk change procedure
    GCR_DEBUG,         ///< Debug the GCR encoder and decoder
    FS_DEBUG,          ///< Debug files system related code
    PAR_DEBUG,         ///< Debug the parallel drive cable

    // Media
    CRT_DEBUG,         ///< Debug cartridges
    FILE_DEBUG,        ///< Debug the handling of media files

    // Peripherals
    JOY_DEBUG,         ///< Debug the joysticks
    DRV_DEBUG,         ///< Debug the floppy drives
    TAP_DEBUG,         ///< Debug the datasette
    KBD_DEBUG,         ///< Debug the keyboard
    PRT_DEBUG,         ///< Debug the control port and connected devices
    EXP_DEBUG,         ///< Debug the expansion port
    USR_DEBUG,         ///< User port

    // Other components
    REC_DEBUG,         ///< Debug the screen recorder
    REU_DEBUG,         ///< Debug the REU memory expansion
    SCK_DEBUG,         ///< Debug the socket interface
    SRV_DEBUG,         ///< Debug the remote servers

    //! Forced error condition
    FORCE_LAUNCH_ERROR,
    FORCE_ROM_MISSING,
    FORCE_MEGA64_MISMATCH,
    FORCE_SNAP_TOO_OLD,
    FORCE_SNAP_TOO_NEW,
    FORCE_SNAP_IS_BETA,
    FORCE_SNAP_CORRUPTED,
    FORCE_CRT_UNKNOWN,
    FORCE_CRT_UNSUPPORTED,
    FORCE_RECORDING_ERROR,
    FORCE_NO_FFMPEG
};

struct DebugFlagEnum : Reflection<DebugFlagEnum, DebugFlag>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(DebugFlag::FORCE_NO_FFMPEG);

    static const char *_key(DebugFlag value)
    {
        switch (value) {

            case DebugFlag::XFILES:                   return "XFILES";
            case DebugFlag::CNF_DEBUG:                return "CNF_DEBUG";
            case DebugFlag::DEF_DEBUG:                return "DEF_DEBUG";

                // Emulator
            case DebugFlag::RUN_DEBUG:                return "RUN_DEBUG";
            case DebugFlag::TIM_DEBUG:                return "TIM_DEBUG";
            case DebugFlag::WARP_DEBUG:               return "WARP_DEBUG";
            case DebugFlag::CMD_DEBUG:                return "CMD_DEBUG";
            case DebugFlag::MSG_DEBUG:                return "MSG_DEBUG";
            case DebugFlag::SNP_DEBUG:                return "SNP_DEBUG";

                // Run-ahead
            case DebugFlag::RUA_DEBUG:                return "RUA_DEBUG";
            case DebugFlag::RUA_CHECKSUM:             return "RUA_CHECKSUM";
            case DebugFlag::RUA_ON_STEROIDS:          return "RUA_ON_STEROIDS";

                // CPU
            case DebugFlag::CPU_DEBUG:                return "CPU_DEBUG";
            case DebugFlag::IRQ_DEBUG:                return "IRQ_DEBUG";

                // Memory
            case DebugFlag::MEM_DEBUG:                return "MEM_DEBUG";

                // CIAs
            case DebugFlag::CIA_DEBUG:                return "CIA_DEBUG";
            case DebugFlag::CIAREG_DEBUG:             return "CIAREG_DEBUG";

                // VICII
            case DebugFlag::VICII_DEBUG:              return "VICII_DEBUG";
            case DebugFlag::VICII_REG_DEBUG:          return "VICII_REG_DEBUG";
            case DebugFlag::VICII_SAFE_MODE:          return "VICII_SAFE_MODE";
            case DebugFlag::VICII_STATS:              return "VICII_STATS";
            case DebugFlag::RASTERIRQ_DEBUG:          return "RASTERIRQ_DEBUG";

                // SID
            case DebugFlag::SID_DEBUG:                return "SID_DEBUG";
            case DebugFlag::SID_EXEC:                 return "SID_EXEC";
            case DebugFlag::SIDREG_DEBUG:             return "SIDREG_DEBUG";
            case DebugFlag::AUDBUF_DEBUG:             return "AUDBUF_DEBUG";
            case DebugFlag::AUDVOL_DEBUG:             return "AUDVOL_DEBUG";

                // Drive
            case DebugFlag::VIA_DEBUG:                return "VIA_DEBUG";
            case DebugFlag::PIA_DEBUG:                return "PIA_DEBUG";
            case DebugFlag::SER_DEBUG:                return "SER_DEBUG";
            case DebugFlag::DSK_DEBUG:                return "DSK_DEBUG";
            case DebugFlag::DSKCHG_DEBUG:             return "DSKCHG_DEBUG";
            case DebugFlag::GCR_DEBUG:                return "GCR_DEBUG";
            case DebugFlag::FS_DEBUG:                 return "FS_DEBUG";
            case DebugFlag::PAR_DEBUG:                return "PAR_DEBUG";

                // Media
            case DebugFlag::CRT_DEBUG:                return "CRT_DEBUG";
            case DebugFlag::FILE_DEBUG:               return "FILE_DEBUG";

                // Peripherals
            case DebugFlag::JOY_DEBUG:                return "JOY_DEBUG";
            case DebugFlag::DRV_DEBUG:                return "DRV_DEBUG";
            case DebugFlag::TAP_DEBUG:                return "TAP_DEBUG";
            case DebugFlag::KBD_DEBUG:                return "KBD_DEBUG";
            case DebugFlag::PRT_DEBUG:                return "PRT_DEBUG";
            case DebugFlag::EXP_DEBUG:                return "EXP_DEBUG";
            case DebugFlag::USR_DEBUG:                return "USR_DEBUG";

                // Other components
            case DebugFlag::REC_DEBUG:                return "REC_DEBUG";
            case DebugFlag::REU_DEBUG:                return "REU_DEBUG";
            case DebugFlag::SCK_DEBUG:                return "SCK_DEBUG";
            case DebugFlag::SRV_DEBUG:                return "SRV_DEBUG";

                // Forced error conditions
            case DebugFlag::FORCE_LAUNCH_ERROR:       return "FORCE_LAUNCH_ERROR";
            case DebugFlag::FORCE_ROM_MISSING:        return "FORCE_ROM_MISSING";
            case DebugFlag::FORCE_MEGA64_MISMATCH:    return "FORCE_MEGA64_MISMATCH";
            case DebugFlag::FORCE_SNAP_TOO_OLD:       return "FORCE_SNAP_TOO_OLD";
            case DebugFlag::FORCE_SNAP_TOO_NEW:       return "FORCE_SNAP_TOO_NEW";
            case DebugFlag::FORCE_SNAP_IS_BETA:       return "FORCE_SNAP_IS_BETA";
            case DebugFlag::FORCE_SNAP_CORRUPTED:     return "FORCE_SNAP_CORRUPTE";
            case DebugFlag::FORCE_CRT_UNKNOWN:        return "FORCE_CRT_UNKNOWN";
            case DebugFlag::FORCE_CRT_UNSUPPORTED:    return "FORCE_CRT_UNSUPPORTED";
            case DebugFlag::FORCE_RECORDING_ERROR:    return "FORCE_RECORDING_ERROR";
            case DebugFlag::FORCE_NO_FFMPEG:          return "FORCE_NO_FFMPEG";
        }
        return "???";
    }

    static const char *help(DebugFlag value)
    {
        switch (value) {

                //General
            case DebugFlag::XFILES:                   return "Report paranormal activity";
            case DebugFlag::CNF_DEBUG:                return "Configuration";
            case DebugFlag::DEF_DEBUG:                return "User defaults";

                // Emulator
            case DebugFlag::RUN_DEBUG:                return "Run loop, component states";
            case DebugFlag::TIM_DEBUG:                return "Thread synchronization";
            case DebugFlag::WARP_DEBUG:               return "Warp mode";
            case DebugFlag::CMD_DEBUG:                return "Command queue";
            case DebugFlag::MSG_DEBUG:                return "Message queue";
            case DebugFlag::SNP_DEBUG:                return "Serializing (snapshots)";

                // Run ahead
            case DebugFlag::RUA_DEBUG:                return "Inform about run-ahead activity";
            case DebugFlag::RUA_CHECKSUM:             return "Verify the integrity of the run-ahead instance";
            case DebugFlag::RUA_ON_STEROIDS:          return "Update the run-ahead instance in every frame";

                // CPU
            case DebugFlag::CPU_DEBUG:                return "CPU";
            case DebugFlag::IRQ_DEBUG:                return "Interrupts";

                // Memory
            case DebugFlag::MEM_DEBUG:                return "RAM and ROM";

                // CIAs
            case DebugFlag::CIA_DEBUG:                return "Complex Interface Adapter";
            case DebugFlag::CIAREG_DEBUG:             return "CIA registers";

                // VICII
            case DebugFlag::VICII_DEBUG:              return "Video Interface Controller";
            case DebugFlag::VICII_REG_DEBUG:          return "VICII registers";
            case DebugFlag::VICII_SAFE_MODE:          return "Disable performance optimizations";
            case DebugFlag::VICII_STATS:              return "Collect statistical information";
            case DebugFlag::RASTERIRQ_DEBUG:          return "Raster interrupt";

                // SID
            case DebugFlag::SID_DEBUG:                return "Sound Interface Device";
            case DebugFlag::SID_EXEC:                 return "Sound Interface Device (Execution)";
            case DebugFlag::SIDREG_DEBUG:             return "Sound Interface Device (Registers)";
            case DebugFlag::AUDBUF_DEBUG:             return "Audio buffers";
            case DebugFlag::AUDVOL_DEBUG:             return "Audio volumes";
                
                // Drive
            case DebugFlag::VIA_DEBUG:                return "Versatile Interface Adapter";
            case DebugFlag::PIA_DEBUG:                return "Peripheral Interface Adapter";
            case DebugFlag::SER_DEBUG:                return "Serial port (IEC bus)";
            case DebugFlag::DSK_DEBUG:                return "Disk controller execution";
            case DebugFlag::DSKCHG_DEBUG:             return "Disk change procedure";
            case DebugFlag::GCR_DEBUG:                return "Disk encoding / decoding";
            case DebugFlag::FS_DEBUG:                 return "File System Classes";
            case DebugFlag::PAR_DEBUG:                return "Parallel cable";

                // Media
            case DebugFlag::CRT_DEBUG:                return "Cartridges";
            case DebugFlag::FILE_DEBUG:               return "Media files (D64,T64,...)";

                // Peripherals
            case DebugFlag::JOY_DEBUG:                return "Joystick";
            case DebugFlag::DRV_DEBUG:                return "Floppy drives";
            case DebugFlag::TAP_DEBUG:                return "Datasette";
            case DebugFlag::KBD_DEBUG:                return "Keyboard";
            case DebugFlag::PRT_DEBUG:                return "Control ports and connected devices";
            case DebugFlag::EXP_DEBUG:                return "Expansion port";
            case DebugFlag::USR_DEBUG:                return "User port";

                // Other components
            case DebugFlag::REC_DEBUG:                return "Screen recorder";
            case DebugFlag::REU_DEBUG:                return "REU memory expansion";
            case DebugFlag::SCK_DEBUG:                return "Sockets";
            case DebugFlag::SRV_DEBUG:                return "Remote servers";

                // Forced error conditions
            case DebugFlag::FORCE_LAUNCH_ERROR:       return "";
            case DebugFlag::FORCE_ROM_MISSING:        return "";
            case DebugFlag::FORCE_MEGA64_MISMATCH:    return "";
            case DebugFlag::FORCE_SNAP_TOO_OLD:       return "";
            case DebugFlag::FORCE_SNAP_TOO_NEW:       return "";
            case DebugFlag::FORCE_SNAP_IS_BETA:       return "";
            case DebugFlag::FORCE_SNAP_CORRUPTED:     return "";
            case DebugFlag::FORCE_CRT_UNKNOWN:        return "";
            case DebugFlag::FORCE_CRT_UNSUPPORTED:    return "";
            case DebugFlag::FORCE_RECORDING_ERROR:    return "";
            case DebugFlag::FORCE_NO_FFMPEG:          return "";
        }
        return "???";
    }    
};


//
// Structures
//

//! The current emulator state
typedef struct
{
    ExecState state;        ///< The current emulator state
    bool powered;           ///< Indicates if the emulator is powered on
    bool paused;            ///< Indicates if emulation is paused
    bool running;           ///< Indicates if the emulator is running
    bool suspended;         ///< Indicates if the emulator is in suspended state
    bool warping;           ///< Indicates if warp mode is currently on
    bool tracking;          ///< Indicates if track mode is enabled
}
EmulatorInfo;

//! Collected run-time data
typedef struct
{
    double cpuLoad;         ///< Measured CPU load
    double fps;             ///< Measured frames per seconds
    isize resyncs;          ///< Number of out-of-sync conditions
    isize clones;           ///< Number of created run-ahead instances
}
EmulatorStats;

}
