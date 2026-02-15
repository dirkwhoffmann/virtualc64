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

namespace utl {

template <typename T> class Memorized {

    mutable std::optional<T> value;
    std::function<T()> getter;

public:

    explicit Memorized(std::function<T()> g = {}) : getter(g) {}
    void bind(std::function<T()> g) { getter = g; value.reset(); }

    void record() const {

        if (getter) { value = getter(); }
    }

    const T &current() const {

        record();
        return *value;
    }

    const T &cached() const {

        if (!value) record();
        return *value;
    }
};

}

