/*
 * This file belongs to the FastSID implementation of VirtualC64,
 * an adaption of the code used in VICE 3.1, the Versatile Commodore Emulator.
 *
 * Original code written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Michael Schwendt <sidplay@geocities.com>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *
 * Adapted for VirtualC64 by
 *  Dirk Hoffmann
 */
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "FastSID.h"
#include "waves.h"

u16 FastVoice::wavetable10[2][4096];
u16 FastVoice::wavetable20[2][4096];
u16 FastVoice::wavetable30[2][4096];
u16 FastVoice::wavetable40[2][8192];
u16 FastVoice::wavetable50[2][8192];
u16 FastVoice::wavetable60[2][8192];
u16 FastVoice::wavetable70[2][8192];
u8 FastVoice::noiseMSB[256];
u8 FastVoice::noiseMID[256];
u8 FastVoice::noiseLSB[256];


// Table for pseudo-exponential ADSR calculations
static u32 exptable[6] =
{
    0x30000000, 0x1c000000, 0x0e000000, 0x08000000, 0x04000000, 0x00000000
};

FastVoice::FastVoice()
{
    setDescription("Voice");
    debug(3, "  Creating Voice at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        { &waveTableOffset,  sizeof(waveTableOffset),  CLEAR_ON_RESET },
        { &waveTableCounter, sizeof(waveTableCounter), CLEAR_ON_RESET },
        { &step,             sizeof(step),             CLEAR_ON_RESET },
        { &ringmod,          sizeof(ringmod),          CLEAR_ON_RESET },
        { &adsrm,            sizeof(adsrm),            CLEAR_ON_RESET },
        { &adsr,             sizeof(adsr),             CLEAR_ON_RESET },
        { &adsrInc,          sizeof(adsrInc),          CLEAR_ON_RESET },
        { &adsrCmp,          sizeof(adsrCmp),          CLEAR_ON_RESET },
        { &lsfr,             sizeof(lsfr),             CLEAR_ON_RESET },
        { &filterIO,         sizeof(filterIO),         CLEAR_ON_RESET },
        { &filterType,       sizeof(filterType),       CLEAR_ON_RESET },
        { &filterLow,        sizeof(filterLow),        CLEAR_ON_RESET },
        { &filterRef,        sizeof(filterRef),        CLEAR_ON_RESET },
        { &filterDy,         sizeof(filterDy),         CLEAR_ON_RESET },
        { &filterResDy,      sizeof(filterResDy),      CLEAR_ON_RESET },
        { NULL,              0,                        0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

FastVoice::~FastVoice()
{
    debug(3, "  Releasing FastVoice %d...\n", nr);
}

void
FastVoice::reset()
{
    HardwareComponent::reset();
    updateWaveTablePtr(); 
    lsfr = NSEED;
}

void
FastVoice::initWaveTables()
{
    // Most tables are the same for SID6581 and SID8580, so let's initialize both.
    for (unsigned m = 0; m < 2; m++) {
        for (unsigned i = 0; i < 4096; i++) {
            wavetable10[m][i] = (u16)(i < 2048 ? i << 4 : 0xffff - (i << 4));
            wavetable20[m][i] = (u16)(i << 3);
            wavetable30[m][i] = waveform30_8580[i] << 7;
            wavetable40[m][i + 4096] = 0x7fff;
            wavetable50[m][i + 4096] = waveform50_6581[i >> 3] << 7;
            wavetable60[m][i + 4096] = 0;
            wavetable70[m][i + 4096] = 0;
        }
    }
    
    // Modify some tables for SID8580
    for (unsigned i = 0; i < 4096; i++) {
        wavetable50[1][i + 4096] = waveform50_8580[i] << 7;
        wavetable60[1][i + 4096] = waveform60_8580[i] << 7;
        wavetable70[1][i + 4096] = waveform70_8580[i] << 7;
    }
    
    // Noise tables are the same for both SID models
    for (unsigned i = 0; i < 256; i++) {
        noiseLSB[i] = (u8)((((i >> (7 - 2)) & 0x04) | ((i >> (4 - 1)) & 0x02)
                                 | ((i >> (2 - 0)) & 0x01)));
        noiseMID[i] = (u8)((((i >> (13 - 8 - 4)) & 0x10)
                                 | ((i << (3 - (11 - 8))) & 0x08)));
        noiseMSB[i] = (u8)((((i << (7 - (22 - 16))) & 0x80)
                                 | ((i << (6 - (20 - 16))) & 0x40)
                                 | ((i << (5 - (16 - 16))) & 0x20)));
    }
}

void
FastVoice::init(FastSID *owner, unsigned voiceNr, FastVoice *prevVoice)
{
    assert(prevVoice != NULL);
    
    nr = voiceNr;
    fastsid = owner; 
    prev = prevVoice;
    sidreg = owner->sidreg + (voiceNr * 7);
}

void
FastVoice::updateWaveTablePtr()
{
    SIDModel chipModel = fastsid->getModel(); 
    assert(chipModel == MOS_6581 || chipModel == MOS_8580);
    
    unsigned offset;
    switch (waveform()) {
            
        case FASTSID_TRIANGLE:
            wavetable = wavetable10[chipModel];
            break;
            
        case FASTSID_SAW:
            wavetable = wavetable20[chipModel];
            break;
            
        case FASTSID_SAW | FASTSID_TRIANGLE:
            wavetable = wavetable30[chipModel];
            break;
            
        case FASTSID_PULSE:
            offset = testBit() ? 0 : pulseWidth();
            wavetable = wavetable40[chipModel] + (4096 - offset);
            break;
            
        case FASTSID_PULSE | FASTSID_TRIANGLE:
            offset = 4096 - pulseWidth();
            wavetable = wavetable50[chipModel] + offset;
            break;
            
        case FASTSID_PULSE | FASTSID_SAW:
            offset = 4096 - pulseWidth();
            wavetable = wavetable60[chipModel] + offset;
            break;
            
        case FASTSID_PULSE | FASTSID_SAW | FASTSID_TRIANGLE:
            offset = 4096 - pulseWidth();
            wavetable = wavetable70[chipModel] + offset;
            break;
            
        default:
            wavetable = NULL;
    }
}

void
FastVoice::updateInternals(bool gateBitFlipped)
{
    updateWaveTablePtr();
    
    if (testBit()) {
        waveTableCounter = 0;
        step = 0;
        lsfr = NSEED;
    } else {
        step = fastsid->speed1 * frequency();
    }
    
    unsigned offset;
    switch (waveform()) {
         
        case 0:
            ringmod = false;
            break;

        case FASTSID_TRIANGLE:
            waveTableOffset = 0;
            ringmod = ringModBit();
            break;
            
        case FASTSID_SAW:
            waveTableOffset = 0;
            ringmod = false;
            break;

        case FASTSID_SAW | FASTSID_TRIANGLE:
            waveTableOffset = 0;
            ringmod = ringModBit();
            break;
            
        case FASTSID_PULSE:
            offset = testBit() ? 0 : pulseWidth();
            waveTableOffset = 0;
            ringmod = false;
            break;
        
        case FASTSID_PULSE | FASTSID_TRIANGLE:
            offset = 4096 - pulseWidth();
            waveTableOffset = offset << 20;
            ringmod = ringModBit();
            break;

        case FASTSID_PULSE | FASTSID_SAW:
            offset = 4096 - pulseWidth();
            waveTableOffset = offset << 20;
            ringmod = false;
            break;

        case FASTSID_PULSE | FASTSID_SAW | FASTSID_TRIANGLE:
            offset = 4096 - pulseWidth();
            waveTableOffset = offset << 20;
            ringmod = ringModBit();
            break;
            
        case FASTSID_NOISE:
            ringmod = false;
            break;
            
        default:
            lsfr = 0;
            ringmod = false;
    }
    
    switch (adsrm) {
            
        case FASTSID_ATTACK:
        case FASTSID_DECAY:
        case FASTSID_SUSTAIN:
            
            if (gateBit()) {
                
                // Initiate attack phase
                set_adsr((u8)(gateBitFlipped ? FASTSID_ATTACK : adsrm));
            } else {
                
                // Proceed immediately to release phase
                set_adsr(FASTSID_RELEASE);
            }
            break;
            
        case FASTSID_RELEASE:
        case FASTSID_IDLE:
            
            if (gateBit()) {
                set_adsr(FASTSID_ATTACK);
            } else {
                set_adsr(adsrm);
            }
            break;
    }
}

void
FastVoice::setFilterType(u8 type)
{
    if (filterType == type)
        return;
    
    filterType = type;
    filterLow = 0.0;
    filterRef = 0.0;
}

void
FastVoice::set_adsr(u8 phase)
{
    int i;
    
    adsrm = phase;
    
    switch (phase) {
            
        case FASTSID_ATTACK:
            adsrInc = fastsid->adrs[attackRate()];
            adsrCmp = 0;
            return;
            
        case FASTSID_DECAY:
            if (adsr <= fastsid->sz[sustainRate()]) {
                set_adsr(FASTSID_SUSTAIN);
            } else {
                for (i = 0; adsr < exptable[i]; i++) {}
                adsrInc = -fastsid->adrs[decayRate()] >> i;
                adsrCmp = fastsid->sz[sustainRate()];
                if (exptable[i] > adsrCmp) {
                    adsrCmp = exptable[i];
                }
            }
            return;
            
        case FASTSID_SUSTAIN:
            if (adsr > fastsid->sz[sustainRate()]) {
                set_adsr(FASTSID_DECAY);
            } else {
                adsrInc = 0;
                adsrCmp = 0;
            }
            return;

        case FASTSID_RELEASE:
            if (!adsr) {
                set_adsr(FASTSID_IDLE);
            } else {
                for (i = 0; adsr < exptable[i]; i++) {}
                adsrInc = -fastsid->adrs[releaseRate()] >> i;
                adsrCmp = exptable[i];
            }
            return;
            
        default:
            assert(phase == FASTSID_IDLE);
            adsrInc = 0;
            adsrCmp = 0;
            return;
    }
}

void
FastVoice::trigger_adsr()
{
    switch (adsrm) {
            
        case FASTSID_ATTACK:
            adsr = 0x7fffffff;
            set_adsr(FASTSID_DECAY);
            break;
            
        case FASTSID_DECAY:
        case FASTSID_RELEASE:
            if (adsr >= 0x80000000) {
                adsr = 0;
            }
            set_adsr(adsrm);
            break;
    }
}

u32
FastVoice::doosc()
{
    if (waveform() == FASTSID_NOISE) {
        return ((u32)NVALUE(NSHIFT(lsfr, waveTableCounter >> 28))) << 7;
    }
    
    if (wavetable) {
        u32 index = (waveTableCounter + waveTableOffset) >> 20; /* 12 bit value */
        if (ringmod) {
            if ((prev->waveTableCounter >> 31) == 1) {
                return wavetable[index] ^ 0x7FFF;
            }
        }
        return wavetable[index];
    }
    
    return 0;
}

void
FastVoice::applyFilter()
{
    float sample, sample2;
    
    if (filterType == 0) {
        filterIO = 0;
        return;
    }
    
    if (filterType == FASTSID_BAND_PASS) {
        filterLow += filterRef * filterDy;
        filterRef += (filterIO - filterLow - (filterRef * filterResDy)) * filterDy;
        filterIO = (signed char)(filterRef - filterLow / 4);
        return;
    }
    
    if (filterType == FASTSID_HIGH_PASS) {
        filterLow += filterRef * filterDy * 0.1;
        filterRef += (filterIO - filterLow - (filterRef * filterResDy)) * filterDy;
        sample = filterRef - (filterIO / 8);
        sample = MAX(sample, -128);
        sample = MIN(sample, 127);
        filterIO = (signed char)sample;
        return;
    }
    
    filterLow += filterRef * filterDy;
    sample = filterIO;
    sample2 = sample - filterLow;
    int tmp = (int)sample2;
    sample2 -= filterRef * filterResDy;
    filterRef += sample2 * filterDy;
    
    switch (filterType) {
            
        case FASTSID_LOW_PASS:
        case FASTSID_BAND_PASS | FASTSID_LOW_PASS:
            filterIO = (signed char)filterLow;
            break;
            
        case FASTSID_HIGH_PASS | FASTSID_LOW_PASS:
        case FASTSID_HIGH_PASS | FASTSID_BAND_PASS | FASTSID_LOW_PASS:
            filterIO = (signed char)((int)(sample) - (tmp >> 1));
            break;
            
        case FASTSID_HIGH_PASS | FASTSID_BAND_PASS:
            filterIO = (signed char)tmp;
            break;
            
        default:
            assert(false); 
            filterIO = 0;
    }
}
