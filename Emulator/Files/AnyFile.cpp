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
AnyFile::make(const u8 *buffer, size_t length)
{
    T *obj = new T();
    
    try { obj->readFromBuffer(buffer, length); } catch (Error &err) {
        delete obj;
        printf("Prior to throw\n");
        throw err;
    }
    
    return obj;
}

template <class T> T *
AnyFile::make(const char *path)
{
    T *obj = new T();
    
    try { obj->readFromFile(path); } catch (Error &err) {
        delete obj;
        printf("Prior to throw\n");
        throw err;
    }

    return obj;
}

template <class T> T *
AnyFile::make(FILE *file)
{
    T *obj = new T();
    
    try { obj->readFromFile(file); } catch (Error &err) {
        delete obj;
        throw err;
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
    printf("Destructor %p\n", this);

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

void
AnyFile::readFromFile(const char *filename)
{
    assert (filename);
        
    // Get properties
    struct stat fileProperties;
    if (stat(filename, &fileProperties) != 0) {
        throw Error(ERROR_FILE_NOT_FOUND);
        return;
    }

    // Check type
    if (!matchingFile(filename)) {
        throw Error(ERROR_INVALID_TYPE);
    }

    // Open
    FILE *file = nullptr;
    if (!(file = fopen(filename, "r"))) {
        throw Error(ERROR_CANT_READ);
    }

    // Read
    try { readFromFile(file); }
    catch (Error &err) { fclose(file); throw err; }
    fclose(file);
    
    setPath(filename);
}

void
AnyFile::readFromFile(FILE *file)
{
    assert (file);
    
    u8 *buffer = nullptr;

    // Get size
    fseek(file, 0, SEEK_END);
    size_t size = (size_t)ftell(file);
    rewind(file);
    
    // Allocate memory
    if (!(buffer = new u8[size])) {
        throw Error(ERROR_OUT_OF_MEMORY);
    }

    // Read from file
    int c;
    for (unsigned i = 0; i < size; i++) {
        if ((c = fgetc(file)) == EOF) break;
        buffer[i] = (u8)c;
    }
    
    // Read from buffer
    try { readFromBuffer(buffer, size); }
    catch (Error &err) { delete[] buffer; throw err; }
    
    delete[] buffer;
}

void
AnyFile::readFromBuffer(const u8 *buf, size_t len)
{
    assert(buf);
        
    // Check file type
    if (!matchingBuffer(buf, len)) {
        throw(Error(ERROR_INVALID_TYPE));
    }
    
    // Allocate memory
    if (!alloc(len)) {
        throw(Error(ERROR_OUT_OF_MEMORY));
    }
    
    // Read from buffer
    memcpy(data, buf, len);
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

template Snapshot* AnyFile::make <Snapshot> (const u8 *, size_t);
template RomFile* AnyFile::make <RomFile> (const u8 *, size_t);
template TAPFile* AnyFile::make <TAPFile> (const u8 *, size_t);
template CRTFile* AnyFile::make <CRTFile> (const u8 *, size_t);
template T64File* AnyFile::make <T64File> (const u8 *, size_t);
template PRGFile* AnyFile::make <PRGFile> (const u8 *, size_t);
template P00File* AnyFile::make <P00File> (const u8 *, size_t);
template D64File* AnyFile::make <D64File> (const u8 *, size_t);
template G64File* AnyFile::make <G64File> (const u8 *, size_t);

template Snapshot* AnyFile::make <Snapshot> (const char *);
template RomFile* AnyFile::make <RomFile> (const char *);
template TAPFile* AnyFile::make <TAPFile> (const char *);
template CRTFile* AnyFile::make <CRTFile> (const char *);
template T64File* AnyFile::make <T64File> (const char *);
template PRGFile* AnyFile::make <PRGFile> (const char *);
template P00File* AnyFile::make <P00File> (const char *);
template D64File* AnyFile::make <D64File> (const char *);
template G64File* AnyFile::make <G64File> (const char *);
