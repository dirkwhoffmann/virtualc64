// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Synchronizable.h"
#include <functional>

/* Wraps a value with a live view and a cached snapshot. The class provides two
 * access modes:
 *
 *     current(): returns the live value from a getter.
 *     backed():  returns a cached snapshot (lazy, synchronized).
 *
 * By default, the public member `value` is used as the data source. A custom
 * getter can be installed via the constructor or bind().
 *
 * The snapshot can be updated with record() and invalidated with invalidate().
 * Snapshot operations are synchronized; current() is not.
 */

namespace utl {

template <typename T> class Backed : utl::Synchronizable {

public:

    // Stored value (used by default getter)
    T value = {};

private:

    mutable std::optional<T> cache;
    mutable T live;

    // Default getter returns the stored value ("backed by variable” mode)
    std::function<T()> getter = [&] { return value; };

public:

    Backed() = default;
    explicit Backed(std::function<T()> g) : getter(std::move(g)) { }
    void bind(std::function<T()> g) { getter = std::move(g); }

    // Start over with a clean state
    void clear() {

        {   SYNCHRONIZED

            value = {};
            cache.reset();
        }
    }

    // Returns the live value
    const T& current() const {

        live = getter();
        return live;
    }

    // Returns the latest snapshot
    const T& backed() const {

        {   SYNCHRONIZED

            if (!cache) { cache = getter(); }
            return *cache;
        }
    }

    // Takes a snapshot
    void record() {

        {   SYNCHRONIZED

            cache = getter();
        }
    }

    // Force the snapshot to be recomputed in the next backed() call
    void invalidate() const {

        {   SYNCHRONIZED

            cache.reset();
        }
    }
};

}
