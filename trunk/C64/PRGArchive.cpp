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
	cleanup();
}

PRGArchive::~PRGArchive()
{
	cleanup();
}

PRGArchive *PRGArchive::archiveFromFile(const char *filename)
{
	PRGArchive *archive;
	
	archive = new PRGArchive();	
	if (!archive->loadFile(filename)) {
		delete archive;
		archive = NULL;
	}
	return archive;
}

const char *PRGArchive::getTypeOfContainer() 
{
	return "PRG";
}

void PRGArchive::cleanup()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
}

bool PRGArchive::fileIsValid(const char *filename)
{
	assert(filename != NULL);

	if (!checkFileSuffix(filename, ".PRG") && !checkFileSuffix(filename, ".prg"))
		return false;

	if (!checkFileSize(filename, 2, -1))
		return false;
		
	return true;
}

bool PRGArchive::loadFromFile(FILE *file, struct stat fileProperties)
{
	int c = 0;
	
	// Allocate memory
	if ((data = (uint8_t *)malloc(fileProperties.st_size)) == NULL) {
		return false;
	}
		
	// Read data
	c = fgetc(file);
	while(c != EOF) {
		data[size++] = c;
		c = fgetc(file);
	}

	fprintf(stderr, "PRG Container imported successfully (%d bytes total, size = %d)\n", (int)fileProperties.st_size, size);
	return true;
}

int PRGArchive::getNumberOfItems()
{
	return 1;
}

const char *PRGArchive::getNameOfItem(int n)
{
	return "UNKNOWN";
}
	
int PRGArchive::getSizeOfItem(int n)
{
	if (size > 0)
		return size-2;
	else
		return 0;
}		

const char *PRGArchive::getTypeOfItem(int n)
{
	return "PRG";
}

uint16_t PRGArchive::getDestinationAddrOfItem(int n)
{
	uint16_t result = data[0] + (data[1] << 8);
	fprintf(stderr, "Will load to location %X\n", result);
	return result;
}

void PRGArchive::selectItem(int n)
{
	fp = 2; // skip load address

	if (fp >= size)
		fp = -1;
}

int PRGArchive::getByte()
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
