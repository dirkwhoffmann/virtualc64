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

#include "config.h"
#include "SIDBridge.h"
#include "Emulator.h"

namespace vc64 {

void
AudioPort::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::State) {

    }
}

void
AudioPort::_reset(bool hard)
{
    lock();

    // Wipe out the buffer contents
    this->clear(SamplePair{0,0});

    // Realign the write pointer
    alignWritePtr();

    // Clear statistics
    if (hard) clearStats();

    unlock();
}

void
AudioPort::_run()
{
    unmute(10000);
}

void
AudioPort::_pause()
{
    fadeOut();
    mute(0);
}

void
AudioPort::_warpOn()
{
    fadeOut();
    mute(0);
}

void
AudioPort::_warpOff()
{
    unmute(10000);
}

void
AudioPort::_focus()
{
    unmute(100000);
}

void
AudioPort::_unfocus()
{
    mute(100000);
}

void
AudioPort::alignWritePtr()
{
    this->align(this->cap() / 2);
}

void
AudioPort::handleBufferUnderflow()
{
    // There are two common scenarios in which buffer underflows occur:
    //
    // (1) The consumer runs slightly faster than the producer.
    // (2) The producer is halted or not startet yet.

    trace(AUDBUF_DEBUG, "BUFFER UNDERFLOW (r: %ld w: %ld)\n", r, w);

    // Wipe out the buffer and reset the write pointer
    clear(SamplePair{0,0});
    alignWritePtr();

    // Determine the elapsed seconds since the last pointer adjustment
    auto elapsedTime = util::Time::now() - lastAlignment;
    lastAlignment = util::Time::now();

    // Check for condition (1)
    if (elapsedTime.asSeconds() > 10.0) {

        stats.bufferUnderflows++;
        warn("Last underflow: %f seconds ago\n", elapsedTime.asSeconds());
    }
}

void
AudioPort::handleBufferOverflow()
{
    // There are two common scenarios in which buffer overflows occur:
    //
    // (1) The consumer runs slightly slower than the producer
    // (2) The consumer is halted or not startet yet

    trace(AUDBUF_DEBUG, "BUFFER OVERFLOW (r: %ld w: %ld)\n", r, w);

    // Reset the write pointer
    alignWritePtr();

    // Determine the number of elapsed seconds since the last adjustment
    auto elapsedTime = util::Time::now() - lastAlignment;
    lastAlignment = util::Time::now();

    // Check for condition (1)
    if (elapsedTime.asSeconds() > 10.0) {

        stats.bufferOverflows++;
        warn("Last overflow: %f seconds ago\n", elapsedTime.asSeconds());
    }
}

void 
AudioPort::generateSamples()
{
    lock();

    // Check how many samples can be generated
    auto s0 = sidBridge.sidStream[0].count();
    auto s1 = sidBridge.sidStream[1].count();
    auto s2 = sidBridge.sidStream[2].count();
    auto s3 = sidBridge.sidStream[3].count();

    auto numSamples = s0;
    if (s1) numSamples = std::min(numSamples, s1);
    if (s2) numSamples = std::min(numSamples, s2);
    if (s3) numSamples = std::min(numSamples, s3);

    // Generate the samples
    if (sidBridge.isEnabled(1) || sidBridge.isEnabled(2) || sidBridge.isEnabled(3)) {
        mixMultiSID(numSamples);
    } else {
        mixSingleSID(numSamples);
    }

    unlock();
}

void
AudioPort::fadeOut()
{
    debug(AUDVOL_DEBUG, "Fading out (%ld samples)...\n", count());

    volL.set(0.0);
    volR.set(0.0);

    float scale = 1.0f;
    float delta = 1.0f / count();

    // Rescale the existing samples
    for (isize i = begin(); i != end(); i = next(i)) {

        scale -= delta;
        assert(scale >= -0.1 && scale < 1.0);

        elements[i].left *= scale;
        elements[i].right *= scale;
    }

    // Wipe out the rest of the buffer
    for (isize i = end(); i != begin(); i = next(i)) {

        elements[i] = SamplePair { 0, 0 };
    }
}

void
AudioPort::mixSingleSID(isize numSamples)
{
    auto vol0 = sidBridge.sid[0].vol;
    auto pan0 = sidBridge.sid[0].pan;

    debug(SID_EXEC, "vol0: %f pan0: %f volL: %f volR: %f\n",
          vol0, pan0, volL.current, volR.current);

    // Check for buffer overflow
    if (free() < numSamples) handleBufferOverflow();

    // Convert sound samples to floating point values and write into ringbuffer
    if (volL.isFading() || volR.isFading()) {

        printf("Fading %f %f\n", volL.current, volR.current);
        for (isize i = 0; i < numSamples; i++) {

            // Read SID sample from ring buffer
            float ch0 = (float)sidBridge.sidStream[0].read() * vol0;

            // Compute left and right channel output
            float l = ch0 * (1 - pan0);
            float r = ch0 * pan0;

            // Apply master volume
            volL.shift();
            volR.shift();
            l *= volL.current;
            r *= volR.current;

            // Apply ear protection
            assert(abs(l) < 1.0);
            assert(abs(r) < 1.0);

            write(SamplePair { l, r } );
        }

    } else {

        auto currentL = volL.current;
        auto currentR = volR.current;

        for (isize i = 0; i < numSamples; i++) {

            // Read SID sample from ring buffer
            float ch0 = (float)sidBridge.sidStream[0].read() * vol0;

            // Compute left and right channel output
            float l = ch0 * (1 - pan0);
            float r = ch0 * pan0;

            // Apply master volume
            l *= currentL;
            r *= currentR;

            // Apply ear protection
            assert(abs(l) < 1.0);
            assert(abs(r) < 1.0);

            write(SamplePair { l, r } );
        }
    }
}

