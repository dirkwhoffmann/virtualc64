/*
 * (C) 2011 Dirk W. Hoffmann. All rights reserved.
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

ReSID::ReSID()
{
	setDescription("ReSID");
	debug(3, "  Creating ReSID at address %p...\n", this);

    sid = new SID();
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Configuration items
        { &chipModel,           sizeof(chipModel),              KEEP_ON_RESET },
        { &sampleRate,          sizeof(sampleRate),             KEEP_ON_RESET },
        { &samplingMethod,      sizeof(samplingMethod),         KEEP_ON_RESET },
        { &cpuFrequency,        sizeof(cpuFrequency),           KEEP_ON_RESET },
        { &audioFilter,         sizeof(audioFilter),            KEEP_ON_RESET },
        { &externalAudioFilter, sizeof(externalAudioFilter),    KEEP_ON_RESET },
        { &volume,              sizeof(volume),                 KEEP_ON_RESET },
        { &targetVolume,        sizeof(targetVolume),           KEEP_ON_RESET },
        
        // ReSID state
        { st.sid_register,                  sizeof(st.sid_register),                    KEEP_ON_RESET },
        { &st.bus_value,                    sizeof(st.bus_value),                       KEEP_ON_RESET },
        { &st.bus_value_ttl,                sizeof(st.bus_value_ttl),                   KEEP_ON_RESET },
        { &st.accumulator[0],               sizeof(st.accumulator[0]),                  KEEP_ON_RESET },
        { &st.accumulator[1],               sizeof(st.accumulator[1]),                  KEEP_ON_RESET },
        { &st.accumulator[2],               sizeof(st.accumulator[2]),                  KEEP_ON_RESET },
        { &st.shift_register[0],            sizeof(&st.shift_register[0]),              KEEP_ON_RESET },
        { &st.shift_register[1],            sizeof(&st.shift_register[1]),              KEEP_ON_RESET },
        { &st.shift_register[2],            sizeof(&st.shift_register[2]),              KEEP_ON_RESET },
        { &st.rate_counter[0],              sizeof(st.rate_counter[0]),                 KEEP_ON_RESET },
        { &st.rate_counter[1],              sizeof(st.rate_counter[1]),                 KEEP_ON_RESET },
        { &st.rate_counter[2],              sizeof(st.rate_counter[2]),                 KEEP_ON_RESET },
        { &st.rate_counter_period[0],       sizeof(st.rate_counter_period[0]),          KEEP_ON_RESET },
        { &st.rate_counter_period[1],       sizeof(st.rate_counter_period[1]),          KEEP_ON_RESET },
        { &st.rate_counter_period[2],       sizeof(st.rate_counter_period[2]),          KEEP_ON_RESET },
        { &st.exponential_counter[0],       sizeof(st.exponential_counter[0]),          KEEP_ON_RESET },
        { &st.exponential_counter[1],       sizeof(st.exponential_counter[1]),          KEEP_ON_RESET },
        { &st.exponential_counter[2],       sizeof(st.exponential_counter[2]),          KEEP_ON_RESET },
        { &st.exponential_counter_period[0],sizeof(st.exponential_counter_period[0]),   KEEP_ON_RESET },
        { &st.exponential_counter_period[1],sizeof(st.exponential_counter_period[1]),   KEEP_ON_RESET },
        { &st.exponential_counter_period[2],sizeof(st.exponential_counter_period[2]),   KEEP_ON_RESET },
        { &st.envelope_counter[0],          sizeof(st.envelope_counter[0]),             KEEP_ON_RESET },
        { &st.envelope_counter[1],          sizeof(st.envelope_counter[1]),             KEEP_ON_RESET },
        { &st.envelope_counter[2],          sizeof(st.envelope_counter[2]),             KEEP_ON_RESET },
        { &st.envelope_state[0],            sizeof(st.envelope_state[0]),               KEEP_ON_RESET },
        { &st.envelope_state[1],            sizeof(st.envelope_state[1]),               KEEP_ON_RESET },
        { &st.envelope_state[2],            sizeof(st.envelope_state[2]),               KEEP_ON_RESET },
        { &st.hold_zero[0],                 sizeof(st.hold_zero[0]),                    KEEP_ON_RESET },
        { &st.hold_zero[1],                 sizeof(st.hold_zero[1]),                    KEEP_ON_RESET },
        { &st.hold_zero[2],                 sizeof(st.hold_zero[2]),                    KEEP_ON_RESET },
        
        { NULL,                             0,                                          0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
    // Set default values
    setChipModel(MOS6581); 
    
    cpuFrequency = PAL_CYCLES_PER_FRAME * PAL_REFRESH_RATE;
    samplingMethod = SAMPLE_FAST;
    sampleRate = 44100;
    sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate);
    
    setAudioFilter(false);
    setExternalAudioFilter(false);
    
    volume = 100000;
    targetVolume = 100000;
}

ReSID::~ReSID()
{
    delete sid;
}

void
ReSID::reset()
{
    VirtualComponent::reset();
    clearRingbuffer();
    sid->reset();
}

void
ReSID::setChipModel(chip_model model)
{
    if (model != MOS6581 && model != MOS8580) {
        warn("Unknown chip model (%d). Using  MOS8580\n", model);
        model = MOS8580;
    }
    
    chipModel = model;
    sid->set_chip_model(model);
}

void 
ReSID::setAudioFilter(bool enable)
{
    audioFilter = enable;
    sid->enable_filter(enable);
}

void
ReSID::setExternalAudioFilter(bool enable)
{
    externalAudioFilter = enable;
    sid->enable_external_filter(enable);
}

void 
ReSID::setSamplingMethod(sampling_method method)
{
    switch (method) {
        case SAMPLE_FAST:
            debug(2, "Using sample method SAMPLE_FAST\n");
            break;
        case SAMPLE_INTERPOLATE:
            debug(2, "Using sample method SAMPLE_INTERPOLATE\n");
            break;
        case SAMPLE_RESAMPLE_INTERPOLATE:
            debug(2, "Using sample method SAMPLE_RESAMPLE_INTERPOLATE\n");
            break;
        case SAMPLE_RESAMPLE_FAST:
            debug(2, "Using sample method SAMPLE_RESAMPLE_FAST\n");
            break;
        default:
            warn("Unknown sample method. Using SAMPLE_FAST\n");
            method = SAMPLE_FAST;
    }
    
    samplingMethod = method;
    sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate); 
}

void
ReSID::setSampleRate(uint32_t sr)
{
    sampleRate = sr;
    sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate);
}

void 
ReSID::setClockFrequency(uint32_t frequency)
{ 
	cpuFrequency = frequency;
    sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate);
}


void
ReSID::loadFromBuffer(uint8_t **buffer)
{
    VirtualComponent::loadFromBuffer(buffer);

    // reset();
    clearRingbuffer();

    /*
    setChipModel(chipModel);
    setSampleRate(sampleRate);
    setSamplingMethod(samplingMethod);
    setAudioFilter(audioFilter);
    setExternalAudioFilter(externalAudioFilter);
    setClockFrequency(cpuFrequency);
    */
    
    // Push state to reSID
    sid->write_state(st);
}

