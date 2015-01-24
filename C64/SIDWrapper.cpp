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

#include "SIDWrapper.h"
#include "CPU.h"

SIDWrapper::SIDWrapper(C64 *c64)
{
	name = "SIDWrapper";
    
    this->c64 = c64;
    oldsid = new OldSID();
    resid = new ReSID();
    
    useReSID = true;
}

SIDWrapper::~SIDWrapper()
{
    delete oldsid;
    delete resid;
}

void 
SIDWrapper::setReSID(bool enable)
{
    if (enable)
        debug(2, "Using ReSID library\n");
    else
        debug(2, "Using old SID implementation\n");
    
    useReSID = enable;
}

void 
SIDWrapper::reset()
{
    oldsid->reset();
    resid->reset();
}

uint32_t
SIDWrapper::stateSize()
{
    return 1 + oldsid->stateSize();
}

void
SIDWrapper::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;

    latchedDataBus = read8(buffer);
    oldsid->loadFromBuffer(buffer);
    
    debug(2, "  SID state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void 
SIDWrapper::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;

    write8(buffer, latchedDataBus);
    oldsid->saveToBuffer(buffer);
    
    debug(2, "  SID state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());

}

void 
SIDWrapper::dumpState()
{
    if (useReSID)
        resid->dumpState();
    else
        oldsid->dumpState();
}

void
SIDWrapper::setPAL()
{
    setClockFrequency(CPU::CLOCK_FREQUENCY_PAL);
}

void
SIDWrapper::setNTSC()
{
    setClockFrequency(CPU::CLOCK_FREQUENCY_NTSC);    
}

uint8_t 
SIDWrapper::peek(uint16_t addr)
{
    // Take care of possible side effects, but discard value
    if (useReSID)
        (void)resid->peek(addr);
    else
        (void)oldsid->peek(addr);

    if (addr == 0x19 || addr == 0x1A) {
        latchedDataBus = 0;
        return 0xFF;
    }
    
    if (addr == 0x1B || addr == 0x1C) {
        latchedDataBus = 0;
        return rand();
    }
    
    return latchedDataBus;
}

void 
SIDWrapper::poke(uint16_t addr, uint8_t value)
{
    latchedDataBus = value;
    oldsid->poke(addr, value);
    resid->poke(addr, value);
}

bool 
SIDWrapper::execute(int cycles)
{
    if (useReSID)
        return resid->execute(cycles);
    else
        return oldsid->execute(cycles);
}

void 
SIDWrapper::run()
{   
    oldsid->run();
    resid->run();
}

void 
SIDWrapper::halt()
{   
    oldsid->halt();
    resid->halt();
}

float 
SIDWrapper::readData()
{
    if (useReSID)
        return resid->readData();
    else
        return oldsid->readData();
}

void 
SIDWrapper::setAudioFilter(bool enable)
{
    if (enable)
        debug(2, "Enabling audio filters\n");
    else
        debug(2, "Disabling audio filters\n");

    oldsid->setAudioFilter(enable);
    resid->setAudioFilter(enable);
}

void
SIDWrapper::setSamplingMethod(sampling_method value)
{
     resid->setSamplingMethod(value);
}

void 
SIDWrapper::setChipModel(chip_model value)
{
    resid->setChipModel(value);
}

void 
SIDWrapper::setSampleRate(uint32_t sr)
{
    oldsid->setSampleRate(sr);
    resid->setSampleRate(sr);
}

void 
SIDWrapper::setClockFrequency(uint32_t frequency)
{
    oldsid->setClockFrequency(frequency);
    resid->setClockFrequency(frequency);
}
