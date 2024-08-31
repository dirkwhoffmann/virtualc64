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
#include "TAPFile.h"
#include "IOUtils.h"
#include "Macros.h"

namespace vc64 {

bool
TAPFile::isCompatible(const fs::path &path)
{
    auto s = util::uppercased(path.extension().string());
    return s == ".TAP" || s == ".T64";
}

bool
TAPFile::isCompatible(const u8 *buf, isize len)
{
    return util::matchingBufferHeader(buf, len, string("C64-TAPE-RAW"));
}

bool
TAPFile::isCompatible(const Buffer<u8> &buf)
{
    return isCompatible(buf.ptr, buf.size);
}

PETName<16>
TAPFile::getName() const
{
    return PETName<16>(data.ptr + 8);
}

isize
TAPFile::headerSize() const
{
    return 0x14;
}

isize
TAPFile::numPulses()
{
    isize result;

    seek(0);
    for (result = 0; read() != -1; result++) { }
    
    return result;
}

void
TAPFile::seek(isize nr)
{
    fp = headerSize();
    for (isize i = 0; i < nr; i++) read();
}

isize
TAPFile::read()
{
    isize result = -1;
    
    if (fp != -1) {

        if (data[fp]) {
            
            // TAP0 and TAP1 with a non-zero pulse byte
            result = 8 * data[fp];
            fp += 1;

        } else if (version() == 0) {

            // TAP0 with a zero pulse byte
            result = 8 * 256;
            fp += 1;
            
        } else {
            
            debug(TAP_DEBUG, "TAP1 with a zero pulse byte\n");

            // TAP1 with a zero pulse byte
            result = LO_LO_HI_HI(fp + 1 < data.size ? data[fp + 1] : 0,
                                 fp + 2 < data.size ? data[fp + 2] : 0,
                                 fp + 3 < data.size ? data[fp + 3] : 0, 0);
            fp += 4;
        }
        
        // Check for EOF
        if (fp >= data.size) fp = -1;
    }
    
    return result;
}

void
TAPFile::finalizeRead()
{
    isize length = LO_LO_HI_HI(data[0x10], data[0x11], data[0x12], data[0x13]);
    isize header = 0x14;
    
    if (length + header != data.size) {
        warn("TAP: Expected %lu bytes, found %lu\n", length + header, data.size);
    } else {
        debug(TAP_DEBUG, "TAP file has been scanned with no errros\n");
    }
}

}
