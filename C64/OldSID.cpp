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
#include "fastsid/FastSid.h"

OldSID::OldSID()
{
	setDescription("SID");
	debug(3, "  Creating FastSID at address %p...\n", this);
    
    // Initialize wave and noise tables
    Voice::initWaveTables();
    
    fastsid_init(&st, 44100, PAL_CYCLES_PER_SECOND, 1000);
    
    
    // Set default values
    // setChipModel(reSID::MOS6581);
    st.newsid = false; 
    
    /*
    cpuFrequency = PAL_CYCLES_PER_FRAME * PAL_REFRESH_RATE;
    samplingMethod = reSID::SAMPLE_FAST;
    sampleRate = 44100;
     */
    // sid->set_sampling_parameters(cpuFrequency, samplingMethod, sampleRate);
    setAudioFilter(false);
    
    volume = 100000;
    targetVolume = 100000;
}

OldSID::~OldSID()
{

}

//! Bring the SID chip back to it's initial state.
void
OldSID::reset()
{
    
}

//! Load state
void
OldSID::loadFromBuffer(uint8_t **buffer)
{
    
}

//! Save state
void
OldSID::saveToBuffer(uint8_t **buffer)
{
    
}

//! Dump internal state to console
void
OldSID::dumpState()
{
    
}

//! Special peek function for the I/O memory range.
uint8_t
OldSID::peek(uint16_t addr)
{
    return fastsid_read(&st, addr);
}

//! Special poke function for the I/O memory range.
void
OldSID::poke(uint16_t addr, uint8_t value)
{
    fastsid_store(&st, addr, value);
}

/*! @brief   Execute SID
 *  @details Runs reSID for the specified amount of CPU cycles and writes
 *           the generated sound samples into the internal ring buffer.
 */
void
OldSID::execute(uint64_t cycles)
{
    int16_t buf[2049];
    int buflength = 2048;
    
    // debug("executing for %d cycles\n", cycles);
    
    // How many samples do we need?
    // For now, we assume a sample rate of 44100
    int numSamples = (int)(cycles * 44100 / PAL_CYCLES_PER_SECOND);
    if (numSamples > buflength) {
        debug("Number of samples exceeds buffer size\n");
        numSamples = buflength;
    }
    // Let FastSID compute
    st.factor = 1000; // ??? CORRECT?
    int computed = fastsid_calculate_samples(&st, buf, numSamples, 1, NULL);
    assert(computed == numSamples);
    
    // Write samples into ringbuffer
    if (computed) {
        writeData(buf, computed);
    }
}


