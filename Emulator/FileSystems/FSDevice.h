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
#include "FSBlock.h"
#include "FSDirEntry.h"
#include "D64File.h"
#include "AnyCollection.h"

#include <dirent.h>

class FSDevice : C64Object {
    
    friend class FSBlock;
        
    // The block storage
    std::vector<BlockPtr> blocks;

public:
    
    // Layout descriptor for this device
    FSDeviceDescriptor layout;

    // Result of the latest directory scan
    std::vector<FSDirEntry *> dir;
    
    
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

    // Creates a file system from a GCR encoded disk
    static FSDevice *makeWithDisk(class Disk *disk, FSError *error);

    // Creates a file from an object implementing the Archive interface DEPRECATED
    static FSDevice *makeWithArchive(AnyArchive *otherArchive, FSError *error);

    // Creates a file from an object implementing the AnyCollection interface
    static FSDevice *makeWithCollection(AnyCollection *collection, FSError *error);

    
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

    // Returns the number of free or used blocks
    u32 numFreeBlocks();
    u32 numUsedBlocks();

    // Returns the number of stored files (run a directory scan first!)
    u32 numFiles() { return (u32)dir.size(); }
    
    
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
    FSBlock *blockPtr(TSLink ref);
    FSBlock *blockPtr(Track t, Sector s);
    FSBlock *bamPtr() { return blocks[357]; }

    // Follows the block chain link of a specific block
    FSBlock *nextBlockPtr(Block b);
    FSBlock *nextBlockPtr(Track t, Sector s);
    FSBlock *nextBlockPtr(FSBlock *ptr);

    
    //
    // Working with the BAM (Block Allocation Map)
    //

    // Returns the disk name
    PETName<16> getName();
    
    // Checks if a block is marked as free in the allocation bitmap
    bool isFree(Block b);
    bool isFree(Track t, Sector s);
    bool isFree(TSLink ts) { return isFree(ts.t, ts.s); }

    // Returns the first or the next free block in the interleaving chain
    TSLink nextFreeBlock(TSLink start);
    TSLink firstFreeBlock() { return nextFreeBlock({1,0}); }

    // Marks a block as allocated or free
    void markAsAllocated(Block b) { setAllocationBit(b, 0); }
    void markAsAllocated(Track t, Sector s) { setAllocationBit(t, s, 0); }
    void markAsAllocated(TSLink ts) { markAsAllocated(ts.t, ts.s); }
    
    void markAsFree(Block b) { setAllocationBit(b, 1); }
    void markAsFree(Track t, Sector s) { setAllocationBit(t, s, 1); }
    void markAsFree(TSLink ts) { markAsFree(ts.t, ts.s); }

    void setAllocationBit(Block b, bool value);
    void setAllocationBit(Track t, Sector s, bool value);
    void setAllocationBit(TSLink ts) { setAllocationBit(ts.t, ts.s); }

    // Allocates a certain amount of (interleaved) blocks
    std::vector<TSLink> allocate(TSLink ref, u32 n);
    std::vector<TSLink> allocate(u32 n) { return allocate( {1,0}, n); }

private:
    
    // Locates the allocation bit for a certain block
    FSBlock *locateAllocationBit(Block b, u32 *byte, u32 *bit);
    FSBlock *locateAllocationBit(Track t, Sector s, u32 *byte, u32 *bit);
    FSBlock *locateAllocationBit(TSLink ref, u32 *byte, u32 *bit);

    
    //
    // Reading
    //
    
public:
    
    // Returns the name of a file
    PETName<16> fileName(unsigned nr);
    PETName<16> fileName(FSDirEntry *entry);

    // Returns the type of a file
    FSFileType fileType(unsigned nr);
    FSFileType fileType(FSDirEntry *entry);
    
    // Returns the precise size of a file in bytes
    u64 fileSize(unsigned nr);
    u64 fileSize(FSDirEntry *entry);
    
    // Returns the size of a file in blocks (read from the BAM)
    u64 fileBlocks(unsigned nr);
    u64 fileBlocks(FSDirEntry *entry);

    // Returns the load address of a file
    u16 loadAddr(unsigned nr);
    u16 loadAddr(FSDirEntry *entry);
    
    // Copies the file contents into a buffer
    void copyFile(unsigned nr, u8 *buf, u64 len, u64 offset = 0);
    void copyFile(FSDirEntry *entry, u8 *buf, u64 len, u64 offset = 0);

    // Scans the directory and stores the result in variable 'dir'
    void scanDirectory(bool skipInvisible = true);

    
    //
    // Writing
    //

    // Returns the next free directory entry
    FSDirEntry *nextFreeDirEntry(); 
                
    // Ensures that the disk has enough directory blocks to host 'n' files
    bool setCapacity(u32 n);
    
    // Creates a new file
    bool makeFile(PETName<16> name, const u8 *buf, size_t cnt);

private:
    
    bool makeFile(PETName<16> name, FSDirEntry *entry, const u8 *buf, size_t cnt);

    
    //
    // Integrity checking
    //

public:
    
    // Checks all blocks in this volume
    FSErrorReport check(bool strict);

    // Checks a single byte in a certain block
    FSError check(u32 blockNr, u32 pos, u8 *expected, bool strict);
    
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
