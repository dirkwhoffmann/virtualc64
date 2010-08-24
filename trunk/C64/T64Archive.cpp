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
	printf("T64Archive::constructor\n");
	data = NULL;
	cleanup();
}

T64Archive::~T64Archive()
{
	cleanup();
}

T64Archive *T64Archive::archiveFromFile(const char *filename)
{
	T64Archive *archive;
	
	archive = new T64Archive();	
	if (!archive->loadFile(filename)) {
		delete archive;
		archive = NULL;
	}
	return archive;
}

const char *T64Archive::getTypeOfContainer() 
{
	return "T64";
}

void T64Archive::cleanup()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
	fp_eof = -1;
}

bool T64Archive::fileIsValid(const char *filename)
{
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

bool T64Archive::loadFromFile(FILE *file, struct stat fileProperties)
{
	int c;
	
	// Allocate memory
	if ((data = (uint8_t *)malloc(fileProperties.st_size)) == NULL) {
		// Didn't get enough memory
		return false;
	}
		
	// Read data
	c = fgetc(file);
	while(c != EOF) {
		data[size++] = c;
		c = fgetc(file);
	}
	
	fprintf(stderr, "T64 Container imported successfully (%d bytes total, size = %d)\n", (int)fileProperties.st_size, size);
	return true;
}

const char *T64Archive::getName()
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

bool T64Archive::directoryItemIsPresent(int n)
{
	int first = 0x40 + (n * 0x20);
	int last  = 0x60 + (n * 0x20);
	int i;
	
	// check for zeros...
	if (size >= last)
		for (i = first; i < last; i++)
			if (data[i] != 0)
				return true;

	return false;
}

int T64Archive::getNumberOfItems()
{
	int noOfItems;

	// Get number of files from the file header...
	noOfItems = ((int)data[0x25] << 8) + data[0x24];

	if (noOfItems == 0) {
		// Note: Some archives don't store this value properly.
		// In this case, we can determine the number of files
		// by iterating through the directory area...
		while (directoryItemIsPresent(noOfItems))
			noOfItems++;
	}
	
	return noOfItems;
}

const char *T64Archive::getNameOfItem(int n)
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
	
int T64Archive::getSizeOfItem(int n)
{
	int i = 0x42 + (n * 0x20);
	uint16_t startAddrInMemory = data[i] + (data[i+1] << 8);

	int j = 0x44 + (n * 0x20);
	uint16_t endAddrInMemory = data[j] + (data[j+1] << 8);

	if (endAddrInMemory == 0xC3C6) {
		printf("WARNING: Corrupted archive. Mostly likely created with CONV64!\n");
		// WHAT DO WE DO ABOUT IT?
	}
	return (endAddrInMemory - startAddrInMemory) + 1;
}

const char *T64Archive::getTypeOfItem(int n)
{
	int i = 0x41 + (n * 0x20);
	if (data[i] != 00)
		return "PRG";
	if (data[i] == 00 && data[i-1] > 0x00)
		return "FRZ";
	return "???";
}

uint16_t T64Archive::getDestinationAddrOfItem(int n)
{
	int i = 0x42 + (n * 0x20);
	uint16_t result = data[i] + (data[i+1] << 8);
	printf("Will load to location %X\n", result);
	return result;
}

void T64Archive::selectItem(int n)
{
	int i;
	
	// compute start address in container
	i = 0x48 + (n * 0x20);
	fp = data[i] + (data[i+1] << 8) + (data[i+2] << 16) + (data[i+2] << 24);
	fp_eof = fp + getSizeOfItem(n); // largest offset, that belongs to the file

	if (fp >= size)
		fp = fp_eof = -1;
		
	return;
}

int T64Archive::getByte()
{
	int result;
	
	if (fp < 0)
		return -1;
		
	// get byte
	result = data[fp];
	
	// check for end of file
	if (fp == fp_eof || fp == (size-1)) {
		fp = -1;
	} else {
		// advance file pointer
		fp++;
	}

	return result;
}
