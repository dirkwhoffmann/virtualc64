/*
 * (C) 2007 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, org
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

#include "T64Archive.h"

T64Archive::T64Archive()
{
	data = NULL;
	dealloc();
}

T64Archive::~T64Archive()
{
	dealloc();
}

bool 
T64Archive::isT64File(const char *filename)
{
    /* "Anmerkung: Der String muß nicht wortwörtlich so vorhanden sein. Man sollte nach den Substrings "C64" und "tape" suchen. Vorsicht: TAP Images verwenden den String: C64-TAPE-RAW der ebenfalls die Substrings "C64" und "TAPE" enthält." [Power64 doc] 
        TODO: Make sure that the archive is not a TAP file. */
	int magic_bytes[] = { 0x43, 0x36, 0x34, EOF };
	
	assert(filename != NULL);
	
	if (!checkFileSuffix(filename, ".T64") && !checkFileSuffix(filename, ".t64"))
		return false;
	
	if (!checkFileSize(filename, 0x40, -1))
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
	
	return true;
}

T64Archive *
T64Archive::archiveFromT64File(const char *filename)
{
	T64Archive *archive;
	
	fprintf(stderr, "Loading T64 archive from T64 file...\n");
	archive = new T64Archive();	
	if (!archive->readFromFile(filename)) {
        fprintf(stderr, "Failed to load archive\n");
		delete archive;
		archive = NULL;
	}
	
	return archive;
}

T64Archive *
T64Archive::archiveFromArchive(Archive *otherArchive)
{
    T64Archive *archive;
    
    if (otherArchive == NULL)
        return NULL;
    
    fprintf(stderr, "Creating T64 archive from %s archive...\n", otherArchive->getTypeAsString());
    
    if ((archive = new T64Archive()) == NULL) {
        fprintf(stderr, "Failed to create archive\n");
        return NULL;
    }
    
    // Determine container size and allocate memory
    unsigned currentFiles = otherArchive->getNumberOfItems();
    unsigned maxFiles = (currentFiles < 30) ? 30 : currentFiles;
    archive->size = 64 /* header */ + maxFiles * 32 /* tape entries */;
    
    for (unsigned i = 0; i < otherArchive->getNumberOfItems(); i++)
        archive->size += otherArchive->getSizeOfItem(i);
    
    if ((archive->data = (uint8_t *)malloc(archive->size)) == NULL) {
        fprintf(stderr, "Failed to allocate %d bytes of memory\n", archive->size);
        delete archive;
        return NULL;
    }
    
    // Magic bytes (32 bytes)
    uint8_t *ptr = archive->data;
    strncpy((char *)ptr, "C64 tape image file", 32);
    ptr += 32;
    
    // Version (2 bytes)
    *ptr++ = 0x00;
    *ptr++ = 0x01;
    
    // Max files (2 bytes)
    *ptr++ = LO_BYTE(maxFiles);
    *ptr++ = HI_BYTE(maxFiles);

    // Current files (2 bytes)
    *ptr++ = LO_BYTE(currentFiles);
    *ptr++ = HI_BYTE(currentFiles);

    // Reserved (2 bytes)
    *ptr++ = 0x00;
    *ptr++ = 0x00;
    
    // User description (24 bytes)
    strncpy((char *)ptr, (char *)otherArchive->getName(), 24);
    for (unsigned i = 0; i < 24; i++, ptr++)
        *ptr = ascii2pet(*ptr);
    
    // Tape entries
    uint32_t tapePosition = 64 + maxFiles * 32; // data of item 0 starts here
    memset(ptr, 0, 32 * maxFiles);
    for (unsigned n = 0; n < maxFiles; n++) {

        if (n >= currentFiles) {
            // Empty tape slot
            ptr += 32;
            continue;
        }
        
        // Entry used (1 byte)
        *ptr++ = 0x01;
        
        // File type (1 byte)
        *ptr++ = 0x82;
        
        // Start address (2 bytes)
        uint16_t startAddr = otherArchive->getDestinationAddrOfItem(n);
        *ptr++ = LO_BYTE(startAddr);
        *ptr++ = HI_BYTE(startAddr);
            
        // Start address (2 bytes)
        uint16_t endAddr = startAddr + otherArchive->getSizeOfItem(n);
        *ptr++ = LO_BYTE(endAddr);
        *ptr++ = HI_BYTE(endAddr);
        
        // Reserved (2 bytes)
        ptr += 2;
            
        // Tape position (4 bytes)
        *ptr++ = LO_BYTE(tapePosition);
        *ptr++ = LO_BYTE(tapePosition >> 8);
        *ptr++ = LO_BYTE(tapePosition >> 16);
        *ptr++ = LO_BYTE(tapePosition >> 24);
        tapePosition += otherArchive->getSizeOfItem(n);
            
        // Reserved (4 bytes)
        ptr += 4;
            
        // File name (16 bytes)
        strncpy((char *)ptr, (char *)otherArchive->getNameOfItem(n), 16);
        for (unsigned i = 0; i < 16; i++, ptr++)
            *ptr = ascii2pet(*ptr);
    }
    
    // File data
    for (unsigned n = 0; n < currentFiles; n++) {

        int byte;
        otherArchive->selectItem(n);
        while ((byte = otherArchive->getByte()) != EOF) {
            *ptr++ = (uint8_t)byte;
        }
        
    }
    
    otherArchive->dumpDirectory();
    archive->dumpDirectory();
    
    return archive;
}

