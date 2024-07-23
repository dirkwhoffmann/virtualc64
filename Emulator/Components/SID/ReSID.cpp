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
#include "ReSID.h"
#include "C64.h"
#include "IOUtils.h"

namespace vc64 {

ReSID::ReSID(C64 &ref, isize id) : SubComponent(ref, id)
{
    model = MOS_6581;
    emulateFilter = true;
    sampleRate = 44100;

    sid = new reSID::SID();
    sid->set_chip_model(reSID::MOS6581);
    sid->set_sampling_parameters((double)PAL::CLOCK_FREQUENCY,
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

void
ReSID::_dump(Category category, std::ostream& os) const
{
    using namespace util;
    
    auto filter = [&](u8 bits) {

        switch (bits & 0x70) {

            case 0x00: return "NONE";
            case 0x10: return "LOW_PASS";
            case 0x20: return "BAND_PASS";
            case 0x40: return "HIGH_PASS";
            default:   return "INVALID";
        }
    };

    reSID::SID::State state = sid->read_state();
    u8 *reg = (u8 *)state.sid_register;
    // u8 ft = reg[0x18] & 0x70;

    if (category == Category::State) {

        os << tab("Engine");
        os << "ReSID" << dec(objid) << std::endl;
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
        os << filter(reg[0x18]) << std::endl;
        os << tab("Filter cut off");
        os << dec((u16)(reg[0x16] << 3 | (reg[0x15] & 0x07))) << std::endl;
        os << tab("Filter resonance");
        os << dec((u8)(reg[0x17] >> 4)) << std::endl;
        os << tab("Filter enable bits");
        os << hex((u8)(reg[0x17] & 0x0F)) << std::endl;

        os << std::endl;
        for (isize i = 0; i <= 0x1C; i++) {

            isize row = i / 4;
            isize col = i % 4;

            if (col == 0) row == 0 ? os << tab("Registers") : os << tab("");
            os << hex((u8)i) << ": " << hex(reg[i]);
            col == 3 ? os << std::endl : os << "  ";
        }
    }
}

void
ReSID::operator << (SerReader &worker)
{
    serialize(worker);
    sid->write_state(st);
}

void
ReSID::operator << (SerWriter &worker)
{
    st = sid->read_state();
    serialize(worker);
}

void
ReSID::record() const
{
    Inspectable<SIDInfo>::record();
}

void
ReSID::cacheInfo(SIDInfo &info) const
{
    {   SYNCHRONIZED

        reSID::SID::State state = sid->read_state();
        u8 *reg = (u8 *)state.sid_register;

        info.volume = reg[0x18] & 0xF;
        info.filterModeBits = reg[0x18] & 0xF0;
        info.filterType = reg[0x18] & 0x70;
        info.filterCutoff = u16(reg[0x16] << 3 | (reg[0x15] & 0x07));
        info.filterResonance = reg[0x17] >> 4;
        info.filterEnableBits = reg[0x17] & 0x0F;

        for (isize i = 0; i < 3; i++, reg += 7) {

            for (isize j = 0; j < 7; j++) voiceInfo[i].reg[j] = reg[j];
            voiceInfo[i].frequency = HI_LO(reg[0x1], reg[0x0]);
            voiceInfo[i].pulseWidth = u16((reg[0x3] & 0xF) << 8 | reg[0x02]);
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

u32
ReSID::getClockFrequency() const
{
    assert((u32)sid->clock_frequency == clockFrequency);
    return (u32)sid->clock_frequency;
}

void
ReSID::setClockFrequency(u32 frequency)
{
    if (clockFrequency != frequency) {

        clockFrequency = frequency;
        sid->set_sampling_parameters((double)clockFrequency,
                                     (reSID::sampling_method)samplingMethod,
                                     (double)sampleRate);
        trace(SID_DEBUG, "Setting clock frequency to %d\n", frequency);
    }

    assert((u32)sid->clock_frequency == clockFrequency);
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
    assert(revision == 0 || revision == 1);

    if (model != revision) {

        model = revision;
        sid->set_chip_model((reSID::chip_model)revision);
        trace(SID_DEBUG, "Emulating SID revision %s.\n", SIDRevisionEnum::key(revision));
    }

    assert((SIDRevision)sid->sid_model == revision);
}

void
ReSID::setSampleRate(double value)
{
    if (sampleRate != value) {

        sampleRate = value;
        sid->set_sampling_parameters((double)clockFrequency,
                                     (reSID::sampling_method)samplingMethod,
                                     sampleRate);
        trace(SID_DEBUG, "Setting sample rate to %f samples per second\n", sampleRate);
    }
}

void 
ReSID::setAudioFilter(bool value)
{
    if (emulateFilter != value) {

        emulateFilter = value;
        sid->enable_filter(value);

        trace(SID_DEBUG, "%s audio filter emulation.\n", value ? "Enabling" : "Disabling");
    }
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
    if (samplingMethod != value) {
        
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
                warn("Unknown sampling method: %ld\n", value);
        }

        samplingMethod = value;
        sid->set_sampling_parameters((double)clockFrequency,
                                     (reSID::sampling_method)samplingMethod,
                                     (double)sampleRate);
    }

    assert((SamplingMethod)sid->sampling == samplingMethod);
}

u8
ReSID::peek(u16 addr)
{	
    return u8(sid->read(addr));
}

void 
ReSID::poke(u16 addr, u8 value)
{
    sid->write(addr, value);
}

isize
ReSID::executeCycles(isize numCycles, SampleStream &stream)
{
    short buf[2048];
    isize buflength = 2047;

    if (numCycles > PAL::CYCLES_PER_SECOND) {
        warn("Number of missing SID cycles is far too large\n");
        numCycles = PAL::CYCLES_PER_SECOND;
    }
    
    // Let reSID compute sound samples
    isize samples = 0;
    reSID::cycle_count cycles = (reSID::cycle_count)numCycles;
    while (cycles && samples < buflength) {
        int resid = sid->clock(cycles, buf + samples, int(buflength) - int(samples));
        samples += (isize)resid;
    }
    
    // Check for a buffer overflow
    if (unlikely(samples > stream.free())) {
        warn("SID %ld: SAMPLE BUFFER OVERFLOW", objid);
        stream.clear();
    }
    
    // Write samples into ringbuffer
    if (samples) { for (isize i = 0; i < samples; i++) stream.write(buf[i]); }
    
    return samples;
}

}
