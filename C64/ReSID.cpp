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
        { &sampleRate,          sizeof(sampleRate),             KEEP_ON_RESET },
        { &emulateFilter,       sizeof(emulateFilter),          KEEP_ON_RESET },
        
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
    sid->set_chip_model(reSID::MOS6581);
    sampleRate = 44100;
    sid->set_sampling_parameters((double)PAL_CYCLES_PER_FRAME * PAL_REFRESH_RATE,
                                 reSID::SAMPLE_FAST,
                                 (double)sampleRate);
    
    setAudioFilter(true);
}

ReSID::~ReSID()
{
    delete sid;
}

void
ReSID::reset()
{
    VirtualComponent::reset();
    sid->reset();
}

void
ReSID::setChipModel(SIDChipModel model)
{
    sid->set_chip_model((reSID::chip_model)model);
}

void
ReSID::setClockFrequency(uint32_t value)
{
    double frequency = (double)value;
    reSID::sampling_method method = sid->sampling;
    double rate = (double)sampleRate;
    
    sid->set_sampling_parameters(frequency, method, rate);
}

void
ReSID::setSampleRate(uint32_t value)
{
    double frequency = sid->clock_frequency;
    reSID::sampling_method method = sid->sampling;
    double rate = (double)value;
    
    sid->set_sampling_parameters(frequency, method, rate);
}

void 
ReSID::setAudioFilter(bool value)
{
    emulateFilter = value;
    sid->enable_filter(value);
}

void 
ReSID::setSamplingMethod(SamplingMethod value)
{
    double frequency = sid->clock_frequency;
    reSID::sampling_method method = (reSID::sampling_method)value;
    double rate = (double)sampleRate;
    
    sid->set_sampling_parameters(frequency, method, rate);
}

void
ReSID::loadFromBuffer(uint8_t **buffer)
{
    VirtualComponent::loadFromBuffer(buffer);
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
        bridge->writeData(buf, bufindex);
    }
}

void
ReSID::dumpState()
{
	msg("SID\n");
	msg("---\n\n");
    msg("   Sample rate : %d\n", getSampleRate());
    msg(" CPU frequency : %d\n", getClockFrequency());
	msg("\n");
}


