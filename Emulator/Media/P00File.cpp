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
#include "P00File.h"
#include "FileSystem.h"
#include "IOUtils.h"

namespace vc64 {

bool
P00File::isCompatible(const fs::path &path)
{
    auto s = util::uppercased(path.extension().string());
    return s == ".P00";
}

bool
P00File::isCompatible(std::istream &stream)
{
    const u8 magicBytes[] = { 0x43, 0x36, 0x34, 0x46, 0x69, 0x6C, 0x65 };

    if (util::streamLength(stream) < 0x1A) return false;
    return util::matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

void
P00File::init(FileSystem &fs)
{
    isize item = 0;
    isize itemSize = fs.fileSize(item);

    // Only proceed if the requested file exists
    if (fs.numFiles() <= item) throw Error(ERROR_FS_HAS_NO_FILES);

    // Create new archive
    isize p00Size = itemSize + 8 + 17 + 1;
    init(p00Size);

    // Write magic bytes (8 bytes)
    u8 *p = data;
    strcpy((char *)p, "C64File");
    p += 8;
    
    // Write name in PET format (16 bytes)
    fs.fileName(item).write(p);
    p += 16;
    
    // Always 0 (1 byte)
    *p++ = 0;

    // Record size (applies to REL files, only) (1 byte)
    *p++ = 0;

    // Add data
    fs.copyFile(item, p, itemSize);
}

PETName<16>
P00File::getName() const
{
    // P00 files use 0x00 as padding character
    auto result = PETName<16>(data + 8, 0x00);

    // Rectify the padding characters
    result.setPad(0xA0);

    return result;
}

PETName<16>
P00File::collectionName()
{
    return getName();
}

isize
P00File::collectionCount() const
{
    return 1;
}

PETName<16>
P00File::itemName(isize nr) const
{
    assert(nr == 0);

    return getName();
}

isize
P00File::itemSize(isize nr) const
{
    assert(nr == 0);
    return size - 0x1A;
}

u8
P00File::readByte(isize nr, isize pos) const
{
    assert(nr == 0);
    assert(pos < itemSize(nr));
    return data[0x1A + pos];
}

}
