// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _FS_DEVICES_H
#define _FS_DEVICES_H

#include "FSDescriptors.h"
#include "FSObjects.h"
#include "FSBlock.h"
#include "FSDirEntry.h"
#include "D64File.h"

#include <dirent.h>

class FSDevice : C64Object {
    
    friend class FSBlock;
    
    // The block storage
    std::vector<BlockPtr> blocks;

public:
    
    // Layout descriptor for this device
    FSDeviceDescriptor layout;

    
    //
    // Factory methods
    //
    
public:

    // Creates a file system with a custom device descriptor
    static FSDevice *makeWithFormat(FSDeviceDescriptor &layout);

    // Creates a file system for a standard floppy disk
    static FSDevice *makeWithFormat(DiskType type);

    // Creates a file system from a D64 image
    static FSDevice *makeWithD64(class D64File *d64, FSError *error);

    // Creates a file from an object implementing the Archive interface
    static FSDevice *makeWithArchive(AnyArchive *otherArchive, FSError *error);

    
    //
    // Initializing
    //
    
public:

    FSDevice(u32 capacity);
    ~FSDevice();
    
    const char *getDescription() override { return "FSVolume"; }
        
    // Prints information about this volume
    void info();

    
    //
    // Debugging
    //

public:
    
    // Prints debug information
    void dump();

    // Prints a directory listing
    void printDirectory();

    
    //
    // Querying file system properties
    //

public:
    
    // Returns the DOS version of this file system
    FSVolumeType dos() { return layout.dos; }
    
    // Reports layout information
    u32 getNumCyls() { return layout.numCyls; }
    u32 getNumHeads() { return layout.numHeads; }
    u32 getNumTracks() { return layout.numTracks(); }
    u32 getNumSectors(Track track) { return layout.numSectors(track); }
    u32 getNumBlocks() { return layout.numBlocks(); }

    // Returns the number of stored files
    u32 numFiles() { return (u32)scanDirectory().size(); }

    
    //
    // Accessing blocks
    //
    
public:
    
    // Returns the type of a certain block
    FSBlockType blockType(u32 nr);
    
    // Returns the usage type of a certain byte in a certain block
    FSItemType itemType(u32 nr, u32 pos);
    
    // Queries a pointer from the block storage (may return nullptr)
    FSBlock *blockPtr(Block b);
    FSBlock *blockPtr(BlockRef ref);
    FSBlock *blockPtr(Track t, Sector s);
    FSBlock *bamPtr() { return blocks[357]; }

    // Follows the block chain link of a specific block
    FSBlock *nextBlockPtr(Block b);
    FSBlock *nextBlockPtr(Track t, Sector s);
    FSBlock *nextBlockPtr(FSBlock *ptr);

    
    //
    // Working with the BAM (Block Allocation Map)
    //

    // Checks if a block is marked as free in the allocation bitmap
    bool isFree(Block b);
    bool isFree(BlockRef r);
    bool isFree(Track t, Sector s);

    // Returns the first or the next free block in the interleaving chain
    BlockRef nextFreeBlock(BlockRef start);
    BlockRef firstFreeBlock() { return nextFreeBlock({1,0}); }

    // Marks a block as allocated or free
    void markAsAllocated(Block b) { setAllocationBit(b, 0); }
    void markAsAllocated(Track t, Sector s) { setAllocationBit(t, s, 0); }
    
    void markAsFree(Block b) { setAllocationBit(b, 1); }
    void markAsFree(Track t, Sector s) { setAllocationBit(t, s, 1); }
    
    void setAllocationBit(Block b, bool value);
    void setAllocationBit(Track t, Sector s, bool value);

    // Allocates a certain amount of (interleaved) blocks
    std::vector<BlockRef> allocate(BlockRef ref, u32 n);
    std::vector<BlockRef> allocate(u32 n) { return allocate( {1,0}, n); }

private:
    
    // Locates the allocation bit for a certain block
    FSBlock *locateAllocationBit(Block b, u32 *byte, u32 *bit);
    FSBlock *locateAllocationBit(BlockRef ref, u32 *byte, u32 *bit);
    FSBlock *locateAllocationBit(Track t, Sector s, u32 *byte, u32 *bit);

    
    //
    // Working with files
    //
    
public:

    // Seeks a file by number or name
    // FSDirEntry *seek(u32 nr);
    // FSDirEntry *seek(FSName &name);
    
    // Returns the next free directory entry
    FSDirEntry *nextFreeDirEntry(); 
    
    // Collects pointers to the directory entries of all existing files
    std::vector<FSDirEntry *> scanDirectory(bool skipInvisible = true);
        
    // Ensures that the disk has enough directory blocks to host 'n' files
    bool setCapacity(u32 n);
    
    // Creates a new file
    bool makeFile(const char *name, const u8 *buf, size_t cnt);

private:
    
    bool makeFile(const char *name, FSDirEntry *dir, const u8 *buf, size_t cnt);

    
    //
    // Integrity checking
    //

public:
    
    // Checks all blocks in this volume
    FSErrorReport check(bool strict);

    // Checks a single byte in a certain block
    FSError check(u32 blockNr, u32 pos, u8 *expected, bool strict);

    /*
    // Checks if the type of a block matches one of the provides types
    FSError checkBlockType(u32, FSBlockType type);
    FSError checkBlockType(u32, FSBlockType type, FSBlockType altType);
    */
    
    // Checks if a certain block is corrupted
    bool isCorrupted(u32 blockNr) { return getCorrupted(blockNr) != 0; }

    // Returns the position in the corrupted block list (0 = OK)
    u32 getCorrupted(u32 blockNr);

    // Returns the number of the next or previous corrupted block
    u32 nextCorrupted(u32 blockNr);
    u32 prevCorrupted(u32 blockNr);

    // Checks if a certain block is the n-th corrupted block
    bool isCorrupted(u32 blockNr, u32 n);

    // Returns the number of the the n-th corrupted block
    u32 seekCorruptedBlock(u32 n);

    
    //
    // Importing and exporting
    //
    
public:
        
    // Reads a single byte from a block
    u8 readByte(u32 block, u32 offset);

    // Imports the volume from a buffer
    bool importVolume(const u8 *src, size_t size, FSError *error = nullptr);
    
    // Exports the volume to a buffer
    bool exportVolume(u8 *dst, size_t size, FSError *error = nullptr);

    // Exports a single block or a range of blocks
    bool exportBlock(u32 nr, u8 *dst, size_t size, FSError *error = nullptr);
    bool exportBlocks(u32 first, u32 last, u8 *dst, size_t size, FSError *error = nullptr);

    // Exports a file the volume to a directory of the host file system
    bool exportFile(FSDirEntry *item, const char *path, FSError *error);
    bool exportDirectory(const char *path, FSError *error);
};

#endif
