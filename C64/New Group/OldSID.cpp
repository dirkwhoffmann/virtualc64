/*
 * (C) 2006 Dirk W. Hoffmann, Jérôme Lang. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

OldSID::OldSID()
{
	setDescription("SID");
	debug(3, "  Creating FastSID at address %p...\n", this);
    
    // Initialize wave and noise tables
    Voice::initWaveTables();
    
    init(44100, PAL_CYCLES_PER_SECOND);
    
    
    // Set default values
    // setChipModel(reSID::MOS6581);
    st.newsid = false; 
    
    /*
    cpuFrequency = PAL_CYCLES_PER_FRAME * PAL_REFRESH_RATE;
    samplingMethod = reSID::SAMPLE_FAST;
    sampleRate = 44100;
     */
    // sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate);
    setAudioFilter(false);
    
    volume = 100000;
    targetVolume = 100000;
}

OldSID::~OldSID()
{

}

//! Bring the SID chip back to it's initial state.
void
OldSID::reset()
{
    
}

//! Load state
void
OldSID::loadFromBuffer(uint8_t **buffer)
{
    
}

//! Save state
void
OldSID::saveToBuffer(uint8_t **buffer)
{
    
}

//! Dump internal state to console
void
OldSID::dumpState()
{
    
}

//! Special peek function for the I/O memory range.
uint8_t
OldSID::peek(uint16_t addr)
{
    switch (addr) {
            
        case 0x19: // POTX
        case 0x1A: // POTY

            return 0xFF;
  
        case 0x1B: { // OSC 3/RANDOM
            
            // This register allows the microprocessor to read the
            // upper 8 output bits of oscillator 3.
            
            uint16_t ffix = (uint16_t)(42 * st.v[2].vt.fs);
            uint32_t rvstore = st.v[2].vt.rv;
            if (st.v[2].noise && st.v[2].vt.f + ffix < st.v[2].vt.f) {
                st.v[2].vt.rv = NSHIFT(st.v[2].vt.rv, 16);
            }
            st.v[2].vt.f += ffix;
            uint8_t ret = (uint8_t)(st.v[2].doosc() >> 7);
            st.v[2].vt.f -= ffix;
            st.v[2].vt.rv = rvstore;
            return ret;
        }
            // return (uint8_t)(st.v[2].doosc() >> 7);
            
        case 0x1C:
            
            // This register allows the microprocessor to read the
            // output of the voice 3 envelope generator.
            return (uint8_t)(st.v[2].vt.adsr >> 23);
            
        default:
            /*
             while ((tmp = psid->laststorebit) &&
             (tmp = psid->laststoreclk + sidreadclocks[tmp]) < maincpu_clk) {
             psid->laststoreclk = tmp;
             psid->laststore &= 0xfeff >> psid->laststorebit--;
             }
             */
            return st.laststore;
    }
}

//! Special poke function for the I/O memory range.
void
OldSID::poke(uint16_t addr, uint8_t value)
{
    switch (addr) {
 
        case 0: // VOICE 1
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            st.v[0].vt.update = 1;
            break;
  
        case 7: // VOICE 2
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            st.v[1].vt.update = 1;
            break;

        case 14: // VOICE 3
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            st.v[2].vt.update = 1;
            break;
            
        default: // GENERAL SID
            st.update = 1;
    }
    
    // For each voice, check if gate bit flips
    if (addr == 0x04 && ((st.d[addr] ^ value) & 1))
        st.v[0].vt.gateflip = 1;
    if (addr == 0x0B && ((st.d[addr] ^ value) & 1))
        st.v[1].vt.gateflip = 1;
    if (addr == 0x12 && ((st.d[addr] ^ value) & 1))
        st.v[2].vt.gateflip = 1;

    st.d[addr] = value;
    st.laststore = value;
    // st.laststorebit = 8;
}

/*! @brief   Execute SID
 *  @details Runs reSID for the specified amount of CPU cycles and writes
 *           the generated sound samples into the internal ring buffer.
 */
void
OldSID::execute(uint64_t cycles)
{
    int16_t buf[2049];
    int buflength = 2048;
    
    // debug("executing for %d cycles\n", cycles);
    
    // How many samples do we need?
    // For now, we assume a sample rate of 44100
    int numSamples = (int)(cycles * 44100 / PAL_CYCLES_PER_SECOND);
    
    if (numSamples > buflength) {
        debug("Number of samples exceeds buffer size\n");
        numSamples = buflength;
    }
    
    // Compute samples
    for (unsigned i = 0; i < numSamples; i++) {
        buf[i] = fastsid_calculate_single_sample();
    }
    
    // Write samples into ringbuffer
    writeData(buf, numSamples);
}

