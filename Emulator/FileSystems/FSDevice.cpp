// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FSDevice.h"
#include "Disk.h"
#include "IO.h"
#include "PRGFile.h"
#include "P00File.h"
#include "T64File.h"

#include <limits.h>
#include <set>

FSDevice *
FSDevice::makeWithFormat(FSDeviceDescriptor &layout)
{
    FSDevice *dev = new FSDevice(layout.numBlocks());
    dev->layout = layout;
    return dev;
}

FSDevice *
FSDevice::makeWithType(DiskType type, DOSType vType)
{
    FSDeviceDescriptor layout = FSDeviceDescriptor(type);
    FSDevice *fileSystem = makeWithFormat(layout);
    
    if (vType != DOS_TYPE_NODOS) {
        fileSystem->bamPtr()->writeBAM();
    }
    
    return fileSystem;
}

FSDevice *
FSDevice::makeWithD64(D64File &d64)
{
    // Get device descriptor
    FSDeviceDescriptor descriptor = FSDeviceDescriptor(d64);
        
    // Create the device
    FSDevice *device = makeWithFormat(descriptor);

    // Import file system
    try { device->importVolume(d64.data, d64.size); }
    catch (VC64Error &exception) { delete device; throw exception; }
    
    // Import error codes (if any)
    for (Block b = 0; b < (Block)device->blocks.size(); b++) {
        device->setErrorCode(b, d64.getErrorCode(b));
    }
    
    return device;
}

FSDevice *
FSDevice::makeWithD64(D64File &d64, ErrorCode *err)
{
    *err = ERROR_OK;
    try { return makeWithD64(d64); }
    catch (VC64Error &exception) { *err = exception.data; }
    return nullptr;
}
  
FSDevice *
FSDevice::makeWithDisk(class Disk &disk)
{
    // Translate the GCR stream into a byte stream
    u8 buffer[D64File::D64_802_SECTORS];
    usize len = disk.decodeDisk(buffer);
    
    // Create a suitable device descriptor
    FSDeviceDescriptor descriptor = FSDeviceDescriptor(DISK_TYPE_SS_SD);
    switch (len) {
            
        case D64File::D64_683_SECTORS: descriptor.numCyls = 35; break;
        case D64File::D64_768_SECTORS: descriptor.numCyls = 40; break;
        case D64File::D64_802_SECTORS: descriptor.numCyls = 42; break;

        default:
            throw VC64Error(ERROR_FS_CORRUPTED);
    }
        
    // Create the device
    FSDevice *device = makeWithFormat(descriptor);

    // Import file system
    try { device->importVolume(buffer, len); }
    catch (VC64Error &exception) { delete device; throw exception; }
        
    return device;
}

FSDevice *
FSDevice::makeWithDisk(class Disk &disk, ErrorCode *err)
{
    *err = ERROR_OK;
    try { return makeWithDisk(disk); }
    catch (VC64Error &exception) { *err = exception.data; }
    return nullptr;
}

FSDevice *
FSDevice::makeWithCollection(AnyCollection &collection)
{
    // Create the device
    FSDevice *device = makeWithType(DISK_TYPE_SS_SD);
    assert(device);
    
    // Write BAM
    auto name = PETName<16>(collection.collectionName());
    device->bamPtr()->writeBAM(name);

    // Loop over all items
    isize numberOfItems = collection.collectionCount();
    for (isize i = 0; i < numberOfItems; i++) {
        
        // Serialize item into a buffer
        u64 size = collection.itemSize(i);
        u8 *buffer = new u8[size];
        collection.copyItem(i, buffer, size);
        
        // Create a file for this item
        device->makeFile(collection.itemName(i), buffer, size);
        delete[] buffer;
    }
    
    device->printDirectory();
    return device;
}

FSDevice *
FSDevice::makeWithCollection(AnyCollection &collection, ErrorCode *err)
{
    *err = ERROR_OK;
    try { return makeWithCollection(collection); }
    catch (VC64Error &exception) { *err = exception.data; }
    return nullptr;
}

