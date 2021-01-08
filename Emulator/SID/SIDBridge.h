// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"
#include "SIDPublicTypes.h"
#include "Volume.h"
#include "SIDStreams.h"
#include "FastSID.h"
#include "ReSID.h"

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

class SIDBridge : public C64Component {

    friend C64Memory;

    // Current configuration
    SIDConfig config;
    
    
    //
    // Sub components
    //
        
private:
    
    FastSID fastsid[4] = {
        
        FastSID(c64, *this, 0),
        FastSID(c64, *this, 1),
        FastSID(c64, *this, 2),
        FastSID(c64, *this, 3)
    };
    
    ReSID resid[4] = {
        
        ReSID(c64, *this, 0),
        ReSID(c64, *this, 1),
        ReSID(c64, *this, 2),
        ReSID(c64, *this, 3)
    };
        
    // CPU cycle at the last call to executeUntil()
    u64 cycles = 0;
    
    // Current CPU frequency
    u32 cpuFrequency = PAL_CLOCK_FREQUENCY;
    
    // Sample rate (44.1 kHz per default)
    double sampleRate = 44100.0;
        
    // Time stamp of the last write pointer alignment
    u64 lastAlignment = 0;

    // Master volumes (fadable)
    Volume volL;
    Volume volR;

    // Channel volumes
    float vol[4] = { 0, 0, 0, 0 };

    // Panning factors
    float pan[4] = { 0, 0, 0, 0 };
        
public:
    
    // Number of buffer underflows since power up
    // TODO: MOVE TO SIDStats
    u64 bufferUnderflows;

    // Number of buffer overflows since power up
    // TODO: MOVE TO SIDStats
    u64 bufferOverflows;
    
    // Set to true to signal a buffer exception
    bool signalUnderflow = false;

    
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
	
	SIDBridge(C64 &ref);
    const char *getDescription() const override { return "SIDBridge"; }

private:
    
    void _reset() override;

    
    //
    // Configuring
    //
    
public:
    
    SIDConfig getConfig() { return config; }
    
    long getConfigItem(Option option);
    long getConfigItem(Option option, long id);

    bool setConfigItem(Option option, long value) override;
    bool setConfigItem(Option option, long id, long value) override;

    bool isEnabled(int nr) { return GET_BIT(config.enabled, nr); }
    
    bool isMuted();

    u32 getClockFrequency();
    void setClockFrequency(u32 frequency);

    // DEPRECATED: Use OPT_xxx
    SIDRevision getRevision() const;
    void setRevision(SIDRevision revision);
    
    double getSampleRate() const;
    void setSampleRate(double rate);
    
    // DEPRECATED: Use OPT_xxx
    bool getAudioFilter() const;
    void setAudioFilter(bool enable);

    // DEPRECATED: Use OPT_xxx
    SamplingMethod getSamplingMethod() const;
    void setSamplingMethod(SamplingMethod method);

private:
    
    void _dumpConfig() const override;
    
    
    //
    // Analyzing
    //

public:
    
    SIDInfo getInfo(unsigned nr);
    VoiceInfo getVoiceInfo(unsigned nr, unsigned voice);
    
private:
    
    void _dump() const override;
    void _dump(int nr) const;
    void _dump(SIDInfo &info, VoiceInfo (&vinfo)[3]) const;

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        & config.revision
        & config.enabled
        & config.address
        & config.filter
        & config.engine
        & config.sampling
        & config.volL
        & config.volR
        & config.vol
        & config.pan
        & cycles
        & cpuFrequency
        & lastAlignment
        & volL
        & volR
        & vol
        & pan;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        & cycles;
    }
    
    usize _size() override { COMPUTE_SNAPSHOT_SIZE }
    usize _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    usize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    usize didLoadFromBuffer(u8 *buffer) override;
    
 
private:
    
    void _run() override;
    void _pause() override;
    void _setWarp(bool enable) override;
    
  
    //
    // Controlling the volume
    //
    
public:
        
    /* Starts to ramp up the volume. This function configures variables volume
     * and targetVolume to simulate a smooth audio fade in.
     */
    void rampUp();
    void rampUpFromZero();
    
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
    
    // Clears the ringbuffer and resets the read and write pointer
    void clearRingbuffer();
    
    // Reads a single audio sample from the ringbuffer
    // float readData();
    
    // Reads a audio sample pair without moving the read pointer
    void ringbufferData(usize offset, float *left, float *right);
            
    /* Handles a buffer underflow condition.
     * A buffer underflow occurs when the computer's audio device needs sound
     * samples than SID hasn't produced, yet.
     */
    void handleBufferUnderflow();
    
    /* Handles a buffer overflow condition.
     * A buffer overflow occurs when SID is producing more samples than the
     * computer's audio device is able to consume.
     */
    void handleBufferOverflow();
    
    // Signals to ignore the next underflow or overflow condition.
    void ignoreNextUnderOrOverflow();
    
    /* Aligns the write pointer.
     * This function puts the write pointer somewhat ahead of the read pointer.
     * With a standard sample rate of 44100 Hz, 735 samples is 1/60 sec.
     */
    const u32 samplesAhead = 8 * 735;
    void alignWritePtr() { stream.clear(SamplePair {0,0} ); stream.align(samplesAhead); }
    
    /* Executes SID until a certain cycle is reached.
     * // The function returns the number of produced sound samples (not yet).
     */
    void executeUntil(u64 targetCycle);

    // Executes SID for a certain number of CPU cycles
	i64 executeCycles(u64 numCycles);

private:
    
    // Called by executeCycles to produce the final stereo stream
    void mixSingleSID(u64 numSamples);
    void mixMultiSID(u64 numSamples);

    
    //
    // Copying data from the ring buffer
    //
    
public:
    
    void copyMono(float *buffer, usize n);
    void copyStereo(float *left, float *right, usize n);
    void copyInterleaved(float *buffer, usize n);

    
     
	//
	// Accessig memory
	//
    
public:
    
    // Translates a memory address to the mapped in SID
    int mappedSID(u16 addr); 
    
	// Special peek function for the I/O memory range
	u8 peek(u16 addr);
	
    // Same as peek without side effects
    u8 spypeek(u16 addr);
    
	// Special poke function for the I/O memory range
	void poke(u16 addr, u8 value);
};
