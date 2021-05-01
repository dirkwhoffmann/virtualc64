// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
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
    
    if (util::streamLength(stream) < 0x15) return false;
    return util::matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

PETName<16>
TAPFile::getName() const
{
    return PETName<16>(data + 8);
}

isize
TAPFile::headerSize() const
{
    /* According to the specs, the first pulse byte is supposed to be found at
     * position 0x14. However, some TAP files that are found in the wild don't
     * seem to comply to this scheme. Hence, we take a slightly approach. The
     * function starts scanning at position 0x14 and returns the position of
     * the first non-zero byte.
     */
    /*
    for (isize i = 0x14; i < (isize)size; i++) {
        if (data[i]) return i;
    }
    */
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
    printf("seek(%zd): fp = %zd\n", nr, fp);
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
            
            printf("TAP1 with a zero pulse byte\n");
            // TAP1 with a zero pulse byte
            result = LO_LO_HI_HI(fp + 1 < (isize)size ? data[fp + 1] : 0,
                                 fp + 2 < (isize)size ? data[fp + 2] : 0,
                                 fp + 3 < (isize)size ? data[fp + 3] : 0, 0);
            fp += 4;
        }
        
        // Check for EOF
        if (fp >= (isize)size) fp = -1;
    }
    
    return result;
}

void
TAPFile::repair()
{
    usize length = LO_LO_HI_HI(data[0x10], data[0x11], data[0x12], data[0x13]);
    usize header = 0x14;
    
    if (length + header != size) {
        warn("TAP: Expected %lu bytes, found %lu\n", length + header, size);
    } else {
        debug(TAP_DEBUG, "TAP file has been scanned with no errros\n");
    }
}
