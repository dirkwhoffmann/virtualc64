// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "AnyFile.h"

AnyFile::AnyFile()
{
    const char *defaultName = "HELLO VIRTUALC64";
    memcpy(name, defaultName, strlen(defaultName) + 1);
    memset(name, 0, sizeof(name));
}

AnyFile::AnyFile(size_t capacity)
{
    size = capacity;
    data = new u8[capacity]();
}

AnyFile::~AnyFile()
{
    dealloc();

    if (path)
		free(path);
}

void
AnyFile::dealloc()
{
    if (data == NULL) {
        assert(size == 0);
        return;
    }
    
    delete[] data;
    data = NULL;
    size = 0;
    fp = -1;
    eof = -1;
}

/*
bool
AnyC64File::checkBufferHeader(const u8 *buffer, size_t length, const u8 *header)
{
    assert(buffer != NULL);
    assert(header != NULL);
    
    unsigned i;
    
    for (i = 0; i < length && header[i] != 0; i++) {
        if (header[i] != buffer[i])
            return false;
    }
 
    return header[i] == 0;
}
*/

void
AnyFile::setPath(const char *str)
{
    assert(str != NULL);
    
    // Set path
    if (path) free(path);
    path = strdup(str);
    
    // Set default name (path without suffix)
    memset(name, 0, sizeof(name));
    char *filename = extractFilenameWithoutSuffix(path);
    strncpy(name, filename, sizeof(name) - 1);
    free(filename);
    ascii2petStr(name);
}

u64
AnyFile::fnv()
{
    return data ? fnv_1a_64(data, size) : 0;    
}

void
AnyFile::flash(u8 *buffer, size_t offset)
{
    assert(buffer);
    memcpy(buffer + offset, data, size);
}

bool
AnyFile::readFromBuffer(const u8 *buffer, size_t length)
{
    assert (buffer != NULL);
    
    dealloc();
    if ((data = new u8[length]) == NULL)
        return false;
    
    memcpy(data, buffer, length);
    size = length;
    eof = length;
    fp = 0;
    return true;
}

bool
AnyFile::readFromFile(const char *filename)
{
    assert (filename != NULL);
    
    bool success = false;
	u8 *buffer = NULL;
	FILE *file = NULL;
	struct stat fileProperties;
	
	// Check file type
    if (!hasSameType(filename)) {
		goto exit;
	}
	
	// Get file properties
    if (stat(filename, &fileProperties) != 0) {
		goto exit;
	}
		
	// Open file
	if (!(file = fopen(filename, "r"))) {
		goto exit;
	}

	// Allocate memory
	if (!(buffer = new u8[fileProperties.st_size])) {
		goto exit;
	}
	
	// Read from file
	int c;
	for (unsigned i = 0; i < fileProperties.st_size; i++) {
		c = fgetc(file);
		if (c == EOF)
			break;
		buffer[i] = (u8)c;
	}
	
	// Read from buffer (subclass specific behaviour)
	dealloc();
	if (!readFromBuffer(buffer, (unsigned)fileProperties.st_size)) {
		goto exit;
	}

    setPath(filename);
    success = true;
    
    trace(FILE_DEBUG, "File %s read successfully\n", path);
	
exit:
	
    if (file)
		fclose(file);
	if (buffer)
		delete[] buffer;

	return success;
}

size_t
AnyFile::writeToBuffer(u8 *buffer)
{
    assert(data != NULL);
    
    if (buffer) {
        memcpy(buffer, data, size);
    }
    return size;
}

bool 
AnyFile::writeToFile(const char *filename)
{
	bool success = false;
	u8 *data = NULL;
	FILE *file;
	size_t filesize;
   
    // Determine file size
    filesize = writeToBuffer(NULL);
    if (filesize == 0)
        return false;
    
	// Open file
    assert (filename != NULL);
	if (!(file = fopen(filename, "w"))) {
		goto exit;
	}
		
	// Allocate memory
    if (!(data = new u8[filesize])) {
		goto exit;
	}
	
	// Write to buffer 
	if (!writeToBuffer(data)) {
		goto exit;
	}

	// Write to file
	for (unsigned i = 0; i < filesize; i++) {
		fputc(data[i], file);
	}	
	
	success = true;

exit:
		
	if (file)
        fclose(file);
	if (data)
        delete[] data;
		
	return success;
}
