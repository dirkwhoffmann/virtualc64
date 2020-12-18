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
    assert(d64);

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

FSDevice *
FSDevice::makeWithArchive(AnyArchive *archive, FSError *error)
{
    assert(archive);
    
    // Get device descriptor
    FSDeviceDescriptor descriptor = FSDeviceDescriptor(DISK_SS_SD);
        
    // Create the device
    FSDevice *device = makeWithFormat(descriptor);
        
    // Write BAM
    device->blockPtr(18,0)->writeBAM(archive->getName());

    // Create the proper amount of directory blocks
    int numberOfItems = archive->numberOfItems();
    device->setCapacity(numberOfItems);

    // The first directory block is located on track 18, sector 1
    Track t = 1; Sector s = 0; u32 offset = 2;

    // Loop over all entries in archive
    for (int i = 0; i < numberOfItems; i++) {
        
        archive->selectItem(i);
        
        // Add directory entry
        FSDirEntry *entry = device->nextFreeDirEntry();
        printf("nextFreeDirEntry = %p\n", entry);
        entry->init(archive->getNameOfItem(), t, s, archive->getSizeOfItem());
        device->markAsAllocated(t, s);
        
        // Add data blocks
        u16 loadAddr = archive->getDestinationAddrOfItem();
        device->writeByte(LO_BYTE(loadAddr), &t, &s, &offset);
        device->writeByte(HI_BYTE(loadAddr), &t, &s, &offset);
        
        int byte;
        while ((byte = archive->readItem()) != EOF) {
            device->writeByte(byte, &t, &s, &offset);
        }
    }
    
    device->printDirectory();
        
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
        msg("Item %p: %3d \"%16s\" %s\n",
            item, HI_LO(item->fileSizeHi, item->fileSizeLo),
            item->getName().c_str(), item->typeString());
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
FSDevice::writeByte(u8 byte, Track *pt, Sector *ps, u32 *pOffset)
{
    Track t = *pt;
    Sector s = *ps;
    u32 offset = *pOffset;
    
    assert(layout.isTrackSectorPair(t, s));
    assert(offset >= 2 && offset <= 0x100);
    
    FSBlock *ptr = blockPtr(t, s);
        
    // No free slots in this sector, proceed to next one
    if (offset == 0x100) {
        
        // Only proceed if there is space left on the disk
        if (!layout.nextTrackAndSector(t, s, &t, &s)) return false;
        
        // Mark the new block as allocated
        markAsAllocated(t, s);
        
        // Link previous sector with the new one
        ptr->data[0] = (u8)t;
        ptr->data[1] = (u8)s;
        ptr = blockPtr(t, s);
        offset = 2;
    }
    
    // Write byte
    ptr->data[offset] = byte;
    
    *pt = t;
    *ps = s;
    *pOffset = offset + 1;
    return true;
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
    
    if (!value && GET_BIT(bam->data[byte], bit)) {
        
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
        
    *byte = (4 * t) + 1 + (s >> 3);
    *bit = s & 0x07;
    
    return bamPtr();
}

/*
FSDirEntry *
FSDevice::seek(u32 nr)
{
    return nullptr;
}
*/

FSDirEntry *
FSDevice::nextFreeDirEntry()
{
    // The directory starts on track 18, sector 1
    FSBlock *ptr = blockPtr(18, 1);
    
    // The number of files is limited by 144
    for (int i = 0; ptr && i < 144; i++) {
    
        FSDirEntry *entry = (FSDirEntry *)ptr->data + (i % 8);
        
        // Return if this entry is not used
        if (entry->isEmpty()) return entry;
     
        // Jump to the next sector if this was the last directory item
        if (i % 8 == 7) ptr = nextBlockPtr(ptr);
    }
    
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
        if (entry->isEmpty()) break;

        // Add file to the result list
        if (!(skipInvisible && entry->isHidden())) result.push_back(entry);
     
        // Jump to the next sector if this was the last directory item
        if (i % 8 == 7) ptr = nextBlockPtr(ptr);
    }
    
    return result;
}

bool
FSDevice::setCapacity(u32 n)
{
    // A disk can hold up to 144 files
    assert(n <= 144);
    
    // Determine how many directory blocks are needed
    u32 numBlocks = n / 8;
    
    // The firsr directory block is located at (18,1)
    Track t = 18;
    Sector s = 1;
    FSBlock *ptr = blockPtr(t, s);
    
    for (u32 i = 1; i < numBlocks; i++) {

        // Get location of the next block
        if (!layout.nextTrackAndSector(t, s, &t, &s)) return false;
        markAsAllocated(t, s);
        
        // Link blocks
        ptr->data[0] = t;
        ptr->data[1] = s;
    }
    
    return true;
}

bool
FSDevice::increaseCapacity(u32 n)
{
    return setCapacity(numFiles() + n);
}

FSBlock *
FSDevice::makeFile(const char *name)
{
    // The directory starts on track 18, sector 1
    FSBlock *ptr = blockPtr(18, 1);
    
    // Search for the next free slot
    for (int i = 0; ptr && i < 144; i++) {
    
        FSDirEntry *entry = (FSDirEntry *)ptr->data + (i % 8);

        if (entry->isEmpty()) {
            
            makeFile(name, entry); 
        }
     
        // Jump to the next sector if this was the last directory item
        if (i % 8 == 7) ptr = nextBlockPtr(ptr);
    }
    
    return nullptr;
}

FSBlock *
FSDevice::makeFile(const char *name, const u8 *buffer, size_t size)
{
    assert(false);
    return nullptr;
}

FSBlock *
FSDevice::makeFile(const char *name, const char *str)
{
    assert(str);
    return makeFile(name, (const u8 *)str, strlen(str));
}

FSBlock *
FSDevice::makeFile(const char *name, FSDirEntry *entry)
{
    assert(false);
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

bool
FSDevice::exportVolume(u8 *dst, size_t size, FSError *error)
{
    return exportBlocks(0, layout.numBlocks() - 1, dst, size, error);
}

bool
FSDevice::exportBlock(u32 nr, u8 *dst, size_t size, FSError *error)
{
    return exportBlocks(nr, nr, dst, size, error);
}

bool
FSDevice::exportBlocks(u32 first, u32 last, u8 *dst, size_t size, FSError *error)
{
    assert(last < layout.numBlocks());
    assert(first <= last);
    assert(dst);
    
    u32 count = last - first + 1;
    
    debug(FS_DEBUG, "Exporting %d blocks (%d - %d)\n", count, first, last);

    // Only proceed if the source buffer contains the right amount of data
    if (count * 256 != size) {
        if (error) *error = FS_WRONG_CAPACITY;
        return false;
    }
        
    // Wipe out the target buffer
    memset(dst, 0, size);
    
    // Export all blocks
    for (u32 i = 0; i < count; i++) {
        
        blocks[first + i]->exportBlock(dst + i * 256);
    }

    debug(FS_DEBUG, "Success\n");
    
    if (error) *error = FS_OK;
    return true;
}

bool
FSDevice::exportFile(FSDirEntry *item, const char *path, FSError *error)
{
    debug(FS_DEBUG, "Exporting file %s to %s\n", item->getName().c_str(), path);

    assert(false);
}

bool
FSDevice::exportDirectory(const char *path, FSError *err)
{
    assert(path);
        
    // Only proceed if path points to an empty directory
    long numItems = numDirectoryItems(path);
    if (numItems != 0) return FS_DIRECTORY_NOT_EMPTY;
    
    // Collect all directory entries
    auto items = scanDirectory();
    
    // Export all items
    for (auto const& item : items) {

        if (!exportFile(item, path, err)) {
            msg("Export error: %d\n", err);
            return false;
        }
    }
    
    msg("Exported %d items", items.size());
    *err = FS_OK;
    return true;
}
