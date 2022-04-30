// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "PRGFile.h"
#include "FileSystem.h"
#include "IOUtils.h"

bool
PRGFile::isCompatible(const string &path)
{
    auto s = util::extractSuffix(path);
    return s == "prg" || s == "PRG";
}

bool
PRGFile::isCompatible(std::istream &stream)
{
    return (util::streamLength(stream) >= 2);
}

void
PRGFile::init(FileSystem &fs)
{
    isize item = 0;
    isize itemSize = fs.fileSize(item);

    // Only proceed if the requested file exists
    if (fs.numFiles() <= item) throw VC64Error(ERROR_FS_HAS_NO_FILES);
        
    // Create new archive
    init(itemSize);
                
    // Add data
    fs.copyFile(item, data, itemSize);
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
    return size;
}

u8
PRGFile::readByte(isize nr, isize pos) const
{
    assert(nr == 0);
    assert(pos < itemSize(nr));
    return data[pos];
}
