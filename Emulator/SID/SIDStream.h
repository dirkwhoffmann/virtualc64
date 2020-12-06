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

    // Scaling factor applied to all sound samples produced by reSID
    static constexpr float scale = 0.000005f;

    // Reference to the connected SID bridge
    class SIDBridge &bridge;

    // Mutex for synchronizing read / write accesses
    Mutex mutex;

    
    //
    // Initializing
    //
    
public:
        
    SIDStream(SIDBridge &bridgeref) : bridge(bridgeref) { }
    
    
    //
    // Synchronizing access
    //
    
    // Locks or unlocks the synchronization mutex
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }

    
    //
    // Adding sound samples
    //
    
    // Writes a certain amount of sound samples into the ring buffer
    void append(short *data, size_t count);
};

#endif
