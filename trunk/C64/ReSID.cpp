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

    sid = new SID();
    
    // Default chip model
    chipModel = MOS8580;
    sid->set_chip_model(chipModel);
        
    // Default audio filter settings
    audioFilter = false;
    sid->enable_filter(audioFilter);
    sid->enable_external_filter(false);
    	
    // Default sample parameters
	sampleRate = 44100;
    cpuFrequency = CPU::CLOCK_FREQUENCY_PAL; 
    samplingMethod = SAMPLE_FAST;
    sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate);
					
	// init ringbuffer
	bufferSize = 12288;
	ringBuffer = new float[bufferSize];
	endBuffer = &ringBuffer[(bufferSize - 1)];
}

ReSID::~ReSID()
{
    delete sid;
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
ReSID::setAudioFilter(bool enable)
{
    audioFilter = enable;
    sid->enable_filter(enable);
}

void 
ReSID::setSampleRate(uint32_t sr) 
{
	sampleRate = sr;
    sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate);
}

void 
ReSID::setSamplingMethod(sampling_method method)
{
    switch (method) {
        case SAMPLE_FAST:
            debug("Using sample method SAMPLE_FAST\n");
            break;
        case SAMPLE_INTERPOLATE:
            debug("Using sample method SAMPLE_INTERPOLATE\n");
            break;
        case SAMPLE_RESAMPLE_INTERPOLATE:
            debug("Using sample method SAMPLE_RESAMPLE_INTERPOLATE\n");
            break;
        case SAMPLE_RESAMPLE_FAST:
            debug("Using sample method SAMPLE_RESAMPLE_FAST\n");
            break;
        default:
            warn("Unknown sample method. Using SAMPLE_FAST\n");
            method = SAMPLE_FAST;
    }
    
    samplingMethod = method;
    sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate); 
}

void 
ReSID::setChipModel(chip_model model)
{
    switch (model) {
        case MOS6581:
            debug("Plugging in MOS6581\n");
            break;
        case MOS8580:
            debug("Plugging in MOS8580\n");
            break;
        default:
            warn("Unknown chip model. Using  MOS8580\n");
            model = MOS8580;
    }
    
    chipModel = model;
    sid->set_chip_model(model);
}

void 
ReSID::setClockFrequency(uint32_t frequency)
{ 
	cpuFrequency = frequency;
    sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate);
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
    return sid->read(addr);
}

void 
ReSID::poke(uint16_t addr, uint8_t value)
{
    sid->write(addr, value);
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
        bufindex += sid->clock(delta_t, buf + bufindex, buflength - bufindex);

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
        // fprintf(stderr, "SID RINGBUFFER OVERFLOW (%d)\n", writeBuffer - ringBuffer);
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
	msg("   Sample rate : %d\n", sampleRate);
	msg(" CPU frequency : %d\n", cpuFrequency);
	msg("   Buffer size : %d\n", bufferSize);
	msg("\n");
}


