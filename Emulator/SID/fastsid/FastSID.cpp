/*
 * This file belongs to the FastSID implementation of VirtualC64, an adaption
 * of the code used in VICE 3.1, the Versatile Commodore Emulator.
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

#include "config.h"
#include "FastSID.h"
#include "C64.h"
#include "IO.h"

#include <cmath>

FastSID::FastSID(C64 &ref, SIDBridge &bridgeref, int n) : SubComponent(ref), bridge(bridgeref), nr(n)
{    
    subComponents = std::vector<C64Component *> {
        
        &voice[0],
        &voice[1],
        &voice[2]
    };
    
    // Initialize wave and noise tables
    FastVoice::initWaveTables();
    
    // Initialize voices
    voice[0].init(this, 0, &voice[3]);
    voice[1].init(this, 1, &voice[0]);
    voice[2].init(this, 2, &voice[1]);
}

void
FastSID::init(double sampleRate, int cycles_per_sec)
{
    u32 i;
    
    executedCycles = 0LL;
    computedSamples = 0LL;

    // Table for internal ADSR counter step calculations
    u16 adrtable[16] = {
        1, 4, 8, 12, 19, 28, 34, 40, 50, 125, 250, 400, 500, 1500, 2500, 4000
    };
    
    speed1 = (cycles_per_sec << 8) / (u32)sampleRate;
    for (i = 0; i < 16; i++) {
        adrs[i] = 500 * 8 * speed1 / adrtable[i];
        sz[i] = 0x8888888 * i;
    }
    
    initFilter(sampleRate);
    updateInternals();
    voice[0].updateInternals(false);
    voice[1].updateInternals(false);
    voice[2].updateInternals(false);
}

void
FastSID::initFilter(double sampleRate)
{
    u16 uk;
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
FastSID::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)

    init(sampleRate, cpuFrequency);
}

void
FastSID::setClockFrequency(u32 frequency)
{
    trace(SID_DEBUG, "Setting clock frequency to %d\n", frequency);
    
    cpuFrequency = frequency;
    
    // Recompute frequency dependent data structures
    init(sampleRate, cpuFrequency);
}

void
FastSID::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    u8 ft = filterType();
    string fts =
    ft == FASTSID_LOW_PASS ? "LOW_PASS" :
    ft == FASTSID_HIGH_PASS ? "HIGH_PASS" :
    ft == FASTSID_BAND_PASS ? "BAND_PASS" : "???";
    
    if (category & dump::State) {

        os << tab("Chip");
        os << "FastSID " << dec(nr) << std::endl;
        os << tab("Model");
        os << SIDRevisionEnum::key(model) << std::endl;
        os << tab("Sampling rate");
        os << sampleRate << std::endl;
        os << tab("CPU frequency");
        os << dec(cpuFrequency) << std::endl;
        os << tab("Emulate filter");
        os << bol(emulateFilter) << std::endl;
        os << tab("Volume");
        os << dec(sidVolume()) << std::endl;
        os << tab("Filter type");
        os << fts << std::endl;
        os << tab("Filter cut off");
        os << filterCutoff() << std::endl;
        os << tab("Filter resonance");
        os << filterResonance() << std::endl;
        os << tab("Filter enable bits");
        os << hex((u8)(sidreg[0x17] & 0x0F));
    }
    
    if (category & dump::Registers) {
   
        for (isize i = 0; i < 0x1C; i++) {
            
            os << tab("Register " + std::to_string(i));
            os << hex(sidreg[i]) << std::endl;
        }
    }
    
    /*
    for (isize i = 0; i < 3; i++) {
        VoiceInfo vinfo = getVoiceInfo(i);
        u8 wf = vinfo.waveform;
        msg("Voice %d:       Frequency: %d\n", i, vinfo.frequency);
        msg("             Pulse width: %d\n", vinfo.pulseWidth);
        msg("                Waveform: %s\n",
            (wf == FASTSID_NOISE) ? "NOISE" :
            (wf == FASTSID_PULSE) ? "PULSE" :
            (wf == FASTSID_SAW) ? "SAW" :
            (wf == FASTSID_TRIANGLE) ? "TRIANGLE" : "NONE");
        msg("         Ring modulation: %s\n", vinfo.ringMod ? "yes" : "no");
        msg("               Hard sync: %s\n", vinfo.hardSync ? "yes" : "no");
        msg("             Attack rate: %d\n", vinfo.attackRate);
        msg("              Decay rate: %d\n", vinfo.decayRate);
        msg("            Sustain rate: %d\n", vinfo.sustainRate);
        msg("            Release rate: %d\n", vinfo.releaseRate);
    }
    */
}

SIDInfo
FastSID::getInfo()
{
    SIDInfo info;
 
    info.volume = sidVolume();
    info.filterModeBits = sidreg[0x18] & 0xF0;
    info.filterType = filterType();
    info.filterCutoff = filterCutoff();
    info.filterResonance = filterResonance();
    info.filterEnableBits = sidreg[0x17] & 0x0F;

    return info;
}

