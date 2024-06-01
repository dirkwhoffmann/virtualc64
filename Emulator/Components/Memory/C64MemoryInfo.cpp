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
#include "C64Memory.h"
#include "C64.h"

namespace vc64 {

void
C64Memory::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        auto info = [&](const string &title, RomType type) {

            auto traits = c64.getRomTraits(type);

            os << tab(title) << (traits.crc ? "Installed" : "Not installed") << std::endl;

            if (traits.crc) {

                os << tab("Title") << traits.title << std::endl;
                os << tab("Subtitle") << traits.subtitle << std::endl;
                os << tab("Revision") << traits.revision << std::endl;
                os << tab("CRC32") << hex(traits.crc) << std::endl;
                os << tab("FNV64") << hex(traits.fnv) << std::endl;
            }
        };

        info("Basic ROM", ROM_TYPE_BASIC);
        os << std::endl;
        info("Character ROM", ROM_TYPE_CHAR);
        os << std::endl;
        info("Kernal ROM", ROM_TYPE_KERNAL);
        os << std::endl;
        info("Drive ROM", ROM_TYPE_VC1541);
    }
}

void
C64Memory::cacheInfo(MemInfo &result) const
{
    {   SYNCHRONIZED

        result.exrom = expansionPort.getExromLine();
        result.game = expansionPort.getGameLine();
        result.loram = cpu.getLoram();
        result.hiram = cpu.getHiram();
        result.charen = cpu.getCharen();

        result.bankMap = cpu.readPort();
        if (expansionPort.getGameLine()) result.bankMap |= 0x08;
        if (expansionPort.getExromLine()) result.bankMap |= 0x10;

        for (int i = 0; i < 16; i++) result.peekSrc[i] = peekSrc[i];
        for (int i = 0; i < 16; i++) result.vicPeekSrc[i] = vic.memSrc[i];
    }
}

}
