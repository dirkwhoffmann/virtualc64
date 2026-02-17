// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

/* The FileSystem class represents a Commodore CBM file system.
 * It models a logical volume that can be created on top of, e.g., a D64 file,
 * an HDF file, or a GCR-encoded FloppyDisk.
 *
 * The FileSystem class is organized as a layered architecture to separate
 * responsibilities and to enforce downward-only dependencies.
 *
 *          <-- Layer view --->     <-------------  Class view  -------------->
 *
 *           -----------------       -----------------
 * Layer 5: |    POSIX API    |<--->| PosixFileSystem |
 *           -----------------       -----------------
 *           |  |  |                        / \
 *           |  |  |                        \ /
 *           |  |  V                         |
 *           |  |  -----------               |               -----------------
 * Layer 4:  |  | | Services  |..............|..............|                 |
 *           |  |  -----------               |              |    FSImporter   |
 *           |  |  |         |               |              |                 |
 *           |  |  |         |       ----------------- /\   |    FSExporter   |
 *           |  V  V         |      |                 |  ---|                 |
 *           |  -----------  |      |                 |\/   |    FSCrawler    |
 * Layer 3:  | |   Paths   |.|......|                 |     |                 |
 *           |  -----------  |      |                 |     |    FSDoctor     |
 *           |  |            |      |   FileSystem    |     |                 |
 *           |  |            |      |                 |      -----------------
 *           V  V            V      |                 |
 *           -----------------      |                 |
 * Layer 2: |     Nodes       |.....|                 |
 *           -----------------       -----------------
 *                   |                      / \
 *                   |                      \ /
 *                   V                       |
 *           -----------------       -----------------
 * Layer 1: |   Block cache   |.....| FSCache/FSBlock |
 *           -----------------       -----------------
 *                   |                      / \
 *                   |                      \ /
 *                   V                       |
 *           -----------------       -----------------
 * Layer 0: |  Block device   |.....|     Volume      |
 *           -----------------       -----------------
 *
 *
 * Notes:
 *
 *   POSIX layer:
 *
 *   The uppermost layer implements a POSIX-like file system interface. It
 *   wraps a FileSystem instance and hides all lower-level access mechanisms.
 *   This layer exposes a high-level API with POSIX-style semantics, including
 *   operations such as open, close, read, write, and file-handle management.
 *
 *   Path Layer:
 *
 *   This layer is part of the FileSystem class. It resolves symbolic and
 *   relative paths into canonical file system objects. This layer is
 *   responsible for path normalization and name resolution.
 *
 *   Node Layer:
 *
 *   Interprets storage blocks as files and directories according to OFS or FFS
 *   semantics. It provides primitives for creating and deleting files and
 *   directories, as well as for accessing and modifying file metadata.
 *
 *   Block Cache Layer:
 *
 *   Bridges the node layer and the underlying block device. It manages cached
 *   access to blocks and maintains block-level metadata to improve performance
 *   and consistency.
 *
 *   Block Device Layer:
 *
 *   Provides access to the physical or virtual storage medium and stores the
 *   actual data. Any object implementing the BlockDevice protocol can serve as
 *   a backing store.
 */

#pragma once

#include "FileSystems/CBM/FSTypes.h"
#include "FileSystems/CBM/FSBlock.h"
#include "FileSystems/CBM/FSContract.h"
#include "FileSystems/CBM/FSObjects.h"
#include "FileSystems/CBM/FSCache.h"
#include "FileSystems/CBM/FSDirEntry.h"
#include "FileSystems/CBM/FSDoctor.h"
#include "FileSystems/CBM/FSAllocator.h"
#include "FileSystems/CBM/FSImporter.h"
#include "FileSystems/CBM/FSExporter.h"
#include "FileSystems/CBM/FSTraits.h"
#include "FileSystems/FSError.h"
#include "FileSystems/PosixViewTypes.h"
#include "DeviceError.h"
#include "Volume.h"
#include "utl/abilities/Loggable.h"

namespace retro::vault::cbm {

using retro::vault::Volume;

class FileSystem : public Loggable {

    friend struct FSBlock;

    // Immutable file system properties
    FSTraits traits;

    // Generation counter (increased with every write access)
    isize generation = 0;


    // Block layer

    // Gateway to the underlying block device
    FSCache cache;

    // Allocation and allocation map managenent
    FSAllocator allocator = FSAllocator(*this);


    // Service layer

public:

    // Block import
    FSImporter importer = FSImporter(*this);

    // Block export
    FSExporter exporter = FSExporter(*this);

