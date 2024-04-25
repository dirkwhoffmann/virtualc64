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
#include "VICII.h"
#include "C64.h"

namespace vc64 {

void
VICII::initFuncTable(VICIIRevision revision)
{
    initFuncTable(revision, 0);
    initFuncTable(revision, DEBUG_CYCLE);
    initFuncTable(revision, HEADLESS_CYCLE);
}

void
VICII::initFuncTable(VICIIRevision revision, u16 flags)
{
    auto *table = functable[revision][flags];

    switch (revision) {

        case VICII_PAL_6569_R1:
        case VICII_PAL_6569_R3:
        case VICII_PAL_8565:

            for (isize i = 1; i <= 63; i++) {
                table[i] = getViciiFunc(PAL_CYCLE | flags, i);
            }
            break;

        case VICII_NTSC_6567_R56A:

            for (isize i = 1; i <= 11; i++) {
                table[i] = getViciiFunc(PAL_CYCLE | flags, i);
            }
            for (isize i = 12; i <= 64; i++) {
                table[i] = getViciiFunc(flags, i);
            }
            break;

        case VICII_NTSC_6567:
        case VICII_NTSC_8562:

            for (isize i = 1; i <= 65; i++) {
                table[i] = getViciiFunc(flags, i);
                assert(table[i] != nullptr);
            }
            break;

        default:
            fatalError;
    }
}

void
VICII::updateVicFunctionTable()
{    
    trace(VICII_DEBUG, "updateVicFunctionTable\n");
    
    u16 flags = c64.getHeadless() ? HEADLESS_CYCLE : dmaDebug() ? DEBUG_CYCLE : 0;
    
    for (isize i = 1; i < 66; i++) {

        vicfunc[i] = functable[config.revision][flags][i];
        if (i <= 63) assert(vicfunc[i] != nullptr);
    }
}

VICII::ViciiFunc
VICII::getViciiFunc(u16 flags, isize cycle)
{
    switch (flags) {

        case 0:
            return getViciiFunc <0> (cycle);

        case DEBUG_CYCLE:
            return getViciiFunc <DEBUG_CYCLE> (cycle);

        case HEADLESS_CYCLE:
            return getViciiFunc <HEADLESS_CYCLE> (cycle);

        case PAL_CYCLE:
            return getViciiFunc <PAL_CYCLE> (cycle);

        case PAL_CYCLE | DEBUG_CYCLE:
            return getViciiFunc <PAL_CYCLE | DEBUG_CYCLE> (cycle);

        case PAL_CYCLE | HEADLESS_CYCLE:
            return getViciiFunc <PAL_CYCLE | HEADLESS_CYCLE> (cycle);
            
        default:
            fatalError;
    }
}

template <u16 flags> VICII::ViciiFunc
VICII::getViciiFunc(isize cycle)
{
    assert(cycle >= 1 && cycle <= 65);
    
    switch (cycle) {
            
        case 1: return &VICII::cycle1 <flags>;
        case 2: return &VICII::cycle2 <flags>;
        case 3: return &VICII::cycle3 <flags>;
        case 4: return &VICII::cycle4 <flags>;
        case 5: return &VICII::cycle5 <flags>;
        case 6: return &VICII::cycle6 <flags>;
        case 7: return &VICII::cycle7 <flags>;
        case 8: return &VICII::cycle8 <flags>;
        case 9: return &VICII::cycle9 <flags>;
        case 10: return &VICII::cycle10 <flags>;
        case 11: return &VICII::cycle11 <flags>;
        case 12: return &VICII::cycle12 <flags>;
        case 13: return &VICII::cycle13 <flags>;
        case 14: return &VICII::cycle14 <flags>;
        case 15: return &VICII::cycle15 <flags>;
        case 16: return &VICII::cycle16 <flags>;
        case 17: return &VICII::cycle17 <flags>;
        case 18: return &VICII::cycle18 <flags>;

        case 55: return &VICII::cycle55 <flags>;
        case 56: return &VICII::cycle56 <flags>;
        case 57: return &VICII::cycle57 <flags>;
        case 58: return &VICII::cycle58 <flags>;
        case 59: return &VICII::cycle59 <flags>;
        case 60: return &VICII::cycle60 <flags>;
        case 61: return &VICII::cycle61 <flags>;
        case 62: return &VICII::cycle62 <flags>;
        case 63: return &VICII::cycle63 <flags>;
        case 64: return &VICII::cycle64 <flags>;
        case 65: return &VICII::cycle65 <flags>;

        default: return &VICII::cycle19to54<flags>;
    }
}

}
