// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "VICII.h"

namespace vc64 {

void
VICII::updateVicFunctionTable()
{    
    trace(VIC_DEBUG, "updateVicFunctionTable (dmaDebug: %d)\n", dmaDebug());
    
    vicfunc[0] = nullptr;
    vicfunc[64] = nullptr;
    vicfunc[65] = nullptr;

    // Assign model specific execution functions
    switch (config.revision) {
            
        case VICII_PAL_6569_R1:
        case VICII_PAL_6569_R3:
        case VICII_PAL_8565:
            
            if (dmaDebug()) {
                for (isize i = 1; i <= 63; i++) {
                    vicfunc[i] = getViciiFunc <PAL_CYCLE | DEBUG_CYCLE> (i);
                }
            } else {
                for (isize i = 1; i <= 63; i++) {
                    vicfunc[i] = getViciiFunc <PAL_CYCLE> (i);
                }
            }
            break;

        case VICII_NTSC_6567_R56A:
            
            if (dmaDebug()) {
                for (isize i = 1; i <= 11; i++) {
                    vicfunc[i] = getViciiFunc <PAL_CYCLE | DEBUG_CYCLE> (i);
                }
                for (isize i = 12; i <= 64; i++) {
                    vicfunc[i] = getViciiFunc <NTSC_CYCLE | DEBUG_CYCLE> (i);
                }
            } else {
                for (isize i = 1; i <= 11; i++) {
                    vicfunc[i] = getViciiFunc <PAL_CYCLE> (i);
                }
                for (isize i = 12; i <= 64; i++) {
                    vicfunc[i] = getViciiFunc <NTSC_CYCLE> (i);
                }
            }
            break;

        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
            
            if (dmaDebug()) {
                for (isize i = 1; i <= 65; i++) {
                    vicfunc[i] = getViciiFunc <NTSC_CYCLE | DEBUG_CYCLE> (i);
                }
            } else {
                for (isize i = 1; i <= 65; i++) {
                    vicfunc[i] = getViciiFunc <NTSC_CYCLE> (i);
                }
            }
            break;
            
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
