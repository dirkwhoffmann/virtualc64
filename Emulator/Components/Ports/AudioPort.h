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

#include "SIDTypes.h"
#include "Concurrency.h"
#include "Volume.h"

namespace vc64 {

class SIDBridge;

class AudioPort final : public SubComponent, public Dumpable, public util::RingBuffer <SamplePair, 12288> {

    Descriptions descriptions = {{

        .name           = "AudioPort",
        .shellName      = "AudioPort",
        .description    = "Audio Port"
    }};

    // Set to true to disconnect from Mac audio (used by the recorder)
    bool muted = false;

    // Mutex for synchronizing read / write accesses
    util::ReentrantMutex mutex;

    // The audio sample provider
    SIDBridge *dataSource = nullptr;

    // Time stamp of the last write pointer alignment
    util::Time lastAlignment;

    // Master volumes (fadable)
    Volume volL;
    Volume volR;


    //
    // Methods
    //

public:

    AudioPort(C64 &ref) : SubComponent(ref) { };
    const Descriptions &getDescriptions() const override { return descriptions; }
    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from CoreComponent
    //

public:

    AudioPort& operator= (const AudioPort& other) {

        CLONE(muted)
        CLONE(lastAlignment)
        CLONE(volL)
        CLONE(volR)

        return *this;
    }

    template <class T>
    void serialize(T& worker)
    {
        worker

        << muted
        << volL
        << volR;

    } SERIALIZERS(serialize);

    void _reset(bool hard) override;


    //
    // Managing the data source
    //

public:

    // Assigns a data source
    void connectDataSource(SIDBridge *bridge);

    // Removes a data source
    void disconnectDataSource();
    void disconnectDataSource(SIDBridge *bridge);

    // Delegation methods
    void reset(SIDBridge *bridge, bool hard);
    void run(SIDBridge *bridge);
    void pause(SIDBridge *bridge);
    void warpOn(SIDBridge *bridge);
    void warpOff(SIDBridge *bridge);


    //
    // Managing the ring buffer
    //

public:

    // Locks or unlocks the mutex
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }

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

public:

    // Generates samples
    void generateSamples(SIDBridge *bridge);

    // Rescale new sound samples such that their volume steadily increases
    void fadeIn();

    // Rescale the existing samples in the buffer to gradually fade out
    void fadeOut();

    // Disconnect from the Mac audio backend
    void mute() { muted = true; }

    // Connect to the Mac audio backend
    void unmute() { muted = false; }

private:

    // Generates samples from the audio source with a single active SID
    void mixSingleSID(isize numSamples);

    // Generates samples from the audio source with multiple active SIDs
    void mixMultiSID(isize numSamples);


    //
    // Reading audio samples
    //
    
public:
    
    /* Copies n audio samples into a memory buffer. These functions mark the
     * final step in the audio pipeline. They are used to copy the generated
     * sound samples into the buffers of the native sound device.
     */
    void copyMono(float *buffer, isize n);
    void copyStereo(float *left, float *right, isize n);
    void copyInterleaved(float *buffer, isize n);
};

}
