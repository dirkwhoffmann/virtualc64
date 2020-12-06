// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _SID_STREAM_H
#define _SID_STREAM_H

#include "Buffers.h"
#include "Concurrency.h"

class SIDStream : public RingBuffer < float, 12288 > {

    // Mutex for synchronizing read / write accesses
    Mutex mutex;

public:
    
    // Locks or unlocks the synchronization mutex
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }

};

#endif
