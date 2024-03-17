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

#pragma once

#include "Concurrency.h"

namespace vc64 {

class Synchronizable {

public:

    /* Mutex for implementing the 'synchronized' macro. The macro can be used
     * to prevent multiple threads to enter the same code block. It mimics the
     * behaviour of the well known Java construct 'synchronized(this) { }'.
     */
    mutable util::ReentrantMutex mutex;

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
#define SYNCHRONIZED util::AutoMutex _am(mutex);

}
