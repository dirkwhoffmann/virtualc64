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
    Config,
    Current,
    Debug,
    Defaults,
    Disk,
    Dma,
    Layout,
    Properties,
    Registers,
    Slots,
    State,
    Stats,
    Summary,
    Tod,
};

class Dumpable {

public:

    virtual ~Dumpable() { }
    virtual void _dump(Category category, std::ostream& ss) const { }

    void dump(Category category, std::ostream& ss) const { _dump(category, ss); }
    void dump(Category category) const { dump(category, std::cout); }
};

template <typename T>
class Inspectable : public Dumpable {

    mutable T info;

public:

    Inspectable() { }
    virtual ~Inspectable() { }


    virtual void doinspect(T &result) const { };
    T &getInfo() const { doinspect(info); return info; }
};

}
