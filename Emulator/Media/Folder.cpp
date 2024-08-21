// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "Folder.h"
#include "C64.h"

namespace vc64 {

bool
Folder::isCompatible(const fs::path &path)
{
    return util::isDirectory(path);
}

void
Folder::init(const fs::path &path)
{
    if (!isCompatiblePath(path)) throw Error(VC64ERROR_FILE_TYPE_MISMATCH);
    
    fs = new FileSystem(path);

    // REMOVE ASAP
    fs->scanDirectory();
    fs->printDirectory();
    printf("NumFiles: %ld\n", fs->numFiles());
}

PETName<16>
Folder::collectionName()
{
    return fs->getName();
}

isize
Folder::collectionCount() const
{
    printf("collectionCount: %ld\n", fs->numFiles());
    return (isize)fs->numFiles();
}

PETName<16>
Folder::itemName(isize nr) const
{
    return fs->fileName(nr);
}

isize
Folder::itemSize(isize nr) const
{
    return fs->fileSize(nr);
}

u8
Folder::readByte(isize nr, isize pos) const
{
    u8 result;
    fs->copyFile(nr, &result, 1, pos);
    return result;
}

void
Folder::copyItem(isize nr, u8 *buf, isize len, isize offset) const
{
    fs->copyFile(nr, buf, len, offset);
}

}
