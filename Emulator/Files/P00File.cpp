// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "P00File.h"
#include "FSDevice.h"
#include "IO.h"

bool
P00File::isCompatibleName(const std::string &name)
{
    auto s = util::extractSuffix(name);
    return s == "p00" || s == "P00";
}

bool
P00File::isCompatibleStream(std::istream &stream)
{
    const u8 magicBytes[] = { 0x43, 0x36, 0x34, 0x46, 0x69, 0x6C, 0x65 };

    if (util::streamLength(stream) < 0x1A) return false;
    return util::matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

P00File *
P00File::makeWithFileSystem(FSDevice &fs)
{
    unsigned item = 0;
    usize itemSize = fs.fileSize(item);

    // Only proceed if the requested file exists
    if (fs.numFiles() <= item) throw VC64Error(ERROR_FS_HAS_NO_FILES);
        
    // Create new archive
    usize p00Size = itemSize + 8 + 17 + 1;
    P00File *p00 = new P00File(p00Size);
            
    // Write magic bytes (8 bytes)
    u8 *p = p00->data;
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
        
    return p00;
}
    
PETName<16>
P00File::getName() const
{
    return PETName<16>(data + 8, 0x00);
}

PETName<16>
P00File::collectionName()
{
    return PETName<16>(data + 8, 0x00);
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
    u8 padChar = 0x00;
    return PETName<16>(data + 0x08, padChar);
}

u64
P00File::itemSize(isize nr) const
{
    assert(nr == 0);
    return size - 0x1A;
}

u8
P00File::readByte(isize nr, u64 pos) const
{
    assert(nr == 0);
    assert(pos < itemSize(nr));
    return data[0x1A + pos];
}
