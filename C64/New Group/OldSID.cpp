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
#include "FastSid.h"

OldSID::OldSID()
{
	setDescription("SID");
	debug(3, "  Creating FastSID at address %p...\n", this);
    
    // Initialize wave and noise tables
    Voice::initWaveTables();
    
    fastsid_init(&st, 44100, PAL_CYCLES_PER_SECOND);
    
    
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
    
    // Compute samples
    for (unsigned i = 0; i < numSamples; i++) {
        buf[i] = fastsid_calculate_single_sample(i);
    }
    
    // Write samples into ringbuffer
    writeData(buf, numSamples);
}

int16_t
OldSID::fastsid_calculate_single_sample()
{
    uint32_t o0, o1, o2;
    uint32_t osc0, osc1, osc2;
    Voice *v0 = &st.v[0];
    Voice *v1 = &st.v[1];
    Voice *v2 = &st.v[2];
    
    setup_sid(&st);
    v0->prepare();
    v1->prepare();
    v2->prepare();
    
    // addfptrs, noise
    if ((v0->vt.f += v0->vt.fs) < v0->vt.fs) {
        v0->vt.rv = NSHIFT(v0->vt.rv, 16);
    }
    if ((v1->vt.f += v1->vt.fs) < v1->vt.fs) {
        v1->vt.rv = NSHIFT(v1->vt.rv, 16);
    }
    if ((v2->vt.f += v2->vt.fs) < v2->vt.fs) {
        v2->vt.rv = NSHIFT(v2->vt.rv, 16);
    }
    
    // Hard sync
    if (v0->hardSync()) {
        v0->vt.rv = NSHIFT(v0->vt.rv, v0->vt.f >> 28);
        v0->vt.f = 0;
    }
    if (v2->hardSync()) {
        v2->vt.rv = NSHIFT(v2->vt.rv, v2->vt.f >> 28);
        v2->vt.f = 0;
    }
    if (v1->hardSync()) {
        v1->vt.rv = NSHIFT(v1->vt.rv, v1->vt.f >> 28);
        v1->vt.f = 0;
    }
    
    // Do adsr
    if ((v0->vt.adsr += v0->vt.adsrs) + 0x80000000 < v0->vt.adsrz + 0x80000000) {
        v0->trigger_adsr();
    }
    if ((v1->vt.adsr += v1->vt.adsrs) + 0x80000000 < v1->vt.adsrz + 0x80000000) {
        v1->trigger_adsr();
    }
    if ((v2->vt.adsr += v2->vt.adsrs) + 0x80000000 < v2->vt.adsrz + 0x80000000) {
        v2->trigger_adsr();
    }
    
    // Oscillators
    o0 = v0->vt.adsr >> 16;
    o1 = v1->vt.adsr >> 16;
    o2 = v2->vt.adsr >> 16;
    osc0 = (v0->vt.adsr >> 16) * v0->doosc();
    osc1 = (v1->vt.adsr >> 16) * v1->doosc();
    osc2 = st.has3 ? ((v2->vt.adsr >> 16) * v2->doosc()) : 0;
    
    // Sample
    if (st.emulatefilter) {
        v0->vt.filtIO = ampMod1x8[(o0 >> 22)];
        v0->applyFilter();
        o0 = ((uint32_t)(v0->vt.filtIO) + 0x80) << (7 + 15);
        
        v1->vt.filtIO = ampMod1x8[(o1 >> 22)];
        v1->applyFilter();
        o1 = ((uint32_t)(v1->vt.filtIO) + 0x80) << (7 + 15);
        
        v2->vt.filtIO = ampMod1x8[(o2 >> 22)];
        v2->applyFilter();
        o2 = ((uint32_t)(v2->vt.filtIO) + 0x80) << (7 + 15);
    }
    
    return (int16_t)(((int32_t)((o0 + o1 + o2) >> 20) - 0x600) * st.vol);
}
