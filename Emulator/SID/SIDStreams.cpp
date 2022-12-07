// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Muxer.h"

namespace vc64 {

void
StereoStream::alignWritePtr()
{
    this->align(this->cap() / 2);
}

void
StereoStream::copyMono(float *buffer, isize n, Volume &volL, Volume &volR)
{    
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);
    
    if (volL.isFading()) {
        
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
}

void
StereoStream::copyStereo(float *left, float *right, isize n, Volume &volL, Volume &volR)
{
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);

    // printf("isFading: %d\n", volume.isFading());
    // printf("cur: %f target: %f delta: %f\n", volume.current, volume.target[0], volume.delta[0]);
    
    if (volL.isFading() || volR.isFading()) {

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
}

void
StereoStream::copyInterleaved(float *buffer, isize n, Volume &volL, Volume &volR)
{
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);

    if (volL.isFading()) {

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
