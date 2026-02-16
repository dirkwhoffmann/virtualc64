// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/PosixViewTypes.h"

namespace retro::vault {

class PosixView {
    
public:
    
    virtual ~PosixView() = default;
    
    
    //
    // Configuring
    //
    
    virtual bool isWriteProtected() const noexcept = 0;
    virtual void writeProtect(bool yesno) noexcept = 0;
    
    
    //
    // Obtaining meta information
    //
    
    // Queries information about the file system
    virtual FSPosixStat stat() const noexcept = 0;
    
    // Queries information about a specific file
    virtual FSPosixAttr attr(const fs::path &path) const = 0;
    
    
    //
    // Working with directories
    //
    
public:
    
    // Creates a directory
    virtual void mkdir(const fs::path &path) = 0;
    
    // Removes a directory
    virtual void rmdir(const fs::path &path) = 0;
    
    // Returns the contents of a directory
    virtual std::vector<string> readDir(const fs::path &path) const = 0;
    
    
    //
    // Working with files
    //
    
public:
    
    // Opens a file
    virtual HandleRef open(const fs::path &path, u32 flags) = 0;
    
    // Closes a file
    virtual void close(HandleRef handle) = 0;
    
    // Creates a new file
    virtual void create(const fs::path &path) = 0;
    
    // Removes a file from its directory
    virtual void unlink(const fs::path &path) = 0;
    
    // Moves the file to a different location
    virtual void move(const fs::path &oldPath, const fs::path &newPath) = 0;
    
    // Changes the size of a file
    virtual void resize(const fs::path &path, isize size) = 0;
    
    // Moves the read/write pointer
    virtual isize lseek(HandleRef ref, isize offset, u16 whence = 0) = 0;
    
    // Reads data from a file
    virtual isize read(HandleRef ref, std::span<u8> buffer) = 0;
    
    // Writes data to a file
    virtual isize write(HandleRef ref, std::span<const u8> buffer) = 0;
    
    // Changes file permissions
    virtual void chmod(const fs::path &path, u32 mode) = 0;
    
    
    //
    // Working with caches
    //
    
    // Writes dirty cache blocks back to the block device
    virtual void flush() = 0;

    // Invalidates all cache entries
    virtual void invalidate() = 0;
};

}
