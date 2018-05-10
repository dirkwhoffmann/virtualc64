/*!
 * @header      SIDbase.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
 * @brief       Base class for ReSID and FastSID
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _SIDBASE_INC
#define _SIDBASE_INC

#include "VirtualComponent.h"
// #include "resid/sid.h"

class SIDbase : public VirtualComponent {
    
public:
    
    /*! @brief   Currently used chip model.
     *  @details MOS6581 is the older SID chip exhibiting the "volume bug".
     *           This chip must be selected to hear synthesized speech.
     *           MOS8580 is the newer SID chip model with the "volume bug" fixed.
     */
    SIDChipModel chipModel;
    
    /*! @brief   Switches filter emulation on or off.
     *  @details Switching off filter emulation slightly improves emulation speed.
     */
    bool emulateFilter;
    
    /*! @brief   Sample rate
     *  @details By default, a sample rate of 44.1 kHz is used.
     */
    uint32_t sampleRate;
    
    /*! @brief   Sampling method used by the reSID library
     */
    SamplingMethod samplingMethod;
    
    /*! @brief   Current CPU frequency
     *  @details This variable must always mirror the frequency of the C64 CPU to get the
     *           proper audio samples at the right time. The CPU frequency differs in PAL and NTSC mode.
     */
    uint32_t cpuFrequency;
    
    /*! @brief   Configuration option offered by the reSID library
     */
    bool audioFilter;
    
    /*! @brief   Configuration option offered by the reSID library
     */
    bool externalAudioFilter;
    
    /*! @brief   Size of the audio samples ringbuffer.
     *  @see     ringBuffer
     */
    static constexpr size_t bufferSize = 12288;
    
    /*! @brief   The audio sample ringbuffer.
     *  @details This ringbuffer serves as the data interface between the SID emulation code and
     *           computers audio API (CoreAudio on Mac OS X).
     */
    float ringBuffer[bufferSize];
    
    /*! @brief   Scaling value for sound samples
     *  @details All sound samples produced by reSID are scaled by this value
     *           before they are written into the ringBuffer
     */
    static constexpr float scale = 0.000005f;
  
    /*! @brief   Ring buffer read pointer
     */
    unsigned readPtr;
    
    /*! @brief   Ring buffer write pointer
     */
    unsigned writePtr;
    
    /*! @brief   Current volume
     *  @note    A value of 0 or below silences the audio playback.
     */
    int32_t volume;
    
    /*! @brief   Target volume
     *  @details Whenever an audio sample is written, the volume is increased or decreased by volumeDelta
     *           to make it reach the target volume eventually. This feature simulates a fading effect.
     *  @note    0 = silent
     */
    int32_t targetVolume;
    
    /*! @brief   Maximum volume
     */
    const static int32_t maxVolume = 100000;
    
    /*! @brief   Volume offset
     *  @details If the current volume does not match the target volume, it is increased or decreased by the
     *           specified amount. The increase or decrease takes place whenever an audio sample is generated.
     */
    int32_t volumeDelta;
    
public:
    
    //
    // Running the device
    //
    
    /*! @brief   Execute SID
     *  @details Runs reSID for the specified amount of CPU cycles and writes
     *           the generated sound samples into the internal ring buffer.
     */
    virtual void execute(uint64_t cycles) = 0;
    
    //! Notifies the SID chip that the emulator has started
    void run();
    
    //! Notifies the SID chip that the emulator has started
    void halt();
    
    //
    // Configuring the device
    //
    
    //! Returns true iff audio filters should be emulated.
    virtual bool getAudioFilter() { return emulateFilter; }
    
    //! Enable or disable audio filter emulation
    virtual void setAudioFilter(bool value) { emulateFilter = value; }
    
    //! Return samplerate.
    virtual uint32_t getSampleRate() = 0;
    
    //! Set sample rate
    virtual void setSampleRate(uint32_t sr) = 0;
    
    //! Get sampling method
    virtual SamplingMethod getSamplingMethod() = 0;
    
    //! Set sampling method
    virtual void setSamplingMethod(SamplingMethod value) = 0;
    
    //! Get chip model
    virtual SIDChipModel getChipModel() = 0;
    
    //! Set chip model
    virtual void setChipModel(SIDChipModel value) = 0;
    
    //! Get clock frequency
    virtual uint32_t getClockFrequency() = 0;
    
    //! Set clock frequency
    virtual void setClockFrequency(uint32_t f) = 0;
    
    //
    // Volume control
    //
    
    /*! @brief Sets the current volume
     */
    void setVolume(int32_t vol) { volume = vol; }
    
    /*! @brief   Triggers volume ramp up phase
     *  @details Configures volume and targetVolume to simulate a smooth audio fade in
     */
    void rampUp() { targetVolume = maxVolume; volumeDelta = 3; }
    void rampUpFromZero() { volume = 0; targetVolume = maxVolume; volumeDelta = 3; }
    
    /*! @brief   Triggers volume ramp down phase
     *  @details Configures volume and targetVolume to simulate a quick audio fade out
     */
    void rampDown() { targetVolume = 0; volumeDelta = 50; }
    
    //
    // Ringbuffer handling
    //
    
    /*! @brief  Clears the ringbuffer and resets the read and write pointer to its inital values
     */
    void clearRingbuffer();
    
    //! @brief  Reads a single audio sample from ringbuffer
    float readData();
    
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
    
    /*! @brief   Handles a buffer underflow condition
     *  @details A buffer underflow occurs when the computer's audio device
     *           needs sound samples than SID hasn't produced, yet!
     */
    void handleBufferUnderflow();
    
    /*! @brief   Handles a buffer overflow condition
     *  @details A buffer overflow occurs when SID is producing more samples
     *           than the computer's audio device is able to consume
     */
    void handleBufferOverflow();
    
    //! @brief   Moves read pointer one position forward
    void advanceReadPtr() { readPtr = (readPtr + 1) % bufferSize; }
    
    //! @brief   Moves read pointer forward or backward
    void advanceReadPtr(int steps) { readPtr = (readPtr + bufferSize + steps) % bufferSize; }
    
    //! @brief   Moves write pointer one position forward
    void advanceWritePtr() { writePtr = (writePtr + 1) % bufferSize; }
    
    //! @brief   Moves write pointer forward or backward
    void advanceWritePtr(int steps) { writePtr = (writePtr + bufferSize + steps) % bufferSize; }
    
    //! @brief   Returns number of stored sampes in ringbuffer
    unsigned samplesInBuffer() { return (writePtr + bufferSize - readPtr) % bufferSize; }
    
    //! @brief   Returns remaining storage capacity of ringbuffer
    unsigned bufferCapacity() { return (readPtr + bufferSize - writePtr) % bufferSize; }
    
    /*! @brief   Align write pointer
     *  @details This function puts the write pointer somewhat ahead of the read pointer.
     *           With a standard sample rate of 44100 Hz, 735 samples is 1/60 sec.
     */
    void alignWritePtr() { writePtr = (readPtr + (8 * 735)) % bufferSize; }
};

#endif
