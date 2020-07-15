// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "AnyC64File.h"

AnyC64File::AnyC64File()
{
    const char *defaultName = "HELLO VIRTUALC64";
    memcpy(name, defaultName, strlen(defaultName) + 1);
    
    memset(name, 0, sizeof(name));
    memset(unicode, 0, sizeof(unicode));
}

AnyC64File::~AnyC64File()
{
    dealloc();

    if (path)
		free(path);
}

void
AnyC64File::dealloc()
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
AnyC64File::setPath(const char *str)
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

const unsigned short *
AnyC64File::getUnicodeName()
{
    translateToUnicode(getName(), unicode, 0xE000, sizeof(unicode) / 2);
    return unicode;
}

void
AnyC64File::seek(long offset)
{
    fp = (offset < size) ? offset : -1;
}

int
AnyC64File::read()
{
    int result;
    
    assert(eof <= size);
    
    if (fp < 0)
        return -1;
    
    // Get byte
    result = data[fp++];
    
    // Check for end of file
    if (fp == eof)
        fp = -1;

    return result;
}

const char *
AnyC64File::readHex(size_t num)
{
    assert(sizeof(name) > 3 * num);
    
    for (unsigned i = 0; i < num; i++) {
        
        int byte = read();
        if (byte == EOF) break;
        sprintf(name + (3 * i), "%02X ", byte);
    }
    
    return name;
}

void
AnyC64File::flash(u8 *buffer, size_t offset)
{
    int byte;
    assert(buffer != NULL);
    
    seek(0);

    while ((byte = read()) != EOF) {
        if (offset <= 0xFFFF) {
            buffer[offset++] = (u8)byte;
        } else {
            break;
        }
    }
}

bool
AnyC64File::readFromBuffer(const u8 *buffer, size_t length)
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
AnyC64File::readFromFile(const char *filename)
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
    
    debug(FILE_DEBUG, "File %s read successfully\n", path);
	
exit:
	
    if (file)
		fclose(file);
	if (buffer)
		delete[] buffer;

	return success;
}

size_t
AnyC64File::writeToBuffer(u8 *buffer)
{
    assert(data != NULL);
    
    if (buffer) {
        memcpy(buffer, data, size);
    }
    return size;
}

bool 
AnyC64File::writeToFile(const char *filename)
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
