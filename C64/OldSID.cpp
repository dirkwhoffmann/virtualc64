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

OldSID::OldSID()
{
	setDescription("SID");
	debug(3, "  Creating FastSID at address %p...\n", this);
    

}

OldSID::~OldSID()
{

}

void
OldSID::reset()
{
    VirtualComponent::reset();
}

void
OldSID::setAudioFilter(bool enable)
{
    
}

float
OldSID::getVolumeControl()
{
    return 1.0;
}

void
OldSID::setVolumeControl(float value)
{
    
}

void
OldSID::setSampleRate(uint32_t sr)
{
    
}

uint32_t
OldSID::getSampleRate()
{
    return 42;
}

void 
OldSID::setClockFrequency(uint32_t frequency)
{

}

size_t
OldSID::stateSize()
{
    return 0;
}

void 
OldSID::loadFromBuffer(uint8_t **buffer)
{
}

void
OldSID::saveToBuffer(uint8_t **buffer)
{
}

uint8_t 
OldSID::peek(uint16_t addr)
{	
    return 0;
}

void 
OldSID::poke(uint16_t addr, uint8_t value)
{
}

void
OldSID::execute(uint64_t elapsedCycles)
{
}

void
OldSID::run()
{
}

void
OldSID::halt()
{
}

void
OldSID::readMonoSamples(float *target, size_t n)
{
}

void
OldSID::readStereoSamples(float *target1, float *target2, size_t n)
{
}

void
OldSID::readStereoSamplesInterleaved(float *target, size_t n)
{
}

/*
void OldSID::writeData(float data)
{
}
*/

void OldSID::dumpState()
{
}