int
OldSID::init(int sampleRate, int cycles_per_sec)
{
    uint32_t i;
    int sid_model;
    
    // Table for internal ADSR counter step calculations
    uint16_t adrtable[16] = {
        1, 4, 8, 12, 19, 28, 34, 40, 50, 125, 250, 400, 500, 1500, 2500, 4000
    };
    
    st.speed1 = (cycles_per_sec << 8) / sampleRate;
    for (i = 0; i < 16; i++) {
        st.adrs[i] = 500 * 8 * st.speed1 / adrtable[i];
        st.sz[i] = 0x8888888 * i;
    }
    st.update = 1;
    
    /*
     if (resources_get_int("SidFilters", &(psid->emulatefilter)) < 0) {
     return 0;
     }
     */
    
    init_filter(sampleRate);
    prepare();
    
    /*
     if (resources_get_int("SidModel", &sid_model) < 0) {
     return 0;
     }
     */
    sid_model = 0;
    st.newsid = 0;
    
    // Voices
    for (i = 0; i < 3; i++) {
        st.v[i].init(&st, i);
        st.v[i].prepare();
    }
    
    switch (sid_model) {
        default:
        case 0: /* 6581 */
        case 3: /* 6581R4 */
        case 4: /* DTVSID */
            st.newsid = 0;
            break;
        case 1: /* 8580 */
        case 2: /* 8580 + digi boost */
            st.newsid = 1;
            break;
    }
    
    /*
    for (i = 0; i < 9; i++) {
        sidreadclocks[i] = 13;
    }
    */
    
    return 1;
}

void
OldSID::init_filter(int sampleRate)
{
    uint16_t uk;
    float rk;
    long int si;
    
    const float filterRefFreq = 44100.0;
    
    float yMax = 1.0;
    float yMin = (float)0.01;
    float resDyMax = 1.0;
    float resDyMin = 2.0;
    float resDy = resDyMin;
    
    float yAdd, yTmp;
    
    float filterFs = 400.0;
    float filterFm = 60.0;
    float filterFt = (float)0.05;
    
    float filterAmpl = 1.0;
    
    st.filterValue = 0;
    st.filterType = 0;
    st.filterCurType = 0;
    st.filterDy = 0;
    st.filterResDy = 0;
    
    for (uk = 0, rk = 0; rk < 0x800; rk++, uk++) {
        float h;
        
        h = (float)((((exp(rk / 2048 * log(filterFs)) / filterFm) + filterFt) * filterRefFreq) / sampleRate);
        if (h < yMin) {
            h = yMin;
        }
        if (h > yMax) {
            h = yMax;
        }
        lowPassParam[uk] = h;
    }
    
    yMax = (float)0.22;
    yMin = (float)0.002;
    yAdd = (float)((yMax - yMin) / 2048.0);
    yTmp = yMin;
    
    for (uk = 0, rk = 0; rk < 0x800; rk++, uk++) {
        bandPassParam[uk] = (yTmp * filterRefFreq) / sampleRate;
        yTmp += yAdd;
    }
    
    for (uk = 0; uk < 16; uk++) {
        filterResTable[uk] = resDy;
        resDy -= ((resDyMin - resDyMax ) / 15);
    }
    
    filterResTable[0] = resDyMin;
    filterResTable[15] = resDyMax;
    
    /* XXX: if psid->emulatefilter = 0, ampMod1x8 is never referenced */
    if (st.emulatefilter) {
        filterAmpl = (float)0.7;
    } else {
        filterAmpl = (float)1.0;
    }
    
    for (uk = 0, si = 0; si < 256; si++, uk++) {
        ampMod1x8[uk] = (signed char)((si - 0x80) * filterAmpl);
    }
}

