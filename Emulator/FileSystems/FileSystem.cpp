// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystem.h"
#include "Disk.h"
#include "IOUtils.h"
#include "Folder.h"
#include "PRGFile.h"
#include "P00File.h"
#include "T64File.h"

#include <limits.h>
#include <set>

FileSystem::~FileSystem()
{
    for (auto &b : blocks) delete b;
}

void
FileSystem::init(isize capacity)
{
    debug(FS_DEBUG, "Creating device with %zd blocks\n", capacity);

    // Initialize the block storage
    blocks.reserve(capacity);
    blocks.assign(capacity, 0);
    
    // Create all blocks
    for (u32 i = 0; i < capacity; i++) blocks[i] = new FSBlock(*this, i);
}

void
FileSystem::init(FSDeviceDescriptor &layout)
{
    init(layout.numBlocks());
    this->layout = layout;
}

void
FileSystem::init(DiskType type, DOSType vType)
{
    FSDeviceDescriptor layout = FSDeviceDescriptor(type);
    init(layout);
    
    if (vType != DOS_TYPE_NODOS) {
        bamPtr()->writeBAM();
    }
}

void
FileSystem::init(const D64File &d64)
{
    // Get device descriptor
    FSDeviceDescriptor descriptor = FSDeviceDescriptor(d64);
        
    // Create the device
    init(descriptor);

    // Import file system
    importVolume(d64.data, d64.size);
    
    // Import error codes (if any)
    for (Block b = 0; b < (Block)blocks.size(); b++) {
        setErrorCode(b, d64.getErrorCode(b));
    }
}

void
FileSystem::init(class Disk &disk)
{
    // Translate the GCR stream into a byte stream
    u8 buffer[D64File::D64_802_SECTORS];
    isize len = disk.decodeDisk(buffer);
    
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
    init(descriptor);

    // Import file system
    importVolume(buffer, len);
}

void
FileSystem::init(AnyCollection &collection)
{
    // Create the device
    init(DISK_TYPE_SS_SD, DOS_TYPE_CBM);
    
    // Write BAM
    auto name = PETName<16>(collection.collectionName());
    bamPtr()->writeBAM(name);

    // Loop over all items
    isize numberOfItems = collection.collectionCount();
    for (isize i = 0; i < numberOfItems; i++) {
        
        // Serialize item into a buffer
        u64 size = collection.itemSize(i);
        u8 *buffer = new u8[size];
        collection.copyItem(i, buffer, size);
        
        // Create a file for this item
        makeFile(collection.itemName(i), buffer, size);
        delete[] buffer;
    }
    
    printDirectory();
}

void
FileSystem::init(const string &path)
{
    if (Folder::isCompatible(path)) {
    
        // Create the device
        init(DISK_TYPE_SS_SD, DOS_TYPE_NODOS);
        
        // Write BAM
        auto name = PETName<16>(util::extractName(path));
        bamPtr()->writeBAM(name);
        
        // Import the folder
        importDirectory(path);

        printDirectory();
        return;
    }

    if (D64File::isCompatible(path)) {
    
        auto file = D64File(path);
        init(file);
        return;
    }

    if (T64File::isCompatible(path)) {
        
        auto file = T64File(path);
        init(file);
        return;
    }

    if (PRGFile::isCompatible(path)) {
        
        auto file = PRGFile(path);
        init(file);
        return;
    }

    if (P00File::isCompatible(path)) {
        
        auto file = P00File(path);
        init(file);
        return;
    }
        
    throw VC64Error(ERROR_FILE_TYPE_MISMATCH);
}

void
FileSystem::info()
{
    scanDirectory();
}

void
FileSystem::dump() const
{
    isize blocksSize = (isize)blocks.size();
    
    for (isize i = 0; i < blocksSize; i++)  {
        
        msg("\nBlock %zu (%zd):", i, blocks[i]->nr);
        msg(" %s\n", FSBlockTypeEnum::key(blocks[i]->type()));
        
        blocks[i]->dump();
    }
}

