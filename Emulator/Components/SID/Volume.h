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

#include "Aliases.h"
#include "Serializable.h"

#pragma once

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

    // Fading parameters
    T target = 1.0;
    T delta = 1.0;


    //
    // Methods
    //

    AudioVolume<T>& operator= (const AudioVolume<T>& other) {

        CLONE(current)
        CLONE(maximum)
        CLONE(target)
        CLONE(delta)

        return *this;
    }
    
    template <class W>
    void serialize(W& worker)
    {
        worker

        << maximum
        << target
        << delta;

        if (isChecker(worker)) return;

        worker

        << current;

    } SERIALIZERS(serialize);

    // Checks whether the volume is currently fading
    bool isFading() const { return current != target; }
    bool isFadingIn() const { return isFading() && target != 0; }
    bool isFadingOut() const { return isFading() && target == 0; }

    // Fades in to the maximum volume
    void fadeIn(isize steps = 10000) {

        target = maximum;
        delta  = std::abs(current - maximum) / steps;
    }

    // Fades out to zero
    void fadeOut(isize steps = 10000) {

        target = 0;
        delta  = std::abs(current - maximum) / steps;
    }

    // Shifts the current volume towards the target volume
    void shift() {
        
        if (current == target) return;
        
        if (current < target) {
            if ((current += delta) < target) return;
        } else {
            if ((current -= delta) > target) return;
        }
        
        current = target;
    }
};

typedef AudioVolume<float> Volume;

}
