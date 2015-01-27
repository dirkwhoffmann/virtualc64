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

P00Archive::P00Archive()
{
	data = NULL;
	dealloc(); 
}

P00Archive::~P00Archive()
{
	dealloc();
}

bool 
P00Archive::isP00File(const char *filename)
{
	int magic_bytes[] = {0x43, 0x36, 0x34, 0x46, 0x69, 0x6C, 0x65, 0x00, EOF};	
	
	assert (filename != NULL);
	
	if (!checkFileSize(filename, 0x1A, -1))
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
	
	return true;
}

P00Archive *
P00Archive::archiveFromP00File(const char *filename)
{
	P00Archive *archive;

	fprintf(stderr, "Loading P00 archive from P00 file...\n");
	archive = new P00Archive();	
	if (!archive->readFromFile(filename)) {
        fprintf(stderr, "Failed to load archive\n");
        delete archive;
		archive = NULL;
	}
	
	return archive;
}

P00Archive *
P00Archive::archiveFromArchive(Archive *otherArchive)
{
    P00Archive *archive;
    
    if (otherArchive == NULL)
        return NULL;
    
    fprintf(stderr, "Creating P00 archive from %s archive...\n", otherArchive->getTypeAsString());
    
    if ((archive = new P00Archive()) == NULL) {
        fprintf(stderr, "Failed to create archive\n");
        return NULL;
    }
    
    fprintf(stderr, "IMPLEMENTATION MISSING\n");
    
    return archive;
}

void 
P00Archive::dealloc()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
}

bool 
P00Archive::fileIsValid(const char *filename)
{
	return isP00File(filename);
}

bool 
P00Archive::readFromBuffer(const uint8_t *buffer, unsigned length)
{
	if ((data = (uint8_t *)malloc(length)) == NULL)
		return false;
		
	memcpy(data, buffer, length);
	size = length;
	
	return true;
}
		
int 
P00Archive::getNumberOfItems()
{
	return 1;
}

const char *
P00Archive::getNameOfItem(int n)
{
	int i;
	
	if (n != 0)
		return NULL;
		
	for (i = 0; i < 17; i++) {
		name[i] = data[0x08+i];
	}
	name[i] = 0x00;
	return name;
}
	
int 
P00Archive::getSizeOfItem(int n)
{
	if (size > 0)
		return size-0x1A;
	else
		return 0;
}		

const char *
P00Archive::getTypeOfItem(int n)
{
	return "PRG";
}

uint16_t 
P00Archive::getDestinationAddrOfItem(int n)
{
	uint16_t result = data[0x1A] + (data[0x1B] << 8);
	printf("Will load to location %X\n", result);
	return result;
}

void 
P00Archive::selectItem(int n)
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
