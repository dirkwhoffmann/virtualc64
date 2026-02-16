// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/Amiga/FSDoctor.h"
#include "FileSystems/Amiga/FileSystem.h"
#include "utl/io.h"
#include "utl/support.h"
#include <unordered_map>
#include <unordered_set>
#include <sstream>

//
// Macros used inside the check() methods
//

#define EXPECT_VALUE(exp) { \
if ((u32)value != u32(exp)) { expected = u32(exp); return FSBlockError::EXPECTED_VALUE; } }

#define EXPECT_CHECKSUM EXPECT_VALUE(node.checksum())

#define EXPECT_LESS_OR_EQUAL(exp) { \
if ((u32)value > (u32)exp) \
{ expected = (u8)(exp); return FSBlockError::EXPECTED_SMALLER_VALUE; } }

#define EXPECT_REF { \
if (!fs.block(value)) return FSBlockError::EXPECTED_REF; }

#define EXPECT_SELFREF { \
if ((u32)value != (u32)ref) { expected = ref; return FSBlockError::EXPECTED_SELFREF; } }

#define EXPECT_FILEHEADER_REF { \
if (!fs.is(value, FSBlockType::FILEHEADER)) { \
return FSBlockError::EXPECTED_FILE_HEADER_BLOCK; } }

#define EXPECT_HASH_REF { \
if (!fs.is(value, FSBlockType::FILEHEADER) && !fs.is(value, FSBlockType::USERDIR)) { \
return FSBlockError::EXPECTED_HASHABLE_BLOCK; } }

#define EXPECT_OPTIONAL_HASH_REF { \
if (value) { EXPECT_HASH_REF } }

#define EXPECT_PARENT_DIR_REF { \
if (!fs.is(value, FSBlockType::ROOT) && !fs.is(value, FSBlockType::USERDIR)) { \
return FSBlockError::EXPECTED_USERDIR_OR_ROOT; } }

#define EXPECT_FILELIST_REF { \
if (!fs.is(value, FSBlockType::FILELIST)) { \
return FSBlockError::EXPECTED_FILE_LIST_BLOCK; } }

#define EXPECT_OPTIONAL_FILELIST_REF { \
if (value) { EXPECT_FILELIST_REF } }

#define EXPECT_BITMAP_REF(nr) { \
if (!fs.is(value, FSBlockType::BITMAP)) { \
if (fs.getBmBlocks().size() > usize(nr)) { expected = fs.getBmBlocks()[nr]; } \
return FSBlockError::EXPECTED_BITMAP_BLOCK; } }

#define EXPECT_OPTIONAL_BITMAP_REF(nr) { \
if (value) { EXPECT_BITMAP_REF(nr) } }

#define EXPECT_BITMAP_EXT_REF { \
if (!fs.is(value, FSBlockType::BITMAP_EXT)) { \
return FSBlockError::EXPECTED_BITMAP_EXT_BLOCK; } }

#define EXPECT_OPTIONAL_BITMAP_EXT_REF { \
if (value) { EXPECT_BITMAP_EXT_REF } }

#define EXPECT_DATABLOCK_REF { \
if (traits.ofs() && !fs.is(value, FSBlockType::DATA_OFS)) { \
return FSBlockError::EXPECTED_DATA_BLOCK; } }

#define EXPECT_OPTIONAL_DATABLOCK_REF { \
if (value) { EXPECT_DATABLOCK_REF } }

#define EXPECT_DATABLOCK_NUMBER { \
if (value == 0) return FSBlockError::EXPECTED_DATABLOCK_NR; }

#define EXPECT_HTABLE_SIZE { \
if (isize(value) != (traits.bsize / 4) - 56) { \
expected = u32((traits.bsize / 4) - 56); return FSBlockError::INVALID_HASHTABLE_SIZE; } }

