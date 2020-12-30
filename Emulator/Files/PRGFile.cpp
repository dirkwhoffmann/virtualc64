// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "PRGFile.h"
#include "FSDevice.h"

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
    debug(FILE_DEBUG, "Creating PRG archive from %s archive...\n", other->typeString());
    
    other->selectItem(item);
    
    // Determine file size and allocate memory
    archive->size = 2 + other->getSizeOfItem();
    if ((archive->data = new u8[archive->size]) == NULL) {
        warn("Failed to allocate %zu bytes of memory\n", archive->size);
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

/*
PRGFile *
PRGFile::makeWithAnyCollection(AnyCollection *collection, int item)
{
    assert(collection);

    debug(FILE_DEBUG, "Creating PRG archive...\n");

    // Only proceed if at least one file is present
    if (collection->collectionCount() <= (u64)item) return nullptr;
        
    // Create new archive
    size_t itemSize = collection->itemSize(item);
    PRGFile *prg = new PRGFile(itemSize);
                
    // Add data
    collection->copyItem(item, prg->data, itemSize);
    
    return prg;
}
*/

PRGFile *
PRGFile::makeWithFileSystem(FSDevice *fs, int item)
{
    assert(fs);

    debug(FILE_DEBUG, "Creating PRG archive...\n");

    // Only proceed if the requested file exists
    if (fs->numFiles() <= (u64)item) return nullptr;
        
    // Create new archive
    size_t itemSize = fs->fileSize(item);
    PRGFile *prg = new PRGFile(itemSize);
                
    // Add data
    fs->copyFile(item, prg->getData(), itemSize);
    
    return prg;
}

PETName<16>
PRGFile::collectionName()
{
    return PETName<16>(getName());
}

u64
PRGFile::collectionCount()
{
    return 1;
}

PETName<16>
PRGFile::itemName(unsigned nr)
{
    assert(nr == 0);
    return PETName<16>(getName());
}

u64
PRGFile::itemSize(unsigned nr)
{
    assert(nr == 0);
    return size;
}

u8
PRGFile::readByte(unsigned nr, u64 pos)
{
    assert(nr == 0);
    assert(pos < itemSize(nr));
    return data[pos];
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
    
    if (iFp >= (long)size)
        iFp = -1;
}

u16 
PRGFile::getDestinationAddrOfItem()
{
	return LO_HI(data[0], data[1]);
}
