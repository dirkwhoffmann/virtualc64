// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/Amiga/FSBlock.h"
#include "FileSystems/Amiga/FileSystem.h"
#include "utl/io.h"
#include "utl/support.h"
#include <algorithm>
#include <fstream>

namespace retro::vault::amiga {

FSBlock::FSBlock(FileSystem *ref, BlockNr nr) : fs(ref), cache(ref->cache)
{
    this->nr = nr;
}

FSBlock::FSBlock(FileSystem *ref, BlockNr nr, FSBlockType t) : FSBlock(ref, nr)
{
    init(t);
}

FSBlock::~FSBlock()
{

}

void
FSBlock::init(FSBlockType t)
{
    type = t;

    if (type == FSBlockType::UNKNOWN) return;
    if (type == FSBlockType::EMPTY) return;

    // Allocate memory
    auto *bdata = data();

    // Wipe out existing data
    memset(bdata, 0, bsize());
    
    // Initialize
    switch (type) {

        case FSBlockType::BOOT:

            if (nr == 0 && fs->traits.dos != FSFormat::NODOS) {
                bdata[0] = 'D';
                bdata[1] = 'O';
                bdata[2] = 'S';
                bdata[3] = (u8)fs->traits.dos;
            }
            break;

        case FSBlockType::ROOT:

            assert(hashTableSize() == 72);

            set32(0, 2);                         // Type
            set32(3, (u32)hashTableSize());      // Hash table size
            set32(-50, 0xFFFFFFFF);              // Bitmap validity
            setCreationDate(time(nullptr));      // Creation date
            setModificationDate(time(nullptr));  // Modification date
            set32(-1, 1);                        // Sub type
            break;

        case FSBlockType::USERDIR:

            set32(0, 2);                         // Type
            set32(1, nr);                        // Block pointer to itself
            setCreationDate(time(nullptr));      // Creation date
            set32(-1, 2);                        // Sub type
            break;

        case FSBlockType::FILEHEADER:

            set32(0, 2);                         // Type
            set32(1, nr);                        // Block pointer to itself
            setCreationDate(time(nullptr));      // Creation date
            set32(-1, (u32)-3);                  // Sub type
            break;

        case FSBlockType::FILELIST:

            set32(0, 16);                        // Type
            set32(1, nr);                        // Block pointer to itself
            set32(-1, (u32)-3);                  // Sub type
            break;

        case FSBlockType::DATA_OFS:

            set32(0, 8);                         // Block type
            break;

        default:
            break;
    }
}

FSBlock *
FSBlock::make(FileSystem *ref, BlockNr nr, FSBlockType type)
{
    switch (type) {

        case FSBlockType::EMPTY:
        case FSBlockType::BOOT:
        case FSBlockType::ROOT:
        case FSBlockType::BITMAP:
        case FSBlockType::BITMAP_EXT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
        case FSBlockType::DATA_OFS:
        case FSBlockType::DATA_FFS:

            return new FSBlock(ref, nr, type);

        default:
            throw FSError(FSError::FS_WRONG_BLOCK_TYPE);
    }
}

std::vector<BlockNr>
FSBlock::refs(const std::vector<const FSBlock *> blocks)
{
    std::vector<BlockNr> result;
    for (auto &it : blocks) { if (it) result.push_back(it->nr); }
    return result;
}

const char *
FSBlock::objectName() const
{
    switch (type) {

        case FSBlockType::UNKNOWN:     return "FSBlock (Unknown)";
        case FSBlockType::EMPTY:       return "FSBlock (Empty)";
        case FSBlockType::BOOT:        return "FSBlock (Boot)";
        case FSBlockType::ROOT:        return "FSBlock (Root)";
        case FSBlockType::BITMAP:      return "FSBlock (Bitmap)";
        case FSBlockType::BITMAP_EXT:  return "FSBlock (ExtBitmap)";
        case FSBlockType::USERDIR:     return "FSBlock (UserDir)";
        case FSBlockType::FILEHEADER:  return "FSBlock (FileHeader)";
        case FSBlockType::FILELIST:    return "FSBlock (FileList)";
        case FSBlockType::DATA_OFS:    return "FSBlock (OFS)";
        case FSBlockType::DATA_FFS:    return "FSBlock (FFF)";

        default:
            throw FSError(FSError::FS_WRONG_BLOCK_TYPE);
    }
}

bool
FSBlock::is(FSBlockType type) const
{
    return this->type == type;
}

bool
FSBlock::isEmpty() const
{
    return type == FSBlockType::EMPTY;
}

bool
FSBlock::isRoot() const
{
    return type == FSBlockType::ROOT;
}

bool
FSBlock::isFile() const
{
    return type == FSBlockType::FILEHEADER;
}

bool
FSBlock::isDirectory() const
{
    return type == FSBlockType::ROOT || type == FSBlockType::USERDIR;
}

bool
FSBlock::isRegular() const
{
    return isFile() || isDirectory();
}

bool
FSBlock::isData() const
{
    if (fs->traits.ofs()) {
        return type == FSBlockType::DATA_OFS;
    }

    // Note: As FFS data blocks have no header, each block can be a data block.
    return true;
}

FSName
FSBlock::name() const
{
    return isRoot() ? FSName("") : getName();
}

string
FSBlock::cppName() const
{
    return isRoot() ? string("") : getName().cpp_str();
}

string
FSBlock::absName() const
{
    return "/" + relName(fs->root());
}

string
FSBlock::relName() const
{
    return relName(fs->pwd());
}

string
FSBlock::acabsName() const
{
    return absName() + (isDirectory() ? "/" : "");
}

string
FSBlock::acrelName() const
{
    return relName() + (isDirectory() ? "/" : "");
}

string
FSBlock::relName(BlockNr top) const
{
    string result;

    auto nodes = fs->collect(*this, [](auto *node) { return node->getParentDirBlock(); });

    for (auto &it : nodes) {

        if (it->nr == top) break;
        auto name = it->cppName();
        result = name + "/" + result;
    }

    return utl::trim(result, "/");
}

fs::path
FSBlock::sanitizedPath() const
{
    fs::path result;

    auto nodes = fs->collect(*this, [](auto *node) { return node->getParentDirBlock(); });

    // Delete the root node
    if (!nodes.empty()) nodes.pop_back();

    for (auto &it : nodes) {

        auto name = it->getName().path();
        result = result.empty() ? name : name / result;
    }

    return result;
}

bool
FSBlock::matches(const FSPattern &pattern) const
{
    if (pattern.isAbsolute()) {
        printf("Abs matching %s and %s (%d)\n", absName().c_str(), pattern.glob.c_str(), pattern.match(absName()));
        return pattern.match(absName());
    } else {
        printf("Rel matching %s and %s (%d)\n", relName().c_str(), pattern.glob.c_str(), pattern.match(relName()));
        return pattern.match(cppName());
    }
}

isize
FSBlock::bsize() const
{
    return fs->traits.bsize;
}

isize
FSBlock::dsize() const
{
    switch (type) {

        case FSBlockType::DATA_OFS: return bsize() - 24;
        case FSBlockType::DATA_FFS: return bsize();
        case FSBlockType::EMPTY:    return bsize();

        default:
            fatalError;
    }
}

FSItemType
FSBlock::itemType(isize byte) const
{
    // Translate the byte index to a (signed) long word index
    isize word = byte / 4; if (word >= 6) word -= bsize() / 4;

    switch (type) {

        case FSBlockType::EMPTY:

            return FSItemType::UNUSED;

        case FSBlockType::BOOT:

            if (nr == 0) {

                if (byte <= 2) return FSItemType::DOS_HEADER;
                if (byte == 3) return FSItemType::DOS_VERSION;
                if (byte <= 7) return FSItemType::CHECKSUM;
            }

            return FSItemType::BOOTCODE;

        case FSBlockType::ROOT:

            if (byte == 432) return FSItemType::BCPL_STRING_LENGTH;

            switch (word) {
                case 0:   return FSItemType::TYPE_ID;
                case 1:
                case 2:   return FSItemType::UNUSED;
                case 3:   return FSItemType::HASHTABLE_SIZE;
                case 4:   return FSItemType::UNUSED;
                case 5:   return FSItemType::CHECKSUM;
                case -50: return FSItemType::BITMAP_VALIDITY;
                case -24: return FSItemType::BITMAP_EXT_BLOCK_REF;
                case -23: return FSItemType::MODIFIED_DAY;
                case -22: return FSItemType::MODIFIED_MIN;
                case -21: return FSItemType::MODIFIED_TICKS;
                case -7:  return FSItemType::CREATED_DAY;
                case -6:  return FSItemType::CREATED_MIN;
                case -5:  return FSItemType::CREATED_TICKS;
                case -4:
                case -3:
                case -2:  return FSItemType::UNUSED;
                case -1:  return FSItemType::SUBTYPE_ID;

                default:

                    if (word <= -51)                return FSItemType::HASH_REF;
                    if (word <= -25)                return FSItemType::BITMAP_BLOCK_REF;
                    if (word >= -20 && word <= -8)  return FSItemType::BCPL_DISK_NAME;
            }

            fatalError;

        case FSBlockType::BITMAP:

            return byte < 4 ? FSItemType::CHECKSUM : FSItemType::BITMAP;

        case FSBlockType::BITMAP_EXT:

            return byte < (bsize() - 4) ? FSItemType::BITMAP : FSItemType::BITMAP_EXT_BLOCK_REF;

        case FSBlockType::USERDIR:

            if (byte == 328) return FSItemType::BCPL_STRING_LENGTH;
            if (byte == 432) return FSItemType::BCPL_STRING_LENGTH;

            switch (word) {
                case 0:   return FSItemType::TYPE_ID;
                case 1:   return FSItemType::SELF_REF;
                case 2:
                case 3:
                case 4:   return FSItemType::UNUSED;
                case 5:   return FSItemType::CHECKSUM;
                case -50:
                case -49: return FSItemType::UNUSED;
                case -48: return FSItemType::PROT_BITS;
                case -47: return FSItemType::UNUSED;
                case -23: return FSItemType::CREATED_DAY;
                case -22: return FSItemType::CREATED_MIN;
                case -21: return FSItemType::CREATED_TICKS;
                case -4:  return FSItemType::NEXT_HASH_REF;
                case -3:  return FSItemType::PARENT_DIR_REF;
                case -2:  return FSItemType::UNUSED;
                case -1:  return FSItemType::SUBTYPE_ID;
            }

            if (word <= -51)                return FSItemType::HASH_REF;
            if (word >= -46 && word <= -24) return FSItemType::BCPL_COMMENT;
            if (word >= -20 && word <= -5)  return FSItemType::BCPL_DIR_NAME;

            fatalError;

        case FSBlockType::FILEHEADER:

            if (byte == 328) return FSItemType::BCPL_STRING_LENGTH;
            if (byte == 432) return FSItemType::BCPL_STRING_LENGTH;

            switch (word) {
                case 0:   return FSItemType::TYPE_ID;
                case 1:   return FSItemType::SELF_REF;
                case 2:   return FSItemType::DATA_BLOCK_REF_COUNT;
                case 3:   return FSItemType::UNUSED;
                case 4:   return FSItemType::FIRST_DATA_BLOCK_REF;
                case 5:   return FSItemType::CHECKSUM;
                case -50:
                case -49: return FSItemType::UNUSED;
                case -48: return FSItemType::PROT_BITS;
                case -47: return FSItemType::FILESIZE;
                case -23: return FSItemType::CREATED_DAY;
                case -22: return FSItemType::CREATED_MIN;
                case -21: return FSItemType::CREATED_TICKS;
                case -4:  return FSItemType::NEXT_HASH_REF;
                case -3:  return FSItemType::PARENT_DIR_REF;
                case -2:  return FSItemType::EXT_BLOCK_REF;
                case -1:  return FSItemType::SUBTYPE_ID;
            }

            if (word <= -51)                return FSItemType::DATA_BLOCK_REF;
            if (word >= -46 && word <= -24) return FSItemType::BCPL_COMMENT;
            if (word >= -20 && word <= -5)  return FSItemType::BCPL_FILE_NAME;

            fatalError;

        case FSBlockType::FILELIST:

            if (byte == 328) return FSItemType::BCPL_STRING_LENGTH;
            if (byte == 432) return FSItemType::BCPL_STRING_LENGTH;

            switch (word) {

                case 0:   return FSItemType::TYPE_ID;
                case 1:   return FSItemType::SELF_REF;
                case 2:   return FSItemType::DATA_BLOCK_REF_COUNT;
                case 3:   return FSItemType::UNUSED;
                case 4:   return FSItemType::FIRST_DATA_BLOCK_REF;
                case 5:   return FSItemType::CHECKSUM;
                case -50:
                case -49:
                case -4:  return FSItemType::UNUSED;
                case -3:  return FSItemType::FILEHEADER_REF;
                case -2:  return FSItemType::EXT_BLOCK_REF;
                case -1:  return FSItemType::SUBTYPE_ID;
            }

            return word <= -51 ? FSItemType::DATA_BLOCK_REF : FSItemType::UNUSED;

        case FSBlockType::DATA_OFS:

            switch (word) {

                case 0: return FSItemType::TYPE_ID;
                case 1: return FSItemType::FILEHEADER_REF;
                case 2: return FSItemType::DATA_BLOCK_NUMBER;
                case 3: return FSItemType::DATA_COUNT;
                case 4: return FSItemType::NEXT_DATA_BLOCK_REF;
                case 5: return FSItemType::CHECKSUM;
            }

            return FSItemType::DATA;

        case FSBlockType::DATA_FFS:

            return FSItemType::DATA;

        default:
            fatalError;
    }
}

u32
FSBlock::typeID() const
{
    return type == FSBlockType::EMPTY ? 0 : get32(0);
}

u32
FSBlock::subtypeID() const
{
    return type == FSBlockType::EMPTY ? 0 : get32((bsize() / 4) - 1);
}

const u8 *
FSBlock::addr32(isize nr) const
{
    return (data() + 4 * nr) + (nr < 0 ? bsize() : 0);
}

u8 *
FSBlock::addr32(isize nr)
{
    return (data() + 4 * nr) + (nr < 0 ? bsize() : 0);
}

u8 *
FSBlock::data()
{
    if (dataCache.empty()) {

        dataCache.alloc(bsize());
        cache.dev.readBlock(dataCache.ptr, nr);
    }

    assert(dataCache.size == bsize());
    assert(dataCache.ptr);

    return dataCache.ptr;
}

const u8 *
FSBlock::data() const
{
    return const_cast<const u8 *>(const_cast<FSBlock *>(this)->data());
}

FSBlock &
FSBlock::mutate() const
{
    cache.markAsDirty(nr);
    return const_cast<FSBlock &>(*this);
}

void
FSBlock::invalidate()
{
    cache.markAsDirty(nr);
}

void
FSBlock::flush()
{
    if (!dataCache.empty()) {
        
        cache.dev.writeBlock(dataCache.ptr, nr);
    }
}

u32
FSBlock::read32(const u8 *p)
{
    return p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
}

void
FSBlock::write32(u8 *p, u32 value)
{
    p[0] = (value >> 24) & 0xFF;
    p[1] = (value >> 16) & 0xFF;
    p[2] = (value >>  8) & 0xFF;
    p[3] = (value >>  0) & 0xFF;
}

isize
FSBlock::checksumLocation() const
{
    switch (type) {

        case FSBlockType::BOOT:

            return nr == 0 ? 1 : -1;

        case FSBlockType::BITMAP:

            return 0;

        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
        case FSBlockType::DATA_OFS:

            return 5;

        default:

            return -1;
    }
}

u32
FSBlock::checksum() const
{
    return type == FSBlockType::BOOT ? checksumBootBlock() : checksumStandard();
}

u32
FSBlock::checksumStandard() const
{
    isize pos = checksumLocation();
    assert(pos >= 0 && pos <= 5);

    // Wipe out the old checksum
    u32 old = get32(pos);
    const_cast<FSBlock *>(this)->set32(pos, 0); // TODO: DON'T DO THIS

    // Compute the new checksum
    u32 result = 0;
    for (isize i = 0; i < bsize() / 4; i++)  U32_INC(result, get32(i));
    result = ~result;
    U32_INC(result, 1);

    // Undo the modification
    const_cast<FSBlock *>(this)->set32(pos, old); // TODO: DON'T DO THIS

    // Compute the new checksum
    u32 result2 = 0;
    for (isize i = 0; i < bsize() / 4; i++)  if (i != pos) U32_INC(result2, get32(i));
    result2 = ~result2;
    U32_INC(result2, 1);

    assert(result == result2);

    return result;
}

u32
FSBlock::checksumBootBlock() const
{
    // Only call this function for the first boot block in a partition
    assert(nr == 0);

    u32 result = get32(0), prec;

    // First boot block
    for (isize i = 2; i < bsize() / 4; i++) {

        prec = result;
        if ( (result += get32(i)) < prec) result++;
    }

    // Second boot block
    auto *p = fs->cache[1].data();

    for (isize i = 0; i < bsize() / 4; i++) {

        prec = result;
        if ( (result += FSBlock::read32(p + 4*i)) < prec) result++;
    }

    return ~result;
}

void
FSBlock::updateChecksum()
{
    isize pos = checksumLocation();
    if (pos >= 0 && pos < bsize() / 4) set32(pos, checksum());
}

void
FSBlock::dumpInfo(std::ostream &os) const
{
    using namespace utl;

    auto byteStr = [&os](isize num) {

        auto str = std::to_string(num) + " Byte" + (num == 1 ? "" : "s");
        os << std::setw(13) << std::left << std::setfill(' ') << str;
    };
    auto blockStr = [&os](isize num) {

        auto str = std::to_string(num) + " Block" + (num == 1 ? "" : "s");
        os << std::setw(13) << std::left << std::setfill(' ') << str;
    };

    switch (type) {

        case FSBlockType::FILEHEADER:
        {
            auto name = getName().cpp_str();
            auto size = getFileSize();
            auto listBlocks = isize(fs->collectListBlocks(nr).size());
            auto dataBlocks = isize(fs->collectDataBlocks(nr).size());
            auto totalBlocks = 1 + listBlocks + dataBlocks;
            auto tab = int(name.size()) + 4;

            os << std::setw(tab) << std::left << "Name";
            os << "Size         Header       Lists        Data         Total" << std::endl;

            os << std::setw(tab) << std::left << name;
            byteStr(size);
            blockStr(1);
            blockStr(listBlocks);
            blockStr(dataBlocks);
            blockStr(totalBlocks);
            os << std::endl;
        }
        default:
            break;
    }
}

void
FSBlock::dumpBlocks(std::ostream &os) const
{
    using namespace utl;

    switch (type) {

        case FSBlockType::FILEHEADER:
        {
            auto size = getFileSize();
            auto listBlocks = fs->collectListBlocks(nr);
            auto dataBlocks = fs->collectDataBlocks(nr);
            auto totalBlocks = 1 + listBlocks.size() + dataBlocks.size();

            os << tab("Name");
            os << getName().cpp_str() << std::endl;
            os << tab("Blocks");
            os << totalBlocks << " Block" << (totalBlocks == 1 ? "" : "s") << std::endl;
            os << tab("Size");
            os << size << " Byte" << (size == 1 ? "" : "s") << std::endl;
            os << tab("File header block");
            os << nr << std::endl;
            os << tab("File list blocks");
            os << FSBlock::rangeString(listBlocks) << std::endl;
            os << tab("Data blocks");
            os << FSBlock::rangeString(dataBlocks) << std::endl;
        }
        default:
            break;
    }
}

void
FSBlock::dumpStorage(std::ostream &os) const
{
    fs->doctor.dump(nr, os);
}

/*
void
FSBlock::hexDump(std::ostream &os, const DumpOpt &opt) const
{
    if (type == FSBlockType::EMPTY) {

        Dumpable::dump(os, [&](isize offset, isize bytes) { return offset < bsize() ? 0 : -1; }, opt, DumpFmt{} );

    } else {

        Dumpable::dump(os, Dumpable::dataProvider(data(), bsize()), opt, DumpFmt{});
    }
}
*/

string
FSBlock::rangeString(const std::vector<BlockNr> &vec)
{
    if (vec.empty()) return "";

    // Create a sorted copy
    std::vector<BlockNr> v = vec;
    std::sort(v.begin(), v.end());

    // Replicate the last element to get the last interval right
    v.push_back(v.back());

    isize start = v[0], end = v[0];
    std::vector<string> chunks;

    for (usize i = 1; i < v.size(); i++) {

        if (v[i - 1] + 1 != v[i]) {

            if (start == end) chunks.push_back(std::to_string(start));
            else chunks.push_back(std::to_string(start) + " - " + std::to_string(end));
            start = v[i];
        }
        end = v[i];
    }

    return utl::concat(chunks, ", ");
}

void
FSBlock::importBlock(const u8 *src, isize size)
{    
    assert(src);
    assert(size == bsize());

    auto *bdata = data();

    if (bdata) {
        std::memcpy(data(), src, size);
    }
}

void
FSBlock::exportBlock(u8 *dst, isize size) const
{
    assert(dst);
    assert(size == bsize());

    auto *bdata = data();

    if (bdata) {
        std::memcpy(dst, data(), size);
    } else {
        std::memset(dst, 0, size);
    }
}

FSFault
FSBlock::exportBlock(const fs::path &path) const
{
    switch (type) {
            
        case FSBlockType::USERDIR:    return exportUserDirBlock(path);
        case FSBlockType::FILEHEADER: return exportFileHeaderBlock(path);
            
        default:
            return FSError::FS_OK;
    }
}

FSFault
FSBlock::exportUserDirBlock(const fs::path &path) const
{
    // Assemble the host file name
    auto filename = path / sanitizedPath();
    loginfo(FS_DEBUG >= 2, "Creating directory %s\n", filename.string().c_str());

    // Create directory
    if (!utl::createDirectory(filename)) return FSError::FS_EXPORT_ERROR;

    return FSError::FS_OK;
}

FSFault
FSBlock::exportFileHeaderBlock(const fs::path &path) const
{
    // Assemble the host file name
    auto filename = path; //  / sanitizedPath();
    loginfo(FS_DEBUG >= 2, "  Exporting file %s\n", filename.string().c_str());

    // Open file
    std::ofstream file(filename, std::ofstream::binary);
    if (!file.is_open()) return FSError::FS_EXPORT_ERROR;

    // Write data
    writeData(file);
    return FSError::FS_OK;
}

bool
FSBlock::hasName() const
{
    switch (type) {

        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:

            return true;

        default:

            return false;
    }
}

FSName
FSBlock::getName() const
{
    switch (type) {

        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
            
            return FSName(addr32(-20));

        default:
            
            return FSName("");
    }
}

void
FSBlock::setName(FSName name)
{
    switch (type) {

        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
            
            name.write(addr32(-20));

        default:
            break;
    }
}

bool
FSBlock::isNamed(const FSName &other) const
{
    switch (type) {

        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
            
            return getName() == other;
            
        default:
            
            return false;
    }
}

FSComment
FSBlock::getComment() const
{
    switch (type) {

        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:

            return FSComment(addr32(-46));

        default:
            
            return FSComment("");
    }
}

void
FSBlock::setComment(FSComment name)
{
    switch (type) {

        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
            
            name.write(addr32(-46));

        default:
            
            break;
    }
}

FSTime
FSBlock::getCreationDate() const
{
    switch (type) {
            
        case FSBlockType::ROOT:
            
            return FSTime(addr32(-7));
            
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
            
            return FSTime(addr32(-23));
            
        default:
            return FSTime((time_t)0);
    }
}

void
FSBlock::setCreationDate(FSTime t)
{
    switch (type) {
            
        case FSBlockType::ROOT:

            t.write(addr32(-7));
            break;
            
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:

            t.write(addr32(-23));
            break;

        default:
            break;
    }
}

FSTime
FSBlock::getModificationDate() const
{
    switch (type) {
            
        case FSBlockType::ROOT:
            
            return FSTime(addr32(-23));

        default:
            return FSTime((time_t)0);
    }
}

void
FSBlock::setModificationDate(FSTime t)
{
    switch (type) {
            
        case FSBlockType::ROOT:
            
            t.write(addr32(-23));
            break;

        default:
            break;
    }
}

u32
FSBlock::getProtectionBits() const
{
    switch (type) {

        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:

            return get32(-48);

        default:
            return 0;
    }
}

void
FSBlock::setProtectionBits(u32 val)
{
    switch (type) {

        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:

            set32(-48, val);
            break;
            
        default:
            break;
    }
}

string
FSBlock::getProtectionBitString() const
{
    auto bits = getProtectionBits();

    // From dos/dos.h (AmigaDOS)
    constexpr isize FIBB_SCRIPT  = 6; // program is a script (execute) file
    constexpr isize FIBB_PURE    = 5; // program is reentrant and rexecutable
    constexpr isize FIBB_ARCHIVE = 4; // cleared whenever file is changed
    constexpr isize FIBB_READ    = 3; // ignored by old filesystem
    constexpr isize FIBB_WRITE   = 2; // ignored by old filesystem
    constexpr isize FIBB_EXECUTE = 1; // ignored by system, used by Shell
    constexpr isize FIBB_DELETE  = 0; // prevent file from being deleted

    string result;
    result += (bits & (1 << 7))            ? "h" : "-";
    result += (bits & (1 << FIBB_SCRIPT))  ? "s" : "-";
    result += (bits & (1 << FIBB_PURE))    ? "p" : "-";
    result += (bits & (1 << FIBB_ARCHIVE)) ? "a" : "-";
    result += (bits & (1 << FIBB_READ))    ? "-" : "r";
    result += (bits & (1 << FIBB_WRITE))   ? "-" : "w";
    result += (bits & (1 << FIBB_EXECUTE)) ? "-" : "e";
    result += (bits & (1 << FIBB_DELETE))  ? "-" : "d";

    return result;
}

u32
FSBlock::getFileSize() const
{
    switch (type) {

        case FSBlockType::FILEHEADER:

            return get32(-47);

        default:
            return 0;
    }
}

void
FSBlock::setFileSize(u32 val)
{
    switch (type) {

        case FSBlockType::FILEHEADER:

            set32(-47, val);
            break;
            
        default:
            break;
    }
}

bool
FSBlock::hasHeaderKey() const
{
    switch (type) {

        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
        case FSBlockType::DATA_OFS:

            return true;

        default:

            return false;
    }
}

u32
FSBlock::getHeaderKey() const
{
    switch (type) {

        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
        case FSBlockType::DATA_OFS:

            return get32(1);

        default:

            return 0;
    }
}

void
FSBlock::setHeaderKey(u32 val)
{
    switch (type) {

        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
        case FSBlockType::DATA_OFS:

            set32(1, val);
            break;

        default:
            break;
    }
}

bool
FSBlock::hasChecksum() const
{
    switch (type) {

        case FSBlockType::BOOT:

            return nr == 0;

        case FSBlockType::BITMAP:
        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
        case FSBlockType::DATA_OFS:

            return true;

        default:

            return false;
    }
}

u32
FSBlock::getChecksum() const
{
    switch (type) {

        case FSBlockType::BOOT:

            return nr == 0 ? get32(1) : 0;

        case FSBlockType::BITMAP:

            return get32(0);

        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
        case FSBlockType::DATA_OFS:

            return get32(5);

        default:

            return 0;
    }
}

void
FSBlock::setChecksum(u32 val)
{
    switch (type) {

        case FSBlockType::BOOT:

            if (nr == 0) set32(1, val);

        case FSBlockType::BITMAP:

            set32(0, val);

        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
        case FSBlockType::DATA_OFS:

            set32(5, val);

        default:

            break;
    }
}

BlockNr
FSBlock::getParentDirRef() const
{
    switch (type) {

        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:

            return get32(-3);

        default:
            return 0;
    }
}

void
FSBlock::setParentDirRef(BlockNr ref)
{
    switch (type) {

        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:

            set32(-3, ref);
            break;
            
        default:
            break;
    }
}

const FSBlock *
FSBlock::getParentDirBlock() const
{
    BlockNr ref = getParentDirRef();
    return ref ? &fs->fetch(ref) : nullptr;
}

BlockNr
FSBlock::getFileHeaderRef() const
{
    switch (type) {
            
        case FSBlockType::FILELIST:  return get32(-3);
        case FSBlockType::DATA_OFS:  return get32(1);
            
        default:
            return 0;
    }
}

void
FSBlock::setFileHeaderRef(BlockNr ref)
{
    switch (type) {

        case FSBlockType::FILELIST:

            set32(-3, ref);
            break;

        case FSBlockType::DATA_OFS:

            set32(1, ref);
            break;

        default:
            break;
    }
}

const FSBlock *
FSBlock::getFileHeaderBlock() const
{
    BlockNr ref = getFileHeaderRef();
    if (auto &result = fs->fetch(ref); result.isFile()) {
        return &result;
    }
    return nullptr;
}

BlockNr
FSBlock::getNextHashRef() const
{
    switch (type) {

        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:

            return get32(-4);
            
        default:
            return 0;
    }
}

void
FSBlock::setNextHashRef(BlockNr ref)
{
    switch (type) {

        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:

            set32(-4, ref);
            break;
            
        default:
            break;
    }
}

const FSBlock *
FSBlock::getNextHashBlock() const
{
    BlockNr ref = getNextHashRef();
    return ref ? &fs->fetch(ref) : nullptr;
}

BlockNr
FSBlock::getNextListBlockRef() const
{
    switch (type) {

        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
            
            return get32(-2);
            
        default:
            return 0;
    }
}

void
FSBlock::setNextListBlockRef(BlockNr ref)
{
    switch (type) {

        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
            
            set32(-2, ref);
            break;
            
        default:
            break;
    }
}

const FSBlock *
FSBlock::getNextListBlock() const
{
    BlockNr ref = getNextListBlockRef();
    if (auto &node = fs->fetch(ref); node.is(FSBlockType::FILELIST)) {
        return &node;
    }
    return nullptr;
    // return fs->tryModify(getNextListBlockRef(), FSBlockType::FILELIST);
}

BlockNr
FSBlock::getNextBmExtBlockRef() const
{
    switch (type) {
            
        case FSBlockType::ROOT:        return get32(-24);
        case FSBlockType::BITMAP_EXT:  return get32(-1);
            
        default:
            return 0;
    }
}

void
FSBlock::setNextBmExtBlockRef(BlockNr ref)
{
    switch (type) {
            
        case FSBlockType::ROOT:

            set32(-24, ref);
            break;

        case FSBlockType::BITMAP_EXT:

            set32(-1, ref);
            break;

        default:
            break;
    }
}

const FSBlock *
FSBlock::getNextBmExtBlock() const
{
    if (BlockNr ref = getNextBmExtBlockRef()) {
        if (auto &node = fs->fetch(ref); node.is(FSBlockType::BITMAP_EXT)) {
            return &node;
        }
    }
    return nullptr;
}

BlockNr
FSBlock::getFirstDataBlockRef() const
{
    switch (type) {
            
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
            
            return get32(4);
            
        default:
            return 0;
    }
}

void
FSBlock::setFirstDataBlockRef(BlockNr ref)
{
    switch (type) {

        case FSBlockType::FILEHEADER:

            set32(4, ref);
            break;
            
        default:
            break;
    }
}

const FSBlock *
FSBlock::getFirstDataBlock() const
{
    BlockNr ref = getFirstDataBlockRef();
    if (auto &node = fs->fetch(ref); node.isData()) {
        return &node;
    }
    return nullptr;

//    if (auto *node = fs->tryModify(getFirstDataBlockRef()); node->isData()) return node;
//    return nullptr;
}

BlockNr
FSBlock::getDataBlockRef(isize nr) const
{
    switch (type) {
            
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
            
            return get32(-51 - nr);
            
        default:
            fatalError;
    }
}

void
FSBlock::setDataBlockRef(isize nr, BlockNr ref)
{
    switch (type) {
            
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
            
            set32(-51-nr, ref);
            return;
            
        default:
            fatalError;
    }
}

const FSBlock *
FSBlock::getDataBlock(isize nr) const
{
    BlockNr ref = getDataBlockRef(nr);
    if (auto &node = fs->fetch(ref); node.isData()) {
        return &node;
    }
    return nullptr;

    // if (auto *node = fs->tryModify(getDataBlockRef(nr)); node->isData()) return node;
    // return nullptr;
}

BlockNr
FSBlock::getNextDataBlockRef() const
{
    return type == FSBlockType::DATA_OFS ? get32(4) : 0;
}

void
FSBlock::setNextDataBlockRef(BlockNr ref)
{
    if (type == FSBlockType::DATA_OFS) {

        set32(4, ref);
    }
}

const FSBlock *
FSBlock::getNextDataBlock() const
{
    BlockNr ref = getNextDataBlockRef();
    if (auto &node = fs->fetch(ref); node.isData()) {
        return &node;
    }
    return nullptr;

    // if (auto *node = fs->tryModify(getNextDataBlockRef()); node->isData()) return node;
    // return nullptr;
}

bool
FSBlock::isHashable() const
{
    return type == FSBlockType::FILEHEADER || type == FSBlockType::USERDIR;
}

isize
FSBlock::hashTableSize() const
{
    switch (type) {
            
        case FSBlockType::ROOT:
        case FSBlockType::USERDIR:
            
            assert(bsize() != 512 || (bsize() / 4) - 56 == 72);
            return (bsize() / 4) - 56;

        default:
            return 0;
    }
}

u32
FSBlock::hashValue() const
{
    switch (type) {
            
        case FSBlockType::USERDIR:
        case FSBlockType::FILEHEADER:
            
            return getName().hashValue(fs->traits.dos);

        default:
            return 0;
    }
}

BlockNr
FSBlock::getHashRef(BlockNr nr) const
{
    return (nr < (BlockNr)hashTableSize()) ? get32(6 + nr) : 0;
}

void
FSBlock::setHashRef(BlockNr nr, BlockNr ref)
{
    if (nr < (BlockNr)hashTableSize()) {

        set32(6 + nr, ref);
    }
}

void
FSBlock::writeBootBlock(BootBlockId id, isize page)
{
    assert(page == 0 || page == 1);
    assert(type == FSBlockType::BOOT);
    
    loginfo(FS_DEBUG, "writeBootBlock(%s, %ld)\n", BootBlockIdEnum::key(id), page);
    
    if (id != BootBlockId::NONE) {

        // Read boot block image from the database
        auto image = FSBootBlockImage(id);

        if (page == 0) {
            image.write(data() + 4, 4, 511); // Write 508 bytes (skip header)
        } else {
            image.write(data(), 512, 1023);  // Write 512 bytes
        }
    }
}

bool
FSBlock::addBitmapBlockRefs(std::vector<BlockNr> &refs)
{
    assert(type == FSBlockType::ROOT);
    
    auto it = refs.begin();

    // Record the first 25 references in the root block
    for (isize i = 0; i < 25; i++, it++) {
        if (it == refs.end()) return true;
        setBmBlockRef(i, *it);
    }

    // Record the remaining references in bitmap extension blocks
    auto *ext = getNextBmExtBlock();
    while (ext && it != refs.end()) {
        ext->mutate().addBitmapBlockRefs(refs, it);
        ext = getNextBmExtBlock();
    }
    return it == refs.end();
}

void
FSBlock::addBitmapBlockRefs(std::vector<BlockNr> &refs,
                            std::vector<BlockNr>::iterator &it)
{
    assert(type == FSBlockType::BITMAP_EXT);
    
    isize max = (bsize() / 4) - 1;
    
    for (isize i = 0; i < max; i++, it++) {
        if (it == refs.end()) return;
        setBmBlockRef(i, *it);
    }
}

isize
FSBlock::numBmBlockRefs() const
{
    switch (type) {

        case FSBlockType::ROOT:       return 25;
        case FSBlockType::BITMAP_EXT: return (bsize() / 4) - 1;

        default:
            return 0;
    }
}

BlockNr
FSBlock::getBmBlockRef(isize nr) const
{
    switch (type) {
            
        case FSBlockType::ROOT:
            
            return get32(nr - 49);
            
        case FSBlockType::BITMAP_EXT:
            
            return get32(nr);
            
        default:
            fatalError;
    }
}

void
FSBlock::setBmBlockRef(isize nr, BlockNr ref)
{
    switch (type) {
            
        case FSBlockType::ROOT:
            
            set32(nr - 49, ref);
            return;
            
        case FSBlockType::BITMAP_EXT:
            
            set32(nr, ref);
            return;
            
        default:
            fatalError;
    }
}

std::vector<BlockNr>
FSBlock::getBmBlockRefs() const
{
    isize maxRefs =
    type == FSBlockType::ROOT ? 25 :
    type == FSBlockType::BITMAP_EXT ? (bsize() / 4) - 1 : 0;

    std::vector<BlockNr> result;
    for (isize i = 0; i < maxRefs; i++) {
        if (auto ref = getBmBlockRef(i); ref) result.push_back(ref);
    }
    return result;
}

u32
FSBlock::getDataBlockNr() const
{
    switch (type) {
            
        case FSBlockType::DATA_OFS: return get32(2);
        case FSBlockType::DATA_FFS: return 0;

        default:
            fatalError;
    }
}

void
FSBlock::setDataBlockNr(BlockNr val)
{
    switch (type) {
            
        case FSBlockType::DATA_OFS: set32(2, val); break;
        case FSBlockType::DATA_FFS: break;

        default:
            fatalError;
    }
}

isize
FSBlock::getMaxDataBlockRefs() const
{
    return bsize() / 4 - 56;
}

isize
FSBlock::getNumDataBlockRefs() const
{
    switch (type) {
            
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:
            
            return get32(2);

        default:
            return 0;
    }
}

void
FSBlock::setNumDataBlockRefs(u32 val)
{
    switch (type) {
            
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:

            set32(2, val);
            break;

        default:
            break;
    }
}

void
FSBlock::incNumDataBlockRefs()
{
    switch (type) {
            
        case FSBlockType::FILEHEADER:
        case FSBlockType::FILELIST:

            inc32(2);
            break;

        default:
            break;
    }
}

std::vector<BlockNr>
FSBlock::getDataBlockRefs() const
{
    isize maxRefs = getNumDataBlockRefs();

    std::vector<BlockNr> result;
    for (isize i = 0; i < maxRefs; i++) {
        if (auto ref = getDataBlockRef(i); ref) result.push_back(ref);
    }
    return result;
}

void
FSBlock::addDataBlockRef(BlockNr first, BlockNr ref)
{    
    assert(getNumDataBlockRefs() < getMaxDataBlockRefs());
    
    switch (type) {
            
        case FSBlockType::FILEHEADER:
            
            setFirstDataBlockRef(first);
            setDataBlockRef(getNumDataBlockRefs(), ref);
            incNumDataBlockRefs();
            break;
            
        case FSBlockType::FILELIST:
            
            setDataBlockRef(getNumDataBlockRefs(), ref);
            incNumDataBlockRefs();
            break;
            
        default:
            
            break;
    }
}

u32
FSBlock::getDataBytesInBlock() const
{
    switch (type) {
            
        case FSBlockType::DATA_OFS: return get32(3);
        case FSBlockType::DATA_FFS: return 0;

        default:
            fatalError;
    }
}

void
FSBlock::setDataBytesInBlock(u32 val)
{
    switch (type) {
            
        case FSBlockType::DATA_OFS: set32(3, val); break;
        case FSBlockType::DATA_FFS: break;

        default:
            fatalError;
    }
}

isize
FSBlock::writeData(std::ostream &os) const
{
    Buffer<u8> buffer;
    
    // Write block into buffer
    extractData(buffer);
    
    // Export the buffer
    os << buffer;
    
    return buffer.size;
}

isize
FSBlock::writeData(std::ostream &os, isize size) const
{
    isize count = std::min(dsize(), size);
    auto *bdata = data();

    switch (type) {
            
        case FSBlockType::DATA_OFS:
            
            os.write((char *)(bdata + 24), count);
            return count;
            
        case FSBlockType::DATA_FFS:
            
            os.write((char *)bdata, count);
            return count;
            
        default:
            fatalError;
    }
}

isize
FSBlock::extractData(Buffer<u8> &buf) const
{
    // Only call this function for file header blocks
    if (type != FSBlockType::FILEHEADER) throw FSError(FSError::FS_NOT_A_FILE);

    isize bytesRemaining = getFileSize();
    isize bytesTotal = 0;

    buf.init(bytesRemaining);

    for (auto &it : fs->collectDataBlocks(*this)) {

        isize bytesWritten = it->writeData(buf, bytesTotal, bytesRemaining);
        bytesTotal += bytesWritten;
        bytesRemaining -= bytesWritten;
    }

    if (bytesRemaining != 0) {
        logwarn("%ld remaining bytes. Expected 0.\n", bytesRemaining);
    }

    return bytesTotal;

    /*
     isize blocksTotal = 0;
    // Start here and iterate through all connected file list blocks
    const FSBlock *block = this;
    
    while (block && blocksTotal < fs->numBlocks()) {

        blocksTotal++;
        
        // Iterate through all data blocks references in this block
        isize num = std::min(block->getNumDataBlockRefs(), block->getMaxDataBlockRefs());
        for (isize i = 0; i < num; i++) {
            
            Block ref = block->getDataBlockRef(i);
            if (FSBlock *dataBlock = fs->dataBlockPtr(ref)) {

                isize bytesWritten = dataBlock->writeData(buf, bytesTotal, bytesRemaining);
                bytesTotal += bytesWritten;
                bytesRemaining -= bytesWritten;
                
            } else {
                
                warn("Ignoring block %d (no data block)\n", ref);
            }
        }
        
        // Continue with the next list block
        block = block->getNextListBlock();
    }
    
    if (bytesRemaining != 0) {
        warn("%ld remaining bytes. Expected 0.\n", bytesRemaining);
    }

    return bytesTotal;
     */
}

isize
FSBlock::writeData(Buffer<u8> &buf, isize offset, isize count) const
{
    count = std::min(dsize(), count);
    auto *bdata = data();

    switch (type) {
            
        case FSBlockType::DATA_OFS:
            
            std::memcpy((void *)(buf.ptr + offset), (void *)(bdata + 24), count);
            return count;
            
        case FSBlockType::DATA_FFS:

            std::memcpy((void *)(buf.ptr + offset), (void *)(bdata), count);
            return count;

        case FSBlockType::EMPTY:

            std::memset((void *)(buf.ptr + offset), 0, count);
            return count;

        default:
            fatalError;
    }
}

isize
FSBlock::overwriteData(Buffer<u8> &buf)
{
    // Only call this function for file header blocks
    assert(type == FSBlockType::FILEHEADER);
    
    isize bytesRemaining = getFileSize();
    isize bytesTotal = 0;
    isize blocksTotal = 0;
    
    assert(buf.size == bytesRemaining);
    
    // Start here and iterate through all connected file list blocks
    const FSBlock *block = this;

    while (block && blocksTotal < fs->blocks()) {

        blocksTotal++;
        
        // Iterate through all data blocks references in this block
        isize num = std::min(block->getNumDataBlockRefs(), block->getMaxDataBlockRefs());
        for (isize i = 0; i < num; i++) {

            BlockNr ref = block->getDataBlockRef(i);
            if (auto &dataBlock = fs->fetch(ref); dataBlock.isData()) { //} dataBlockPtr(ref)) {

                isize bytesWritten = dataBlock.mutate().overwriteData(buf, bytesTotal, bytesRemaining);
                bytesTotal += bytesWritten;
                bytesRemaining -= bytesWritten;
                
            } else {
                
                logwarn("Ignoring block %ld (no data block)\n", ref);
            }
        }
        
        // Continue with the next list block
        block = block->getNextListBlock();
    }
    
    if (bytesRemaining != 0) {
        logwarn("%ld remaining bytes. Expected 0.\n", bytesRemaining);
    }
    
    return bytesTotal;
}

isize
FSBlock::overwriteData(Buffer<u8> &buf, isize offset, isize count)
{
    count = std::min(dsize(), count);
    auto *bdata = data();

    switch (type) {
            
        case FSBlockType::DATA_OFS:
            
            std::memcpy((void *)(bdata + 24), (void *)(buf.ptr + offset), count);
            return count;
            
        case FSBlockType::DATA_FFS:

            std::memcpy((void *)(bdata), (void *)(buf.ptr + offset), count);
            return count;
            
        default:
            fatalError;
    }
}

}
