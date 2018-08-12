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

#include "P00Archive.h"

const uint8_t
P00Archive::magicBytes[] = { 0x43, 0x36, 0x34, 0x46, 0x69, 0x6C, 0x65, 0x00 };

P00Archive::P00Archive()
{
    setDescription("P00Archive");
	data = NULL;
	dealloc(); 
}

P00Archive *
P00Archive::makeP00ArchiveWithBuffer(const uint8_t *buffer, size_t length)
{
    P00Archive *archive = new P00Archive();
    
    if (!archive->readFromBuffer(buffer, length)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

P00Archive *
P00Archive::makeP00ArchiveWithFile(const char *filename)
{
    P00Archive *archive = new P00Archive();
    
    if (!archive->readFromFile(filename)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

P00Archive *
P00Archive::makeP00ArchiveWithAnyArchive(Archive *otherArchive)
{
    if (otherArchive == NULL || otherArchive->getNumberOfItems() == 0)
        return NULL;
    
    P00Archive *archive = new P00Archive();
    archive->debug(1, "Creating P00 archive from %s archive...\n", otherArchive->typeAsString());
    
    // Determine container size and allocate memory
    archive->size = 8 + 17 + 1 + 2 + otherArchive->getSizeOfItem(0);
    if ((archive->data = (uint8_t *)malloc(archive->size)) == NULL) {
        archive->warn("Failed to allocate %d bytes of memory\n", archive->size);
        delete archive;
        return NULL;
    }
    
    // Magic bytes (8 bytes)
    uint8_t *ptr = archive->data;
    strcpy((char *)ptr, "C64File");
    ptr += 8;
    
    // Name in PET format (17 bytes)
    strncpy((char *)ptr, (char *)otherArchive->getName(), 17);
    for (unsigned i = 0; i < 17; i++, ptr++)
        *ptr = ascii2pet(*ptr);
    
    // Record size (applies to REL files, only) (1 byte)
    *ptr++ = 0;
    
    // Load address (2 bytes)
    *ptr++ = LO_BYTE(otherArchive->getDestinationAddrOfItem(0));
    *ptr++ = HI_BYTE(otherArchive->getDestinationAddrOfItem(0));
    
    // File data
    int byte;
    otherArchive->selectItem(0);
    while ((byte = otherArchive->getByte()) != EOF) {
        *ptr++ = (uint8_t)byte;
    }
    
    return archive;
}

P00Archive::~P00Archive()
{
	dealloc();
}

bool
P00Archive::isP00(const uint8_t *buffer, size_t length)
{
    if (length < 0x1A) return false;
    return checkBufferHeader(buffer, length, magicBytes);
}

bool 
P00Archive::isP00File(const char *filename)
{
	assert (filename != NULL);
	
	if (!checkFileSize(filename, 0x1A, -1))
		return false;
	
	if (!checkFileHeader(filename, magicBytes))
		return false;
	
	return true;
}

void 
P00Archive::dealloc()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
}

const char *
P00Archive::getName()
{
    unsigned i;
    
    for (i = 0; i < 17; i++) {
        name[i] = data[0x08+i];
    }
    name[i] = 0x00;
    return name;
}

bool
P00Archive::hasSameType(const char *filename)
{
	return isP00File(filename);
}

bool 
P00Archive::readFromBuffer(const uint8_t *buffer, size_t length)
{
	if ((data = (uint8_t *)malloc(length)) == NULL)
		return false;
		
	memcpy(data, buffer, length);
	size = length;
	
	return true;
}

size_t
P00Archive::writeToBuffer(uint8_t *buffer)
{
    assert(data != NULL);
    
    if (buffer) {
        memcpy(buffer, data, size);
    }
    return size;
}

int 
P00Archive::getNumberOfItems()
{
	return 1;
}

const char *
P00Archive::getNameOfItem(unsigned n)
{
    assert(n < getNumberOfItems());
    
    unsigned i;
    
    if (n != 0)
        return NULL;
    
    for (i = 0; i < 17; i++) {
        name[i] = data[0x08+i];
    }
    name[i] = 0x00;
    return name;
}

const unsigned short *
P00Archive::getUnicodeNameOfItem(unsigned n)
{
    (void)getNameOfItem(n);
    translateToUnicode(name, unicode, 0xE000, sizeof(unicode) / 2);
    return unicode;
}

const char *
P00Archive::getTypeOfItem(unsigned n)
{
	return "PRG";
}

uint16_t 
P00Archive::getDestinationAddrOfItem(unsigned n)
{
//	uint16_t result = data[0x1A] + (data[0x1B] << 8);
//	return result;
    return LO_HI(data[0x1A], data[0x1B]);
}

void 
P00Archive::selectItem(unsigned n)
{		
	fp = 0x1C; // skip header and load address

	if (fp >= size || n != 0)
		fp = -1;
}

int 
P00Archive::getByte()
{
	int result;
	
	if (fp < 0)
		return -1;
		
	// get byte
	result = data[fp];
	
	// check for end of file
	if (fp == (size-1)) {
		fp = -1;
	} else {
		// advance file pointer
		fp++;
	}

	return result;
}
