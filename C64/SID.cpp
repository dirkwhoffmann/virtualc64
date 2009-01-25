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

#include "C64.h"

//! Resonance frequency polynomials

#define CALC_RESONANCE_LP(f) (227.755\
				- 1.7635 * f\
				- 0.0176385 * f * f\
				+ 0.00333484 * f * f * f\
				- 9.05683E-6 * f * f * f * f)

#define CALC_RESONANCE_HP(f) (366.374\
				- 14.0052 * f\
				+ 0.603212 * f * f\
				- 0.000880196 * f * f * f)

//! Filter types.
enum {
	FILT_NONE,
	FILT_LP,
	FILT_BP,
	FILT_LPBP,
	FILT_HP,
	FILT_NOTCH,
	FILT_HPBP,
	FILT_ALL
};

const float SID::volumeLevelTable[16] = { 0.0f, 0.07f, 0.13f, 0.20f, 0.27f, 0.33f,
 0.4f, 0.47f, 0.53f, 0.6f, 0.67f, 0.73f, 0.8f, 0.87f, 0.93f, 1.0f };


SID::SID()
{
	debug("  Creating SID at address %p...\n", this);

	// link voices together
	voice[0].mod_by = &voice[2];
	voice[1].mod_by = &voice[0];
	voice[2].mod_by = &voice[1];
	voice[0].mod_to = &voice[1];
	voice[1].mod_to = &voice[2];
	voice[2].mod_to = &voice[0];
	
	// set default samplerate
	setSamplerate(44100);
	
	// use NTSC
	setVideoMode(true);
	
	// by default SID doesn't filter voices
	filtersEnabled = false;
	
	// by default SID treats buffer as interleaved buffer
	mono = false;
		
	// init ringbuffer
	bufferSize = 12288;
	ringBuffer = new float[bufferSize];
	endBuffer = &ringBuffer[(bufferSize - 1)];

	volumeControl = 0.1;
}

SID::~SID()
{
	delete ringBuffer;
	ringBuffer = writeBuffer = readBuffer = endBuffer = NULL;
}

void
SID::reset() 
{
	debug("  Resetting SID...\n");

	// set mastervolume to half amplitude
	masterVolume = 0.5f;
	
	// reset ringBuffer
	for (unsigned i = 0; i < bufferSize; i++)
	{
		ringBuffer[i] = 0.0f;
	}
	readBuffer = ringBuffer;
	writeBuffer = ringBuffer;
	
	preCalcSamples = 0; // precalculated samples in ringbuffer
	
	// filter stuff
	f_type = 0;
	f_freq = 0;
	f_res = 0;
	
	f_ampl = 0.0f; 
	d1 = d2 = g1 = g2 = 0.0f; 
	xn1 = xn2 = yn1 = yn2 = 0.0f;
	
	// stuff in order to control callback
	callbackStarted = false;
	startPlaying = false;
	
	// reset registers
	for (int i = 0; i < NO_OF_REGISTERS; i++)
	{
		iomem[i] = 0x00;
	}
	
	// reset voices
	for (int i = 0; i < 3; i++)
	{
		voice[i].reset();
	}
	
}

void SID::setSamplerate(uint32_t sr) 
{
	this->samplerate = sr;
	for (int i = 0; i < 3; i++)
	{
		this->voice[i].setSamplerate(sr);
	}
	this->updateConstants();
}


void 
SID::setVideoMode(uint32_t frequency)
{
	cpuFrequency = frequency;
	
	// Tell SID's voices how to calculate voice frequency from register values
	for (int i = 0; i < 3; i++)
		voice[i].setFrequencyFactor(this->cpuFrequency);
	
	updateConstants();
}

bool 
SID::load(FILE *file)
{
	debug("  Loading SID state...\n");

	// reset ringbuffer, buffer pointers, callback synchronisation mechanism, etc. 
	this->reset();
	for (int i = 0; i < NO_OF_REGISTERS; i++) 
	{
		// call poke for every register with value from file
		// poke will store this value in iomem[] beside other things
		this->poke(i,read8(file));
	}
	return true;
}

bool
SID::save(FILE *file)
{
	debug("  Saving SID state...\n");

	// store every single register value in file
	for (int i = 0; i < NO_OF_REGISTERS; i++) 
	{
		write8(file, iomem[i]);
	}	
	return true;
}

