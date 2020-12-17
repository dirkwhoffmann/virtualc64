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

    // Layout descriptor for this device
    FSDeviceDescriptor layout;
    
    // The block storage
    std::vector<BlockPtr> blocks;

    
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
    FSBlock *blockPtr(Track t, Sector s);

    // Follows the block chain link of a specific block
    FSBlock *nextBlockPtr(Block b);
    FSBlock *nextBlockPtr(Track t, Sector s);
    FSBlock *nextBlockPtr(FSBlock *ptr);

    // Writes a byte to a block (returns true on success)
    // bool writeByteToSector(u8 byte, Block *b, u8 *offset);
    bool writeByteToSector(u8 byte, Track *t, Sector *s, u32 *offset);

    
    //
    // Working with the BAM (Block Allocation Map)
    //

    // Checks if a block is marked as free in the allocation bitmap
    bool isFree(Block b);
    bool isFree(Track t, Sector s);

    // Marks a block as allocated or free
    void markAsAllocated(Block b) { setAllocationBit(b, 0); }
    void markAsAllocated(Track t, Sector s) { setAllocationBit(t, s, 0); }
    void markAsFree(Block b) { setAllocationBit(b, 1); }
    void markAsFree(Track t, Sector s) { setAllocationBit(t, s, 1); }
    void setAllocationBit(Block b, bool value);
    void setAllocationBit(Track t, Sector s, bool value);

private:
    
    // Locates the allocation bit for a certain block
    FSBlock *locateAllocationBit(Block b, u32 *byte, u32 *bit);
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
    bool increaseCapacity(u32 n);
    
    // Creates a new file
    FSBlock *makeFile(const char *name);
    FSBlock *makeFile(const char *name, const u8 *buffer, size_t size);
    FSBlock *makeFile(const char *name, const char *str);

private:
    
    FSBlock *makeFile(const char *name, FSDirEntry *entry);
    
    
    //
    // Importing and exporting
    //
    
public:
        
    // Reads a single byte from a block
    u8 readByte(u32 block, u32 offset);

    // Imports the volume from a buffer
    bool importVolume(const u8 *src, size_t size, FSError *error = nullptr);    
};

#endif
