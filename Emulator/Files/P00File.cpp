// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "P00File.h"
#include "FSDevice.h"

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
    debug(FILE_DEBUG, "Creating P00 archive from %s archive...\n", otherArchive->typeString());
    
    // Determine file size and allocate memory
    try {
        
        archive->size = 8 + 17 + 1 + 2 + otherArchive->getSizeOfItem();
        archive->data = new u8[archive->size];
    }
    catch (std::bad_alloc&) {
        
        warn("Failed to allocate %zu bytes of memory\n", archive->size);
        delete archive;
        return NULL;
    }
    
    printf("old P00 exporter: getSizeOfItem = %zu\n", otherArchive->getSizeOfItem());
    
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
    printf("Load addr (old): %04x\n", otherArchive->getDestinationAddrOfItem());
    *ptr++ = LO_BYTE(otherArchive->getDestinationAddrOfItem());
    *ptr++ = HI_BYTE(otherArchive->getDestinationAddrOfItem());
    printf("%02X %02X\n", *(ptr-2), *(ptr-1));

    // File data
    int byte;
    otherArchive->selectItem(0);
    while ((byte = otherArchive->readItem()) != EOF) {
        *ptr++ = (u8)byte;
    }
    
    return archive;
}

/*
P00File *
P00File::makeWithAnyCollection(AnyCollection *collection)
{
    assert(collection);

    debug(FILE_DEBUG, "Creating P00 archive...\n");

    // Only proceed if at least one file is present
    if (collection->collectionCount() == 0) return nullptr;
        
    // Create new archive
    size_t itemSize = collection->itemSize(0);
    size_t capacity = itemSize + 8 + 17 + 1;
    P00File *p00 = new P00File(capacity);
        
    printf("new P00 exporter: itemSize = %zu\n", itemSize);

    // Write magic bytes (8 bytes)
    u8 *p = p00->data;
    strcpy((char *)p, "C64File");
    p += 8;
    
    // Write name in PET format (17 bytes)
    strncpy((char *)p, (char *)collection->collectionName().c_str(), 17);
    for (unsigned i = 0; i < 17; i++, p++)
    *p = ascii2pet(*p);
    
    // Record size (applies to REL files, only) (1 byte)
    *p++ = 0;
        
    // Write data bytes
    collection->copyItem(0, p, itemSize);
    
    return p00;
}
*/

P00File *
P00File::makeWithFileSystem(FSDevice *fs, int item)
{
    assert(fs);

    debug(FILE_DEBUG, "Creating P00 archive...\n");

    // Only proceed if the requested file exists
    if (fs->numFiles() <= (u64)item) return nullptr;
        
    // Create new archive
    size_t fileSize = fs->fileSize(item);
    size_t p00Size = fileSize + 8 + 17 + 1;
    P00File *p00 = new P00File(p00Size);
        
    debug(FILE_DEBUG, "File size = %zu\n", fileSize);
    
    // Write magic bytes (8 bytes)
    u8 *p = p00->getData();
    strcpy((char *)p, "C64File");
    p += 8;
    
    // Write name in PET format (17 bytes)
    strncpy((char *)p, fs->fileName(item).c_str(), 17);
    for (unsigned i = 0; i < 17; i++, p++)
    *p = ascii2pet(*p);
    
    // Record size (applies to REL files, only) (1 byte)
    *p++ = 0;
        
    // Add data
    fs->copyFile(item, p, fileSize);
        
    return p00;
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
    
    if (iFp >= (long)size)
        iFp = -1;
}

u16 
P00File::getDestinationAddrOfItem()
{
    return LO_HI(data[0x1A], data[0x1B]);
}

std::string
P00File::collectionName()
{
    std::string result = "";
    for (int i = 0; i < 17; i++) result += (char)data[0x08+i];
    return result;
}

u64
P00File::collectionCount()
{
    return 1;
}

PETName<16>
P00File::itemName(unsigned nr)
{
    assert(nr == 0);
    u8 padChar = 0x00;
    return PETName<16>(data + 0x08, padChar);
}

u64
P00File::itemSize(unsigned nr)
{
    assert(nr == 0);
    return size - 0x1A;
}

u8
P00File::readByte(unsigned nr, u64 pos)
{
    assert(nr == 0);
    assert(pos < itemSize(nr));
    return data[0x1A + pos];
}
