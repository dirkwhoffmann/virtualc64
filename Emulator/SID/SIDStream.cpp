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
StereoStream::copyMono(float *buffer, size_t n, Volume &volume)
{    
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);
    
    if (volume.current == volume.target) {
        
        for (size_t i = 0; i < n; i++) {
            
            SamplePair pair = read();
            *buffer++ = (pair.left + pair.right) * volume.current;
        }

    } else {
        
        for (size_t i = 0; i < n; i++, volume.shift()) {
                            
            SamplePair pair = read();
            *buffer++ = (pair.left + pair.right) * volume.current;
        }
    }
}

void
StereoStream::copyStereo(float *left, float *right, size_t n, Volume &volume)
{
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);

    if (volume.current == volume.target) {
                
        for (size_t i = 0; i < n; i++) {
            
            SamplePair pair = read();
            *left++ = pair.left * volume.current;
            *right++ = pair.right * volume.current;
        }

    } else {
        
        for (size_t i = 0; i < n; i++, volume.shift()) {
                                        
            SamplePair pair = read();
            *left++ = pair.left * volume.current;
            *right++ = pair.right * volume.current;
        }
    }
}

void
StereoStream::copyInterleaved(float *buffer, size_t n, Volume &volume)
{
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);

    if (volume.current == volume.target) {
                
        for (size_t i = 0; i < n; i++) {
            
            SamplePair pair = read();
            *buffer++ = pair.left * volume.current;
            *buffer++ = pair.right * volume.current;
        }

    } else {
        
        for (size_t i = 0; i < n; i++, volume.shift()) {
                                        
            SamplePair pair = read();
            *buffer++ = pair.left * volume.current;
            *buffer++ = pair.right * volume.current;
        }
    }
}
