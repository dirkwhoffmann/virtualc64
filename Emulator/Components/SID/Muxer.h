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

#pragma once

#include "SubComponent.h"
#include "SIDTypes.h"
#include "Constants.h"
#include "Volume.h"
#include "SIDStreams.h"
#include "SID.h"
#include "Chrono.h"

namespace vc64 {

/* Architecture of the audio pipeline
 *
 *           Mux class
 *           -------------------------------------------------
 *          |   --------  vol                                 |
 *   SID 0 --->| Buffer |----->                               |
 *          |   --------       |                              |
 *          |                  |                              |
 *          |   --------  vol  |             --------------   |
 *   SID 1 --->| Buffer |----->|          ->| StereoStream |-----> Speaker
 *          |   --------       |   pan   |   --------------   |
 *          |                  |-------->|                    |
 *          |   --------  vol  |  l vol  |   --------------
 *   SID 2 --->| Buffer |----->|  r vol   ->| StereoStream |-----> Recorder
 *          |   --------       |             --------------   |
 *          |                  |                              |
 *          |   --------  vol  |                              |
 *   SID 3 --->| Buffer |----->                               |
 *          |   --------                                      |
 *           -------------------------------------------------
 */

class Muxer final : public SubComponent, public Inspectable<SIDInfo, Void> {

    friend C64Memory;

    // Current configuration
    MuxerConfig config = { };
    
    // Statistics
    MuxerStats stats = { };

    
    //
    // Sub components
    //

public:
    
    SID sid[4] =  {

        SID(c64, 0),
        SID(c64, 1),
        SID(c64, 2),
        SID(c64, 3)
    };

private:
    
    // CPU cycle at the last call to executeUntil()
    Cycle cycles = 0;
    
    // Current CPU frequency
    u32 cpuFrequency = PAL_CLOCK_FREQUENCY;
    
    // Sample rate (44.1 kHz per default)
    double sampleRate = 44100.0;

    // Time stamp of the last write pointer alignment
    util::Time lastAlignment;

    // Master volumes (fadable)
    Volume volL;
    Volume volR;

public:


    //
    // Audio streams
    //
    
    /* The four SID streams. Each stream stores the sound samples produced by
     * one of the four supported SIDs.
     */
    SampleStream sidStream[4];

    /* The mixed stereo stream. This stream contains the final audio stream
     * ready to be handed over to the audio device of the host OS.
     */
    StereoStream stream;
    
    
    //
    // Initializing
    //
    
public:

    Muxer(C64 &ref);

    // Resets the output buffer
    void clear();

    
    //
    // Methods from CoreObject
    //

private:
    
    const char *getDescription() const override { return "Muxer"; }
    void _dump(Category category, std::ostream& os) const override;
    void _dump(Category category, std::ostream& os, isize nr) const;

    
    //
    // Methods from CoreComponent
    //

public:
    
    void _run() override;
    void _pause() override;
    void _warpOn() override;
    void _warpOff() override;
    
    Muxer& operator= (const Muxer& other) {

        CLONE_ARRAY(sid)

        CLONE(cycles)

        CLONE(cycles)
        CLONE(cpuFrequency)
        CLONE(volL)
        CLONE(volR)

        CLONE(config)

        return *this;
    }

    template <class T>
    void serialize(T& worker)
    {
        worker
        
        << sid
        << cycles;

        if (isResetter(worker)) return;

        worker

        << cycles
        << cpuFrequency
        << volL
        << volR

        << config.revision
        << config.enabled
        << config.filter
        << config.engine
        << config.sampling
        << config.volL
        << config.volR;
    }
    
    void operator << (SerResetter &worker) override { serialize(worker); }
    void operator << (SerChecker &worker) override { serialize(worker); }
    void operator << (SerCounter &worker) override { serialize(worker); }
    void operator << (SerReader &worker) override;
    void operator << (SerWriter &worker) override { serialize(worker); }

    void _reset(bool hard) override;

    
    //
    // Configuring
    //
    
public:
    
    // static MuxerConfig getDefaultConfig();
    const MuxerConfig &getConfig() const { return config; }
    void resetConfig() override;

    i64 getConfigItem(Option option) const;
    i64 getConfigItem(Option option, long id) const;

    void setConfigItem(Option option, i64 value);
    void setConfigItem(Option option, long id, i64 value);

    bool isEnabled(isize nr) const { return GET_BIT(config.enabled, nr); }
    
    bool isMuted() const;

    u32 getClockFrequency();
    void setClockFrequency(u32 frequency);
    
    double getSampleRate() const;
    void setSampleRate(double rate);
    

    //
    // Analyzing
    //

public:
    
    SIDInfo getInfo(isize nr);
    VoiceInfo getVoiceInfo(isize nr, isize voice);
    CoreComponent &getSID(isize nr);
    MuxerStats getStats();
    
private:
    
    void clearStats();


    //
    // Controlling the volume
    //
    
public:

    /* Starts to ramp up the volume. This function configures variables volume
     * and targetVolume to simulate a smooth audio fade in.
     */
    void rampUp();
    void rampUp(float from);

    /* Starts to ramp down the volume. This function configures variables
     * volume and targetVolume to simulate a quick audio fade out.
     */
    void rampDown();

    
    //
    // Managing the four sample buffers
    //
    
public:

    // Clears the SID sample buffers
    void clearSampleBuffers();
    void clearSampleBuffer(long nr);

    
    //
    // Managing the ring buffer
    //
    
public:

    // Reads a audio sample pair without moving the read pointer
    void ringbufferData(isize offset, float *left, float *right);

    /* Handles a buffer underflow condition. A buffer underflow occurs when the
     * audio device of the host machine needs sound samples than SID hasn't
     * produced, yet.
     */
    void handleBufferUnderflow();
    
    /* Handles a buffer overflow condition. A buffer overflow occurs when SID
     * is producing more samples than the audio device of the host machine is
     * able to consume.
     */
    void handleBufferOverflow();
    
    // Signals to ignore the next underflow or overflow condition.
    void ignoreNextUnderOrOverflow();

    /* Executes SID until a certain cycle is reached. The function returns the
     * number of produced sound samples (not yet).
     */
    void executeUntil(Cycle targetCycle);

    // Executes SID for a certain number of CPU cycles
    isize executeCycles(isize numCycles);

private:
    
    // Called by executeCycles to produce the final stereo stream
    void mixSingleSID(isize numSamples);
    void mixMultiSID(isize numSamples);

    
    //
    // Copying data from the ring buffer
    //
    
public:
    
    void copyMono(float *buffer, isize n);
    void copyStereo(float *left, float *right, isize n);
    void copyInterleaved(float *buffer, isize n);

    
    //
    // Accessig memory
    //
    
public:
    
    // Translates a memory address to the mapped SID
    isize mappedSID(u16 addr) const;
    
    // Special peek function for the I/O memory range
    u8 peek(u16 addr);

    // Same as peek without side effects
    u8 spypeek(u16 addr) const;
    
    // Reads the pot bits that show up in register 0x19 and 0x1A
    u8 readPotX() const;
    u8 readPotY() const;
    
    // Special poke function for the I/O memory range
    void poke(u16 addr, u8 value);


    //
    // Visualizing the waveform
    //

public:

    /* Plots a graphical representation of the waveform. Returns the highest
     * amplitute that was found in the ringbuffer. To implement auto-scaling,
     * pass the returned value as parameter maxAmp in the next call to this
     * function.
     */
    float draw(u32 *buffer, isize width, isize height,
               float maxAmp, u32 color, isize sid = -1) const;
};

}
