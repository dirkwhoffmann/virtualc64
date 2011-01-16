/*
 * (C) 2009 Dirk W. Hoffmann. All rights reserved.
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

#include "C64.h"

Snapshot::Snapshot()
{
	cleanup();
}

Snapshot::~Snapshot()
{
}

Snapshot *
Snapshot::snapshotFromFile(const char *filename)
{
	Snapshot *snapshot;
	
	snapshot = new Snapshot();	
	if (!snapshot->readFromFile(filename)) {
		delete snapshot;
		snapshot = NULL;
	}
	return snapshot;
}

Snapshot *
Snapshot::snapshotFromBuffer(const void *buffer, unsigned size)
{
	Snapshot *snapshot;
	
	snapshot = new Snapshot();	
	if (!snapshot->readFromBuffer(buffer, size)) {
		delete snapshot;
		snapshot = NULL;
	}
	return snapshot;	
}

void 
Snapshot::cleanup()
{
	fileContents.magic[0] = 'V';
	fileContents.magic[1] = 'C';
	fileContents.magic[2] = '6';
	fileContents.magic[3] = '4';
	fileContents.major = 1;
	fileContents.minor = 0;
	memset(fileContents.data, 0, sizeof(fileContents.data));

	//size = 0;
	timestamp = (time_t)0;
}

const char *
Snapshot::getTypeOfContainer() 
{
	return "V64";
}

bool 
Snapshot::fileIsValid(const char *filename)
{
	int magic_bytes[] = { 'V', 'C', '6', '4', EOF };
	
	assert(filename != NULL);
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
	
	return true;
}

bool 
Snapshot::readDataFromFile(FILE *file, struct stat fileProperties)
{
	// Read binary snapshot data
	uint8_t *fc = (uint8_t *)&fileContents;
	for (unsigned i = 0; i < sizeof(fileContents); i++) {
		int c = fgetc(file);
		if (c == EOF)
			break;
		fc[i] = (uint8_t)c;
	}

	// Do we support this snapshot format?
	if (fileContents.major != 1 || fileContents.minor != 0) {
		fprintf(stderr, "Found unsupported snapshot format V%d.%d\n", fileContents.major, fileContents.minor);		
		return false;
	}
	
	fprintf(stderr, "Snapshot data imported from file\n");
	return true;
}

bool 
Snapshot::writeDataToFile(FILE *file, struct stat fileProperties)
{	
	// Write binary snapshot data
	uint8_t *fc = (uint8_t *)&fileContents;
	for (unsigned i = 0; i < sizeof(fileContents); i++) {
		fputc((int)fc[i], file);
	}
	return true;	
}

bool 
Snapshot::readFromBuffer(const void *buffer, unsigned size)
{	
	if (size > sizeof(fileContents))
		return false;
	
	memcpy((void *)&fileContents, buffer, size);
	return true;
}
