// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "P00File.h"
#include <new>

const u8
P00File::magicBytes[] = { 0x43, 0x36, 0x34, 0x46, 0x69, 0x6C, 0x65 };

bool
P00File::isP00Buffer(const u8 *buffer, size_t length)
{
    if (length < 0x1A) return false;
    return matchingBufferHeader(buffer, magicBytes, sizeof(magicBytes));
}

bool
P00File::isP00File(const char *filename)
{
    assert (filename != NULL);
    
    if (!checkFileSize(filename, 0x1A, -1))
        return false;
    
    if (!matchingFileHeader(filename, magicBytes, sizeof(magicBytes)))
        return false;
    
    return true;
}

P00File::P00File()
{
    setDescription("P00Archive");
}

P00File *
P00File::makeWithBuffer(const u8 *buffer, size_t length)
{
    P00File *archive = new P00File();
    
    if (!archive->readFromBuffer(buffer, length)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

P00File *
P00File::makeWithFile(const char *filename)
{
    P00File *archive = new P00File();
    
    if (!archive->readFromFile(filename)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

P00File *
P00File::makeWithAnyArchive(AnyArchive *otherArchive)
{
    if (otherArchive == NULL || otherArchive->numberOfItems() == 0)
        return NULL;
    
    otherArchive->selectItem(0);
    
    P00File *archive = new P00File();
    archive->debug(FILE_DEBUG, "Creating P00 archive from %s archive...\n", otherArchive->typeAsString());
    
    // Determine file size and allocate memory
    try {
        
        archive->size = 8 + 17 + 1 + 2 + otherArchive->getSizeOfItem();
        archive->data = new u8[archive->size];
    }
    catch (std::bad_alloc&) {
        
        archive->warn("Failed to allocate %d bytes of memory\n", archive->size);
        delete archive;
        return NULL;
    }
    
    // Magic bytes (8 bytes)
    u8 *ptr = archive->data;
    strcpy((char *)ptr, "C64File");
    ptr += 8;
    
    // Name in PET format (17 bytes)
    strncpy((char *)ptr, (char *)otherArchive->getName(), 17);
    for (unsigned i = 0; i < 17; i++, ptr++)
        *ptr = ascii2pet(*ptr);
    
    // Record size (applies to REL files, only) (1 byte)
    *ptr++ = 0;
    
    // Load address (2 bytes)
    *ptr++ = LO_BYTE(otherArchive->getDestinationAddrOfItem());
    *ptr++ = HI_BYTE(otherArchive->getDestinationAddrOfItem());
    
    // File data
    int byte;
    otherArchive->selectItem(0);
    while ((byte = otherArchive->readItem()) != EOF) {
        *ptr++ = (u8)byte;
    }
    
    return archive;
}

const char *
P00File::getName()
{
    unsigned i;
    
    for (i = 0; i < 17; i++) {
        name[i] = data[0x08+i];
    }
    name[i] = 0x00;
    return name;
}

void
P00File::selectItem(unsigned item)
{
    if (item == 0) {
        iFp = 0x1C;
        iEof = size;
    } else {
        iFp = -1;
    }
}

const char *
P00File::getNameOfItem()
{
    unsigned i;
    
    for (i = 0; i < 17; i++) {
        name[i] = data[0x08+i];
    }
    name[i] = 0x00;
    return name;
}

void
P00File::seekItem(long offset)
{
    assert(iFp != -1);
    
    iFp = 0x1C + offset;
    
    if (iFp >= size)
        iFp = -1;
}

u16 
P00File::getDestinationAddrOfItem()
{
    return LO_HI(data[0x1A], data[0x1B]);
}
