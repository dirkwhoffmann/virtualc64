// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

bool
Folder::isFolder(const char *path)
{
    DIR *dir;
    
    // We accept all directories
    if ((dir = opendir(path)) == nullptr) return false;
    
    closedir(dir);
    return true;
}

Folder *
Folder::makeWithFolder(const std::string &path)
{
    Folder *folder = new Folder();
    
    ErrorCode err;
    if (!(folder->fs = FSDevice::makeWithFolder(path, &err))) {
        delete folder;
        // TODO: throw
        warn("Failed to create file system from folder %s\n", path.c_str());
        return nullptr;
    }
    return folder;
}

Folder *
Folder::makeWithFolder(const char *path)
{
    assert(path);
    return makeWithFolder(string(path));
}
    
PETName<16>
Folder::collectionName()
{
    return fs->getName();
}

u64
Folder::collectionCount()
{
    return fs->numFiles();
}

PETName<16>
Folder::itemName(unsigned nr)
{
    return fs->fileName(nr);
}

u64
Folder::itemSize(unsigned nr)
{
    return fs->fileSize(nr);
}

u8
Folder::readByte(unsigned nr, u64 pos)
{
    u8 result;
    fs->copyFile(nr, &result, 1, pos);
    return result;
}

void
Folder::copyItem(unsigned nr, u8 *buf, u64 len, u64 offset)
{
    fs->copyFile(nr, buf, len, offset);
}
