// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "Muxer.h"
#include "Emulator.h"
#include "IOUtils.h"

#include <algorithm>
#include <cmath>

namespace vc64 {

Muxer::Muxer(C64 &ref) : SubComponent(ref)
{
    subComponents = std::vector<CoreComponent *> {

        &sid[0],
        &sid[1],
        &sid[2],
        &sid[3]
    };
}

void
Muxer::_reset(bool hard)
{
    if (hard) clearStats();
    clear();
}

void
Muxer::clear()
{
    debug(AUDBUF_DEBUG, "clear()\n");
    
    // Wipe out the ringbuffer
    stream.lock();
    stream.wipeOut();
    stream.alignWritePtr();
    stream.unlock();
}

void
Muxer::resetConfig()
{
    Configurable::resetConfig(emulator.defaults);
}

i64
Muxer::getOption(Option option) const
{
    switch (option) {
            
        case OPT_SID_REVISION:
            return config.revision;
            
        case OPT_SID_POWER_SAVE:
            return config.powerSave;
            
        case OPT_SID_FILTER:
            return config.filter;
            
        case OPT_SID_ENGINE:
            return config.engine;
            
        case OPT_SID_SAMPLING:
            return config.sampling;
            
        case OPT_AUD_VOL_L:
            return config.volL;

        case OPT_AUD_VOL_R:
            return config.volR;
            
        default:
            fatalError;
    }
}

void
Muxer::setOption(Option option, i64 value)
{
    bool wasMuted = isMuted();

    switch (option) {

        case OPT_HOST_SAMPLE_RATE:
        {
            setSampleRate(double(value));
            return;
        }
        case OPT_SID_POWER_SAVE:
        {
            {   SUSPENDED

                config.powerSave = bool(value);
            }
            return;
        }
        case OPT_SID_REVISION:
        {
            if (!SIDRevisionEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, SIDRevisionEnum::keyList());
            }
            
            {   SUSPENDED
                
                config.revision = SIDRevision(value);
                sid[0].setRevision(SIDRevision(value));
                sid[1].setRevision(SIDRevision(value));
                sid[2].setRevision(SIDRevision(value));
                sid[3].setRevision(SIDRevision(value));
            }
            return;
        }
        case OPT_SID_FILTER:
        {
            {   SUSPENDED
                
                config.filter = bool(value);
                sid[0].setAudioFilter(bool(value));
                sid[1].setAudioFilter(bool(value));
                sid[2].setAudioFilter(bool(value));
                sid[3].setAudioFilter(bool(value));
            }
            return;
        }
        case OPT_SID_ENGINE:
        {
            if (!SIDEngineEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, SIDEngineEnum::keyList());
            }

            {   SUSPENDED

                config.engine = SIDEngine(value);
            }
            return;
        }
        case OPT_SID_SAMPLING:
        {
            if (!SamplingMethodEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, SamplingMethodEnum::keyList());
            }

            {   SUSPENDED
                
                config.sampling = SamplingMethod(value);
                sid[0].setSamplingMethod(SamplingMethod(value));
                sid[1].setSamplingMethod(SamplingMethod(value));
                sid[2].setSamplingMethod(SamplingMethod(value));
                sid[3].setSamplingMethod(SamplingMethod(value));
            }
            return;
        }
        case OPT_AUD_VOL_L:
            
            config.volL = std::clamp(value, 0LL, 100LL);
            volL.set(powf((float)config.volL / 50, 1.4f));
            
            if (wasMuted != isMuted()) {
                msgQueue.put(MSG_MUTE, isMuted());
            }
            return;
            
        case OPT_AUD_VOL_R:

            config.volR = std::clamp(value, 0LL, 100LL);
            volR.set(powf((float)config.volR / 50, 1.4f));

            if (wasMuted != isMuted()) {
                msgQueue.put(MSG_MUTE, isMuted());
            }
            return;
            
        default:
            fatalError;
    }
}

