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

#include "C64.h"

FastSID::FastSID()
{
	setDescription("SID");
	debug(3, "  Creating FastSID at address %p...\n", this);
    
    // Initialize wave and noise tables
    Voice::initWaveTables();
    
    init(44100, PAL_CYCLES_PER_SECOND);
    
    
    // Set default values
    // setChipModel(reSID::MOS6581);
    chipModel = MOS_6581;
    
    /*
    cpuFrequency = PAL_CYCLES_PER_FRAME * PAL_REFRESH_RATE;
    samplingMethod = reSID::SAMPLE_FAST;
    sampleRate = 44100;
     */
    // sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate);
    // setAudioFilter(false);
    emulateFilter = true;
    
    volume = 100000;
    targetVolume = 100000;
}

FastSID::~FastSID()
{

}

//! Bring the SID chip back to it's initial state.
void
FastSID::reset()
{
    
}

//! Load state
void
FastSID::loadFromBuffer(uint8_t **buffer)
{
    
}

//! Save state
void
FastSID::saveToBuffer(uint8_t **buffer)
{
    
}

//! Dump internal state to console
void
FastSID::dumpState()
{
    
}

//! Special peek function for the I/O memory range.
uint8_t
FastSID::peek(uint16_t addr)
{
    switch (addr) {
            
        case 0x19: // POTX
        case 0x1A: // POTY

            return 0xFF;
  
        case 0x1B: // OSC 3/RANDOM
            
            // This register allows the microprocessor to read the
            // upper 8 output bits of oscillator 3.
            return (uint8_t)(voice[2].doosc() >> 7);

        case 0x1C:
            
            // This register allows the microprocessor to read the
            // output of the voice 3 envelope generator.
            return (uint8_t)(voice[2].adsr >> 23);
            
        default:
            
            return st.laststore;
    }
}

//! Special poke function for the I/O memory range.
void
FastSID::poke(uint16_t addr, uint8_t value)
{
    switch (addr) {
 
        case 0x00: // Voice 1 registers
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
            voice[0].gateflip = (st.d[0x04] ^ value) & 1;
            // Fallthrough
        case 0x05:
        case 0x06:
            voice[0].updateInternals();
            break;
  
        case 0x07: // Voice 2 registers
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
            voice[1].gateflip = (st.d[0x0B] ^ value) & 1;
            // Fallthrough
        case 0x0C:
        case 0x0D:
            voice[1].updateInternals();
            break;

        case 0x0E: // Voice 3 registers
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
            voice[2].gateflip = (st.d[0x12] ^ value) & 1;
            // Fallthrough
        case 0x13:
        case 0x14:
            voice[2].updateInternals();
            break;
            
        default: // Voice independent registers
            isDirty = true;
    }

    st.d[addr] = value;
    st.laststore = value;
}

/*! @brief   Execute SID
 *  @details Runs reSID for the specified amount of CPU cycles and writes
 *           the generated sound samples into the internal ring buffer.
 */
void
FastSID::execute(uint64_t cycles)
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
FastSID::init(int sampleRate, int cycles_per_sec)
{
    uint32_t i;
    
    // Table for internal ADSR counter step calculations
    uint16_t adrtable[16] = {
        1, 4, 8, 12, 19, 28, 34, 40, 50, 125, 250, 400, 500, 1500, 2500, 4000
    };
    
    st.speed1 = (cycles_per_sec << 8) / sampleRate;
    for (i = 0; i < 16; i++) {
        adrs[i] = 500 * 8 * st.speed1 / adrtable[i];
        sz[i] = 0x8888888 * i;
    }
    
    /*
     if (resources_get_int("SidFilters", &(psid->emulatefilter)) < 0) {
     return 0;
     }
     */
    
    initFilter(sampleRate);
    
    isDirty = true;
    prepare();
    
    /*
     if (resources_get_int("SidModel", &sid_model) < 0) {
     return 0;
     }
     */
    
    // Voices
    voice[0].init(this, 0, &voice[3]);
    voice[1].init(this, 1, &voice[0]);
    voice[2].init(this, 2, &voice[1]);
    
    return 1;
}

void
FastSID::initFilter(int sampleRate)
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
    
    // Low pass lookup table
    for (uk = 0, rk = 0; rk < 0x800; rk++, uk++) {
        
        float h = (float)((((exp(rk / 2048 * log(filterFs)) / filterFm) + filterFt) * filterRefFreq) / sampleRate);
        if (h < yMin) {
            h = yMin;
        }
        if (h > yMax) {
            h = yMax;
        }
        lowPassParam[uk] = h;
    }
    
    // Band pass lookup table
    yMax = (float)0.22;
    yMin = (float)0.002;
    yAdd = (float)((yMax - yMin) / 2048.0);
    yTmp = yMin;
    for (uk = 0, rk = 0; rk < 0x800; rk++, uk++) {
        bandPassParam[uk] = (yTmp * filterRefFreq) / sampleRate;
        yTmp += yAdd;
    }
    
    // Resonance lookup table
    for (uk = 0; uk < 16; uk++) {
        filterResTable[uk] = resDy;
        resDy -= ((resDyMin - resDyMax ) / 15);
    }
    filterResTable[0] = resDyMin;
    filterResTable[15] = resDyMax;
    filterAmpl = emulateFilter ? 0.7 : 1.0;
    
    // Amplifier lookup table
    for (uk = 0, si = 0; si < 256; si++, uk++) {
        ampMod1x8[uk] = (signed char)((si - 0x80) * filterAmpl);
    }
}

