#include "T64Archive.h"

T64Archive::T64Archive()
{
	path = NULL;
	data = NULL;
	size = 0; 
}

T64Archive::~T64Archive()
{
	if (path) free(path);
	if (data) free(data);
}

void T64Archive::eject()
{
	if (path) free(path);
	if (data) free(data);
	path = NULL;
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

bool T64Archive::loadFile(const char *filename)
{
	struct stat fileProperties;
	FILE *file;
	int c = 0;
	
	assert (filename != NULL);

	// Free old data
	eject();
	
	// Check file type
	if (!fileIsValid(filename)) 
		return false;
	
	// Get file properties
    if (stat(filename, &fileProperties) != 0) {
		// Could not open file...
		return false;
	}

	// Check file size, archive must at least contain a valid header
	if (fileProperties.st_size < 0x40) {
		// too small
		return false;
	}
	
	// Open file
	if (!(file = fopen(filename, "r"))) {
		// Can't open for read (Huh?)
		return false;
	}

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
	fclose(file);

	path = strdup(filename);
			
	printf("T64 container imported successfully (%d bytes total, size = %d)\n", (int)fileProperties.st_size, size);
	return true;
}

char *T64Archive::getPath() 
{
	return path;
}

char *T64Archive::getName()
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

char *T64Archive::getNameOfItem(int n)
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

char *T64Archive::getTypeOfItem(int n)
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
