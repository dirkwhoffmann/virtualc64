// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Utils.h"
#include "Concurrency.h"
#include "Colors.h"

#include <vector>
#include <map>
#include <mutex>
#include <queue>

using std::vector;
using std::map;
using std::pair;
using std::swap;

#define synchronized \
for (AutoMutex _am(mutex); _am.active; _am.active = false)

/* Base class for all C64 objects. This class contains a textual description
 * of the object and offers various functions for printing debug messages and
 * warnings.
 */
class C64Object {
                     
protected:
    
    /* Mutex for implementing the 'synchronized' macro. The macro can be used
     * to prevent multiple threads to enter the same code block. It mimics the
     * behaviour of the well known Java construct 'synchronized(this) { }'.
     */
    Mutex mutex;

    
    //
    // Initializing
    //
    
public:
    
    virtual ~C64Object() { };
    
    // Returns the name for this component (e.g., "CPU" or "VICII")
    virtual const char *getDescription() const = 0;
    
    // Called by debug() and trace() to produce a detailed debug output
    virtual void prefix() const;
};
