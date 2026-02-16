// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/CBM/FSDoctor.h"
#include "FileSystems/CBM/FileSystem.h"
#include "utl/io.h"
#include "utl/support.h"
#include <format>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

//
// Macros used inside the check() methods
//

#define EXPECT_BYTE(exp) { \
if (value != (exp)) { expected = (u8)(exp); return FSBlockError::EXPECTED_VALUE; } }

#define EXPECT_MIN(min) { \
if (value < (min)) { expected = (u8)(min); return FSBlockError::EXPECTED_LARGER_VALUE; } }

#define EXPECT_MAX(max) { \
if (value > (max)) { expected = (u8)(max); return FSBlockError::EXPECTED_SMALLER_VALUE; } }

#define EXPECT_RANGE(min,max) { \
EXPECT_MIN(min); EXPECT_MAX(max) }

#define EXPECT_TRACK_REF(s) \
EXPECT_RANGE(0, traits.numTracks() + 1)

#define EXPECT_SECTOR_REF(t) { \
if (isize num = traits.numSectors(t)) \
EXPECT_RANGE(0,num) else if (strict) EXPECT_MAX(254) }


namespace retro::vault::cbm {

FSDoctor::FSDoctor(FileSystem& fs, FSAllocator &a) : FSService(fs), allocator(a)
{

}

void
FSDoctor::dump(BlockNr nr, std::ostream &os)
{
    using namespace utl;

    auto &p   = fs.fetch(nr);
    auto ts   = p.tsLink();
    auto data = p.data();

    os << tab("Block");
    os << dec(nr) << std::endl;
    os << tab("Type");
    os << FSBlockTypeEnum::key(p.type) << std::endl;
    os << tab("TS link");
    os << ts.t << ":" << ts.s << std::endl;

    switch (p.type) {

        case FSBlockType::BAM:
        {
            os << tab("Name");
            os << p.getName().str() << std::endl;
            os << tab("DOS version");
            os << data[0x02] << std::endl;
            os << tab("DOS type");
            os << data[0xA5] << data[0xA6] << std::endl;
            break;
        }
        case FSBlockType::DIR:
        {
            isize slot = 0;
            for (const auto &it : fs.readDirBlock(nr)) {

                os << tab("Slot " + std::to_string(++slot));

                auto name = "\"" + it.getName().str() + "\"";
                auto size = it.getFileSize();
                auto type = it.typeString();
                auto t    = it.firstDataTrack;
                auto s    = it.firstDataSector;

                if (it.empty()) {
                    os << "<empty>" << std::endl;
                } else {
                    os << std::format("{:<5} {:<16} {} -> {:>2}:{:<2}\n",
                                      size, name, type, t, s);
                }
            }
            break;
        }
        case FSBlockType::DATA:
        {
            auto lastBlock = ts.t == 0;
            os << tab("Stored bytes");
            os << (lastBlock ? ts.s : 254) << std::endl;
            break;
        }
        default:
            break;
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

    // Read directory
    auto dir = fs.readDir();

    // Collect all used blocks
    for (auto &it : dir) {

        auto dataBlocks = fs.collectDataBlocks(it);
        used.insert(dataBlocks.begin(), dataBlocks.end());
    }

    // Read allocation map
    auto bitmap = allocator.readBitmap();

    // Start from scratch
    diagnosis.unusedButAllocated.clear();
    diagnosis.usedButUnallocated.clear();

    // Check all blocks
    for (isize i = 0; i < fs.blocks(); ++i) {

        bool allocated = !bitmap[i];
        bool contained = used.contains(BlockNr(i));

        if (strict && allocated && !contained) {
            
            diagnosis.unusedButAllocated.push_back(BlockNr(i));
            
        } else if (!allocated && contained) {
            
            diagnosis.usedButUnallocated.push_back(BlockNr(i));
        }
    }

    return
    (isize)diagnosis.unusedButAllocated.size() +
    (isize)diagnosis.usedButUnallocated.size();
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
    auto &node      = fs.fetch(ref);
    auto dirBlocks  = fs.collectDirBlocks();
    isize erroneous = 0;


    for (isize i = 0; i < node.bsize(); ++i) {

        std::optional<u8> expected;
        auto error = xray8(ref, i, strict, expected, dirBlocks);

        if ( error != FSBlockError::OK) {

            erroneous++;
            loginfo(FS_DEBUG, "Block %ld [%ld]: %s\n", node.nr, i, FSBlockErrorEnum::key(error));
        }
    }

    return erroneous;
}

FSBlockError
FSDoctor::xray8(BlockNr ref, isize pos, bool strict, optional<u8> &expected) const
{
    auto dirBlocks = fs.collectDirBlocks();
    return xray8(ref, pos, strict, expected, dirBlocks);
}

FSBlockError
FSDoctor::xray8(BlockNr ref, isize pos, bool strict,
                optional<u8> &expected, const std::vector<BlockNr> &dirBlocks) const
{
    assert(pos >= 0 && pos < 256);

    auto& block = fs.fetch(ref);
    auto *data  = block.data();
    u8 value    = data[pos];

    switch (block.type) {

        case FSBlockType::UNKNOWN:
        case FSBlockType::EMPTY:

            return FSBlockError::OK;

        case FSBlockType::BAM:

            switch (pos) {

                case 0x00: EXPECT_BYTE(18);               break;
                case 0x01: EXPECT_BYTE(1);                break;
                case 0x02: EXPECT_BYTE(0x41);             break;
                case 0xA0:
                case 0xA1:
                case 0xA4: if (strict) EXPECT_BYTE(0xA0); break;
                case 0xA5: EXPECT_BYTE('2');              break;
                case 0xA6: EXPECT_BYTE('A');              break;
                case 0xA7:
                case 0xA8:
                case 0xA9:
                case 0xAA: if (strict) EXPECT_BYTE(0xA0); break;
            }

            if (strict && pos >= 0xAB && pos <= 0xFF) EXPECT_BYTE(0x00);

            return FSBlockError::OK;

        case FSBlockType::DIR:

            if (std::find(dirBlocks.begin(), dirBlocks.end(), ref) != dirBlocks.end()) {

                if (pos == 0) EXPECT_TRACK_REF (data[pos + 1]);
                if (pos == 1) EXPECT_SECTOR_REF(data[pos - 1]);

                if (!utl::isZero(data + pos, 0x20)) {

                    switch (pos & 0x1F) {

                        case 0x03: EXPECT_TRACK_REF (data[pos + 1]); break;
                        case 0x04: EXPECT_SECTOR_REF(data[pos - 1]); break;
                        case 0x15: EXPECT_TRACK_REF (data[pos + 1]); break;
                        case 0x16: EXPECT_SECTOR_REF(data[pos - 1]); break;
                        case 0x17: EXPECT_MAX(254);                  break;
                    }
                }
            }

            return FSBlockError::OK;

        case FSBlockType::DATA:

            if (pos == 0 && strict) EXPECT_TRACK_REF (data[pos + 1]);
            if (pos == 1 && strict) EXPECT_SECTOR_REF(data[pos - 1]);

            return FSBlockError::OK;

        default:
            fatalError;
    }

}

isize
FSDoctor::xray(BlockNr ref, bool strict, std::ostream &os) const
{
    auto &node     = fs.fetch(ref);
    auto dirBlocks = fs.collectDirBlocks();
    auto *data     = node.data();

    isize errors = 0;
    std::string report;

    auto describeExpected =
        [](FSBlockError fault, const std::optional<u8> &value) -> std::string
    {
        if (!value)
            return "";

        switch (fault) {

            case FSBlockError::EXPECTED_VALUE:
                return std::format("Expected: {:02x}", *value);

            case FSBlockError::EXPECTED_SMALLER_VALUE:
                return std::format("Expected: {:02x} or lower", *value);

            case FSBlockError::EXPECTED_LARGER_VALUE:
                return std::format("Expected: {:02x} or higher", *value);

            default:
                return "???";
        }
    };

    for (isize i = 0; i < traits.bsize; ++i) {

        std::optional<u8> expected;
        const auto fault = xray8(ref, i, strict, expected, dirBlocks);

        if (fault == FSBlockError::OK) continue;

        const auto type = fs.typeOf(node.nr, i);

        report += std::format(
            "{:<7}+{:02x}    {:02x}    {:<16}{}\n",
            node.nr,
            static_cast<unsigned>(i),
            static_cast<unsigned>(data[i]),
            FSItemTypeEnum::help(type),
            describeExpected(fault, expected)
        );

        ++errors;
    }

    if (errors) {
        os << "Block  Entry  Data  Item type       \n" // Expected\n"
           << report;
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
    auto &block    = fs.fetch(ref);
    auto dirBlocks = fs.collectDirBlocks();

    for (isize i = 0; i < traits.bsize; ++i) {

        optional<u8> expected;
        auto fault = xray8(ref, i, strict, expected, dirBlocks);

        if (fault != FSBlockError::OK) {

            if (expected)
                block.mutate().data()[i] = *expected;
        }
    }
}

void
FSDoctor::rectifyBitmap(bool strict)
{
    xrayBitmap(strict);

    // TODO: SETUP VECTOR, WRITE BACK

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
    pri[isize(FSBlockType::BAM)]          = 4;
    pri[isize(FSBlockType::DIR)]      = 3;
    pri[isize(FSBlockType::DATA)]         = 2;

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
