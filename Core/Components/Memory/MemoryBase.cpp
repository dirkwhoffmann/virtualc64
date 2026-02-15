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
#include "Memory.h"
#include "C64.h"

namespace vc64 {

void
Memory::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

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

        info("Basic ROM", RomType::BASIC);
        os << std::endl;
        info("Character ROM", RomType::CHAR);
        os << std::endl;
        info("Kernal ROM", RomType::KERNAL);
        os << std::endl;
        info("Drive ROM", RomType::VC1541);
    }
}

void
Memory::cacheInfo(MemInfo &result) const
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

        for (isize i = 0; i < 16; i++) result.peekSrc[i] = peekSrc[i];
        for (isize i = 0; i < 16; i++) result.vicPeekSrc[i] = vic.memSrc[i];
    }
}

i64
Memory::getOption(Opt option) const
{
    switch (option) {

        case Opt::MEM_INIT_PATTERN:  return (i64)config.ramPattern;
        case Opt::MEM_HEATMAP:       return (i64)config.heatmap;
        case Opt::MEM_SAVE_ROMS:     return (i64)config.saveRoms;

        default:
            fatalError;
    }
}

void
Memory::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::MEM_INIT_PATTERN:

            if (!RamPatternEnum::isValid(value)) {
                throw AppError(Fault::OPT_INV_ARG, RamPatternEnum::keyList());
            }
            return;

        case Opt::MEM_HEATMAP:
        case Opt::MEM_SAVE_ROMS:

            return;

        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
Memory::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);
    
    switch (opt) {

        case Opt::MEM_INIT_PATTERN:

            config.ramPattern = (RamPattern)value;
            return;

        case Opt::MEM_HEATMAP:

            config.heatmap = (bool)value;
            return;

        case Opt::MEM_SAVE_ROMS:

            config.saveRoms = (bool)value;
            return;

        default:
            fatalError;
    }
}

}
