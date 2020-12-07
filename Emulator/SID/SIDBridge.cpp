// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

SIDBridge::SIDBridge(C64 &ref) : C64Component(ref)
{
	setDescription("SIDBridge");
        
    subComponents = vector<HardwareComponent *> {
        
        &resid[0],
        &resid[1],
        &resid[2],
        &resid[3],
        &fastsid[0],
        &fastsid[1],
        &fastsid[2],
        &fastsid[3]
    };
    
    config.engine = ENGINE_RESID;

    for (int i = 0; i < 4; i++) {
        resid[i].setClockFrequency(PAL_CLOCK_FREQUENCY);
        fastsid[i].setClockFrequency(PAL_CLOCK_FREQUENCY);
    }
}

void
SIDBridge::_reset()
{
    RESET_SNAPSHOT_ITEMS
    
    clearRingbuffer();
    
    volume = 100000;
    targetVolume = 100000;
}

long
SIDBridge::getConfigItem(ConfigOption option)
{
    switch (option) {
            
        case OPT_SID_REVISION:  return config.revision;
        case OPT_SID_FILTER:    return config.filter;
        case OPT_SID_ENGINE:    return config.engine;
        case OPT_SID_SAMPLING:  return config.sampling;
            
        default: assert(false);
    }
}

bool
SIDBridge::setConfigItem(ConfigOption option, long value)
{
    switch (option) {
            
        case OPT_VIC_REVISION:
        {
            u32 newFrequency = VICII::getFrequency((VICRevision)value);
                                    
            suspend();
            setClockFrequency(newFrequency);
            resume();
            
            return true;
        }
            
        case OPT_SID_REVISION:
            
            if (!isSIDRevision(value)) {
                warn("Invalid SID revision: %d\n", value);
                return false;
            }
            if (config.revision == value) {
                return false;
            }
            
            suspend();
            config.revision = (SIDRevision)value;
            setRevision((SIDRevision)value);
            resume();
            
            return true;
            
        case OPT_SID_FILTER:
            
            if (config.filter == value) {
                return false;
            }

            suspend();
            config.filter = value;
            setAudioFilter(value);
            resume();
            
            return true;
            
        case OPT_SID_ENGINE:
            
            if (!isAudioEngine(value)) {
                warn("Invalid SID engine: %d\n", value);
                return false;
            }
            if (config.engine == value) {
                return false;
            }
            suspend();
            config.engine = (SIDEngine)value;
            resume();
            
            return true;
            
        case OPT_SID_SAMPLING:
            
            if (!isSamplingMethod(value)) {
                warn("Invalid sampling method: %d\n", value);
                return false;
            }
            if (config.engine == value) {
                return false;
            }
            suspend();
            config.sampling = (SamplingMethod)value;
            setSamplingMethod((SamplingMethod)value);
            resume();
            
            return true;
            
        default:
            return false;
    }
}

u32
SIDBridge::getClockFrequency()
{
    u32 result = resid[0].getClockFrequency();
    
    for (int i = 0; i < 4; i++) {
        assert(resid[i].getClockFrequency() == result);
        assert(fastsid[i].getClockFrequency() == result);
    }
    
    return result;
}

void
SIDBridge::setClockFrequency(u32 frequency)
{
    debug(SID_DEBUG, "Setting clock frequency to %d\n", frequency);

    for (int i = 0; i < 4; i++) {
        resid[i].setClockFrequency(frequency);
        fastsid[i].setClockFrequency(frequency);
    }
}

SIDRevision
SIDBridge::getRevision()
{
    SIDRevision result = resid[0].getRevision();
    
    for (int i = 0; i < 4; i++) {
        assert(resid[i].getRevision() == result);
        assert(fastsid[i].getRevision() == result);
    }
    
    return result;
}

void
SIDBridge::setRevision(SIDRevision revision)
{
    debug(SID_DEBUG, "Setting SID revision to %s\n", sidRevisionName(revision));

    for (int i = 0; i < 4; i++) {
        resid[i].setRevision(revision);
        fastsid[i].setRevision(revision);
    }
}

double
SIDBridge::getSampleRate()
{
    double result = resid[0].getSampleRate();
    
    for (int i = 0; i < 4; i++) {
        assert(resid[i].getSampleRate() == result);
        assert(fastsid[i].getClockFrequency() == result);
    }
    
    return result;
}

void
SIDBridge::setSampleRate(double rate)
{
    debug(SID_DEBUG, "Setting sample rate to %f\n", rate);

    for (int i = 0; i < 4; i++) {
        resid[i].setSampleRate(rate);
        fastsid[i].setSampleRate(rate);
    }
}

