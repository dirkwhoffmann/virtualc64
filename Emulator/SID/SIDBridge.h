// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _SIDBRIDGE_H
#define _SIDBRIDGE_H

#include "C64Component.h"
#include "FastSID.h"
#include "ReSID.h"
#include "SIDTypes.h"

class SIDBridge : public C64Component {

    friend C64Memory;

    // Current configuration
    SIDConfig config;
    
    //
    // Sub components
    //
        
private:

    // FastSID (Adapted from VICE 3.1)
    FastSID fastsid = FastSID(c64);

    // ReSID (Taken from VICE 3.1)
    ReSID resid = ReSID(c64);
       
    // CPU cycle at the last call to executeUntil()
    u64 cycles;
    
    // Time stamp of the last write pointer alignment
    u64 lastAlignment = 0;
    
public:
    
    // Number of buffer underflows since power up
    u64 bufferUnderflows;

    // Number of buffer overflows since power up
    u64 bufferOverflows;
    
    //
    // Audio ringbuffer
    //

private:

    // Number of sound samples stored in ringbuffer
    static constexpr size_t bufferSize = 12288;
    
    /* The audio sample ringbuffer.
     * This ringbuffer serves as the data interface between the emulation code
     * and the audio API (CoreAudio on Mac OS X).
     */
    float ringBuffer[bufferSize];
    
    /* Scaling value for sound samples.
     * All sound samples produced by reSID are scaled by this value before they
     * are written into the ringBuffer.
     */
    static constexpr float scale = 0.000005f;
    
    // Ring buffer pointers
    u32 readPtr;
    u32 writePtr;
    
    // Current volume (0 = silent)
    i32 volume;
    
    /* Target volume
     * Whenever an audio sample is written, the volume is increased or decreased
     * by volumeDelta steps to make it reach the target volume eventually. This
     * feature simulates a fading effect.
     */
    i32 targetVolume;
    
    // Maximum volume
    const static i32 maxVolume = 100000;
    
    /* Volume offset
     * If the current volume does not match the target volume, it is increased
     * or decreased by the specified amount. The increase or decrease takes
     * place whenever an audio sample is generated.
     */
    i32 volumeDelta;
    
    //
    // Constructing and serializing
    //
    
public:
	
	SIDBridge(C64 &ref);
	
    //
    // Configuring
    //
    
    SIDConfig getConfig() { return config; }
    
    SIDRevision getRevision() { return config.revision; }
    void setRevision(SIDRevision rev);

    bool getAudioFilter() { return config.filter; }
    void setFilter(bool enable);
    
    SIDEngine getEngine() { return config.engine; }
    void setEngine(SIDEngine engine);
        
    SamplingMethod getSamplingMethod() { return config.sampling; }
    void setSamplingMethod(SamplingMethod method);
    
    double getSampleRate();
    void setSampleRate(double rate);
    
    u32 getClockFrequency();
    
    
    //
    // Methods from HardwareComponent
    //
    
public:
    
    void _reset() override;
    void didLoadFromBuffer(u8 **buffer) override { clearRingbuffer(); }

private:
    
    void _run() override;
    void _pause() override;
    void _dump() override;
    void _dump(SIDInfo info);
    void _setWarp(bool enable) override;
    
    
    //
    // Analyzing
    //
    
public:
    
    // Returns the result of the most recent call to inspect()
    SIDInfo getInfo();
    VoiceInfo getVoiceInfo(unsigned voice);
    
  
    //
    // Volume control
    //
    
public:
    
    // Sets the current volume
    void setVolume(i32 vol) { volume = vol; }
    
    /* Ramps the volume up
     * Configures volume and targetVolume to simulate a smooth audio fade in
     */
    void rampUp() { targetVolume = maxVolume; volumeDelta = 3; ignoreNextUnderOrOverflow(); }
    void rampUpFromZero() { volume = 0; rampUp(); }
    
    /* Ramps the volume down
     * Configures volume and targetVolume to simulate a quick audio fade out
     */
    void rampDown() { targetVolume = 0; volumeDelta = 50; ignoreNextUnderOrOverflow(); }
    
    
    //
    // Managing the ringbuffer
    //
    
public:
    
    // Returns the size of the ringbuffer (constant value)
    size_t ringbufferSize() { return bufferSize; }
    
    // Returns the position of the read or write pointer
    u32 getReadPtr() { return readPtr; }
    u32 getWritePtr() { return writePtr; }

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
    
    /* Writes a certain number of audio samples into ringbuffer
     */
    void writeData(short *data, size_t count);
    
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
    void ignoreNextUnderOrOverflow() { lastAlignment = mach_absolute_time(); }
        
    // Moves read or write pointer forwards or backwards
    void advanceReadPtr() { readPtr = (readPtr + 1) % bufferSize; }
    void advanceReadPtr(int steps) { readPtr = (readPtr + bufferSize + steps) % bufferSize; }
    void advanceWritePtr() { writePtr = (writePtr + 1) % bufferSize; }
    void advanceWritePtr(int steps) { writePtr = (writePtr + bufferSize + steps) % bufferSize; }
    
    // Returns number of stored samples in ringbuffer
    unsigned samplesInBuffer() { return (writePtr + bufferSize - readPtr) % bufferSize; }
    
    // Returns remaining storage capacity of ringbuffer
    unsigned bufferCapacity() { return (readPtr + bufferSize - writePtr) % bufferSize; }
    
    // Returns the fill level as a percentage value
    double fillLevel() { return (double)samplesInBuffer() / (double)bufferSize; }
    
    /* Aligns the write pointer.
     * This function puts the write pointer somewhat ahead of the read pointer.
     * With a standard sample rate of 44100 Hz, 735 samples is 1/60 sec.
     */
    const u32 samplesAhead = 8 * 735;
    void alignWritePtr() { writePtr = (readPtr  + samplesAhead) % bufferSize; }
    
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