bool
Muxer::isMuted() const
{
    if (config.volL == 0 && config.volR == 0) return true;
    
    return
    sid[0].config.vol == 0 &&
    sid[1].config.vol == 0 &&
    sid[2].config.vol == 0 &&
    sid[3].config.vol == 0;
}

u32
Muxer::getClockFrequency()
{
    assert(sid[0].getClockFrequency() == sid[1].getClockFrequency());
    assert(sid[0].getClockFrequency() == sid[2].getClockFrequency());
    assert(sid[0].getClockFrequency() == sid[3].getClockFrequency());

    return sid[0].getClockFrequency();
}

void
Muxer::setClockFrequency(u32 frequency)
{
    trace(SID_DEBUG, "Setting clock frequency to %d\n", frequency);

    cpuFrequency = frequency;

    sid[0].setClockFrequency(frequency);
    sid[1].setClockFrequency(frequency);
    sid[2].setClockFrequency(frequency);
    sid[3].setClockFrequency(frequency);
}

double
Muxer::getSampleRate() const
{
    assert(sid[0].getSampleRate() == sid[1].getSampleRate());
    assert(sid[0].getSampleRate() == sid[2].getSampleRate());
    assert(sid[0].getSampleRate() == sid[3].getSampleRate());

    return sid[0].getSampleRate();
}

void
Muxer::setSampleRate(double rate)
{
    trace(true, "Setting sample rate to %f\n", rate);

    sampleRate = rate;
    
    sid[0].setSampleRate(rate);
    sid[1].setSampleRate(rate);
    sid[2].setSampleRate(rate);
    sid[3].setSampleRate(rate);
}

void 
Muxer::operator << (SerReader &worker)
{
    serialize(worker);

    for (isize i = 0; i < 4; i++) sidStream[i].clear(0);
}

void
Muxer::_run()
{
    if (volL.current == 0.0) {
        debug(AUDBUF_DEBUG, "Ramping up from 0\n");
        clear();
    }
    rampUp();
}

void
Muxer::_pause()
{
    rampDown();
}

void
Muxer::_warpOn()
{
    /* Warping has the unavoidable drawback that audio playback gets out of
     * sync. To cope with it, we ramp down the volume when warping is switched
     * on and fade in smoothly when it is switched off.
     */
    rampDown();
}

void
Muxer::_warpOff()
{
    rampUp();
    clear();
}

void
Muxer::_dump(Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category == Category::Config) {
        
        os << tab("Chip revision");
        os << SIDRevisionEnum::key(config.revision) << std::endl;
        os << tab("Power save mode");
        os << bol(config.powerSave, "during warp", "never") << std::endl;
        os << tab("Filter");
        os << bol(config.filter) << std::endl;
        os << tab("Engine");
        os << SIDEngineEnum::key(config.engine) << std::endl;
        os << tab("Sampling");
        os << SamplingMethodEnum::key(config.sampling) << std::endl;
        os << tab("Volume 1");
        os << sid[0].config.vol << std::endl;
        os << tab("Volume 2");
        os << sid[1].config.vol << std::endl;
        os << tab("Volume 3");
        os << sid[2].config.vol << std::endl;
        os << tab("Volume 4");
        os << sid[3].config.vol << std::endl;
        os << tab("Volume L");
        os << config.volL << std::endl;
        os << tab("Volume R");
        os << config.volR << std::endl;

        if (sid[1].config.enabled) {
            os << tab("1st extra SID");
            os << hex(sid[1].config.address) << std::endl;
        }
        if (sid[2].config.enabled) {
            os << tab("2nd extra SID");
            os << hex(sid[2].config.address) << std::endl;
        }
        if (sid[3].config.enabled) {
            os << tab("3rd extra SID");
            os << hex(sid[3].config.address) << std::endl;
        }

    } else {

        _dump(category, os, 0);
    }
}

void
Muxer::_dump(Category category, std::ostream& os, isize nr) const
{
    switch (config.engine) {
            
        case SIDENGINE_RESID:   sid[nr].resid.dump(category, os); break;
        case SIDENGINE_FASTSID: sid[nr].fastsid.dump(category, os); break;

        default:
            fatalError;
    }
}

