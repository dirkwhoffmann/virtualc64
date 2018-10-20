/*!
 * @file        P00File.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
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

#include "P00File.h"

const uint8_t
P00File::magicBytes[] = { 0x43, 0x36, 0x34, 0x46, 0x69, 0x6C, 0x65, 0x00 };

P00File::P00File()
{
    setDescription("P00Archive");
}

P00File *
P00File::makeP00ArchiveWithBuffer(const uint8_t *buffer, size_t length)
{
    P00File *archive = new P00File();
    
    if (!archive->readFromBuffer(buffer, length)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

P00File *
P00File::makeP00ArchiveWithFile(const char *filename)
{
    P00File *archive = new P00File();
    
    if (!archive->readFromFile(filename)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

P00File *
P00File::makeP00ArchiveWithAnyArchive(AnyArchive *otherArchive)
{
    if (otherArchive == NULL || otherArchive->getNumberOfItems() == 0)
        return NULL;
    
    P00File *archive = new P00File();
    archive->debug(1, "Creating P00 archive from %s archive...\n", otherArchive->typeAsString());
    
    // Determine container size and allocate memory
    archive->size = 8 + 17 + 1 + 2 + otherArchive->getSizeOfItem(0);
    if ((archive->data = new uint8_t[archive->size]) == NULL) {
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

bool
P00File::isP00(const uint8_t *buffer, size_t length)
{
    if (length < 0x1A) return false;
    return checkBufferHeader(buffer, length, magicBytes);
}

bool 
P00File::isP00File(const char *filename)
{
	assert (filename != NULL);
	
	if (!checkFileSize(filename, 0x1A, -1))
		return false;
	
	if (!checkFileHeader(filename, magicBytes))
		return false;
	
	return true;
}

const char *
P00File::getName()
{
    unsigned i;
    
    for (i = 0; i < 17; i++) {
        name[i] = data[0x08+i];
    }
    name[i] = 0x00;
    return name;
}

size_t
P00File::writeToBuffer(uint8_t *buffer)
{
    assert(data != NULL);
    
    if (buffer) {
        memcpy(buffer, data, size);
    }
    return size;
}

const char *
P00File::getNameOfItem(unsigned n)
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

const char *
P00File::getTypeOfItem(unsigned n)
{
	return "PRG";
}

uint16_t 
P00File::getDestinationAddrOfItem(unsigned n)
{
//	uint16_t result = data[0x1A] + (data[0x1B] << 8);
//	return result;
    return LO_HI(data[0x1A], data[0x1B]);
}

void 
P00File::selectItem(unsigned n)
{		
	fp = 0x1C; // skip header and load address

	if (fp >= size || n != 0)
		fp = -1;
}