bool
SIDBridge::getAudioFilter()
{
    bool result = resid[0].getAudioFilter();
    
    for (int i = 0; i < 4; i++) {
        assert(resid[i].getAudioFilter() == result);
        assert(fastsid[i].getAudioFilter() == result);
    }
    
    return result;
}

void
SIDBridge::setAudioFilter(bool enable)
{
    debug(SID_DEBUG, "%s audio filter\n", enable ? "Enabling" : "Disabling");

    for (int i = 0; i < 4; i++) {
        resid[i].setAudioFilter(enable);
        fastsid[i].setAudioFilter(enable);
    }
}

SamplingMethod
SIDBridge::getSamplingMethod()
{
    SamplingMethod result = resid[0].getSamplingMethod();
    
    for (int i = 0; i < 4; i++) {
        assert(resid[i].getSamplingMethod() == result);
        // Note: fastSID has no such option
    }
    
    return result;
}

void
SIDBridge::setSamplingMethod(SamplingMethod method)
{
    debug(SID_DEBUG, "Setting sampling method to %s\n",sidSamplingMethodName(method));

    for (int i = 0; i < 4; i++) {
        resid[i].setSamplingMethod(method);
        // Note: fastSID has no such option
    }
}

size_t
SIDBridge::didLoadFromBuffer(u8 *buffer)
{
    clearRingbuffer();
    return 0;
}

void
SIDBridge::_run()
{
    clearRingbuffer();
}

void
SIDBridge::_pause()
{
    clearRingbuffer();
}

void 
SIDBridge::_dump()
{
    msg("ReSID:\n");
    msg("------\n");
    _dump(resid[0].getInfo());

    msg("FastSID:\n");
    msg("--------\n");
    msg("    Chip model: %d (%s)\n", sidRevisionName(fastsid[0].getRevision()));
    msg(" Sampling rate: %d\n", fastsid[0].getSampleRate());
    msg(" CPU frequency: %d\n", fastsid[0].getClockFrequency());
    msg("Emulate filter: %s\n", fastsid[0].getAudioFilter() ? "yes" : "no");
    msg("\n");
    _dump(fastsid[0].getInfo());
}

