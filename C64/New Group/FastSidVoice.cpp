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

uint16_t Voice::wavetable10[2][4096];
uint16_t Voice::wavetable20[2][4096];
uint16_t Voice::wavetable30[2][4096];
uint16_t Voice::wavetable40[2][8192];
uint16_t Voice::wavetable50[2][8192];
uint16_t Voice::wavetable60[2][8192];
uint16_t Voice::wavetable70[2][8192];
uint8_t Voice::noiseMSB[256];
uint8_t Voice::noiseMID[256];
uint8_t Voice::noiseLSB[256];


// Table for pseudo-exponential ADSR calculations
static uint32_t exptable[6] =
{
    0x30000000, 0x1c000000, 0x0e000000, 0x08000000, 0x04000000, 0x00000000
};

Voice::Voice()
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
        { &filtIO,           sizeof(filtIO),           CLEAR_ON_RESET },
        { &filtLow,          sizeof(filtLow),          CLEAR_ON_RESET },
        { &filtRef,          sizeof(filtRef),          CLEAR_ON_RESET },
        { NULL,              0,                        0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

Voice::~Voice()
{
    debug(3, "  Releasing Voice %d...\n", nr);
}

void
Voice::reset()
{
    VirtualComponent::reset();
    lsfr = NSEED;
}

void
Voice::initWaveTables()
{
    // Most tables are the same for SID6581 and SID8580, so let's initialize both.
    for (unsigned m = 0; m < 2; m++) {
        for (unsigned i = 0; i < 4096; i++) {
            wavetable10[m][i] = (uint16_t)(i < 2048 ? i << 4 : 0xffff - (i << 4));
            wavetable20[m][i] = (uint16_t)(i << 3);
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
        noiseLSB[i] = (uint8_t)((((i >> (7 - 2)) & 0x04) | ((i >> (4 - 1)) & 0x02)
                                 | ((i >> (2 - 0)) & 0x01)));
        noiseMID[i] = (uint8_t)((((i >> (13 - 8 - 4)) & 0x10)
                                 | ((i << (3 - (11 - 8))) & 0x08)));
        noiseMSB[i] = (uint8_t)((((i << (7 - (22 - 16))) & 0x80)
                                 | ((i << (6 - (20 - 16))) & 0x40)
                                 | ((i << (5 - (16 - 16))) & 0x20)));
    }
}

void
Voice::init(FastSID *owner, unsigned voiceNr, Voice *prevVoice)
{
    assert(prevVoice != NULL);
    
    nr = voiceNr;
    fastsid = owner; 
    prev = prevVoice;
    sidreg = owner->st.d + (voiceNr * 7);
}

void
Voice::updateInternals(bool gateBitFlipped)
{
    SIDChipModel chipModel = fastsid->chipModel;
    assert(chipModel == MOS_6581 || chipModel == MOS_8580);
    
    if (testBit()) {
        waveTableCounter = 0;
        step = 0;
        lsfr = NSEED;
    } else {
        step = fastsid->st.speed1 * frequency();
    }
    
    unsigned offset;
    switch (waveform()) {
         
        case 0:
            wavetable = NULL;
            ringmod = false;
            break;

        case FASTSID_TRIANGLE:
            assert(waveform() == 0x10);
            wavetable = wavetable10[chipModel];
            waveTableOffset = 0;
            ringmod = ringModBit();
            break;
            
        case FASTSID_SAW:
            assert(waveform() == 0x20);
            wavetable = wavetable20[chipModel];
            waveTableOffset = 0;
            ringmod = false;
            break;

        case FASTSID_SAW | FASTSID_TRIANGLE:
            assert(waveform() == 0x30);
            wavetable = wavetable30[chipModel];
            waveTableOffset = 0;
            ringmod = ringModBit();
            break;
            
        case FASTSID_PULSE:
            assert(waveform() == 0x40);
            offset = testBit() ? 0 : pulseWidth();
            wavetable = wavetable40[chipModel] + (4096 - offset);
            waveTableOffset = 0;
            ringmod = false;
            break;
        
        case FASTSID_PULSE | FASTSID_TRIANGLE:
            assert(waveform() == 0x50);
            offset = 4096 - pulseWidth();
            wavetable = wavetable50[chipModel] + offset;
            waveTableOffset = offset << 20;
            ringmod = ringModBit();
            break;

        case FASTSID_PULSE | FASTSID_SAW:
            assert(waveform() == 0x60);
            offset = 4096 - pulseWidth();
            wavetable = wavetable60[chipModel] + offset;
            waveTableOffset = offset << 20;
            ringmod = false;
            break;

        case FASTSID_PULSE | FASTSID_SAW | FASTSID_TRIANGLE:
            assert(waveform() == 0x70);
            offset = 4096 - pulseWidth();
            wavetable = wavetable70[chipModel] + offset;
            waveTableOffset = offset << 20;
            ringmod = ringModBit();
            break;
            
        case FASTSID_NOISE:
            assert(waveform() == 0x80);
            wavetable = NULL;
            ringmod = false;
            break;
            
        default:
            lsfr = 0;
            wavetable = NULL;
            ringmod = false;
    }
    
    switch (adsrm) {
            
        case FASTSID_ATTACK:
        case FASTSID_DECAY:
        case FASTSID_SUSTAIN:
            
            if (gateBit()) {
                
                // Initiate attack phase
                set_adsr((uint8_t)(gateBitFlipped ? FASTSID_ATTACK : adsrm));
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
Voice::set_adsr(uint8_t phase)
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
Voice::trigger_adsr()
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

uint32_t
Voice::doosc()
{
    if (waveform() == FASTSID_NOISE) {
        return ((uint32_t)NVALUE(NSHIFT(lsfr, waveTableCounter >> 28))) << 7;
    }
    
    if (wavetable) {
        uint32_t index = (waveTableCounter + waveTableOffset) >> 20; /* 12 bit value */
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
Voice::applyFilter()
{    
    if (fastsid->st.filterType) {
        if (fastsid->st.filterType == 0x20) {
            filtLow += filtRef * fastsid->st.filterDy;
            filtRef +=
            (filtIO - filtLow -
             (filtRef * fastsid->st.filterResDy)) *
            fastsid->st.filterDy;
            filtIO = (signed char)(filtRef - filtLow / 4);
        } else if (fastsid->st.filterType == 0x40) {
            float sample;
            filtLow += (float)((filtRef *
                                        fastsid->st.filterDy) * 0.1);
            filtRef += (filtIO - filtLow -
                                (filtRef * fastsid->st.filterResDy)) *
            fastsid->st.filterDy;
            sample = filtRef - (filtIO / 8);
            if (sample < -128) {
                sample = -128;
            }
            if (sample > 127) {
                sample = 127;
            }
            filtIO = (signed char)sample;
        } else {
            int tmp;
            float sample, sample2;
            filtLow += filtRef * fastsid->st.filterDy;
            sample = filtIO;
            sample2 = sample - filtLow;
            tmp = (int)sample2;
            sample2 -= filtRef * fastsid->st.filterResDy;
            filtRef += sample2 * fastsid->st.filterDy;
            
            filtIO = fastsid->st.filterType == 0x10
            ? (signed char)filtLow :
            (fastsid->st.filterType == 0x30
             ? (signed char)filtLow :
             (fastsid->st.filterType == 0x50
              ? (signed char)
              ((int)(sample) - (tmp >> 1)) :
              (fastsid->st.filterType == 0x60
               ? (signed char)
               tmp :
               (fastsid->st.filterType == 0x70
                ? (signed char)
                ((int)(sample) - (tmp >> 1)) : 0))));
        }
    } else { /* filterType == 0x00 */
        filtIO = 0;
    }
}
