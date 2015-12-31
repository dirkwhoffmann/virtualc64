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

#include "PRGArchive.h"

PRGArchive::PRGArchive()
{
    setDescription("PRGArchive");
	data = NULL;
	dealloc();
}

PRGArchive::~PRGArchive()
{
	dealloc();
}

bool 
PRGArchive::isPRGFile(const char *filename)
{
	assert(filename != NULL);
	
	if (!checkFileSuffix(filename, ".PRG") && !checkFileSuffix(filename, ".prg"))
		return false;
	
	if (!checkFileSize(filename, 2, -1))
		return false;
	
	return true;
}

PRGArchive *
PRGArchive::archiveFromPRGFile(const char *filename)
{
	PRGArchive *archive = new PRGArchive();
    
    if (!archive->readFromFile(filename)) {
        delete archive;
        return NULL;
	}
	
    archive->debug(1, "PRG archive created from file %s.\n", filename);
	return archive;
}

PRGArchive *
PRGArchive::archiveFromArchive(Archive *otherArchive)
{
    if (otherArchive == NULL || otherArchive->getNumberOfItems() == 0)
        return NULL;
    
    PRGArchive *archive = new PRGArchive();
    archive->debug(1, "Creating PRG archive from %s archive...\n", otherArchive->getTypeAsString());
    
    // Determine container size and allocate memory
    archive->size = 2 + otherArchive->getSizeOfItem(0);
    if ((archive->data = (uint8_t *)malloc(archive->size)) == NULL) {
        archive->warn("Failed to allocate %d bytes of memory\n", archive->size);
        delete archive;
        return NULL;
    }
    
    // Load address
    uint8_t* ptr = archive->data;
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

void 
PRGArchive::dealloc()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
}

bool 
PRGArchive::fileIsValid(const char *filename)
{
	return isPRGFile(filename);
}

bool 
PRGArchive::readFromBuffer(const uint8_t *buffer, unsigned length)
{	
	if ((data = (uint8_t *)malloc(length)) == NULL)
		return false;

	memcpy(data, buffer, length);
	size = length;
	    
	return true;
}

unsigned
PRGArchive::writeToBuffer(uint8_t *buffer)
{
    assert(data != NULL);

    if (buffer) {
        memcpy(buffer, data, size);
    }
    return size;
}

int
PRGArchive::getNumberOfItems()
{
	return 1;
}

const char *
PRGArchive::getNameOfItem(int n)
{
	return "UNKNOWN";
}
	
const char *
PRGArchive::getTypeOfItem(int n)
{
	return "PRG";
}

uint16_t 
PRGArchive::getDestinationAddrOfItem(int n)
{
	uint16_t result = LO_HI(data[0], data[1]);
	return result;
}

void 
PRGArchive::selectItem(int n)
{
	fp = 2; // skip load address

	if (fp >= size)
		fp = -1;
}

int 
PRGArchive::getByte()
{
	int result;
	
	if (fp < 0)
		return -1;
		
	// get byte
	result = data[fp++];
	
	// check for end of file
	if (fp == size)
		fp = -1;

	return result;
}
