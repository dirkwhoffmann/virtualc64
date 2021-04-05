// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Folder.h"
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
Folder::makeWithFolder(const std::string &path, ErrorCode *err)
{
    *err = ERROR_OK;
    
    try { return makeWithFolder(path); }
    catch (VC64Error &exception) { *err = exception.data; }
    return nullptr;
}

PETName<16>
Folder::collectionName()
{
    return fs->getName();
}

u64
Folder::collectionCount() const
{
    return fs->numFiles();
}

PETName<16>
Folder::itemName(unsigned nr) const
{
    return fs->fileName(nr);
}

u64
Folder::itemSize(unsigned nr) const
{
    return fs->fileSize(nr);
}

u8
Folder::readByte(unsigned nr, u64 pos) const
{
    u8 result;
    fs->copyFile(nr, &result, 1, pos);
    return result;
}

void
Folder::copyItem(unsigned nr, u8 *buf, u64 len, u64 offset) const
{
    fs->copyFile(nr, buf, len, offset);
}
