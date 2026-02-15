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
#include <mutex>

namespace utl {

class Mutex
{
    std::mutex mutex;

public:

    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }
    bool tryLock() { return mutex.try_lock(); }
};

class ReentrantMutex
{
    std::recursive_mutex mutex;

public:

    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }
    bool tryLock() { return mutex.try_lock(); }
};

}
