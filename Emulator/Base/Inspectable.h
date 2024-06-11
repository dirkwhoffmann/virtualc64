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

#include "Dumpable.h"
#include <iostream>

namespace vc64 {

struct Void { };

/** Inspection interface
 *
 *  The purpose of the inspection interface is to provide functions for
 *  recording portions of the emulator's current state and returning them to
 *  the caller. All components record two different kinds of information:
 *  Infos and statistics. Infos comprise the values of important variables that
 *  are used internally by the component. Examples of statistical information
 *  are the average CIA activity or the current fill level of the audio buffer.
 */
template <typename T1, typename T2 = Void>
class Inspectable {

protected:
    
    mutable T1 info;
    mutable T2 stats;

public:

    Inspectable() { }
    virtual ~Inspectable() = default;

    T1 &getInfo() const {

        cacheInfo(info);
        return info;
    }

    T1 &getCachedInfo() const {

        return info;
    }

    T2 &getStats() const {

        cacheStats(stats);
        return stats;
    }

    T2 &getCachedStats() const {

        return stats;
    }

    virtual void clearStats() {

        memset(&stats, 0, sizeof(stats));
    }

    virtual void record() const {

        cacheInfo(info);
        cacheStats(stats);
    }

private:

    virtual void cacheInfo(T1 &result) const { };
    virtual void cacheStats(T2 &result) const { };
};

}
