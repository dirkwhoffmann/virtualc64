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

#include "config.h"
#include "AnyCollection.h"
#include "Macros.h"

namespace vc64 {

u16
AnyCollection::readWordBE(isize nr, isize pos) const
{
    return HI_LO(readByte(nr, pos), readByte(nr, pos + 1));
}

u16
AnyCollection::readWordLE(isize nr, isize pos) const
{
    return LO_HI(readByte(nr, pos), readByte(nr, pos + 1));
}

u16
AnyCollection::itemLoadAddr(isize nr) const
{
    return readWordLE(nr, 0);
}

void
AnyCollection::copyItem(isize nr, u8 *buf, isize len, isize offset) const
{
    for (isize i = 0; i < len; i++) buf[i] = readByte(nr, i + offset);
}

}
