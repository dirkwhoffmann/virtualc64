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

#include "CoreObject.h"
#include "utl/abilities/Synchronizable.h"
#include "utl/concurrency.h"
#include "utl/storage/RingBuffer.h"

namespace vc64 {

/* About the AudioStream
 *
 * The audio stream is the last element in the audio pipeline. It is a temporary
 * storage for the final audio samples, waiting to be handed over to the audio
 * unit of the host machine.
 */

struct SamplePair
{
    // Audio sample of the left stereo channel
    float l;

    // Audio sample of the right stereo channel
    float r;
};


//
// AudioStream
//

class AudioStream : public CoreObject, public utl::Synchronizable, public utl::ResizableRingBuffer<SamplePair> {

public:

    const char *objectName() const override { return "AudioStream"; }

    // Initializes the ring buffer with zeroes
    void wipeOut();

    // Rescales the existing samples to gradually fade out
    void eliminateCracks();

    // Puts the write pointer somewhat ahead of the read pointer
    void alignWritePtr();


    //
    // Initializing
    //
    AudioStream(isize capacity) : utl::ResizableRingBuffer<SamplePair>(capacity) { }


    //
    // Copying data
    //

    /* Copies n audio samples into a memory buffer. These functions mark the
     * final step in the audio pipeline. They are used to copy the generated
     * sound samples into the buffers of the native sound device. In additon
     * to copying, the volume is modulated if the music is supposed to fade
     * in or fade out.
     */
    isize copyMono(float *buffer, isize n);
    isize copyStereo(float *left, float *right, isize n);
    isize copyInterleaved(float *buffer, isize n);


    //
    // Visualizing the waveform
    //

    /* Plots a graphical representation of the waveform. Returns the highest
     * amplitute that was found in the ringbuffer. To implement auto-scaling,
     * pass the returned value as parameter highestAmplitude in the next call
     * to this function.
     */
    void drawL(u32 *buffer, isize width, isize height, u32 color) const;
    void drawR(u32 *buffer, isize width, isize height, u32 color) const;

private:

    float drawL(u32 *buffer, isize width, isize height, float highest, u32 color) const;
    float drawR(u32 *buffer, isize width, isize height, float highest, u32 color) const;

    float draw(u32 *buffer, isize width, isize height, float scale,
               std::function<float(float x)>data, u32 color) const;

};

}
