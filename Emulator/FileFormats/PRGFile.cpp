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
PRGFile::isPRGFile(const char *filename)
{
    assert(filename != NULL);
    
    if (!checkFileSuffix(filename, ".PRG") && !checkFileSuffix(filename, ".prg"))
        return false;
    
    if (!checkFileSize(filename, 2, -1))
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
PRGFile::makeWithFile(const char *filename)
{
    PRGFile *archive = new PRGFile();
    
    if (!archive->readFromFile(filename)) {
        
        delete archive;
        return NULL;
    }
    
    return archive;
}

PRGFile *
PRGFile::makeWithAnyArchive(AnyArchive *otherArchive) {
    
    int exportItem = 0;
    
    if (otherArchive == NULL || otherArchive->numberOfItems() <= exportItem)
        return NULL;
    
    PRGFile *archive = new PRGFile();
    archive->debug(1, "Creating PRG archive from %s archive...\n",
                   otherArchive->typeAsString());
    
    otherArchive->selectItem(exportItem);
    
    // Determine file size and allocate memory
    archive->size = 2 + otherArchive->getSizeOfItem();
    if ((archive->data = new u8[archive->size]) == NULL) {
        archive->warn("Failed to allocate %d bytes of memory\n", archive->size);
        delete archive;
        return NULL;
    }
    
    // Load address
    u8* ptr = archive->data;
    *ptr++ = LO_BYTE(otherArchive->getDestinationAddrOfItem());
    *ptr++ = HI_BYTE(otherArchive->getDestinationAddrOfItem());
    
    // File data
    int byte;
    otherArchive->selectItem(exportItem);
    while ((byte = otherArchive->readItem()) != EOF) {
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