void
FileSystem::printDirectory()
{
    scanDirectory();

    for (auto &item : dir) {
        msg("%3llu \"%-16s\" %s (%5llu bytes)\n",
            fileBlocks(item),
            item->getName().c_str(), item->typeString().c_str(), fileSize(item));
    }
}

i32
FileSystem::numFreeBlocks() const
{
    u32 result = 0;
    isize blocksSize = (isize)blocks.size();
    
    for (isize i = 0; i < blocksSize; i++) {
        if (isFree((Block)i)) result++;
    }
    
    return result;
}

i32
FileSystem::numUsedBlocks() const
{
    u32 result = 0;
    isize blocksSize = (isize)blocks.size();
    
    for (isize i = 0; i < blocksSize; i++) {
        if (!isFree((Block)i)) result++;
    }
    
    return result;
}

FSBlockType
FileSystem::blockType(Block b) const
{
    return blockPtr(b) ? blocks[b]->type() : FS_BLOCKTYPE_UNKNOWN;
}

FSUsage
FileSystem::usage(Block b, u32 pos) const
{
    return blockPtr(b) ? blocks[b]->itemType(pos) : FS_USAGE_UNUSED;
}

u8
FileSystem::getErrorCode(Block b) const
{
    return blockPtr(b) ? blocks[b]->errorCode : 0;
}

void
FileSystem::setErrorCode(Block b, u8 code)
{
    if (blockPtr(b)) blocks[b]->errorCode = code;
}

FSBlock *
FileSystem::blockPtr(Block b) const
{
    return (u64)b < (u64)blocks.size() ? blocks[b] : nullptr;
}

FSBlock *
FileSystem::nextBlockPtr(Block b) const
{
    FSBlock *ptr = blockPtr(b);
    
    // Jump to linked block
    if (ptr) { ptr = blockPtr(ptr->tsLink()); }
    
    return ptr;
}

FSBlock *
FileSystem::nextBlockPtr(FSBlock *ptr) const
{
    return ptr ? blockPtr(ptr->tsLink()) : nullptr;
}

PETName<16>
FileSystem::getName() const
{
    FSBlock *bam = bamPtr();
    return PETName<16>(bam->data + 0x90);
}

void
FileSystem::setName(PETName<16> name)
{
    FSBlock *bam = bamPtr();
    name.write(bam->data + 0x90);
}

bool
FileSystem::isFree(TSLink ts) const
{
    isize byte, bit;
    FSBlock *bam = locateAllocBit(ts, &byte, &bit);
    
    return GET_BIT(bam->data[byte], bit);
}

TSLink
FileSystem::nextFreeBlock(TSLink ref) const
{
    if (!layout.isValidLink(ref)) return {0,0};
    
    while (ref.t && !isFree(ref)) {
        ref = layout.nextBlockRef(ref);
    }
    
    return ref;
}

void
FileSystem::setAllocBit(TSLink ts, bool value)
{
    isize byte, bit;
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
FileSystem::allocate(TSLink ts, u32 n)
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
            block->data[0] = (u8)ts.t;
            block->data[1] = (u8)ts.s;
        }
        
        // Delete the block reference in the last block
        block->data[0] = 0;
        block->data[1] = 0;
    }
    
    return result;
}

FSBlock *
FileSystem::locateAllocBit(Block b, isize *byte, isize *bit) const
{
    return locateAllocBit(layout.tsLink(b), byte, bit);
}

FSBlock *
FileSystem::locateAllocBit(TSLink ts, isize *byte, isize *bit) const
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
FileSystem::fileName(isize nr) const
{
    assert(nr < numFiles());
    return fileName(dir[nr]);
}

PETName<16>
FileSystem::fileName(FSDirEntry *entry) const
{
    assert(entry);
    return PETName<16>(entry->fileName);
}

FSFileType
FileSystem::fileType(isize nr) const
{
    assert(nr < numFiles());
    return fileType(dir[nr]);
}

FSFileType
FileSystem::fileType(FSDirEntry *entry) const
{
    assert(entry);
    return entry->getFileType();
}

