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

typedef struct { float left; float right; } SamplePair;
class StereoStream : public RingBuffer < SamplePair, 12288 > {

public:
    
    // Scaling factor applied to all sound samples produced by reSID
    static constexpr float scale = 0.000005f;

private:
    
    // Reference to the connected SID bridge
    class SIDBridge &bridge;

    // Mutex for synchronizing read / write accesses
    Mutex mutex;

    
    //
    // Initializing
    //
    
public:
        
    StereoStream(SIDBridge &bridgeref) : bridge(bridgeref) { }
    
    
    //
    // Synchronizing access
    //
    
    // Locks or unlocks the synchronization mutex
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }

    
    //
    // Copying data
    //
    
    /* Copies n audio samples into a memory buffer. These functions mark the
     * final step in the audio pipeline. They are used to copy the generated
     * sound samples into the buffers of the native sound device. In additon
     * to copying, the volume is modulated and audio filters can be applied.
     */
    void copyMono(float *buffer, size_t n,
                  i32 &volume, i32 targetVolume, i32 volumeDelta);
    
    void copy(float *left, float *right, size_t n,
                    i32 &volume, i32 targetVolume, i32 volumeDelta);
    
    void copyInterleaved(float *buffer, size_t n,
                         i32 &volume, i32 targetVolume, i32 volumeDelta);
    
};

#endif
