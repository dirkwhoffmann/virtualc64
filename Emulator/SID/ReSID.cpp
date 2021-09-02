// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "ReSID.h"
#include "C64.h"
#include "IO.h"

ReSID::ReSID(C64 &ref, Muxer &bridgeref, int n) : SubComponent(ref), bridge(bridgeref), nr(n)
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
ReSID::_reset(bool hard)
{
    assert(sid);

    RESET_SNAPSHOT_ITEMS(hard)
    
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
ReSID::_inspect() const
{
    synchronized {
        
        reSID::SID::State state = sid->read_state();
        u8 *reg = (u8 *)state.sid_register;
        
        info.volume = reg[0x18] & 0xF;
        info.filterModeBits = reg[0x18] & 0xF0;
        info.filterType = reg[0x18] & 0x70;
        info.filterCutoff = (reg[0x16] << 3) | (reg[0x15] & 0x07);
        info.filterResonance = reg[0x17] >> 4;
        info.filterEnableBits = reg[0x17] & 0x0F;
        
        for (isize i = 0; i < 3; i++, reg += 7) {
            
            for (isize j = 0; j < 7; j++) voiceInfo[i].reg[j] = reg[j];
            voiceInfo[i].frequency = HI_LO(reg[0x1], reg[0x0]);
            voiceInfo[i].pulseWidth = ((reg[0x3] & 0xF) << 8) | reg[0x02];
            voiceInfo[i].waveform = reg[0x4] & 0xF0;
            voiceInfo[i].ringMod = (reg[0x4] & 0x4) != 0;
            voiceInfo[i].hardSync = (reg[0x4] & 0x2) != 0;
            voiceInfo[i].gateBit = (reg[0x4] & 0x1) != 0;
            voiceInfo[i].testBit = (reg[0x4] & 0x8) != 0;
            voiceInfo[i].attackRate = reg[0x5] >> 4;
            voiceInfo[i].decayRate = reg[0x5] & 0xF;
            voiceInfo[i].sustainRate = reg[0x6] >> 4;
            voiceInfo[i].releaseRate = reg[0x6] & 0xF;
        }
    }
}

void
ReSID::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    reSID::SID::State state = sid->read_state();
    u8 *reg = (u8 *)state.sid_register;
    u8 ft = reg[0x18] & 0x70;
    string fts =
    ft == FASTSID_LOW_PASS ? "LOW_PASS" :
    ft == FASTSID_HIGH_PASS ? "HIGH_PASS" :
    ft == FASTSID_BAND_PASS ? "BAND_PASS" : "???";
    
    if (category & dump::State) {
   
        os << tab("Chip");
        os << "ReSID " << dec(nr) << std::endl;
        os << tab("Model");
        os << SIDRevisionEnum::key(getRevision()) << std::endl;
        os << tab("Sampling rate");
        os << getSampleRate() << std::endl;
        os << tab("CPU frequency");
        os << dec(getClockFrequency()) << std::endl;
        os << tab("Emulate filter");
        os << bol(getAudioFilter()) << std::endl;
        os << tab("Volume");
        os << dec((u8)(reg[0x18] & 0xF)) << std::endl;
        os << tab("Filter type");
        os << fts << std::endl;
        os << tab("Filter cut off");
        os << dec((u16)(reg[0x16] << 3 | (reg[0x15] & 0x07))) << std::endl;
        os << tab("Filter resonance");
        os << dec((u8)(reg[0x17] >> 4)) << std::endl;
        os << tab("Filter enable bits");
        os << hex((u8)(reg[0x17] & 0x0F));
    }
    
    if (category & dump::Registers) {
   
        for (isize i = 0; i <= 0x1C; i++) {
   
            os << "  " << hex((u8)i) << ": " << hex(reg[i]);
            if ((i + 1) % 8 == 0) os << std::endl;
        }
    }
}

isize
ReSID::didLoadFromBuffer(const u8 *buffer)
{
    sid->write_state(st);
    return 0;
}
 
isize
ReSID::willSaveToBuffer(const u8 *buffer)
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
    
    suspended { sid->set_chip_model((reSID::chip_model)revision); }
        
    assert((SIDRevision)sid->sid_model == revision);
    trace(SID_DEBUG, "Emulating SID revision %s.\n", SIDRevisionEnum::key(revision));
}

void
ReSID::setSampleRate(double value)
{
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
    
    suspended { sid->enable_filter(value); }
    
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
    
    suspended {
        sid->set_sampling_parameters((double)clockFrequency,
                                     (reSID::sampling_method)samplingMethod,
                                     (double)sampleRate);
    }
    
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
ReSID::executeCycles(isize numCycles, SampleStream &stream)
{
    short buf[2049];
    isize buflength = 2048;
    
    if (numCycles > PAL_CYCLES_PER_SECOND) {
        warn("Number of missing SID cycles is far too large\n");
        numCycles = PAL_CYCLES_PER_SECOND;
    }
    
    // Let reSID compute sound samples
    isize samples = 0;
    reSID::cycle_count cycles = (reSID::cycle_count)numCycles;
    while (cycles) {
        int resid = sid->clock(cycles, buf + samples, int(buflength) - int(samples));
        samples += (isize)resid;
    }
    
    // Check for a buffer overflow
    if (unlikely(samples > stream.free())) {
        warn("SID %d: SAMPLE BUFFER OVERFLOW", nr);
        stream.clear();
    }
    
    // Write samples into ringbuffer
    if (samples) { for (isize i = 0; i < samples; i++) stream.write(buf[i]); }
    
    return samples;
}

i64
ReSID::executeCycles(isize numCycles)
{
    return executeCycles(numCycles, bridge.sidStream[nr]);
}
