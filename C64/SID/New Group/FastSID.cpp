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
    
    // Register sub components
    VirtualComponent *subcomponents[] = { &voice[0], &voice[1], &voice[2], NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));
    
    // Register snapshot items
    SnapshotItem items[] = {
        { &sidreg,           sizeof(sidreg),           CLEAR_ON_RESET },
        { &speed1,           sizeof(speed1),           CLEAR_ON_RESET },
        { &chipModel,        sizeof(chipModel),        KEEP_ON_RESET },
        { &cpuFrequency,     sizeof(cpuFrequency),     CLEAR_ON_RESET },
        { &sampleRate,       sizeof(sampleRate),       CLEAR_ON_RESET },
        { &emulateFilter,    sizeof(emulateFilter),    CLEAR_ON_RESET },
        { &latchedDataBus,   sizeof(latchedDataBus),   CLEAR_ON_RESET },
        { NULL,              0,                        0 }};
    registerSnapshotItems(items, sizeof(items));
    
    // Initialize wave and noise tables
    Voice::initWaveTables();
    
    // Initialize voices
    voice[0].init(this, 0, &voice[3]);
    voice[1].init(this, 1, &voice[0]);
    voice[2].init(this, 2, &voice[1]);
    
    chipModel = MOS_6581;
    emulateFilter = true;
    
    reset();
}

FastSID::~FastSID()
{
    debug(3, "  Releasing FastSID...\n");
}

void
FastSID::reset()
{
    VirtualComponent::reset();
    
    cpuFrequency = PAL_CYCLES_PER_SECOND;
    sampleRate = 44100;
    emulateFilter = true;
    
    init(sampleRate, cpuFrequency);
}

void
FastSID::loadFromBuffer(uint8_t **buffer)
{
    VirtualComponent::loadFromBuffer(buffer);
}

void
FastSID::dumpState()
{
    SIDInfo info = getInfo();
    
    uint8_t ft = info.filterType;
    msg("        Volume: %d\n", info.volume);
    msg("   Filter type: %s\n",
        (ft == FASTSID_LOW_PASS) ? "LOW PASS" :
        (ft == FASTSID_HIGH_PASS) ? "HIGH PASS" :
        (ft == FASTSID_BAND_PASS) ? "BAND PASS" : "NONE");
    msg("Filter cut off: %d\n\n", info.filterCutoff);

    for (unsigned i = 0; i < 3; i++) {
        
        uint8_t wf = info.voice[i].waveform;
        msg("Voice %d:       Frequency: %d\n", i, info.voice[i].frequency);
        msg("              Pulse width: %d\n", info.voice[i].pulseWidth);
        msg("                 Waveform: %s\n",
            (wf == FASTSID_NOISE) ? "NOISE" :
            (wf == FASTSID_PULSE) ? "PULSE" :
            (wf == FASTSID_SAW) ? "SAW" :
            (wf == FASTSID_TRIANGLE) ? "TRIANGLE" : "NONE");
        msg("          Ring modulation: %s\n", info.voice[i].ringMod ? "yes" : "no");
        msg("                Hard sync: %s\n", info.voice[i].hardSync ? "yes" : "no");
        msg("              Attack rate: %d\n", info.voice[i].attackRate);
        msg("               Decay rate: %d\n", info.voice[i].decayRate);
        msg("             Sustain rate: %d\n", info.voice[i].sustainRate);
        msg("             Release rate: %d\n", info.voice[i].releaseRate);
        msg("             Apply filter: %s\n\n", info.voice[i].filterOn ? "yes" : "no");
    }
}

SIDInfo
FastSID::getInfo()
{
    SIDInfo info;
 
    for (unsigned i = 0; i < 3; i++) {
        info.voice[i].frequency = voice[i].frequency();
        info.voice[i].pulseWidth = voice[i].pulseWidth();
        info.voice[i].waveform = voice[i].waveform();
        info.voice[i].ringMod = voice[i].ringModBit();
        info.voice[i].hardSync = voice[i].syncBit();
        info.voice[i].attackRate = voice[i].attackRate();
        info.voice[i].decayRate = voice[i].decayRate();
        info.voice[i].sustainRate = voice[i].sustainRate();
        info.voice[i].releaseRate = voice[i].releaseRate();
        info.voice[i].filterOn = filterOn(i);
    }
    info.volume = sidVolume();
    info.filterType = filterType();
    info.filterCutoff = filterCutoff();
    
    return info;
}

void
FastSID::setChipModel(SIDChipModel model)
{
    chipModel = model;
    
    // Switch wave tables according to new model
    voice[0].updateWaveTablePtr();
    voice[1].updateWaveTablePtr();
    voice[2].updateWaveTablePtr();
}

void
FastSID::setClockFrequency(uint32_t frequency)
{
    cpuFrequency = frequency;
    
    // Recompute frequency dependent data structures
    init(sampleRate, cpuFrequency);
}