void
ReSID::saveToBuffer(uint8_t **buffer)
{
    // Pull state from reSID
    st = sid->read_state();
    
    VirtualComponent::saveToBuffer(buffer);
}

uint8_t
ReSID::peek(uint16_t addr)
{	
    return sid->read(addr);
}

void 
ReSID::poke(uint16_t addr, uint8_t value)
{
    // addr &= 0x1F;
    sid->write(addr, value);
}

void
ReSID::execute(uint64_t elapsedCycles)
{
    short buf[2049];
    int buflength = 2048;
    cycle_count delta_t = (cycle_count)elapsedCycles;
    int bufindex = 0;
    
    // TODO: Can't we write directly into Core Audios ringbuffer (for Speedup)?

    // Let reSID compute some sound samples
    while (delta_t) {
        bufindex += sid->clock(delta_t, buf + bufindex, buflength - bufindex);
        // if (delta_t != 0) debug(2, "delta_t = %d\n", delta_t);
    }
    
    // Write samples into ringbuffer (output is silenced in warp mode) 
    
    // float volume = c64->getWarp() ? 0.0f : 0.000005f;
    for (int i = 0; i < bufindex; i++) {
        writeData((float)buf[i]);
    }

    // fprintf(stderr,"wrote %d samples\n", bufindex);
}

