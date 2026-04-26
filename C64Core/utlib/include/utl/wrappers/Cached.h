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

namespace utl {

template <typename T> class Cached : utl::Synchronizable {

public:

    // Underlying stored value (used by default getter)
    T value = {};

private:

    mutable std::optional<T> cache;
    mutable T live;

    // Default getter returns the stored value (for “backed by variable” mode)
    std::function<T()> getter = [&] { return value; };

public:

    Cached() = default;
    explicit Cached(std::function<T()> g) : getter(std::move(g)) { }
    void bind(std::function<T()> g) { getter = std::move(g); }

    // Returns the live value (without touching the cache)
    const T& current() const {

        live = getter();
        return live;
    }

    // Creates a stable snapshot
    void record() {

        SYNCHRONIZED
        cache = getter();
    }

    // Returns the last snapshot
    const T& backed() const {

        SYNCHRONIZED
        if (!cache) { cache = getter(); }
        return *cache;
    }

    // Force the cache to be recomputed in the next cached() call
    void invalidate() const {

        SYNCHRONIZED
        cache.reset();
    }
};

}
