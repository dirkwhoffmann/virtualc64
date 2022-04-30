/// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "AnyCollection.h"

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