void
FastSID::prepare()
{
    assert((st.d[0x18] & 0x70) == filterType());
    assert (filterCutoff() == (0x7ff & ((st.d[0x15] & 7) | ((uint16_t)st.d[0x16]) << 3)));
    
    if (isDirty && emulateFilter) {
        st.filterType = filterType();
        if (st.filterType != st.filterCurType) {
            st.filterCurType = st.filterType;
            voice[0].filtLow = 0;
            voice[0].filtRef = 0;
            voice[1].filtLow = 0;
            voice[1].filtRef = 0;
            voice[2].filtLow = 0;
            voice[2].filtRef = 0;
        }
        
        //st.filterValue = 0x7ff & ((st.d[0x15] & 7) | ((uint16_t)st.d[0x16]) << 3);
        st.filterValue = filterCutoff();
        
        assert((st.filterType == 0x20) == (filterType() == FASTSID_BAND_PASS));
        
        // if (st.filterType == 0x20) {
        if (filterType() == FASTSID_BAND_PASS) {
            st.filterDy = bandPassParam[st.filterValue];
        } else {
            st.filterDy = lowPassParam[st.filterValue];
        }
        assert((st.d[0x17] >> 4) == (filterResonance()));
        // st.filterResDy = filterResTable[st.d[0x17] >> 4] - st.filterDy;
        st.filterResDy = filterResTable[filterResonance()] - st.filterDy;
        st.filterResDy = MAX(st.filterResDy, 1.0);
        assert(st.filterResDy >= 1.0);
    }
    
    isDirty = false;
}

int16_t
FastSID::fastsid_calculate_single_sample()
{
    uint32_t osc0, osc1, osc2;
    Voice *v0 = &voice[0];
    Voice *v1 = &voice[1];
    Voice *v2 = &voice[2];
    bool sync0 = false;
    bool sync1 = false;
    bool sync2 = false;

    prepare();
    
    // Advance wavetable counters
    v0->counter += v0->step;
    v1->counter += v1->step;
    v2->counter += v2->step;
    
    // Check for counter overflows (waveform loops)
    if (v0->counter < v0->step) {
        v0->lsfr = NSHIFT(v0->lsfr, 16);
        sync1 = v1->syncBit();
    }
    if (v1->counter < v1->step) {
        v1->lsfr = NSHIFT(v1->lsfr, 16);
        sync2 = v2->syncBit();
    }
    if (v2->counter < v2->step) {
        v2->lsfr = NSHIFT(v2->lsfr, 16);
        sync0 = v0->syncBit();
    }
    
    // Perform hard sync
    if (sync0) {
        v0->lsfr = NSHIFT(v0->lsfr, v0->counter >> 28);
        v0->counter = 0;
    }
    if (sync1) {
        v1->lsfr = NSHIFT(v1->lsfr, v1->counter >> 28);
        v1->counter = 0;
    }
    if (sync2) {
        v2->lsfr = NSHIFT(v2->lsfr, v2->counter >> 28);
        v2->counter = 0;
    }
    
    // Advance ADSR counters
    v0->adsr += v0->adsrInc;
    v1->adsr += v1->adsrInc;
    v2->adsr += v2->adsrInc;
    
    // Check if we need to perform state changes
    if (v0->adsr + 0x80000000 < v0->adsrCmp + 0x80000000) {
        v0->trigger_adsr();
    }
    if (v1->adsr + 0x80000000 < v1->adsrCmp + 0x80000000) {
        v1->trigger_adsr();
    }
    if (v2->adsr + 0x80000000 < v2->adsrCmp + 0x80000000) {
        v2->trigger_adsr();
    }
    
    // Oscillators
    osc0 = (v0->adsr >> 16) * v0->doosc();
    osc1 = (v1->adsr >> 16) * v1->doosc();
    osc2 = (v2->adsr >> 16) * v2->doosc();
    
    // Silence voice 3 if it is disconnected from the output
    if (voiceThreeDisconnected()) {
        osc2 = 0;
    }
    
    // Apply filter
    if (emulateFilter) {
        v0->filtIO = ampMod1x8[(osc0 >> 22)];
        if (filterOn(0)) v0->applyFilter();
        osc0 = ((uint32_t)(v0->filtIO) + 0x80) << (7 + 15);
        
        v1->filtIO = ampMod1x8[(osc1 >> 22)];
        if (filterOn(1)) v1->applyFilter();
        osc1 = ((uint32_t)(v1->filtIO) + 0x80) << (7 + 15);
        
        v2->filtIO = ampMod1x8[(osc2 >> 22)];
        if (filterOn(2)) v2->applyFilter();
        osc2 = ((uint32_t)(v2->filtIO) + 0x80) << (7 + 15);
    }
    
    return (int16_t)(((int32_t)((osc0 + osc1 + osc2) >> 20) - 0x600) * sidVolume());
}
