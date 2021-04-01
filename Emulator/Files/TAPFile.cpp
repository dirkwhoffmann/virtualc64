// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "TAPFile.h"
#include "IO.h"

bool
TAPFile::isCompatibleName(const std::string &name)
{
    auto s = util::extractSuffix(name);
    return s == "tap" || s == "TAP" || s == "t64" || s == "T64";
}

bool
TAPFile::isCompatibleStream(std::istream &stream)
{
    const u8 magicBytes[] = {
        0x43, 0x36, 0x34, 0x2D, 0x54, 0x41, 0x50, 0x45, 0x2D, 0x52, 0x41, 0x57 };
    
    if (streamLength(stream) < 0x15) return false;
    return util::matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

PETName<16>
TAPFile::getName() const
{
    return PETName<16>(data + 8);
}

void
TAPFile::repair()
{
    usize length = LO_LO_HI_HI(data[0x10], data[0x11], data[0x12], data[0x13]);
    usize header = 0x14;
    
    if (length + header != size) {
        warn("TAP: Expected %lu bytes, found %lu\n", length + header, size);
    }
}
