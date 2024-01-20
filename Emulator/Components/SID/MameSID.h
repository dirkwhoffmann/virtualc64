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

    // Stores for how many cycles FastSID was executed so far
    i64 executedCycles = 0;

    // Stores how many sound samples were computed so far
    i64 computedSamples = 0;

    // Switches filter emulation on or off
    bool emulateFilter = true;

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

    const char *getDescription() const override { return "MameSID"; }


    //
    // Methods from CoreComponent
    //

private:

    void _reset(bool hard) override;

    template <class T>
    void applyToPersistentItems(T& worker)
    {
        /*
        worker

        << xyz;

         save_item(NAME(token->type));
         save_item(NAME(token->clock));

         save_item(NAME(token->PCMfreq));
         save_item(NAME(token->PCMsid));
         save_item(NAME(token->PCMsidNoise));

         save_item(NAME(token->reg));
         //save_item(NAME(token->sidKeysOn));
         //save_item(NAME(token->sidKeysOff));

         save_item(NAME(token->masterVolume));
         save_item(NAME(token->masterVolumeAmplIndex));

         save_item(NAME(token->filter.Enabled));
         save_item(NAME(token->filter.Type));
         save_item(NAME(token->filter.CurType));
         save_item(NAME(token->filter.Dy));
         save_item(NAME(token->filter.ResDy));
         save_item(NAME(token->filter.Value));

         for (int v = 0; v < m_token->max_voices; v++)
         {
         save_item(NAME(token->optr[v].reg), v);

         save_item(NAME(token->optr[v].SIDfreq), v);
         save_item(NAME(token->optr[v].SIDpulseWidth), v);
         save_item(NAME(token->optr[v].SIDctrl), v);
         save_item(NAME(token->optr[v].SIDAD), v);
         save_item(NAME(token->optr[v].SIDSR), v);

         save_item(NAME(token->optr[v].sync), v);

         save_item(NAME(token->optr[v].pulseIndex), v);
         save_item(NAME(token->optr[v].newPulseIndex), v);

         save_item(NAME(token->optr[v].curSIDfreq), v);
         save_item(NAME(token->optr[v].curNoiseFreq), v);

         save_item(NAME(token->optr[v].output), v);
         //save_item(NAME(token->optr[v].outputMask), v);

         save_item(NAME(token->optr[v].filtVoiceMask), v);

         save_item(NAME(token->optr[v].filtEnabled), v);
         save_item(NAME(token->optr[v].filtLow), v);
         save_item(NAME(token->optr[v].filtRef), v);
         save_item(NAME(token->optr[v].filtIO), v);

         save_item(NAME(token->optr[v].cycleLenCount), v);
         #if defined(DIRECT_FIXPOINT)
         save_item(NAME(token->optr[v].cycleLen.l), v);
         save_item(NAME(token->optr[v].cycleAddLen.l), v);
         #else
         save_item(NAME(token->optr[v].cycleAddLenPnt), v);
         save_item(NAME(token->optr[v].cycleLen), v);
         save_item(NAME(token->optr[v].cycleLenPnt), v);
         #endif

         #if defined(DIRECT_FIXPOINT)
         save_item(NAME(token->optr[v].waveStep.l), v);
         save_item(NAME(token->optr[v].waveStepAdd.l), v);
         #else
         save_item(NAME(token->optr[v].waveStep), v);
         save_item(NAME(token->optr[v].waveStepAdd), v);
         save_item(NAME(token->optr[v].waveStepPnt), v);
         save_item(NAME(token->optr[v].waveStepAddPnt), v);
         #endif
         save_item(NAME(token->optr[v].waveStepOld), v);
         for (int n = 0; n < 2; n++)
         {
         save_item(NAME(token->optr[v].wavePre[n].len), v | (n << 4));
         #if defined(DIRECT_FIXPOINT)
         save_item(NAME(token->optr[v].wavePre[n].stp), v | (n << 4));
         #else
         save_item(NAME(token->optr[v].wavePre[n].pnt), v | (n << 4));
         save_item(NAME(token->optr[v].wavePre[n].stp), v | (n << 4));
         #endif
         }

         #if defined(DIRECT_FIXPOINT)
         save_item(NAME(token->optr[v].noiseReg.l), v);
         #else
         save_item(NAME(token->optr[v].noiseReg), v);
         #endif
         save_item(NAME(token->optr[v].noiseStep), v);
         save_item(NAME(token->optr[v].noiseStepAdd), v);
         save_item(NAME(token->optr[v].noiseOutput), v);
         save_item(NAME(token->optr[v].noiseIsLocked), v);

         save_item(NAME(token->optr[v].ADSRctrl), v);
         //save_item(NAME(token->optr[v].gateOnCtrl), v);
         //save_item(NAME(token->optr[v].gateOffCtrl), v);

         #ifdef SID_FPUENVE
         save_item(NAME(token->optr[v].fenveStep), v);
         save_item(NAME(token->optr[v].fenveStepAdd), v);
         save_item(NAME(token->optr[v].enveStep), v);
         #elif defined(DIRECT_FIXPOINT)
         save_item(NAME(token->optr[v].enveStep.l), v);
         save_item(NAME(token->optr[v].enveStepAdd.l), v);
         #else
         save_item(NAME(token->optr[v].enveStep), v);
         save_item(NAME(token->optr[v].enveStepAdd), v);
         save_item(NAME(token->optr[v].enveStepPnt), v);
         save_item(NAME(token->optr[v].enveStepAddPnt), v);
         #endif
         save_item(NAME(token->optr[v].enveVol), v);
         save_item(NAME(token->optr[v].enveSusVol), v);
         save_item(NAME(token->optr[v].enveShortAttackCount), v);
         }

         save_item(NAME(token->optr3_outputmask));
        */
    }

    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {

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

    double getSampleRate() const { return sampleRate; }
    void setSampleRate(double rate);

    bool getAudioFilter() const { return emulateFilter; }
    void setAudioFilter(bool enable);

    // SamplingMethod getSamplingMethod() const;
    // void setSamplingMethod(SamplingMethod value);


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
