// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

/* The FileSystem class represents an Amiga file system (OFS or FFS).
 * It models a logical volume that can be created on top of, e.g., an ADF file,
 * an HDF file, or an MFM-encoded FloppyDisk. In the case of an HDF, the file
 * system may span either the entire HDF or a single partition, only.
 *
 * The FileSystem class is organized as a layered architecture to separate
 * responsibilities and to enforce downward-only dependencies.
 *
 *          <-- Layer view --->     <-------------  Class view  -------------->
 *
 *           -----------------       -----------------
 * Layer 5: |    POSIX API    |<--->| PosixFileSystem |
 *           -----------------       -----------------
 *           |  |  |                        < >
 *           |  |  |                         |
 *           |  |  V                         |
 *           |  |  -----------               |               -----------------
 * Layer 4:  |  | | Services  |..............|..............|                 |
 *           |  |  -----------               |              |    FSImporter   |
 *           |  |  |         |               |              |                 |
 *           |  |  |         |       -----------------      |    FSExporter   |
 *           |  V  V         |      |                 |<>---|                 |
 *           |  -----------  |      |                 |     |    FSCrawler    |
 * Layer 3:  | |   Paths   |.|......|                 |     |                 |
 *           |  -----------  |      |                 |     |    FSDoctor     |
 *           |  |            |      |   FileSystem    |     |                 |
 *           |  |            |      |                 |      -----------------
 *           V  V            V      |                 |
 *           -----------------      |                 |
 * Layer 2: |     Nodes       |.....|                 |
 *           -----------------       -----------------
 *                   |                      < >
 *                   |                       |
 *                   V                       |
 *           -----------------       -----------------
 * Layer 1: |   Block cache   |.....| FSCache/FSBlock |
 *           -----------------       -----------------
 *                   |                      < >
 *                   |                       |
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
 *   a backing store, including ADFFile, HDFFile, or FloppyDisk.
 */

#pragma once

#include "FileSystems/Amiga/FSTypes.h"
#include "FileSystems/Amiga/FSBlock.h"
#include "FileSystems/Amiga/FSContract.h"
#include "FileSystems/Amiga/FSDescriptor.h"
#include "FileSystems/Amiga/FSObjects.h"
#include "FileSystems/Amiga/FSCache.h"
#include "FileSystems/Amiga/FSDoctor.h"
#include "FileSystems/Amiga/FSAllocator.h"
#include "FileSystems/Amiga/FSImporter.h"
#include "FileSystems/Amiga/FSExporter.h"
#include "FileSystems/Amiga/FSTree.h"
#include "FileSystems/FSError.h"
#include "FileSystems/PosixViewTypes.h"
#include "DeviceError.h"
#include "Volume.h"
#include "utl/abilities/Loggable.h"

namespace retro::vault::amiga {

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


    // Node layer

    // Location of the root block
    BlockNr rootBlock = 0;

    // Location of bitmap blocks and extended bitmap blocks
    vector<BlockNr> bmBlocks;
    vector<BlockNr> bmExtBlocks;


    // Path layer

    // Location of the current directory
    BlockNr current = 0;


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

    void dumpInfo(std::ostream &ss = std::cout) const noexcept;
    void dumpState(std::ostream &ss = std::cout) const noexcept;
    void dumpProps(std::ostream &ss = std::cout) const noexcept;
    void dumpBlocks(std::ostream &ss = std::cout) const noexcept;


    //
    // Querying file system properties
    //

public:

    // Queries immutable file system properties
    const FSTraits &getTraits() const noexcept { return traits; }
    isize blocks() const noexcept { return traits.blocks; }
    isize bytes() const noexcept { return traits.bytes; }
    isize bsize() const noexcept { return traits.bsize; }

    // Returns a textual description of the file system
    vector<string> describe() const noexcept;

    // Checks whether the file system is formatted
    bool isFormatted() const noexcept;

    // Returns usage information and root metadata
    FSStat stat() const noexcept;

    // Returns information about the boot block
    FSBootStat bootStat() const noexcept;

