/*
 * (C) 2007 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "D64Archive.h"
#include "T64Archive.h"
#include "PRGArchive.h"
#include "P00Archive.h"
#include "FileArchive.h"
#include "Disk.h"

typedef struct D64TrackInfo {
	int numberOfSectors;
	int sectorsIn;
	int offset;
} D64TrackInfo;

static const D64TrackInfo D64Map[] =
{
	{ 0,  0,   0 }, // Padding
	{ 21, 0,   0x00000 },
	{ 21, 21,  0x01500 },
	{ 21, 42,  0x02A00 },
 	{ 21, 63,  0x03F00 },
	{ 21, 84,  0x05400 },
	{ 21, 105, 0x06900 },
	{ 21, 126, 0x07E00 },
	{ 21, 147, 0x09300 },
	{ 21, 168, 0x0A800 },
	{ 21, 189, 0x0BD00 }, 
	{ 21, 210, 0x0D200 },
	{ 21, 231, 0x0E700 },
	{ 21, 252, 0x0FC00 },
	{ 21, 273, 0x11100 },
	{ 21, 294, 0x12600 },
	{ 21, 315, 0x13B00 },
	{ 21, 336, 0x15000 },
	{ 19, 357, 0x16500 }, // Track 18, Directory
	{ 19, 376, 0x17800 },
	{ 19, 395, 0x18B00 },
	{ 19, 414, 0x19E00 },
	{ 19, 433, 0x1B100 },
	{ 19, 452, 0x1C400 },
	{ 19, 471, 0x1D700 },
	{ 18, 490, 0x1EA00 },
	{ 18, 508, 0x1FC00 },
	{ 18, 526, 0x20E00 },
	{ 18, 544, 0x22000 },
	{ 18, 562, 0x23200 },
	{ 18, 580, 0x24400 },
	{ 17, 598, 0x25600 },
	{ 17, 615, 0x26700 },
	{ 17, 632, 0x27800 },
	{ 17, 649, 0x28900 },
	{ 17, 666, 0x29A00 },
	{ 17, 683, 0x2AB00 },
	{ 17, 700, 0x2BC00 },
	{ 17, 717, 0x2CD00 },
	{ 17, 734, 0x2DE00 },
	{ 17, 751, 0x2EF00 },
	// Unusual, tracks 41 & 42
	{ 17, 768, 0x30000 },
	{ 17, 785, 0x31100 }
};

//
// Creating and destructing D64 archives
//

D64Archive::D64Archive()
{
    setDescription("D64Archive");
    memset(name, 0, sizeof(name));
    memset(data, 0, sizeof(data));
    memset(errors, 0x01, sizeof(errors));
    numTracks = 35;
    fp = 0;
}

D64Archive *
D64Archive::makeD64ArchiveWithBuffer(const uint8_t *buffer, size_t length)
{
    D64Archive *archive = new D64Archive();
    
    if (!archive->readFromBuffer(buffer, length)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

D64Archive *
D64Archive::makeD64ArchiveWithFile(const char *path)
{
    D64Archive *archive = new D64Archive();
    
    if (!archive->readFromFile(path)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

D64Archive *
D64Archive::makeD64ArchiveWithAnyArchive(Archive *otherArchive)
{
    assert(otherArchive != NULL);
    
    D64Archive *archive = new D64Archive();
    archive->debug(1, "Creating D64 archive from a %s archive...\n",
                   otherArchive->typeAsString());
    
    // Copy file path
    archive->setPath(otherArchive->getPath());
    
    // Current position of data write ptr
    Track track = 1;
    Sector sector = 0;
    
    // Write BAM
    archive->writeBAM(otherArchive->getName());
    
    // Loop over all entries in archive
    int numberOfItems = otherArchive->getNumberOfItems();
    for (int i = 0; i < numberOfItems; i++) {
        
        archive->writeDirectoryEntry(i, otherArchive->getNameOfItem(i), track, sector, otherArchive->getSizeOfItem(i));
        
        // Every file is preceded with two bytes containing its load address
        uint16_t loadAddr = otherArchive->getDestinationAddrOfItem(i);
        archive->writeByteToSector(LO_BYTE(loadAddr), &track, &sector);
        archive->writeByteToSector(HI_BYTE(loadAddr), &track, &sector);
        
        // Write raw data to disk
        int byte;
        unsigned num = 0;
        
        archive->debug(2, "Will write %d bytes\n", otherArchive->getSizeOfItem(i));
        
        otherArchive->selectItem(i);
        while ((byte = otherArchive->getByte()) != EOF) {
            archive->writeByteToSector(byte, &track, &sector);
            num++;
        }
        
        archive->debug(2, "D64 item %d: %d bytes written\n", i, num);
        // Item i has been written. Goto next free sector and proceed with the next item
        (void)archive->nextTrackAndSector(track, sector, &track, &sector);
    }
    
    archive->debug(2, "Archive created (item 0 has %d bytes)\n", archive->getSizeOfItem(0));
    archive->debug(2, "%s archive created (size of item 0 = %d).\n",
                   archive->typeAsString(), archive->getSizeOfItem(0));
    
    return archive;
}

D64Archive::~D64Archive()
{
	dealloc();
}

bool
D64Archive::isD64(const uint8_t *buffer, size_t length)
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
D64Archive::isD64File(const char *filename)
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


//
// Virtual functions from Container class
//

bool
D64Archive::hasSameType(const char *filename)
{
	return D64Archive::isD64File(filename);
}

bool 
D64Archive::readFromBuffer(const uint8_t *buffer, size_t length)
{
	size_t numberOfErrors = 0;
	
	switch (length)
	{
		case D64_683_SECTORS: // 35 tracks, no errors
			
            debug(2, "D64 file contains 35 tracks, no EC bytes\n");
			numTracks = 35;
			break;
            
		case D64_683_SECTORS_ECC: // 35 tracks, 683 error bytes
			
            debug(2, "D64 file contains 35 tracks, 683 EC bytes\n");
			numTracks = 35;
			numberOfErrors = 683;
			break;
            
		case D64_768_SECTORS: // 40 tracks, no errors
			
            debug(2, "D64 file contains 40 tracks, no EC bytes\n");
			numTracks = 40;
			break;
            
		case D64_768_SECTORS_ECC: // 40 tracks, 768 error bytes
			
            debug(2, "D64 file contains 40 tracks, 768 EC bytes\n");
			numTracks = 40;
			numberOfErrors = 768;
			break;
            
		case D64_802_SECTORS: // 42 tracks, no error bytes
            
            debug(2, "D64 file contains 42 tracks, no EC bytes\n");
			numTracks = 42;
			break;
            
		case D64_802_SECTORS_ECC: // 42 tracks, 802 error bytes
            
            debug(2, "D64 file contains 42 tracks, 802 EC bytes\n");
			numTracks = 42;
            numberOfErrors = 802;
			break;
            
		default:
            warn("D64 has an unknown format\n");
			return false;
	}
	
	// Read tracks
	uint8_t *source = (uint8_t *)buffer;
	for(Track t = 1; t <= numTracks; t++) {
		
		uint8_t *destination = &data[D64Map[t].offset];
		int sectors = D64Map[t].numberOfSectors;
		memcpy(destination, source, 256 * sectors);
		source += 256 * sectors;
	}
	
	// Read errors
	if (numberOfErrors > 0) {
		memcpy(errors, source, numberOfErrors);
	}

	return true;	
}

size_t
D64Archive::writeToBuffer(uint8_t *buffer)
{
    switch (numTracks) {
            
        case 35:
            if (buffer)
                memcpy(buffer, data, D64_683_SECTORS);
            return D64_683_SECTORS;
            
        case 40:
            if (buffer)
                memcpy(buffer, data, D64_768_SECTORS);
            return D64_768_SECTORS;
            
        case 42:
            if (buffer)
                memcpy(buffer, data, D64_802_SECTORS);
            return D64_802_SECTORS;
            
        default:
            assert(0);
    }
    
    return 0;
}

const char *
D64Archive::getName()
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

const unsigned short *
D64Archive::getUnicodeName(size_t maxChars)
{
    (void)getName();
    translateToUnicode(name, unicode, 0xE000, maxChars);
    return unicode;
}

//
// Virtual functions from Archive class
//

int
D64Archive::getNumberOfItems()
{
    unsigned offsets[144]; // a C64 disk contains at most 144 files
    unsigned noOfFiles;
    
    scanDirectory(offsets, &noOfFiles);
    
    return noOfFiles;
}

const char *
D64Archive::getNameOfItem(int n)
{
    int i, pos = findDirectoryEntry(n);
    
    if (pos <= 0) return NULL;
    pos += 0x03; // filename begins here
    for (i = 0; i < 16; i++) {
        if (data[pos+i] == 0xA0)
            break;
        name[i] = data[pos+i];
    }
    name[i] = 0x00;
    return name;
}

const unsigned short *
D64Archive::getUnicodeNameOfItem(int n, size_t maxChars)
{
    (void)getNameOfItem(n);
    translateToUnicode(name, unicode, 0xE000, maxChars);
    return unicode;
}

const char *
D64Archive::getTypeOfItem(int n)
{
    const char *extension = "";
    int pos = findDirectoryEntry(n);
    
    if (pos > 0)
        (void)itemIsVisible(data[pos] /* file type byte */, &extension);

    return extension;
}