void 
ReSID::run()
{
    clearRingbuffer();
}

void 
ReSID::halt()
{
    clearRingbuffer();
}

void
ReSID::clearRingbuffer()
{
    debug(4,"Clearing ringbuffer\n");

    // Reset ringbuffer contents
    for (unsigned i = 0; i < bufferSize; i++) {
        ringBuffer[i] = 0.0f;
    }
    
    // Reset read pointer and put write pointer somewhat ahead
    readPtr = 0;
    alignWritePtr();
}

float
ReSID::readData()
{
    readDataCnt++;
    
    // Check for buffer underflow
    if (readPtr == writePtr)
        debug(4, "SID RINGBUFFER UNDERFLOW (%ld)\n", readPtr);
    
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
    value = (volume <= 0) ? 0.0f : value * (float)volume / 100000.0f;
    
    // Advance read pointer
    readPtr++;
    if (readPtr == bufferSize)
        readPtr = 0;

	return value;
}

void
ReSID::readMonoSamples(float *target, size_t n)
{
    // static unsigned j = 0; /* For debugging */

    for (size_t i = 0; i < n; i++) {
        float value = readData();
        // float value = 5.0 * sin(float(j++)*(2*3.14159265)*(440.0/44100.0));
        target[i] = value;
    }
}

void
ReSID::readStereoSamples(float *target1, float *target2, size_t n)
{
    for (unsigned i = 0; i < n; i++) {
        float value = readData();
        target1[i] = target2[i] = value;
    }
}

void
ReSID::readStereoSamplesInterleaved(float *target, size_t n)
{
    for (unsigned i = 0; i < n; i++) {
        float value = readData();
        target[i*2] = value;
        target[i*2+1] = value;
    }
}

inline void
ReSID::writeData(float data)
{
    writeDataCnt++;
    
    // Check for buffer overflow
    if (readPtr == writePtr) {

        debug(4, "SID RINGBUFFER OVERFLOW (%ld)\n", writePtr);
        
        if (!c64->getWarp()) // In real-time mode, we put the write ptr somewhat ahead of the read ptr
            alignWritePtr();
        else
            return; // In warp mode, we don't advance the write ptr to avoid crack noises
    }
    
#if 0
    // Adjust volume
    if (volume != targetVolume) {
        if (volume < targetVolume) {
            volume += MIN(volumeDelta, targetVolume - volume);
        } else {
            volume -= MIN(volumeDelta, volume - targetVolume);
        }
    }
#endif 
    
    // Write sound sample
    // float scale = (volume <= 0) ? 0.0f : 0.000005f * (float)volume / 100000.0f;
    float scale = 0.000005f;
    ringBuffer[writePtr] = data * scale;
    
    // Advance write pointer
    writePtr++;
    if (writePtr == bufferSize)
        writePtr = 0;
}


void
ReSID::dumpState()
{
	msg("SID\n");
	msg("---\n\n");
	msg("   Sample rate : %d\n", sampleRate);
	msg(" CPU frequency : %d\n", cpuFrequency);
	msg("   Buffer size : %d\n", bufferSize);
	msg("\n");
}


