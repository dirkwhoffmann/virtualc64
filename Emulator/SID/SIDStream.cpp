// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "SIDBridge.h"

void
SIDStream::append(short *data, size_t count)
{    
    // Check for buffer overflow
    if (free() < count) {
         bridge.handleBufferOverflow();
    }
    
    // Convert sound samples to floating point values and write into ringbuffer
    for (unsigned i = 0; i < count; i++) {
        write(float(data[i]) * scale);
    }
}
