// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/concurrency/AutoMutex.h"

namespace utl {

class Synchronizable {

public:

    /* Mutex for implementing the 'synchronized' macro. The macro can be used
     * to prevent multiple threads to enter the same code block. It mimics the
     * behaviour of the well known Java construct 'synchronized(this) { }'.
     */
    mutable ReentrantMutex mutex;

};

/* The following macro can be utilized to prevent multiple threads to enter the
 * same code block. It mimics the behaviour of the Java construct
 * 'synchronized(this) { }'. To secure a code-block, use the following syntax:
 *
 *     {    SYNCHRONIZED
 *
 *          <command>
 *          ...
 *     }
 */
#define SYNCHRONIZED utl::AutoMutex _am(mutex);

}
