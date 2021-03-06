// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

ReSID::ReSID(C64 &ref, SIDBridge &bridgeref, int n) : C64Component(ref), bridge(bridgeref), nr(n)
{
    model = MOS_6581;
    emulateFilter = true;
    sampleRate = 44100;

    sid = new reSID::SID();
    sid->set_chip_model(reSID::MOS6581);
    sid->set_sampling_parameters((double)PAL_CLOCK_FREQUENCY,
                                 reSID::SAMPLE_FAST,
                                 (double)sampleRate);
    sid->enable_filter(emulateFilter);
}

ReSID::~ReSID()
{
    delete sid;
}

void
ReSID::_reset()
{
    assert(sid);

    RESET_SNAPSHOT_ITEMS
    
    // Resetting reSID is done by creating a new reSID object. We don't call
    // reSID::reset() because it only performs a soft reset.

    delete sid;
    sid = new reSID::SID();
    
    sid->set_chip_model((reSID::chip_model)model);
    sid->set_sampling_parameters((double)clockFrequency,
                                 (reSID::sampling_method)samplingMethod,
                                 (double)sampleRate);
    sid->enable_filter(emulateFilter);
}

u32
ReSID::getClockFrequency() const
{
    assert((u32)sid->clock_frequency == clockFrequency);
    return (u32)sid->clock_frequency;
}

void
ReSID::setClockFrequency(u32 frequency)
{
    trace(SID_DEBUG, "Setting clock frequency to %d\n", frequency);

    clockFrequency = frequency;
    
    sid->set_sampling_parameters((double)clockFrequency,
                                 (reSID::sampling_method)samplingMethod,
                                 (double)sampleRate);
    
    assert((u32)sid->clock_frequency == clockFrequency);
}

void
ReSID::_inspect()
{
    synchronized {
        
        reSID::SID::State state = sid->read_state();
        u8 *reg = (u8 *)state.sid_register;
        
        info.volume = reg[0x18] & 0x0F;
        info.filterModeBits = reg[0x18] & 0xF0;
        info.filterType = reg[0x18] & 0x70;
        info.filterCutoff = (reg[0x16] << 3) | (reg[0x15] & 0x07);
        info.filterResonance = reg[0x17] >> 4;
        info.filterEnableBits = reg[0x17] & 0x0F;
        
        for (unsigned i = 0; i < 3; i++, reg += 7) {
            
            for (unsigned j = 0; j < 7; j++) voiceInfo[i].reg[j] = reg[j];
            voiceInfo[i].frequency = HI_LO(reg[0x01], reg[0x00]);
            voiceInfo[i].pulseWidth = ((reg[3] & 0x0F) << 8) | reg[0x02];
            voiceInfo[i].waveform = reg[0x04] & 0xF0;
            voiceInfo[i].ringMod = (reg[0x04] & 0x04) != 0;
            voiceInfo[i].hardSync = (reg[0x04] & 0x02) != 0;
            voiceInfo[i].gateBit = (reg[0x04] & 0x01) != 0;
            voiceInfo[i].testBit = (reg[0x04] & 0x08) != 0;
            voiceInfo[i].attackRate = reg[0x05] >> 4;
            voiceInfo[i].decayRate = reg[0x05] & 0x0F;
            voiceInfo[i].sustainRate = reg[0x06] >> 4;
            voiceInfo[i].releaseRate = reg[0x06] & 0x0F;
        }
    }
}

usize
ReSID::didLoadFromBuffer(u8 *buffer)
{
    sid->write_state(st);
    return 0;
}
 
usize
ReSID::willSaveToBuffer(u8 *buffer)
{
    st = sid->read_state();
    return 0;
}

SIDRevision
ReSID::getRevision() const
{
    assert((SIDRevision)sid->sid_model == model);
    return model;
}

void
ReSID::setRevision(SIDRevision revision)
{
    assert(!isRunning());

    assert(revision == 0 || revision == 1);
    model = revision;
    
    suspend();
    sid->set_chip_model((reSID::chip_model)revision);
    resume();
        
    assert((SIDRevision)sid->sid_model == revision);
    trace(SID_DEBUG, "Emulating SID revision %s.\n", SIDRevisionEnum::key(revision));
}

void
ReSID::setSampleRate(double value)
{
    // assert(!isRunning());

    sampleRate = value;

    sid->set_sampling_parameters((double)clockFrequency,
                                 (reSID::sampling_method)samplingMethod,
                                 sampleRate);
    
    trace(SID_DEBUG, "Setting sample rate to %f samples per second\n", sampleRate);
}

void 
ReSID::setAudioFilter(bool value)
{
    assert(!isRunning());

    emulateFilter = value;
    
    suspend();
    sid->enable_filter(value);
    resume();
    
    trace(SID_DEBUG, "%s audio filter emulation.\n", value ? "Enabling" : "Disabling");
}

SamplingMethod
ReSID::getSamplingMethod() const
{
    assert((SamplingMethod)sid->sampling == samplingMethod);
    return samplingMethod;
}

void 
ReSID::setSamplingMethod(SamplingMethod value)
{
    assert(!isRunning());
    
    switch(value) {
        case SAMPLING_FAST:
            trace(SID_DEBUG, "Using sampling method SAMPLE_FAST.\n");
            break;
        case SAMPLING_INTERPOLATE:
            trace(SID_DEBUG, "Using sampling method SAMPLE_INTERPOLATE.\n");
            break;
        case SAMPLING_RESAMPLE:
            trace(SID_DEBUG, "Using sampling method SAMPLE_RESAMPLE.\n");
            break;
        case SAMPLING_RESAMPLE_FASTMEM:
            warn("SAMPLE_RESAMPLE_FASTMEM not supported. Using SAMPLE_INTERPOLATE.\n");
            value = SAMPLING_INTERPOLATE;
            break;
        default:
            warn("Unknown sampling method: %lld\n", value);
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

i64
ReSID::executeCycles(usize numCycles, SampleStream &stream)
{
    short buf[2049];
    usize buflength = 2048;
    
    if (numCycles > PAL_CYCLES_PER_SECOND) {
        warn("Number of missing SID cycles is far too large\n");
        numCycles = PAL_CYCLES_PER_SECOND;
    }
    
    // Let reSID compute sound samples
    usize samples = 0;
    reSID::cycle_count cycles = (reSID::cycle_count)numCycles;
    while (cycles) {
        int resid = sid->clock(cycles, buf + samples, int(buflength) - int(samples));
        assert(resid >= 0); // TODO: REMOVE AFTER A WHILE
        samples += (usize)resid;
    }
    
    // Check for a buffer overflow
    if (unlikely(samples > stream.free())) {
        warn("SID %d: SAMPLE BUFFER OVERFLOW", nr);
        stream.clear();
    }
    
    // Write samples into ringbuffer
    if (samples) { for (usize i = 0; i < samples; i++) stream.write(buf[i]); }
    
    return samples;
}

i64
ReSID::executeCycles(usize numCycles)
{
    return executeCycles(numCycles, bridge.sidStream[nr]);
}
