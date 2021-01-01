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

/*
PRGFile *
PRGFile::makeWithBuffer(const u8 *buffer, size_t length)
{
    PRGFile *archive = new PRGFile();
    
    if (!archive->oldReadFromBuffer(buffer, length)) {
        
        delete archive;
        return NULL;
    }
    
    return archive;
}

PRGFile *
PRGFile::makeWithFile(const char *path)
{
    PRGFile *archive = new PRGFile();
    
    if (!archive->oldReadFromFile(path)) {
        
        delete archive;
        return NULL;
    }
    
    return archive;
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

bool
PRGFile::matchingBuffer(const u8 *buf, size_t len)
{
    return isPRGBuffer(buf, len);
}

bool
PRGFile::matchingFile(const char *path)
{
    return isPRGFile(path);
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
