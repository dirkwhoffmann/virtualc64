// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "SIDBridge.h"

void
SIDStream::append(short *data, size_t count)
{
    assert(false);
    
    // Check for buffer overflow
    if (free() < count) {
        bridge.signalOverflow = true;
        return;
        // bridge.handleBufferOverflow();
    }
    
    // Convert sound samples to floating point values and write into ringbuffer
    for (unsigned i = 0; i < count; i++) {
        write(float(data[i]) * scale);
    }
}

void
StereoStream::copyMono(float *buffer, size_t n,
                       i32 &volume, i32 targetVolume, i32 volumeDelta)
{
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);

    // REMOVE ASAP
    targetVolume = volume;
    
    if (volume == targetVolume) {
        
        // float scale = volume / 10000.0f;
        float scale = 1.0;
        
        for (size_t i = 0; i < n; i++) {
            
            SamplePair pair = read();
            *buffer++ = (pair.left + pair.right) * scale;
        }

    } else {
        
        for (size_t i = 0; i < n; i++) {
                            
            if (volume < targetVolume) {
                volume += MIN(volumeDelta, targetVolume - volume);
            } else {
                volume -= MIN(volumeDelta, volume - targetVolume);
            }

            float scale = volume / 10000.0f;

            SamplePair pair = read();
            *buffer++ = (pair.left + pair.right) * scale;
        }
    }
}

void
StereoStream::copy(float *left, float *right, size_t n,
                   i32 &volume, i32 targetVolume, i32 volumeDelta)
{
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);

    // REMOVE ASAP
    targetVolume = volume;

    if (volume == targetVolume) {
        
        // float scale = volume / 10000.0f;
        float scale = 1.0;
        
        for (size_t i = 0; i < n; i++) {
            
            SamplePair pair = read();
            *left++ = pair.left * scale;
            *right++ = pair.right * scale;
        }

    } else {
        
        for (size_t i = 0; i < n; i++) {
                            
            if (volume < targetVolume) {
                volume += MIN(volumeDelta, targetVolume - volume);
            } else {
                volume -= MIN(volumeDelta, volume - targetVolume);
            }

            float scale = volume / 10000.0f;

            SamplePair pair = read();
            *left++ = pair.left * scale;
            *right++ = pair.right * scale;
        }
    }
}

void
StereoStream::copyInterleaved(float *buffer, size_t n,
                              i32 &volume, i32 targetVolume, i32 volumeDelta)
{
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);

    // REMOVE ASAP
    targetVolume = volume;

    if (volume == targetVolume) {
        
        // float scale = volume / 10000.0f;
        float scale = 1.0;
        
        for (size_t i = 0; i < n; i++) {
            
            SamplePair pair = read();
            *buffer++ = pair.left * scale;
            *buffer++ = pair.right * scale;
        }

    } else {
        
        for (size_t i = 0; i < n; i++) {
                            
            if (volume < targetVolume) {
                volume += MIN(volumeDelta, targetVolume - volume);
            } else {
                volume -= MIN(volumeDelta, volume - targetVolume);
            }

            float scale = volume / 10000.0f;

            SamplePair pair = read();
            *buffer++ = pair.left * scale;
            *buffer++ = pair.right * scale;
        }
    }
}
