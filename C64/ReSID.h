/*!
 * @header      ReSID.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2011 - 2015 Dirk W. Hoffmann
 * @brief       Declares ReSID wrapper class
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

// Good candidate for testing sound emulation: INTERNAT.P00

#ifndef _RESID_INC
#define _RESID_INC

#include "VirtualComponent.h"
#include "sid.h"

class ReSID : public VirtualComponent {

private:
    
    SID *sid;
    
    /*! @brief   Currently used chip model.
     *  @details MOS6581 is the older SID chip exhibiting the "volume bug".
     *           This chip must be selected to hear synthesized speech.
     *           MOS8580 is the newer SID chip model with the "volume bug" fixed.
     */
    chip_model chipModel;
	
	/*! @brief   Sample rate
     *  @details By default, a sample rate of 44.1 kHz is used. 
     */
	uint32_t sampleRate;
	
    /*! @brief   Sampling method used by the reSID library
     */
    sampling_method samplingMethod;
    
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
    const static size_t bufferSize = 12288;
    // const static size_t bufferSize = 44100;
	
	/*! @brief   The audio sample ringbuffer.
     *  @details This ringbuffer serves as the data interface between the SID emulation code and 
     *           computers audio API (CoreAudio on Mac OS X).
     */
    float ringBuffer[bufferSize];

public:
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
        
private:
    
    /*! @brief   Maximum volume
     */
    const static int32_t maxVolume = 100000;

    /*! @brief   Volume offset
     *  @details If the current volume does not match the target volume, it is increased or decreased by the 
     *           specified amount. The increase or decrease takes place whenever an audio sample is generated.
     */
    int32_t volumeDelta;

public:
    unsigned readDataCnt, writeDataCnt;
    
public:
		
	//! Constructor.
	ReSID();
	
	//! Destructor.
	~ReSID();
	
	//! Bring the SID chip back to it's initial state.
	void reset();
	
    //! Load state
	void loadFromBuffer(uint8_t **buffer);
		
	//! Dump internal state to console
	void dumpState();
	
	//! Special peek function for the I/O memory range.
	uint8_t peek(uint16_t addr);
	
	//! Special poke function for the I/O memory range.
	void poke(uint16_t addr, uint8_t value);
	
	//! Execute SID
	void execute(int cycles);
	
    //! Notifies the SID chip that the emulator has started
    void run();
	
	//! Notifies the SID chip that the emulator has started
	void halt();

    /*! @brief  Reads audio sample from ringbuffer
     *  @see    handleBufferUnderflow
     */
	float readData();

    /*! @brief  Reads audio sample from ringbuffer
     *  @see    handleBufferOverflow
     */
    void writeData(float data);

    /*! @brief  Clears the ringbuffer and resets the read and write pointer to its inital values
     */
    void clearRingbuffer();


    // Configuring
    
    //! Returns true iff audio filters are enabled.
	inline bool getAudioFilter() { return audioFilter; }

    //! Returns true iff external audio filters are enabled.
    inline bool getExternalAudioFilter() { return externalAudioFilter; }

	//! Enable or disable reSIDs audio filtering capability
	void setAudioFilter(bool enable);

    //! Enable or disable reSIDs external audio filtering capability
    void setExternalAudioFilter(bool enable);

    //! Return samplerate.
	inline uint32_t getSampleRate() { return sampleRate; }
    
	//! Set sample rate 
	void setSampleRate(uint32_t sr);
    
    //! Get sampling method
    inline sampling_method getSamplingMethod() { return samplingMethod; }
    
    //! Set sampling method
    void setSamplingMethod(sampling_method value);
    
    //! Get chip model 
    inline chip_model getChipModel() { return chipModel; }
    
    //! Set chip model 
    void setChipModel(chip_model value);
    
    //! Get clock frequency
    uint32_t getClockFrequency() { return cpuFrequency; }
    
	//! Set clock frequency
    void setClockFrequency(uint32_t f);

    /*! @brief Sets the current volume
     */
    void setVolume(int32_t v) { volume = v; }

    /*! @brief Sets the target volume
     */
    void setTargetVolume(int32_t targetVolume) { targetVolume = volume; }

    /*! @brief   Triggers volume ramp up phase
     *  @details Configures volume and targetVolume to simulate a smooth audio fade in
     */
    void rampUp() { targetVolume = maxVolume; volumeDelta = 3; }
    void rampUpFromZero() { volume = 0; targetVolume = maxVolume; volumeDelta = 3; }

    /*! @brief   Triggers volume ramp down phase
     *  @details Configures volume and targetVolume to simulate a quick audio fade out
     */
    void rampDown() { targetVolume = 0; volumeDelta = 50; }

    /*! @brief   Align write pointer
     *  @details This function puts the write pointer somewhat ahead of the read pointer
     */
    void alignWritePtr() { writePtr = (readPtr + (8 * 735)) % bufferSize; }
};

#endif