    // Returns information about file permissions
    FSAttr attr(BlockNr nr) const;


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
    bool isRoot(BlockNr nr) const { return fetch(nr).isRoot(); }
    bool isFile(BlockNr nr) const { return fetch(nr).isFile(); }
    bool isDirectory(BlockNr nr) const { return fetch(nr).isDirectory(); }
    bool isRegular(BlockNr nr) const { return fetch(nr).isRegular(); }
    bool isData(BlockNr nr) const { return fetch(nr).isData(); }

    // Predicts the type of a block based on the stored data
    FSBlockType predictType(BlockNr nr, const u8 *buf) const noexcept;


    //
    // Accessing the block storage
    //

public:

    // Returns a pointer to a block with read permissions (maybe null)
    const FSBlock *tryFetch(BlockNr nr) const noexcept { return cache.tryFetch(nr); }
    const FSBlock *tryFetch(BlockNr nr, FSBlockType t) const noexcept { return cache.tryFetch(nr, t); }
    const FSBlock *tryFetch(BlockNr nr, vector<FSBlockType> ts) const noexcept { return cache.tryFetch(nr, ts); }

    // Returns a reference to a block with read permissions (may throw)
    const FSBlock &fetch(BlockNr nr) const { return cache.fetch(nr); }
    const FSBlock &fetch(BlockNr nr, FSBlockType t) const { return cache.fetch(nr, t); }
    const FSBlock &fetch(BlockNr nr, vector<FSBlockType> ts) const { return cache.fetch(nr, ts); }

    // Writes back dirty cache blocks to the block device
    void flush();

    // Invalidates all cached blocks
    void invalidate();
    
    // Operator overload for fetch
    const FSBlock &operator[](size_t nr) { return cache.fetch(BlockNr(nr)); }


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
    void setName(const FSName &name);

    // Installs a boot block
    void makeBootable(BootBlockId id);

    // Removes a boot block virus (if any)
    void killVirus();


    //
    // Managing directories
    //

    // Returns the number of items in a directory or the items themselves
    isize numItems(BlockNr at) const;
    vector<BlockNr> getItems(BlockNr at) const;

    // Looks up a specific directory item
    optional<BlockNr> searchdir(BlockNr at, const FSName &name) const;
    vector<BlockNr> searchdir(BlockNr at, const FSPattern &pattern) const;

    // Creates a new directory
    BlockNr mkdir(BlockNr at, const FSName &name);

    // Removes an empty directory
    void rmdir(BlockNr at);

    // Creates a new directory entry
    void link(BlockNr at, BlockNr fhb);

    // Removes an existing directory entry
    void unlink(BlockNr fhb);

private:

    // Adds a hash-table entry for a given item
    void addToHashTable(BlockNr parent, BlockNr ref);

    // Removes the hash-table entry for a given item
    void deleteFromHashTable(BlockNr item);


    //
    // Managing files
    //

public:

    // Creates a new file
    BlockNr createFile(BlockNr at, const FSName &name);
    BlockNr createFile(BlockNr at, const FSName &name, const u8 *buf, isize size);
    BlockNr createFile(BlockNr at, const FSName &name, const Buffer<u8> &buf);
    BlockNr createFile(BlockNr at, const FSName &name, const string &str);

    // Delete a file
    void rm(BlockNr at);

    // Renames a file or directory
    void rename(BlockNr item, const FSName &name);

    // Moves a file or directory to another location
    void move(BlockNr item, BlockNr dest);
    void move(BlockNr item, BlockNr dest, const FSName &name);

    // Copies a file
    void copy(BlockNr item, BlockNr dest);
    void copy(BlockNr item, BlockNr dest, const FSName &name);

    // Shrinks or expands an existing file (pad with 0)
    void resize(BlockNr at, isize size);

    // Replaces the cotents of an existing file
    void replace(BlockNr at, const u8 *buf, isize size);
    void replace(BlockNr at, const Buffer<u8> &data);
    void replace(BlockNr at, const string &str);

private:

