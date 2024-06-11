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
    Breakpoints,
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
    Watchpoints
};

class Dumpable {

public:

    virtual ~Dumpable() = default;
    virtual void _dump(Category category, std::ostream& ss) const { }

    void dump(Category category, std::ostream& ss) const;
    void dump(Category category) const;
};

}
