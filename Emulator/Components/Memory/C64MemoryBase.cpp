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

i64
C64Memory::getOption(Option option) const
{
    switch (option) {

        case OPT_MEM_INIT_PATTERN:  return config.ramPattern;
        case OPT_MEM_HEATMAP:       return config.heatmap;
        case OPT_MEM_SAVE_ROMS:     return config.saveRoms;

        default:
            fatalError;
    }
}

void
C64Memory::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_MEM_INIT_PATTERN:

            if (!RamPatternEnum::isValid(value)) {
                throw Error(ERROR_OPT_INV_ARG, RamPatternEnum::keyList());
            }
            return;

        case OPT_MEM_HEATMAP:
        case OPT_MEM_SAVE_ROMS:

            return;

        default:
            throw Error(ERROR_OPT_UNSUPPORTED);
    }
}

void
C64Memory::setOption(Option opt, i64 value)
{
    checkOption(opt, value);
    
    switch (opt) {

        case OPT_MEM_INIT_PATTERN:

            config.ramPattern = (RamPattern)value;
            return;

        case OPT_MEM_HEATMAP:

            config.heatmap = (bool)value;
            return;

        case OPT_MEM_SAVE_ROMS:

            config.saveRoms = (bool)value;
            return;

        default:
            fatalError;
    }
}

}
