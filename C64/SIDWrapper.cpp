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

#include "C64.h"

SIDWrapper::SIDWrapper()
{
	setDescription("SIDWrapper");
    
    oldsid = new OldSID();
    resid = new ReSID();
    
    // Register sub components
    VirtualComponent *subcomponents[] = { oldsid, resid, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));

    // Register snapshot items
    SnapshotItem items[] = {
        
        // Configuration items
        { &useReSID,        sizeof(useReSID),       KEEP_ON_RESET },
        // Internal state
        { &latchedDataBus,  sizeof(latchedDataBus), CLEAR_ON_RESET },
        { &cycles,          sizeof(cycles),         CLEAR_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
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
    debug(2, "SIDWrapper::setPAL\n");
    setClockFrequency(PAL_CYCLES_PER_FRAME * PAL_REFRESH_RATE);
}

void
SIDWrapper::setNTSC()
{
    debug(2, "SIDWrapper::setNTSC\n");
    setClockFrequency(NTSC_CYCLES_PER_FRAME * NTSC_REFRESH_RATE);
}

uint8_t 
SIDWrapper::peek(uint16_t addr)
{
    // Get SID up to date
    executeUntil(c64->getCycles());
    
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
    // Get SID up to date
    executeUntil(c64->getCycles());

    latchedDataBus = value;
    oldsid->poke(addr, value);
    resid->poke(addr, value);
}

void
SIDWrapper::executeUntil(uint64_t targetCycle)
{
    execute(targetCycle - cycles);
    cycles = targetCycle;
}

inline void
SIDWrapper::execute(uint64_t numCycles)
{
    // printf("Execute SID for %lld cycles", numCycles);
    if (numCycles == 0)
        return;
    
    if (useReSID)
        resid->execute(numCycles);
    else
        oldsid->execute(numCycles);
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
    // resid->setAudioFilter(enable);
    resid->setExternalAudioFilter(enable); 
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

uint32_t
SIDWrapper::getClockFrequency()
{
    return resid->getClockFrequency();
}

void 
SIDWrapper::setClockFrequency(uint32_t frequency)
{
    oldsid->setClockFrequency(frequency);
    resid->setClockFrequency(frequency);
}
