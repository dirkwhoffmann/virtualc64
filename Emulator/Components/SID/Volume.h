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

#pragma once

#include "Serializable.h"

namespace vc64 {

/* An object of this class stores a single volume value and provides the means
 * to emulate a fading effect. Fading is utilized to avoid cracking noises if,
 * e.g., the emulator is put in pause mode.
 */
template <typename T> struct AudioVolume : Serializable {

    // Current volume
    T current = 1.0;

    // Maximum volume
    T maximum = 1.0;

    // Fading direction and speed
    T delta = .0001f;


    //
    // Methods
    //

    AudioVolume<T>& operator= (const AudioVolume<T>& other) {

        CLONE(current)
        CLONE(maximum)
        CLONE(delta)

        return *this;
    }
    
    template <class W>
    void serialize(W& worker)
    {
        worker

        << maximum
        << delta;

        if (isChecker(worker)) return;

        worker

        << current;

    } SERIALIZERS(serialize);

    // Checks whether the volume is currently modulated
    bool isFadingIn() const { return delta > 0 &&  current != maximum; }
    bool isFadingOut() const { return delta < 0 && current != 0; }
    bool isFading() const { return isFadingIn() || isFadingOut(); }

    // Sets the volume to a fixed value
    void set(float value) { current = value; delta = 0.0; }

    // Gradually decrease the volume to zero
    void mute(isize steps = 10000) {

        if (steps == 0) {
            current = delta = 0;
        } else {
            delta = -maximum / steps;
        }
    }

    // Gradually increase the volume to max
    void unmute(isize steps = 10000) {

        if (steps == 0) {
            current = maximum; delta = 0;
        } else {
            delta = maximum / steps;
        }
    }

    // Shifts the current volume towards the target volume
    void shift() {
        
        if (delta < 0 && current != 0) {

            if ((current += delta) > 0) return;
            current = 0;
        }
        if (delta > 0 && current != maximum) {
            
            if ((current += delta) < maximum) return;
            current = maximum;
        }
    }
};

typedef AudioVolume<float> Volume;

}