bool
D64Archive::itemIsVisible(uint8_t typeChar, const char **extension)
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

size_t
D64Archive::getSizeOfItemInBlocks(int n)
{
    int pos = findDirectoryEntry(n);
    
    return (pos > 0) ? LO_HI(data[pos+0x1C],data[pos+0x1D]) : 0;
}

uint16_t
D64Archive::getDestinationAddrOfItem(int n)
{
    int pos;
    int track;
    int sector;
    uint16_t result;
    
    // Search for beginning of file data
    pos = findDirectoryEntry(n);
    if (pos <= 0)
        return 0;
    
    track = data[pos + 0x01];
    sector = data[pos + 0x02];
    if ((pos = offset(track, sector)) < 0)
        return 0;
    
    result = LO_HI(data[pos+2],data[pos+3]);
    return result;
}

void
D64Archive::selectItem(int item)
{
    fp = -1;
    
    // check, if item exists
    if (item >= getNumberOfItems())
        return;
    
    // find directory entry
    if ((fp = findDirectoryEntry(item)) <= 0)
        return;
    
    // find first data sector
    if ((fp = offset(data[fp+0x01], data[fp+0x02])) < 0)
        return;
    
    // Skip t/s sequence
    fp += 2;
    
    // Skip destination address
    fp += 2;
    
    // We finally reached the first real data byte :-)
}

