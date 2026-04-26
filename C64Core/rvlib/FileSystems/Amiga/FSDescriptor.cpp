// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/Amiga/FSDescriptor.h"
#include "FileSystems/FSError.h"
#include "BlockDevice.h"
#include "ADFFile.h"
#include "utl/io.h"
#include "utl/types/Literals.h"

namespace retro::vault::amiga {

using retro::vault::image::ADFFile;

FSDescriptor::FSDescriptor(isize numBlocks)
{
    init(numBlocks);
}

FSDescriptor::FSDescriptor(Diameter dia, Density den)
{
    init(dia, den);
}

FSDescriptor::FSDescriptor(const GeometryDescriptor &geometry)
{
    init(geometry);
}

FSDescriptor::FSDescriptor(const PartitionDescriptor &des)
{
    init(des);
}


void
FSDescriptor::init(isize numBlocks)
{
    // Copy parameters
    this->numBlocks = numBlocks;
    this->numReserved = 2;

    // Determine the location of the root block
    if (numBlocks * 512 == ADFFile::ADFSIZE_35_DD_81 ||
        numBlocks * 512 == ADFFile::ADFSIZE_35_DD_82 ||
        numBlocks * 512 == ADFFile::ADFSIZE_35_DD_83 ||
        numBlocks * 512 == ADFFile::ADFSIZE_35_DD_84) {

        rootBlock = 880;

    } else {

        isize highKey = numBlocks - 1;
        isize rootKey = (numReserved + highKey) / 2;
        rootBlock = BlockNr(rootKey);
    }

    // Determine the number of required bitmap blocks
    isize bitsPerBlock = (bsize - 4) * 8;
    isize neededBlocks = (numBlocks + bitsPerBlock - 1) / bitsPerBlock;
    isize bmKey = rootBlock + 1;

    // Add bitmap blocks
    for (isize i = 0; i < neededBlocks; i++) {
        bmBlocks.push_back(BlockNr(bmKey++));
    }
    
    // Add bitmap extension blocks (the first 25 references are stored in the root block)
    for (; neededBlocks - 25 > 0; neededBlocks -= 127) {
        bmExtBlocks.push_back(BlockNr(bmKey++));
    }
}

void
FSDescriptor::init(const GeometryDescriptor &geometry)
{
    init(geometry.numBlocks());
}

void
FSDescriptor::init(const PartitionDescriptor &des)
{

    init(des.numBlocks());
}

void
FSDescriptor::init(Diameter dia, Density den)
{
    init(GeometryDescriptor(dia, den));
}


void
FSDescriptor::dump() const
{
    dump(std::cout);
}

void
FSDescriptor::dump(std::ostream &os) const
{
    using namespace utl;

    os << tab("Blocks");
    os << dec(numBlocks) << std::endl;
    os << tab("BSize");
    os << dec(bsize) << std::endl;
    os << tab("Reserved");
    os << dec(numReserved) << std::endl;
    os << tab("Root block");
    os << dec(rootBlock) << std::endl;
    os << tab("Bitmap blocks");
    for (auto& it : bmBlocks) { os << dec(it) << " "; }; os << std::endl;
    os << tab("Extension blocks");
    for (auto& it : bmExtBlocks) { os << dec(it) << " "; }; os << std::endl;
}

void
FSDescriptor::checkCompatibility() const
{
    if constexpr (force::FS_WRONG_CAPACITY)
        throw FSError(FSError::FS_WRONG_CAPACITY);

    if constexpr (force::FS_WRONG_BSIZE)
        throw FSError(FSError::FS_WRONG_BSIZE);

    if (numBytes() > 504_MB)
        throw FSError(FSError::FS_WRONG_CAPACITY);

    if (bsize != 512)
        throw FSError(FSError::FS_WRONG_BSIZE);

    if (isize(rootBlock) >= numBlocks)
        throw FSError(FSError::FS_OUT_OF_RANGE);
}

}
