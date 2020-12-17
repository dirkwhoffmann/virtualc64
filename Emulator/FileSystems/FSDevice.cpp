// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "FSDevice.h"

FSDevice *
FSDevice::makeWithFormat(FSDeviceDescriptor &layout)
{
    FSDevice *dev = new FSDevice(layout.numBlocks());
    dev->layout = layout;
        
    /*
    if (FS_DEBUG) {
        printf("cd = %d\n", dev->cd);
        dev->info();
        dev->dump();
    }
    */
    
    return dev;
}

FSDevice *
FSDevice::makeWithFormat(DiskType type)
{
    FSDeviceDescriptor layout = FSDeviceDescriptor(type);
    return makeWithFormat(layout);
}

FSDevice *
FSDevice::makeWithD64(D64File *d64, FSError *error)
{
    assert(d64 != nullptr);

    // Get device descriptor
    FSDeviceDescriptor descriptor = FSDeviceDescriptor(DISK_SS_SD);
        
    // Create the device
    FSDevice *device = makeWithFormat(descriptor);

    // Import file system
    if (!device->importVolume(d64->getData(), d64->getSize(), error)) {
        delete device;
        return nullptr;
    }
    
    return device;
}

FSDevice::FSDevice(u32 capacity)
{
    debug(FS_DEBUG, "Creating device with %d blocks\n", capacity);
    
    // Initialize the block storage
    blocks.reserve(capacity);
    blocks.assign(capacity, 0);
    
    // Create all blocks
    for (u32 i = 0; i < capacity; i++) blocks[i] = new FSBlock(*this, i);
}

FSDevice::~FSDevice()
{
    for (auto &b : blocks) delete b;
}

void
FSDevice::info()
{
}

void
FSDevice::dump()
{
    // Dump all blocks
    for (size_t i = 0; i < blocks.size(); i++)  {
        
        msg("\nBlock %d (%d):", i, blocks[i]->nr);
        msg(" %s\n", sFSBlockType(blocks[i]->type()));
        
        blocks[i]->dump();
    }
}

void
FSDevice::printDirectory()
{
    auto dir = scanDirectory();
    
    for (auto &item : dir) {
        msg("Item: %s\n", item->getName().c_str());
    }
}

FSBlockType
FSDevice::blockType(u32 nr)
{
    return blockPtr(nr) ? blocks[nr]->type() : FS_UNKNOWN_BLOCK;
}

FSItemType
FSDevice::itemType(u32 nr, u32 pos)
{
    return blockPtr(nr) ? blocks[nr]->itemType(pos) : FSI_UNUSED;
}

FSBlock *
FSDevice::blockPtr(Block b)
{
    return b < blocks.size() ? blocks[b] : nullptr;
}

FSBlock *
FSDevice::blockPtr(Track t, Sector s)
{
    Block b;
    layout.translateBlockNr(&b, t, s);
    
    return blockPtr(b);
}

FSBlock *
FSDevice::nextBlockPtr(Block b)
{
    FSBlock *ptr = blockPtr(b);
    
    // Jump to linked block
    if (ptr) { ptr = blockPtr(ptr->data[0], ptr->data[1]); }
    
    return ptr;
}

FSBlock *
FSDevice::nextBlockPtr(Track t, Sector s)
{
    FSBlock *ptr = blockPtr(t, s);
    
    // Jump to linked block
    if (ptr) { ptr = blockPtr(ptr->data[0], ptr->data[1]); }
    
    return ptr;
}

FSBlock *
FSDevice::nextBlockPtr(FSBlock *ptr)
{
    return ptr ? nextBlockPtr(ptr->nr) : nullptr;
}

bool
FSDevice::isFree(Block b)
{
    u32 byte, bit;
    FSBlock *bam = locateAllocationBit(b, &byte, &bit);
    
    return GET_BIT(bam->data[byte], bit);
}

bool
FSDevice::isFree(Track t, Sector s)
{
    u32 byte, bit;
    FSBlock *bam = locateAllocationBit(t, s, &byte, &bit);
    
    return GET_BIT(bam->data[byte], bit);
}

