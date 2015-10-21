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

#include "C64.h"

typedef struct D64TrackInfo {
	int numberOfSectors;
	int sectorsIn;
	int offset;
} D64TrackInfo;

static const D64TrackInfo D64Map[] =
{
	{ 0,  0,   0 }, // Ignore - track starts at 1, sector starts at 0
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
    memset(name, 0, sizeof(name));
    memset(data, 0, sizeof(data));
    memset(errors, 0, sizeof(errors));
    numTracks = 35;
    fp = 0;
}

D64Archive::~D64Archive()
{
	dealloc();
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

D64Archive *
D64Archive::archiveFromD64File(const char *filename)
{
	D64Archive *archive;
		
	fprintf(stderr, "Loading D64 archive from D64 file...\n");
	archive = new D64Archive();	
	if (!archive->readFromFile(filename)) {
        fprintf(stderr, "Failed to load archive\n");
        delete archive;
		archive = NULL;
	}
	
	return archive;
}

D64Archive *
D64Archive::archiveFromArbitraryFile(const char *filename)
{
	if (D64Archive::isD64File(filename)) {
		return D64Archive::archiveFromD64File(filename);
	}
	
	if (T64Archive::isT64File(filename)) {
		return D64Archive::archiveFromArchive(T64Archive::archiveFromT64File(filename));
	}

	if (PRGArchive::isPRGFile(filename)) {
		return D64Archive::archiveFromArchive(PRGArchive::archiveFromPRGFile(filename));
	}

	if (P00Archive::isP00File(filename)) {
		return D64Archive::archiveFromArchive(P00Archive::archiveFromP00File(filename));
	}

	if (FileArchive::isAcceptableFile(filename)) {
		return D64Archive::archiveFromArchive(FileArchive::archiveFromRawFiledata(filename));
	}
	
	return NULL;
}

D64Archive *
D64Archive::archiveFromD64Archive(D64Archive *otherArchive)
{
    D64Archive *archive;
    
    if (otherArchive == NULL)
        return NULL;
    
    fprintf(stderr, "Cloning D64 archive...\n");
    
    if ((archive = new D64Archive()) == NULL) {
        fprintf(stderr, "Failed to create D64 archive\n");
        return NULL;
    }

    memcpy(archive->name, otherArchive->name, sizeof(archive->name));
    memcpy(archive->data, otherArchive->data, sizeof(archive->data));
    memcpy(archive->errors, otherArchive->errors, sizeof(archive->errors));
    archive->numTracks = otherArchive->numTracks;
    archive->fp = otherArchive->fp;

    return archive;
}

D64Archive *
D64Archive::archiveFromArchive(Archive *otherArchive)
{
    D64Archive *archive;
    
    if (otherArchive == NULL)
		return NULL;
        
	fprintf(stderr, "Creating D64 archive from an %s archive...\n", otherArchive->getTypeAsString());

    if ((archive = new D64Archive()) == NULL) {
        fprintf(stderr, "Failed to create D64 archive\n");
        return NULL;
    }
    
    // Copy file path
    archive->setPath(otherArchive->getPath());
    
    // Current position of data write ptr
    uint8_t track = 1, sector = 0;
        
    // Write BAM
    archive->writeBAM(otherArchive->getName());
    
    // Loop over all entries in archive
    for (int i = 0; i < otherArchive->getNumberOfItems(); i++) {
        
        archive->writeDirectoryEntry(i, otherArchive->getNameOfItem(i), track, sector, otherArchive->getSizeOfItem(i));
        
        // Every file is preceded with two bytes containing its load address
        archive->writeByteToSector(LO_BYTE(otherArchive->getDestinationAddrOfItem(i)), &track, &sector);
        archive->writeByteToSector(HI_BYTE(otherArchive->getDestinationAddrOfItem(i)), &track, &sector);
						  
        // Write raw data to disk
        int byte;
        unsigned num = 0;

        fprintf(stderr, "Will write %d bytes\n", otherArchive->getSizeOfItem(i));

        otherArchive->selectItem(i);
        while ((byte = otherArchive->getByte()) != EOF) {
            archive->writeByteToSector(byte, &track, &sector);
            num++;
        }
        
        fprintf(stderr, "D64 item %d: %d bytes written\n", i, num);
        // Item i has been written. Goto next free sector and proceed with the next item
        (void)archive->nextTrackAndSector(track, sector, &track, &sector, true /* skip directory track */);
    }

    fprintf(stderr, "Archive created (item 0 has %d bytes)\n", archive->getSizeOfItem(0));
    fprintf(stderr, "%s archive created (size of item 0 = %d).\n",
            archive->getTypeAsString(), archive->getSizeOfItem(0));

    return archive;
}

D64Archive *
D64Archive::archiveFromDrive(VC1541 *drive)
{
    D64Archive *archive;
    int error;
    
    fprintf(stderr, "Creating D64 archive from VC1541 drive...\n");
    
    if ((archive = new D64Archive()) == NULL)
        return NULL;
    
    // Perform test run
    if (drive->disk.decodeDisk(NULL, &error) > D64_802_SECTORS_ECC || error) {
        fprintf(stderr, "Cannot create archive (error code: %d)\n", error);
        delete archive;
        return NULL;
    }

    // Get data from drive
    archive->numTracks = 42;
    drive->disk.decodeDisk(archive->data);

    fprintf(stderr, "Archive has %d files\n", archive->getNumberOfItems());
    fprintf(stderr, "Item %d has size: %d\n", 0, archive->getSizeOfItem(0));

    return archive;
}


//
// Virtual functions from Container class
//

bool
D64Archive::fileIsValid(const char *filename)
{
	return D64Archive::isD64File(filename);
}

bool 
D64Archive::readFromBuffer(const uint8_t *buffer, unsigned length)
{
	int numberOfErrors = 0;
	
	switch (length)
	{
		case D64_683_SECTORS: // 35 tracks, no errors
			
            fprintf(stderr, "D64 file contains 35 tracks, no EC bytes\n");
			numTracks = 35;
			break;
            
		case D64_683_SECTORS_ECC: // 35 tracks, 683 error bytes
			
            fprintf(stderr, "D64 file contains 35 tracks, 683 EC bytes\n");
			numTracks = 35;
			numberOfErrors = 683;
			break;
            
		case D64_768_SECTORS: // 40 tracks, no errors
			
            fprintf(stderr, "D64 file contains 40 tracks, no EC bytes\n");
			numTracks = 40;
			break;
            
		case D64_768_SECTORS_ECC: // 40 tracks, 768 error bytes
			
            fprintf(stderr, "D64 file contains 40 tracks, 768 EC bytes\n");
			numTracks = 40;
			numberOfErrors = 768;
			break;
            
		case D64_802_SECTORS: // 42 tracks, no error bytes
            
            fprintf(stderr, "D64 file contains 42 tracks, no EC bytes\n");
			numTracks = 42;
			break;
            
		case D64_802_SECTORS_ECC: // 42 tracks, 802 error bytes
            
            fprintf(stderr, "D64 file contains 42 tracks, 802 EC bytes\n");
			numTracks = 42;
            numberOfErrors = 802;
			break;
            
		default:
            fprintf(stderr, "D64 has an unknown format\n");
			return false;
	}
	
	// Read tracks
	uint8_t *source = (uint8_t *)buffer;
	for(unsigned track = 1; track <= numTracks; track++) {
		
		uint8_t *destination = &data[D64Map[track].offset];
		int sectors = D64Map[track].numberOfSectors;
		memcpy(destination, source, 256 * sectors);
		source += 256 * sectors;
	}
	
	// Read errors
	if (numberOfErrors > 0) {
		memcpy(errors, source, numberOfErrors);
	}

	return true;	
}

unsigned
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
    (void)getNameAsPETString();
    pet2ascii(name);
    return name;
}