u64
FileSystem::fileSize(isize nr) const
{
    assert(nr < numFiles());
    return fileSize(dir[nr]);
}

u64
FileSystem::fileSize(FSDirEntry *entry) const
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
FileSystem::fileBlocks(isize nr) const
{
    assert(nr < numFiles());
    return fileBlocks(dir[nr]);
}

u64
FileSystem::fileBlocks(FSDirEntry *entry) const
{
    assert(entry);
    return HI_LO(entry->fileSizeHi, entry->fileSizeLo);
}

u16
FileSystem::loadAddr(isize nr) const
{
    assert(nr < numFiles());
    return loadAddr(dir[nr]);
}

u16
FileSystem::loadAddr(FSDirEntry *entry) const
{
    assert(entry);
    u8 addr[2]; copyFile(entry, addr, 2);
    return LO_HI(addr[0], addr[1]);
}

void
FileSystem::copyFile(isize nr, u8 *buf, u64 len, u64 offset) const
{
    assert(nr < numFiles());
    copyFile(dir[nr], buf, len, offset);
}

void
FileSystem::copyFile(FSDirEntry *entry, u8 *buf, u64 len, u64 offset) const
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
FileSystem::getOrCreateNextFreeDirEntry()
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
        ptr->data[0] = (u8)ts.t;
        ptr->data[1] = (u8)ts.s;
    }
    
    return nullptr;
}

void
FileSystem::scanDirectory(bool skipInvisible)
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
FileSystem::makeFile(PETName<16> name, const u8 *buf, isize cnt)
{
    // Search the next free directory slot
    FSDirEntry *dir = getOrCreateNextFreeDirEntry();

    // Create the file if we've found a free slot
    if (dir) return makeFile(name, dir, buf, cnt);
         
    return false;
}

bool
FileSystem::makeFile(PETName<16> name, FSDirEntry *dir, const u8 *buf, isize cnt)
{
    // Determine the number of blocks needed for this file
    u32 numBlocks = (u32)((cnt + 253) / 254);
        
    // Allocate data blocks
    auto blockList = allocate(numBlocks);
    if (blockList.empty()) return false;
        
    auto it = blockList.begin();
    FSBlock *ptr = blockPtr(*it);
    
    // Write data
    isize i, j;
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
    ptr->data[1] = (u8)((cnt % 254) + 1);
    
    // Write directory entry
    dir->init(name, blockList[0], numBlocks);
    
    return true;
}

