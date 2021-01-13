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
PRGFile::isCompatibleName(const std::string &name)
{
    auto s = suffix(name);
    return s == "prg" || s == "PRG";
}

bool
PRGFile::isCompatibleStream(std::istream &stream)
{
    return (streamLength(stream) >= 2);
}

PRGFile *
PRGFile::makeWithFileSystem(FSDevice &fs)
{
    unsigned item = 0;
    usize itemSize = fs.fileSize(item);

    debug(FILE_DEBUG, "Creating PRG archive...\n");

    // Only proceed if the requested file exists
    if (fs.numFiles() <= item) throw VC64Error(ERROR_FS_HAS_NO_FILES);
        
    // Create new archive
    PRGFile *prg = new PRGFile(itemSize);
                
    // Add data
    fs.copyFile(item, prg->data, itemSize);
    
    return prg;
}

PETName<16>
PRGFile::collectionName()
{
    return PETName<16>(getName());
}

u64
PRGFile::collectionCount() const
{
    return 1;
}

PETName<16>
PRGFile::itemName(unsigned nr) const
{
    assert(nr == 0);
    return PETName<16>(getName());
}

u64
PRGFile::itemSize(unsigned nr) const
{
    assert(nr == 0);
    return size;
}

u8
PRGFile::readByte(unsigned nr, u64 pos) const
{
    assert(nr == 0);
    assert(pos < itemSize(nr));
    return data[pos];
}
