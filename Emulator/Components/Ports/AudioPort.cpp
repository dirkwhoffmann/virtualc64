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
#include "Muxer.h"

namespace vc64 {

AudioBufferStats
AudioPort::getStats()
{
    stats.fillLevel = fillLevel();
    return stats;
}

void
AudioPort::connectMuxer(class Muxer *muxer)
{
    if (this->muxer != muxer) {

        debug(true, "Connecting data source: %p\n", (void *)muxer);

        disconnectMuxer(this->muxer);
        this->muxer = muxer;
    }
}

void 
AudioPort::disconnectMuxer(class Muxer *muxer)
{
    if (this->muxer && this->muxer == muxer) {

        debug(true, "Disconnecting data source: %p\n", (void *)muxer);

        this->muxer = nullptr;
    }
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

    // Reset the write pointer
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
AudioPort::mixSingleSID(isize numSamples)
{
    lock();

    auto vol0 = muxer->sid[0].vol; 
    auto pan0 = muxer->sid[0].pan;

    debug(SID_EXEC, "vol0: %f pan0: %f volL: %f volR: %f\n", 
          vol0, pan0, volL.current, volR.current);

    // Check for buffer overflow
    if (free() < numSamples) handleBufferOverflow();

    // Convert sound samples to floating point values and write into ringbuffer
    for (isize i = 0; i < numSamples; i++) {

        // Read SID sample from ring buffer
        float ch0 = (float)muxer->sidStream[0].read() * vol0;

        // Compute left and right channel output
        float l = ch0 * (1 - pan0);
        float r = ch0 * pan0;

        // Apply master volume
        l *= volL.current;
        r *= volR.current;

        // Apply ear protection
        assert(abs(l) < 1.0);
        assert(abs(r) < 1.0);

        write(SamplePair { l, r } );
    }

    unlock();
}

void
AudioPort::mixMultiSID(isize numSamples)
{
    lock();

    auto vol0 = muxer->sid[0].vol; auto pan0 = muxer->sid[0].pan;
    auto vol1 = muxer->sid[1].vol; auto pan1 = muxer->sid[1].pan;
    auto vol2 = muxer->sid[2].vol; auto pan2 = muxer->sid[2].pan;
    auto vol3 = muxer->sid[3].vol; auto pan3 = muxer->sid[3].pan;

    debug(SID_EXEC, "vol0: %f pan0: %f vol1: %f pan1: %f volL: %f volR: %f\n",
          vol0, pan0, vol1, pan1, volL.current, volR.current);

    // Check for buffer overflow
    if (free() < numSamples) handleBufferOverflow();

    // Convert sound samples to floating point values and write into ringbuffer
    for (isize i = 0; i < numSamples; i++) {

        float ch0, ch1, ch2, ch3, l, r;

        ch0 = (float)muxer->sidStream[0].read()  * vol0;
        ch1 = (float)muxer->sidStream[1].read(0) * vol1;
        ch2 = (float)muxer->sidStream[2].read(0) * vol2;
        ch3 = (float)muxer->sidStream[3].read(0) * vol3;

        // Compute left and right channel output
        l = ch0 * (1 - pan0) + ch1 * (1 - pan1) + ch2 * (1 - pan2) + ch3 * (1 - pan3);
        r = ch0 * pan0 + ch1 * pan1 + ch2 * pan2 + ch3 * pan3;

        // Apply master volume
        l *= volL.current;
        r *= volR.current;

        // Apply ear protection
        assert(abs(l) < 1.0);
        assert(abs(r) < 1.0);

        write(SamplePair { l, r } );
    }

    unlock();
}

void
AudioPort::copyMono(float *buffer, isize n)
{
    lock();

    // Check for buffer underflows
    if (count() < n) {

        unlock();
        handleBufferUnderflow();
        return;  // TODO: THROW EXCEPTION
    }

    if (false) { // }(volL.isFading()) {

        debug(AUDVOL_DEBUG, "L: %f R: %f (-> %f %f)\n",
              volL.current, volR.current, volL.target, volR.target);
        
        for (isize i = 0; i < n; i++, volL.shift()) {
            
            SamplePair pair = read();
            *buffer++ = (pair.left + pair.right) * volL.current;
        }

    } else {
        
        for (isize i = 0; i < n; i++) {

            SamplePair pair = read();
            *buffer++ = (pair.left + pair.right) * volL.current;
        }
    }

    unlock();
}

void
AudioPort::copyStereo(float *left, float *right, isize n)
{
    lock();

    // Check for buffer underflows
    if (count() < n) {

        unlock();
        handleBufferUnderflow();
        return;  // TODO: THROW EXCEPTION
    }

    if (false) { // }(volL.isFading() || volR.isFading()) {

        debug(AUDVOL_DEBUG, "L: %f R: %f (-> %f %f)\n",
              volL.current, volR.current, volL.target, volR.target);

        for (isize i = 0; i < n; i++, volL.shift(), volR.shift()) {
            
            SamplePair pair = read();
            *left++ = pair.left * volL.current;
            *right++ = pair.right * volR.current;
        }

    } else {
        
        for (isize i = 0; i < n; i++) {

            SamplePair pair = read();
            *left++ = pair.left * volL.current;
            *right++ = pair.right * volR.current;
        }
    }

    unlock();
}

void
AudioPort::copyInterleaved(float *buffer, isize n)
{
    lock();

    // Check for buffer underflows
    if (count() < n) {

        unlock();
        handleBufferUnderflow();
        return;  // TODO: THROW EXCEPTION
    }

    if (false) { // (volL.isFading()) {

        for (isize i = 0; i < n; i++, volL.shift()) {
            
            SamplePair pair = read();
            *buffer++ = pair.left * volL.current;
            *buffer++ = pair.right * volR.current;
        }

    } else {
        
        for (isize i = 0; i < n; i++) {

            SamplePair pair = read();
            *buffer++ = pair.left * volL.current;
            *buffer++ = pair.right * volR.current;
        }
    }
}

}
