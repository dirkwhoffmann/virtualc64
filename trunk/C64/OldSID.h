/*
 * (C) 2006 Dirk W. Hoffmann, Jérôme Lang. All rights reserved.
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

#ifndef _OLDSID_INC
#define _OLDSID_INC

#include "VirtualComponent.h"
#include "SIDVoice.h"
#include "sid.h"

//! The virtual sound interface device (SID)
/*! SID is the sound chip of the Commodore 64.
	The SID chip occupied the memory mapped I/O space from address 0xD400 to 0xD7FF. 
*/
class OldSID : public VirtualComponent {

    friend class SIDWrapper;

public:	
	//! Table holding volume levels of SID.
	static const float volumeLevelTable[16];
    
private:    
	//! SID I/O Memory.
	/*! If a value is poked to the SID address space, it is stored here.
	    The first 25 are write only, the following 4 are read only */
	uint8_t iomem[32];
	
	//! Samplerate of SID.
	uint32_t samplerate;
	
	//! CPU frequency of Emulation (NTSC or PAL).
	uint32_t cpuFrequency;
	
	//! Precalculated constant (for performance reasons) holding value of \a samplerate * ( 1.0f/ \a cpuFrequency).
	float samplerateCpuFrequencyRp;
	
	//! The 3 voices of the SID.
	SIDVoice voice[3];
	
	//! Stores copy of last written byte via poke().
	uint8_t lastByte;
	
	//! Size of \a ringBuffer.
	uint32_t bufferSize;
	
	//!  Ringbuffer with sound data.
	float* ringBuffer;
	
	//! Pointer to write position in buffer.
	float* writeBuffer;
	
	//! Pointer to read position in buffer.
	float* readBuffer;
	
	//! Pointer to end of buffer.
	float* endBuffer;
	
	//! True if output buffer filled in mix() with samples is not a interleaved buffer.
	// bool mono;
		
	//! Stores number of precalculated samples in \a ringBuffer.
	uint32_t preCalcSamples;
	
	//! True if callback is started.
	bool callbackStarted;
	
	//! True if mix() is allowed to read from \a ringBuffer.
	bool startPlaying;
	
	//! Master volume value.
	/*! Value from 0 - 1.0f. */
	float masterVolume;

	//! Volume control
	/*! This value is used to finally scale the computed waveform samples.
		The volume control of the GUI is controlling this value to increase or decrease loundness.
		Range: 0.0 (silent) - 1.0 (no effect, full blast) 
	*/
	float volumeControl;
	
	/////////////////////////////////////
	// members for filters implementation
	/////////////////////////////////////
	
	//! Filter type
	uint8_t f_type;
	
	//! SID filter frequency (upper 8 bits).
	uint8_t f_freq;
	
	//! Filter resonance (0..15).
	uint8_t f_res;	
	
	//! IIR filter input attenuation.
	float f_ampl;
	
	//! IIR filter coefficient.
	float d1;
	
	//! IIR filter coefficient.
	float d2;
	
	//! IIR filter coefficient.
	float g1;
	
	//! IIR filter coefficient.
	float g2;
	
	//! IIR filter previous input/output signal.
	float xn1;
	
	//! IIR filter previous input/output signal.
	float xn2;
	
	//! IIR filter previous input/output signal.
	float yn1;
	
	//! IIR filter previous input/output signal.
	float yn2; 
	
	//! True if filters are enabled.
	bool filtersEnabled;
	
	///////////////////
	// methods
	//////////////////
		
	//! Compute filter.
	void computeFilter();
	
    //! Invoked on buffer underflow or overflow
    void handleBufferException();
    	
	//! Write sample to \a ringBuffer.
	void writeData(float data);
	
	//! Returns oscillator output of voice 3.
	/*! Implements read-register 0x1B. */
	uint8_t getOsciOutput();
	
	//! Returns envelope generator output of voice 3. 
	/*! Implements read-register 0x1C. */
	uint8_t getEGOutput();
	
	//! Recalculate constants depending of \a samplerate and \a cpuFrequency. 	
	inline void updateConstants()
	{
		this->samplerateCpuFrequencyRp = this->samplerate * ( 1.0/(float)this->cpuFrequency );
	}
	
	// waveforms
	//! Generate triangle waveform sample.
	/*! 
		\param voice Pointer to voice for which sample will be generated.
	*/
	float triangleWave(SIDVoice* voice);
	
	//! Generate sawtooth waveform sample.
	/*! 
		\param voice Pointer to voice for which sample will be generated.
	*/
	float sawtoothWave(SIDVoice* voice);
	
	//! Generate pulse waveform sample.
	/*! 
		\param voice Pointer to voice for which sample will be generated.
	*/
	float pulseWave(SIDVoice* voice);
	
	//! Generate noise waveform sample.
	/*! 
		\param voice Pointer to voice for which sample will be generated.
	*/
	float randomWave(SIDVoice* voice);
	
	//! Generate noise output (according to algorithm of Andreas Varg).
	/*! 
		Helper method for randomWave().
		See http://stud1.tuwien.ac.at/~e9426444/ for further information.
		\param voice Pointer to voice for which random number will be generated.
	*/
	float noise(SIDVoice* voice);
	
	//! Helper for noise().
	long bit(long val, uint8_t bitnr);

	//! Generate square wave sample.
	/*!
		\param voice Indicates for which voice sample will be generated.
	*/
	float squareWave(SIDVoice* voice);

public:	
	//! Returns true if the \a addr is located in the I/O range of the SID chip.
	//static inline bool isSidAddr(uint16_t addr) 
	//{ return (SID_START_ADDR <= addr && addr <= SID_END_ADDR); }
	
	//! Constructor.
	OldSID();
	
	//! Destructor.
	~OldSID();
	
	//! Bring the SID chip back to it's initial state.
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
	
	//! Enable or disable filters of SID.
	/*!
		By default filters of SID are disabled.
		\param enable True to switch filters on, else false.
	*/
	void setAudioFilter(bool enable) {this->filtersEnabled = enable;}

	//! Return volumeControl value
	float getVolumeControl() { return volumeControl; }
	
	//! Set volume control value
	void setVolumeControl(float value) { debug(1, "New volume: %f\n", 1-log10(9*(1-value) + 1)); volumeControl = 1-log10(9*(1-value) + 1); } // value*value; }
		
	//! Sets samplerate of SID and it's 3 voices.
	/*!	
		\param sr Samplerate in Hz.
	*/
	void setSampleRate(uint32_t sr);
	
	//! Returns samplerate of SID.
	uint32_t getSampleRate() {return this->samplerate;}
	
	//! Set clock frequency
	/*! Value differs between PAL and NTSC machines */
	void setClockFrequency(uint32_t frequency);	
};

#endif
