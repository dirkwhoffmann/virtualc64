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
}

Container::~Container()
{
	if (path)
		free(path);
}

const char *
Container::getPath()
{
	return path ? path : "";
}

const char *
Container::getName()
{
 	return name ? name : "";
}

bool 
Container::readDataFromFile(FILE *file, struct stat fileProperties)
{
	return false;
}

bool 
Container::writeDataToFile(FILE *file, struct stat fileProperties)
{
	return false;
}

bool 
Container::readFromFile(const char *filename)
{
	struct stat fileProperties;
	FILE *file;
	
	assert (filename != NULL);
		
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
	
	// Free old data
	cleanup();

	// Load data
	if (!readDataFromFile(file, fileProperties)) {
		fclose(file);
		return false;
	}
	fclose(file);

	// Set path and default name
	if (path)
		free (path);
	path = strdup(filename);
	if (name)
		free(name);
	name = strdup(ChangeExtension(ExtractFilename(getPath()), "").c_str());
		   
	return true;
}

bool 
Container::writeToFile(const char *filename)
{
	struct stat fileProperties;
	FILE *file;
	
	assert (filename != NULL);
		
	// Open file
	if (!(file = fopen(filename, "w"))) {
		return false;
	}
		
	// Write data
	if (!writeDataToFile(file, fileProperties)) {
		fclose(file);
		return false;
	}
	fclose(file);
		
	return true;
}
