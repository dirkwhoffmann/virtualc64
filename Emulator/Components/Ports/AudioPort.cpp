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

        /*
        // Increase the sample rate based on what we've measured
        sampleRateCorrection += count() / elapsedTime.asSeconds();

        stats.bufferUnderflows++;
        warn("Last underflow: %f seconds ago\n", elapsedTime.asSeconds());
        warn("New sample rate correction: %f\n", sampleRateCorrection);
        */
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

    // Adjust the sample rate, if condition (1) holds
    if (elapsedTime.asSeconds() > 10.0) {

        /*
        // Decrease the sample rate based on what we've measured
        sampleRateCorrection -= count() / elapsedTime.asSeconds();

        stats.bufferOverflows++;
        warn("Last overflow: %f seconds ago\n", elapsedTime.asSeconds());
        warn("New sample rate correction: %f\n", sampleRateCorrection);
        */
    }
}

void 
AudioPort::clamp(isize maxSamples)
{
    SYNCHRONIZED

    while (count() > maxSamples) read();
}

void
AudioPort::generateSamples()
{
    SYNCHRONIZED

    // Check how many samples can be generated
    auto s0 = sid0.stream.count();
    auto s1 = sid1.stream.count();
    auto s2 = sid2.stream.count();
    auto s3 = sid3.stream.count();

    auto numSamples = s0;
    if (s1) numSamples = std::min(numSamples, s1);
    if (s2) numSamples = std::min(numSamples, s2);
    if (s3) numSamples = std::min(numSamples, s3);

    // Generate the samples
    bool fading = volL.isFading() || volR.isFading();

    if (sid1.isEnabled() || sid2.isEnabled() || sid3.isEnabled()) {
        fading ? mixMultiSID<true>(numSamples) : mixMultiSID<false>(numSamples);
    } else {
        fading ? mixSingleSID<true>(numSamples) : mixSingleSID<false>(numSamples);
    }
}

void
AudioPort::fadeOut()
{
    SYNCHRONIZED

    debug(AUDVOL_DEBUG, "Fading out (%ld samples)...\n", count());

    volL.set(0.0);
    volR.set(0.0);

    float scale = 1.0f;
    float delta = 1.0f / count();

    // Rescale the existing samples
    for (isize i = begin(); i != end(); i = next(i)) {

        scale -= delta;
        assert(scale >= -0.1 && scale < 1.0);

        elements[i].l *= scale;
        elements[i].r *= scale;
    }

    // Wipe out the rest of the buffer
    for (isize i = end(); i != begin(); i = next(i)) {

        elements[i] = SamplePair { 0, 0 };
    }
}

template <bool fading> void
AudioPort::mixSingleSID(isize numSamples)
{
    auto vol0 = vol[0];
    auto pan0 = pan[0];
    auto curL = volL.current;
    auto curR = volR.current;

    // Print some debug info
    debug(SID_EXEC, "volL: %f volR: %f vol0: %f pan0: %f\n", curL, curR, vol0, pan0);

    // Check for buffer overflow
    if (free() < numSamples) handleBufferOverflow();

    if constexpr (fading == false) {

        if (curL + curR == 0.0 || vol0 == 0.0) {

            // Fast path: All samples are zero
            for (isize i = 0; i < numSamples; i++) (void)sid0.stream.read();
            for (isize i = 0; i < numSamples; i++) write(SamplePair { 0, 0 } );

            // Send a MUTE message if applicable
            if (!muted) { muted = true; msgQueue.put(MSG_MUTE, true); }
            return;
        }
    }

    // Slow path: There is something to hear
    for (isize i = 0; i < numSamples; i++) {

        // Read SID sample from ring buffer
        float ch0 = (float)sidBridge.sid0.stream.read() * vol0;

        // Compute left and right channel output
        float l = ch0 * (1 - pan0);
        float r = ch0 * pan0;

        // Modulate the master volume
        if constexpr (fading) { volL.shift(); curL = volL.current; }
        if constexpr (fading) { volR.shift(); curR = volR.current; }

        // Apply master volume
        l *= curL;
        r *= curR;

        // Prevent hearing loss
        assert(abs(l) < 1.0);
        assert(abs(r) < 1.0);

        write(SamplePair { l, r } );
    }

    // Send a MUTE message if applicable
    if (muted) { muted = false; msgQueue.put(MSG_MUTE, false); }
}