int 
D64Archive::getByte()
{
    int result;
    
    if (fp < 0)
        return -1;
    
    // Get byte
    result = data[fp];
    
    // Check for end of file
    if (isEndOfFile(fp)) {
        fp = -1;
        return result;
    }
    
    if (isLastByteOfSector(fp)) {
        
        // Continue reading in new sector
        if (!jumpToNextSector(&fp)) {
            // The current sector points to an invalid next track/sector
            // We won't jump off the cliff and terminate reading here.
            fp = -1;
            return result;
        } else {
            // Skip the first two data bytes of the new sector as they encode the
            // next track/sector
            fp += 2;
            return result;
        }
    }
    
    // Continue reading in current sector
    fp++;
    return result;
}

//
// Accessing archive attributes
//

/*
unsigned
D64Archive::numberOfSectors(unsigned halftrack)
{
    assert(halftrack >= 1 && halftrack <= 84);
    
    // convert halftrack number to track number
    unsigned track = (halftrack + 1) / 2;
    
    return D64Map[track].numberOfSectors;
}
*/

unsigned
D64Archive::numberOfTracks()
{
    assert(numTracks == 35 || numTracks == 40 || numTracks == 42);
    return numTracks;
}

void
D64Archive::setNumberOfTracks(unsigned tracks)
{
    assert(numTracks == 35 || numTracks == 40 || numTracks == 42);
    numTracks = tracks;
}


//
//! @functiongroup Accessing tracks and sectors
//

uint8_t *
D64Archive::findSector(Track t, Sector s)
{
    assert(isValidTrackSectorPair(t, s));
    return data + offset(t, s);
}

uint8_t
D64Archive::errorCode(Track t, Sector s)
{
   assert(isValidTrackSectorPair(t, s));
    
    Sector index = Disk::trackDefaults[t].firstSectorNr + s;
    assert(index < 802);
    
    return errors[index];
    
}


int
D64Archive::offset(Track track, Sector sector)
{
    if (isValidTrackSectorPair(track, sector)) {
        return D64Map[track].offset + (sector * 256);
    } else {
        return -1;
    }
}

