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

const char *
Snapshot::getTypeOfContainer() 
{
	return "V64";
}

void 
Snapshot::cleanup()
{
	major = 0;
	minor = 0;
	size = 0;
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
	int i, c;
	
	// Skip header
	for (i = 0; i < 4; i++)
		(void)fgetc(file);
	
	// Read version number
	major = (uint8_t)fgetc(file);
	minor = (uint8_t)fgetc(file);
	
	// Do we support this snapshot format?
	if (major != 1 || minor != 0) {
		fclose(file);
		return false;	
	}
	
	// Read binary snapshot data
	for (i = 0; i < MAX_SNAPSHOT_SIZE; i++) {
		if ((c = fgetc(file)) == EOF)
			break;
		data[i] = (uint8_t)c;
	}
	size = i;

	return true;
}

bool 
Snapshot::writeDataToFile(FILE *file, struct stat fileProperties)
{	
	// Write magic bytes
	fputc((int)'V', file);
	fputc((int)'C', file);
	fputc((int)'6', file);
	fputc((int)'4', file);
	
	// Write version number
	fputc(1, file);
	fputc(0, file);
		
	// Write binary snapshot data
	for (int i = 0; i < size; i++) {
		fputc((int)data[i], file);
	}

	return true;	
}

bool 
Snapshot::initWithContentsOfC64(C64 *c64)
{
	uint8_t *ptr = data;
	major = 1;
	minor = 0;
	memcpy(screen, c64->vic->screenBuffer(), sizeof(screen));
	c64->save(&ptr);
	size = ptr - data;
	
	fprintf(stderr, "initWithContentsOfC64: Packed state into %d bytes\n", size);
	return true;
}
	
bool 
Snapshot::writeToC64(C64 *c64)
{
	uint8_t *ptr = data;
	c64->load(&ptr);
	fprintf(stderr, "writeToC64: Extracted state from %d bytes\n", ptr - data);
	return true;
}
