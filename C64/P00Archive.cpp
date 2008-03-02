#include "P00Archive.h"

P00Archive::P00Archive()
{
	path = NULL;
	data = NULL;
	size = 0; 
}

P00Archive::~P00Archive()
{
	if (path) free(path);
	if (data) free(data);
}

void P00Archive::eject()
{
	if (path) free(path);
	if (data) free(data);
	path = NULL;
	data = NULL;
	size = 0;
	fp = -1;
}

bool P00Archive::fileIsValid(const char *filename)
{
	int magic_bytes[] = {0x43, 0x36, 0x34, 0x46, 0x69, 0x6C, 0x65, 0x00, EOF};	
	
	assert (filename != NULL);

	if (!checkFileSize(filename, 0x1A, -1))
		return false;

	if (!checkFileHeader(filename, magic_bytes))
		return false;

	return true;
}

bool P00Archive::loadFile(const char *filename)
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
		return false;
	}
	
	// Open file
	if (!(file = fopen(filename, "r"))) {
		return false;
	}

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
	fclose(file);

	path = strdup(filename);
			
	debug("P00 container imported successfully (%d bytes total, size = %d)\n", fileProperties.st_size, size);
	return true;
}

char *P00Archive::getPath() 
{
	return path;
}

char *P00Archive::getName()
{
	return getNameOfItem(0);
}

int P00Archive::getNumberOfItems()
{
	return 1;
}

char *P00Archive::getNameOfItem(int n)
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
	
int P00Archive::getSizeOfItem(int n)
{
	if (size > 0)
		return size-0x1A;
	else
		return 0;
}		

char *P00Archive::getTypeOfItem(int n)
{
	return "PRG";
}

uint16_t P00Archive::getDestinationAddrOfItem(int n)
{
	uint16_t result = data[0x1A] + (data[0x1B] << 8);
	debug("Will load to location %X\n", result);
	return result;
}

void P00Archive::selectItem(int n)
{		
	fp = 0x1C; // skip header and load address

	if (fp >= size || n != 0)
		fp = -1;
}

int P00Archive::getByte()
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