MuxerStats
Muxer::getStats()
{
    stats.fillLevel = stream.fillLevel();
    return stats;
}

void
Muxer::clearStats()
{
    memset(&stats, 0, sizeof(stats));
}

SIDInfo
Muxer::getInfo(isize nr)
{
    assert(nr < 4);
    
    SIDInfo info = { };
    
    switch (config.engine) {
            
        case SIDENGINE_RESID:   info = sid[nr].resid.getState(); break;
        case SIDENGINE_FASTSID: info = sid[nr].fastsid.getInfo(); break;

        default:
            fatalError;
    }
    
    info.potX = port1.mouse.readPotX() & port2.mouse.readPotX();
    info.potY = port1.mouse.readPotY() & port2.mouse.readPotY();
    
    return info;
}

VoiceInfo
Muxer::getVoiceInfo(isize nr, isize voice)
{
    assert(nr >= 0 && nr <= 3);

    switch (config.engine) {
            
        case SIDENGINE_RESID:   return sid[nr].resid.getVoiceInfo(voice);
        case SIDENGINE_FASTSID: return sid[nr].fastsid.getVoiceInfo(voice);

        default:
            fatalError;
    }
}

CoreComponent &
Muxer::getSID(isize nr)
{
    assert(nr >= 0 && nr <= 3);
    
    switch (config.engine) {
            
        case SIDENGINE_RESID:   return sid[nr].resid;
        case SIDENGINE_FASTSID: return sid[nr].fastsid;

        default:
            fatalError;
    }
}

void
Muxer::rampUp()
{
    trace(AUDBUF_DEBUG, "rampUp()\n");

    const isize steps = 20000;
    volL.fadeIn(steps);
    volR.fadeIn(steps);
    
    ignoreNextUnderOrOverflow();
}

void 
Muxer::rampUp(float from)
{
    trace(AUDBUF_DEBUG, "rampUp(%f)\n", from);

    volL.current = 0;
    volR.current = 0;

    rampUp();
}

void
Muxer::rampDown()
{
    trace(AUDBUF_DEBUG, "rampDown()\n");

    const isize steps = 2000;
    volL.fadeOut(steps);
    volR.fadeOut(steps);
    
    ignoreNextUnderOrOverflow();
}

isize
Muxer::mappedSID(u16 addr) const
{
    addr &= 0xFFE0;
    
    if (isEnabled(1) && addr == sid[1].config.address) return 1;
    if (isEnabled(2) && addr == sid[2].config.address) return 2;
    if (isEnabled(3) && addr == sid[3].config.address) return 3;

    return 0;
}

u8 
Muxer::peek(u16 addr)
{
    // Get SIDs up to date
    executeUntil(cpu.clock);

    // Select the target SID
    isize sidNr = mappedSID(addr);

    addr &= 0x1F;

    if (sidNr == 0) {
        
        if (addr == 0x19) {
            
            port1.updatePotX();
            port2.updatePotX();
            return readPotX();
        }
        if (addr == 0x1A) {

            port1.updatePotY();
            port2.updatePotY();
            return readPotY();
        }
    }
    
    switch (config.engine) {
            
        case SIDENGINE_RESID:   return sid[sidNr].resid.peek(addr);
        case SIDENGINE_FASTSID: return sid[sidNr].fastsid.peek(addr);

        default:
            fatalError;
    }
}

u8
Muxer::spypeek(u16 addr) const
{
    // Select the target SID
    isize sidNr = mappedSID(addr);

    addr &= 0x1F;

    if (sidNr == 0) {
        if (addr == 0x19) { return port1.readPotX() & port2.readPotX(); }
        if (addr == 0x1A) { return port1.readPotY() & port2.readPotY(); }
    }

    /* Currently, only FastSID allows us to peek into the SID registers
     * without causing side effects. Hence, we get the return value from there,
     * regardless of the selected SID engine.
     */
    return sid[sidNr].fastsid.spypeek(addr);
}

