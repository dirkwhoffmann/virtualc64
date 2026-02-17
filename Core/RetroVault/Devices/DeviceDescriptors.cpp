// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "DeviceDescriptors.h"
#include "DeviceError.h"
#include "FileSystems/Amiga/FSTypes.h"
#include "utl/io.h"
#include "utl/types/Literals.h"
#include <algorithm>

namespace retro::vault {

//
// GeometryDescriptor
//

GeometryDescriptor::GeometryDescriptor(isize c, isize h, isize s, isize b)
{
    cylinders = c;
    heads = h;
    sectors = s;
    bsize = b;
}

GeometryDescriptor::GeometryDescriptor(isize size)
{
    // Create a default geometry for the provide size

    bsize = 512;
    sectors = 32;
    heads = 1;

    auto tsize = bsize * sectors;
    cylinders = (size / tsize) + (size % tsize ? 1 : 0);

    while (cylinders > 1024) {

        cylinders = (cylinders + 1) / 2;
        heads = heads * 2;
    }
}

GeometryDescriptor::GeometryDescriptor(Diameter type, Density density)
{
    if (type == Diameter::INCH_525 && density == Density::SD) {

        cylinders = 40;
        heads = 2;
        sectors = 11;
        bsize = 512;
        return;
    }
    if (type == Diameter::INCH_35 && density == Density::DD) {

        cylinders = 80;
        heads = 2;
        sectors = 11;
        bsize = 512;
        return;
    }
    if (type == Diameter::INCH_35 && density == Density::HD) {

        cylinders = 80;
        heads = 2;
        sectors = 22;
        bsize = 512;
        return;
    }

    fatalError;
}

bool
GeometryDescriptor::operator == (const GeometryDescriptor &rhs) const
{
    return
    this->cylinders == rhs.cylinders &&
    this->heads == rhs.heads &&
    this->sectors == rhs.sectors &&
    this->bsize == rhs.bsize;
}

bool
GeometryDescriptor::operator != (const GeometryDescriptor &rhs) const
{
    return !(*this == rhs);
}

bool
GeometryDescriptor::operator < (const GeometryDescriptor &rhs) const
{
    return cylinders < rhs.cylinders;
}

std::vector<std::tuple<isize,isize,isize>> 
GeometryDescriptor::driveGeometries(isize numBlocks)
{
    std::vector<std::tuple<isize,isize,isize>> result;

    for (auto &geometry : driveGeometries(numBlocks, 512)) {

        result.push_back(std::tuple<isize,isize,isize>
                         (geometry.cylinders, geometry.heads, geometry.sectors));
    }
    return result;
}

std::vector<GeometryDescriptor>
GeometryDescriptor::driveGeometries(isize numBlocks, isize bsize)
{
    std::vector<GeometryDescriptor> result;
    
    // Typical number of sectors per track
    // https://www.win.tue.nl/~aeb/linux/hdtypes/hdtypes-4.html
    
    static i8 sizes[] = {
        
        16, 17, 24, 26, 27, 28, 29, 32, 34,
        35, 36, 38, 47, 50, 51, 52, 53, 55,
        56, 59, 60, 61, 62, 63
    };
    
    // Compute all geometries compatible with the given block count
    for (isize h = GeometryDescriptor::hMin; h <= GeometryDescriptor::hMax; h++) {
        for (usize i = 0; i < sizeof(sizes); i++) {

            auto s = isize(sizes[i]);
            auto blocksPerCyl = h * s;
            
            if (numBlocks % blocksPerCyl == 0) {
                
                auto c = numBlocks / blocksPerCyl;

                if (c > GeometryDescriptor::cMax) continue;
                if (c < GeometryDescriptor::cMin && h > 1) continue;
                
                result.push_back(GeometryDescriptor(c, h, s, bsize));
            }
        }
    }

    // Sort all entries
    std::sort(result.begin(), result.end());

    return result;
}

bool
GeometryDescriptor::unique() const
{
    return driveGeometries(numBytes(), 512).size() == 1;
}

void
GeometryDescriptor::dump() const
{
    dump(std::cout);
}

void
GeometryDescriptor::dump(std::ostream &os) const
{
    using namespace utl;

    os << tab("Geometry");
    os << dec(cylinders) << " - ";
    os << dec(heads) << " - ";
    os << dec(sectors) << std::endl;
}

void
GeometryDescriptor::checkCompatibility() const
{
    // if constexpr (debug::HDR_ACCEPT_ALL) return;

    // Check forced error conditions
    if constexpr (force::HDR_UNKNOWN_GEOMETRY) {
        throw DeviceError(DeviceError::HDR_UNKNOWN_GEOMETRY);
    }
    if constexpr (force::HDR_TOO_LARGE) {
        throw DeviceError(DeviceError::HDR_TOO_LARGE);
    }
    if constexpr (force::HDR_UNSUPPORTED_C) {
        throw DeviceError(DeviceError::HDR_UNSUPPORTED_CYL_CNT, cylinders);
    }
    if constexpr (force::HDR_UNSUPPORTED_H) {
        throw DeviceError(DeviceError::HDR_UNSUPPORTED_HEAD_CNT, heads);
    }
    if constexpr (force::HDR_UNSUPPORTED_S) {
        throw DeviceError(DeviceError::HDR_UNSUPPORTED_SEC_CNT, sectors);
    }
    if constexpr (force::HDR_UNSUPPORTED_B) {
        throw DeviceError(DeviceError::HDR_UNSUPPORTED_BSIZE);
    }
    
    // Check for real error conditions
    if (cylinders == 0) {
        throw DeviceError(DeviceError::HDR_UNKNOWN_GEOMETRY);
    }
    if (numBytes() > 504_MB) {
        throw DeviceError(DeviceError::HDR_TOO_LARGE);
    }
    if ((cylinders < cMin && heads > 1) || cylinders > cMax) {
        throw DeviceError(DeviceError::HDR_UNSUPPORTED_CYL_CNT, cylinders);
    }
    if (heads < hMin || heads > hMax) {
        throw DeviceError(DeviceError::HDR_UNSUPPORTED_HEAD_CNT, heads);
    }
    if (sectors < sMin || sectors > sMax) {
        throw DeviceError(DeviceError::HDR_UNSUPPORTED_SEC_CNT, sectors);
    }
    if (bsize != 512) {
        throw DeviceError(DeviceError::HDR_UNSUPPORTED_BSIZE);
    }
}

//
// PartitionDescriptor
//

PartitionDescriptor::PartitionDescriptor(const GeometryDescriptor &geo)
{
    sizeBlock   = u32(geo.bsize / 4);
    heads       = u32(geo.heads);
    sectors     = u32(geo.sectors);
    lowCyl      = 0;
    highCyl     = u32(geo.upperCyl());
}

GeometryDescriptor
PartitionDescriptor::geometry() const noexcept {

    return GeometryDescriptor(numCylinders(), heads, sectors, sizeBlock * 4);
}

isize
PartitionDescriptor::translate(isize block) const noexcept
{
    assert(block >= 0 && block < numBlocks());
    return lowCyl * heads * sectors + block;
}

void
PartitionDescriptor::dump() const
{
    dump(std::cout);
}

void
PartitionDescriptor::dump(std::ostream &os) const
{
    using namespace utl;

    os << tab("Name");
    os << name << std::endl;
    os << tab("Flags");
    os << dec(flags) << std::endl;
    os << tab("SizeBlock");
    os << dec(sizeBlock) << std::endl;
    os << tab("Heads");
    os << dec(heads) << std::endl;
    os << tab("Sectors");
    os << dec(sectors) << std::endl;
    os << tab("Reserved");
    os << dec(reserved) << std::endl;
    os << tab("Interleave");
    os << dec(interleave) << std::endl;
    os << tab("LowCyl");
    os << dec(lowCyl) << std::endl;
    os << tab("HighCyl");
    os << dec(highCyl) << std::endl;
    os << tab("NumBuffers");
    os << dec(numBuffers) << std::endl;
    os << tab("BufMemType");
    os << dec(bufMemType) << std::endl;
    os << tab("MaxTransfer");
    os << dec(maxTransfer) << std::endl;
    os << tab("Mask");
    os << dec(mask) << std::endl;
    os << tab("BootPrio");
    os << dec(bootPri) << std::endl;
    os << tab("DosType");
    os << dec(dosType) << std::endl;
}

void PartitionDescriptor::checkCompatibility(const GeometryDescriptor &geo) const
{
    auto bsize = 4 * sizeBlock;
    
    if (bsize != 512) {
        throw DeviceError(DeviceError::HDR_UNSUPPORTED_BSIZE, std::to_string(bsize));
    }
    if (lowCyl > highCyl) {
        throw DeviceError(DeviceError::HDR_CORRUPTED_PTABLE);
    }
    if (isize(highCyl) >= geo.cylinders) {
        throw DeviceError(DeviceError::HDR_CORRUPTED_PTABLE);
    }
}

void
DriverDescriptor::dump() const
{
    dump(std::cout);
}

void
DriverDescriptor::dump(std::ostream &os) const
{
    using namespace utl;

    os << tab("DOS type");
    os << hex(dosType);
    // os << " (" << OSDebugger::dosTypeStr(dosType) << ")" << std::endl;
    os << tab("DOS version");
    os << hex(dosVersion);
    // os << " (" << OSDebugger::dosVersionStr(dosVersion) << ")" << std::endl;
    os << tab("Patch flags");
    os << hex(patchFlags) << std::endl;
    os << tab("Code blocks");
    for (usize i = 0; i < blocks.size(); i++) {
        os << (i ? ", " : "") << dec(blocks[i]);
    }
    os << std::endl;
}

void
DriverDescriptor::checkCompatibility() const
{

}

}