FSDevice *
FSDevice::makeWithPath(const std::string &path)
{
    ErrorCode ec;
        
    if (auto file = AnyFile::make <D64File> (path, &ec)) {
        return makeWithD64(*file);
    }
    if (auto file = AnyFile::make <T64File> (path, &ec)) {
        return makeWithCollection(*file);
    }
    if (auto file = AnyFile::make <PRGFile> (path, &ec)) {
        return makeWithCollection(*file);
    }
    if (auto file = AnyFile::make <P00File> (path, &ec)) {
        return makeWithCollection(*file);
    }
    
    throw VC64Error(ERROR_FILE_TYPE_MISMATCH);
 }

FSDevice *
FSDevice::makeWithPath(const std::string &path, ErrorCode *err)
{
    *err = ERROR_OK;
    try { return makeWithPath(path); }
    catch (VC64Error &exception) { *err = exception.data; }
    return nullptr;
}

FSDevice *
FSDevice::makeWithFolder(const std::string &path)
{
    // Create the device
    FSDevice *device = makeWithType(DISK_TYPE_SS_SD);
    assert(device);
    
    // Write BAM
    auto name = PETName<16>(util::extractName(path));
    device->bamPtr()->writeBAM(name);
    
    // Import the folder
    try { device->importDirectory(path); }
    catch (VC64Error &exception) { delete device; throw exception; }
    
    device->printDirectory();
    return device;
}

FSDevice *
FSDevice::makeWithFolder(const std::string &path, ErrorCode *err)
{
    *err = ERROR_OK;
    try { return makeWithFolder(path); }
    catch (VC64Error &exception) { *err = exception.data; }
    return nullptr;
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
    scanDirectory();
}

void
FSDevice::dump() const
{
    // Dump all blocks
    for (usize i = 0; i < blocks.size(); i++)  {
        
        msg("\nBlock %zu (%d):", i, blocks[i]->nr);
        msg(" %s\n", FSBlockTypeEnum::key(blocks[i]->type()));
        
        blocks[i]->dump();
    }
}

void
FSDevice::printDirectory()
{
    scanDirectory();
    
    for (auto &item : dir) {
        msg("%3llu \"%-16s\" %s (%5llu bytes)\n",
            fileBlocks(item),
            item->getName().c_str(), item->typeString(), fileSize(item));
    }
}

i32
FSDevice::numFreeBlocks() const
{
    u32 result = 0;
    
    for (usize i = 0; i < blocks.size(); i++) {
        if (isFree((Block)i)) result++;
    }
    
    return result;
}

i32
FSDevice::numUsedBlocks() const
{
    u32 result = 0;
    
    for (usize i = 0; i < blocks.size(); i++) {
        if (!isFree((Block)i)) result++;
    }
    
    return result;
}

FSBlockType
FSDevice::blockType(Block b) const
{
    return blockPtr(b) ? blocks[b]->type() : FS_BLOCKTYPE_UNKNOWN;
}

FSUsage
FSDevice::usage(Block b, u32 pos) const
{
    return blockPtr(b) ? blocks[b]->itemType(pos) : FS_USAGE_UNUSED;
}

u8
FSDevice::getErrorCode(Block b) const
{
    return blockPtr(b) ? blocks[b]->errorCode : 0;
}

void
FSDevice::setErrorCode(Block b, u8 code)
{
    if (blockPtr(b)) blocks[b]->errorCode = code;
}

FSBlock *
FSDevice::blockPtr(Block b) const
{
    return (u64)b < (u64)blocks.size() ? blocks[b] : nullptr;
}

FSBlock *
FSDevice::nextBlockPtr(Block b) const
{
    FSBlock *ptr = blockPtr(b);
    
    // Jump to linked block
    if (ptr) { ptr = blockPtr(ptr->tsLink()); }
    
    return ptr;
}

FSBlock *
FSDevice::nextBlockPtr(FSBlock *ptr) const
{
    return ptr ? blockPtr(ptr->tsLink()) : nullptr;
}

PETName<16>
FSDevice::getName() const
{
    FSBlock *bam = bamPtr();
    return PETName<16>(bam->data + 0x90);
}