    // Main replace function
    BlockNr replace(BlockNr fhb,
                    const u8 *buf, isize size,
                    vector<BlockNr> listBlocks,
                    vector<BlockNr> dataBlocks);


    //
    // Creating and destroying blocks
    //

public:

    // Frees the blocks of a deleted directory or file
    void reclaim(BlockNr fhb);

private:

    // Creates a new block of a certain kind
    BlockNr newUserDirBlock(const FSName &name);
    BlockNr newFileHeaderBlock(const FSName &name);

    // Adds a new block of a certain kind
    void addFileListBlock(BlockNr at, BlockNr head, BlockNr prev);
    void addDataBlock(BlockNr at, BlockNr id, BlockNr head, BlockNr prev);

    // Adds bytes to a data block
    isize addData(BlockNr nr, const u8 *buf, isize size);


    //
    // Traversing linked lists
    //

public:

    vector<BlockNr> collectDataBlocks(BlockNr nr) const;
    vector<BlockNr> collectListBlocks(BlockNr nr) const;
    vector<BlockNr> collectHashedBlocks(BlockNr nr, isize bucket) const;
    vector<BlockNr> collectHashedBlocks(BlockNr nr) const;

private:

    // Follows a linked list and collects all blocks
    using BlockIterator = std::function<const FSBlock *(const FSBlock *)>;
    vector<const FSBlock *> collect(const FSBlock &block, BlockIterator succ) const;
    vector<BlockNr> collect(const BlockNr nr, BlockIterator succ) const;

    // Collects blocks of a certain type
    vector<const FSBlock *> collectDataBlocks(const FSBlock &block) const;
    vector<const FSBlock *> collectListBlocks(const FSBlock &block) const;
    vector<const FSBlock *> collectHashedBlocks(const FSBlock &block, isize bucket) const;
    vector<const FSBlock *> collectHashedBlocks(const FSBlock &block) const;


    //
    // P A T H   L A Y E R
    //

    //
    // Managing the working directory
    //

public:

    // Returns the working directory
    BlockNr pwd() const { return current; }

    // Changes the working directory
    void cd(BlockNr nr);
    void cd(const string &path) { cd(seek(path)); }
    void cd(const fs::path &path) { cd(seek(path)); }


    //
    // Seeking files and directories
    //

public:

    // Returns the root of the directory tree
    BlockNr root() const { return rootBlock; }

    // Returns the locations of the bitmap and bitmap extension blocks
    const vector<BlockNr> &getBmBlocks() const { return bmBlocks; }
    const vector<BlockNr> &getBmExtBlocks() const { return bmExtBlocks; }

    // Checks if a an item exists in the directory tree
    bool exists(const FSPath &path) const { return trySeek(path).has_value(); }
    bool exists(const char *path) const { return trySeek(path).has_value(); }
    bool exists(const string &path) const { return trySeek(path).has_value(); }
    bool exists(const fs::path &path) const { return trySeek(path).has_value(); }

    // Resolves a path by name
    optional<BlockNr> trySeek(const FSPath &path) const;
    optional<BlockNr> trySeek(const char *path) const { return trySeek(FSPath(path)); }
    optional<BlockNr> trySeek(const string &path) const { return trySeek(FSPath(path)); }
    optional<BlockNr> trySeek(const fs::path &path) const { return trySeek(FSPath(path)); }

    // Resolves a path by name (may throw)
    BlockNr seek(const FSPath &path) const;
    BlockNr seek(const char *path) const { return seek(FSPath(path)); }
    BlockNr seek(const string &path) const { return seek(FSPath(path)); }
    BlockNr seek(const fs::path &path) const { return seek(FSPath(path)); }

    // Resolves a path by a regular expression
    vector<BlockNr> match(BlockNr top, const vector<FSPattern> &patterns);
    vector<BlockNr> match(const string &path);


    //
    // S E R V I C E   L A Y E R
    //

public:

    // Builds a directory tree with traversal capabilities
    FSTree build(BlockNr root, const FSTreeBuildOptions &opt = {}) const;
};

}
