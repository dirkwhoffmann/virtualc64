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
#include "PRGFile.h"
#include "FileSystems/OldFileSystem.h" // DEPRECATED
#include "filesystems/CBM/FileSystem.h"

namespace vc64 {

bool
PRGFile::isCompatible(const fs::path &path)
{
    auto s = utl::uppercased(path.extension().string());
    return s == ".PRG";
}

bool
PRGFile::isCompatible(const u8 *buf, isize len)
{
    return (len >= 2);
}

bool
PRGFile::isCompatible(const Buffer<u8> &buf)
{
    return isCompatible(buf.ptr, buf.size);
}

void
PRGFile::init(const OldFileSystem &fs)
{
    isize item = 0;
    isize itemSize = fs.fileSize(item);

    // Only proceed if the requested file exists
    if (fs.numFiles() <= item) throw AppError(Fault::FS_HAS_NO_FILES);

    // Create new archive
    init(itemSize);

    // Add data
    fs.copyFile(item, data.ptr, itemSize);
}

void
PRGFile::init(const FileSystem &fs)
{    
    // Read directory
    auto dir = fs.readDir();
    
    // Only proceed if the requested file exists
    if (dir.size() == 0) throw AppError(Fault::FS_HAS_NO_FILES);
    
    // Get the first item
    fs.extractData(dir[0], data);
}

PETName<16>
PRGFile::collectionName()
{
    return PETName<16>(getName());
}

isize
PRGFile::collectionCount() const
{
    return 1;
}

PETName<16>
PRGFile::itemName(isize nr) const
{
    assert(nr == 0);
    return PETName<16>(getName());
}

isize
PRGFile::itemSize(isize nr) const
{
    assert(nr == 0);
    return data.size;
}

u8
PRGFile::readByte(isize nr, isize pos) const
{
    assert(nr == 0);
    assert(pos < itemSize(nr));
    return data[pos];
}

}
