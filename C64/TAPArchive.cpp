/*
 * (C) 2015 Dirk W. Hoffmann. All rights reserved.
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

TAPArchive::TAPArchive()
{
    data = NULL;
    dealloc();
}

TAPArchive::~TAPArchive()
{
    dealloc();
}

bool
TAPArchive::isTAPFile(const char *filename)
{
    int magic_bytes[] = {0x43, 0x36, 0x34, 0x2D, 0x54, 0x41, 0x50, 0x45, 0x2D, 0x52, 0x41, 0x57, EOF};
    
    assert (filename != NULL);
    
    if (!checkFileSuffix(filename, ".TAP") && !checkFileSuffix(filename, ".tap"))
        return false;

    if (!checkFileSize(filename, 0x15, -1))
        return false;
    
    if (!checkFileHeader(filename, magic_bytes))
        return false;
    
    return true;
}

TAPArchive *
TAPArchive::archiveFromTAPFile(const char *filename)
{
    TAPArchive *archive;
    
    fprintf(stderr, "Loading TAP archive from TAP file...\n");
    archive = new TAPArchive();
    if (!archive->readFromFile(filename)) {
        fprintf(stderr, "Failed to load archive\n");
        delete archive;
        archive = NULL;
    }
    
    return archive;
}

void
TAPArchive::dealloc()
{
    if (data) free(data);
    data = NULL;
    size = 0;
    fp = -1;
}

const char *
TAPArchive::getName()
{
    unsigned i;
    
    for (i = 0; i < 17; i++) {
        name[i] = pet2ascii(data[0x08+i]);
    }
    name[i] = 0x00;
    return name;
}

bool
TAPArchive::fileIsValid(const char *filename)
{
    return isTAPFile(filename);
}

bool
TAPArchive::readFromBuffer(const uint8_t *buffer, unsigned length)
{
    if ((data = (uint8_t *)malloc(length)) == NULL)
        return false;
    
    memcpy(data, buffer, length);
    size = length;
    
    for (unsigned i = 0; i < 10; i++) {
        for (unsigned j = 0; j < 16; j++) {
            fprintf(stderr, "%02X ", data[i*16+j]);
        }
        fprintf(stderr, "\n");
    }

    int l = LO_LO_HI_HI(data[0x10], data[0x11], data[0x12], data[0x13]);
    if (l + 0x14 /* Header */ != size) {
        fprintf(stderr, "Size mismatch! Archive should have %d data bytes, found %d\n", l, size - 0x14);
    }
        
    return true;
}

unsigned
TAPArchive::writeToBuffer(uint8_t *buffer)
{
    assert(data != NULL);
    
    if (buffer) {
        memcpy(buffer, data, size);
    }
    return size;
}

#if 0
void
TAPArchive::selectItem(int n)
{
    if (n == 0)
        fp = 0x0014; // Rewind
    else
        fp = -1;     // No such item
}

int
TAPArchive::getByte()
{
    int result;
    
    if (fp < 0)
        return -1;
    
    // get byte
    result = data[fp];
    
    // check for end of file
    if (fp == (size - 1)) {
        fp = -1;
    } else {
        // advance file pointer
        fp++;
    }
    
    return result;
}

#endif