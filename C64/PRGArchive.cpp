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

PRGArchive::PRGArchive()
{
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
	PRGArchive *archive;
	
	fprintf(stderr, "Loading PRG archive from PRG file...\n");
	archive = new PRGArchive();	
	if (!archive->readFromFile(filename)) {
        fprintf(stderr, "Failed to load archive\n");
        delete archive;
		archive = NULL;
	}
	
	return archive;
}

PRGArchive *
PRGArchive::archiveFromArchive(Archive *otherArchive)
{
    PRGArchive *archive;
    
    if (otherArchive == NULL)
        return NULL;
    
    fprintf(stderr, "Creating PRG archive from %s archive...\n", otherArchive->getTypeAsString());
    
    if ((archive = new PRGArchive()) == NULL) {
        fprintf(stderr, "Failed to create archive\n");
        return NULL;
    }
    
    if (otherArchive->getNumberOfItems() > 0) {
        unsigned numBytes = otherArchive->getSizeOfItem(0)+2;
        if ((archive->data = (uint8_t *)malloc(numBytes)) == NULL) {
            fprintf(stderr, "Failed to allocate %d bytes\n", numBytes);
            delete archive;
            return NULL;
        }
        
        // Copy start address
        uint8_t* ptr = archive->data;
        *ptr++ = LO_BYTE(otherArchive->getDestinationAddrOfItem(0));
        *ptr++ = HI_BYTE(otherArchive->getDestinationAddrOfItem(0));
        
        // Copy data
        int byte;
        otherArchive->selectItem(0);
        while ((byte = otherArchive->getByte()) != EOF) {
            *ptr++ = (uint8_t)byte;
        }
        archive->size = ptr - archive->data;
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
        fprintf(stderr, "Copied %d bytes\n", size);
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
	
int 
PRGArchive::getSizeOfItem(int n)
{
	if (size > 0)
		return size-2;
	else
		return 0;
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
