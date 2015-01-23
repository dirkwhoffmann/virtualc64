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
    fileContents.magic[0] = 'V';
    fileContents.magic[1] = 'C';
    fileContents.magic[2] = '6';
    fileContents.magic[3] = '4';
    fileContents.major = V_MAJOR;
    fileContents.minor = V_MINOR;
    fileContents.size = 0;
    memset(fileContents.data, 0, sizeof(fileContents.data));
    timestamp = (time_t)0;
    state = NULL;
}

Snapshot::~Snapshot()
{
    dealloc();
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
Snapshot::dealloc()
{
    if (state != NULL) {
        free(state);
        fileContents.size = 0;
    }
}

bool
Snapshot::alloc(C64 *c64)
{
    dealloc();
    
    // determine size of snapshot data
    // uint32_t size = c64->calculateSnapshotSize();
    uint32_t size = MAX_SNAPSHOT_SIZE;
    
    if ((state = (uint8_t *)malloc(size)) == NULL)
        return false;
    
    fileContents.size = size;
    return true;
}

Container::ContainerType
Snapshot::getType()
{
    return V64_CONTAINER;
}

const char *
Snapshot::getTypeAsString() 
{
	return "V64";
}

bool
Snapshot::isSnapshot(const char *filename, int *major, int *minor)
{
    int magic_bytes[] = { 'V', 'C', '6', '4', EOF };
    
    assert(filename != NULL);
    
    if (!checkFileHeader(filename, magic_bytes, major, minor))
        return false;
    
    return true;
}

bool 
Snapshot::fileIsValid(const char *filename)
{
	int magic_bytes[] = { 'V', 'C', '6', '4', V_MAJOR, V_MINOR, EOF };
	
	assert(filename != NULL);
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
	
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
Snapshot::readFromBuffer(const void *buffer, unsigned length)
{
    // NEW CODE:
    // 1. Read header
    // 2. Allocate memory for state
    // 3. Read state
    
	if (length > sizeof(fileContents)) {
		fprintf(stderr, "Snapshot image is too big %d\n", length);
		return false;
	}
	
	memcpy((void *)&fileContents, buffer, length);
	return true;
}

bool 
Snapshot::writeToBuffer(void *buffer)
{	
	assert(buffer != NULL);
	
	memcpy(buffer, (void *)&fileContents, sizeof(fileContents));
	return true;
}

unsigned
Snapshot::sizeOnDisk()
{	
	return sizeof(fileContents);
}
