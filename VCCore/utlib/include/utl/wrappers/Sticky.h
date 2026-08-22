// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Streamable.h"

namespace utl {

/* A time-aware value wrapper with delayed value transitions.
 *
 * Sticky<T> ensures a minimum visible duration for each assigned value.
 * When a new value is set, the change becomes effective immediately if
 * the previous value has been stable for at least `stickiness` time units.
 * Otherwise, the previous value remains visible until the configured
 * stickiness interval has elapsed, after which the new value takes effect.
 *
 * Example        : --------------------> time
 * Unwrapped      : 111122223334445566789
 * stickiness = 0 : 111122223334445566789
 * stickiness = 1 : 111122223334445566789
 * stickiness = 2 : 111122223334445566779
 * stickiness = 3 : 111122223334445556669
 * stickiness = 4 : 111122223333444455559
 * stickiness = 5 : 111112222233333444449
 *
 * Invariants:
 *
 * - Timestamps must be monotonic (when >= last change).
 */

template <typename T> class Sticky {

    // Indicates how long the old value overrides the new value
    i64 stickiness;
    
    // The old value
    T oldValue;
    
    // Time stamp indicating when oldValue changed to newValue
    i64 change;
    
    // The new value
    T newValue;
    
public:
    
    explicit Sticky(i64 stickiness) : stickiness(stickiness) { init(); }
    
    void init()
    {
        oldValue = {};
        change = 0;
        newValue = {};
    }

    bool operator==(const Sticky &rhs) const {
        
        return
        stickiness == rhs.stickiness &&
        oldValue   == rhs.oldValue &&
        change     == rhs.change &&
        newValue   == rhs.newValue;
    }
        
    T get(i64 when) const {

        assert(when >= change);

        return when - change >= stickiness ? newValue : oldValue;
    }
    
    void set(T value, i64 when) {
        
        assert(when >= change);

        if (value == newValue) return;

        // Override the old value immediately when it was stable long enough
        if (when - change >= stickiness) oldValue = value;

        // Record the signal change
        newValue = value;
        change = when;
    }
};

}
