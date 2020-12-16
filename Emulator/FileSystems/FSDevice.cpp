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
    debug("printDirectory: IMPLEMENTATION MISSING\n");
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
FSDevice::blockPtr(u32 nr)
{
    return nr < blocks.size() ? blocks[nr] : nullptr;
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
        info();
        dump();
        hexdump(blocks[0]->data, 256);
        printDirectory();
    }
    
    return true;
}
