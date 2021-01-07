// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

bool
D64File::isCompatibleName(const std::string &name)
{
    auto s = suffix(name);
    return s == "d64" || s == "D64";
}

bool
D64File::isCompatibleStream(std::istream &stream)
{
    usize len = streamLength(stream);
    
    return
    len == D64_683_SECTORS ||
    len == D64_683_SECTORS_ECC ||
    len == D64_768_SECTORS ||
    len == D64_768_SECTORS_ECC ||
    len == D64_802_SECTORS ||
    len == D64_802_SECTORS_ECC;
}

D64File::D64File()
{
    memset(errors, 0x01, sizeof(errors));
}

D64File::D64File(unsigned tracks, bool ecc) : D64File()
{
    switch(tracks) {
            
        case 35: size = ecc ? D64_683_SECTORS_ECC : D64_683_SECTORS; break;
        case 40: size = ecc ? D64_768_SECTORS_ECC : D64_768_SECTORS; break;
        case 42: size = ecc ? D64_802_SECTORS_ECC : D64_802_SECTORS; break;
            
        default:
            assert(false);
    }
    
    data = new u8[size]();
}

D64File *
D64File::makeWithDisk(Disk &disk)
{
    u8 buffer[D64_802_SECTORS];
    
    // Serialize disk data into a byte stream
    usize len = disk.decodeDisk(buffer);

    // Check if the disk has been fully decoded
    if (len != D64_683_SECTORS && len != D64_768_SECTORS && len != D64_802_SECTORS) {
        return nullptr;
    }
    
    // Create object from byte stream
    return make <D64File> (buffer, len);
}

D64File *
D64File::makeWithFileSystem(FSDevice &volume)
{
    D64File *d64 = nullptr;
        
    switch (volume.getNumBlocks() * 256) {
                        
        case D64_683_SECTORS: d64 = new D64File(35, false); break;
        case D64_768_SECTORS: d64 = new D64File(40, false); break;
        case D64_802_SECTORS: d64 = new D64File(42, false); break;

        default:
            assert(false);
    }

    ErrorCode err;
    if (!volume.exportVolume(d64->data, d64->size, &err)) {

        delete d64;
        throw VC64Error(err);
    }
    
    return d64;
}

PETName<16>
D64File::getName()
{
    return PETName<16>(data + offset(18, 0) + 0x90);
}

usize
D64File::readFromStream(std::istream &stream)
{
    usize result = AnyFile::readFromStream(stream);
    assert(result == size);
    
    unsigned numSectors;
    bool errorCodes;
 
    switch (result)
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
            assert(false);
    }
        
    // Copy error codes
    if (errorCodes) {
        memcpy(errors, data + (numSectors * 256), numSectors);
    }
    
    return result;
}

Track
D64File::numHalftracks()
{
    switch (size) {
            
        case D64_683_SECTORS: case D64_683_SECTORS_ECC: return 2 * 35;
        case D64_768_SECTORS: case D64_768_SECTORS_ECC: return 2 * 40;
        case D64_802_SECTORS: case D64_802_SECTORS_ECC: return 2 * 42;
            
        default:
            assert(false);
            return 0;
    }
}

u8
D64File::getErrorCode(Block b)
{
    assert(b < 802);
    return errors[b];
}

int
D64File::offset(Track track, Sector sector)
{
    // secCnt[track] is the number of the first sector on track 'track'
    const unsigned secCnt[43] = {  0 /* pad */,
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
D64File::dump(Track track, Sector sector)
{
    hexdump(data + offset(track, sector), 256);
}
