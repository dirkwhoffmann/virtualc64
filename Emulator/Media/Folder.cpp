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
Folder::isCompatible(const string &path)
{
    return util::isDirectory(path);
}

void
Folder::init(const string &path)
{
    if (!isCompatiblePath(path)) throw VC64Error(ERROR_FILE_TYPE_MISMATCH);
    
    fs = new FileSystem(path);
}

PETName<16>
Folder::collectionName()
{
    return fs->getName();
}

isize
Folder::collectionCount() const
{
    return (isize)fs->numFiles();
}

PETName<16>
Folder::itemName(isize nr) const
{
    return fs->fileName(nr);
}

u64
Folder::itemSize(isize nr) const
{
    return fs->fileSize(nr);
}

u8
Folder::readByte(isize nr, u64 pos) const
{
    u8 result;
    fs->copyFile(nr, &result, 1, pos);
    return result;
}

void
Folder::copyItem(isize nr, u8 *buf, u64 len, u64 offset) const
{
    fs->copyFile(nr, buf, len, offset);
}
