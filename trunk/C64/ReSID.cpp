/*
 * (C) 2011 Dirk W. Hoffmann. All rights reserved.
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

ReSID::ReSID()
{
	name = "ReSID";

	debug(2, "  Creating ReSID at address %p...\n", this);

    reSID = new SID();
    reSID->enable_filter(false);
    reSID->enable_external_filter(false);
    reSID->set_chip_model(MOS8580);
    	
	// set default samplerate
	setSampleRate(44100);
		
	// by default SID doesn't filter voices
	filtersEnabled = false;
			
	// init ringbuffer
	bufferSize = 12288;
	ringBuffer = new float[bufferSize];
	endBuffer = &ringBuffer[(bufferSize - 1)];
}

ReSID::~ReSID()
{
    delete reSID;
	delete ringBuffer;
	ringBuffer = writeBuffer = readBuffer = endBuffer = NULL;
}

void
ReSID::reset() 
{
	debug(2, "  Resetting ReSID...\n");
	
	// reset ringBuffer
	for (unsigned i = 0; i < bufferSize; i++)
	{
		ringBuffer[i] = 0.0f;
	}
	readBuffer = ringBuffer;
	writeBuffer = ringBuffer;	
}

void 
ReSID::setSampleRate(uint32_t sr) 
{
	samplerate = sr;
}


void 
ReSID::setClockFrequency(uint32_t frequency)
{
    reSID->set_sampling_parameters(frequency, SAMPLE_FAST, 44100);
    // reSID->adjust_sampling_frequency(frequency);

	cpuFrequency = frequency;
}

void 
ReSID::loadFromBuffer(uint8_t **buffer)
{
	debug(2, "  Loading ReSID state...\n");

    debug(1, "IMPLEMENTATION MISSING");
}

void
ReSID::saveToBuffer(uint8_t **buffer)
{
	debug(2, "  Saving ReSID state...\n");

    debug(1, "IMPLEMENTATION MISSING");
}

uint8_t 
ReSID::peek(uint16_t addr)
{	
    return reSID->read(addr);
}

void 
ReSID::poke(uint16_t addr, uint8_t value)
{
    reSID->write(addr, value);
}

bool 
ReSID::execute(int elapsedCycles)
{
    short buf[2049];
    int buflength = 2048;
    int delta_t = elapsedCycles;
    int bufindex = 0;
    
    // TODO: SPEEDUP: Write directly into ringbuffer
    while (delta_t) {
        bufindex += reSID->clock(delta_t, buf + bufindex, buflength - bufindex);

        // write to ringbuffer
        for (int i = 0; i < bufindex; i++) {
            float sample = (float)buf[i] * 0.000005f; 
            writeData(sample);
            // if (sample != 0) fprintf(stderr,"Sample %d\n", buf[i]);
        }
        // fprintf(stderr,"wrote %d samples\n", bufindex);
        bufindex = 0;
    }
        
    return true;
}

void 
ReSID::handleBufferException()
{    
    // Reset pointers
    readBuffer = writeBuffer = ringBuffer;

    // Add some delay
    size_t delay = 8*735;
    memset(ringBuffer, 0, delay); 
    writeBuffer += delay;
}

float 
ReSID::readData()
{	
	float value;
    
    if (readBuffer == writeBuffer) {
        fprintf(stderr, "SID RINGBUFFER UNDERFLOW (%d)\n", readBuffer - ringBuffer);
        handleBufferException();
    }

    value = *readBuffer;
    
    if (readBuffer == endBuffer)
        readBuffer = ringBuffer;
    else 
        readBuffer++;

	return value;
}
	
void 
ReSID::writeData(float data)
{
    if (readBuffer == writeBuffer) {
        fprintf(stderr, "SID RINGBUFFER OVERFLOW (%d)\n", writeBuffer - ringBuffer);
        handleBufferException();
    }

    *writeBuffer = data;
    
    if (writeBuffer == endBuffer)
        writeBuffer = ringBuffer;
    else 
        writeBuffer++;
}
	
void 
ReSID::dumpState()
{
	msg("SID\n");
	msg("---\n\n");
	msg("   Sample rate : %d\n", samplerate);
	msg(" CPU frequency : %d\n", cpuFrequency);
	msg("   Buffer size : %d\n", bufferSize);
	msg("  Sound filter : %s\n", filtersEnabled ? "on" : "off");
	msg("\n");
}


