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
}

void
StereoStream::copyStereo(float *left, float *right, isize n, Volume &volL, Volume &volR)
{
    // The caller has to ensure that no buffer underflows occurs
    assert(count() >= n);

    if (volL.isFading() || volR.isFading()) {

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
