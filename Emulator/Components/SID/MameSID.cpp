// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "MameSID.h"
#include "C64.h"
#include "IOUtils.h"

namespace vc64 {

MameSID::MameSID(C64 &ref, int n) : SubComponent(ref), nr(n)
{
    model = MOS_6581;
    emulateFilter = true;
    sampleRate = 44100;

    sid = new SID6581_t();
    sid->type = (int)model;
    sid->PCMfreq = (uint16_t)sampleRate;
    sid->clock = PAL_CLOCK_FREQUENCY;

    // initialize SID engine
    /*
    m_token->device = this;
    m_token->mixer_channel = m_stream;
    m_token->PCMfreq = machine().sample_rate();
    m_token->clock = clock();
    m_token->type = m_variant;
    */

    sid->init();
    // sidInitWaveformTables(model);

    // save_state(m_token.get());

    /*
    sid->set_chip_model(reSID::MOS6581);
    sid->set_sampling_parameters((double)PAL_CLOCK_FREQUENCY,
                                 reSID::SAMPLE_FAST,
                                 (double)sampleRate);
    sid->enable_filter(emulateFilter);
    */
}

MameSID::~MameSID()
{
    delete sid;
}

const char *
MameSID::getDescription() const
{
    assert(usize(nr) < 4);

    return
    nr == 0 ? "MameSID0" :
    nr == 1 ? "MameSID1" : 
    nr == 2 ? "MameSID2" : "MameSID3";
}

void
MameSID::_reset(bool hard)
{
    assert(sid);

    RESET_SNAPSHOT_ITEMS(hard)

    sid->reset();

    /*
    sid->set_chip_model((reSID::chip_model)model);
    sid->set_sampling_parameters((double)clockFrequency,
                                 (reSID::sampling_method)samplingMethod,
                                 (double)sampleRate);
    sid->enable_filter(emulateFilter);
    */
}

u32
MameSID::getClockFrequency() const
{
    assert(sid->clock == cpuFrequency);
    return cpuFrequency;
}

void
MameSID::setClockFrequency(u32 frequency)
{
    trace(SID_DEBUG, "Setting clock frequency to %d\n", frequency);

    cpuFrequency = frequency;
    sid->clock = frequency;
}

void
MameSID::_inspect() const
{
    {   SYNCHRONIZED

        /*
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
        */
    }
}

void
MameSID::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    /*
    reSID::SID::State state = sid->read_state();
    u8 *reg = (u8 *)state.sid_register;
    u8 ft = reg[0x18] & 0x70;
    string fts =
    ft == FASTSID_LOW_PASS ? "LOW_PASS" :
    ft == FASTSID_HIGH_PASS ? "HIGH_PASS" :
    ft == FASTSID_BAND_PASS ? "BAND_PASS" : "???";

    if (category == Category::State) {

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

        for (isize i = 0, col = 0, row = 0; i <= 0x1C; i++, row = i / 4, col = i % 4) {

            if (col == 0 && row != 0) os << std::endl;
            if (col != 0) os << "  ";

            os << hex((u8)i) << ": " << hex(reg[i]);
        }
    }
    */
}

isize
MameSID::didLoadFromBuffer(const u8 *buffer)
{
    // TODO: sid->write_state(st);
    return 0;
}

isize
MameSID::willSaveToBuffer(u8 *buffer)
{
    // TODO: st = sid->read_state();
    return 0;
}

SIDRevision
MameSID::getRevision() const
{
    assert((SIDRevision)sid->type == model);
    return model;
}

void
MameSID::setRevision(SIDRevision revision)
{
    {   SUSPENDED

        model = revision;
        sid->type = (int)revision;
        sidInitWaveformTables(int(model));
    }

    trace(SID_DEBUG, "Emulating SID revision %s.\n", SIDRevisionEnum::key(revision));
}

double 
MameSID::getSampleRate() const
{
    return sampleRate;
}

void
MameSID::setSampleRate(double value)
{
    sampleRate = value;
    sid->sample_rate = (int)value;

    trace(SID_DEBUG, "Setting sample rate to %f samples per second\n", sampleRate);
}

bool 
MameSID::getAudioFilter() const
{
    assert(emulateFilter == sid->filter.Enabled);
    return emulateFilter;
}


void
MameSID::setAudioFilter(bool value)
{
    assert(!isRunning());

    {   SUSPENDED

        emulateFilter = value;
        sid->filter.Enabled = value;
    }

    trace(SID_DEBUG, "%s audio filter emulation.\n", value ? "Enabling" : "Disabling");
}

u8
MameSID::peek(u16 addr)
{
    return u8(sid->port_r(addr));
}

void
MameSID::poke(u16 addr, u8 value)
{
    sid->port_w(addr, value);
}

isize
MameSID::executeCycles(isize numCycles, SampleStream &stream)
{
    static constexpr isize buflength = 2048;
    short buf[buflength];

    if (numCycles > PAL_CYCLES_PER_SECOND) {
        warn("Number of missing SID cycles is far too large\n");
        numCycles = PAL_CYCLES_PER_SECOND;
    }

    // Compute the number of sound samples to produce
    executedCycles += numCycles;
    double samplesPerCycle = (double)sampleRate / (double)cpuFrequency;
    isize shouldHave = (isize)(executedCycles * samplesPerCycle);

    // How many sound samples are missing?
    isize samples = isize(shouldHave - computedSamples);
    computedSamples = shouldHave;

    // Check consistency
    if (samples < 0 || samples > buflength) {
        warn("Number of missing cycles is out of bounds: %ld\n", samples);
        executedCycles = samples = 0;
    }
    // Ensure the temporary buffer is large enough to hold all samples
    /*
    if (samples > buflength) {
        warn("Number of missing sound samples exceeds buffer size\n");
        samples = buflength;
    }
    */

    // Compute missing samples
    sid->fill_buffer(buf, (int)samples);

    // Check for a buffer overflow
    if (samples > stream.free()) {
        warn("SID %d: SAMPLE BUFFER OVERFLOW", nr);
        stream.clear();
    }

    // Write samples into ringbuffer
    i16 min = 0, max = 0;
    for (isize i = 0; i < samples; i++) {
        short s = buf[i];
        min = i == 0 ? s : std::min(min, s);
        max = i == 0 ? s : std::max(max, s);
        stream.write(s);
    }

    // trace(true, "Computed %ld samples (%d - %d)\n", samples, min, max);

    return samples;
}

isize
MameSID::executeCycles(isize numCycles)
{
    return executeCycles(numCycles, muxer.sidStream[nr]);
}

}