bool
D64Archive::nextTrackAndSector(Track track, Sector sector,
                               Track *nextTrack, Sector *nextSector,
                               bool skipDirectoryTrack)
{
    assert(nextTrack != NULL);
    assert(nextSector != NULL);
    assert(isValidTrackSectorPair(track, sector));
    
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
        if (track < numTracks) {
            track = (track == 17 && skipDirectoryTrack) ? 19 : track + 1;
            sector = 0;
        } else {
            return false; // there is no next track
        }
    }
    
    assert(isValidTrackSectorPair(track, sector));
    *nextTrack = track;
    *nextSector = sector;
    return true;
}

bool
D64Archive::jumpToNextSector(int *pos)
{ 
	int nTrack, nSector, newPos;
    
	nTrack = nextTrack(*pos);
	nSector = nextSector(*pos);
    
    if (nTrack > (int)numTracks)
        return false;
    
    if ((newPos = offset(nTrack, nSector)) < 0)
        return false;
    
    *pos = newPos;
    return true;
}

bool
D64Archive::writeByteToSector(uint8_t byte, Track *t, Sector *s)
{
    Track track = *t;
    Sector sector = *s;
 
    assert(isValidTrackSectorPair(track, sector));

    int pos = offset(track, sector);
    uint8_t positionOfLastDataByte = data[pos + 1];
    
    if (positionOfLastDataByte == 0xFF) {

        // No free slots in this sector, proceed to next one
        if (!nextTrackAndSector(track, sector, &track, &sector)) {
            return false; // Sorry, disk is full
        }

        // link previous sector with the new one
        data[pos++] = (uint8_t)track;
        data[pos] = (uint8_t)sector;
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
//! Accessing file and directory items
//

void
D64Archive::markSectorAsUsed(Track track, Sector sector)
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
    uint8_t bitmask = 0x01 << (sector & 0x07);
    
    if (data[bam+offset] & bitmask) {
        // Clear bit
        data[bam + offset] &= ~bitmask;
        
        // Descrease number of free sectors
        assert(data[bam] > 0);
        data[bam]--;
    }
}

void
D64Archive::writeBAM(const char *name)
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
            int sectors = D64Map[k].numberOfSectors;
            data[pos++] = sectors; // Number of free sectors on this track
            data[pos++] = 0xFF;    // Occupation bitmap: 1 = sector is free
            data[pos++] = 0xFF;
            if (sectors == 21) data[pos++] = 0x1F;
            else if (sectors == 19) data[pos++] = 0x07;
            else if (sectors == 18) data[pos++] = 0x03;
            else if (sectors == 17) data[pos++] = 0x01;
            else assert(0);
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
D64Archive::scanDirectory(unsigned *offsets, unsigned *noOfFiles, bool skipInvisibleFiles)
{
    // Directory starts on track 18 in sector 1
    int pos = offset(18, 1);
    
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


int
D64Archive::findDirectoryEntry(int item, bool skipInvisibleFiles)
{
    unsigned offsets[144];
    unsigned noOfFiles;
    
    scanDirectory(offsets, &noOfFiles, skipInvisibleFiles);
    // printf("scanDirectory: %d %d\n", noOfFiles, offsets[0]);
    
    return (item < noOfFiles) ? offsets[item] : -1;
}

bool
D64Archive::writeDirectoryEntry(unsigned nr, const char *name,
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
    uint8_t sector = secnr[1 + (nr / 8)];
	uint8_t rel = (nr % 8) * 0x20;
	
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
	data[pos++] = (uint8_t)startTrack;
	data[pos++] = (uint8_t)startSector;
	
	// 05-14: 16 character filename (in PETASCII, padded with $A0)
	size_t len = strlen(name);
	for (unsigned k = 0; k < 16; k++)
		data[pos++] = (len > k) ? name[k] : 0xA0;
	
	assert(pos == offset(18, sector) + rel + 0x15);
	
	// 1E-1F: File size in sectors, low/high byte order
	pos = offset(18, sector) + rel + 0x1E;
	filesize += 2; // Each file stores 2 additional bytes containing the load address
	uint16_t fileSizeInSectors = (filesize % 254 == 0) ? filesize / 254 : filesize / 254 + 1; 
	data[pos++] = LO_BYTE(fileSizeInSectors);
	data[pos++] = HI_BYTE(fileSizeInSectors);

	return true;
}


//
// Debugging
//

void
D64Archive::dumpSector(Track track, Sector sector)
{
    int pos = offset(track, sector);
    
    msg("Sector %d/%d\n", track, sector);
    for (int i = 0; i < 256; i++) {
        msg("%02X ", data[pos++]);
    }
}