uint8_t 
SID::peek(uint16_t addr)
{	
	switch(addr) 
	{
		case 0x19: // Potentiometer (paddle) x position
		case 0x1A: // Potentiometer (paddle) y position
			lastByte = 0;
			return 0xff;
		case 0x1B: // Voice 3 oscillator (waveform) output
			lastByte = 0;
			iomem[27] = getOsciOutput();
			return iomem[27];
		case 0x1C: // Voice 3 (envelope generator) ADSR output
			lastByte = 0;
			iomem[28] = getEGOutput();
			return iomem[28];
		// the unused registers - always return 0
		case 0x1D:
		case 0x1E:
		case 0x1F:
			return lastByte;
		default:
			return lastByte;
			
	}
}

void 
SID::poke(uint16_t addr, uint8_t value)
{
	lastByte = value; // local copy for peek()
	
	iomem[addr] = value; // store value in SID I/O Memory
	
	short v = addr / 7; // get which voice
	
	switch(addr) {
		// frequency lowbyte
		case 0x00: 
		case 0x07:
		case 0x0E:
			voice[v].setFreqLowByte(value);
			break;
		// frequency highbyte
		case 0x01: 
		case 0x08:
		case 0x0F:
			voice[v].setFreqHighByte(value);
			break;
		// pulsewidth lowbyte
		case 0x02:
		case 0x09:
		case 0x10:
			voice[v].setPWLowByte(value);
			break;
		// pulsewidth highbyte
		case 0x03:
		case 0x0A:
		case 0x11:
			voice[v].setPWHighByte(value);
			break;
		// control register
		case 0x04:
		case 0x0B:
		case 0x12:
			// waveform: bit 4-7
			voice[v].wave = (value & 0xF0);
			
			// set envelope state depending of gate flag
			if ((value & 0x1) != voice[v].gate) // set only if value changes
				if (value & 0x1) // gate turned on
					voice[v].on(); // turn on
				else // gate turned off
					voice[v].off(); // turn off
			// set gate flag
			voice[v].gate = (value & 0x1); // bit 0
			// sync flag
			voice[v].mod_by->sync = (value  & 0x2); // bit 1
			// ring flag
			voice[v].ring = (value & 0x4); // bit 2
			// test flag
			voice[v].test = (value & 0x8); // bit 3
			if (voice[v].test == 1)
			{
				voice[v].counter = 0.0f; //reset counter to zero
				voice[v].randomReg = 0x7ffff8; // reset random waveform
			}
			break;
		// attack/delay register
		case 0x05:
		case 0x0C:
		case 0x13:
			voice[v].setAttackDecay(value);
			break;
		// sustain/release register	
		case 0x06:
		case 0x0D:
		case 0x14:
			voice[v].setSustainRelease(value);
			break;
		/* filter stuff */
		//case 0x15: ignore lower 3 bits of cutoff frequency
		// filter cutoff frequency (upper 8 bits)
		case 0x16:
			if (value != f_freq)
			{
				f_freq = value;
				if (filtersEnabled)
					computeFilter();
			}
			break;
		// is voice filtered, set resonance frequency
		case 0x17:
			voice[0].filter = value & 0x1;
			voice[1].filter = value & 0x2;
			voice[2].filter = value & 0x4;
			if ( (value >> 4) != f_res)
			{
				f_res = value >> 4; // bits 4-7
				if (filtersEnabled)
					computeFilter();
			}
			break;
		// volume/filter mode register
		case 0x18:
			// set master volume
			this->masterVolume = SID::volumeLevelTable[(value & 0x0F)]; 
			// bit 7: muting voice 3
			voice[2].mute = ((value & 0x80) >> 7);
			
			// again some filter stuff
			// bits 4-6: tells us which filter type is activated
			if ( ((value >> 4) & 0x7) != f_type)
			{
				f_type = (value >> 4) & 0x7;
				xn1 = xn2 = yn1 = yn2 = 0.0f;
				if (filtersEnabled)
					computeFilter();
			}
			break;
		// unused, do nothing
		case 0x1D: 
		case 0x1E:
		case 0x1F:
			break;
	}
}



