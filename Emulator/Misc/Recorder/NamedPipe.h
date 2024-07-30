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

#include "BasicTypes.h"

namespace vc64 {

class NamedPipe final {

    int pipe = -1;
    string name;

public:

    bool create(const string &name);
    bool open();
    bool isOpen();
    bool close();
    isize write(u8 *buffer, isize length);
};

}