namespace retro::vault::amiga {

FSDoctor::FSDoctor(FileSystem& fs, FSAllocator &a) : FSService(fs), allocator(a)
{

}

void
FSDoctor::dump(BlockNr nr, std::ostream &os)
{
    using namespace utl;

    auto &p = fs.fetch(nr);
    auto *bdata = p.data();

    os << tab("Block");
    os << dec(nr) << std::endl;
    os << tab("Type");
    os << FSBlockTypeEnum::key(p.type) << std::endl;

    if (p.hasHeaderKey()) {

        os << tab("Header Key");
        os << dec(p.getHeaderKey()) << std::endl;
    }
    if (p.hasChecksum()) {

        os << tab("Checksum");
        os << hex(p.getChecksum()) << std::endl;
    }

    switch (p.type) {

        case FSBlockType::BOOT:

            os << tab("Header");
            for (isize i = 0; i < 8; i++) os << hex(bdata[i]) << " ";
            os << std::endl;
            break;

        case FSBlockType::ROOT:

            os << tab("Name");
            os << p.getName() << std::endl;
            os << tab("Created");
            os << p.getCreationDate().str() << std::endl;
            os << tab("Modified");
            os << p.getCreationDate().str() << std::endl;
            os << tab("Bitmap blocks");
            os << FSBlock::rangeString(p.getBmBlockRefs()) << std::endl;
            os << tab("Bitmap extension block");
            os << dec(p.getNextBmExtBlockRef()) << std::endl;
            break;

        case FSBlockType::BITMAP:
        {
            isize count = 0;
            for (isize i = 1; i < p.bsize() / 4; i++) {
                if (u32 value = p.get32(i)) {
                    for (isize j = 0; j < 32; j++) {
                        if (GET_BIT(value, j)) count++;
                    }
                }
            }
            os << tab("Free");
            os << dec(count) << " blocks" << std::endl;
            break;
        }
        case FSBlockType::BITMAP_EXT:

            os << tab("Bitmap blocks");
            os << FSBlock::rangeString(p.getBmBlockRefs()) << std::endl;
            os << tab("Next extension block");
            os << dec(p.getNextBmExtBlockRef()) << std::endl;
            break;

        case FSBlockType::USERDIR:

            os << tab("Name");
            os << p.getName() << std::endl;
            os << tab("Comment");
            os << p.getComment() << std::endl;
            os << tab("Created");
            os << p.getCreationDate().str() << std::endl;
            os << tab("Parent");
            os << dec(p.getParentDirRef()) << std::endl;
            os << tab("Next");
            os << dec(p.getNextHashRef()) << std::endl;
            break;

        case FSBlockType::FILEHEADER:

            os << tab("Name");
            os << p.getName() << std::endl;
            os << tab("Comment");
            os << p.getComment() << std::endl;
            os << tab("Created");
            os << p.getCreationDate().str() << std::endl;
            os << tab("UID (User ID)");
            os << hex(HI_WORD(p.get32(-49))) << std::endl;
            os << tab("GID (Group ID)");
            os << hex(LO_WORD(p.get32(-49))) << std::endl;
            os << tab("Protection flags");
            os << hex(p.getProtectionBits()) << std::endl;
            os << tab("File size");
            os << dec(p.getFileSize()) << " bytes" << std::endl;
            os << tab("First data block");
            os << dec(p.getFirstDataBlockRef()) << std::endl;
            os << tab("Data block count");
            os << dec(p.getNumDataBlockRefs()) << " out of " << dec(p.getMaxDataBlockRefs()) << std::endl;
            os << tab("Data block refs");
            os << FSBlock::rangeString(p.getDataBlockRefs()) << std::endl;
            os << tab("First extension block");
            os << dec(p.getNextListBlockRef()) << std::endl;
            os << tab("Parent dir");
            os << dec(p.getParentDirRef()) << std::endl;
            os << tab("Next file");
            os << dec(p.getNextHashRef()) << std::endl;
            break;

        case FSBlockType::FILELIST:

            os << tab("Header block");
            os << p.getFileHeaderRef() << std::endl;
            os << tab("Data block count");
            os << p.getNumDataBlockRefs() << " out of " << p.getMaxDataBlockRefs() << std::endl;
            os << tab("First");
            os << p.getFirstDataBlockRef() << std::endl;
            os << tab("Data blocks");
            os << FSBlock::rangeString(p.getDataBlockRefs()) << std::endl;
            os << tab("Next extension block");
            os << p.getNextListBlockRef() << std::endl;
            break;

        case FSBlockType::DATA_OFS:

            os << tab("File header block");
            os << p.getFileHeaderRef() << std::endl;
            os << tab("Chain number");
            os << p.getDataBlockNr() << std::endl;
            os << tab("Data bytes");
            os << p.getDataBytesInBlock() << std::endl;
            os << tab("Next data block");
            os << p.getNextDataBlockRef() << std::endl;
            break;

        default:
            break;
    }

    if (p.hashTableSize() > 0) {

        os << tab("Hash table");
        for (isize i = 0, j = 0; i < p.hashTableSize(); i++) {

            if (BlockNr ref = p.read32(bdata + 24 + 4 * i); ref) {

                if (j++) os << std::endl << tab();
                os << std::setfill(' ') << std::setw(2) << i << " -> ";
                os << std::setfill(' ') << std::setw(4) << ref;

                if (auto ptr = fs.tryFetch(ref)) {
                    os << " (" << ptr->getName().cpp_str() << ")";
                }
            }
        }
        os << std::endl;
    }
}

isize
FSDoctor::xray(bool strict)
{
    std::stringstream ss;
    return xray(strict, ss, false);
}

isize
FSDoctor::xray(bool strict, std::ostream &os, bool verbose)
{
    diagnosis.blockErrors = {};

    for (BlockNr nr = 0; isize(nr) < traits.blocks; nr++) {

        if (auto errors = xray(nr, strict)) {

            if (verbose) {

                if (!diagnosis.blockErrors.empty()) os << std::endl;
                xray(nr, strict, os);

            } else {

                os << utl::tab("Block " + std::to_string(nr) + "");
                os << errors << (errors == 1 ? " anomaly" : " anomalies") << std::endl;
            }

            diagnosis.blockErrors.push_back(BlockNr(nr));
        }
    }

    return isize(diagnosis.blockErrors.size());
}

isize
FSDoctor::xrayBitmap(bool strict)
{
    std::unordered_set<BlockNr> used;

    // Extract the directory tree
    auto tree = fs.build(fs.root(), { .depth = MAX_ISIZE });

    // Collect all used blocks
    for (auto &it : tree.dfs()) {

        used.insert(it.nr);
        auto &node = fs.fetch(it.nr);

        if (node.isFile()) {

            auto listBlocks = fs.collectListBlocks(it.nr);
            auto dataBlocks = fs.collectDataBlocks(it.nr);
            used.insert(listBlocks.begin(), listBlocks.end());
            used.insert(dataBlocks.begin(), dataBlocks.end());
        }
    }
    used.insert(fs.getBmBlocks().begin(), fs.getBmBlocks().end());
    used.insert(fs.getBmExtBlocks().begin(), fs.getBmExtBlocks().end());

    // Start from scratch
    diagnosis.unusedButAllocated.clear();
    diagnosis.usedButUnallocated.clear();

    // Check all blocks (ignoring the first two boot blocks)
    for (isize i = 2, capacity = fs.blocks(); i < capacity; i++) {

        bool allocated = allocator.isAllocated(BlockNr(i));
        bool contained = used.contains(BlockNr(i));

        if (strict && allocated && !contained) {

            diagnosis.unusedButAllocated.push_back(BlockNr(i));

        } else if (!allocated && contained) {

            diagnosis.usedButUnallocated.push_back(BlockNr(i));
        }
    }

    return isize(diagnosis.unusedButAllocated.size() + diagnosis.usedButUnallocated.size());
}

isize
FSDoctor::xrayBitmap(std::ostream &os, bool strict)
{
    auto result = xrayBitmap(strict);

    auto &usedButUnallocated = fs.doctor.diagnosis.usedButUnallocated;
    auto &unusedButAllocated = fs.doctor.diagnosis.unusedButAllocated;

    auto blocks = [&](size_t s) { return std::to_string(s) + (s == 1 ? " block" : " blocks"); };

    if (auto total = usedButUnallocated.size() + unusedButAllocated.size(); total) {

        os << utl::tab("Bitmap anomalies:") << blocks(total) << std::endl;

        if (!usedButUnallocated.empty()) {

            os << utl::tab("Used but unallocated:");
            os << FSBlock::rangeString(usedButUnallocated) << std::endl;
        }
        if (!unusedButAllocated.empty()) {

            os << utl::tab("Allocated but unused:");
            os << FSBlock::rangeString(unusedButAllocated) << std::endl;
        }
    }
    return result;
}

isize
FSDoctor::xray(BlockNr ref, bool strict) const
{
    auto &node = fs.fetch(ref);
    isize count = 0;

    for (isize i = 0; i < node.bsize(); i += 4) {

        std::optional<u32> expected;
        if (auto error = xray32(ref, i, strict, expected); error != FSBlockError::OK) {

            count++;
            loginfo(FS_DEBUG, "Block %ld [%ld]: %s\n", node.nr, i, FSBlockErrorEnum::key(error));
        }
    }

    return count;
}

FSBlockError
FSDoctor::xray8(BlockNr ref, isize pos, bool strict, optional<u8> &expected) const
{
    // auto &node = fs.fetch(ref);
    optional<u32> exp;
    auto result = xray32(ref, pos & ~3, strict, exp);
    if (exp) expected = GET_BYTE(*exp, 3 - (pos & 3));
    return result;
}

FSBlockError
FSDoctor::xray32(BlockNr ref, isize pos, bool strict, optional<u32> &expected) const
{
    assert(pos % 4 == 0);

    auto& node = fs.fetch(ref);
    isize word = pos / 4;
    isize sword = word - (node.bsize() / 4);
    BlockNr value = node.get32(word);

    switch (node.type) {

        case FSBlockType::BOOT:

            if (ref == 0) {

                constexpr u32 DOS = u32('D') << 24 | u32('O') << 16 | u32('S') << 8;

                if (word == 0) { EXPECT_VALUE(DOS | u32(traits.dos)); }
                if (word == 1) { value = node.get32(1); EXPECT_CHECKSUM; }
            }
            break;

        case FSBlockType::ROOT:

            switch (word) {

                case 0:   EXPECT_VALUE(2);                  break;
                case 1:   if (strict) EXPECT_VALUE(0);      break;
                case 2:   if (strict) EXPECT_VALUE(0);      break;
                case 3:   if (strict) EXPECT_HTABLE_SIZE;   break;
                case 4:   EXPECT_VALUE(0);                  break;
                case 5:   EXPECT_CHECKSUM;                  break;
            }
            switch (sword) {

                case -50:                                   break;
                case -49: EXPECT_BITMAP_REF(0);             break;
                case -24: EXPECT_OPTIONAL_BITMAP_EXT_REF;   break;
                case -4:
                case -3:
                case -2:  if (strict) EXPECT_VALUE(0);      break;
                case -1:  EXPECT_VALUE(1);                  break;
            }

            // Hash table area
            if (word >= 6 && sword <= -51) { EXPECT_OPTIONAL_HASH_REF; }

            // Bitmap block area
            if (sword >= -49 && sword <= -25) { EXPECT_OPTIONAL_BITMAP_REF(word + 49); }
            break;

        case FSBlockType::BITMAP:

            if (word == 0) EXPECT_CHECKSUM;
            break;

        case FSBlockType::BITMAP_EXT:

            if (word == (i32)(node.bsize() - 4)) EXPECT_OPTIONAL_BITMAP_EXT_REF;
            break;

        case FSBlockType::USERDIR:

            switch (word) {

                case  0: EXPECT_VALUE(2);           break;
                case  1: EXPECT_SELFREF;            break;
                case  2: EXPECT_VALUE(0);           break;
                case  3: EXPECT_VALUE(0);           break;
                case  4: EXPECT_VALUE(0);           break;
                case  5: EXPECT_CHECKSUM;           break;
            }
            switch (sword) {

                case -4: EXPECT_OPTIONAL_HASH_REF;  break;
                case -3: EXPECT_PARENT_DIR_REF;     break;
                case -2: EXPECT_VALUE(0);           break;
                case -1: EXPECT_VALUE(2);           break;
            }
            if (word <= -51) EXPECT_OPTIONAL_HASH_REF;
            break;

        case FSBlockType::FILEHEADER:

            /* Note: At locations -4 and -3, many disks reference the bitmap
             * block which is wrong. We ignore to report this common
             * inconsistency if 'strict' is set to false.
             */
            switch (word) {

                case   0: EXPECT_VALUE(2);                      break;
                case   1: EXPECT_SELFREF;                       break;
                case   3: EXPECT_VALUE(0);                      break;
                // case   4: EXPECT_DATABLOCK_REF;                 break;
                case   5: EXPECT_CHECKSUM;                      break;
            }
            switch (sword) {

                case -50: EXPECT_VALUE(0);                      break;
                case  -4: if (strict) EXPECT_OPTIONAL_HASH_REF; break;
                case  -3: if (strict) EXPECT_PARENT_DIR_REF;    break;
                case  -2: EXPECT_OPTIONAL_FILELIST_REF;         break;
                case  -1: EXPECT_VALUE(-3);                     break;
            }

            // First data block reference
            if (word == 4) {

                if (node.getNumDataBlockRefs()) {
                    EXPECT_DATABLOCK_REF;
                } else {
                    EXPECT_VALUE(0);
                }
            }

            // Data block reference area
            if (word >= 6 && sword <= -51) {

                // Map the index position to the corresponding data block number.
                // The first data block pointer is at -51, the second at -52 etc.
                auto index = -51 - sword;

                if (index < node.getNumDataBlockRefs()) {
                    EXPECT_DATABLOCK_REF;
                } else {
                    EXPECT_VALUE(0);
                }
            }
            break;

        case FSBlockType::FILELIST:

            /* Note: At location -3, many disks reference the bitmap
             * block which is wrong. We ignore to report this common
             * inconsistency if 'strict' is set to false.
             */
            switch (word) {

                case   0: EXPECT_VALUE(16);                     break;
                case   1: EXPECT_SELFREF;                       break;
                case   3: EXPECT_VALUE(0);                      break;
                case   4: EXPECT_OPTIONAL_DATABLOCK_REF;        break;
                case   5: EXPECT_CHECKSUM;                      break;
            }
            switch (sword) {

                case  -3: if (strict) EXPECT_FILEHEADER_REF;    break;
                case  -2: EXPECT_OPTIONAL_FILELIST_REF;         break;
                case  -1: EXPECT_VALUE(-3);                     break;
            }

            // Data block references
            if (word >= 6 && sword <= -51) {

                // Map the index position to the corresponding data block number.
                // The first data block pointer is at -51, the second at -52 etc.
                auto index = -51 - sword;

                if (index < node.getNumDataBlockRefs()) {
                    EXPECT_DATABLOCK_REF;
                } else {
                    EXPECT_VALUE(0);
                }
            }

            // Unused area
            if (sword >= -50 && sword <= -4) {
                EXPECT_VALUE(0);
            }
            break;

        case FSBlockType::DATA_OFS:

            /* Note: At location 1, many disks store a reference to the bitmap
             * block instead of a reference to the file header block. We ignore
             * to report this common inconsistency if 'strict' is set to false.
             */
            switch (word) {

                case 0: EXPECT_VALUE(8);                    break;
                case 1: if (strict) EXPECT_FILEHEADER_REF;  break;
                case 2: EXPECT_DATABLOCK_NUMBER;            break;
                case 3: EXPECT_LESS_OR_EQUAL(node.dsize()); break;
                case 4: EXPECT_OPTIONAL_DATABLOCK_REF;      break;
                case 5: EXPECT_CHECKSUM;                    break;
            }
            break;

        default:
            break;
    }

    return FSBlockError::OK;
}

isize
FSDoctor::xray(BlockNr ref, bool strict, std::ostream &os) const
{
    auto &node   = fs.fetch(ref);
    auto errors  = isize(0);

    std::stringstream ss;

    auto hex = [&](int width, isize value, string delim = "") {
        ss << std::setw(width) << std::right << std::setfill('0') << std::hex << value << delim;
    };
    auto hex4 = [&](u32 value, string delim = "") {
        hex(2, BYTE3(value), " ");
        hex(2, BYTE2(value), " ");
        hex(2, BYTE1(value), " ");
        hex(2, BYTE0(value), delim);
    };

    auto describe = [&](FSBlockError fault, const optional<u32> &value) {

        if (value) { hex4(*value); return; }

        switch (fault) {

            case FSBlockError::EXPECTED_BITMAP_BLOCK:        ss << "Link to a bitmap block"; break;
            case FSBlockError::EXPECTED_BITMAP_EXT_BLOCK:    ss << "Link to a bitmap extension block"; break;
            case FSBlockError::EXPECTED_HASHABLE_BLOCK:      ss << "Link to a file header or directory block"; break;
            case FSBlockError::EXPECTED_USERDIR_OR_ROOT:     ss << "Link to a directory or the root block"; break;
            case FSBlockError::EXPECTED_DATA_BLOCK:          ss << "Link to a data block"; break;
            case FSBlockError::EXPECTED_FILE_HEADER_BLOCK:   ss << "Link to a file header block"; break;
            case FSBlockError::EXPECTED_FILE_LIST_BLOCK:     ss << "Link to a file extension block"; break;
            case FSBlockError::EXPECTED_DATABLOCK_NR:        ss << "Data block number"; break;

            default:
                ss << "???";
        }
    };

    for (isize i = 0; i < traits.bsize; i += 4) {

        optional<u32> expected;

        if (auto fault = xray32(ref, i, strict, expected); fault != FSBlockError::OK) {

            auto *data = node.data();
            auto type = fs.typeOf(node.nr, i);

            //hex(6, node.nr, ":+");
            ss << std::setw(7) << std::left << std::to_string(node.nr);
            ss << "+";
            hex(4, i, "  ");
            hex4(node.read32(data + i), "  ");

            ss << std::setw(36) << std::left << std::setfill(' ') << FSItemTypeEnum::help(type);
            describe(fault, expected);
            ss << std::endl;

            errors++;
        }
    }

    if (errors) {

        os << "Block  Entry  Data         Item type                           Expected" << std::endl;
        string line;
        while(std::getline(ss, line)) os << line << '\n';

    } else {

        // os << "No errors found" << std::endl;
    }

    return errors;
}

void
FSDoctor::rectify(bool strict)
{
    xray(strict);

    // Rectify all erroneous blocks
    for (auto &it : diagnosis.blockErrors) rectify(it, strict);
}

void
FSDoctor::rectify(BlockNr ref, bool strict)
{
    auto &node = fs.fetch(ref);

    for (isize i = 0; i < traits.bsize / 4; i += 4) {

        optional<u32> expected;

        if (auto fault = xray32(ref, i, strict, expected); fault != FSBlockError::OK) {

            if (expected) {

                auto &mutatableNode = node.mutate();
                auto *data = mutatableNode.data();
                mutatableNode.mutate().write32(data + i, *expected);
            }
        }
    }
}

void
FSDoctor::rectifyBitmap(bool strict)
{
    xrayBitmap(strict);

    for (auto &it : diagnosis.unusedButAllocated) {
        allocator.markAsFree(BlockNr(it));
    }
    for (auto &it : diagnosis.usedButUnallocated) {
        allocator.markAsAllocated(BlockNr(it));
    }
}

string
FSDoctor::ascii(BlockNr nr, isize offset, isize len) const noexcept
{
    assert(offset + len <= traits.bsize);

    return  utl::createAscii(fs.fetch(nr).data() + offset, len);
}

void
FSDoctor::createUsageMap(u8 *buffer, isize len) const
{
    // Setup priorities
    i8 pri[12];
    pri[isize(FSBlockType::UNKNOWN)]      = 0;
    pri[isize(FSBlockType::EMPTY)]        = 1;
    pri[isize(FSBlockType::BOOT)]         = 8;
    pri[isize(FSBlockType::ROOT)]         = 9;
    pri[isize(FSBlockType::BITMAP)]       = 7;
    pri[isize(FSBlockType::BITMAP_EXT)]   = 6;
    pri[isize(FSBlockType::USERDIR)]      = 5;
    pri[isize(FSBlockType::FILEHEADER)]   = 3;
    pri[isize(FSBlockType::FILELIST)]     = 2;
    pri[isize(FSBlockType::DATA_OFS)]     = 2;
    pri[isize(FSBlockType::DATA_FFS)]     = 2;

    isize max = traits.blocks;

    // Start from scratch
    for (isize i = 0; i < len; i++) buffer[i] = (u8)FSBlockType::UNKNOWN;

    // Mark all free blocks
    for (isize i = 0; i < max; i++) buffer[i * (len - 1) / (max - 1)] = (u8)FSBlockType::EMPTY;

    // Mark all used blocks
    for (isize i = 0; i < max; i++) {

        if (auto type = fs.typeOf(BlockNr(i)); type != FSBlockType::EMPTY) {

            auto val = u8(type);
            auto pos = i * (len - 1) / (max - 1);
            if (pri[buffer[pos]] < pri[val]) buffer[pos] = val;
            if (pri[buffer[pos]] == pri[val] && pos > 0 && buffer[pos-1] != val) buffer[pos] = val;
        }
    }

    // Fill gaps
    for (isize pos = 1; pos < len; pos++) {
        if (buffer[pos] == (u8)FSBlockType::UNKNOWN) buffer[pos] = buffer[pos - 1];
    }
}

void
FSDoctor::createAllocationMap(u8 *buffer, isize len) const
{
    auto &unusedButAllocated = diagnosis.unusedButAllocated;
    auto &usedButUnallocated = diagnosis.usedButUnallocated;

    isize max = traits.blocks;

    // Start from scratch
    for (isize i = 0; i < len; i++) buffer[i] = 255;

    // Mark all free blocks
    for (isize i = 0; i < max; i++) buffer[i * (len - 1) / (max - 1)] = 0;

    // Mark all used blocks
    for (isize i = 0; i < max; i++) {

        if (auto type = fs.typeOf(BlockNr(i)); type != FSBlockType::EMPTY) {
            buffer[i * (len - 1) / (max - 1)] = 1;
        }
    }

    // Mark all erroneous blocks
    for (auto &it : unusedButAllocated) buffer[it * (len - 1) / (max - 1)] = 2;
    for (auto &it : usedButUnallocated) buffer[it * (len - 1) / (max - 1)] = 3;

    // Fill gaps
    for (isize pos = 1; pos < len; pos++) {
        if (buffer[pos] == 255) buffer[pos] = buffer[pos - 1];
    }
}

void
FSDoctor::createHealthMap(u8 *buffer, isize len) const
{
    auto &blockErrors = diagnosis.blockErrors;

    isize max = traits.blocks;

    // Start from scratch
    for (isize i = 0; i < len; i++) buffer[i] = 255;

    // Mark all free blocks
    for (isize i = 0; i < max; i++) buffer[i * (len - 1) / (max - 1)] = 0;

    // Mark all used blocks
    for (isize i = 0; i < max; i++) {

        if (auto type = fs.typeOf(BlockNr(i)); type != FSBlockType::EMPTY) {
            buffer[i * (len - 1) / (max - 1)] = 1;
        }
    }

    // Mark all erroneous blocks
    for (auto &it : blockErrors) buffer[it * (len - 1) / (max - 1)] = 2;

    // Fill gaps
    for (isize pos = 1; pos < len; pos++) {
        if (buffer[pos] == 255) buffer[pos] = buffer[pos - 1];
    }
}

isize
FSDoctor::nextBlockOfType(FSBlockType type, BlockNr after) const
{
    assert(isize(after) < traits.blocks);

    isize result = after;

    do {
        result = (result + 1) % traits.blocks;
        if (fs.typeOf(BlockNr(result)) == type) return result;

    } while (result != isize(after));

    return -1;
}

}
