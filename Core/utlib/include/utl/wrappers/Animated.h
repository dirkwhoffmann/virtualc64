// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"

namespace utl::wrappers {

/* This class provides a wrapper around a value type making it fadable. I.e.,
 * the value can be smoothly increased to it's maximum value or decreases to
 * zero by calling the shift() function.
 */
template <typename T> class Animated {

    static_assert(std::is_arithmetic_v<T>, "Animated<T> requires an arithmetic type");
    
public:

    // Current value
    double current = 1.0;

    // Maximum value
    double maximum = 1.0;

    // Fading direction and speed
    double delta = 0.0;


    //
    // Operators
    //

    Animated<T>& operator= (T other) {

        current = maximum = (double)other;
        delta = 0;
        return *this;
    }

    operator T() const { return (T)current; }


    //
    // Methods
    //

    // Checks whether the value is currently modulated
    bool isFadingIn() const { return delta > 0 &&  current != maximum; }
    bool isFadingOut() const { return delta < 0 && current != 0; }
    bool isFading() const { return isFadingIn() || isFadingOut(); }

    // Gradually decrease the value to zero
    void fadeOut(isize steps) {

        if (steps == 0) {
            current = delta = 0;
        } else {
            delta = -maximum / steps;
        }
    }

    // Gradually increase the volume to max
    void fadeIn(isize steps) {

        if (steps == 0) {
            current = maximum; delta = 0;
        } else {
            delta = maximum / steps;
        }
    }

    // Shifts the current value
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

}
