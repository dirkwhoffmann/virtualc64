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
#include "Serializable.hpp"

#pragma once

namespace vc64 {

/* An object of this class stores a single volume value and provides the means
 * to emulate a fading effect. Fading is utilized to avoid cracking noises if,
 * e.g., the emulator is put in pause mode.
 */
template <typename T> struct AudioVolume : Serializable {

    // Current volume
    T current = 1.0;

    // Value of 'current' if no fading takes place
    T normal = 1.0;

    // Target value and delta step
    T target = 1.0;
    T delta = 1.0;


    //
    // Methods
    //

    AudioVolume<T>& operator= (const AudioVolume<T>& other) {

        CLONE(current)
        CLONE(normal)
        CLONE(target)
        CLONE(delta)

        return *this;
    }
    
    template <class W>
    void serialize(W& worker)
    {
        worker

        << normal
        << target
        << delta;

        if (isChecker(worker)) return;

        worker

        << current;

    } SERIALIZERS(serialize);

    // Setter and getter
    T get() const { return current; }
    void set(T value) { current = normal = target = value; }
    
    // Returns true if the volume is currently fading in or out
    bool isFading() const { return current != target; }

    // Initiates a fading effect
    void fadeIn(isize steps) {
        
        target = normal;
        target = normal;
        delta  = normal / steps;
        delta  = normal / steps;
    }
    void fadeOut(isize steps) {
        
        target = 0;
        target = 0;
        delta  = normal / steps;
        delta  = normal / steps;
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
