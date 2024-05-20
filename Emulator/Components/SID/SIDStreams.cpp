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
StereoStream::getStats()
{
    stats.fillLevel = fillLevel();
    return stats;
}

void
StereoStream::connectMuxer(class Muxer *muxer)
{
    if (this->muxer != muxer) {

        debug(true, "Connecting data source: %p\n", (void *)muxer);

        disconnectMuxer(this->muxer);
        this->muxer = muxer;
    }
}

void 
StereoStream::disconnectMuxer(class Muxer *muxer)
{
    if (this->muxer && this->muxer == muxer) {

        debug(true, "Disconnecting data source: %p\n", (void *)muxer);

        this->muxer = nullptr;
    }
}

void
StereoStream::alignWritePtr()
{
    this->align(this->cap() / 2);
}

void
StereoStream::handleBufferUnderflow()
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
StereoStream::handleBufferOverflow()
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
StereoStream::copyMono(float *buffer, isize n)
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
StereoStream::copyStereo(float *left, float *right, isize n)
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
StereoStream::copyInterleaved(float *buffer, isize n)
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