void
SIDBridge::_dump(SIDInfo info)
{
    u8 ft = info.filterType;
    msg("        Volume: %d\n", info.volume);
    msg("   Filter type: %s\n",
        (ft == FASTSID_LOW_PASS) ? "LOW PASS" :
        (ft == FASTSID_HIGH_PASS) ? "HIGH PASS" :
        (ft == FASTSID_BAND_PASS) ? "BAND PASS" : "NONE");
    msg("Filter cut off: %d\n\n", info.filterCutoff);
    msg("Filter resonance: %d\n\n", info.filterResonance);
    msg("Filter enable bits: %d\n\n", info.filterEnableBits);

    for (unsigned i = 0; i < 3; i++) {
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
}

void
SIDBridge::_setWarp(bool enable)
{
    if (enable) {
        
        // Warping has the unavoidable drawback that audio playback gets out of
        // sync. To cope with this issue, we ramp down the volume when warping
        // is switched on and fade in smoothly when it is switched off.
        sid.rampDown();
        
    } else {
        
        sid.rampUp();
        sid.alignWritePtr();
    }
}

SIDInfo
SIDBridge::getInfo()
{
    SIDInfo info;
    
    switch (config.engine) {
            
        case ENGINE_FASTSID: info = fastsid[0].getInfo(); break;
        case ENGINE_RESID:   info = resid[0].getInfo(); break;
    }
    
    info.potX = mouse.readPotX();
    info.potY = mouse.readPotY();
    
    return info;
}

VoiceInfo
SIDBridge::getVoiceInfo(unsigned voice)
{
    VoiceInfo info;
    
    switch (config.engine) {
            
        case ENGINE_FASTSID: info = fastsid[0].getVoiceInfo(voice); break;
        case ENGINE_RESID:   info = resid[0].getVoiceInfo(voice); break;
    }
    
    return info;
}

u8 
SIDBridge::peek(u16 addr)
{
    assert(addr <= 0x1F);
    
    // Get SID up to date
    executeUntil(cpu.cycle);
    
    if (addr == 0x19) {
        return mouse.readPotX();
    }
    if (addr == 0x1A) {
        return mouse.readPotY();
    }
    
    switch (config.engine) {
            
        case ENGINE_FASTSID: return fastsid[0].peek(addr);
        case ENGINE_RESID:   return resid[0].peek(addr);
            
        default:
            assert(false);
            return 0;
    }
}

u8
SIDBridge::spypeek(u16 addr)
{
    assert(addr <= 0x1F);
    return peek(addr);
}

void 
SIDBridge::poke(u16 addr, u8 value)
{
    // Get SID up to date
    executeUntil(cpu.cycle);

    // Keep both SID implementations up to date
    resid[0].poke(addr, value);
    fastsid[0].poke(addr, value);
    
    // Run ReSID for at least one cycle to make pipelined writes work
    if (config.engine != ENGINE_RESID) resid[0].clock();
}

void
SIDBridge::executeUntil(u64 targetCycle)
{
    u64 missingCycles = targetCycle - cycles;
    
    if (missingCycles > PAL_CYCLES_PER_SECOND) {
        debug(SID_DEBUG, "Far too many SID cycles are missing.\n");
        missingCycles = PAL_CYCLES_PER_SECOND;
    }
    
    execute(missingCycles);
    cycles = targetCycle;
}

void
SIDBridge::execute(u64 numCycles)
{
    // Only proceed if we should advances some cycles
    if (numCycles == 0) return;
 
    u64 numSamples;

    switch (config.engine) {
            
        case ENGINE_FASTSID: numSamples = fastsid[0].execute(numCycles); break;
        case ENGINE_RESID:   numSamples = resid[0].execute(numCycles); break;
            
        default:
            assert(false);
    }
    
    
    // TODO:
    // ADD MIXING CODE HERE
}

void
SIDBridge::clearRingbuffer()
{
    for (int i = 0; i < 4; i++) ringBuffer[i].clear(0);
    alignWritePtr();
}

float
SIDBridge::readData()
{
    // Read sound sample
    float value = ringBuffer[0].read();
    
    // Adjust volume
    if (volume != targetVolume) {
        if (volume < targetVolume) {
            volume += MIN(volumeDelta, targetVolume - volume);
        } else {
            volume -= MIN(volumeDelta, volume - targetVolume);
        }
    }
    // float divider = 75000.0f; // useReSID ? 100000.0f : 150000.0f;
    float divider = 40000.0f;
    value = (volume <= 0) ? 0.0f : value * (float)volume / divider;
        
    return value;
}

float
SIDBridge::ringbufferData(size_t offset)
{
    return ringBuffer[0].current((int)offset);
}

void
SIDBridge::readMonoSamples(float *target, size_t n)
{
    // Check for buffer underflow
    if (ringBuffer[0].count() < n) {
        handleBufferUnderflow();
    }
    
    // Read samples
    for (size_t i = 0; i < n; i++) {
        float value =   readData();
        target[i] = value;
    }
}

void
SIDBridge::readStereoSamples(float *target1, float *target2, size_t n)
{
    // Check for buffer underflow
    if (ringBuffer[0].count() < n) {
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
    if (ringBuffer[0].count() < n) {
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
SIDBridge::handleBufferUnderflow()
{
    // There are two common scenarios in which buffer underflows occur:
    //
    // (1) The consumer runs slightly faster than the producer.
    // (2) The producer is halted or not startet yet.
    
    debug(SID_DEBUG, "BUFFER UNDERFLOW (r: %ld w: %ld)\n", ringBuffer[0].r, ringBuffer[0].w);

    // Determine the elapsed seconds since the last pointer adjustment.
    u64 now = Oscillator::nanos();
    double elapsedTime = (double)(now - lastAlignment) / 1000000000.0;
    lastAlignment = now;

    // Adjust the sample rate, if condition (1) holds
    if (elapsedTime > 10.0) {
        
        bufferUnderflows++;
        
        // Increase the sample rate based on what we've measured
        int offPerSecond = (int)(samplesAhead / elapsedTime);
        setSampleRate(getSampleRate() + offPerSecond);
    }

    // Reset the write pointer
    alignWritePtr();
}

void
SIDBridge::handleBufferOverflow()
{
    // There are two common scenarios in which buffer overflows occur:
    //
    // (1) The consumer runs slightly slower than the producer
    // (2) The consumer is halted or not startet yet
    
    debug(SID_DEBUG, "BUFFER OVERFLOW (r: %ld w: %ld)\n", ringBuffer[0].r, ringBuffer[0].w);
    
    // Determine the elapsed seconds since the last pointer adjustment
    u64 now = Oscillator::nanos();
    double elapsedTime = (double)(now - lastAlignment) / 1000000000.0;
    lastAlignment = now;
    
    // Adjust the sample rate, if condition (1) holds
    if (elapsedTime > 10.0) {
        
        bufferOverflows++;
        
        // Decrease the sample rate based on what we've measured
        int offPerSecond = (int)(samplesAhead / elapsedTime);
        setSampleRate(getSampleRate() - offPerSecond);
    }
    
    // Reset the write pointer
    alignWritePtr();
}

void
SIDBridge::ignoreNextUnderOrOverflow()
{
    lastAlignment = Oscillator::nanos();
}