template <bool fading> void
AudioPort::mixMultiSID(isize numSamples)
{
    auto vol0 = vol[0]; auto pan0 = pan[0];
    auto vol1 = vol[1]; auto pan1 = pan[1];
    auto vol2 = vol[2]; auto pan2 = pan[2];
    auto vol3 = vol[3]; auto pan3 = pan[3];
    auto curL = volL.current;
    auto curR = volR.current;

    // Print some debug info
    debug(SID_EXEC, "volL: %f volR: %f\n", curL, curR);
    debug(SID_EXEC, "vol0: %f vol1: %f vol2: %f vol3: %f\n", vol0, vol1, vol2, vol3);

    // Check for buffer overflow
    if (free() < numSamples) handleBufferOverflow();

    if constexpr (fading == false) {

        if (curL + curR == 0.0 || vol0 + vol1 + vol2 + vol3 == 0.0) {

            // Fast path: All samples are zero
            for (isize i = 0; i < numSamples; i++) (void)sid0.stream.read();
            for (isize i = 0; i < numSamples; i++) (void)sid1.stream.read(0);
            for (isize i = 0; i < numSamples; i++) (void)sid2.stream.read(0);
            for (isize i = 0; i < numSamples; i++) (void)sid3.stream.read(0);
            for (isize i = 0; i < numSamples; i++) write(SamplePair { 0, 0 } );

            // Send a MUTE message if applicable
            if (!muted) { muted = true; msgQueue.put(MSG_MUTE, true); }
            return;
        }
    }

    // Slow path: There is something to hear
    for (isize i = 0; i < numSamples; i++) {

        float ch0, ch1, ch2, ch3, l, r;

        ch0 = (float)sid0.stream.read()  * vol0;
        ch1 = (float)sid1.stream.read(0) * vol1;
        ch2 = (float)sid2.stream.read(0) * vol2;
        ch3 = (float)sid3.stream.read(0) * vol3;

        // Compute left and right channel output
        l = ch0 * (1 - pan0) + ch1 * (1 - pan1) + ch2 * (1 - pan2) + ch3 * (1 - pan3);
        r = ch0 * pan0 + ch1 * pan1 + ch2 * pan2 + ch3 * pan3;

        // Modulate the master volume
        if constexpr (fading) { volL.shift(); curL = volL.current; }
        if constexpr (fading) { volR.shift(); curR = volR.current; }

        // Apply master volume
        l *= curL;
        r *= curR;

        // Prevent hearing loss
        assert(abs(l) < 1.0);
        assert(abs(r) < 1.0);

        write(SamplePair { l, r } );
    }

    // Send a MUTE message if applicable
    if (muted) { muted = false; msgQueue.put(MSG_MUTE, false); }
}

isize
AudioPort::copyMono(float *buffer, isize n)
{
    {   SYNCHRONIZED

        // Be silent when the recorder is running (fill with zeroes)
        if (recorder.isRecording()) {

            for (isize i = 0; i < n; i++) { *buffer++ = 0; }
            return 0;
        }

        // Check for buffer underflows
        if (auto cnt = count(); cnt < n) {

            // Copy all we have and stepwise lower the volume to minimize cracks
            for (isize i = 0; i < cnt; i++) {

                SamplePair pair = read();
                *buffer++ = (pair.l + pair.r) * float(cnt - i) / float(cnt);
            }
            assert(isEmpty());

            // Fill the rest with zeroes
            for (isize i = cnt; i < n; i++) *buffer++ = 0;

            // Realign the ring buffer
            handleBufferUnderflow();

            return cnt;
        }

        // The standard case: We have enough samples. Copy the requested number
        for (isize i = 0; i < n; i++) {

            SamplePair pair = read();
            *buffer++ = pair.l + pair.r;
        }

        return n;
    }
}

isize
AudioPort::copyStereo(float *left, float *right, isize n)
{
    {   SYNCHRONIZED

        // Be silent when the recorder is running (fill with zeroes)
        if (recorder.isRecording()) {

            for (isize i = 0; i < n; i++) { *left++ = *right++ = 0; }
            return 0;
        }

        // Check for buffer underflows
        if (auto cnt = count(); cnt < n) {

            // Copy all we have and stepwise lower the volume to minimize cracks
            for (isize i = 0; i < cnt; i++) {

                SamplePair pair = read();
                *left++ = pair.l * float(cnt - i) / float(cnt);
                *right++ = pair.r * float(cnt - i) / float(cnt);
            }
            assert(isEmpty());

            // Fill the rest with zeroes
            for (isize i = cnt; i < n; i++) *left++ = *right++ = 0;

            // Realign the ring buffer
            handleBufferUnderflow();

            return cnt;
        }

        // The standard case: We have enough samples. Copy the requested number
        for (isize i = 0; i < n; i++) {

            SamplePair pair = read();
            *left++ = pair.l;
            *right++ = pair.r;
        }

        return n;
    }
}

isize
AudioPort::copyInterleaved(float *buffer, isize n)
{
    {   SYNCHRONIZED

        // Be silent when the recorder is running (fill with zeroes)
        if (recorder.isRecording()) {

            // Fill with zeroes
            for (isize i = 0; i < n; i++) { *buffer++ = 0; }
            return 0;
        }

        // Check for buffer underflows
        if (auto cnt = count(); cnt < n) {

            // Copy all we have and stepwise lower the volume to minimize cracks
            for (isize i = 0; i < cnt; i++) {

                SamplePair pair = read();
                *buffer++ = pair.l * float(cnt - i) / float(cnt);
                *buffer++ = pair.r * float(cnt - i) / float(cnt);
            }
            assert(isEmpty());

            // Fill the rest with zeroes
            for (isize i = cnt; i < n; i++) *buffer++ = *buffer++ = 0;

            // Realign the ring buffer
            handleBufferUnderflow();

            return cnt;
        }

        // We have enough samples. Copy over the requested number
        for (isize i = 0; i < n; i++) {

            SamplePair pair = read();
            *buffer++ = pair.l;
            *buffer++ = pair.r;
        }
        
        return n;
    }
}

}
