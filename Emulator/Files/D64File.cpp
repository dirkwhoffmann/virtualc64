// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/*
#include "D64File.h"
#include "T64File.h"
#include "PRGFile.h"
#include "P00File.h"
#include "FSDevice.h"
*/
#include "C64.h"

bool
D64File::isD64Buffer(const u8 *buffer, size_t length)
{
    // Unfortunaltely, D64 containers do not contain magic bytes.
    // We can only check the buffer size
    
    return
    length == D64_683_SECTORS ||
    length == D64_683_SECTORS_ECC ||
    length == D64_768_SECTORS ||
    length == D64_768_SECTORS_ECC ||
    length == D64_802_SECTORS ||
    length == D64_802_SECTORS_ECC;
}

bool
D64File::isD64File(const char *filename)
{
    bool fileOK = false;
    
    assert (filename != NULL);
    
    if (!checkFileSuffix(filename, ".D64") && !checkFileSuffix(filename, ".d64"))
        return false;
    
    fileOK = checkFileSize(filename, D64_683_SECTORS, D64_683_SECTORS)
    || checkFileSize(filename, D64_683_SECTORS_ECC, D64_683_SECTORS_ECC)
    || checkFileSize(filename, D64_768_SECTORS, D64_768_SECTORS)
    || checkFileSize(filename, D64_768_SECTORS_ECC, D64_768_SECTORS_ECC)
    || checkFileSize(filename, D64_802_SECTORS, D64_802_SECTORS)
    || checkFileSize(filename, D64_802_SECTORS_ECC, D64_802_SECTORS_ECC);
    
    // Unfortunaltely, D64 containers do not contain magic bytes,
    // so we can't check anything further here
    
    return fileOK;
}


D64File::D64File()
{
    memset(errors, 0x01, sizeof(errors));
}

D64File::D64File(unsigned tracks, bool ecc) : D64File()
{
    switch(tracks) {
        case 35:
            size = ecc ? D64_683_SECTORS_ECC : D64_683_SECTORS;
            break;
            
        case 40:
            size = ecc ? D64_768_SECTORS_ECC : D64_768_SECTORS;
            break;
            
        case 42:
            size = ecc ? D64_802_SECTORS_ECC : D64_802_SECTORS;
            break;
            
        default:
            assert(false);
    }
    
    data = new u8[size];
    memset(data, 0, size);
}