void
FastSID::setSampleRate(uint32_t rate)
{
    sampleRate = rate;
    
    // Recompute sample rate dependent data structures
    init(sampleRate, cpuFrequency);
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
            
            return latchedDataBus;
    }
}

//! Special poke function for the I/O memory range.
void
FastSID::poke(uint16_t addr, uint8_t value)
{
    bool gateBitFlipped = false;
    
    switch (addr) {
            
        // Voice 1 registers
        case 0x04:
            gateBitFlipped = (sidreg[0x04] ^ value) & 1;
            // Fallthrough
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x05:
        case 0x06:
            voice[0].updateInternals(gateBitFlipped);
            break;
  
        // Voice 2 registers
        case 0x0B:
            gateBitFlipped = (sidreg[0x0B] ^ value) & 1;
            // Fallthrough
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0C:
        case 0x0D:
            voice[1].updateInternals(gateBitFlipped);
            break;

        // Voice 3 registers
        case 0x12:
            gateBitFlipped = (sidreg[0x12] ^ value) & 1;
            // Fallthrough
        case 0x0E:
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x13:
        case 0x14:
            voice[2].updateInternals(gateBitFlipped);
            break;
            
        default: // Voice independent registers
            updateInternals();
    }

    sidreg[addr] = value;
    latchedDataBus = value;
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
        buf[i] = calculateSingleSample();
    }
    
    // Write samples into ringbuffer
    bridge->writeData(buf, numSamples);
}

void
FastSID::init(int sampleRate, int cycles_per_sec)
{
    uint32_t i;
    
    // Table for internal ADSR counter step calculations
    uint16_t adrtable[16] = {
        1, 4, 8, 12, 19, 28, 34, 40, 50, 125, 250, 400, 500, 1500, 2500, 4000
    };
    
    speed1 = (cycles_per_sec << 8) / sampleRate;
    for (i = 0; i < 16; i++) {
        adrs[i] = 500 * 8 * speed1 / adrtable[i];
        sz[i] = 0x8888888 * i;
    }
    
    initFilter(sampleRate);
    updateInternals();
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
FastSID::updateInternals()
{
    uint8_t type = filterType();
    uint8_t res = filterResonance();
    uint16_t cutoff = filterCutoff();
    
    for (unsigned i = 0; i < 3; i++) {
    
        voice[i].setFilterType(type);
        
        if (type == FASTSID_BAND_PASS) {
            voice[i].filterDy = bandPassParam[cutoff];
        } else {
            voice[i].filterDy = lowPassParam[cutoff];
        }
        voice[i].filterResDy = MAX(filterResTable[res] - voice[i].filterDy, 1.0);
    }
}
    
int16_t
FastSID::calculateSingleSample()
{
    uint32_t osc0, osc1, osc2;
    Voice *v0 = &voice[0];
    Voice *v1 = &voice[1];
    Voice *v2 = &voice[2];
    bool sync0 = false;
    bool sync1 = false;
    bool sync2 = false;
    
    // Advance wavetable counters
    v0->waveTableCounter += v0->step;
    v1->waveTableCounter += v1->step;
    v2->waveTableCounter += v2->step;
    
    // Check for counter overflows (waveform loops)
    if (v0->waveTableCounter < v0->step) {
        v0->lsfr = NSHIFT(v0->lsfr, 16);
        sync1 = v1->syncBit();
    }
    if (v1->waveTableCounter < v1->step) {
        v1->lsfr = NSHIFT(v1->lsfr, 16);
        sync2 = v2->syncBit();
    }
    if (v2->waveTableCounter < v2->step) {
        v2->lsfr = NSHIFT(v2->lsfr, 16);
        sync0 = v0->syncBit();
    }
    
    // Perform hard sync
    if (sync0) {
        v0->lsfr = NSHIFT(v0->lsfr, v0->waveTableCounter >> 28);
        v0->waveTableCounter = 0;
    }
    if (sync1) {
        v1->lsfr = NSHIFT(v1->lsfr, v1->waveTableCounter >> 28);
        v1->waveTableCounter = 0;
    }
    if (sync2) {
        v2->lsfr = NSHIFT(v2->lsfr, v2->waveTableCounter >> 28);
        v2->waveTableCounter = 0;
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
        v0->filterIO = ampMod1x8[(osc0 >> 22)];
        if (filterOn(0)) v0->applyFilter();
        osc0 = ((uint32_t)(v0->filterIO) + 0x80) << (7 + 15);
        
        v1->filterIO = ampMod1x8[(osc1 >> 22)];
        if (filterOn(1)) v1->applyFilter();
        osc1 = ((uint32_t)(v1->filterIO) + 0x80) << (7 + 15);
        
        v2->filterIO = ampMod1x8[(osc2 >> 22)];
        if (filterOn(2)) v2->applyFilter();
        osc2 = ((uint32_t)(v2->filterIO) + 0x80) << (7 + 15);
    }
    
    return (int16_t)(((int32_t)((osc0 + osc1 + osc2) >> 20) - 0x600) * sidVolume());
}
