// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/concurrency/ReentrantMutex.h"

namespace utl {

class AutoMutex
{
    ReentrantMutex &mutex;

public:

    bool active = true;

    AutoMutex(ReentrantMutex &ref) : mutex(ref) { mutex.lock(); }
    ~AutoMutex() { mutex.unlock(); }
};

}
