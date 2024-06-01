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
#include "DriveMemory.h"

namespace vc64 {

void
DriveMemory::_reset(bool hard)
{
    // Initialize RAM with the power-up pattern (pattern from Hoxs64)
    for (isize i = 0; i < isizeof(ram); i++) {
        ram[i] = (i & 64) ? 0xFF : 0x00;
    }
}

void
DriveMemory::operator << (SerCounter &worker)
{
    serialize(worker);
}

void
DriveMemory::operator << (SerReader &worker)
{
    serialize(worker);
}

void
DriveMemory::operator << (SerWriter &worker)
{
    serialize(worker);
}

void
DriveMemory::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::BankMap) {

        DrvMemType oldsrc = usage[0];
        isize oldi = 0;

        for (isize i = 0; i <= 64; i++) {
            DrvMemType newsrc = i < 64 ? usage[i] : (DrvMemType)-1;
            if (oldsrc != newsrc) {
                os << "        ";
                os << util::hex((u16)(oldi << 10)) << " - ";
                os << util::hex((u16)((i << 10) - 1)) << " : ";
                os << DrvMemTypeEnum::key(oldsrc) << std::endl;
                oldsrc = newsrc; oldi = i;
            }
        }
    }
}

}
