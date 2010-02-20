/*
 * (C) 2006 Jérôme Lang. All rights reserved.
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
 
#ifndef _SIDVOICE_INC
#define _SIDVOICE_INC

#include "basic.h"

//! The SID's waveforms.
enum {
	WAVE_NONE = 0x00,
	WAVE_TRI = 0x10,
	WAVE_SAW = 0x20,
	WAVE_PULSE = 0x40,
	WAVE_NOISE = 0x80
};

//! ADSR envelope states.
enum {
	E_IDLE,
	E_ATTACK,
	E_DECAY,
	E_SUSTAIN,
	E_RELEASE
};


//! Represents one voice of the SID.
class SIDVoice {

private:

	//! Samplerate of voice in Hz.
	uint32_t samplerate;
	
	//! Samplerate as reciprocal in 1/Hz.
	float samplerateRp;
	
	//! Stores frequency factor in order to calculate waveform frequency from register value/
	/*! frequencyFactor = cpuFrequency / 16777216 */
	float frequencyFactor;
	
	//! Holds the frequency register value of voice.
	uint16_t freq;
	
	//! Calculated frequency dependent on register value.
	/*! 
		calcFreq = register value * clockrate/16777216 
	*/
	float calcFreq;
	
	//! Pulsewidth register value.
	uint16_t pw;
	
	//! Calculated duty cycle for pulse waveform.
	uint32_t dutyCycle;
	
	// ADSR stuff
	//! Attack rate of envelope.
	float attackRate;
	//! Decay rate of envelope.
	float decayRate;
	//! Sustain level of envelope.
	float sustainLevel;
	//! Release rate of envelope.
	float releaseRate;
	//! Actual value of envelope.
	float envelopeValue;
	//! Actual state of envelope.
	uint16_t e_state;
	
	
	//! Store reciprocal values of decay and release times in table.
	static const float decayReleaseTimeRpTable[16];
	
	//! Store reciprocal values of attack time in table.
	static const float attackTimeRpTable[16];
	
	//! Recalculates frequency of voice depending of register value
	//  and recalculates value which is added to counter for each sample.
	inline void updateFrequency()
	{
		this->calcFreq = this->freq * this->frequencyFactor;
		this->addToCounter = this->calcFreq * this->samplerateRp;
	}

	
	//! Recalcultares duty cycle depending of pulsewidth register value.
	inline void updateDutyCycle()
	{
		this->dutyCycle = roundf( 100.0f - ( ((float)this->pw) / 40.95f) ); 
	}

public:
		
	//! Pointer to voice that modulates this one.
	SIDVoice *mod_by;
	//! Pointer to voice that is modulated by this one.
	SIDVoice *mod_to;
	
	// control registers
	//! The gate bit.
	/*! Corresponds to bit 0 of control register. */
	bool gate;
	
	//! Synchronisation bit.
	/*! This bit is set for the modulating voice, not for the modulated one (as the SID bits). */
	bool sync;
	
	//! Ring modulation bit.
	/*! Corresponds to bit 2 of control register. */
	bool ring;
	
	//! The test bit.
	/*! Corresponds to bit 3 of control register. */
	bool test;	
	
	//! Selected waveform of voice.
	uint16_t wave;
	
	//! Holds value of counter of waveform generator.
	float counter;
	
	//! Value which is added to counter when a sample is calculated.
	float addToCounter;
	
	//! Counter for randomWave of SID.
	uint32_t randomCount;
	
	//! Last calculated random value of SID::randomWave().
	float lastRandom;
	
	//! Some kind of random register (needed in SID::noise()).
	long randomReg;
	
	//! True if voice is filtered.
	bool filter;	
	
	//! True if voice is muted (voice 3 only).
	bool mute;		
	
	
	
	//////////////////////
	// methods
	//////////////////////
	
	//! Constructor.
	SIDVoice();
	
	
	//! Set samplerate of voice.
	/*! Tell voice at which samplerate SID is working.
		\param sr Samplerate in Hz.
	*/
	void setSamplerate(uint32_t sr)
	{
		this->samplerate = sr;
		this->samplerateRp = (1.0 / ((float)sr));
	}
	
	
	//! Set frequency factor of Emulation.
	/*
		\param frequency Frequency of Emulation in Hz.
	*/
	void setFrequencyFactor(uint32_t cpuFrequency) {this->frequencyFactor = ((float)cpuFrequency) / ((float)16777216);}
	
	
	//! Returns \a calcFreq.
	inline float getFrequency() {return this->calcFreq;  }
	//! Returns \a dutyCycle.
	inline uint32_t getDutyCycle() {return this->dutyCycle; }
	
	//! Returns \a freq.
	uint16_t getFreqRegValue() {return this->freq; }
	
	//! Calculates attack and decay rate of envelope.
	/*! 
		\param value Register value.
	*/
	void setAttackDecay(uint8_t value);
	
	//! Calculates sustain level and release rate of envelope.
	/*!
		\param value Register value.
	*/
	void setSustainRelease(uint8_t value);
	
	//! Generates ADSR envelope.
	float generateEnvelope();
	
	//! Returns \a envelopeValue.
	inline float getEnvelopeValue(){ return envelopeValue;}
	
	//! Starts attack portion of envelope.
	/*! Gates the voice. */
	void on();
	
	//! Starts the release phase of envelope.
	/*! Sound output level will fade away to silence. */
	void off();
		
	//! Sets lower byte of voice frequency and updates \a calcFreq.
	/*!
		\param value Register value.
	*/
	void setFreqLowByte(uint8_t value)
	{
		this->freq = (this->freq & 0xFF00) | value;
		this->updateFrequency();
	}
	
	//! Sets higher byte of voice frequency and updates \a calcFreq.
	/*!
		\param value Register value.
	*/
	void setFreqHighByte(uint8_t value)
	{
		this->freq = (this->freq & 0x00FF) | (value << 8);
		this->updateFrequency();
	}
	
	//! Sets lower byte of pulsewidth and updates \a dutyCycle.
	/*!
		\param value Register value.
	*/
	void setPWLowByte(uint8_t value)
	{
		this->pw = (this->pw & 0xFF00) | value;
		this->updateDutyCycle();
	}
	
	//! Sets higher byte of pulsewidth and updates \a dutyCycle.
	/*!
		\param value Register value.
	*/
	void setPWHighByte(uint8_t value)
	{
		this->pw = (this->pw & 0x00FF) | (value << 8);
		this->pw = (this->pw & 0x0FFF); // make sure that only the bits 0-3 from value are used as pulsewidth 
		this->updateDutyCycle();
	}
	
	//! Resets the voice to default values.
	void reset();
};


#endif