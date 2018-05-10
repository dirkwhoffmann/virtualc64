/*
 * (C) 2011 - 2017 Dirk W. Hoffmann. All rights reserved.
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

    sid = new reSID::SID();
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Configuration items
        // { &chipModel,           sizeof(chipModel),              KEEP_ON_RESET },
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
    setChipModel(MOS_6581);
    
    cpuFrequency = PAL_CYCLES_PER_FRAME * PAL_REFRESH_RATE;
    samplingMethod = SID_SAMPLE_FAST;
    sampleRate = 44100;
    sid->set_sampling_parameters(cpuFrequency, (reSID::sampling_method)samplingMethod, sampleRate);
    
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

SIDChipModel
ReSID::getChipModel()
{
    return (SIDChipModel)sid->sid_model;
}

void
ReSID::setChipModel(SIDChipModel model)
{
    sid->set_chip_model((reSID::chip_model)model);
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
ReSID::setSamplingMethod(SamplingMethod method)
{
    switch (method) {
        case SID_SAMPLE_FAST:
            debug(2, "Using sample method SAMPLE_FAST\n");
            break;
        case SID_SAMPLE_INTERPOLATE:
            debug(2, "Using sample method SAMPLE_INTERPOLATE\n");
            break;
        case SID_SAMPLE_RESAMPLE:
            debug(2, "Using sample method SAMPLE_RESAMPLE\n");
            break;
        case SID_SAMPLE_RESAMPLE_FASTMEM:
            debug(2, "Using sample method SAMPLE_RESAMPLE_FASTMEM\n");
            break;
        default:
            warn("Unknown sample method. Using SAMPLE_FAST\n");
            method = SID_SAMPLE_FAST;
    }
    
    samplingMethod = method;
    sid->set_sampling_parameters(cpuFrequency, (reSID::sampling_method)samplingMethod, sampleRate);
}

void
ReSID::setSampleRate(uint32_t sr)
{
    sampleRate = sr;
    sid->set_sampling_parameters(cpuFrequency, (reSID::sampling_method)samplingMethod, sampleRate);
}

void 
ReSID::setClockFrequency(uint32_t frequency)
{
    debug("Setting clock freq to %d\n", frequency);
	cpuFrequency = frequency;
    sid->set_sampling_parameters(cpuFrequency, (reSID::sampling_method)samplingMethod, sampleRate);
}


void
ReSID::loadFromBuffer(uint8_t **buffer)
{
    VirtualComponent::loadFromBuffer(buffer);

    clearRingbuffer();
    sid->write_state(st);
}

void
ReSID::saveToBuffer(uint8_t **buffer)
{
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
    reSID::cycle_count delta_t = (reSID::cycle_count)elapsedCycles;
    int bufindex = 0;
    
    // Let reSID compute some sound samples
    while (delta_t) {
        bufindex += sid->clock(delta_t, buf + bufindex, buflength - bufindex);
    }
    
    // Write samples into ringbuffer
    if (bufindex) {
        writeData(buf, bufindex);
    }
    /*
    for (int i = 0; i < bufindex; i++) {
        writeData((float)buf[i]);
    }
    */
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


