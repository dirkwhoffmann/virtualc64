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

#pragma once

#include "Concurrency.h"
#include "SIDTypes.h"
#include "Volume.h"

namespace vc64 {

class AudioPort final : CoreObject, public util::RingBuffer < SamplePair, 12288 > {

    const char *objectName() const override { return "StereoStream"; }

    // Mutex for synchronizing read / write accesses
    util::ReentrantMutex mutex;

    // The data source
    class Muxer *muxer = nullptr;

    // Time stamp of the last write pointer alignment
    util::Time lastAlignment;

    // Master volumes (fadable)
    Volume volL;
    Volume volR;
    
    // Statistics
    AudioBufferStats stats = { };


    //
    // Inspecting
    //

public:

    AudioBufferStats getStats();


    //
    // Managing the data source
    //

public:

    // Assign a data source
    void connectMuxer(class Muxer *muxer);

    // Remove a data source
    void disconnectMuxer(class Muxer *muxer);

    // Check if the provided Muxer is the active data source
    bool isActive(class Muxer *muxer) const { return this->muxer == muxer; }


    //
    // Managing the ring buffer
    //

public:

    // Locks or unlocks the mutex
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }

    // Initializes the ring buffer with zeroes
    void wipeOut() { this->clear(SamplePair {0,0} ); }
    
    // Adds a sample to the ring buffer
    void add(float l, float r) { this->write(SamplePair {l,r} ); }

    // Puts the write pointer somewhat ahead of the read pointer
    void alignWritePtr();

    /* Handles a buffer underflow condition. A buffer underflow occurs when the
     * audio device of the host machine needs sound samples than SID hasn't
     * produced, yet.
     */
    void handleBufferUnderflow();

    /* Handles a buffer overflow condition. A buffer overflow occurs when SID
     * is producing more samples than the audio device of the host machine is
     * able to consume.
     */
    void handleBufferOverflow();


    //
    // Generating audio samples
    //

    // Generates samples from the audio source with a single active SID
    void mixSingleSID(isize numSamples);

    // Generates samples from the audio source with multiple active SIDs
    void mixMultiSID(isize numSamples);


    //
    // Reading audio samples
    //
    
    /* Copies n audio samples into a memory buffer. These functions mark the
     * final step in the audio pipeline. They are used to copy the generated
     * sound samples into the buffers of the native sound device.
     */
    void copyMono(float *buffer, isize n);
    void copyStereo(float *left, float *right, isize n);
    void copyInterleaved(float *buffer, isize n);
};

}
