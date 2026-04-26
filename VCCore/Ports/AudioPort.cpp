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

#if 0

void
AudioPort::alignWritePtr()
{
    this->align(this->cap() / 2);
}

void 
AudioPort::clamp(isize maxSamples)
{
    SYNCHRONIZED

    while (count() > maxSamples) read();
}
#endif

void
AudioPort::generateSamples()
{
    bool muted = isMuted();

    // Send the MUTE message if needed
    if (muted != wasMuted) { msgQueue.put(Msg::MUTE, wasMuted = muted); }

    stream.mutex.lock();

    // Check how many samples can be generated
    auto s0 = sid0.stream.count();
    auto s1 = sid1.stream.count();
    auto s2 = sid2.stream.count();
    auto s3 = sid3.stream.count();

    auto numSamples = s0;
    if (s1) numSamples = std::min(numSamples, s1);
    if (s2) numSamples = std::min(numSamples, s2);
    if (s3) numSamples = std::min(numSamples, s3);

    // Check for a buffer overflow
    if (stream.free() < numSamples) handleBufferOverflow();

    // Generate the samples
    bool fading = volL.isFading() || volR.isFading();

    if (sid1.isEnabled() || sid2.isEnabled() || sid3.isEnabled()) {
        fading ? mixMultiSID<true>(numSamples) : mixMultiSID<false>(numSamples);
    } else {
        fading ? mixSingleSID<true>(numSamples) : mixSingleSID<false>(numSamples);
    }

    stream.mutex.unlock();
}

void
AudioPort::updateSampleRateCorrection()
{
    if (config.asr) {

        // Compute the difference between the ideal and the current fill level
        auto error = (0.5 - stream.fillLevel());

        // Smooth it out
        sampleRateError = 0.75 * sampleRateError + 0.25 * error;

        // Compute a sample rate correction
        auto correction = (0.5 - stream.fillLevel()) * 4000;

        // Smooth it out
        sampleRateCorrection = (sampleRateCorrection * 0.75) + (correction * 0.25);

        loginfo(AUDBUF_DEBUG, "ASR correction: %.0f Hz (fill: %.2f)\n",
              sampleRateCorrection, stream.fillLevel());

    } else {

        sampleRateCorrection = 0;
    }
}

/*
void
AudioPort::fadeOut()
{
    SYNCHRONIZED

    loginfo(AUDVOL_DEBUG, "Fading out (%ld samples)...\n", count());

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
*/

template <bool fading> void
AudioPort::mixSingleSID(isize numSamples)
{
    auto vol0 = vol[0];
    auto pan0 = pan[0];
    auto curL = volL.current;
    auto curR = volR.current;

    // Print some debug info
    loginfo(SID_EXEC, "volL: %f volR: %f vol0: %f pan0: %f\n", curL, curR, vol0, pan0);

    if (wasMuted) {

        // Fast path: All samples are zero
        for (isize i = 0; i < numSamples; i++) (void)sid0.stream.read();
        for (isize i = 0; i < numSamples; i++) stream.write(SamplePair { 0, 0 } );

    } else {

        // Slow path: There is something to hear
        for (isize i = 0; i < numSamples; i++) {

            // Read SID sample from ring buffer
            float ch0 = (float)sidBridge.sid0.stream.read() * vol0;

            // Compute left and right channel output
            double l = ch0 * (1 - pan0);
            double r = ch0 * pan0;

            // Modulate the master volume
            if constexpr (fading) { volL.shift(); curL = volL.current; }
            if constexpr (fading) { volR.shift(); curR = volR.current; }

            // Apply master volume
            l *= curL;
            r *= curR;

            // Prevent hearing loss
            assert(std::abs(l) < 1.0);
            assert(std::abs(r) < 1.0);

            stream.write(SamplePair { float(l), float(r) } );
        }
    }
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
    loginfo(SID_EXEC, "volL: %f volR: %f\n", curL, curR);
    loginfo(SID_EXEC, "vol0: %f vol1: %f vol2: %f vol3: %f\n", vol0, vol1, vol2, vol3);

    if (wasMuted) {

        // Fast path: All samples are zero
        for (isize i = 0; i < numSamples; i++) (void)sid0.stream.read();
        for (isize i = 0; i < numSamples; i++) (void)sid1.stream.read(0);
        for (isize i = 0; i < numSamples; i++) (void)sid2.stream.read(0);
        for (isize i = 0; i < numSamples; i++) (void)sid3.stream.read(0);
        for (isize i = 0; i < numSamples; i++) stream.write(SamplePair { 0, 0 } );

    } else {

        // Slow path: There is something to hear
        for (isize i = 0; i < numSamples; i++) {

            double ch0, ch1, ch2, ch3, l, r;

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

            stream.write(SamplePair { float(l), float(r) } );
        }
    }
}

void
AudioPort::handleBufferUnderflow()
{
    // Wipe out the buffer and reset the write pointer
    stream.clear(SamplePair{0,0});
    stream.alignWritePtr();

    // Determine the elapsed seconds since the last pointer adjustment
    auto elapsedTime = utl::Time::now() - lastAlignment;
    lastAlignment = utl::Time::now();

    // Adjust the sample rate if the emulator runs under normal conditions
    if (emulator.isRunning() && !emulator.isWarping()) {

        stats.bufferUnderflows++;
        loginfo(AUDBUF_DEBUG, "Audio buffer underflow after %f seconds\n", elapsedTime.asSeconds());

        // Adjust the sample rate
        setSampleRate(host.getConfig().sampleRate);
        loginfo(AUDBUF_DEBUG, "New sample rate = %.2f\n", sampleRate);
    }
}

void
AudioPort::handleBufferOverflow()
{
    // Reset the write pointer
    stream.alignWritePtr();

    // Determine the number of elapsed seconds since the last adjustment
    auto elapsedTime = utl::Time::now() - lastAlignment;
    lastAlignment = utl::Time::now();

    // Adjust the sample rate if the emulator runs under normal conditions
    if (emulator.isRunning() && !emulator.isWarping()) {

        stats.bufferOverflows++;
        loginfo(AUDBUF_DEBUG, "Audio buffer overflow after %f seconds\n", elapsedTime.asSeconds());

        // Adjust the sample rate
        setSampleRate(host.getConfig().sampleRate);
        loginfo(AUDBUF_DEBUG, "New sample rate = %.2f\n", sampleRate);
    }
}

void
AudioPort::eliminateCracks()
{
    stream.eliminateCracks();
    volL.current = 0;
    volR.current = 0;
}

isize
AudioPort::copyMono(float *buffer, isize n)
{
    // Copy sound samples
    auto cnt = stream.copyMono(buffer, n);
    stats.consumedSamples += cnt;

    // Check for a buffer underflow
    if (cnt < n) handleBufferUnderflow();

    return cnt;
}

isize
AudioPort::copyStereo(float *left, float *right, isize n)
{
    // Inform the sample rate detector about the number of requested samples
    detector.feed(n);

    // Copy sound samples
    auto cnt = stream.copyStereo(left, right, n);
    stats.consumedSamples += cnt;

    // Check for a buffer underflow
    if (cnt < n) handleBufferUnderflow();

    return cnt;
}

isize
AudioPort::copyInterleaved(float *buffer, isize n)
{
    // Copy sound samples
    auto cnt = stream.copyInterleaved(buffer, n);
    stats.consumedSamples += cnt;

    // Check for a buffer underflow
    if (cnt < n) handleBufferUnderflow();

    return cnt;
}

}
