/*
 * (C) 2011 Dirk W. Hoffmann, All rights reserved.
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

SIDBridge::SIDBridge()
{
	setDescription("SIDBridge");
    
    fastsid.bridge = this;
    resid.bridge = this;
    
    // Register sub components
    VirtualComponent *subcomponents[] = { &resid, &fastsid, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));

    // Register snapshot items
    SnapshotItem items[] = {
        
        // Configuration items
        { &useReSID,        sizeof(useReSID),       KEEP_ON_RESET },

        // Internal state
        { &cycles,          sizeof(cycles),         CLEAR_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
    useReSID = true;
}

SIDBridge::~SIDBridge()
{
}

void
SIDBridge::reset()
{
    VirtualComponent::reset();

    clearRingbuffer();
    resid.reset();
    fastsid.reset();
    
    volume = 100000;
    targetVolume = 100000;
}

void
SIDBridge::loadFromBuffer(uint8_t **buffer)
{
    VirtualComponent::loadFromBuffer(buffer);
    clearRingbuffer();
}

void 
SIDBridge::setReSID(bool enable)
{
    useReSID = enable;
}

void
SIDBridge::dumpState(SIDInfo info)
{
    uint8_t ft = info.filterType;
    msg("        Volume: %d\n", info.volume);
    msg("   Filter type: %s\n",
        (ft == FASTSID_LOW_PASS) ? "LOW PASS" :
        (ft == FASTSID_HIGH_PASS) ? "HIGH PASS" :
        (ft == FASTSID_BAND_PASS) ? "BAND PASS" : "NONE");
    msg("Filter cut off: %d\n\n", info.filterCutoff);
    msg("Filter resonance: %d\n\n", info.filterResonance);

    for (unsigned i = 0; i < 3; i++) {
        VoiceInfo *vinfo = (i == 0) ? &info.voice1 : (i == 1) ? &info.voice2 : &info.voice3;
        uint8_t wf = vinfo->waveform;
        msg("Voice %d:       Frequency: %d\n", i, vinfo->frequency);
        msg("             Pulse width: %d\n", vinfo->pulseWidth);
        msg("                Waveform: %s\n",
            (wf == FASTSID_NOISE) ? "NOISE" :
            (wf == FASTSID_PULSE) ? "PULSE" :
            (wf == FASTSID_SAW) ? "SAW" :
            (wf == FASTSID_TRIANGLE) ? "TRIANGLE" : "NONE");
        msg("         Ring modulation: %s\n", vinfo->ringMod ? "yes" : "no");
        msg("               Hard sync: %s\n", vinfo->hardSync ? "yes" : "no");
        msg("             Attack rate: %d\n", vinfo->attackRate);
        msg("              Decay rate: %d\n", vinfo->decayRate);
        msg("            Sustain rate: %d\n", vinfo->sustainRate);
        msg("            Release rate: %d\n", vinfo->releaseRate);
        msg("            Apply filter: %s\n\n", vinfo->filterOn ? "yes" : "no");
    }
}

void 
SIDBridge::dumpState()
{
    msg("ReSID:\n");
    msg("------\n");
    dumpState(resid.getInfo());

    msg("FastSID:\n");
    msg("--------\n");
    msg("    Chip model: %s\n",
        (fastsid.getChipModel() == MOS_6581) ? "6581" :
        (fastsid.getChipModel() == MOS_8580) ? "8580" : "???");
    msg(" Sampling rate: %d\n", fastsid.getSampleRate());
    msg(" CPU frequency: %d\n", fastsid.getClockFrequency());
    msg("Emulate filter: %s\n", fastsid.getAudioFilter() ? "yes" : "no");
    msg("\n");
    dumpState(fastsid.getInfo());
}

SIDInfo
SIDBridge::getInfo()
{
    SIDInfo info = useReSID ? resid.getInfo() : fastsid.getInfo();
    info.potX = c64->potXBits();
    info.potY = c64->potYBits();
    return info;
}

void
SIDBridge::setPAL()
{
    debug(2, "SIDWrapper::setPAL\n");
    setClockFrequency(PAL_CYCLES_PER_SECOND);
}

void
SIDBridge::setNTSC()
{
    debug(2, "SIDWrapper::setNTSC\n");
    setClockFrequency(NTSC_CYCLES_PER_SECOND);
}

uint8_t 
SIDBridge::peek(uint16_t addr)
{
    assert(addr <= 0x1F);
    
    // Get SID up to date
    executeUntil(c64->getCycles());
    
    if (addr == 0x19) {
        return c64->potXBits();
    }
    if (addr == 0x1A) {
        return c64->potYBits();
    }
    
    if (useReSID) {
        return resid.peek(addr);
    } else {
        return fastsid.peek(addr);
    }
}

uint8_t
SIDBridge::spy(uint16_t addr)
{
    assert(addr <= 0x1F);
    return peek(addr);
}

void 
SIDBridge::poke(uint16_t addr, uint8_t value)
{
    // Get SID up to date
    executeUntil(c64->getCycles());

    // Keep both SID implementations up to date
    resid.poke(addr, value);
    fastsid.poke(addr, value);
    
    // Run ReSID for at least one cycle to make pipelined writes work
    if (!useReSID) resid.sid->clock();
}

void
SIDBridge::executeUntil(uint64_t targetCycle)
{
    uint64_t missingCycles = targetCycle - cycles;
    
    if (missingCycles > PAL_CYCLES_PER_SECOND) {
        debug("Far too many SID cycles are missing.\n");
        missingCycles = PAL_CYCLES_PER_SECOND;
    }
    
    execute(missingCycles);
    cycles = targetCycle;
}

void
SIDBridge::execute(uint64_t numCycles)
{
    // debug("Execute SID for %lld cycles (%d samples in buffer)\n", numCycles, samplesInBuffer());
    if (numCycles == 0)
        return;
    
    if (useReSID) {
        resid.execute(numCycles);
    } else {
        fastsid.execute(numCycles);
    }
}

void 
SIDBridge::run()
{
    clearRingbuffer();
}

void 
SIDBridge::halt()
{
    clearRingbuffer();
}

bool
SIDBridge::getAudioFilter()
{
    if (useReSID) {
        return resid.getAudioFilter();
    } else {
        return fastsid.getAudioFilter();
    }
}

void 
SIDBridge::setAudioFilter(bool value)
{
    resid.setAudioFilter(value);
    fastsid.setAudioFilter(value);
}

SamplingMethod
SIDBridge::getSamplingMethod()
{
    // Option is ReSID only
    return resid.getSamplingMethod();
}

void
SIDBridge::setSamplingMethod(SamplingMethod value)
{
    // Option is ReSID only
    resid.setSamplingMethod(value);
}

SIDChipModel
SIDBridge::getChipModel()
{
    if (useReSID) {
        return resid.getChipModel();
    } else {
        return fastsid.getChipModel();
    }
}

void 
SIDBridge::setChipModel(SIDChipModel model)
{
    if (model != MOS_6581 && model != MOS_8580) {
        warn("Unknown chip model (%d). Using  MOS8580\n", model);
        model = MOS_8580;
    }
    
    resid.setChipModel(model);
    fastsid.setChipModel(model);
}

uint32_t
SIDBridge::getSampleRate()
{
    if (useReSID) {
        return resid.getSampleRate();
    } else {
        return fastsid.getSampleRate();
    }
}

void 
SIDBridge::setSampleRate(uint32_t rate)
{
    resid.setSampleRate(rate);
    fastsid.setSampleRate(rate);
}

uint32_t
SIDBridge::getClockFrequency()
{
    if (useReSID) {
        return resid.getClockFrequency();
    } else {
        return fastsid.getClockFrequency();
    }
}

void 
SIDBridge::setClockFrequency(uint32_t frequency)
{
    resid.setClockFrequency(frequency);
    fastsid.setClockFrequency(frequency);
}



void
SIDBridge::clearRingbuffer()
{
    debug(4,"Clearing ringbuffer\n");
    
    // Reset ringbuffer contents
    for (unsigned i = 0; i < bufferSize; i++) {
        ringBuffer[i] = 0.0f;
    }
    
    // Reset pointer positions
    readPtr = 0;
    alignWritePtr();
}

float
SIDBridge::readData()
{
    // Read sound sample
    float value = ringBuffer[readPtr];
    
    // Adjust volume
    if (volume != targetVolume) {
        if (volume < targetVolume) {
            volume += MIN(volumeDelta, targetVolume - volume);
        } else {
            volume -= MIN(volumeDelta, volume - targetVolume);
        }
    }
    float divider = 75000.0f; // useReSID ? 100000.0f : 150000.0f;
    value = (volume <= 0) ? 0.0f : value * (float)volume / divider;
    
    // Advance read pointer
    advanceReadPtr();
    
    return value;
}

float
SIDBridge::snoop(size_t offset)
{
    return ringBuffer[(readPtr + offset) % bufferSize];
}

float
SIDBridge::snoop(size_t offset, unsigned range)
{
    float result = 0.0;
    for (unsigned i = 0; i < range; i++) {
        result += snoop(i);
    }
    return result / range;
}

void
SIDBridge::readMonoSamples(float *target, size_t n)
{
    // Check for buffer underflow
    if (samplesInBuffer() < n) {
        handleBufferUnderflow();
    }
    
    // Read samples
    for (size_t i = 0; i < n; i++) {
        float value = readData();
        target[i] = value;
    }
}

void
SIDBridge::readStereoSamples(float *target1, float *target2, size_t n)
{
    // Check for buffer underflow
    if (samplesInBuffer() < n) {
        handleBufferUnderflow();
    }
    
    // Read samples
    for (unsigned i = 0; i < n; i++) {
        float value = readData();
        target1[i] = target2[i] = value;
    }
}

void
SIDBridge::readStereoSamplesInterleaved(float *target, size_t n)
{
    // Check for buffer underflow
    if (samplesInBuffer() < n) {
        handleBufferUnderflow();
    }
    
    // Read samples
    for (unsigned i = 0; i < n; i++) {
        float value = readData();
        target[i*2] = value;
        target[i*2+1] = value;
    }
}

void
SIDBridge::writeData(short *data, size_t count)
{
    // Check for buffer overflow
    if (bufferCapacity() < count) {
        handleBufferOverflow();
    }
    
    // Convert sound samples to floating point values and write into ringbuffer
    for (unsigned i = 0; i < count; i++) {
        ringBuffer[writePtr] = float(data[i]) * scale;
        advanceWritePtr();
    }
}

void
SIDBridge::handleBufferUnderflow()
{
    bufferUnderflows++;
    debug(3, "SID RINGBUFFER UNDERFLOW (%ld)\n", readPtr);
}

void
SIDBridge::handleBufferOverflow()
{
    bufferOverflows++;
    debug(3, "SID RINGBUFFER OVERFLOW (%ld)\n", writePtr);
    
    if (!c64->getWarp()) {
        // In real-time mode, we readjust the write pointer
        alignWritePtr();
    } else {
        // In warp mode, we don't advance the write ptr to avoid crack noises
        return;
    }
}


