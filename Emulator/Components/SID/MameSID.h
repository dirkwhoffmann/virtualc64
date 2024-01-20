// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "SIDTypes.h"
#include "SubComponent.h"
#include "Constants.h"
#include "SIDStreams.h"
#include "mamesid/sid.h"

namespace vc64 {

/* This class is a wrapper around the third-party MAME SID implementation.
 *
 *   Good candidate for testing sound emulation:
 *
 *     - INTERNAT.P00
 *     - DEFEND1.PRG  ("Das Boot" intro music)
 *     - To Norah (Elysium)
 *     - Vortex (LMan)
 */

class MameSID : public SubComponent {

    // Number of this SID (0 = primary SID)
    int nr = 0;

    // Entry point to the MAME SID backend
    SID6581_t *sid = nullptr;

    // Result of the latest inspection
    mutable SIDInfo info = { };
    mutable VoiceInfo voiceInfo[3] = { };

private:

    // Chip model
    SIDRevision model = MOS_6581;

    // Current CPU frequency
    u32 cpuFrequency = PAL_CLOCK_FREQUENCY;

    // Sample rate (usually set to 44.1 kHz or 48.0 kHz)
    double sampleRate = 44100.0;

    // Switches filter emulation on or off
    bool emulateFilter = true;

    // Stores for how many cycles FastSID was executed so far
    i64 executedCycles = 0;

    // Stores how many sound samples were computed so far
    i64 computedSamples = 0;

    // Last value on the data bus
    u8 latchedDataBus = 0;


    //
    // Initializing
    //

public:

    MameSID(C64 &ref, int n);
    ~MameSID();


    //
    // Methods from CoreObject
    //

private:

    const char *getDescription() const override;


    //
    // Methods from CoreComponent
    //

private:

    void _reset(bool hard) override;

    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker

        // This class
        << model
        << cpuFrequency
        << sampleRate
        << emulateFilter

        // Mame SID backend
        << sid->type
        << sid->clock
        << sid->PCMfreq
        << sid->PCMsid
        << sid->PCMsidNoise
        << sid->reg
        << sid->masterVolume
        << sid->masterVolumeAmplIndex
        << sid->filter.Enabled
        << sid->filter.Type
        << sid->filter.CurType
        << sid->filter.Dy
        << sid->filter.ResDy
        << sid->filter.Value;

        for (int v = 0; v < 3; v++) {

            worker

            << sid->optr[v].reg
            << sid->optr[v].SIDfreq
            << sid->optr[v].SIDpulseWidth
            << sid->optr[v].SIDctrl
            << sid->optr[v].SIDAD
            << sid->optr[v].SIDSR
            << sid->optr[v].sync
            << sid->optr[v].pulseIndex
            << sid->optr[v].newPulseIndex
            << sid->optr[v].curSIDfreq
            << sid->optr[v].curNoiseFreq
            << sid->optr[v].output
            << sid->optr[v].filtEnabled
            << sid->optr[v].filtLow
            << sid->optr[v].filtRef
            << sid->optr[v].filtIO
            << sid->optr[v].reg
            << sid->optr[v].SIDfreq
            << sid->optr[v].SIDpulseWidth
            << sid->optr[v].SIDctrl
            << sid->optr[v].SIDAD
            << sid->optr[v].SIDSR
            << sid->optr[v].sync
            << sid->optr[v].pulseIndex
            << sid->optr[v].newPulseIndex
            << sid->optr[v].curSIDfreq
            << sid->optr[v].curNoiseFreq
            << sid->optr[v].output
            << sid->optr[v].filtVoiceMask
            << sid->optr[v].filtEnabled
            << sid->optr[v].filtLow
            << sid->optr[v].filtRef
            << sid->optr[v].filtIO
            << sid->optr[v].cycleLenCount
            << sid->optr[v].cycleAddLenPnt
            << sid->optr[v].cycleLen
            << sid->optr[v].cycleLenPnt
            << sid->optr[v].waveStep
            << sid->optr[v].waveStepAdd
            << sid->optr[v].waveStepPnt
            << sid->optr[v].waveStepAddPnt
            << sid->optr[v].waveStepOld;

            for (isize n = 0; n < 2; n++) {

                worker

                << sid->optr[v].wavePre[n].len
                << sid->optr[v].wavePre[n].pnt
                << sid->optr[v].wavePre[n].stp;
            }

            worker

            << sid->optr[v].noiseReg
            << sid->optr[v].noiseStep
            << sid->optr[v].noiseStepAdd
            << sid->optr[v].noiseOutput
            << sid->optr[v].noiseIsLocked
            << sid->optr[v].ADSRctrl;

#ifdef SID_FPUENVE

            worker

            << sid->optr[v].fenveStep
            << sid->optr[v].fenveStepAdd
            << sid->optr[v].enveStep;

#else

            worker

            << sid->optr[v].enveStep
            << sid->optr[v].enveStepAdd
            << sid->optr[v].enveStepPnt
            << sid->optr[v].enveStepAddPnt;

#endif

            worker

            << sid->optr[v].enveVol
            << sid->optr[v].enveSusVol
            << sid->optr[v].enveShortAttackCount;
        }

        worker << sid->optr3_outputmask;
    }

    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        if (hard) {

            worker

            << executedCycles
            << computedSamples;
        }

        worker

        << latchedDataBus;
    }

    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    u64 _checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    isize didLoadFromBuffer(const u8 *buffer) override;
    isize willSaveToBuffer(u8 *buffer) override;


    //
    // Configuring
    //

public:

    u32 getClockFrequency() const;
    void setClockFrequency(u32 frequency);

    SIDRevision getRevision() const;
    void setRevision(SIDRevision m);

    double getSampleRate() const;
    void setSampleRate(double rate);

    bool getAudioFilter() const;
    void setAudioFilter(bool enable);


    //
    // Analyzing
    //

public:

    SIDInfo getInfo() const { return CoreComponent::getInfo(info); }
    VoiceInfo getVoiceInfo(isize nr) const { return CoreComponent::getInfo(voiceInfo[nr]); }

private:

    void _inspect() const override;
    void _dump(Category category, std::ostream& os) const override;


    //
    // Accessing
    //

public:

    // Reads or writes a SID register
    u8 peek(u16 addr);
    void poke(u16 addr, u8 value);


    //
    // Emulating
    //

    /* Runs SID for the specified amount of CPU cycles. The generated sound
     * samples are written into the provided ring buffer. The fuction returns
     * the number of written audio samples.
     */
    isize executeCycles(isize numCycles, SampleStream &stream);
    isize executeCycles(isize numCycles);
};

}
