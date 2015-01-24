/*
 * (C) 2010 Dirk W. Hoffmann. All rights reserved.
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

#include "Container.h"

Container::Container()
{
	path = NULL;
	name = NULL;
}

Container::~Container()
{
	if (path)
		free(path);

	if (name)
		free(name);
}

unsigned
Container::sizeOnDisk()
{
	return 0;
}

void
Container::setPath(const char *name)
{
    if (path)
        free(path);
    
    path = strdup(name);
}

bool 
Container::readFromBuffer(const void *buffer, unsigned length)
{
	return false;
}

bool 
Container::readFromFile(const char *filename)
{
	bool success = false;
	uint8_t *buffer = NULL;
	FILE *file = NULL;
	struct stat fileProperties;
	
	assert (filename != NULL);
			
	// Check file type
	if (!fileIsValid(filename)) {
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
	if (!readFromBuffer(buffer, fileProperties.st_size)) {
		goto exit;
	}

	// Set path and default name
	if (path)
		free (path);
	path = strdup(filename);
	if (name)
		free(name);
	name = strdup(ChangeExtension(ExtractFilename(getPath()), "").c_str());
	
	success = true;

exit:
	
	if (file)
		fclose(file);
	if (buffer)
		free(buffer);

	return success;
}

bool 
Container::writeToBuffer(void *buffer)
{
	return false;
}

bool 
Container::writeToFile(const char *filename)
{
	bool success = false;
	uint8_t *data = NULL;
	FILE *file;
	unsigned filesize = sizeOnDisk();
   
	assert (filename != NULL);
		
	// Open file
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