void
AudioPort::mixMultiSID(isize numSamples)
{
    auto vol0 = sidBridge.sid[0].vol; auto pan0 = sidBridge.sid[0].pan;
    auto vol1 = sidBridge.sid[1].vol; auto pan1 = sidBridge.sid[1].pan;
    auto vol2 = sidBridge.sid[2].vol; auto pan2 = sidBridge.sid[2].pan;
    auto vol3 = sidBridge.sid[3].vol; auto pan3 = sidBridge.sid[3].pan;

    debug(SID_EXEC, "vol0: %f pan0: %f vol1: %f pan1: %f volL: %f volR: %f\n",
          vol0, pan0, vol1, pan1, volL.current, volR.current);

    // Check for buffer overflow
    if (free() < numSamples) handleBufferOverflow();

    // Convert sound samples to floating point values and write into ringbuffer
    for (isize i = 0; i < numSamples; i++) {

        float ch0, ch1, ch2, ch3, l, r;

        ch0 = (float)sidBridge.sidStream[0].read()  * vol0;
        ch1 = (float)sidBridge.sidStream[1].read(0) * vol1;
        ch2 = (float)sidBridge.sidStream[2].read(0) * vol2;
        ch3 = (float)sidBridge.sidStream[3].read(0) * vol3;

        // Compute left and right channel output
        l = ch0 * (1 - pan0) + ch1 * (1 - pan1) + ch2 * (1 - pan2) + ch3 * (1 - pan3);
        r = ch0 * pan0 + ch1 * pan1 + ch2 * pan2 + ch3 * pan3;

        // Apply master volume
        volL.shift();
        volR.shift();
        l *= volL.current;
        r *= volR.current;

        // Apply ear protection
        assert(abs(l) < 1.0);
        assert(abs(r) < 1.0);

        write(SamplePair { l, r } );
    }
}

void
AudioPort::copyMono(float *buffer, isize n)
{
    lock();

    if (!recorder.isRecording()) {

        auto cnt = count();

        // Check for buffer underflows
        if (cnt >= count()) {

            // We have enough samples. Copy over the requested number
            for (isize i = 0; i < n; i++) {

                SamplePair pair = read();
                *buffer++ = pair.left + pair.right;
            }

        } else {

            // Copy all we have and stepwise lower the volume to minimize cracks
            for (isize i = 0; i < cnt; i++) {

                SamplePair pair = read();
                *buffer++ = (pair.left + pair.right) * float(cnt - i) / float(cnt);
            }
            assert(isEmpty());

            // Fill the rest with zeroes
            for (isize i = cnt; i < n; i++) *buffer++ = 0;

            // Realign the ring buffer
            handleBufferUnderflow();
        }

    } else {

        // Fill with zeroes
        for (isize i = 0; i < n; i++) *buffer++ = 0;
    }

    unlock();
}

void
AudioPort::copyStereo(float *left, float *right, isize n)
{
    lock();

    if (!recorder.isRecording()) {

        auto cnt = count();

        // Check for buffer underflows
        if (cnt >= n) {

            // We have enough samples. Copy over the requested number
            for (isize i = 0; i < n; i++) {

                SamplePair pair = read();
                *left++ = pair.left;
                *right++ = pair.right;
            }

        } else {

            // Copy all we have and stepwise lower the volume to minimize cracks
            for (isize i = 0; i < cnt; i++) {

                SamplePair pair = read();
                *left++ = pair.left * float(cnt - i) / float(cnt);
                *right++ = pair.right * float(cnt - i) / float(cnt);
            }
            assert(isEmpty());

            // Fill the rest with zeroes
            for (isize i = cnt; i < n; i++) *left++ = *right++ = 0;

            // Realign the ring buffer
            handleBufferUnderflow();
        }

    } else {

        // Fill with zeroes
        for (isize i = 0; i < n; i++) { *left++ = *right++ = 0; }
    }

    unlock();
}

void
AudioPort::copyInterleaved(float *buffer, isize n)
{
    lock();

    if (!recorder.isRecording()) {

        auto cnt = count();

        // Check for buffer underflows
        if (cnt >= n) {

            // We have enough samples. Copy over the requested number
            for (isize i = 0; i < n; i++) {

                SamplePair pair = read();
                *buffer++ = pair.left;
                *buffer++ = pair.right;
            }

        } else {

            // Copy all we have and stepwise lower the volume to minimize cracks
            for (isize i = 0; i < cnt; i++) {

                SamplePair pair = read();
                *buffer++ = pair.left * float(cnt - i) / float(cnt);
                *buffer++ = pair.right * float(cnt - i) / float(cnt);
            }
            assert(isEmpty());

            // Fill the rest with zeroes
            for (isize i = cnt; i < n; i++) { *buffer++ = 0; *buffer++ = 0; }

            // Realign the ring buffer
            handleBufferUnderflow();
        }

    } else {

        // Fill with zeroes
        for (isize i = 0; i < n; i++) { *buffer++ = 0; *buffer++ = 0; }
    }

    unlock();
}

}
