/*
 * (C) 2010 - 2018 Dirk W. Hoffmann. All rights reserved.
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

#include "File.h"

File::File()
{
    const char *defaultName = "HELLO VIRTUALC64";
    
	path = NULL;
    memcpy(name, defaultName, strlen(defaultName) + 1);
}

File::~File()
{
	if (path)
		free(path);
}

bool
File::checkBufferHeader(const uint8_t *buffer, size_t length, const uint8_t *header)
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

void
File::setPath(const char *str)
{
    if (path)
        free(path);
    
    path = strdup(str);
}

const char *
File::getName()
{
    return name;
}

const unsigned short *
File::getUnicodeName(size_t maxChars)
{
    translateToUnicode(name, unicode, 0xE000, maxChars);
    return unicode;
}

void
File::setName(const char *str)
{
    strncpy(name, str, sizeof(name));
    name[sizeof(name) - 1] = 0;
}

bool
File::readFromFile(const char *filename)
{
    assert (filename != NULL);
    
    bool success = false;
	uint8_t *buffer = NULL;
	FILE *file = NULL;
	struct stat fileProperties;
    char *name = NULL;
	
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
	if (!(buffer = (uint8_t *)malloc(fileProperties.st_size))) {
		goto exit;
	}
	
	// Read from file
	int c;
	for (unsigned i = 0; i < fileProperties.st_size; i++) {
		c = fgetc(file);
		if (c == EOF)
			break;
		buffer[i] = (uint8_t)c;
	}
	
	// Read from buffer (subclass specific behaviour)
	dealloc();
	if (!readFromBuffer(buffer, (unsigned)fileProperties.st_size)) {
		goto exit;
	}

	// Set path and default name
    setPath(filename);
    name = ExtractFilenameWithoutSuffix(filename);
    setName(name);
        
    debug(1, "File %s read successfully\n", path);
	success = true;

exit:
	
    if (name)
        free(name);
    if (file)
		fclose(file);
	if (buffer)
		free(buffer);

	return success;
}

size_t
File::writeToBuffer(uint8_t *buffer)
{
	return 0;
}

bool 
File::writeToFile(const char *filename)
{
	bool success = false;
	uint8_t *data = NULL;
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
    if (!(data = (uint8_t *)malloc(filesize))) {
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
        free(data);
		
	return success;
}