FSErrorReport
FileSystem::check(bool strict)
{
    FSErrorReport result;

    long total = 0, min = LONG_MAX, max = 0;
    isize numBlocks = blocks.size();
    
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
FileSystem::check(u32 blockNr, u32 pos, u8 *expected, bool strict)
{
    return blocks[blockNr]->check(pos, expected, strict);
}

u32
FileSystem::getCorrupted(u32 blockNr) const
{
    return blockPtr(blockNr) ? blocks[blockNr]->corrupted : 0;
}

bool
FileSystem::isCorrupted(u32 blockNr, u32 n) const
{
    isize numBlocks = blocks.size();
    
    for (u32 i = 0, cnt = 0; i < numBlocks; i++) {
        
        if (isCorrupted(i)) {
            cnt++;
            if (blockNr == i) return cnt == n;
        }
    }
    return false;
}

u32
FileSystem::nextCorrupted(u32 blockNr) const
{
    isize numBlocks = blocks.size();
    
    for (u32 i = blockNr + 1; i < numBlocks; i++) {
        if (isCorrupted(i)) return i;
    }
    return blockNr;
}

u32
FileSystem::prevCorrupted(u32 blockNr) const
{
    isize numBlocks = blocks.size();
    
    for (u32 i = blockNr - 1; i < numBlocks; i--) {
        if (isCorrupted(i)) return i;
    }
    return blockNr;
}

u8
FileSystem::readByte(Block block, u32 offset) const
{
    assert(offset < 256);
    assert(block < (Block)blocks.size());
    
    return blocks[block]->data[offset];
}

void
FileSystem::importVolume(const u8 *src, isize size)
{
    ErrorCode err;
    importVolume(src, size, &err);
    if (err != ERROR_OK) { throw VC64Error(err); }
}

bool
FileSystem::importVolume(const u8 *src, isize size, ErrorCode *err)
{
    assert(src != nullptr);

    debug(FS_DEBUG, "Importing file system (%zu bytes)...\n", size);

    // Only proceed if the buffer size matches
    if ((isize)blocks.size() * 256 > size) {
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
    
    if constexpr (FS_DEBUG) {
        // info();
        // dump();
        printDirectory();
    }
    
    return true;
}

void
FileSystem::importDirectory(const string &path)
{
    fs::directory_entry dir;

    try { dir = fs::directory_entry(path); }
    catch (...) { throw VC64Error(ERROR_FILE_CANT_READ); }

    importDirectory(dir);
}

void
FileSystem::importDirectory(const fs::directory_entry &dir)
{
    for (const auto& entry : fs::directory_iterator(dir)) {

        const auto path = entry.path().string();
        const auto name = entry.path().filename().string();

        // Skip all hidden files
        if (name[0] == '.') continue;

        debug(FS_DEBUG, "Importing %s\n", path.c_str());

        if (entry.is_regular_file()) {

            // Read file
            Buffer<u8> buffer(path);
            if (buffer) {

                // Add file
                PETName<16> pet = PETName<16>(util::stripSuffix(name));
                if (!makeFile(pet, buffer.ptr, buffer.size)) {

                    throw VC64Error(ERROR_FS_CANT_IMPORT);
                }
            }
        }
    }
}

bool
FileSystem::exportVolume(u8 *dst, isize size, ErrorCode *err)
{
    return exportBlocks(0, layout.numBlocks() - 1, dst, size, err);
}

bool
FileSystem::exportBlock(isize nr, u8 *dst, isize size, ErrorCode *err)
{
    return exportBlocks(nr, nr, dst, size, err);
}

bool
FileSystem::exportBlocks(isize first, isize last, u8 *dst, isize size, ErrorCode *err)
{
    assert(last < (u32)layout.numBlocks());
    assert(first <= last);
    assert(dst);
    
    isize count = last - first + 1;
    
    debug(FS_DEBUG, "Exporting %zd blocks (%zd - %zd)\n", count, first, last);

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

void
FileSystem::exportDirectory(const string &path)
{
    isize numItems = util::numDirectoryItems(path);

    // Only proceed if path points to an empty directory
    if (numItems != 0) throw VC64Error(ERROR_DIR_NOT_EMPTY);
    
    // Rescan the directory to get the directory cache up to date
    scanDirectory();
    
    // Export all items
    for (auto const& entry : dir) {

        if (entry->getFileType() != FS_FILETYPE_PRG) {
            msg("Skipping file %s\n", entry->getName().c_str());
            continue;
        }
        
        exportFile(entry, path);
    }
    
    msg("Exported %lu items", dir.size());
}

void
FileSystem::exportFile(FSDirEntry *entry, const string &path)
{
    string name = path + "/" + entry->getFileSystemRepresentation();
    debug(FS_DEBUG, "Exporting file to %s\n", name.c_str());

    std::ofstream stream(name);
    if (!stream.is_open()) throw VC64Error(ERROR_FILE_CANT_CREATE);

    exportFile(entry, stream);
}

void
FileSystem::exportFile(FSDirEntry *entry, std::ofstream &stream)
{
    std::set<Block> visited;

    // Start at the first data block
    BlockPtr b = blockPtr(entry->firstBlock());

    // Iterate through the block chain
    while (b && visited.find(b->nr) == visited.end()) {
                
        visited.insert(b->nr);
        BlockPtr next = nextBlockPtr(b);
                
        isize size = next ? 254 : b->data[1] ? b->data[1] - 1 : 0;
        stream.write((char *)(b->data + 2), size);
        
        b = next;
    }
}
