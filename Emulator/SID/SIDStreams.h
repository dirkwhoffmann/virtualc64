// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Buffers.h"
#include "Concurrency.h"

typedef RingBuffer<short, 2048> SampleStream;

typedef struct { float left; float right; } SamplePair;

class StereoStream : public RingBuffer < SamplePair, 12288 > {
    
    // Mutex for synchronizing read / write accesses
    Mutex mutex;

    
    //
    // Initializing
    //
    
public:
        
    // StereoStream() { }
    
    
    //
    // Synchronizing access
    //

public:

    // Locks or unlocks the synchronization mutex
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }

    
    //
    // Copying data
    //
    
    /* Copies n audio samples into a memory buffer. These functions mark the
     * final step in the audio pipeline. They are used to copy the generated
     * sound samples into the buffers of the native sound device.
     */
    void copyMono(float *buffer, size_t n, Volume &volL, Volume &volR);
    void copyStereo(float *left, float *right, size_t n, Volume &volL, Volume &volR);
    void copyInterleaved(float *buffer, size_t n, Volume &volL, Volume &volR);
};