bool 
SID::execute(int elapsedCycles)
{
	// get filter coefficients, so the emulator won't change
	// them in the middle of our calculations
	float cf_ampl = f_ampl;
	float cd1 = d1, cd2 = d2, cg1 = g1, cg2 = g2;
	
	// calculate how many samples we have to generate for this video frame
	// (at samplerate of 44,1kHz and NTSC video mode (60Hz) it should be about 735 samples) 
	int samples = lroundf( elapsedCycles * this->samplerateCpuFrequencyRp );
		
	// store here how many samples we've generated in advance of callback
	this->preCalcSamples += samples;
	
	// generate samples
	for (register int j = 0; j < samples; j++)
	{
		float sum_output = 0.0f;
		float sum_output_filter = 0.0f;
		// for all three voices
		for (register uint32_t i = 0; i < 3; i++)
		{
			SIDVoice *v = &voice[i];
			float output = 0.0f;
			
			if (v->test == 1)	// test flag set
				continue;		// no sound output, don't increment counter
			
			// increment counter
			v->counter += v->addToCounter;
			if (v->counter > 1.0f)
			{
				// sync bit set, modulate other voice
				if (v->sync == 1)
					v->mod_to->counter = 0.0f;
					
				v->counter = (v->counter - 1.0f); // is equal to: (v->counter modulo 1.0f)
			} 
							
			// get wavevalue
			switch(v->wave)
			{
				case WAVE_NONE:
					break;
				case WAVE_TRI:
					if (v->ring)
						// ring modulation on
						output = this->triangleWave(v) * this->squareWave(v->mod_by) * v->generateEnvelope();
					else 
						// ring modulation off
						output = triangleWave(v) * v->generateEnvelope();
					break;
				case WAVE_SAW:
					output = sawtoothWave(v) * v->generateEnvelope();
					break;
				case WAVE_PULSE:
					output = pulseWave(v) * v->generateEnvelope();
					break;
				case WAVE_NOISE:
					output = randomWave(v) * v->generateEnvelope();
					break;
				default:
					break;
			}
			if (v->filter)
				sum_output_filter += output;
			else if (!v->mute) // if voice 3 is not muted and not filtered 
				sum_output += output;
		}
		
		// filter output
		if (filtersEnabled)
		{
			float xn = (float)sum_output_filter * cf_ampl;
			float yn = xn + cd1 * xn1 + cd2 * xn2 - cg1 * yn1 - cg2 * yn2;
			yn2 = yn1; yn1 = yn; xn2 = xn1; xn1 = xn;
			sum_output_filter = yn;
		}
		writeData((sum_output + sum_output_filter) * volumeControl * this->masterVolume * 0.33f); // why 0.33f? -> overall sounds of emulator becomes way to loud compared with
																				  // other systems sounds if don't reduced. Nice side effect: samples don't
																				  // leave interval [-1.0, 1.0]
	}
	
	// give SID emulator an advance of about 8 NTSC frames, than let callback take data from ringbuffer
	// latency is used to balance minor speed variations of emulated CPU
	if ((!callbackStarted) && (this->preCalcSamples > (8 * 735))) 
	{
		this->callbackStarted = true;
		this->startPlaying = true;
	}
	return true;
}


// generate triangle waveform
float SID::triangleWave(SIDVoice* voice)
{	
	if (voice->getFreqRegValue() == 0)
		return 0.0f;
	
	// get phase of voice
	float pmod = voice->counter * 360.0;
	
	float tri;
	
	if (0.0f <= pmod && pmod < 90.0f)
	{
		tri = 2 * pmod/180.0f;
	}
	else if (90.0f <= pmod && pmod < 270.0f)
	{
		tri = 2 * (1.0f - pmod/180.0f);
	}
	else // 270.0  <= pmod < 360.0
	{
		tri = 2 * (pmod/180.0f - 2.0f);
	}
		  
	return tri;
}

// generate sawtooth waveform
float SID::sawtoothWave(SIDVoice* voice)
{
	if (voice->getFreqRegValue() == 0)
		return 0.0f;
	
	// get phase of voice
	float pmod = voice->counter * 360.0;
	
	float saw;
	
	if (0.0f <= pmod && pmod < 180.0f)
	{
		saw = pmod/180.0f;
	}
	else // 180.0 <= pmod < 360.0
	{
		saw = (pmod/180.0f - 2.0f);
	}
	
	return saw;
}