u8
Muxer::readPotX() const
{
    u8 result = 0xFF;

    if (GET_BIT(cia1.getPA(), 7) == 0) result &= port1.readPotX();
    if (GET_BIT(cia1.getPA(), 6) == 0) result &= port2.readPotX();

    return result;
}

u8
Muxer::readPotY() const
{
    u8 result = 0xFF;

    if (GET_BIT(cia1.getPA(), 7) == 0) result &= port1.readPotY();
    if (GET_BIT(cia1.getPA(), 6) == 0) result &= port2.readPotY();

    return result;
}

void 
Muxer::poke(u16 addr, u8 value)
{
    trace(SIDREG_DEBUG, "poke(%x,%x)\n", addr, value);
    
    // Get SID up to date
    executeUntil(cpu.clock);

    // Select the target SID
    isize sidNr = mappedSID(addr);

    addr &= 0x1F;
    
    // Keep both SID implementations up to date
    sid[sidNr].resid.poke(addr, value);
    sid[sidNr].fastsid.poke(addr, value);
}

void
Muxer::executeUntil(Cycle targetCycle)
{
    assert(targetCycle >= cycles);
    
    // Skip sample synthesis in power-safe mode
    if (volL.current == 0 && volR.current == 0 && config.powerSave) {

        /* https://sourceforge.net/p/vice-emu/bugs/1374/
         *
         * Due to a bug in reSID, pending register writes are dropped if we
         * skip sample synthesis if SAMPLE_FAST and MOS8580 are selected both.
         * As a workaround, we ignore the power-saving setting in this case.
         */
        if (config.revision != MOS_8580 || config.sampling != SAMPLING_FAST) {

            cycles = targetCycle;
            return;
        }
    }
    
    isize missingCycles  = isize(targetCycle - cycles);
    isize consumedCycles = executeCycles(missingCycles);

    cycles += consumedCycles;
    
    debug(SID_EXEC,
          "target: %lld missing: %ld consumed: %ld reached: %lld still missing: %lld\n",
          targetCycle, missingCycles, consumedCycles, cycles, targetCycle - cycles);
}

isize
Muxer::executeCycles(isize numCycles)
{
    isize numSamples;
    
    // Run reSID for at least one cycle to make pipelined writes work
    if (numCycles == 0) {
        
        numCycles = 1;
        debug(SID_EXEC, "Running SIDs for an extra cycle\n");
    }
    
    switch (config.engine) {
            
        case SIDENGINE_FASTSID:

            // Run the primary SID (which is always enabled)
            numSamples = sid[0].fastsid.executeCycles(numCycles, sidStream[0]);

            // Run all other SIDS (if any)
            for (isize i = 1; i < 4; i++) {

                if (isEnabled(i)) {

                    isize numSamples2 = sid[i].fastsid.executeCycles(numCycles, sidStream[i]);
                    numSamples = std::min(numSamples, numSamples2);
                }
            }
            break;

        case SIDENGINE_RESID:

            // Run the primary SID (which is always enabled)
            numSamples = sid[0].resid.executeCycles(numCycles, sidStream[0]);

            // Run all other SIDS (if any)
            for (isize i = 1; i < 4; i++) {

                if (isEnabled(i)) {

                    isize numSamples2 = sid[i].resid.executeCycles(numCycles, sidStream[i]);
                    numSamples = std::min(numSamples, numSamples2);
                }
            }
            break;

        default:
            fatalError;
    }
    
    // Produce the final stereo stream
    if (isEnabled(1) || isEnabled(2) || isEnabled(3)) {
        mixMultiSID(numSamples);
    } else {
        mixSingleSID(numSamples);
    }

    return numCycles;
}

