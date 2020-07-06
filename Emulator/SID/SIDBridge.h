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

    //
    // Sub components
    //
        
private:

    // FastSID (Adapted from VICE 3.1)
    FastSID fastsid = FastSID(vc64);

    // ReSID (Taken from VICE 3.1)
    ReSID resid = ReSID(vc64);
   
    // SID selector
    bool useReSID;
    
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
    
    /* Target volume.
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
    
    //! @brief    Returns true, whether ReSID or the old implementation should be used.
    bool getReSID() { return useReSID; }
    
    //! @brief    Enables or disables the ReSID library.
    void setReSID(bool enable);
    
    //! @brief    Returns the simulated chip model.
    SIDModel getModel();
    
    //! @brief    Sets chip model
    void setModel(SIDModel m);
    
    //! @brief    Returns true iff audio filters are enabled.
    bool getAudioFilter();
    
    //! @brief    Enables or disables filters of SID.
    void setAudioFilter(bool enable);
    
    //! @brief    Returns the sampling method.
    SamplingMethod getSamplingMethod();
    
    //! @brief    Sets the sampling method (ReSID only).
    void setSamplingMethod(SamplingMethod value);
    
    //! @brief    Returns the sample rate.
    u32 getSampleRate();
    
    //! @brief    Sets the samplerate of SID and it's 3 voices.
    void setSampleRate(u32 sr);
    
    //! @brief    Returns the clock frequency.
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

    
public:
    

    //! @brief    Gathers all values that are displayed in the debugger
    SIDInfo getInfo();

    //! @brief    Gathers all debug information for a specific voice
    VoiceInfo getVoiceInfo(unsigned voice);
    
  
    //
    // Volume control
    //
    
    /*! @brief Sets the current volume
     */
    void setVolume(i32 vol) { volume = vol; }
    
    /*! @brief   Triggers volume ramp up phase
     *  @details Configures volume and targetVolume to simulate a smooth audio fade in
     */
    void rampUp() { targetVolume = maxVolume; volumeDelta = 3; ignoreNextUnderOrOverflow(); }
    void rampUpFromZero() { volume = 0; rampUp(); }
    
    /*! @brief   Triggers volume ramp down phase
     *  @details Configures volume and targetVolume to simulate a quick audio fade out
     */
    void rampDown() { targetVolume = 0; volumeDelta = 50; ignoreNextUnderOrOverflow(); }
    
    //
    // Ringbuffer handling
    //
    
    //! @brief  Returns the size of the ringbuffer
    size_t ringbufferSize() { return bufferSize; }
    
    //! @brief  Returns the position of the read pointer
    u32 getReadPtr() { return readPtr; }

    //! @brief  Returns the position of the write pointer
    u32 getWritePtr() { return writePtr; }

    //! @brief  Clears the ringbuffer and resets the read and write pointer
    void clearRingbuffer();
    
    //! @brief  Reads a single audio sample from the ringbuffer
    float readData();
    
    //! @brief  Reads a single audio sample without moving the read pointer
    float ringbufferData(size_t offset);
    
    /*! @brief   Reads a certain amount of samples from ringbuffer
     *  @details Samples are stored in a single mono stream
     */
    void readMonoSamples(float *target, size_t n);
    
    /*! @brief   Reads a certain amount of samples from ringbuffer
     *  @details Samples are stored in two seperate mono streams
     */
    void readStereoSamples(float *target1, float *target2, size_t n);
    
    /*! @brief   Reads a certain amount of samples from ringbuffer
     *  @details Samples are stored in an interleaved stereo stream
     */
    void readStereoSamplesInterleaved(float *target, size_t n);
    
    /*! @brief  Writes a certain number of audio samples into ringbuffer
     */
    void writeData(short *data, size_t count);
    
    /*! @brief   Handles a buffer underflow condition.
     *  @details A buffer underflow occurs when the computer's audio device
     *           needs sound samples than SID hasn't produced, yet.
     */
    void handleBufferUnderflow();
    
    /*! @brief   Handles a buffer overflow condition
     *  @details A buffer overflow occurs when SID is producing more samples
     *           than the computer's audio device is able to consume.
     */
    void handleBufferOverflow();
    
    //! @brief   Signals to ignore the next underflow or overflow condition.
    void ignoreNextUnderOrOverflow() { lastAlignment = mach_absolute_time(); }
        
    //! @brief   Moves read pointer one position forward
    void advanceReadPtr() { readPtr = (readPtr + 1) % bufferSize; }
    
    //! @brief   Moves read pointer forward or backward
    void advanceReadPtr(int steps) { readPtr = (readPtr + bufferSize + steps) % bufferSize; }
    
    //! @brief   Moves write pointer one position forward
    void advanceWritePtr() { writePtr = (writePtr + 1) % bufferSize; }
    
    //! @brief   Moves write pointer forward or backward
    void advanceWritePtr(int steps) { writePtr = (writePtr + bufferSize + steps) % bufferSize; }
    
    //! @brief   Returns number of stored samples in ringbuffer
    unsigned samplesInBuffer() { return (writePtr + bufferSize - readPtr) % bufferSize; }
    
    //! @brief   Returns remaining storage capacity of ringbuffer
    unsigned bufferCapacity() { return (readPtr + bufferSize - writePtr) % bufferSize; }
    
    //! @brief   Returns the fill level as a percentage value
    double fillLevel() { return (double)samplesInBuffer() / (double)bufferSize; }
    
    /*! @brief    Aligns the write pointer.
     *  @details  This function puts the write pointer somewhat ahead of the
     *            read pointer. With a standard sample rate of 44100 Hz, 735
     *            samples is 1/60 sec.
     */
    const u32 samplesAhead = 8 * 735;
    void alignWritePtr() { writePtr = (readPtr  + samplesAhead) % bufferSize; }
    
public:
    
    /*! @brief    Executes SID until a certain cycle is reached
     *  @param    cycle The target cycle
     */
    void executeUntil(u64 targetCycle);

    /*! @brief    Executes SID for a certain number of cycles
     *  @param    cycles Number of cycles to execute
     */
	void execute(u64 numCycles);

     
	//
	// Accessig device properties
	//
    
public:
    
	//! @brief    Special peek function for the I/O memory range.
	u8 peek(u16 addr);
	
    //! @brief    Same as peek, but without side effects.
    u8 spypeek(u16 addr);
    
	//! @brief    Special poke function for the I/O memory range.
	void poke(u16 addr, u8 value);
};

#endif
