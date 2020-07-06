// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// List of modifications applied to reSID
// 1. Changed visibility of some objects from protected to public

// Good candidate for testing sound emulation: INTERNAT.P00

#ifndef _RESID_H
#define _RESID_H

#include "C64Component.h"
#include "resid/sid.h"

class ReSID : public C64Component {

public:
    
    reSID::SID *sid;
    
public:
    
    void clock() { sid->clock(); }
    
private:
    
    //! ReSID state
    reSID::SID::State st;
    
    //! @brief   The emulated chip model
    SIDModel model;
    
    //! @brief   Clock frequency
    /*! @details Either PAL_CLOCK_FREQUENCY or NTSC_CLOCK_FREQUENCY
     */
    u32 clockFrequency;
    
    //! @brief   Sample rate (usually set to 44.1 kHz)
    u32 sampleRate;
    
    //! @brief   Sampling method
    SamplingMethod samplingMethod;
    
    //! @brief   Switches filter emulation on or off.
    bool emulateFilter;
    
public:
		
    //! Pointer to bridge object
    SIDBridge *bridge;
    
    
    //
    // Constructing and serializing
    //
    
public:
    
	ReSID(C64 &ref);
	~ReSID();
    
    
    //
    // Methods from HardwareComponent
    //
    
public:
    
    void _reset() override;
    void didLoadFromBuffer(u8 **buffer) override { sid->write_state(st); }
    void willSaveToBuffer(u8 **buffer) override { st = sid->read_state(); }

private:

    void _setClockFrequency(u32 value) override;

    
public:
    
    //! @brief    Gathers all values that are displayed in the debugger
    SIDInfo getInfo();
    
    //! @brief    Gathers all debug information for a specific voice
    VoiceInfo getVoiceInfo(unsigned voice);

	//! Special peek function for the I/O memory range.
	u8 peek(u16 addr);
	
	//! Special poke function for the I/O memory range.
	void poke(u16 addr, u8 value);
	
	/* Runs reSID for the specified amount of CPU cycles and writes the
     * generated sound samples into the internal ring buffer.
     */
    void execute(u64 cycles);
	

    // Configuring
    
    //! Returns the chip model
    SIDModel getModel() {
        assert((SIDModel)sid->sid_model == model);
        return model;
    }
    
    //! Sets the chip model
    void setModel(SIDModel m);
    
    //! Returns the clock frequency
    u32 getClockFrequency() {
        assert((u32)sid->clock_frequency == clockFrequency);
        return (u32)sid->clock_frequency;
    }

    //! Returns the sample rate
    u32 getSampleRate() { return sampleRate; }
    
    //! Sets the sample rate
    void setSampleRate(u32 rate);
    
    //! Returns true iff audio filters should be emulated.
    bool getAudioFilter() { return emulateFilter; }
    
    //! Enable or disable audio filter emulation
	void setAudioFilter(bool enable);

    //! Get sampling method
    SamplingMethod getSamplingMethod() {
        assert((SamplingMethod)sid->sampling == samplingMethod);
        return samplingMethod;
    }
    
    //! Set sampling method
    void setSamplingMethod(SamplingMethod value);
};

#endif
