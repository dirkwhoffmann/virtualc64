/*
 * (C) 2011 Dirk W. Hoffmann, All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
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

#ifndef _SIDWRAPPER_INC
#define _SIDWRAPPER_INC

#include "VirtualComponent.h"
#include "OldSID.h"
#include "ReSID.h"

class SIDWrapper : public VirtualComponent {

    friend class C64;
    
public:	
	//! Start address of the SID I/O space.
	static const uint16_t SID_START_ADDR = 0xD400;
	//! End address of the SID I/O space.
	static const uint16_t SID_END_ADDR = 0xD7FF;

private:
    //! Old SID implementation
    OldSID *oldsid;

    //! Implementation based on the ReSID library
    ReSID *resid;
    
    //! SID selector
    bool useReSID;
    
public:
    //! Returns true if the \a addr is located in the I/O range of the SID chip.
	static inline bool isSidAddr(uint16_t addr) 
	{ return (SID_START_ADDR <= addr && addr <= SID_END_ADDR); }
	
	//! Constructor.
	SIDWrapper();
	
	//! Destructor.
	~SIDWrapper();
	
	//! Bring SID back to it's initial state.
	void reset();
	
	//! Load state
	void loadFromBuffer(uint8_t **buffer);
	
	//! Save state
	void saveToBuffer(uint8_t **buffer);	
	
	//! Dump internal state to console
	void dumpState();
	
	//! Special peek function for the I/O memory range.
	uint8_t peek(uint16_t addr);
	
	//! Special poke function for the I/O memory range.
	void poke(uint16_t addr, uint8_t value);
	
	//! Pass control to the SID chip.
	/*! The SID will be executed and generate audio samples for about one video frame.
     Actual number of generated samples depends on executed CPU cycles since last call.
     \param cycles Number of cycles to execute (ignored).
     */
	bool execute(int cycles);
	
    //! Get next sample from \a ringBuffer.
	float readData();
	
    //! Returns true, iff ReSID libray shall be used.
    inline bool getReSID() { return useReSID; }
    
    //! Return samplerate.
	inline uint32_t getSampleRate() { return resid->getSampleRate(); }

	//! Sets samplerate of SID and it's 3 voices.
	void setSampleRate(uint32_t sr);

    //! Get clock frequency
	// inline uint32_t getClockFrequency() { return resid->getClockFrequency(); }	
    
	//! Set clock frequency
	void setClockFrequency(uint32_t frequency);	
    
private:
    //! Enable or disable ReSID library.
	void setReSID(bool enable);
    
    //! Set sampling method (ReSID only)
    void setSamplingMethod(sampling_method value);
    
    //! Set chip model (ReSID only)
    void setChipModel(chip_model value);
    
    //! Enable or disable filters of SID.
	void setAudioFilter(bool enable);
};

#endif
