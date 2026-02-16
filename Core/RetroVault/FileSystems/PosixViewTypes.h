// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DeviceTypes.h"

namespace retro::vault {

using namespace utl;

namespace posix {

static constexpr u32 IRWXU = 0000700;   // RWX mask for owner
static constexpr u32 IRUSR = 0000400;   // R for owner
static constexpr u32 IWUSR = 0000200;   // W for owner
static constexpr u32 IXUSR = 0000100;   // X for owner

static constexpr u32 IRWXG = 0000070;   // RWX mask for group
static constexpr u32 IRGRP = 0000040;   // R for group
static constexpr u32 IWGRP = 0000020;   // W for group
static constexpr u32 IXGRP = 0000010;   // X for group

static constexpr u32 IRWXO = 0000007;   // RWX mask for other
static constexpr u32 IROTH = 0000004;   // R for other
static constexpr u32 IWOTH = 0000002;   // W for other
static constexpr u32 IXOTH = 0000001;   // X for other

}

struct FSPosixAttr {

    // Meta data

    isize size;         // File size in bytes
    isize blocks;       // Number of occupied blocks
    u32 prot;           // Protection flags
    bool isDir;         // Is it a directory?

    // Access times

    time_t btime;       // Time of birth
    time_t atime;       // Time of last access
    time_t mtime;       // Time of last data modification
    time_t ctime;       // Time of last status change
};

struct FSPosixStat {

    // Meta data

    string name;
    isize bsize;        // Block size
    isize blocks;       // File system capacity in blocks

    // Usage information

    isize freeBlocks;   // Available blocks
    isize usedBlocks;   // Occupied blocks
    isize cachedBlocks; // Total number of cached blocks
    isize dirtyBlocks;  // Number of modified cached blocks
    
    // Access times

    time_t btime;       // Time of birth
    time_t mtime;       // Time of last data modification

    // Access statistics

    isize generation;   // File system generation counter
};

enum class HandleRef : isize {};

struct Handle {

    HandleRef id;       // Unique indentifier
    BlockNr node;       // File root node
    isize offset;       // I/O offset
    u32 flags;          // Open mode
};

}
