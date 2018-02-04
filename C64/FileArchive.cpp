/*
 * (C) 2011 Dirk W. Hoffmann. All rights reserved.
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

#include "FileArchive.h"

FileArchive::FileArchive()
{
    setDescription("FileArchive");
	data = NULL;
	dealloc(); 
}

FileArchive::~FileArchive()
{
	dealloc();
}

bool 
FileArchive::isAcceptableFile(const char *filename)
{
	assert (filename != NULL);

	// we accept every file that fits into a standard C64 disk (approx. 170 kB)

	// TODO: Determine, how much data really fits on a disk.
	if (!checkFileSize(filename, 0x1A, 120000))
		return false;
	
	return true;
}

FileArchive *
FileArchive::archiveFromRawFiledata(const char *filename)
{
	FileArchive *archive = new FileArchive();
    
	if (!archive->readFromFile(filename)) {
        delete archive;
        return NULL;
	}
	
    archive->debug(1, "File archive created from file %s.\n", filename);
	return archive;
}

void 
FileArchive::dealloc()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
}

bool 
FileArchive::hasSameType(const char *filename)
{
	return isAcceptableFile(filename);
}

bool 
FileArchive::readFromBuffer(const uint8_t *buffer, size_t length)
{
	if ((data = (uint8_t *)malloc(length)) == NULL)
		return false;
		
	memcpy(data, buffer, length);
	size = length;
	
	return true;
}
		
int 
FileArchive::getNumberOfItems()
{
	return 1;
}

const char *
FileArchive::getNameOfItem(int n)
{
	strncpy(name, getName(), 17);
	for (unsigned i = 0; i < 17; i++) {
		name[i] = toupper(name[i]);
	}

	return name;
}

const unsigned short *
FileArchive::getUnicodeNameOfItem(int n, size_t maxChars)
{
    (void)getNameOfItem(n);
    translateToUnicode(name, unicode, 0xE000, maxChars);
    return unicode;
}

size_t
FileArchive::getSizeOfItem(int n)
{
	return (uint16_t)size - 2;
}		

const char *
FileArchive::getTypeOfItem(int n)
{
	return "PRG";
}

uint16_t 
FileArchive::getDestinationAddrOfItem(int n)
{
	uint16_t result = data[0x00] + (data[0x01] << 8);
	printf("Will load to location %X\n", result);
	return result;
}

void 
FileArchive::selectItem(int n)
{		
	fp = 0x02;

	if (fp >= size || n != 0)
		fp = -1;
}

int 
FileArchive::getByte()
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