float SID::pulseWave(SIDVoice* voice)
{
	// frequency is zero -> no sound output
	if (voice->getFreqRegValue() == 0)
		return 0.0f;
	
	// get phase of voice
	float pmod = voice->counter * 360.0;
	
	float pulse;
	
	if (0.0f <= pmod && pmod < ((0.01f * voice->getDutyCycle()) * 360.0f) )
	{
		pulse = 1.0f;
	} 
	else // if <= (0.01 × duty) × 360.0  pmod < 360.0
	{
		pulse = -1.0f;
	}
	
	return pulse;
}

float SID::randomWave(SIDVoice* voice)
{
	// number of samples with same random values depends from value of frequency register
	uint32_t samples = lroundf(this->samplerate/((float)voice->getFreqRegValue()));
	
	if (voice->randomCount >= samples) // change output level
	{
		voice->randomCount = 0; // reset count
		voice->lastRandom = this->noise(voice);  // get random value and store it
		return voice->lastRandom;
	}
	else
	{
		voice->randomCount++;
		return voice->lastRandom;
	}
}


float SID::noise(SIDVoice* voice)
{
	long bit22;	/* Temp. to keep bit 22 */
	long bit17;	/* Temp. to keep bit 17 */
	
	
	long reg= voice->randomReg; /* Initial value of internal register*/
		
	/* Pick out bits to make output value */
	uint8_t output =	(bit(reg,22) << 7) |
						(bit(reg,20) << 6) |
						(bit(reg,16) << 5) |
						(bit(reg,13) << 4) |
						(bit(reg,11) << 3) |
						(bit(reg, 7) << 2) |
						(bit(reg, 4) << 1) |
						(bit(reg, 2) << 0);
		
	/* Save bits used to feed bit 0 */
	bit22 = bit(reg,22);
	bit17 = bit(reg,17);
		
	/* Shift 1 bit left */
	voice->randomReg = reg << 1;
		
	/* Feed bit 0 */
    voice->randomReg = voice->randomReg | (bit22 ^ bit17);

	// convert from 8bit integer to float32
	return (output - 127.5f) / 127.5f; // 0-255 -> -1 - +1

	//return result;

}

inline long SID::bit(long val, uint8_t bitnr)
{
	return (val & (1<<bitnr))? 1:0;
}



// this method is needed for the ring modulation
// we need a symmetrical square wave
float SID::squareWave(SIDVoice* voice)
{
	float pmod = voice->counter * 360.0;
	float pulse;
	
	if (0.0f <= pmod && pmod < 180.0f)
	{
		pulse = 1.0f;
	} 
	else // 180.0 <= pmod < 360.0
	{
		pulse = -1.0f;
	}
	return pulse;
}

void 
SID::mix(float* myOutBuffer, uint32_t size)
{	
	if (isRunning() && startPlaying) // C64 running && ringbuffer has enough advance
	{
		if (mono) // myOutBuffer is noninterleaved
		{
			for (unsigned i = 0; i < size; i++)
			{
				// fill buffer
				myOutBuffer[i] = readData();
			}
		}
		else // myOutBufer is a interleaved buffer
		{
			for (unsigned i = 0; i < size; i++)
			{
				// fill buffer
				float value = readData();
				myOutBuffer[i*2] = value;		// left channel	
				myOutBuffer[i*2+1] = value;		// right channel
			}
		}
	}
	else // C64 paused there won't be any reasonable data in buffer
	{
		if (mono) // myOutBuffer is noninterleaved
		{
			for (unsigned i = 0; i < size; i++)
			{
				// fill buffer with no sound
				myOutBuffer[i] = 0.0f;
			}
		}
		else // myOutBufer is a interleaved Buffer
		{
			for (unsigned i = 0; i < size; i++)
			{
				// fill buffer with no sound
				myOutBuffer[i*2] = 0.0f;		// left channel	
				myOutBuffer[i*2+1] = 0.0f;		// right channel
			}
		}
	}
}


float SID::readData()
{		
	//! There's no error handling in case that the reading pointer gets ahead
	//! of the writing pointer, because of real time demand.
	//! If this happens CPU emulation doesn't compute at 100 percent on average.
	if (readBuffer == (endBuffer+1)) // end of buffer reached
		readBuffer = ringBuffer; // pass 'go' and don't collect $200	
	float value = *readBuffer;
	readBuffer++;
	return value;
}
	
void SID::writeData(float data)
{
	if (writeBuffer == (endBuffer+1)) // end of buffer reached
		writeBuffer = ringBuffer; // pass 'go' and don't collect $200
	*writeBuffer = data;
	writeBuffer++;
}
	
