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
StereoStream::copyMono(float *buffer, usize n, Volume &volL, Volume &volR)
{    
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);
    
    if (volL.isFading()) {
        
        for (usize i = 0; i < n; i++, volL.shift()) {
            
            SamplePair pair = read();
            *buffer++ = (pair.left + pair.right) * volL.current;
        }

    } else {
        
        for (usize i = 0; i < n; i++) {
                            
            SamplePair pair = read();
            *buffer++ = (pair.left + pair.right) * volL.current;
        }
    }
}

void
StereoStream::copyStereo(float *left, float *right, usize n, Volume &volL, Volume &volR)
{
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);

    // printf("isFading: %d\n", volume.isFading());
    // printf("cur: %f target: %f delta: %f\n", volume.current, volume.target[0], volume.delta[0]);
    
    if (volL.isFading() || volR.isFading()) {
                
        for (usize i = 0; i < n; i++, volL.shift(), volR.shift()) {
            
            SamplePair pair = read();
            *left++ = pair.left * volL.current;
            *right++ = pair.right * volR.current;
        }

    } else {
        
        for (usize i = 0; i < n; i++) {
                                        
            SamplePair pair = read();
            *left++ = pair.left * volL.current;
            *right++ = pair.right * volR.current;
        }
    }
}

void
StereoStream::copyInterleaved(float *buffer, usize n, Volume &volL, Volume &volR)
{
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);

    if (volL.isFading()) {
                
        for (usize i = 0; i < n; i++, volL.shift()) {
            
            SamplePair pair = read();
            *buffer++ = pair.left * volL.current;
            *buffer++ = pair.right * volR.current;
        }

    } else {
        
        for (usize i = 0; i < n; i++) {
                                        
            SamplePair pair = read();
            *buffer++ = pair.left * volL.current;
            *buffer++ = pair.right * volR.current;
        }
    }
}
