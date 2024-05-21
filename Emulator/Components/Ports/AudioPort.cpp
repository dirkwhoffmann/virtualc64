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
AudioPort::connectDataSource(class SIDBridge *bridge)
{
    if (dataSource != bridge) {

        debug(true, "Connecting data source: %p\n", (void *)bridge);

        lock();
        disconnectDataSource();
        dataSource = bridge;
        fadeIn();
        unlock();
    }
}

void
AudioPort::disconnectDataSource()
{
    if (dataSource) {

        debug(true, "Disconnecting data source: %p\n", (void *)dataSource);
        
        lock();
        fadeOut();
        dataSource = nullptr;
        unlock();
    }
}

void
AudioPort::disconnectDataSource(class SIDBridge *bridge)
{
    if (dataSource == bridge) disconnectDataSource();
}

void 
AudioPort::reset(SIDBridge *bridge, bool hard)
{
    if (dataSource == bridge) {

        lock();
        this->clear(SamplePair{0,0});
        alignWritePtr();
        unlock();
    }
}

void
AudioPort::run(SIDBridge *bridge)
{
    if (dataSource == bridge) fadeIn();
}

void 
AudioPort::pause(SIDBridge *bridge)
{
    if (dataSource == bridge) fadeOut();
}

void 
AudioPort::warpOn(SIDBridge *bridge)
{
    if (dataSource == bridge) fadeOut();
}

void 
AudioPort::warpOff(SIDBridge *bridge)
{
    if (dataSource == bridge) fadeIn();
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

        if (dataSource) dataSource->stats.bufferUnderflows++;
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

        if (dataSource) dataSource->stats.bufferOverflows++;
        warn("Last overflow: %f seconds ago\n", elapsedTime.asSeconds());
    }
}

void 
AudioPort::generateSamples(SIDBridge *bridge)
{
    lock();

    if (this->dataSource == bridge) {

        // Get the master volume
        volL.maximum = bridge->volL;
        volR.maximum = bridge->volR;

        // Check how many samples can be generated
        auto s0 = bridge->sidStream[0].count();
        auto s1 = bridge->sidStream[1].count();
        auto s2 = bridge->sidStream[2].count();
        auto s3 = bridge->sidStream[3].count();

        auto numSamples = s0;
        if (s1) numSamples = std::min(numSamples, s1);
        if (s2) numSamples = std::min(numSamples, s2);
        if (s3) numSamples = std::min(numSamples, s3);

        // Generate the samples
        if (bridge->isEnabled(1) || bridge->isEnabled(2) || bridge->isEnabled(3)) {
            mixMultiSID(numSamples);
        } else {
            mixSingleSID(numSamples);
        }

    } else {

        // Discard all generated samples
        bridge->sidStream[0].clear();
        bridge->sidStream[1].clear();
        bridge->sidStream[2].clear();
        bridge->sidStream[3].clear();
    }

    unlock();
}

void
AudioPort::fadeIn()
{
    debug(AUDVOL_DEBUG, "Fading in...\n");

    volL.fadeIn();
    volR.fadeIn();
}

void
AudioPort::fadeOut()
{
    debug(AUDVOL_DEBUG, "Fading out (%ld samples)...\n", count());

    volL.set(0.0);
    volR.set(0.0);

    float scale = 1.0f;
    float delta = 1.0f / count();

    for (isize i = begin(); i != end(); i = next(i)) {

        scale -= delta;
        assert(scale >= -0.1 && scale < 1.0);

        elements[i].left *= scale;
        elements[i].right *= scale;
    }

    for (isize i = end(); i != begin(); i = next(i)) {

        elements[i] = SamplePair { 0, 0 };
    }
}

void
AudioPort::mixSingleSID(isize numSamples)
{
    auto vol0 = dataSource->sid[0].vol;
    auto pan0 = dataSource->sid[0].pan;

    debug(SID_EXEC, "vol0: %f pan0: %f volL: %f volR: %f\n",
          vol0, pan0, volL.current, volR.current);

    // Check for buffer overflow
    if (free() < numSamples) handleBufferOverflow();

    // Convert sound samples to floating point values and write into ringbuffer
    if (volL.isFading() || volR.isFading()) {

        for (isize i = 0; i < numSamples; i++) {

            // Read SID sample from ring buffer
            float ch0 = (float)dataSource->sidStream[0].read() * vol0;

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
            float ch0 = (float)dataSource->sidStream[0].read() * vol0;

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
    auto vol0 = dataSource->sid[0].vol; auto pan0 = dataSource->sid[0].pan;
    auto vol1 = dataSource->sid[1].vol; auto pan1 = dataSource->sid[1].pan;
    auto vol2 = dataSource->sid[2].vol; auto pan2 = dataSource->sid[2].pan;
    auto vol3 = dataSource->sid[3].vol; auto pan3 = dataSource->sid[3].pan;

    debug(SID_EXEC, "vol0: %f pan0: %f vol1: %f pan1: %f volL: %f volR: %f\n",
          vol0, pan0, vol1, pan1, volL.current, volR.current);

    // Check for buffer overflow
    if (free() < numSamples) handleBufferOverflow();

    // Convert sound samples to floating point values and write into ringbuffer
    for (isize i = 0; i < numSamples; i++) {

        float ch0, ch1, ch2, ch3, l, r;

        ch0 = (float)dataSource->sidStream[0].read()  * vol0;
        ch1 = (float)dataSource->sidStream[1].read(0) * vol1;
        ch2 = (float)dataSource->sidStream[2].read(0) * vol2;
        ch3 = (float)dataSource->sidStream[3].read(0) * vol3;

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

    if (!muted) {

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

    if (!muted) {

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

            printf("Underflow: cnt = %ld\n", cnt);
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

    if (!muted) {

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