void
FSDevice::setAllocationBit(Block b, bool value)
{
    Track t; Sector s;
    layout.translateBlockNr(b, &t, &s);

    setAllocationBit(t, s, value);
}

void
FSDevice::setAllocationBit(Track t, Sector s, bool value)
{
    u32 byte, bit;
    FSBlock *bam = locateAllocationBit(t, s, &byte, &bit);

    if (value && !GET_BIT(bam->data[byte], bit)) {

        // Mark sector as free
        SET_BIT(bam->data[byte], bit);

        // Increase the number of free sectors
        bam->data[byte & ~0b11]++;
    }
    
    if (!value && !GET_BIT(bam->data[byte], bit)) {
        
        // Mark sector as allocated
        CLR_BIT(bam->data[byte], bit);

        // Decrease the number of free sectors
        bam->data[byte & ~0b11]--;
    }
}

FSBlock *
FSDevice::locateAllocationBit(Block b, u32 *byte, u32 *bit)
{
    assert(b < blocks.size());
    
    Track t; Sector s;
    layout.translateBlockNr(b, &t, &s);
    
    return locateAllocationBit(t, s, byte, bit);
}

FSBlock *
FSDevice::locateAllocationBit(Track t, Sector s, u32 *byte, u32 *bit)
{
    assert(layout.isTrackSectorPair(t, s));
        
    /* Bytes $04 - $8F store the BAM entries for each track, in groups of four
     * bytes per track, starting on track 1. [...] The first byte is the number
     * of free sectors on that track. The next three bytes represent the bitmap
     * of which sectors are used/free. Since it is 3 bytes we have 24 bits of
     * storage. Remember that at most, each track only has 21 sectors, so there
     * are a few unused bits.
     */
    
    Block bam;
    layout.translateBlockNr(&bam, 18, 0);
    printf("BAM is located in block %d\n", bam);
    u32 group = 4 * t;
    *byte = group + 1 + (s >> 3);
    *bit = s & 0x07;
    
    return blocks[bam];
}

FSDirEntry *
FSDevice::seek(u32 nr)
{
    return nullptr;
}

std::vector<FSDirEntry *>
FSDevice::scanDirectory(bool skipInvisible)
{
    std::vector<FSDirEntry *> result;
    
    // The directory starts on track 18, sector 1
    FSBlock *ptr = blockPtr(18, 1);
    
    // The number of files is limited by 144
    for (int i = 0; ptr && i < 144; i++) {
    
        FSDirEntry *entry = (FSDirEntry *)ptr->data + (i % 8);
        
        // Terminate if there are no more entries
        if (isZero((u8 *)entry, 32)) break;
                
        // Add file to the result list
        if (!(skipInvisible && entry->isHidden())) result.push_back(entry);
     
        // Jump to the next sector if this was the last directory item
        if (i % 8 == 7) ptr = nextBlockPtr(ptr);
    }
    
    return result;
}

u8
FSDevice::readByte(u32 block, u32 offset)
{
    assert(offset < 256);
    assert(block < blocks.size());
    
    return blocks[block]->data[offset];
}

bool
FSDevice::importVolume(const u8 *src, size_t size, FSError *error)
{
    assert(src != nullptr);

    debug(FS_DEBUG, "Importing file system (%zu bytes)...\n", size);

    // Only proceed if the buffer size matches
    if (blocks.size() * 256 != size) {
        warn("BUFFER SIZE MISMATCH (%d %d)\n", blocks.size(), blocks.size() * 256);
        if (error) *error = FS_WRONG_CAPACITY;
        return false;
    }
        
    // Import all blocks
    for (u32 i = 0; i < blocks.size(); i++) {
        
        const u8 *data = src + i * 256;
        blocks[i]->importBlock(data);
    }
    
    if (error) *error = FS_OK;

    if (FS_DEBUG) {
        
        // info();
        // dump();
        printDirectory();
    }
    
    return true;
}