void T64Archive::dealloc()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
	fp_eof = -1;
}

bool 
T64Archive::fileIsValid(const char *filename)
{
	return T64Archive::isT64File(filename);
}

bool 
T64Archive::readFromBuffer(const uint8_t *buffer, unsigned length)
{	
	if ((data = (uint8_t *)malloc(length)) == NULL)
		return false;

	memcpy(data, buffer, length);
	size = length;

	return true;
}

unsigned
T64Archive::writeToBuffer(uint8_t *buffer)
{
    assert(data != NULL);
    
    if (buffer) {
        memcpy(buffer, data, size);
    }
    return size;
}


const char *
T64Archive::getName()
{
	int i,j;
	int first = 0x28;
	int last  = 0x40;
	
	for (j = 0, i = first; i < last; i++, j++) {
		name[j] = (data[i] == 0x20 ? ' ' : data[i]);
		if (j == 255) break; 
	}
	name[j] = 0x00;
	return name;
}

bool 
T64Archive::directoryItemIsPresent(int n)
{
	int first = 0x40 + (n * 0x20);
	int last  = 0x60 + (n * 0x20);
	int i;
	
	// check for zeros...
	if (last < size)
		for (i = first; i < last; i++)
			if (data[i] != 0)
				return true;

	return false;
}

int 
T64Archive::getNumberOfItems()
{
	int noOfItems;

	// Get number of files from the file header...
    noOfItems = LO_HI(data[0x24], data[0x25]);

	if (noOfItems == 0) {

        // Note: Some archives don't store this value properly.
		// In this case, we can determine the number of files
		// by iterating through the directory area...
		while (directoryItemIsPresent(noOfItems))
			noOfItems++;
	}

	return noOfItems;
}

const char *
T64Archive::getNameOfItem(int n)
{
	int i,j;
	int first = 0x50 + (n * 0x20);
	int last  = 0x60 + (n * 0x20);
	
	if (size < last) {
		name[0] = 0;
	} else {
		for (j = 0, i = first; i < last; i++, j++) {
			name[j] = (data[i] == 0x20 ? ' ' : data[i]);
			if (j == 255) break;
		}
		name[j] = 0x00;
	}
	return name;
}

const char *
T64Archive::getTypeOfItem(int n)
{
	int i = 0x41 + (n * 0x20);
	if (data[i] != 00)
		return "PRG";
	if (data[i] == 00 && data[i-1] > 0x00)
		return "FRZ";
	return "???";
}

uint16_t 
T64Archive::getDestinationAddrOfItem(int n)
{
	int i = 0x42 + (n * 0x20);
	uint16_t result = LO_HI(data[i], data[i+1]);
	return result;
}

void 
T64Archive::selectItem(int n)
{
	unsigned i;
    
    if (n < getNumberOfItems()) {

        // Compute start address in container
        i = 0x48 + (n * 0x20);
        if ((fp = LO_LO_HI_HI(data[i], data[i+1], data[i+2], data[i+3])) >= size)
            fprintf(stderr, "PANIC! fp is out of bounds!\n");

        // Compute start address in memory
        i = 0x42 + (n * 0x20);
        uint16_t startAddrInMemory = LO_HI(data[i], data[i+1]);
        
        // Compute end address in memory
        i = 0x44 + (n * 0x20);
        uint16_t endAddrInMemory = LO_HI(data[i], data[i+1]);

        if (endAddrInMemory == 0xC3C6) {
            fprintf(stderr, "WARNING: Corrupted archive. Mostly likely created with CONV64!\n");
            // WHAT DO WE DO ABOUT IT?
        }

        // Compute size of item
        uint16_t length = endAddrInMemory - startAddrInMemory;

        // fprintf(stderr, "start = %d end = %d diff = %d\n", startAddrInMemory, endAddrInMemory, length);
        // fprintf(stderr, "fp = %d fp_eof = %d\n", fp, fp_eof);

        // Store largest offset that belongs to the file
        if ((fp_eof = fp + length) > size)
            fprintf(stderr, "PANIC! fp_eof is out of bounds!\n");
        
        // Return if offset values are safe
        if (fp < size && fp_eof <= size)
            return; // success
    }

    fp = fp_eof = -1; // fail
	return;
}

int 
T64Archive::getByte()
{
	int result;
	
	if (fp < 0)
		return -1;
		
	// get byte
	result = data[fp++];
	
	// check for end of file
	if (fp == fp_eof || fp == size)
		fp = -1;

	// fprintf(stderr, "%02X ", result);
	return result;
}

#if 0
void
T64Archive::dumpDirectory()
{
    Archive::dumpDirectory();
    
    for (unsigned i = 0; i < getNumberOfItems(); i++) {
        fprintf(stderr, "  Item %2d:      %s (%d bytes, load address: %d)\n",
                i, getNameOfItem(i), getSizeOfItem(i), getDestinationAddrOfItem(i));
    }
}
#endif