/*!
 * @header      SidWrapper.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2011 - 2016 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _SIDWRAPPER_INC
#define _SIDWRAPPER_INC

#include "VirtualComponent.h"
#include "OldSID.h"
#include "ReSID.h"

class SIDWrapper : public VirtualComponent {

    
public:	
	//! @brief    Start address of the SID I/O space.
	static const uint16_t SID_START_ADDR = 0xD400;
    
	//! @brief    End address of the SID I/O space.
	static const uint16_t SID_END_ADDR = 0xD7FF;

private:

    //! @brief    Old SID implementation
    OldSID *oldsid;

public:
    //! @brief    Implementation based on the ReSID library
    ReSID *resid;
   
private:
    //! @brief    SID selector
    bool useReSID;
    
    //! @brief    Remembers latest written value
    uint8_t latchedDataBus;
    
public:
    //! @brief    Returns true if the addr is located in the I/O range of the SID chip.
	static inline bool isSidAddr(uint16_t addr) 
	{ return (SID_START_ADDR <= addr && addr <= SID_END_ADDR); }
	
	//! @brief    Constructor
	SIDWrapper();
	
	//! @brief    Destructor
	~SIDWrapper();
			
	//! @brief    Prints debug information
	void dumpState();
	
    
    // -----------------------------------------------------------------------------------------------
	//                                         Configuring
	// -----------------------------------------------------------------------------------------------

    //! @brief    Configures the SID chip for being used in PAL machines.
    void setPAL();
    
    //! @brief    Configures the SID chip for being used in NTSC machines.
    void setNTSC();
    
    //! @brief    Returns true, whether ReSID or the old implementation should be used.
    inline bool getReSID() { return useReSID; }
    
    //! @brief    Enables or disables the ReSID library.
    void setReSID(bool enable);
    
    //! @brief    Returns the simulated chip model.
    inline chip_model getChipModel() { return resid->getChipModel(); }
    
    //! @brief    Sets chip model (ReSID only)
    void setChipModel(chip_model value);
    
    //! @brief    Returns true iff audio filters are enabled.
    inline bool getAudioFilter() { return resid->getExternalAudioFilter(); }
    
    //! @brief    Enables or disables filters of SID.
    void setAudioFilter(bool enable);
    
    //! @brief    Returns the sampling method.
    inline sampling_method getSamplingMethod() { return resid->getSamplingMethod(); }
    
    //! @brief    Sets the sampling method (ReSID only).
    void setSamplingMethod(sampling_method value);

    //! @brief    Returns the sample rate.
	inline uint32_t getSampleRate() { return resid->getSampleRate(); }
    
	//! @brief    Sets the samplerate of SID and it's 3 voices.
	void setSampleRate(uint32_t sr);
    
    //! @brief    Returns the clock frequency.
    inline uint32_t getClockFrequency();
    
	//! @brief    Sets the clock frequency.
	void setClockFrequency(uint32_t frequency);	

    //! @brief    Sets the current volume
    void setVolume(int32_t v) { resid->setVolume(v); }
    
    /*! @brief   Triggers volume ramp up phase
     *  @details Configures volume and targetVolume to simulate a smooth audio fade in
     */
    void rampUp() { resid->rampUp(); }
    void rampUpFromZero() { resid->rampUpFromZero(); }

    /*! @brief   Triggers volume ramp down phase
     *  @details Configures volume and targetVolume to simulate a quick audio fade out
     */
    void rampDown() { resid->rampDown(); }

    //! @brief    Clears the audio sample ringbuffer
    void clearRingbuffer() { resid->clearRingbuffer(); }


    // -----------------------------------------------------------------------------------------------
	//                                           Execution
	// -----------------------------------------------------------------------------------------------

private:
    
    //! @brief    Current clock cycle since power up
    uint64_t cycles;

public:
    
    /*! @brief    Executes SID until a certain cycle is reached
     *  @param    cycle The target cycle
     */
    void executeUntil(uint64_t targetCycle);

    /*! @brief    Executes SID for a certain number of cycles
     *  @param    cycles Number of cycles to execute
     */
	void execute(uint64_t numCycles);

    //! @brief    Notifies the SID chip that the emulator has started
    void run();
	
	//! @brief    Notifies the SID chip that the emulator has started
	void halt();

    
	// -----------------------------------------------------------------------------------------------
	//                                       Getter and setter
	// -----------------------------------------------------------------------------------------------
    
	//! @brief    Special peek function for the I/O memory range.
	uint8_t peek(uint16_t addr);
	
	//! @brief    Special poke function for the I/O memory range.
	void poke(uint16_t addr, uint8_t value);
	
    //! @brief    Reads next audio sample from the ringbuffer
	float readData();
    
    //! @brief    Read a certain amount of mono samples fro the ringbuffer
    void readMonoSamples(float *target, size_t n);

    //! @brief    Read a certain amount of stereo samples fro the ringbuffer
    void readStereoSamples(float *target1, float *target2, size_t n);

    //! @brief    Read a certain amount of stereo samples fro the ringbuffer
    void readStereoSamplesInterleaved(float *target, size_t n);

};

#endif
