// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/support/Bits.h"
#include <cassert>
#include <cstring>

namespace utl {

bool isZero(const u8 *ptr, isize size)
{
    for (isize i = 0; i < size; i++) {
        if (ptr[i]) return false;
    }
    return true;
}

void replace(u8 *p, isize size, const u8 *sequence, const u8 *substitute)
{
    replace((char *)p, size, (char *)sequence, (char *)substitute);
}

void replace(char *p, isize size, const char *sequence, const char *substitute)
{
    assert(p);
    assert(sequence);
    assert(substitute);
    assert(strlen(sequence) == strlen(substitute));

    auto len = strlen(sequence);

    for (isize i = 0; i < size - isize(len); i++) {

        if (strncmp(p + i, sequence, len) == 0) {

            memcpy((void *)(p + i), (void *)substitute, len);
            return;
        }
    }
    assert(false);
}

}