void
Muxer::mixSingleSID(isize numSamples)
{    
    stream.lock();
    
    // Check for buffer overflow
    if (stream.free() < numSamples) {
        handleBufferOverflow();
    }
    
    debug(SID_EXEC, "vol0: %f pan0: %f volL: %f volR: %f\n",
          sid[0].vol, sid[0].pan, volL.current, volR.current);

    // Convert sound samples to floating point values and write into ringbuffer
    for (isize i = 0; i < numSamples; i++) {
        
        // Read SID sample from ring buffer
        float ch0 = (float)sidStream[0].read() * sid[0].vol;

        // Compute left and right channel output
        float l = ch0 * (1 - sid[0].pan);
        float r = ch0 * sid[0].pan;

        // Apply master volume
        l *= volL.current;
        r *= volR.current;
        
        // Apply ear protection
        assert(abs(l) < 1.0);
        assert(abs(r) < 1.0);
        
        stream.write(SamplePair { l, r } );
    }
    stream.unlock();
}

void
Muxer::mixMultiSID(isize numSamples)
{
    stream.lock();
    
    // Check for buffer overflow
    if (stream.free() < numSamples) {
        handleBufferOverflow();
    }
    
    debug(SID_EXEC, "vol0: %f pan0: %f volL: %f volR: %f\n",
          sid[0].vol, sid[0].pan, volL.current, volR.current);

    // Convert sound samples to floating point values and write into ringbuffer
    for (isize i = 0; i < numSamples; i++) {
        
        float ch0, ch1, ch2, ch3, l, r;
        
        ch0 = (float)sidStream[0].read()  * sid[0].vol;
        ch1 = (float)sidStream[1].read(0) * sid[1].vol;
        ch2 = (float)sidStream[2].read(0) * sid[2].vol;
        ch3 = (float)sidStream[3].read(0) * sid[3].vol;

        // Compute left channel output
        l =
        ch0 * (1 - sid[0].pan) + ch1 * (1 - sid[1].pan) +
        ch2 * (1 - sid[2].pan) + ch3 * (1 - sid[3].pan);

        // Compute right channel output
        r =
        ch0 * sid[0].pan + ch1 * sid[1].pan +
        ch2 * sid[2].pan + ch3 * sid[0].pan;

        // Apply master volume
        l *= volL.current;
        r *= volR.current;
        
        // Apply ear protection
        assert(abs(l) < 1.0);
        assert(abs(r) < 1.0);
        
        stream.write(SamplePair { l, r } );
    }
    stream.unlock();
}

void
Muxer::clearSampleBuffers()
{
    for (int i = 0; i < 4; i++) clearSampleBuffer(i);
}

void
Muxer::clearSampleBuffer(long nr)
{
    sidStream[nr].clear(0);
}

void
Muxer::ringbufferData(isize offset, float *left, float *right)
{
    const SamplePair &pair = stream.current((int)offset);
    *left = pair.left;
    *right = pair.right;
}

void
Muxer::handleBufferUnderflow()
{
    // There are two common scenarios in which buffer underflows occur:
    //
    // (1) The consumer runs slightly faster than the producer.
    // (2) The producer is halted or not startet yet.
    
    trace(AUDBUF_DEBUG, "BUFFER UNDERFLOW (r: %ld w: %ld)\n", stream.r, stream.w);

    // Reset the write pointer
    stream.alignWritePtr();
    
    // Determine the elapsed seconds since the last pointer adjustment
    auto elapsedTime = util::Time::now() - lastAlignment;
    lastAlignment = util::Time::now();
    
    // Adjust the sample rate, if condition (1) holds
    if (elapsedTime.asSeconds() > 10.0) {

        stats.bufferUnderflows++;
        
        // Increase the sample rate based on what we've measured
        isize offPerSecond = (isize)(stream.count() / elapsedTime.asSeconds());
        setSampleRate(getSampleRate() + offPerSecond);
    }
}

void
Muxer::handleBufferOverflow()
{
    // There are two common scenarios in which buffer overflows occur:
    //
    // (1) The consumer runs slightly slower than the producer
    // (2) The consumer is halted or not startet yet
    
    trace(AUDBUF_DEBUG, "BUFFER OVERFLOW (r: %ld w: %ld)\n", stream.r, stream.w);

    // Reset the write pointer
    stream.alignWritePtr();
    
    // Determine the number of elapsed seconds since the last adjustment
    auto elapsedTime = util::Time::now() - lastAlignment;
    lastAlignment = util::Time::now();
    trace(AUDBUF_DEBUG, "elapsedTime: %f\n", elapsedTime.asSeconds());
    
    // Adjust the sample rate, if condition (1) holds
    if (elapsedTime.asSeconds() > 10.0) {
        
        stats.bufferOverflows++;
        
        // Decrease the sample rate based on what we've measured
        isize offPerSecond = (isize)(stream.count() / elapsedTime.asSeconds());
        double newSampleRate = getSampleRate() - offPerSecond;

        trace(AUDBUF_DEBUG, "Changing sample rate to %f\n", newSampleRate);
        setSampleRate(newSampleRate);
    }
}

