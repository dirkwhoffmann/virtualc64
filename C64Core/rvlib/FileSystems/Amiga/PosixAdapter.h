// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/PosixView.h"
#include "FileSystems/Amiga/FileSystem.h"
#include <fcntl.h>

namespace retro::vault::amiga {

using retro::vault::PosixView;

struct NodeMeta {

    // Number of directory entries
    isize linkCount = 1;

    // All open handles referencing this node
    std::unordered_set<HandleRef> openHandles;

    // File cache
    Buffer<u8> cache;

    // Returns the number of open handles
    isize openCount() { return (isize)openHandles.size(); };
};

class PosixAdapter : public PosixView {

    // Write protection flag
    bool wp = false;
    
    // The wrapped file system
    FileSystem &fs;

    // Contracts
    FSRequire require = FSRequire(fs);

    // Metadata for nodes indexed by block number
    std::unordered_map<BlockNr, NodeMeta> meta;

    // Active file handles
    std::unordered_map<HandleRef, Handle> handles;

    // Handle ID generator
    isize nextHandle{3};

public:

    explicit PosixAdapter(FileSystem &fs);

    
    //
    // Configuring
    //
 
public:
    
    bool isWriteProtected() const noexcept override { return wp; }
    void writeProtect(bool yesno) noexcept override { wp = yesno; }
    
    
    //
    // Querying statistics and properties
    //

public:

    // Queries information about the file system
    FSPosixStat stat() const noexcept override;

    // Queries information about a specific file
    FSPosixAttr attr(const fs::path &path) const override;


    //
    // Managing metadata
    //

private:

    // Returns a pointer to the meta struct (may be nullptr)
    NodeMeta *getMeta(BlockNr nr);
    NodeMeta *getMeta(const FSBlock &block) { return getMeta(block.nr); }

    // Returns a reference to the meta struct (on-the-fly creation)
    NodeMeta &ensureMeta(BlockNr nr);
    NodeMeta &ensureMeta(const FSBlock &block) { return ensureMeta(block.nr); }
    NodeMeta &ensureMeta(HandleRef ref);


    //
    // Working with directories
    //

public:

    // Creates a directory
    void mkdir(const fs::path &path) override;

    // Removes a directory
    void rmdir(const fs::path &path) override;

    // Returns the contents of a directory
    std::vector<string> readDir(const fs::path &path) const override;


    //
    // Working with files
    //

public:

    // Opens or closes a file
    HandleRef open(const fs::path &path, u32 flags) override;
    void close(HandleRef handle) override;

    // Creates a new file
    void create(const fs::path &path) override;

    // Removes a file from its directory
    void unlink(const fs::path &path) override;

    // Moves the file to a different location
    void move(const fs::path &oldPath, const fs::path &newPath) override;

    // Changes the size of a file
    void resize(const fs::path &path, isize size) override;

    // Moves the read/write pointer
    isize lseek(HandleRef ref, isize offset, u16 whence = 0) override;

    // Reads data from a file
    isize read(HandleRef ref, std::span<u8> buffer) override;

    // Writes data to a file
    isize write(HandleRef ref, std::span<const u8> buffer) override;

    // Changes file permissions
    void chmod(const fs::path &path, u32 mode) override;

private:

    void tryReclaim(BlockNr block);

    Handle &getHandle(HandleRef ref);

    BlockNr ensureFile(const fs::path &path);
    BlockNr ensureFileOrDirectory(const fs::path &path);
    BlockNr ensureDirectory(const fs::path &path);
    
    
    //
    // Working with caches
    //

public:
    
    void flush() override;
    void invalidate() override;
};

}

