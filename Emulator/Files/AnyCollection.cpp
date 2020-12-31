/// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "AnyCollection.h"
#include "Utils.h"

u16
AnyCollection::readWordBE(unsigned nr, u64 pos)
{
    return HI_LO(readByte(nr, pos), readByte(nr, pos + 1));
}

u16
AnyCollection::readWordLE(unsigned nr, u64 pos)
{
    return LO_HI(readByte(nr, pos), readByte(nr, pos + 1));
}

u16
AnyCollection::itemLoadAddr(unsigned nr)
{
    return readWordLE(nr, 0);
}

void
AnyCollection::copyItem(unsigned nr, u8 *buf, u64 len, u64 offset)
{
    for (u64 i = 0; i < len; i++) buf[i] = readByte(nr, i + offset);
}
