// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FSDescriptors.h"
#include "FSBlock.h"
#include "FSDirEntry.h"
#include "D64File.h"
#include "AnyCollection.h"
#include "IOUtils.h"
#include <vector>

class FileSystem : C64Object {
    
    friend class FSBlock;
        
    // The block storage
    std::vector<BlockPtr> blocks;

public:
    
    // Layout descriptor for this device
    FSDeviceDescriptor layout;

    // Result of the latest directory scan
    std::vector<FSDirEntry *> dir;

    
    //
    // Initializing
    //
    
public:

    FileSystem(isize capacity) { init(capacity); }
    FileSystem(FSDeviceDescriptor &layout) { init(layout); }
    FileSystem(DiskType type, DOSType vType) { init(type, vType); }
    FileSystem(const class D64File &d64) throws { init(d64); }
    FileSystem(class Disk &disk) throws { init(disk); }
    FileSystem(AnyCollection &collection) throws { init(collection); }
    FileSystem(const string &path) throws { init(path); }
    ~FileSystem();
    
private:
    
    void init(isize capacity);
    void init(FSDeviceDescriptor &layout);
    void init(DiskType type, DOSType vType);
    void init(const class D64File &d64) throws;
    void init(class Disk &disk) throws;
    void init(AnyCollection &collection) throws;
    void init(const string &path) throws;

    
public:
    
    const char *getDescription() const override { return "FSVolume"; }
        
    // Prints information about this volume
    void info();

    
    //
    // Debugging
    //

public:
    
    // Prints debug information
    void dump() const;

    // Prints a directory listing
    void printDirectory();

    
    //
    // Querying file system properties
    //

public:
    
    // Returns the DOS version of this file system
    DOSType dos() const { return layout.dos; }
    
    // Gets or sets the disk name
    PETName<16> getName() const;
    void setName(PETName<16> name);

    // Returns the first or second disk ID character
    u8 diskId1() const { return bamPtr()->data[0xA2]; }
    u8 diskId2() const { return bamPtr()->data[0xA3]; }
    
    // Reports layout information
    isize getNumCyls() const { return layout.numCyls; }
    isize getNumHeads() const { return layout.numHeads; }
    isize getNumTracks() const { return layout.numTracks(); }
    isize getNumSectors(Track track) const { return layout.numSectors(track); }
    isize getNumBlocks() const { return layout.numBlocks(); }

    // Returns the number of free or used blocks
    isize numFreeBlocks() const;
    isize numUsedBlocks() const;

    // Returns the number of stored files (run a directory scan first!)
    isize numFiles() const { return isize(dir.size()); }
    
    
    //
    // Accessing blocks
    //
    
public:
    
    // Returns the type of a certain block
    FSBlockType blockType(Block b) const;
    FSBlockType blockType(TSLink ts) const { return blockType(layout.blockNr(ts)); }
    
    // Informs about the usage of a certain byte in a certain block
    FSUsage usage(Block b, u32 pos) const;
    FSUsage usage(TSLink ts, u32 pos) const { return usage(layout.blockNr(ts), pos); }

    // Gets or sets the error code for a certain block
    u8 getErrorCode(Block b) const;
    u8 getErrorCode(TSLink ts) const { return getErrorCode(layout.blockNr(ts)); }
    void setErrorCode(Block b, u8 code);
    void setErrorCode(TSLink ts, u8 code) { setErrorCode(layout.blockNr(ts), code); }

    // Queries a pointer from the block storage (may return nullptr)
    FSBlock *blockPtr(Block b) const;
    FSBlock *blockPtr(TSLink ts) const { return blockPtr(layout.blockNr(ts)); }
    FSBlock *bamPtr() const { return blocks[357]; }
    
    // Follows the block chain link of a specific block
    FSBlock *nextBlockPtr(Block b) const;
    FSBlock *nextBlockPtr(TSLink ts) const { return nextBlockPtr(layout.blockNr(ts)); }
    FSBlock *nextBlockPtr(FSBlock *ptr) const;

    
    //
    // Working with the BAM (Block Allocation Map)
    //
    
    // Checks if a block is marked as free in the allocation bitmap
    bool isFree(Block b) const { return isFree(layout.tsLink(b)); }
    bool isFree(TSLink ts) const;

    // Returns the first or the next free block in the interleaving chain
    TSLink nextFreeBlock(TSLink start) const;
    TSLink firstFreeBlock() const { return nextFreeBlock({1,0}); }

    // Marks a block as allocated or free
    void markAsAllocated(Block b) { setAllocBit(b, 0); }
    void markAsAllocated(TSLink ts) { setAllocBit(ts, 0); }
    
    void markAsFree(Block b) { setAllocBit(b, 1); }
    void markAsFree(TSLink ts) { setAllocBit(ts, 1); }