uint8_t SID::getOsciOutput()
{
	// get wavevalue
	float output = 0.0f;
	SIDVoice* v = &voice[2];
	switch(v->wave)
	{
		case WAVE_NONE:
			break;
		case WAVE_TRI:
			output = triangleWave(v);
			break;
		case WAVE_SAW:
			output = sawtoothWave(v);
			break;
		case WAVE_PULSE:
			output = pulseWave(v);
			break;
		case WAVE_NOISE:
			output = v->lastRandom;
			break;
	}
	return lroundf( (output + 1.0f) * 127.5f );
}

uint8_t SID::getEGOutput()
{
	return lroundf(voice[2].getEnvelopeValue() * 255);
}

void SID::dumpState()
{
	debug("SID\n");
	debug("---\n\n");
	debug("   Sample rate : %d\n", samplerate);
	debug(" CPU frequency : %d\n", cpuFrequency);
	debug("        Buffer : %s\n", mono ? "continous" : "interleaved");
	debug("   Buffer size : %d\n", bufferSize);
	debug("        Volume : %f\n", masterVolume);
	debug("  Sound filter : %s\n", filtersEnabled ? "on" : "off");
	
	debug("\n     IO memory : ");
	for (int i = 0; i < 32; i += 16) {
		for (int j = 0; j < 16; j ++) {
			debug("%02X ", iomem[i + j]);
		}
		debug("\n                 ");
	}
	debug("\n");
}


void SID::computeFilter()
{
	//! The filters implementation is taken from SID implementation of Frodo (http://frodo.cebix.net/)
	//! because it would go beyond scope of my student research project (there are a lot of empirical values).
	
	float fr, arg;

	// Check for some trivial cases
	if (f_type == FILT_ALL) {
		d1 = 0.0; d2 = 0.0;
		g1 = 0.0; g2 = 0.0;
		f_ampl = 1.0;
		return;
	} else if (f_type == FILT_NONE) {
		d1 = 0.0; d2 = 0.0;
		g1 = 0.0; g2 = 0.0;
		f_ampl = 0.0;
		return;
	}
	
	// Calculate resonance frequency
	if (f_type == FILT_LP || f_type == FILT_LPBP)
		fr = CALC_RESONANCE_LP(f_freq);
	else
		fr = CALC_RESONANCE_HP(f_freq);

	// Limit to <1/2 sample frequency, avoid div by 0 in case FILT_BP below
	arg = fr / (float)(samplerate >> 1);
	if (arg > 0.99)
		arg = 0.99;
	if (arg < 0.01)
		arg = 0.01;

	// Calculate poles (resonance frequency and resonance)
	// remember: Poles are containers filled with magic powder. 
	// The more poles in a filter, the better the filter works.
	g2 = 0.55 + 1.2 * arg * arg - 1.2 * arg + (float)f_res * 0.0133333333;
	g1 = -2.0 * sqrt(g2) * cos(M_PI * arg);

	// Increase resonance if LP/HP combined with BP
	if (f_type == FILT_LPBP || f_type == FILT_HPBP)
		g2 += 0.1;

	// Stabilize filter
	if (fabs(g1) >= g2 + 1.0)
		if (g1 > 0.0)
			g1 = g2 + 0.99;
		else
			g1 = -(g2 + 0.99);

	// Calculate roots (filter characteristic) and input attenuation
	switch (f_type) {

		case FILT_LPBP:
		case FILT_LP:
			d1 = 2.0; d2 = 1.0;
			f_ampl = 0.25 * (1.0 + g1 + g2);
			break;

		case FILT_HPBP:
		case FILT_HP:
			d1 = -2.0; d2 = 1.0;
			f_ampl = 0.25 * (1.0 - g1 + g2);
			break;

		case FILT_BP:
			d1 = 0.0; d2 = -1.0;
			f_ampl = 0.25 * (1.0 + g1 + g2) * (1 + cos(M_PI * arg)) / sin(M_PI * arg);
			break;

		case FILT_NOTCH:
			d1 = -2.0 * cos(M_PI * arg); d2 = 1.0;
			f_ampl = 0.25 * (1.0 + g1 + g2) * (1 + cos(M_PI * arg)) / (sin(M_PI * arg));
			break;

		default:
			break;
	}
}


