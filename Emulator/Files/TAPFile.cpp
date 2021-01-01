// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "TAPFile.h"

const u8 TAPFile::magicBytes[] = {
    0x43, 0x36, 0x34, 0x2D, 0x54, 0x41, 0x50, 0x45, 0x2D, 0x52, 0x41, 0x57 };

TAPFile::TAPFile()
{
    dealloc();
}

TAPFile *
TAPFile::makeWithBuffer(const u8 *buffer, size_t length)
{
    TAPFile *tape = new TAPFile();
    
    if (!tape->oldReadFromBuffer(buffer, length)) {
        delete tape;
        return NULL;
    }
    
    return tape;
}

TAPFile *
TAPFile::makeWithFile(const char *filename)
{
    TAPFile *tape = new TAPFile();
    
    if (!tape->oldReadFromFile(filename)) {
        delete tape;
        return NULL;
    }
    
    return tape;
}

bool
TAPFile::isTAPBuffer(const u8 *buffer, size_t length)
{
    if (length < 0x15) return false;
    return matchingBufferHeader(buffer, magicBytes, sizeof(magicBytes));
    // return checkBufferHeader(buffer, length, magicBytes);
}

bool
TAPFile::isTAPFile(const char *filename)
{
    assert (filename != NULL);
    
    if (!checkFileSuffix(filename, ".TAP") && !checkFileSuffix(filename, ".tap") &&
        !checkFileSuffix(filename, ".T64") && !checkFileSuffix(filename, ".t64"))
        return false;
    
    if (!checkFileSize(filename, 0x15, -1))
        return false;
    
    if (!matchingFileHeader(filename, magicBytes, sizeof(magicBytes)))
        return false;
    
    return true;
}

void
TAPFile::dealloc()
{
    fp = -1;
}

const char *
TAPFile::getName()
{
    unsigned i;
    
    for (i = 0; i < 17; i++) {
        name[i] = data[0x08+i];
    }
    name[i] = 0x00;
    return name;
}

bool
TAPFile::matchingBuffer(const u8 *buf, size_t len)
{
    return isTAPBuffer(buf, len);
}

bool
TAPFile::matchingFile(const char *path)
{
    return isTAPFile(path);
}

bool
TAPFile::oldReadFromBuffer(const u8 *buffer, size_t length)
{
    if (!AnyFile::oldReadFromBuffer(buffer, length))
        return false;
    
    u32 l = LO_LO_HI_HI(data[0x10], data[0x11], data[0x12], data[0x13]);
    if (l + 0x14 /* Header */ != size) {
        warn("readFromBuffer: Expected %d bytes, found %lu\n", l, size - 0x14);
    }
        
    return true;
}

/*
const char *
TAPFile::description1()
{
    switch (TAPversion()) {
            
        case 0:  sprintf(str, "TAP type 0 (Original pulse layout)"); break;
        case 1:  sprintf(str, "TAP type 1 (Advanced pulse layout)"); break;
        default: sprintf(str, "TAP type %d (Unknown)", TAPversion());
    }
    
    return str;
}
*/
