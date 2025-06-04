// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "VirtualC64Config.h"
#include "C64.h"
#include "IOUtils.h"

namespace vc64 {

bool
D64File::isCompatible(const fs::path &path)
{
    auto s = util::uppercased(path.extension().string());
    return s == ".D64";
}

bool
D64File::isCompatible(const u8 *buf, isize len)
{
    return
    len == D64_683_SECTORS ||
    len == D64_683_SECTORS_ECC ||
    len == D64_768_SECTORS ||
    len == D64_768_SECTORS_ECC ||
    len == D64_802_SECTORS ||
    len == D64_802_SECTORS_ECC;
}

bool
D64File::isCompatible(const Buffer<u8> &buf)
{
    return isCompatible(buf.ptr, buf.size);
}

D64File::D64File()
{
    memset(errors, 0x01, sizeof(errors));
}

D64File::D64File(isize tracks, bool ecc) : D64File()
{
    switch(tracks) {
            
        case 35: init(ecc ? D64_683_SECTORS_ECC : D64_683_SECTORS); break;
        case 40: init(ecc ? D64_768_SECTORS_ECC : D64_768_SECTORS); break;
        case 42: init(ecc ? D64_802_SECTORS_ECC : D64_802_SECTORS); break;

        default:
            fatalError;
    }    
}

void
D64File::init(isize tracks, bool ecc)
{
    switch(tracks) {
            
        case 35: init(ecc ? D64_683_SECTORS_ECC : D64_683_SECTORS); break;
        case 40: init(ecc ? D64_768_SECTORS_ECC : D64_768_SECTORS); break;
        case 42: init(ecc ? D64_802_SECTORS_ECC : D64_802_SECTORS); break;

        default:
            fatalError;
    }
}

void
D64File::init(const FileSystem &volume)
{
    switch (volume.getNumBlocks() * 256) {

        case D64_683_SECTORS: init(35, false); break;
        case D64_768_SECTORS: init(40, false); break;
        case D64_802_SECTORS: init(42, false); break;

        default:
            fatalError;
    }

    Fault err;
    if (!volume.exportVolume(data.ptr, data.size, &err)) {
        throw AppError(err);
    }
}


PETName<16>
D64File::getName() const
{
    return PETName<16>(data.ptr + offset(18, 0) + 0x90);
}

void
D64File::finalizeRead()
{
    isize numSectors;
    bool errorCodes;

    switch (data.size)
    {
        case D64_683_SECTORS: // 35 tracks, no errors
            
            debug(FILE_DEBUG, "D64 file contains 35 tracks, no EC bytes\n");
            numSectors = 683;
            errorCodes = false;
            break;
            
        case D64_683_SECTORS_ECC: // 35 tracks, 683 error bytes
            
            debug(FILE_DEBUG, "D64 file contains 35 tracks, 683 EC bytes\n");
            numSectors = 683;
            errorCodes = true;
            break;
            
        case D64_768_SECTORS: // 40 tracks, no errors
            
            debug(FILE_DEBUG, "D64 file contains 40 tracks, no EC bytes\n");
            numSectors = 768;
            errorCodes = false;
            break;
            
        case D64_768_SECTORS_ECC: // 40 tracks, 768 error bytes
            
            debug(FILE_DEBUG, "D64 file contains 40 tracks, 768 EC bytes\n");
            numSectors = 768;
            errorCodes = true;
            break;
            
        case D64_802_SECTORS: // 42 tracks, no error bytes
            
            debug(FILE_DEBUG, "D64 file contains 42 tracks, no EC bytes\n");
            numSectors = 802;
            errorCodes = false;
            break;
            
        case D64_802_SECTORS_ECC: // 42 tracks, 802 error bytes
            
            debug(FILE_DEBUG, "D64 file contains 42 tracks, 802 EC bytes\n");
            numSectors = 802;
            errorCodes = true;
            break;
            
        default:
            
            numSectors = 0;
            errorCodes = false;
            assert(false);
    }

    // Copy error codes
    if (errorCodes) {
        std::memcpy(errors, data.ptr + (numSectors * 256), numSectors);
    }
}

Track
D64File::numHalftracks() const
{
    switch (data.size) {

        case D64_683_SECTORS: case D64_683_SECTORS_ECC: return 2 * 35;
        case D64_768_SECTORS: case D64_768_SECTORS_ECC: return 2 * 40;
        case D64_802_SECTORS: case D64_802_SECTORS_ECC: return 2 * 42;
            
        default:
            fatalError;
    }
}

u8
D64File::getErrorCode(Block b) const
{
    assert(b < 802);
    return errors[b];
}

isize
D64File::offset(Track track, Sector sector) const
{
    // secCnt[track] is the number of the first sector on track 'track'
    const isize secCnt[43] = {  0 /* pad */,
        0,   21,  42,  63,  84,  105, 126, 147,
        168, 189, 210, 231, 252, 273, 294, 315,
        336, 357, 376, 395, 414, 433, 452, 471,
        490, 508, 526, 544, 562, 580, 598, 615,
        632, 649, 666, 683, 700, 717, 734, 751,
        768, 785 };
    
    if (Disk::isValidTrackSectorPair(track, sector)) {
        return (secCnt[track] + sector) * 256;
    } else {
        return -1;
    }
}

void
D64File::dump(Track track, Sector sector) const
{
    util::hexdump(data.ptr + offset(track, sector), 256);
}

}
