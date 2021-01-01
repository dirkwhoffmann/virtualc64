// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "AnyFile.h"
#include "Snapshot.h"
#include "RomFile.h"
#include "TAPFile.h"
#include "CRTFile.h"
#include "T64File.h"
#include "PRGFile.h"
#include "P00File.h"
#include "D64File.h"
#include "G64File.h"

template <class T> T *
AnyFile::make(const u8 *buffer, size_t length, FileError *error)
{
    T *obj = new T();
    
    if (!obj->readFromBuffer(buffer, length, error)) {
        delete obj;
        return nullptr;
    }
        
    return obj;
}

template <class T> T *
AnyFile::make(const char *path, FileError *error)
{
    T *obj = new T();
    
    if (!obj->readFromFile(path, error)) {
        delete obj;
        return nullptr;
    }

    return obj;
}

template <class T> T *
AnyFile::make(FILE *file, FileError *error)
{
    T *obj = new T();
    
    if (!obj->readFromFile(file, error)) {
        delete obj;
        return nullptr;
    }
    
    return obj;
}

AnyFile::AnyFile()
{
    const char *defaultName = "HELLO VIRTUALC64";
    memcpy(name, defaultName, strlen(defaultName) + 1);
    memset(name, 0, sizeof(name));
}

AnyFile::AnyFile(usize capacity)
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
    if (data == nullptr) {
        assert(size == 0);
        return;
    }
    
    delete[] data;
    data = nullptr;
    size = 0;
}

bool
AnyFile::alloc(usize capacity)
{
    dealloc();
    if ((data = new u8[capacity]()) == nullptr) return false;
    size = capacity;
    
    return true;
}

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
AnyFile::readFromBuffer(const u8 *buffer, size_t length, FileError *error)
{
    assert (buffer);
    
    // Check file type
    if (!matchingBuffer(buffer, length)) {
        if (error) *error = ERR_INVALID_TYPE;
        return false;
    }
    
    // Allocate memory
    if (!alloc(length)) {
        if (error) *error = ERR_OUT_OF_MEMORY;
        return false;
    }
    
    // Read from buffer
    memcpy(data, buffer, length);
    
    if (error) *error = ERR_FILE_OK;
    return true;
}

bool
AnyFile::readFromFile(const char *filename, FileError *error)
{
    assert (filename);
    
    bool success;
    FILE *file = nullptr;
    struct stat fileProperties;
    
    // Get properties
    if (stat(filename, &fileProperties) != 0) {
        if (error) *error = ERR_FILE_NOT_FOUND;
        return false;
    }

    // Check type
    if (!matchingFile(filename)) {
        if (error) *error = ERR_INVALID_TYPE;
        return false;
    }

    // Open
    if (!(file = fopen(filename, "r"))) {
        if (error) *error = ERR_CANT_READ;
        return false;
    }

    // Read
    setPath(filename);
    success = readFromFile(file, error);
    
    fclose(file);
    return success;
}

bool
AnyFile::readFromFile(FILE *file, FileError *error)
{
    assert (file);
    
    u8 *buffer = nullptr;

    // Get size
    fseek(file, 0, SEEK_END);
    size_t size = (size_t)ftell(file);
    rewind(file);
    
    // Allocate memory
    if (!(buffer = new u8[size])) {
        if (error) *error = ERR_OUT_OF_MEMORY;
        return false;
    }

    // Read from file
    int c;
    for (unsigned i = 0; i < size; i++) {
        if ((c = fgetc(file)) == EOF) break;
        buffer[i] = (u8)c;
    }
    
    // Read from buffer
    dealloc();
    if (!readFromBuffer(buffer, size, error)) {
        delete[] buffer;
        return false;
    }
    
    delete[] buffer;
    if (error) *error = ERR_FILE_OK;
    return true;
}

bool
AnyFile::oldReadFromBuffer(const u8 *buffer, size_t length)
{
    assert (buffer);
    
    dealloc();
    if ((data = new u8[length]) == nullptr)
        return false;
    
    memcpy(data, buffer, length);
    size = length;
    return true;
}

bool
AnyFile::oldReadFromFile(const char *filename)
{
    assert (filename != NULL);
    
    bool success = false;
	u8 *buffer = NULL;
	FILE *file = NULL;
	struct stat fileProperties;
	
	// Check file type
    if (!matchingFile(filename)) {
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
	if (!oldReadFromBuffer(buffer, (unsigned)fileProperties.st_size)) {
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


//
// Instantiate template functions
//

template Snapshot* AnyFile::make <Snapshot> (const u8 *, size_t, FileError *);
template RomFile* AnyFile::make <RomFile> (const u8 *, size_t, FileError *);
template TAPFile* AnyFile::make <TAPFile> (const u8 *, size_t, FileError *);
template CRTFile* AnyFile::make <CRTFile> (const u8 *, size_t, FileError *);
template T64File* AnyFile::make <T64File> (const u8 *, size_t, FileError *);
template PRGFile* AnyFile::make <PRGFile> (const u8 *, size_t, FileError *);
template P00File* AnyFile::make <P00File> (const u8 *, size_t, FileError *);
template D64File* AnyFile::make <D64File> (const u8 *, size_t, FileError *);
template G64File* AnyFile::make <G64File> (const u8 *, size_t, FileError *);

template Snapshot* AnyFile::make <Snapshot> (const char *, FileError *);
template RomFile* AnyFile::make <RomFile> (const char *, FileError *);
template TAPFile* AnyFile::make <TAPFile> (const char *, FileError *);
template CRTFile* AnyFile::make <CRTFile> (const char *, FileError *);
template T64File* AnyFile::make <T64File> (const char *, FileError *);
template PRGFile* AnyFile::make <PRGFile> (const char *, FileError *);
template P00File* AnyFile::make <P00File> (const char *, FileError *);
template D64File* AnyFile::make <D64File> (const char *, FileError *);
template G64File* AnyFile::make <G64File> (const char *, FileError *);