VoiceInfo
FastSID::getVoiceInfo(isize i)
{
    assert(i < 3);
    
    VoiceInfo info;
    for (isize j = 0; j < 7; j++) {
        info.reg[j] = sidreg[7*i+j];
    }
    info.frequency = voice[i].frequency();
    info.pulseWidth = voice[i].pulseWidth();
    info.waveform = voice[i].waveform();
    info.ringMod = voice[i].ringModBit();
    info.hardSync = voice[i].syncBit();
    info.gateBit = voice[i].gateBit();
    info.testBit = voice[i].testBit();
    info.attackRate = voice[i].attackRate();
    info.decayRate = voice[i].decayRate();
    info.sustainRate = voice[i].sustainRate();
    info.releaseRate = voice[i].releaseRate();
    
    return info;
}

void
FastSID::setRevision(SIDRevision rev)
{
    model = rev;
    
    // Switch wave tables according to new model
    voice[0].updateWaveTablePtr();
    voice[1].updateWaveTablePtr();
    voice[2].updateWaveTablePtr();
}

void
FastSID::setSampleRate(double rate)
{
    trace(SID_DEBUG, "Setting sample rate to %f\n", rate);
    
    sampleRate = rate;
    
    // Recompute sample rate dependent data structures
    init(sampleRate, cpuFrequency);
}

// Special peek function for the I/O memory range.
u8
FastSID::peek(u16 addr)
{
    switch (addr) {
            
        case 0x19: // POTX
        case 0x1A: // POTY

            return 0xFF;
  
        case 0x1B: // OSC 3/RANDOM
            
            // This register allows the microprocessor to read the
            // upper 8 output bits of oscillator 3.
            // return (u8)(voice[2].doosc() >> 7);
            return (u8)rand();

        case 0x1C:
            
            // This register allows the microprocessor to read the
            // output of the voice 3 envelope generator.
            // return (u8)(voice[2].adsr >> 23);
            return (u8)rand();
            
        default:
            
            return latchedDataBus;
    }
}

u8
FastSID::spypeek(u16 addr) const
{
    switch (addr) {
            
        case 0x19: // POTX
        case 0x1A: // POTY
            
            return 0xFF;
            
        default:
            
            return sidreg[addr];
    }
}

void
FastSID::poke(u16 addr, u8 value)
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

i64
FastSID::executeCycles(isize numCycles, SampleStream &stream)
{
    isize buflength = stream.cap();
    
    executedCycles += numCycles;
    
    // Compute the number of sound samples to produce
    double samplesPerCycle = (double)sampleRate / (double)cpuFrequency;
    isize shouldHave = (isize)(executedCycles * samplesPerCycle);
    
    // How many sound samples are missing?
    isize samples = shouldHave - computedSamples;
    computedSamples = shouldHave;
    
    // Do some consistency checking
    if (samples > buflength) {
        warn("Number of missing sound samples exceeds buffer size\n");
        samples = buflength;
    }
    
    // Check for a buffer overflow
    if (unlikely(samples > stream.free())) {
        warn("SID %d: SAMPLE BUFFER OVERFLOW", nr);
        stream.clear();
    }
    
    // Compute missing samples
    for (isize i = 0; i < samples; i++) {
        stream.write(calculateSingleSample());
    }
    
    return samples;
}

i64
FastSID::executeCycles(isize numCycles)
{
    return executeCycles(numCycles, bridge.sidStream[nr]);
}

void
FastSID::updateInternals()
{
    u8 type = filterType();
    u8 res = filterResonance();
    u16 cutoff = filterCutoff();
    
    for (isize i = 0; i < 3; i++) {
    
        voice[i].setFilterType(type);
        
        if (type == FASTSID_BAND_PASS) {
            voice[i].filterDy = bandPassParam[cutoff];
        } else {
            voice[i].filterDy = lowPassParam[cutoff];
        }
        voice[i].filterResDy = std::max(filterResTable[res] - voice[i].filterDy, 1.0);
    }
}
    
i16
FastSID::calculateSingleSample()
{
    u32 osc0, osc1, osc2;
    FastVoice *v0 = &voice[0];
    FastVoice *v1 = &voice[1];
    FastVoice *v2 = &voice[2];
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
        osc0 = ((u32)(v0->filterIO) + 0x80) << (7 + 15);
        
        v1->filterIO = ampMod1x8[(osc1 >> 22)];
        if (filterOn(1)) v1->applyFilter();
        osc1 = ((u32)(v1->filterIO) + 0x80) << (7 + 15);
        
        v2->filterIO = ampMod1x8[(osc2 >> 22)];
        if (filterOn(2)) v2->applyFilter();
        osc2 = ((u32)(v2->filterIO) + 0x80) << (7 + 15);
    }
    
    return (i16)(((i32)((osc0 + osc1 + osc2) >> 20) - 0x600) * sidVolume() * 0.5);
}
