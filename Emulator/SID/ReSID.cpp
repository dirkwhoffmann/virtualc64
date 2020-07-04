// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

ReSID::ReSID(C64 &ref) : C64Component(ref)
{
	setDescription("ReSID");

    model = MOS_6581;
    emulateFilter = true;
    sampleRate = 44100;

    sid = new reSID::SID();
    sid->set_chip_model(reSID::MOS6581);
    sid->set_sampling_parameters((double)PAL_CLOCK_FREQUENCY,
                                 reSID::SAMPLE_FAST,
                                 (double)sampleRate);
    sid->enable_filter(emulateFilter);
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Configuration items
        { &sampleRate,          sizeof(sampleRate),             KEEP_ON_RESET },
        { &emulateFilter,       sizeof(emulateFilter),          KEEP_ON_RESET },
        
        // ReSID state
        { st.sid_register,                  sizeof(st.sid_register),                  KEEP_ON_RESET },
        { &st.bus_value,                    sizeof(st.bus_value),                     KEEP_ON_RESET },
        { &st.bus_value_ttl,                sizeof(st.bus_value_ttl),                 KEEP_ON_RESET },
        { &st.write_pipeline,               sizeof(st.write_pipeline),                KEEP_ON_RESET },
        { &st.write_address,                sizeof(st.write_address),                 KEEP_ON_RESET },
        { &st.voice_mask,                   sizeof(st.voice_mask),                    KEEP_ON_RESET },
        { &st.accumulator[0],               sizeof(st.accumulator[0]),                KEEP_ON_RESET },
        { &st.accumulator[1],               sizeof(st.accumulator[1]),                KEEP_ON_RESET },
        { &st.accumulator[2],               sizeof(st.accumulator[2]),                KEEP_ON_RESET },
        { &st.shift_register[0],            sizeof(st.shift_register[0]),             KEEP_ON_RESET },
        { &st.shift_register[1],            sizeof(st.shift_register[1]),             KEEP_ON_RESET },
        { &st.shift_register[2],            sizeof(st.shift_register[2]),             KEEP_ON_RESET },
        { &st.shift_register_reset[0],      sizeof(st.shift_register_reset[0]),       KEEP_ON_RESET },
        { &st.shift_register_reset[1],      sizeof(st.shift_register_reset[1]),       KEEP_ON_RESET },
        { &st.shift_register_reset[2],      sizeof(st.shift_register_reset[2]),       KEEP_ON_RESET },
        { &st.shift_pipeline[0],            sizeof(st.shift_pipeline[0]),             KEEP_ON_RESET },
        { &st.shift_pipeline[1],            sizeof(st.shift_pipeline[1]),             KEEP_ON_RESET },
        { &st.shift_pipeline[2],            sizeof(st.shift_pipeline[2]),             KEEP_ON_RESET },
        { &st.pulse_output[0],              sizeof(st.pulse_output[0]),               KEEP_ON_RESET },
        { &st.pulse_output[1],              sizeof(st.pulse_output[1]),               KEEP_ON_RESET },
        { &st.pulse_output[2],              sizeof(st.pulse_output[2]),               KEEP_ON_RESET },
        { &st.floating_output_ttl[0],       sizeof(st.floating_output_ttl[0]),        KEEP_ON_RESET },
        { &st.floating_output_ttl[1],       sizeof(st.floating_output_ttl[1]),        KEEP_ON_RESET },
        { &st.floating_output_ttl[2],       sizeof(st.floating_output_ttl[2]),        KEEP_ON_RESET },
        { &st.rate_counter[0],              sizeof(st.rate_counter[0]),               KEEP_ON_RESET },
        { &st.rate_counter[1],              sizeof(st.rate_counter[1]),               KEEP_ON_RESET },
        { &st.rate_counter[2],              sizeof(st.rate_counter[2]),               KEEP_ON_RESET },
        { &st.rate_counter_period[0],       sizeof(st.rate_counter_period[0]),        KEEP_ON_RESET },
        { &st.rate_counter_period[1],       sizeof(st.rate_counter_period[1]),        KEEP_ON_RESET },
        { &st.rate_counter_period[2],       sizeof(st.rate_counter_period[2]),        KEEP_ON_RESET },
        { &st.exponential_counter[0],       sizeof(st.exponential_counter[0]),        KEEP_ON_RESET },
        { &st.exponential_counter[1],       sizeof(st.exponential_counter[1]),        KEEP_ON_RESET },
        { &st.exponential_counter[2],       sizeof(st.exponential_counter[2]),        KEEP_ON_RESET },
        { &st.exponential_counter_period[0],sizeof(st.exponential_counter_period[0]), KEEP_ON_RESET },
        { &st.exponential_counter_period[1],sizeof(st.exponential_counter_period[1]), KEEP_ON_RESET },
        { &st.exponential_counter_period[2],sizeof(st.exponential_counter_period[2]), KEEP_ON_RESET },
        { &st.envelope_counter[0],          sizeof(st.envelope_counter[0]),           KEEP_ON_RESET },
        { &st.envelope_counter[1],          sizeof(st.envelope_counter[1]),           KEEP_ON_RESET },
        { &st.envelope_counter[2],          sizeof(st.envelope_counter[2]),           KEEP_ON_RESET },
        { &st.envelope_state[0],            sizeof(st.envelope_state[0]),             KEEP_ON_RESET },
        { &st.envelope_state[1],            sizeof(st.envelope_state[1]),             KEEP_ON_RESET },
        { &st.envelope_state[2],            sizeof(st.envelope_state[2]),             KEEP_ON_RESET },
        { &st.hold_zero[0],                 sizeof(st.hold_zero[0]),                  KEEP_ON_RESET },
        { &st.hold_zero[1],                 sizeof(st.hold_zero[1]),                  KEEP_ON_RESET },
        { &st.hold_zero[2],                 sizeof(st.hold_zero[2]),                  KEEP_ON_RESET },
        { &st.envelope_pipeline[0],         sizeof(st.envelope_pipeline[0]),          KEEP_ON_RESET },
        { &st.envelope_pipeline[1],         sizeof(st.envelope_pipeline[1]),          KEEP_ON_RESET },
        { &st.envelope_pipeline[2],         sizeof(st.envelope_pipeline[2]),          KEEP_ON_RESET },
        
        { NULL,                             0,                                        0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

ReSID::~ReSID()
{
    delete sid;
}

void
ReSID::reset()
{
    HardwareComponent::reset();
    
    // Create new reSID object
    // Note: We don't use reSID::reset() which only performs a soft reset
    assert(sid != NULL);
    delete sid;
    sid = new reSID::SID();
    
    // Reconfigure reSID
    sid->set_chip_model((reSID::chip_model)model);
    sid->set_sampling_parameters((double)clockFrequency,
                                 (reSID::sampling_method)samplingMethod,
                                 (double)sampleRate);
    sid->enable_filter(emulateFilter);
}

void
ReSID::setModel(SIDModel m)
{
    assert(m == 0 || m == 1);
    model = m;
    
    suspend();
    sid->set_chip_model((reSID::chip_model)m);
    resume();
    
    // MOS8580 emulation seems to be problematic when combined with filters.
    // TODO: Disable filters in combination with this chip
    
    assert((SIDModel)sid->sid_model == model);
    debug(SID_DEBUG, "Emulating SID model %s.\n",
          (model == MOS_6581) ? "MOS6581" :
          (model == MOS_8580) ? "MOS8580" : "?");
}

void
ReSID::setClockFrequency(u32 value)
{
    clockFrequency = value;
    
    suspend();
    sid->set_sampling_parameters((double)clockFrequency,
                                 (reSID::sampling_method)samplingMethod,
                                 (double)sampleRate);
    resume();
    
    assert((u32)sid->clock_frequency == clockFrequency);
    debug(SID_DEBUG, "Setting clock frequency to %d cycles per second.\n", clockFrequency);
}

void
ReSID::setSampleRate(u32 value)
{
    sampleRate = value;
    
    // suspend();
    sid->set_sampling_parameters((double)clockFrequency,
                                 (reSID::sampling_method)samplingMethod,
                                 (double)sampleRate);
    // resume();
    
    debug(SID_DEBUG, "Setting sample rate to %d samples per second.\n", sampleRate);
}

void 
ReSID::setAudioFilter(bool value)
{
    emulateFilter = value;
    
    suspend();
    sid->enable_filter(value);
    // sid->filter._reset();
    resume();
    
    debug(SID_DEBUG, "%s audio filter emulation.\n", value ? "Enabling" : "Disabling");
}

void 
ReSID::setSamplingMethod(SamplingMethod value)
{
    switch(value) {
        case SID_SAMPLE_FAST:
            debug(SID_DEBUG, "Using sampling method SAMPLE_FAST.\n");
            break;
        case SID_SAMPLE_INTERPOLATE:
            debug(SID_DEBUG, "Using sampling method SAMPLE_INTERPOLATE.\n");
            break;
        case SID_SAMPLE_RESAMPLE:
            debug(SID_DEBUG, "Using sampling method SAMPLE_RESAMPLE.\n");
            break;
        case SID_SAMPLE_RESAMPLE_FASTMEM:
            warn("SAMPLE_RESAMPLE_FASTMEM not supported. Using SAMPLE_INTERPOLATE.\n");
            value = SID_SAMPLE_INTERPOLATE;
            break;
        default:
            warn("Unknown sampling method: %d\n", value);
    }

    samplingMethod = value;
    
    suspend();
    sid->set_sampling_parameters((double)clockFrequency,
                                 (reSID::sampling_method)samplingMethod,
                                 (double)sampleRate);
    resume();
    
    assert((SamplingMethod)sid->sampling == samplingMethod);
}

u8
ReSID::peek(u16 addr)
{	
    return sid->read(addr);
}

void 
ReSID::poke(u16 addr, u8 value)
{
    sid->write(addr, value);
}

void
ReSID::execute(u64 elapsedCycles)
{
    short buf[2049];
    int buflength = 2048;
    
    if (elapsedCycles > PAL_CYCLES_PER_SECOND) {
        warn("Number of missing SID cycles is far too large.\n");
        elapsedCycles = PAL_CYCLES_PER_SECOND;
    }

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

SIDInfo
ReSID::getInfo()
{
    SIDInfo info;
    reSID::SID::State state = sid->read_state();
    u8 *reg = (u8 *)state.sid_register;
    
    info.volume = reg[0x18] & 0x0F;
    info.filterModeBits = reg[0x18] & 0xF0;
    info.filterType = reg[0x18] & 0x70;
    info.filterCutoff = (reg[0x16] << 3) | (reg[0x15] & 0x07);
    info.filterResonance = reg[0x17] >> 4;
    info.filterEnableBits = reg[0x17] & 0x0F;
    return info;
}

VoiceInfo
ReSID::getVoiceInfo(unsigned voice)
{
    VoiceInfo info;
    reSID::SID::State state = sid->read_state();
    u8 *sidreg = (u8 *)state.sid_register + (voice * 7);
    
    for (unsigned j = 0; j < 7; j++) info.reg[j] = sidreg[j];
    info.frequency = HI_LO(sidreg[0x01], sidreg[0x00]);
    info.pulseWidth = ((sidreg[3] & 0x0F) << 8) | sidreg[0x02];
    info.waveform = sidreg[0x04] & 0xF0;
    info.ringMod = (sidreg[0x04] & 0x04) != 0;
    info.hardSync = (sidreg[0x04] & 0x02) != 0;
    info.gateBit = (sidreg[0x04] & 0x01) != 0;
    info.testBit = (sidreg[0x04] & 0x08) != 0;
    info.attackRate = sidreg[0x05] >> 4;
    info.decayRate = sidreg[0x05] & 0x0F;
    info.sustainRate = sidreg[0x06] >> 4;
    info.releaseRate = sidreg[0x06] & 0x0F;
    // info.filterOn = GET_BIT(state->sid_register[0x17], voice) != 0;
    
    return info;
}


