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

void
VICII::updateVicFunctionTable()
{
    // bool headless = false;
    
    trace(VIC_DEBUG, "updateVicFunctionTable (dmaDebug: %d)\n", dmaDebug());
    
    // Assign model independent execution functions
    vicfunc[0] = nullptr;
    if (dmaDebug()) {
        vicfunc[12] = &VICII::cycle12<PAL_CYCLE | DEBUG_CYCLE>;
        vicfunc[13] = &VICII::cycle13<PAL_CYCLE | DEBUG_CYCLE>;
        vicfunc[14] = &VICII::cycle14<PAL_CYCLE | DEBUG_CYCLE>;
        vicfunc[15] = &VICII::cycle15<PAL_CYCLE | DEBUG_CYCLE>;
        vicfunc[16] = &VICII::cycle16<PAL_CYCLE | DEBUG_CYCLE>;
        vicfunc[17] = &VICII::cycle17<PAL_CYCLE | DEBUG_CYCLE>;
        vicfunc[18] = &VICII::cycle18<PAL_CYCLE | DEBUG_CYCLE>;
        for (unsigned cycle = 19; cycle <= 54; cycle++)
            vicfunc[cycle] = &VICII::cycle19to54<PAL_CYCLE | DEBUG_CYCLE>;
        vicfunc[56] = &VICII::cycle56<PAL_CYCLE | DEBUG_CYCLE>;
    } else {
        vicfunc[12] = &VICII::cycle12<PAL_CYCLE>;
        vicfunc[13] = &VICII::cycle13<PAL_CYCLE>;
        vicfunc[14] = &VICII::cycle14<PAL_CYCLE>;
        vicfunc[15] = &VICII::cycle15<PAL_CYCLE>;
        vicfunc[16] = &VICII::cycle16<PAL_CYCLE>;
        vicfunc[17] = &VICII::cycle17<PAL_CYCLE>;
        vicfunc[18] = &VICII::cycle18<PAL_CYCLE>;
        for (unsigned cycle = 19; cycle <= 54; cycle++)
            vicfunc[cycle] = &VICII::cycle19to54<PAL_CYCLE>;
        vicfunc[56] = &VICII::cycle56<PAL_CYCLE>;
    }
    
    // Assign model specific execution functions
    switch (getRevision()) {
            
        case VICII_PAL_6569_R1:
        case VICII_PAL_6569_R3:
        case VICII_PAL_8565:
            
            if (dmaDebug()) {
                vicfunc[1] = &VICII::cycle1<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[2] = &VICII::cycle2<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[3] = &VICII::cycle3<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[4] = &VICII::cycle4<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[5] = &VICII::cycle5<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[6] = &VICII::cycle6<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[7] = &VICII::cycle7<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[8] = &VICII::cycle8<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[9] = &VICII::cycle9<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[10] = &VICII::cycle10<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[11] = &VICII::cycle11<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[55] = &VICII::cycle55<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[57] = &VICII::cycle57<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[58] = &VICII::cycle58<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[59] = &VICII::cycle59<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[60] = &VICII::cycle60<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[61] = &VICII::cycle61<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[62] = &VICII::cycle62<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[63] = &VICII::cycle63<PAL_CYCLE | DEBUG_CYCLE>;
            } else {
                vicfunc[1] = &VICII::cycle1<PAL_CYCLE>;
                vicfunc[2] = &VICII::cycle2<PAL_CYCLE>;
                vicfunc[3] = &VICII::cycle3<PAL_CYCLE>;
                vicfunc[4] = &VICII::cycle4<PAL_CYCLE>;
                vicfunc[5] = &VICII::cycle5<PAL_CYCLE>;
                vicfunc[6] = &VICII::cycle6<PAL_CYCLE>;
                vicfunc[7] = &VICII::cycle7<PAL_CYCLE>;
                vicfunc[8] = &VICII::cycle8<PAL_CYCLE>;
                vicfunc[9] = &VICII::cycle9<PAL_CYCLE>;
                vicfunc[10] = &VICII::cycle10<PAL_CYCLE>;
                vicfunc[11] = &VICII::cycle11<PAL_CYCLE>;
                vicfunc[55] = &VICII::cycle55<PAL_CYCLE>;
                vicfunc[57] = &VICII::cycle57<PAL_CYCLE>;
                vicfunc[58] = &VICII::cycle58<PAL_CYCLE>;
                vicfunc[59] = &VICII::cycle59<PAL_CYCLE>;
                vicfunc[60] = &VICII::cycle60<PAL_CYCLE>;
                vicfunc[61] = &VICII::cycle61<PAL_CYCLE>;
                vicfunc[62] = &VICII::cycle62<PAL_CYCLE>;
                vicfunc[63] = &VICII::cycle63<PAL_CYCLE>;
            }
            vicfunc[64] = nullptr;
            vicfunc[65] = nullptr;
            break;
            
        case VICII_NTSC_6567_R56A:
            
            if (dmaDebug()) {
                vicfunc[1] = &VICII::cycle1<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[2] = &VICII::cycle2<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[3] = &VICII::cycle3<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[4] = &VICII::cycle4<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[5] = &VICII::cycle5<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[6] = &VICII::cycle6<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[7] = &VICII::cycle7<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[8] = &VICII::cycle8<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[9] = &VICII::cycle9<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[10] = &VICII::cycle10<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[11] = &VICII::cycle11<PAL_CYCLE | DEBUG_CYCLE>;
                vicfunc[55] = &VICII::cycle55<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[57] = &VICII::cycle57<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[58] = &VICII::cycle58<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[59] = &VICII::cycle59<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[60] = &VICII::cycle60<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[61] = &VICII::cycle61<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[62] = &VICII::cycle62<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[63] = &VICII::cycle63<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[64] = &VICII::cycle64<NTSC_CYCLE | DEBUG_CYCLE>;
            } else {
                vicfunc[1] = &VICII::cycle1<PAL_CYCLE>;
                vicfunc[2] = &VICII::cycle2<PAL_CYCLE>;
                vicfunc[3] = &VICII::cycle3<PAL_CYCLE>;
                vicfunc[4] = &VICII::cycle4<PAL_CYCLE>;
                vicfunc[5] = &VICII::cycle5<PAL_CYCLE>;
                vicfunc[6] = &VICII::cycle6<PAL_CYCLE>;
                vicfunc[7] = &VICII::cycle7<PAL_CYCLE>;
                vicfunc[8] = &VICII::cycle8<PAL_CYCLE>;
                vicfunc[9] = &VICII::cycle9<PAL_CYCLE>;
                vicfunc[10] = &VICII::cycle10<PAL_CYCLE>;
                vicfunc[11] = &VICII::cycle11<PAL_CYCLE>;
                vicfunc[55] = &VICII::cycle55<NTSC_CYCLE>;
                vicfunc[57] = &VICII::cycle57<NTSC_CYCLE>;
                vicfunc[58] = &VICII::cycle58<NTSC_CYCLE>;
                vicfunc[59] = &VICII::cycle59<NTSC_CYCLE>;
                vicfunc[60] = &VICII::cycle60<NTSC_CYCLE>;
                vicfunc[61] = &VICII::cycle61<NTSC_CYCLE>;
                vicfunc[62] = &VICII::cycle62<NTSC_CYCLE>;
                vicfunc[63] = &VICII::cycle63<NTSC_CYCLE>;
                vicfunc[64] = &VICII::cycle64<NTSC_CYCLE>;
            }
            vicfunc[65] = nullptr;
            break;
            
        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
            
            if (dmaDebug()) {
                vicfunc[1] = &VICII::cycle1<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[2] = &VICII::cycle2<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[3] = &VICII::cycle3<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[4] = &VICII::cycle4<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[5] = &VICII::cycle5<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[6] = &VICII::cycle6<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[7] = &VICII::cycle7<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[8] = &VICII::cycle8<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[9] = &VICII::cycle9<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[10] = &VICII::cycle10<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[11] = &VICII::cycle11<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[55] = &VICII::cycle55<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[57] = &VICII::cycle57<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[58] = &VICII::cycle58<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[59] = &VICII::cycle59<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[60] = &VICII::cycle60<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[61] = &VICII::cycle61<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[62] = &VICII::cycle62<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[63] = &VICII::cycle63<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[64] = &VICII::cycle64<NTSC_CYCLE | DEBUG_CYCLE>;
                vicfunc[65] = &VICII::cycle65<NTSC_CYCLE | DEBUG_CYCLE>;
            } else {
                vicfunc[1] = &VICII::cycle1<NTSC_CYCLE>;
                vicfunc[2] = &VICII::cycle2<NTSC_CYCLE>;
                vicfunc[3] = &VICII::cycle3<NTSC_CYCLE>;
                vicfunc[4] = &VICII::cycle4<NTSC_CYCLE>;
                vicfunc[5] = &VICII::cycle5<NTSC_CYCLE>;
                vicfunc[6] = &VICII::cycle6<NTSC_CYCLE>;
                vicfunc[7] = &VICII::cycle7<NTSC_CYCLE>;
                vicfunc[8] = &VICII::cycle8<NTSC_CYCLE>;
                vicfunc[9] = &VICII::cycle9<NTSC_CYCLE>;
                vicfunc[10] = &VICII::cycle10<NTSC_CYCLE>;
                vicfunc[11] = &VICII::cycle11<NTSC_CYCLE>;
                vicfunc[55] = &VICII::cycle55<NTSC_CYCLE>;
                vicfunc[57] = &VICII::cycle57<NTSC_CYCLE>;
                vicfunc[58] = &VICII::cycle58<NTSC_CYCLE>;
                vicfunc[59] = &VICII::cycle59<NTSC_CYCLE>;
                vicfunc[60] = &VICII::cycle60<NTSC_CYCLE>;
                vicfunc[61] = &VICII::cycle61<NTSC_CYCLE>;
                vicfunc[62] = &VICII::cycle62<NTSC_CYCLE>;
                vicfunc[63] = &VICII::cycle63<NTSC_CYCLE>;
                vicfunc[64] = &VICII::cycle64<NTSC_CYCLE>;
                vicfunc[65] = &VICII::cycle65<NTSC_CYCLE>;
            }
            break;
            
        default:
            assert(false);
    }}