    void setAllocBit(Block b, bool value) { setAllocBit(layout.tsLink(b), value); }
    void setAllocBit(TSLink ts, bool value);

    // Allocates a certain amount of (interleaved) blocks
    std::vector<TSLink> allocate(TSLink ref, u32 n);
    std::vector<TSLink> allocate(u32 n) { return allocate( TSLink{1,0}, n); }

private:
    
    // Locates the allocation bit for a certain block
    FSBlock *locateAllocBit(Block b, isize *byte, isize *bit) const;
    FSBlock *locateAllocBit(TSLink ref, isize *byte, isize *bit) const;

    
    //
    // Reading files
    //
    
public:
    
    // Returns the name of a file
    PETName<16> fileName(isize nr) const;
    PETName<16> fileName(FSDirEntry *entry) const;

    // Returns the type of a file
    FSFileType fileType(isize nr) const;
    FSFileType fileType(FSDirEntry *entry) const;
    
    // Returns the precise size of a file in bytes
    isize fileSize(isize nr) const;
    isize fileSize(FSDirEntry *entry) const;
    
    // Returns the size of a file in blocks (read from the BAM)
    isize fileBlocks(isize nr) const;
    isize fileBlocks(FSDirEntry *entry) const;

    // Returns the load address of a file
    u16 loadAddr(isize nr) const;
    u16 loadAddr(FSDirEntry *entry) const;
    
    // Copies the file contents into a buffer
    void copyFile(isize nr, u8 *buf, u64 len, u64 offset = 0) const;
    void copyFile(FSDirEntry *entry, u8 *buf, u64 len, u64 offset = 0) const;

    // Scans the directory and stores the result in variable 'dir'
    void scanDirectory(bool skipInvisible = true);

    
    //
    // Writing files
    //

    // Returns the next free directory entry or creates one
    FSDirEntry *getOrCreateNextFreeDirEntry(); 
                    
    // Creates a new file
    bool makeFile(PETName<16> name, const u8 *buf, isize cnt);

private:
    
    bool makeFile(PETName<16> name, FSDirEntry *entry, const u8 *buf, isize cnt);

    
    //
    // Integrity checking
    //

public:
    
    // Checks all blocks in this volume
    FSErrorReport check(bool strict);

    // Checks a single byte in a certain block
    ErrorCode check(isize blockNr, u32 pos, u8 *expected, bool strict);

    // Checks if the block with the given number is part of the volume
    bool isBlockNumber(isize nr) const { return nr >= 0 && nr < getNumBlocks(); }

    // Returns the position in the corrupted block list (0 = OK)
    isize getCorrupted(isize blockNr) const;

    // Checks if a certain block is corrupted
    bool isCorrupted(isize blockNr) const { return getCorrupted(blockNr) != 0; }

    // Returns the number of the next or previous corrupted block
    isize nextCorrupted(isize blockNr) const;
    isize prevCorrupted(isize blockNr) const;

    // Checks if a certain block is the n-th corrupted block
    bool isCorrupted(isize blockNr, isize n) const;

    // Returns the number of the the n-th corrupted block
    isize seekCorruptedBlock(isize n) const;

    
    //
    // Importing and exporting
    //
    
public:
        
    // Reads a single byte from a block
    u8 readByte(Block block, isize offset) const;
    u8 readByte(TSLink ts, isize offset) const { return readByte(layout.blockNr(ts), offset); }

    // Imports the volume from a buffer
    void importVolume(const u8 *src, isize size) throws;
    bool importVolume(const u8 *src, isize size, ErrorCode *err);
    
    // Imports a folder from the host file system
    void importDirectory(const string &path) throws;
    void importDirectory(const fs::directory_entry &dir) throws;

    // Exports the volume to a buffer
    bool exportVolume(u8 *dst, isize size, ErrorCode *err = nullptr);

    // Exports a single block or a range of blocks
    bool exportBlock(isize nr, u8 *dst, isize size, ErrorCode *err = nullptr);
    bool exportBlocks(isize first, isize last, u8 *dst, isize size, ErrorCode *err = nullptr);

    // Exports all files or a single file to a folder in the host file system
    void exportDirectory(const string &path) throws;
    void exportFile(FSDirEntry *item, const string &path) throws;
    void exportFile(FSDirEntry *entry, std::ofstream &stream) throws;


    //
    // GUI helper functions
    //

    // Determines how the layout image should look like in a certain column
    FSBlockType getDisplayType(isize column);

    // Determines how the diagnose image should look like in a certain column
    isize diagnoseImageSlice(isize column);

    // Searches the block list for a block of a specific type
    isize nextBlockOfType(FSBlockType type, isize after);

    // Searches the block list for a corrupted block
    isize nextCorruptedBlock(isize after);
};