D64File *
D64File::makeWithBuffer(const u8 *buffer, size_t length)
{
    D64File *archive = new D64File();
    
    if (!archive->readFromBuffer(buffer, length)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

D64File *
D64File::makeWithFile(const char *path)
{
    D64File *archive = new D64File();
    
    if (!archive->readFromFile(path)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

D64File *
D64File::makeWithDisk(Disk *disk)
{
    u8 buffer[D64_802_SECTORS];
    
    assert(disk != NULL);
    
    // Translate disk contents into a byte stream
    size_t len = disk->decodeDisk(buffer);

    // Check if the disk has been fully decoded
    if (len != D64_683_SECTORS && len != D64_768_SECTORS && len != D64_802_SECTORS) {
        return NULL;
    }
    
    // Create object from byte stream
    return makeWithBuffer(buffer, len);
}

D64File *
D64File::makeWithDrive(Drive *drive)
{
    assert(drive);
    return makeWithDisk(&drive->disk);
}

D64File *
D64File::makeWithVolume(FSDevice &volume, FSError *error)
{
    D64File *d64 = nullptr;
    
    printf("numBlocks = %d\n", volume.getNumBlocks());
    
    switch (volume.getNumBlocks() * 256) {
                        
        case D64_683_SECTORS:
            d64 = new D64File(35, false);
            break;
            
        case D64_768_SECTORS:
            d64 = new D64File(40, false);
            break;

        case D64_802_SECTORS:
            d64 = new D64File(42, false);
            break;

        default:
            assert(false);
    }

    if (!volume.exportVolume(d64->data, d64->size, error)) {
        delete d64;
        return nullptr;
    }
    
    return d64;
}

const char *
D64File::getName()
{
    int i, pos = offset(18, 0) + 0x90;
    
    for (i = 0; i < 255; i++) {
        if (data[pos+i] == 0xA0)
            break;
        name[i] = data[pos+i];
    }
    name[i] = 0x00;
    return name;
}

bool 
D64File::readFromBuffer(const u8 *buffer, size_t length)
{
    unsigned numSectors;
    bool errorCodes;
 
    switch (length)
    {
        case D64_683_SECTORS: // 35 tracks, no errors
            
            trace(FILE_DEBUG, "D64 file contains 35 tracks, no EC bytes\n");
            numSectors = 683;
            errorCodes = false;
            break;
            
        case D64_683_SECTORS_ECC: // 35 tracks, 683 error bytes
            
            trace(FILE_DEBUG, "D64 file contains 35 tracks, 683 EC bytes\n");
            numSectors = 683;
            errorCodes = true;
            break;
            
        case D64_768_SECTORS: // 40 tracks, no errors
            
            trace(FILE_DEBUG, "D64 file contains 40 tracks, no EC bytes\n");
            numSectors = 768;
            errorCodes = false;
            break;
            
        case D64_768_SECTORS_ECC: // 40 tracks, 768 error bytes
            
            trace(FILE_DEBUG, "D64 file contains 40 tracks, 768 EC bytes\n");
            numSectors = 768;
            errorCodes = true;
            break;
            
        case D64_802_SECTORS: // 42 tracks, no error bytes
            
            trace(FILE_DEBUG, "D64 file contains 42 tracks, no EC bytes\n");
            numSectors = 802;
            errorCodes = false;
            break;
            
        case D64_802_SECTORS_ECC: // 42 tracks, 802 error bytes
            
            trace(FILE_DEBUG, "D64 file contains 42 tracks, 802 EC bytes\n");
            numSectors = 802;
            errorCodes = true;
            break;
            
        default:
            warn("D64 has an unknown format\n");
            return false;
    }
    
    AnyFile::readFromBuffer(buffer, length);
    
    // Copy error codes into seperate array
    if (errorCodes) {
        memcpy(errors, data + (numSectors * 256), numSectors);
    }
    
    return true;    
}

PETName<16>
D64File::getPETName()
{
    return PETName<16>(data + offset(18,0) + 0x90);
}


//
// Accessing archive attributes
//

int
D64File::numberOfHalftracks()
{
    switch (size) {
            
        case D64_683_SECTORS:
        case D64_683_SECTORS_ECC:
            return 2 * 35;
            
        case D64_768_SECTORS:
        case D64_768_SECTORS_ECC:
            return 2 * 40;
            
        case D64_802_SECTORS:
        case D64_802_SECTORS_ECC:
            return 2 * 42;
            
        default:
            assert(false);
            return 0;
    }
}

void
D64File::selectHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    
    selectedHalftrack = ht;
    Track t = (ht + 1) / 2;

    // Check if a real track is requested (D64 files do not store halftracks)
    if ((selectedHalftrack % 2) == 0) {
        tFp = tEof = -1;
        return;
    }
    
    // Check if the requested track is stored inside the D64 file
    if ((int)t > numberOfTracks()) {
        tFp = tEof = -1;
        return;
    }

    tFp = offset(t, 0);
    tEof = tFp + getSizeOfHalftrack();
}

size_t
D64File::getSizeOfHalftrack()
{
    if (selectedHalftrack % 2) {
        return Disk::numberOfSectorsInHalftrack(selectedHalftrack) * 256;
    } else {
        return 0; // Real halftrack (not stored inside D64 files)
    }
}

void
D64File::seekHalftrack(long offset)
{
    // Reset file pointer to the first data byte.
    selectHalftrack(selectedHalftrack);
    
    // Advance file pointer to the requested position.
    if (tFp != -1)
        tFp += offset;
    
    // Invalidate fp if it is out of range.
    if (tFp >= (long)size)
        tFp = -1;
}

void
D64File::selectTrackAndSector(Track t, Sector s)
{
    assert(Disk::isValidTrackSectorPair(t, s));
    
    selectHalftrack(2 * t - 1);
    assert(tFp != -1);
    
    tFp += 256 * s;
    tEof = tFp + 256;
}


//
//! @functiongroup Accessing tracks and sectors
//

u8
D64File::getErrorCode(Block b)
{
    assert(b < 802);
    return errors[b];
}

u8
D64File::getErrorCode(Track t, Sector s)
{
   assert(Disk::isValidTrackSectorPair(t, s));
    
    Sector index = Disk::trackDefaults[t].firstSectorNr + s;
    assert(index < 802);
    
    return errors[index];
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


//
// Accessing file and directory items
//

/*
void
D64File::markSectorAsUsed(Track track, Sector sector)
{
    // For each track and sector, there exists a single bit in the BAM.
    // 1 = used, 0 = unused
    
    // First byte of BAM
    int bam = offset(18, 0);
    
    // Select byte group correspondig to track
    bam += (4 * track);
    
    // Select byte carrying the information for sector
    int offset = 1 + (sector >> 3);
    assert(offset >= 1 && offset <= 3);
    
    // Select bit for this sector
    u8 bitmask = 0x01 << (sector & 0x07);
    
    if (data[bam+offset] & bitmask) {
        // Clear bit
        data[bam + offset] &= ~bitmask;
        
        // Descrease number of free sectors
        assert(data[bam] > 0);
        data[bam]--;
    }
}
*/

//
// Debugging
//

void
D64File::dump(Track track, Sector sector)
{
    int pos = offset(track, sector);
    
    msg("Sector %d/%d\n", track, sector);
    for (int i = 0; i < 256; i++) {
        msg("%02X ", data[pos++]);
    }
}
