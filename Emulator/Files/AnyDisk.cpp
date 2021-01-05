// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "D64File.h"
#include "G64File.h"

int
AnyDisk::readHalftrack()
{
    int result;
    
    assert(tEof <= (long)size);
    
    if (tFp < 0)
        return -1;
    
    // Get byte
    result = data[tFp++];
    
    // Check for end of file
    if (tFp == tEof)
        tFp = -1;
    
    return result;
}

/*
const char *
AnyDisk::readHalftrackHex(usize num)
{
    assert(sizeof(name) > 3 * num);
    
    for (unsigned i = 0; i < num; i++) {
        
        int byte = readHalftrack();
        if (byte == EOF) break;
        sprintf(name + (3 * i), "%02X ", byte);
    }
    
    return name;
}
*/

void
AnyDisk::copyHalftrack(u8 *buffer, usize offset)
{
    int byte;
    
    assert(buffer != NULL);
    
    seekHalftrack(0);
    
    while ((byte = readHalftrack()) != EOF) {
        buffer[offset++] = (u8)byte;
    }
}
