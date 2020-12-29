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
D64File::makeWithAnyArchive(AnyArchive *otherArchive)
{
    assert(otherArchive);
    
    // Create a standard 35 track disk with no error checking codes
    D64File *archive = new D64File(35, false);
    debug(FILE_DEBUG, "Creating D64 archive from a %s archive...\n",
          otherArchive->typeString());
    
    // Copy file path
    archive->setPath(otherArchive->getPath());
    
    // Current position of data write ptr
    Track track = 1;
    Sector sector = 0;
    
    // Write BAM
    archive->writeBAM(otherArchive->getName());
    
    // Loop over all entries in archive
    int numberOfItems = otherArchive->numberOfItems();
    for (int i = 0; i < numberOfItems; i++) {
        
        otherArchive->selectItem(i);
        
        archive->writeDirectoryEntry(i,
                                     otherArchive->getNameOfItem(),
                                     track, sector,
                                     otherArchive->getSizeOfItem());
        
        // Every file is preceded with two bytes containing its load address
        u16 loadAddr = otherArchive->getDestinationAddrOfItem();
        archive->writeByteToSector(LO_BYTE(loadAddr), &track, &sector);
        archive->writeByteToSector(HI_BYTE(loadAddr), &track, &sector);
        
        // Write raw data to disk
        int byte;
        unsigned num = 0;
        
        debug(FILE_DEBUG, "Will write %zu bytes\n", otherArchive->getSizeOfItem());
        
        otherArchive->selectItem(i);
        while ((byte = otherArchive->readItem()) != EOF) {
            archive->writeByteToSector(byte, &track, &sector);
            num++;
        }
        
        // Make sure the last block doesn't link to another block
        int off = archive->offset(track, sector);
        archive->data[off + 0] = 0;
        archive->data[off + 1] = 0;

        debug(FILE_DEBUG, "D64 item %d: %d bytes written\n", i, num);
        // Item i has been written. Goto next free sector and proceed with the next item
        (void)archive->nextTrackAndSector(track, sector, &track, &sector);
    }
    
    debug(FILE_DEBUG, "%s archive created.\n", archive->typeString());
    
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

FSName
D64File::getFSName()
{
    return FSName(data + offset(18,0) + 0x90);
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

int
D64File::numberOfItems()
{
    long offsets[144]; // A C64 disk contains at most 144 files
    unsigned noOfFiles;
    
    scanDirectory(offsets, &noOfFiles);
    
    return noOfFiles;
}

void
D64File::selectItem(unsigned item)
{
    // Only proceed of item exists
    if ((long)item >= numberOfItems())
        return;
    
    // Remember the selection
    selectedItem = item;
    
    // Move file pointer to the first data byte
    iFp = findItem(item);
}

const char *
D64File::getTypeOfItem()
{
    assert(selectedItem != -1);
    
    const char *extension = "";
    long pos = findDirectoryEntry(selectedItem);
    
    if (pos > 0)
        (void)itemIsVisible(data[pos] /* file type byte */, &extension);
    
    return extension;
}

const char *
D64File::getNameOfItem()
{
    assert(selectedItem != -1);
    
    long pos;
    int i;
    
    // Search the selected item in directory
    if ((pos = findDirectoryEntry(selectedItem)) <= 0)
        return NULL;
    
    pos += 0x03; // The filename begins here
    for (i = 0; i < 16; i++) {
        if (data[pos+i] == 0xA0)
            break;
        name[i] = data[pos+i];
    }
    name[i] = 0x00;
    
    for (size_t i = 0; i < strlen(name); i++) {
        name[i] = petscii2printable(name[i], ' ');
    }
    return name;
}

size_t
D64File::getSizeOfItem()
{
    if (selectedItem < 0)
        return 0;
    
    // In a D64 archive, the bytes of a single file item are not ordered
    // consecutively. Hence, we have to step through the data byte by byte.
    long oldFp = iFp;
    size_t result = 0;
    
    while (readItem() != EOF)
        result++;
    
    iFp = oldFp;
    return result;
}

size_t
D64File::getSizeOfItemInBlocks()
{
    assert(selectedItem != -1);
    
    long pos = findDirectoryEntry(selectedItem);
    return (pos > 0) ? LO_HI(data[pos+0x1C],data[pos+0x1D]) : 0;
}

void
D64File::seekItem(long offset)
{
    // Reset file pointer to the beginning of the selected item
    iFp = findItem(selectedItem);

    // Advance file pointer to the requested position
    for (unsigned i = 0; i < offset; i++)
        (void)readItem();
}

int
D64File::readItem()
{
    int result;
    
    if (iFp < 0)
        return -1;
    
    // Get byte
    result = data[iFp];
    
    // Check for end of file
    if (isEndOfFile(iFp)) {
        iFp = -1;
        return result;
    }
    
    if (isLastByteOfSector(iFp)) {
        
        // Continue reading in new sector
        if (!jumpToNextSector(&iFp)) {
            // The current sector points to an invalid next track/sector
            // We won't jump off the cliff and terminate reading here.
            iFp = -1;
            return result;
        } else {
            // Skip the first two data bytes of the new sector as they encode the
            // next track/sector
            iFp += 2;
            return result;
        }
    }
    
    // Continue reading in current sector
    iFp++;
    return result;
}

u16
D64File::getDestinationAddrOfItem()
{
    int track;
    int sector;
    u16 result;
    
    assert(selectedItem != -1);
    
    // Search for beginning of file data
    long pos = findDirectoryEntry(selectedItem);
    if (pos <= 0)
        return 0;
    
    track = data[pos + 0x01];
    sector = data[pos + 0x02];
    if ((pos = offset(track, sector)) < 0)
        return 0;
    
    result = LO_HI(data[pos+2],data[pos+3]);
    return result;
}

long
D64File::findItem(long item)
{
    long p;
    
    if (item < 0)
        return -1;
    
    // Find directory entry
    if ((p = findDirectoryEntry(item)) <= 0)
        return -1;
    
    // Find first data sector
    if ((p = offset(data[p+0x01], data[p+0x02])) < 0)
        return -1;
    
    // Skip t/s sequence
    p += 2;
    
    // Skip destination address
    p += 2;
    
    return p;
}


bool
D64File::itemIsVisible(u8 typeChar, const char **extension)
{
    const char *result = NULL;
    
    switch (typeChar) {
        case 0x80: result = "DEL"; break;
        case 0x81: result = "SEQ"; break;
        case 0x82: result = "PRG"; break;
        case 0x83: result = "USR"; break;
        case 0x84: result = "REL"; break;
            
        case 0x01: result = "*SEQ"; break;
        case 0x02: result = "*PRG"; break;
        case 0x03: result = "*USR"; break;
            
        case 0xA0: result = "DEL"; break;
        case 0xA1: result = "SEQ"; break;
        case 0xA2: result = "PRG"; break;
        case 0xA3: result = "USR"; break;
            
        case 0xC0: result = "DEL <"; break;
        case 0xC1: result = "SEQ <"; break;
        case 0xC2: result = "PRG <"; break;
        case 0xC3: result = "USR <"; break;
        case 0xC4: result = "REL <"; break;
    }

    if (extension)
        *extension = result ? result : "";

    // printf("itemIsVisible as %s\n", result == NULL ? "" : result);
    return result != NULL;
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
D64File::errorCode(Track t, Sector s)
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

bool
D64File::nextTrackAndSector(Track track, Sector sector,
                               Track *nextTrack, Sector *nextSector,
                               bool skipDirectoryTrack)
{
    assert(nextTrack != NULL);
    assert(nextSector != NULL);
    assert(Disk::isValidTrackSectorPair(track, sector));
    
    // Interleave pattern for all four speed zones and the directory track
    Sector zone3[] = { 10,11,12,13,14,15,16,17,18,19,20,0,1,2,3,4,5,6,7,8,9 };
    Sector zone2[] = { 10,11,12,13,14,15,16,17,18,0,1,2,3,4,5,6,7,8,9 };
    Sector trk18[] = { 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,1,2 };
    Sector zone1[] = { 10,11,12,13,14,15,16,17,1,0,2,3,4,5,6,7,8,9 };
    Sector zone0[] = { 10,11,12,13,14,15,16,0,1,2,3,4,5,6,7,8,9 };

    // Determine interleave pattern for this track
    Sector *next =
    (track < 18) ? zone3 :
    (track == 18) ? trk18 :
    (track < 25) ? zone2 :
    (track < 31) ? zone1 : zone0;
    
    // Move to next sector
    sector = next[sector];
    
    // Move to next track if we wrapped over
    if (sector == 0) {
        if ((int)track < numberOfTracks()) {
            track = (track == 17 && skipDirectoryTrack) ? 19 : track + 1;
            sector = 0;
        } else {
            return false; // there is no next track
        }
    }
    
    assert(Disk::isValidTrackSectorPair(track, sector));
    *nextTrack = track;
    *nextSector = sector;
    return true;
}

bool
D64File::jumpToNextSector(long *pos)
{ 
    int nTrack, nSector;
    long newPos;
    
    nTrack = nextTrack(*pos);
    nSector = nextSector(*pos);
    
    if (nTrack > numberOfTracks())
        return false;
    
    if ((newPos = offset(nTrack, nSector)) < 0)
        return false;
    
    *pos = newPos;
    return true;
}

bool
D64File::writeByteToSector(u8 byte, Track *t, Sector *s)
{
    Track track = *t;
    Sector sector = *s;
 
    assert(Disk::isValidTrackSectorPair(track, sector));

    int pos = offset(track, sector);
    u8 positionOfLastDataByte = data[pos + 1];
    
    if (positionOfLastDataByte == 0xFF) {

        // No free slots in this sector, proceed to next one
        if (!nextTrackAndSector(track, sector, &track, &sector)) {
            return false; // Sorry, disk is full
        }

        // link previous sector with the new one
        data[pos++] = (u8)track;
        data[pos] = (u8)sector;
        pos = offset(track, sector);
        positionOfLastDataByte = 0;
    }
    
    // Write byte
    if (positionOfLastDataByte == 0) {
        markSectorAsUsed(track, sector);
        data[pos + 2] = byte;
        data[pos + 1] = 0x02;
    } else {
        positionOfLastDataByte++;
        data[pos + positionOfLastDataByte] = byte;
        data[pos + 1] = positionOfLastDataByte;        
    }
    
    *t = track;
    *s = sector;
    
    return true;
}


//
// Accessing file and directory items
//

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

void
D64File::writeBAM(const char *name)
{
    int pos;
    
    // 00/01: Track/Sector location of the first directory sector (should be 18/1)
    markSectorAsUsed(18, 0);
    pos = offset(18, 0);
    data[pos++] = 18;
    data[pos++] = 1;
    
    // 02: Disk DOS version type (see note below)
    data[pos++] = 0x41; // "A"
    
    // 03: Unused
    pos++;
    
    // 04-8F: BAM entries for each track, in groups  of  four  bytes
    for (unsigned k = 1; k <= 35; k++) {
        if (k == 18) {
            data[pos++] = 0; // no free sectors on directory track
            data[pos++] = 0x00;
            data[pos++] = 0x00;
            data[pos++] = 0x00;
        } else {
            int sectors = Disk::numberOfSectorsInTrack(k);
            data[pos++] = sectors; // Number of free sectors on this track
            data[pos++] = 0xFF;    // Occupation bitmap: 1 = sector is free
            data[pos++] = 0xFF;
            if (sectors == 21) data[pos++] = 0x1F;
            else if (sectors == 19) data[pos++] = 0x07;
            else if (sectors == 18) data[pos++] = 0x03;
            else if (sectors == 17) data[pos++] = 0x01;
            else assert(false);
        }
    }
    assert(pos == offset(18, 0) + 0x90);
    
    // 90-9F: Disk Name (padded with $A0)
    size_t len = strlen(name);
    for (unsigned k = 0; k < 16; k++)
        data[pos++] = (len > k) ? name[k] : 0xA0;
    
    assert(pos == offset(18, 0) + 0xA0);
    
    // A0-A1: Filled with $A0
    data[pos++] = 0xA0;
    data[pos++] = 0xA0;
    
    // A2-A3: Disk ID
    data[pos++] = 0x56;
    data[pos++] = 0x54;
    
    // A4: Usually $A0
    data[pos++] = 0xA0;
    
    // A5-A6: DOS type
    data[pos++] = 0x32; // "2"
    data[pos++] = 0x41; // "A"
    
    // A7-AA: Filled with $A0
    data[pos++] = 0xA0;
    data[pos++] = 0xA0;
    data[pos++] = 0xA0;
    data[pos++] = 0xA0;
    
    assert(pos == offset(18, 0) + 0xAB);
}

void
D64File::scanDirectory(long *offsets, unsigned *noOfFiles, bool skipInvisibleFiles)
{
    // Directory starts on track 18 in sector 1
    long pos = offset(18, 1);
    
    // Does the directory continue in another sector?
    bool last_sector = (data[pos] == 0x00);

    // Move to the beginning of the first directory entry
    pos += 2;

    unsigned i = 0, item = 0;
    while (i < 144 /* maximum number of files on disk */) {
        
        // Only proceed if the directory entry is not a null entry
        const char nullEntry[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
        if (memcmp(&data[pos], nullEntry, 32) == 0)
            break;
        
        // Skip invisble files if requested
        if (!skipInvisibleFiles || itemIsVisible(data[pos]))
            offsets[item++] = pos;
        
        // Jump to next directory item
        if (++i % 8 == 0) {
            
            // Jump to the next sector
            if (last_sector)
                break; // Sorry, there is no next sector
            
            if (!jumpToNextSector(&pos))
                break; // Sorry, somebody wants to sent us off the cliff
            
            last_sector = (data[pos] == 0x00);
            pos += 2; // Move to the beginning of the first directory entry
            
        } else {
            pos += 0x20; // Jump to next directory entry inside current sector
        }
    }
    
    *noOfFiles = item;
}


long
D64File::findDirectoryEntry(long item, bool skipInvisibleFiles)
{
    long offsets[144];
    unsigned noOfFiles;
    
    scanDirectory(offsets, &noOfFiles, skipInvisibleFiles);
    return (item < noOfFiles) ? offsets[item] : -1;
}

bool
D64File::writeDirectoryEntry(unsigned nr, const char *name,
                             Track startTrack, Sector startSector,
                             size_t filesize)
{
    int pos;
    
    // Sector interleave pattern for the directory track
    // 18,0 is the BAM, and the first 8 directory items are located at 18,1.
    // After that, an interleave pattern of 3 is applied. 
    Sector secnr[] = { 0,1,4,7,10,13,16,2,5,8,11,14,17,3,6,9,12,15,18 };
        
    if (nr >= 144) {
        warn("Cannot write directory entry. Number of files is limited to 144\n");
        return false;
    }

    // Determine sector and relative sector position for this entry
    u8 sector = secnr[1 + (nr / 8)];
    u8 rel = (nr % 8) * 0x20;
    
    // Update BAM
    markSectorAsUsed(18, sector);

    // Link to this sector if it is not the first
    if (sector != 1) {
        pos = offset(18, secnr[nr / 8]);
        data[pos++] = 18;
        data[pos] = sector;
    }

    pos = offset(18, sector) + rel;
    
    // 00-01: Next directory sector (item 0) or 00 00 (other items)
    if (nr == 0) {
        pos++; // don't modify
        pos++; // don't modify
    } else {
        data[pos++] = 0x00;
        data[pos++] = 0x00;
    }
        
    // 02: File type (0x82 = PRG)
    data[pos++] = 0x82;
    
    // 03-04: Track/sector location of first sector of file
    data[pos++] = (u8)startTrack;
    data[pos++] = (u8)startSector;
    
    // 05-14: 16 character filename (in PETASCII, padded with $A0)
    size_t len = strlen(name);
    for (unsigned k = 0; k < 16; k++)
        data[pos++] = (len > k) ? name[k] : 0xA0;
    
    assert(pos == offset(18, sector) + rel + 0x15);
    
    // 1E-1F: File size in sectors, low/high byte order
    pos = offset(18, sector) + rel + 0x1E;
    filesize += 2; // Each file stores 2 additional bytes containing the load address
    u16 fileSizeInSectors = (filesize % 254 == 0) ? filesize / 254 : filesize / 254 + 1; 
    data[pos++] = LO_BYTE(fileSizeInSectors);
    data[pos++] = HI_BYTE(fileSizeInSectors);

    return true;
}


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