    // Error checking, rectification
    FSDoctor doctor = FSDoctor(*this, allocator);

    // Contracts
    FSRequire require = FSRequire(*this);
    FSEnsure ensure = FSEnsure(*this);


    //
    // Initializing
    //

public:

    FileSystem(Volume &vol);
    virtual ~FileSystem() = default;

    FileSystem(const FileSystem &) = delete;
    FileSystem(FileSystem &&) = delete;
    FileSystem& operator=(const FileSystem &) = delete;
    FileSystem& operator=(FileSystem &&) = delete;

    void stepGeneration() { ++generation; }
    
    
    //
    // Printing debug information
    //

public:

    void dumpStatfs(std::ostream &os = std::cout) const noexcept;
    void dumpProps(std::ostream &os = std::cout) const noexcept;
    void dumpBlocks(std::ostream &os = std::cout) const noexcept;


    //
    // Querying file system properties
    //

public:

    // Queries immutable file system properties
    const FSTraits &getTraits() const noexcept { return traits; }
    isize blocks() const noexcept { return traits.blocks; }
    isize bsize() const noexcept { return traits.bsize; }
    isize bytes() const noexcept { return blocks() * bsize(); }

    // Returns a textual description of the file system
    vector<string> describe() const noexcept { return { "CBM File System" }; }
    
    // Checks whether the file system is formatted
    bool isFormatted() const noexcept;

    // Returns usage information and root metadata
    FSStat stat() const noexcept;

    // Returns information about file permissions
    FSAttr attr(const FSDirEntry &entry) const;
    optional<FSAttr> attr(const PETName<16> &name) const;
    optional<FSAttr> attr(const fs::path &path) const;


    //
    // B L O C K   L A Y E R
    //

    //
    // Querying block properties
    //

public:

    // Returns the type of a certain block or a block item
    FSBlockType typeOf(BlockNr nr) const { return fetch(nr).type; }
    FSItemType typeOf(BlockNr nr, isize pos) const { return fetch(nr).itemType(pos); }

    // Convenience wrappers
    bool is(BlockNr nr, FSBlockType type) const { return fetch(nr).is(type); }
    bool isEmpty(BlockNr nr) const { return fetch(nr).isEmpty(); }

    // Predicts the type of a block based on the stored data
    FSBlockType predictType(BlockNr nr, const u8 *buf) const noexcept;


    //
    // Accessing the block storage
    //

public:

    // Returns a pointer to a block with read permissions (maybe null)
    const FSBlock *tryFetch(BlockNr nr) const noexcept { return cache.tryFetch(nr); }
    const FSBlock *tryFetch(BlockNr nr, FSBlockType t) const noexcept { return cache.tryFetch(nr, t); }
    const FSBlock *tryFetch(TSLink ts) const noexcept;
    const FSBlock *tryFetch(TSLink ts, FSBlockType t) const noexcept;

    // Returns a reference to a block with read permissions (may throw)
    const FSBlock &fetch(BlockNr nr) const { return cache.fetch(nr); }
    const FSBlock &fetch(BlockNr nr, FSBlockType t) const { return cache.fetch(nr, t); }
    const FSBlock &fetch(TSLink ts) const;
    const FSBlock &fetch(TSLink ts, FSBlockType t) const;

    // Convenience wrapper
    const FSBlock *tryFetchBAM() const noexcept { return tryFetch({18,0}, FSBlockType::BAM); }
    const FSBlock &fetchBAM() const { return fetch({18,0}, FSBlockType::BAM); }

    // Writes back dirty cache blocks to the block device
    void flush();

    // Invalidates all cached blocks
    void invalidate();

    // Operator overload for fetch
    const FSBlock &operator[](size_t nr) { return cache.fetch(BlockNr(nr)); }

    // Reads the block allocation map
    [[deprecated]] std::vector<bool> readBitmap() const { return allocator.readBitmap(); }


    //
    // N O D E   L A Y E R
    //

    //
    // Formatting
    //

public:

    // Formats the volume with a specific DOS type
    void format(FSFormat dos);

    // Reformats the volume with the existing DOS type
    void format() { format(traits.dos); }

    // Assigns the volume name
    void setName(const PETName<16> &name);

private:

    // Formats the BAM sector
    void formatBAM(FSFormat dos, const PETName<16> &name);


    //
    // Managing directories
    //

public:
    
    // Reads the existing directory
    vector<FSDirEntry> readDir() const;

    // Reads the directory entries from a specific block
    vector<FSDirEntry> readDirBlock(BlockNr block) const;

