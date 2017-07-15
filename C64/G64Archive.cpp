/*
 * (C) 2015 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, org
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

#include "G64Archive.h"

G64Archive::G64Archive()
{
    setDescription("G64Archive");
	data = NULL;
	dealloc();
}

G64Archive::~G64Archive()
{
	dealloc();
}

bool 
G64Archive::isG64File(const char *filename)
{
    /* "GCR-1541" + Version number (0x00) */
	int magic_bytes[] = { 0x47, 0x43, 0x52, 0x2D, 0x31, 0x35, 0x34, 0x31, 0x00, EOF };
	
	assert(filename != NULL);
	
	if (!checkFileSuffix(filename, ".G64") && !checkFileSuffix(filename, ".g64"))
		return false;
	
	if (!checkFileSize(filename, 0x02AC, -1))
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
	
	return true;
}

G64Archive *
G64Archive::archiveFromG64File(const char *filename)
{
	G64Archive *archive = new G64Archive();

	if (!archive->readFromFile(filename)) {
		delete archive;
        return NULL;
	}
	
    archive->debug(1, "G64 archive created from file %s.\n", filename);
	return archive;
}

void G64Archive::dealloc()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
    fp_eof = -1;
}

bool 
G64Archive::fileIsValid(const char *filename)
{
	return G64Archive::isG64File(filename);
}

bool 
G64Archive::readFromBuffer(const uint8_t *buffer, unsigned length)
{	
	if ((data = (uint8_t *)malloc(length)) == NULL)
		return false;

	memcpy(data, buffer, length);
	size = length;

	return true;
}

unsigned
G64Archive::writeToBuffer(uint8_t *buffer)
{
    assert(data != NULL);
    
    if (buffer) {
        memcpy(buffer, data, size);
    }
    return size;
}

const char *
G64Archive::getName()
{
    return "G64 archive";
}

int 
G64Archive::getNumberOfItems()
{
    return 84;
}

int
G64Archive::getStartOfItem(int n)
{
    if (n < 0 || n >= 84)
        return -1;
    
    int offset = 0x00C + (4 * n);
    return LO_LO_HI_HI(data[offset], data[offset+1], data[offset+2], data[offset+3]);
}

int
G64Archive::getSizeOfItem(int n)
{
    int offset = getStartOfItem(n);
    return offset ? (LO_HI(data[offset], data[offset+1])) : 0;
}

const char *
G64Archive::getNameOfItem(int n)
{
    if (n < 0 || n >= 84)
        return "";
    
    if (n % 2 == 0) {
        sprintf(name, "Track %d", (n / 2) + 1);
    } else {
        sprintf(name, "Track %d.5", (n / 2) + 1);
    }
    
	return name;
}

const unsigned short *
G64Archive::getUnicodeNameOfItem(int n, size_t maxChars)
{
    (void)getNameOfItem(n);
    translateToUnicode(name, unicode, 0xE000, maxChars);
    return unicode;
}

const char *
G64Archive::getTypeOfItem(int n)
{
    return ""; // (n % 2 == 0) ? "Full" : "Half";
}

void 
G64Archive::selectItem(int n)
{
    fp = getStartOfItem(n);
    fp += 2; // skip length information
    fp_eof = fp + getSizeOfItem(n);
}

int
G64Archive::getByte()
{
	int result;
	
	if (fp < 0)
		return -1;
		
	// get byte
	result = data[fp++];
	
	// check for end of file
	if (fp == fp_eof)
		fp = -1;

	return result;
}