void
Muxer::ignoreNextUnderOrOverflow()
{
    lastAlignment = util::Time::now();
}

void
Muxer::copyMono(float *target, isize n)
{
    if (recorder.isRecording()) {
        for (isize i = 0; i < n; i++) target[i] = 0.0;
        return;
    }
    
    stream.lock();
    
    // Check for a buffer underflow
    if (stream.count() < n) handleBufferUnderflow();

    // Copy sound samples
    stream.copyMono(target, n, volL, volR);
    
    stream.unlock();
}

void
Muxer::copyStereo(float *target1, float *target2, isize n)
{
    if (recorder.isRecording()) {
        for (isize i = 0; i < n; i++) target1[i] = target2[i] = 0.0;
        return;
    }

    stream.lock();
    
    // Check for a buffer underflow
    if (stream.count() < n) handleBufferUnderflow();

    // Copy sound samples
    stream.copyStereo(target1, target2, n, volL, volR);
    
    stream.unlock();
}

void
Muxer::copyInterleaved(float *target, isize n)
{
    if (recorder.isRecording()) {
        for (isize i = 0; i < n; i++) target[i] = 0.0;
        return;
    }

    stream.lock();
    
    // Check for a buffer underflow
    if (stream.count() < n) handleBufferUnderflow();

    // Read sound samples
    stream.copyInterleaved(target, n, volL, volR);
    
    stream.unlock();
}

float
Muxer::draw(u32 *buffer, isize width, isize height,
            float maxAmp, u32 color, isize sid) const
{
    auto samples = new float[width][2];
    isize hheight = height / 2;
    float newMaxAmp = 0.001f, dw;

    // Gather data
    switch (sid) {

        case 0: case 1: case 2: case 3:

            dw = sidStream[sid].cap() / float(width);

            for (isize w = 0; w < width; w++) {

                auto sample = sidStream[sid].current(isize(w * dw));
                samples[w][0] = float(abs(sample));
                samples[w][1] = float(abs(sample));
            }
            break;

        default:

            dw = stream.cap() / float(width);

            for (isize w = 0; w < width; w++) {

                auto sample = stream.current(isize(w * dw));
                samples[w][0] = abs(sample.left);
                samples[w][1] = abs(sample.right);
            }
            break;
    }

    // Clear buffer
    for (isize i = 0; i < width * height; i++) buffer[i] = 0;

    // Draw waveform
    for (isize w = 0; w < width; w++) {

        u32 *ptr = buffer + width * hheight + w;

        if (samples[w][0] == 0 && samples[w][1] == 0) {

            // Draw some noise to make it look sexy
            *ptr = color;
            if (rand() % 2) *(ptr + width) = color;
            if (rand() % 2) *(ptr - width) = color;

        } else {

            // Remember the highest amplitude
            if (samples[w][0] > newMaxAmp) newMaxAmp = samples[w][0];
            if (samples[w][1] > newMaxAmp) newMaxAmp = samples[w][1];

            // Scale the sample
            isize scaledL = std::min(isize(samples[w][0] * hheight / maxAmp), hheight);
            isize scaledR = std::min(isize(samples[w][1] * hheight / maxAmp), hheight);

            // Draw vertical lines
            for (isize j = 0; j < scaledL; j++) *(ptr - j * width) = color;
            for (isize j = 0; j < scaledR; j++) *(ptr + j * width) = color;
        }
    }

    delete[] samples;
    return newMaxAmp;
}

}