void
FSDevice::setName(PETName<16> name)
{
    FSBlock *bam = bamPtr();
    name.write(bam->data + 0x90);
}

bool
FSDevice::isFree(TSLink ts) const
{
    u32 byte, bit;
    FSBlock *bam = locateAllocBit(ts, &byte, &bit);
    
    return GET_BIT(bam->data[byte], bit);
}

TSLink
FSDevice::nextFreeBlock(TSLink ref) const
{
    if (!layout.isValidLink(ref)) return {0,0};
    
    while (ref.t && !isFree(ref)) {
        ref = layout.nextBlockRef(ref);
    }
    
    return ref;
}

void
FSDevice::setAllocBit(TSLink ts, bool value)
{
    u32 byte, bit;
    FSBlock *bam = locateAllocBit(ts, &byte, &bit);

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

std::vector<TSLink>
FSDevice::allocate(TSLink ts, u32 n)
{
    assert(n > 0);
    
    std::vector<TSLink> result;
    FSBlock *block = nullptr;

    // Get the next free block
    ts = nextFreeBlock(ts);

    if (ts.t) {
        
        for (u32 i = 0; i < n; i++) {
            
            // Collect block reference
            result.push_back(ts);
            markAsAllocated(ts);
            
            // Link this block
            block = blockPtr(ts);
            ts = layout.nextBlockRef(ts);
            block->data[0] = ts.t;
            block->data[1] = ts.s;
        }
        
        // Delete the block reference in the last block
        block->data[0] = 0;
        block->data[1] = 0;
    }
    
    return result;
}

FSBlock *
FSDevice::locateAllocBit(Block b, u32 *byte, u32 *bit) const
{
    return locateAllocBit(layout.tsLink(b), byte, bit);
}

FSBlock *
FSDevice::locateAllocBit(TSLink ts, u32 *byte, u32 *bit) const
{
    assert(layout.isValidLink(ts));
        
    /* Bytes $04 - $8F store the BAM entries for each track, in groups of four
     * bytes per track, starting on track 1. [...] The first byte is the number
     * of free sectors on that track. The next three bytes represent the bitmap
     * of which sectors are used/free. Since it is 3 bytes we have 24 bits of
     * storage. Remember that at most, each track only has 21 sectors, so there
     * are a few unused bits.
     */
        
    *byte = (4 * ts.t) + 1 + (ts.s >> 3);
    *bit = ts.s & 0x07;
    
    return bamPtr();
}

PETName<16>
FSDevice::fileName(usize nr) const
{
    assert(nr < numFiles());
    return fileName(dir[nr]);
}

PETName<16>
FSDevice::fileName(FSDirEntry *entry) const
{
    assert(entry);
    return PETName<16>(entry->fileName);
}

FSFileType
FSDevice::fileType(usize nr) const
{
    assert(nr < numFiles());
    return fileType(dir[nr]);
}

FSFileType
FSDevice::fileType(FSDirEntry *entry) const
{
    assert(entry);
    return entry->getFileType();
}

u64
FSDevice::fileSize(usize nr) const
{
    assert(nr < numFiles());
    return fileSize(dir[nr]);
}

u64
FSDevice::fileSize(FSDirEntry *entry) const
{
    assert(entry);
    
    u64 size = 0;
    std::set<Block> visited;

    // Start at the first data block
    BlockPtr b = blockPtr(entry->firstBlock());

    // Iterate through the block chain
    while (b && visited.find(b->nr) == visited.end()) {
                
        visited.insert(b->nr);
        BlockPtr next = nextBlockPtr(b);
                
        if (next) {
            size += 254;
        } else {
            // The number of remaining bytes can be derived from the sector link
            size += b->data[1] ? b->data[1] - 1 : 0;
        }
        b = next;
    }
    
    return size;
}

u64
FSDevice::fileBlocks(usize nr) const
{
    assert(nr < numFiles());
    return fileBlocks(dir[nr]);
}

u64
FSDevice::fileBlocks(FSDirEntry *entry) const
{
    assert(entry);
    return HI_LO(entry->fileSizeHi, entry->fileSizeLo);
}

u16
FSDevice::loadAddr(usize nr) const
{
    assert(nr < numFiles());
    return loadAddr(dir[nr]);
}

u16
FSDevice::loadAddr(FSDirEntry *entry) const
{
    assert(entry);
    u8 addr[2]; copyFile(entry, addr, 2);
    return LO_HI(addr[0], addr[1]);
}

void
FSDevice::copyFile(usize nr, u8 *buf, u64 len, u64 offset) const
{
    assert(nr < numFiles());
    copyFile(dir[nr], buf, len, offset);
}

void
FSDevice::copyFile(FSDirEntry *entry, u8 *buf, u64 len, u64 offset) const
{
    assert(entry);
    
    // Start at the first data block
    BlockPtr b = blockPtr(entry->firstBlock());
    u64 pos = 2;
    
    // Iterate through the block chain
    while (b && len) {
                
        if (offset) {
            --offset;
        } else {
            --len;
            *buf++ = b->data[pos];
        }
        
        if (++pos == 256) {
            b = nextBlockPtr(b);
            pos = 2;
        }
    }
}

FSDirEntry *
FSDevice::getOrCreateNextFreeDirEntry()
{
    // The directory starts on track 18, sector 1
    FSBlock *ptr = blockPtr(TSLink{18,1});
    
    // A disk can hold up to 144 files
    for (int i = 0; ptr && i < 144; i++) {
    
        FSDirEntry *entry = (FSDirEntry *)ptr->data + (i % 8);
        
        // Return if this entry is unused
        if (entry->isEmpty()) return entry;
     
        // Keep on searching in the current block if slots remain
        if (i % 8 != 7) continue;
        
        // Keep on searching in the next directory block if it already exists
        if (FSBlock *next = nextBlockPtr(ptr)) { ptr = next; continue; }
        
        // Create a new directory block and link to it
        TSLink ts = layout.nextBlockRef(layout.tsLink(ptr->nr));
        ptr->data[0] = ts.t;
        ptr->data[1] = ts.s;
    }
    
    return nullptr;
}

void
FSDevice::scanDirectory(bool skipInvisible)
{
    // Start from scratch
    dir.clear();
    
    // The directory starts on track 18, sector 1
    FSBlock *ptr = blockPtr(TSLink{18,1});
    
    // The number of files is limited by 144
    for (int i = 0; ptr && i < 144; i++) {
    
        FSDirEntry *entry = (FSDirEntry *)ptr->data + (i % 8);
        
        // Terminate if there are no more entries
        if (entry->isEmpty()) break;

        // Add file to the result list
        if (!(skipInvisible && entry->isHidden())) dir.push_back(entry);
     
        // Jump to the next sector if this was the last directory item
        if (i % 8 == 7) ptr = nextBlockPtr(ptr);
    }
}

bool
FSDevice::makeFile(PETName<16> name, const u8 *buf, usize cnt)
{
    // Search the next free directory slot
    FSDirEntry *dir = getOrCreateNextFreeDirEntry();

    // Create the file if we've found a free slot
    if (dir) return makeFile(name, dir, buf, cnt);
         
    return false;
}

bool
FSDevice::makeFile(PETName<16> name, FSDirEntry *dir, const u8 *buf, usize cnt)
{
    // Determine the number of blocks needed for this file
    u32 numBlocks = (u32)((cnt + 253) / 254);
        
    // Allocate data blocks
    auto blockList = allocate(numBlocks);
    if (blockList.empty()) return false;
        
    auto it = blockList.begin();
    FSBlock *ptr = blockPtr(*it);
    
    // Write data
    usize i, j;
    for (i = 0, j = 2; i < cnt; i++, j++) {

        if (j == 0x100) {
            ++it;
            ptr = blockPtr(*it);
            j = 2;
        }
        ptr->data[j] = buf[i];
    }
 
    // Store the size of the last data chunk inside the sector link
    assert(ptr->data[0] == 0);
    ptr->data[1] = (cnt % 254) + 1;
    
    // Write directory entry
    dir->init(name, blockList[0], numBlocks);
    
    return true;
}

FSErrorReport
FSDevice::check(bool strict)
{
    FSErrorReport result;

    long total = 0, min = LONG_MAX, max = 0;
    usize numBlocks = blocks.size();
    
    // Analyze all blocks
    for (u32 i = 0; i < numBlocks; i++) {

        if (blocks[i]->check(strict) > 0) {
            min = std::min(min, (long)i);
            max = std::max(max, (long)i);
            blocks[i]->corrupted = (u32)++total;
        } else {
            blocks[i]->corrupted = 0;
        }
    }

    // Record findings
    if (total) {
        result.corruptedBlocks = total;
        result.firstErrorBlock = min;
        result.lastErrorBlock = max;
    } else {
        result.corruptedBlocks = 0;
        result.firstErrorBlock = min;
        result.lastErrorBlock = max;
    }
    
    return result;
}

ErrorCode
FSDevice::check(u32 blockNr, u32 pos, u8 *expected, bool strict)
{
    return blocks[blockNr]->check(pos, expected, strict);
}

u32
FSDevice::getCorrupted(u32 blockNr) const
{
    return blockPtr(blockNr) ? blocks[blockNr]->corrupted : 0;
}

bool
FSDevice::isCorrupted(u32 blockNr, u32 n) const
{
    usize numBlocks = blocks.size();
    
    for (u32 i = 0, cnt = 0; i < numBlocks; i++) {
        
        if (isCorrupted(i)) {
            cnt++;
            if (blockNr == i) return cnt == n;
        }
    }
    return false;
}

u32
FSDevice::nextCorrupted(u32 blockNr) const
{
    usize numBlocks = blocks.size();
    
    for (u32 i = blockNr + 1; i < numBlocks; i++) {
        if (isCorrupted(i)) return i;
    }
    return blockNr;
}

u32
FSDevice::prevCorrupted(u32 blockNr) const
{
    usize numBlocks = blocks.size();
    
    for (u32 i = blockNr - 1; i < numBlocks; i--) {
        if (isCorrupted(i)) return i;
    }
    return blockNr;
}

u8
FSDevice::readByte(Block block, u32 offset) const
{
    assert(offset < 256);
    assert(block < blocks.size());
    
    return blocks[block]->data[offset];
}

void
FSDevice::importVolume(const u8 *src, usize size)
{
    ErrorCode err;
    importVolume(src, size, &err);
    if (err != ERROR_OK) { throw VC64Error(err); }
}

bool
FSDevice::importVolume(const u8 *src, usize size, ErrorCode *err)
{
    assert(src != nullptr);

    debug(FS_DEBUG, "Importing file system (%zu bytes)...\n", size);

    // Only proceed if the buffer size matches
    if (blocks.size() * 256 > size) {
        warn("BUFFER SIZE MISMATCH (%lu %lu)\n", blocks.size(), blocks.size() * 256);
        if (err) *err = ERROR_FS_WRONG_CAPACITY;
        return false;
    }
        
    // Import all blocks
    for (u32 i = 0; i < blocks.size(); i++) {
        
        const u8 *data = src + i * 256;
        blocks[i]->importBlock(data);
    }
    
    if (err) *err = ERROR_OK;

    // Run a directory scan
    scanDirectory();
    
    if (FS_DEBUG) {
        // info();
        // dump();
        printDirectory();
    }
    
    return true;
}

bool
FSDevice::importDirectory(const string &path)
{
    if (DIR *dir = opendir(path.c_str())) {
        
        bool result = importDirectory(path, dir);
        closedir(dir);
        return result;
    }

    warn("Error opening directory %s\n", path.c_str());
    return false;
}

/*
bool
FSDevice::importDirectory(const char *path)
{
    assert(path);
    return importDirectory(string(path));
}
*/

bool
FSDevice::importDirectory(const std::string &path, DIR *dir)
{
    struct dirent *item;
    bool result = true;
    
    while ((item = readdir(dir))) {
        
        // Skip '.', '..' and all hidden files
        if (item->d_name[0] == '.') continue;
        
        // Assemble file name (TODO: PORT TO std::string)
        std::string name = std::string(item->d_name);
        std::string full = path + "/" + name;
        
        msg("importDirectory: Processing %s (%s)\n", name.c_str(), full.c_str());

        if (item->d_type == DT_DIR) continue;
        
        u8 *buf; long len;
        if (util::loadFile(full, &buf, &len)) {
            
            PETName<16> pet = PETName<16>(util::stripSuffix(name));
            if (!makeFile(pet, buf, len)) {
                warn("Failed to import file %s\n", name.c_str());
                result = false;
            }
            delete[] buf;
        }
    }
    return result;
}

/*
bool
FSDevice::importDirectory(const char *path, DIR *dir)
{
    assert(dir);
    return importDirectory(std::string(path), dir);
}
*/

bool
FSDevice::exportVolume(u8 *dst, usize size, ErrorCode *err)
{
    return exportBlocks(0, layout.numBlocks() - 1, dst, size, err);
}

bool
FSDevice::exportBlock(u32 nr, u8 *dst, usize size, ErrorCode *err)
{
    return exportBlocks(nr, nr, dst, size, err);
}

bool
FSDevice::exportBlocks(u32 first, u32 last, u8 *dst, usize size, ErrorCode *err)
{
    assert(last < layout.numBlocks());
    assert(first <= last);
    assert(dst);
    
    u32 count = last - first + 1;
    
    debug(FS_DEBUG, "Exporting %d blocks (%d - %d)\n", count, first, last);

    // Only proceed if the source buffer contains the right amount of data
    if (count * 256 != size) {
        if (err) *err = ERROR_FS_WRONG_CAPACITY;
        return false;
    }
        
    // Wipe out the target buffer
    memset(dst, 0, size);
    
    // Export all blocks
    for (u32 i = 0; i < count; i++) {
        
        blocks[first + i]->exportBlock(dst + i * 256);
    }

    debug(FS_DEBUG, "Success\n");
    
    if (err) *err = ERROR_OK;
    return true;
}

bool
FSDevice::exportDirectory(const std::string &path, ErrorCode *err)
{
    // Only proceed if path points to an empty directory
    usize numItems = util::numDirectoryItems(path);
    if (numItems != 0) {
        if (err) *err = ERROR_DIR_NOT_EMPTY;
        return false;
    }
    
    // Rescan the directory to get the directory cache up to date
    scanDirectory();
    
    // Export all items
    for (auto const& entry : dir) {

        if (entry->getFileType() != FS_FILETYPE_PRG) {
            msg("Skipping file %s\n", entry->getName().c_str());
            continue;
        }
        
        if (!exportFile(entry, path, err)) {
            msg("Export error: %ld\n", (long)*err);
            return false;
        }
    }
    
    msg("Exported %lu items", dir.size());
    if (err) *err = ERROR_OK;
    return true;
}

bool
FSDevice::exportFile(FSDirEntry *entry, const std::string &path, ErrorCode *err)
{
    std::string name = path + "/" + entry->getName().str();
    debug(FS_DEBUG, "Exporting file to %s\n", name.c_str());

    std::ofstream stream(name);
    if (!stream.is_open()) {
        // TODO: throw
        *err = ERROR_FILE_CANT_CREATE;
        return false;
    }

    exportFile(entry, stream, err);
    return true;
}

void
FSDevice::exportFile(FSDirEntry *entry, std::ofstream &stream, ErrorCode *err)
{
    std::set<Block> visited;

    // Start at the first data block
    BlockPtr b = blockPtr(entry->firstBlock());

    // Iterate through the block chain
    while (b && visited.find(b->nr) == visited.end()) {
                
        visited.insert(b->nr);
        BlockPtr next = nextBlockPtr(b);
                
        usize size = next ? 254 : b->data[1] ? b->data[1] - 1 : 0;
        stream.write((char *)(b->data + 2), size);
        
        b = next;
    }
}