//
// Virtual functions from Archive class
//

int
D64Archive::getNumberOfItems()
{
#if 0
    int i = 0;
    
    while (findDirectoryEntry(i) > 0) {
        i++;
    }
    return i;
#endif

    unsigned offsets[MAX_FILES_ON_DISK];
    unsigned noOfFiles;
    
    scanDirectory(offsets, &noOfFiles);
    
    return noOfFiles;
}

const char *
D64Archive::getNameOfItem(int n)
{
    (void)getNameOfItemAsPETString(n);
    pet2ascii(name);
    return name;
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

int
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
    
    // fprintf(stderr, "selectItem:%d\n", item);
    
    // check, if item exists
    if (item >= getNumberOfItems())
        return;
    
    // find directory entry
    if ((fp = findDirectoryEntry(item)) <= 0)
        return;
    
    // fprintf(stderr, "First data sector: %02X, %02X", data[fp+0x03], data[fp+0x04]);
    
    // find first data sector
    if ((fp = offset(data[fp+0x01], data[fp+0x02])) < 0)
        return;
    
    
    // Skip t/s sequence
    fp += 2;
    
    // Skip destination address
    fp += 2;
    
    // We finally reached the first real data byte :-)
    // fprintf(stderr, "Item selected (%d,%d)\n", data[fp+0x03], data[fp+0x04]);
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

const char *
D64Archive::getNameAsPETString()
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

const char *
D64Archive::getNameOfItemAsPETString(int n)
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

unsigned
D64Archive::numberOfSectors(unsigned halftrack)
{
    assert(halftrack >= 1 && halftrack <= 84);
    
    // convert halftrack number to track number
    unsigned track = (halftrack + 1) / 2;
    
    return D64Map[track].numberOfSectors;
}

unsigned
D64Archive::numberOfTracks()
{
    assert(numTracks == 35 || numTracks == 40 || numTracks == 42);
    return numTracks;
}



//
//! @functiongroup Accessing tracks and sectors
//

uint8_t *
D64Archive::findSector(unsigned track, unsigned sector)
{
    return data + offset(track, sector);
}

int
D64Archive::offset(int track, int sector)
{
    assert(isTrackNumber(track));
    assert(sector < D64Map[track].numberOfSectors);
    
    return D64Map[track].offset + (sector * 256);
}

bool
D64Archive::nextTrackAndSector(uint8_t track, uint8_t sector, uint8_t *nextTrack, uint8_t *nextSector, bool skipDirectoryTrack)
{
    unsigned highestSectorNumberInThisTrack = D64Map[track].numberOfSectors - 1;
    
    // PROBLEM?: A REAL VC1541 DISK USUALLY SHOWS AN INTERLEAVE OF 10
    
    if (sector < highestSectorNumberInThisTrack) {
        sector++;
    } else if (track < numTracks) {
        track++;
        sector = 0;
    } else {
        return false; // there is no next sector
    }
    
    if (track == 18 && skipDirectoryTrack) {
        track = 19;
        sector = 0;
    }
    
    *nextTrack = track;
    *nextSector = sector;
    
    return true;
}

bool
D64Archive::jumpToNextSector(int *pos)
{ 
	int nTrack, nSector;
    
	nTrack = nextTrack(*pos);
	nSector = nextSector(*pos);
    
    if (nTrack > (int)numTracks || nSector > D64Map[nTrack].numberOfSectors) {
        return false;
    }
    
    *pos = offset(nTrack, nSector);
    return true;
}

bool
D64Archive::writeByteToSector(uint8_t byte, uint8_t *t, uint8_t *s)
{
    uint8_t track = *t;
    uint8_t sector = *s;
    
    int pos = offset(track, sector);
    uint8_t positionOfLastDataByte = data[pos + 1];
    
    if (positionOfLastDataByte == 0xFF) {
        // fprintf(stderr, "%d/%d is full. ", track, sector);
        // No rool in this sector, proceed to next one
        if (!nextTrackAndSector(track, sector, &track, &sector, true /* skip directory track */)) {
            // Sorry, disk is full
            return false;
        }
        // fprintf(stderr, "Switching to %d/%d\n", track, sector);
        // link previous sector with the new one
        data[pos++] = track;
        data[pos] = sector;
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
D64Archive::markSectorAsUsed(uint8_t track, uint8_t sector)
{
    // For each track and sector, there exists a single bit in the BAM. 1 = used, 0 = unused
    
    // fprintf(stderr,"Marking track %d and sector %d as used\n", track, sector);
    
    // First byte of BAM
    int bam = offset(18,0);
    
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
    pos = offset(18,0);
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
    assert(pos == offset(18,0) + 0x90);
    
    // 90-9F: Disk Name (padded with $A0)
    size_t len = strlen(name);
    for (unsigned k = 0; k < 16; k++)
        data[pos++] = (len > k) ? name[k] : 0xA0;
    
    assert(pos == offset(18,0) + 0xA0);
    
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
    
    assert(pos == offset(18,0) + 0xAB);	
}

void
D64Archive::scanDirectory(unsigned *offsets, unsigned *noOfFiles, bool skipInvisibleFiles)
{
    int pos = offset(18, 1);                // Directory starts on track 18 in sector 1
    bool last_sector = (data[pos] == 0x00); // Does the directory continue in another sector?
    unsigned i = 0, item = 0;
    
    pos += 2;                               // Move to the beginning of the first directory entry

    while (i < MAX_FILES_ON_DISK) {
        
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
    unsigned offsets[MAX_FILES_ON_DISK];
    unsigned noOfFiles;
    
    scanDirectory(offsets, &noOfFiles, skipInvisibleFiles);
    // printf("scanDirectory: %d %d\n", noOfFiles, offsets[0]);
    
    return (item < noOfFiles) ? offsets[item] : -1;
}

bool
D64Archive::writeDirectoryEntry(unsigned nr, const char *name, uint8_t startTrack, uint8_t startSector, unsigned filesize)
{
	int pos;
	
    if (nr >= MAX_FILES_ON_DISK) {
        fprintf(stderr, "Cannot write directory entry. Number of files is limited to %d\n", MAX_FILES_ON_DISK);
		return false;
	}

	// determine sector and relative sector position
	uint8_t sector = 1 + (nr / 8);
	uint8_t rel = (nr % 8) * 0x20;
	
	markSectorAsUsed(18, sector);

	// link to this sector if it is not the first
	if (sector >= 2) {
		pos = offset(18, sector - 1);
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
	data[pos++] = startTrack;
	data[pos++] = startSector;
	
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
D64Archive::dumpSector(int track, int sector)
{
    int pos = offset(track, sector);
    
    fprintf(stderr, "Sector %d/%d\n", track, sector);
    for (int i = 0; i < 256; i++) {
        fprintf(stderr, "%02X ", data[pos++]);
    }
}