    // Overwrites the existing directory
    void writeDir(const vector<FSDirEntry> &dir);

    // Returns the number of directory items
    isize numItems() const;

    // Looks up a specific directory item
    optional<FSDirEntry> searchDir(const PETName<16> &name) const;
    vector<FSDirEntry> searchDir(const FSPattern &pattern) const;

    // Adds a new entry to the directory
    void link(const FSDirEntry &entry);

    // Removes an existing directory entry
    void unlink(BlockNr b);

    // Applies a function to all items in a directory block
    template <typename Func>
    void forEachDirEntry(BlockNr b, Func &&func) const
    {
        auto *data = fetch(b).data();

        for (int i = 0; i < 8; i++) {
            func((const FSDirEntry *)data + i);
        }
    }

    template <typename Func>
    void forEachDirEntry(BlockNr b, Func &&func)
    {
        auto *data = fetch(b).mutate().data();

        for (int i = 0; i < 8; i++) {
            func((FSDirEntry *)data + i);
        }
    }


    //
    // Managing files
    //

public:

    // Creates a new file
    BlockNr createFile(const PETName<16> &name);
    BlockNr createFile(const PETName<16> &name, const u8 *buf, isize size);
    BlockNr createFile(const PETName<16> &name, const Buffer<u8> &buf);
    BlockNr createFile(const PETName<16> &name, const string &str);

    // Delete a file
    void rm(BlockNr at);
    void rm(const PETName<16> file);

    // Renames a file
    void rename(const PETName<16> &src, const PETName<16> &dst);

    // Extracts the data from a file
    isize extractData(BlockNr b, Buffer<u8> &buf) const;
    isize extractData(TSLink ts, Buffer<u8> &buf) const;
    isize extractData(const FSDirEntry &entry, Buffer<u8> &buf) const;

    // Shrinks or expands an existing file (pad with 0)
    void resize(BlockNr at, isize size);

    // Replaces the cotents of an existing file
    void replace(BlockNr at, const u8 *buf, isize size);
    void replace(BlockNr at, const Buffer<u8> &data);
    void replace(BlockNr at, const string &str);

private:

    // Main replace function
    void replace(vector<BlockNr> blocks, const u8 *buf, isize size);


    //
    // Creating and destroying blocks
    //

public:

    // Frees the blocks of a deleted directory or file
    void reclaim(BlockNr fhb);


    //
    // Managing linked lists
    //

public:

    optional<BlockNr> nextBlock(BlockNr b) const noexcept;
    optional<TSLink> nextBlock(TSLink ts) const noexcept;

    vector<BlockNr> collectDirBlocks() const;
    vector<BlockNr> collectDataBlocks(BlockNr nr) const;
    vector<BlockNr> collectDataBlocks(const FSDirEntry &entry) const;

private:

    // Block iterator
    using BlockIterator = std::function<const FSBlock *(const FSBlock *)>;

    // Follows a linked list and collects all blocks
    vector<BlockNr> collect(const BlockNr nr, BlockIterator succ) const noexcept;
    vector<const FSBlock *> collect(const FSBlock &block, BlockIterator succ) const noexcept;

    
    //
    // P A T H   L A Y E R
    //

    //
    // Seeking files and directories
    //

public:

    // Returns the root of the directory tree
    BlockNr bam() const { return *traits.blockNr({18,0}); }

    // Checks if a an item exists in the directory tree
    bool exists(const PETName<16> &path) const { return trySeek(path).has_value(); }
    bool exists(const char *path) const { return trySeek(path).has_value(); }
    bool exists(const string &path) const { return trySeek(path).has_value(); }
    bool exists(const fs::path &path) const { return trySeek(path).has_value(); }

    // Resolves a path by name (returns the number of the first data block)
    optional<BlockNr> trySeek(const PETName<16> &path) const;
    optional<BlockNr> trySeek(const char *path) const { return trySeek(PETName<16>(path)); }
    optional<BlockNr> trySeek(const string &path) const { return trySeek(PETName<16>(path)); }
    optional<BlockNr> trySeek(const fs::path &path) const { return trySeek(PETName<16>(path)); }

    // Resolves a path by name (may throw)
    BlockNr seek(const PETName<16> &path) const;
    BlockNr seek(const char *path) const { return seek(PETName<16>(path)); }
    BlockNr seek(const string &path) const { return seek(PETName<16>(path)); }
    BlockNr seek(const fs::path &path) const { return seek(PETName<16>(path)); }
};

}