void
OldSID::prepare()
{
    if (!st.update) {
        return;
    }
    
    st.has3 = ((st.d[0x18] & 0x80) && !(st.d[0x17] & 0x04)) ? 0 : 1;
    
    if (st.emulatefilter) {
        st.v[0].vt.filter = st.d[0x17] & 0x01 ? 1 : 0;
        st.v[1].vt.filter = st.d[0x17] & 0x02 ? 1 : 0;
        st.v[2].vt.filter = st.d[0x17] & 0x04 ? 1 : 0;
        st.filterType = st.d[0x18] & 0x70;
        if (st.filterType != st.filterCurType) {
            st.filterCurType = st.filterType;
            st.v[0].vt.filtLow = 0;
            st.v[0].vt.filtRef = 0;
            st.v[1].vt.filtLow = 0;
            st.v[1].vt.filtRef = 0;
            st.v[2].vt.filtLow = 0;
            st.v[2].vt.filtRef = 0;
        }
        st.filterValue = 0x7ff & ((st.d[0x15] & 7) | ((uint16_t)st.d[0x16]) << 3);
        if (st.filterType == 0x20) {
            st.filterDy = bandPassParam[st.filterValue];
        } else {
            st.filterDy = lowPassParam[st.filterValue];
        }
        st.filterResDy = filterResTable[st.d[0x17] >> 4]
        - st.filterDy;
        if (st.filterResDy < 1.0) {
            st.filterResDy = 1.0;
        }
    } else {
        st.v[0].vt.filter = 0;
        st.v[1].vt.filter = 0;
        st.v[2].vt.filter = 0;
    }
    st.update = 0;
}

int16_t
OldSID::fastsid_calculate_single_sample()
{
    uint32_t o0, o1, o2;
    uint32_t osc0, osc1, osc2;
    Voice *v0 = &st.v[0];
    Voice *v1 = &st.v[1];
    Voice *v2 = &st.v[2];
    
    prepare();
    v0->prepare();
    v1->prepare();
    v2->prepare();
    
    // addfptrs, noise
    if ((v0->vt.f += v0->vt.fs) < v0->vt.fs) {
        v0->vt.rv = NSHIFT(v0->vt.rv, 16);
    }
    if ((v1->vt.f += v1->vt.fs) < v1->vt.fs) {
        v1->vt.rv = NSHIFT(v1->vt.rv, 16);
    }
    if ((v2->vt.f += v2->vt.fs) < v2->vt.fs) {
        v2->vt.rv = NSHIFT(v2->vt.rv, 16);
    }
    
    // Hard sync
    if (v0->hardSync()) {
        v0->vt.rv = NSHIFT(v0->vt.rv, v0->vt.f >> 28);
        v0->vt.f = 0;
    }
    if (v2->hardSync()) {
        v2->vt.rv = NSHIFT(v2->vt.rv, v2->vt.f >> 28);
        v2->vt.f = 0;
    }
    if (v1->hardSync()) {
        v1->vt.rv = NSHIFT(v1->vt.rv, v1->vt.f >> 28);
        v1->vt.f = 0;
    }
    
    // Do adsr
    if ((v0->vt.adsr += v0->vt.adsrs) + 0x80000000 < v0->vt.adsrz + 0x80000000) {
        v0->trigger_adsr();
    }
    if ((v1->vt.adsr += v1->vt.adsrs) + 0x80000000 < v1->vt.adsrz + 0x80000000) {
        v1->trigger_adsr();
    }
    if ((v2->vt.adsr += v2->vt.adsrs) + 0x80000000 < v2->vt.adsrz + 0x80000000) {
        v2->trigger_adsr();
    }
    
    // Oscillators
    o0 = v0->vt.adsr >> 16;
    o1 = v1->vt.adsr >> 16;
    o2 = v2->vt.adsr >> 16;
    osc0 = (v0->vt.adsr >> 16) * v0->doosc();
    osc1 = (v1->vt.adsr >> 16) * v1->doosc();
    osc2 = st.has3 ? ((v2->vt.adsr >> 16) * v2->doosc()) : 0;
    
    // Sample
    if (st.emulatefilter) {
        v0->vt.filtIO = ampMod1x8[(o0 >> 22)];
        v0->applyFilter();
        o0 = ((uint32_t)(v0->vt.filtIO) + 0x80) << (7 + 15);
        
        v1->vt.filtIO = ampMod1x8[(o1 >> 22)];
        v1->applyFilter();
        o1 = ((uint32_t)(v1->vt.filtIO) + 0x80) << (7 + 15);
        
        v2->vt.filtIO = ampMod1x8[(o2 >> 22)];
        v2->applyFilter();
        o2 = ((uint32_t)(v2->vt.filtIO) + 0x80) << (7 + 15);
    }
    
    int32_t volume = st.d[0x18] & 0x0F;
    return (int16_t)(((int32_t)((o0 + o1 + o2) >> 20) - 0x600) * volume);
}
