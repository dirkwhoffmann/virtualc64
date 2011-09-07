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

SIDWrapper::SIDWrapper()
{
	name = "SIDWrapper";
    
	// debug(2, "  Creating SIDWrapper at address %p...\n", this);
    
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
        debug("Using ReSID library\n");
    else
        debug("Using old SID implementation\n");
    
    useReSID = enable;
}

void 
SIDWrapper::reset()
{
    oldsid->reset();
    resid->reset();
}

void 
SIDWrapper::loadFromBuffer(uint8_t **buffer)
{
    oldsid->loadFromBuffer(buffer);
    // resid->loadFromBuffer(buffer);
}

void 
SIDWrapper::saveToBuffer(uint8_t **buffer)
{
    oldsid->saveToBuffer(buffer);
    // resid->saveToBuffer(buffer);
}

void 
SIDWrapper::dumpState()
{
    if (useReSID)
        resid->dumpState();
    else
        oldsid->dumpState();
}

uint8_t 
SIDWrapper::peek(uint16_t addr)
{
    if (useReSID)
        return resid->peek(addr);
    else
        return oldsid->peek(addr);
}

void 
SIDWrapper::poke(uint16_t addr, uint8_t value)
{
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
        debug("Enabling audio filters\n");
    else
        debug("Disabling audio filters\n");

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
