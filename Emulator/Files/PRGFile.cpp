// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "PRGFile.h"

bool
PRGFile::isPRGBuffer(const u8 *buffer, size_t length)
{
    return length >= 2;
}

bool
PRGFile::isPRGFile(const char *path)
{
    assert(path != NULL);
    
    if (!checkFileSuffix(path, ".PRG") && !checkFileSuffix(path, ".prg"))
        return false;
    
    if (!checkFileSize(path, 2, -1))
        return false;
    
    return true;
}

PRGFile::PRGFile()
{
    setDescription("PRGArchive");
}

PRGFile *
PRGFile::makeWithBuffer(const u8 *buffer, size_t length)
{
    PRGFile *archive = new PRGFile();
    
    if (!archive->readFromBuffer(buffer, length)) {
        
        delete archive;
        return NULL;
    }
    
    return archive;
}

PRGFile *
PRGFile::makeWithFile(const char *path)
{
    PRGFile *archive = new PRGFile();
    
    if (!archive->readFromFile(path)) {
        
        delete archive;
        return NULL;
    }
    
    return archive;
}

PRGFile *
PRGFile::makeWithAnyArchive(AnyArchive *other, int item)
{
    if (other == NULL || other->numberOfItems() <= item)
        return NULL;
    
    PRGFile *archive = new PRGFile();
    archive->debug(FILE_DEBUG, "Creating PRG archive from %s archive...\n",
                   other->typeAsString());
    
    other->selectItem(item);
    
    // Determine file size and allocate memory
    archive->size = 2 + other->getSizeOfItem();
    if ((archive->data = new u8[archive->size]) == NULL) {
        archive->warn("Failed to allocate %d bytes of memory\n", archive->size);
        delete archive;
        return NULL;
    }
    
    // Load address
    u8* ptr = archive->data;
    *ptr++ = LO_BYTE(other->getDestinationAddrOfItem());
    *ptr++ = HI_BYTE(other->getDestinationAddrOfItem());
    
    // File data
    int byte;
    other->selectItem(item);
    while ((byte = other->readItem()) != EOF) {
        *ptr++ = (u8)byte;
    }
    
    return archive;
}

void
PRGFile::selectItem(unsigned item)
{
    if (item == 0) {
        iFp = 2;
        iEof = size;
    } else {
        iFp = -1;
    }
}

void
PRGFile::seekItem(long offset)
{
    assert(iFp != -1);
    
    iFp = 2 + offset;
    
    if (iFp >= size)
        iFp = -1;
}

u16 
PRGFile::getDestinationAddrOfItem()
{
	return LO_HI(data[0], data[1]);
}
