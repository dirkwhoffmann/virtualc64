// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "GCR.h"

namespace retro::vault::GCR {

void
encodeGcr(MutableBitView &view, isize bitPos, u8 value)
{
    u8 n1 = bin2gcr(value >> 4);
    u8 n2 = bin2gcr(value & 0xF);

    view.set(bitPos++, n1 & 0x10);
    view.set(bitPos++, n1 & 0x08);
    view.set(bitPos++, n1 & 0x04);
    view.set(bitPos++, n1 & 0x02);
    view.set(bitPos++, n1 & 0x01);

    view.set(bitPos++, n2 & 0x10);
    view.set(bitPos++, n2 & 0x08);
    view.set(bitPos++, n2 & 0x04);
    view.set(bitPos++, n2 & 0x02);
    view.set(bitPos++, n2 & 0x01);
}

u8
decodeGcr4(BitView &view, isize offset)
{
    auto codeword =
    (int)view[offset + 0] << 4 |
    (int)view[offset + 1] << 3 |
    (int)view[offset + 2] << 2 |
    (int)view[offset + 3] << 1 |
    (int)view[offset + 4];

    return invgcr[codeword];
}

u8
decodeGcr(BitView &view, isize offset)
{
    u8 nibble1 = decodeGcr4(view, offset);
    u8 nibble2 = decodeGcr4(view, offset + 5);

    return (u8)(nibble1 << 4 | nibble2);
}

}
