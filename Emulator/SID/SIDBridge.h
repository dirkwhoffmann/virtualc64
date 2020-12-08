// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _SID_BRIDGE_H
#define _SID_BRIDGE_H

#include "C64Component.h"
#include "SIDTypes.h"
#include "SIDStream.h"
#include "FastSID.h"
#include "ReSID.h"

class SIDBridge : public C64Component {

    friend C64Memory;

    // Current configuration
    SIDConfig config;
    
    //
    // Sub components
    //
        
private:

    // FastSID (Adapted from VICE 3.1)
    FastSID fastsid[4] = {
        FastSID(c64, ringBuffer[0], samples[0]),
        FastSID(c64, ringBuffer[1], samples[1]),
        FastSID(c64, ringBuffer[2], samples[2]),
        FastSID(c64, ringBuffer[3], samples[3])
    };
    
    // ReSID (Taken from VICE 3.1)
    ReSID resid[4] = {
        ReSID(c64, ringBuffer[0], samples[0]),
        ReSID(c64, ringBuffer[1], samples[1]),
        ReSID(c64, ringBuffer[2], samples[2]),
        ReSID(c64, ringBuffer[3], samples[3])
    };

    // CPU cycle at the last call to executeUntil()
    u64 cycles;
    
    // Time stamp of the last write pointer alignment
    u64 lastAlignment = 0;
    
public:
    
    // Number of buffer underflows since power up
    u64 bufferUnderflows;

    // Number of buffer overflows since power up
    u64 bufferOverflows;
    
    // Set to true to signal a buffer exception
    bool signalUnderflow = false;
    bool signalOverflow = false;

    
    //
    // Audio buffers
    //

    /* Sample buffers. There is a seperate buffer for each of the four SID
     * channels. Every reSID or fastSID instance uses one of these buffers for
     * storing the created sound samples.
     */
    static const size_t sampleBufferSize = 2048;
    short samples[4][sampleBufferSize];
    
public:
    
    /* DEPRECATED
     */
    SIDStream ringBuffer[4] = {
        SIDStream(*this),
        SIDStream(*this),
        SIDStream(*this),
        SIDStream(*this)
    };

    /* The mixed stereo stream. This stream contains the final audio stream
     * ready to be handed over to the audio device of the host OS.
     */
    StereoStream stream = StereoStream(*this);
    
private:
            
    // Current volume (0 = silent)
    i32 volume;
    
    /* Target volume. Whenever an audio sample is written, the volume is
     * increased or decreased by volumeDelta steps to make it reach the target
     * volume eventually. This feature simulates a fading effect.
     */
    i32 targetVolume;
    
    // Maximum volume
    const static i32 maxVolume = 100000;
    
    /* Volume offset. If the current volume does not match the target volume,
     * it is increased or decreased by the specified amount. The increase or
     * decrease takes place whenever an audio sample is generated.
     */
    i32 volumeDelta;
    
    
    //
    // Initializing
    //
    
public:
	
	SIDBridge(C64 &ref);
	
private:
    
    void _reset() override;

    
    //
    // Configuring
    //
    
public:
    
    SIDConfig getConfig() { return config; }
    
    long getConfigItem(ConfigOption option);
    bool setConfigItem(ConfigOption option, long value) override;
    
    u32 getClockFrequency();
    void setClockFrequency(u32 frequency);
    
    SIDRevision getRevision();
    void setRevision(SIDRevision revision);
    
    double getSampleRate();
    void setSampleRate(double rate);
    
    bool getAudioFilter();
    void setAudioFilter(bool enable);
    
    SamplingMethod getSamplingMethod();
    void setSamplingMethod(SamplingMethod method);

    
    //
    // Analyzing
    //

public:
    
    SIDInfo getInfo();
    VoiceInfo getVoiceInfo(unsigned voice);
    
private:
    
    void _dump() override;
    void _dump(int nr);
    void _dump(SIDInfo info);

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        & config.engine
        & config.filter;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        & cycles;
    }
    
    size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    size_t didLoadFromBuffer(u8 *buffer) override;
    
 

private:
    
    void _run() override;
    void _pause() override;
    void _setWarp(bool enable) override;
    
  
    //
    // Volume control
    //
    
public:
    
    // Sets the current volume
    void setVolume(i32 vol) { volume = vol; }
    
    /* Ramps the volume up. Configures volume and targetVolume to simulate a
     * smooth audio fade in
     */
    void rampUp() { targetVolume = maxVolume; volumeDelta = 3; ignoreNextUnderOrOverflow(); }
    void rampUpFromZero() { volume = 0; rampUp(); }
    
    /* Ramps the volume down. Configures volume and targetVolume to simulate a
     * quick audio fade out
     */
    void rampDown() { targetVolume = 0; volumeDelta = 50; ignoreNextUnderOrOverflow(); }
    
    
    //
    // Managing the ringbuffer
    //
    
public:
    
    // Clears the ringbuffer and resets the read and write pointer
    void clearRingbuffer();
    
    // Reads a single audio sample from the ringbuffer
    float readData();
    
    // Reads a single audio sample without moving the read pointer
    float ringbufferData(size_t offset);
    
    /* Reads a number of sound samples from ringbuffer.
     * Samples are stored in a single mono stream.
     */
    void readMonoSamples(float *target, size_t n);
    
    /* Reads a number of sound samples from ringbuffer.
     * Samples are stored in two seperate mono streams
     */
    void readStereoSamples(float *target1, float *target2, size_t n);
    
    /* Reads a certain amount of samples from ringbuffer.
     * Samples are stored in an interleaved stereo stream.
     */
    void readStereoSamplesInterleaved(float *target, size_t n);
        
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
    void alignWritePtr() {
        for (int i = 0; i < 4; i++) ringBuffer[i].align(samplesAhead);
        stream.align(samplesAhead);
    }
    
    // Executes SID until a certain cycle is reached
    void executeUntil(u64 targetCycle);

    // Executes SID for a certain number of cycles
	void execute(u64 numCycles);

     
	//
	// Accessig device properties
	//
    
public:
    
	// Special peek function for the I/O memory range
	u8 peek(u16 addr);
	
    // Same as peek without side effects
    u8 spypeek(u16 addr);
    
	// Special poke function for the I/O memory range
	void poke(u16 addr, u8 value);
};

#endif
