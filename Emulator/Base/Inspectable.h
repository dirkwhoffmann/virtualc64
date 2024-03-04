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

#include <iostream>

namespace vc64 {

enum class Category
{
    BankMap,
    Checksums,
    Config,
    Current,
    Debug,
    Defaults,
    Disk,
    Dma,
    Layout,
    Properties,
    Registers,
    RunAhead,
    Sizeof,
    Slots,
    State,
    Stats,
    Summary,
    Tod,
};

struct Void { };

class Dumpable {

public:

    virtual ~Dumpable() = default;
    virtual void _dump(Category category, std::ostream& ss) const { }

    void dump(Category category, std::ostream& ss) const { _dump(category, ss); }
    void dump(Category category) const { dump(category, std::cout); }
};

/** Inspection interface
 *
 *  The purpose of the inspection interface is to provide functions for
 *  recording portions of the emulator's current state and returning them to
 *  the caller. All components record two different kinds of information:
 *  State variables and statistical information. State variables comprise
 *  important variables that are used internally by the component. Examples
 *  of statistical information are the average CIA activity or the current fill
 *  level of the audio buffer.
 */
template <typename T1, typename T2>
class Inspectable : public Dumpable {

protected:
    
    mutable T1 info;
    mutable T2 stats;

public:

    Inspectable() { }
    virtual ~Inspectable() = default;

    T1 &getState() const {

        if (!autoInspect()) recordState(info);
        return info;
    }

    T2 &getStats() const {

        if (!autoInspect()) recordStats(stats);
        return stats;
    }

    virtual void record() const {

        recordState(info);
        recordStats(stats);
    }

private:

    virtual bool autoInspect() const { return false; }

    virtual void recordState(T1 &result) const { };
    virtual void recordStats(T2 &result) const { };
};

}
