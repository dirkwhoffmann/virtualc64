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

SIDBridge::SIDBridge()
{
	setDescription("SIDWrapper");
    
    fastsid = new FastSID();
    resid = new ReSID();
    
    // Register sub components
    VirtualComponent *subcomponents[] = { fastsid, resid, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));

    // Register snapshot items
    SnapshotItem items[] = {
        
        // Configuration items
        { &useReSID,        sizeof(useReSID),       KEEP_ON_RESET },
        { &chipModel,       sizeof(chipModel),      KEEP_ON_RESET },

        // Internal state
        { &latchedDataBus,  sizeof(latchedDataBus), CLEAR_ON_RESET },
        { &cycles,          sizeof(cycles),         CLEAR_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
    useReSID = true;
}

SIDBridge::~SIDBridge()
{
    delete fastsid;
    delete resid;
}

void 
SIDBridge::setReSID(bool enable)
{
    if (enable)
        debug(2, "Using ReSID\n");
    else
        debug(2, "Using FastSID\n");
    
    useReSID = enable;
}

void 
SIDBridge::dumpState()
{
    if (useReSID) {
        resid->dumpState();
    } else {
        fastsid->dumpState();
    }
}

void
SIDBridge::setPAL()
{
    debug(2, "SIDWrapper::setPAL\n");
    setClockFrequency(PAL_CYCLES_PER_FRAME * PAL_REFRESH_RATE);
}

void
SIDBridge::setNTSC()
{
    debug(2, "SIDWrapper::setNTSC\n");
    setClockFrequency(NTSC_CYCLES_PER_FRAME * NTSC_REFRESH_RATE);
}

uint8_t 
SIDBridge::peek(uint16_t addr)
{
    assert(addr <= 0x1F);
    
    // Get SID up to date
    executeUntil(c64->getCycles());
    
    if (useReSID) {
        return resid->peek(addr);
    } else {
        return fastsid->peek(addr);
    }
}

uint8_t
SIDBridge::spy(uint16_t addr)
{
    assert(addr <= 0x001F);

    return peek(addr);
}

void 
SIDBridge::poke(uint16_t addr, uint8_t value)
{
    // Get SID up to date
    executeUntil(c64->getCycles());

    // Keep both SID implementations up to date all the time
    fastsid->poke(addr, value);
    resid->poke(addr, value);
}

void
SIDBridge::executeUntil(uint64_t targetCycle)
{
    execute(targetCycle - cycles);
    cycles = targetCycle;
}

inline void
SIDBridge::execute(uint64_t numCycles)
{
    // printf("Execute SID for %lld cycles", numCycles);
    if (numCycles == 0)
        return;
    
    if (useReSID) {
        resid->execute(numCycles);
    } else {
        fastsid->execute(numCycles);
    }
}

void 
SIDBridge::run()
{   
    fastsid->run();
    resid->run();
}

void 
SIDBridge::halt()
{   
    fastsid->halt();
    resid->halt();
}

void
SIDBridge::readMonoSamples(float *target, size_t n)
{
    if (useReSID)
        resid->readMonoSamples(target, n);
    else
        fastsid->readMonoSamples(target, n);
}

void
SIDBridge::readStereoSamples(float *target1, float *target2, size_t n)
{
    if (useReSID)
        resid->readStereoSamples(target1, target2, n);
    else
        fastsid->readStereoSamples(target1, target2, n);
}

void
SIDBridge::readStereoSamplesInterleaved(float *target, size_t n)
{
    if (useReSID)
        resid->readStereoSamplesInterleaved(target, n);
    else
        fastsid->readStereoSamplesInterleaved(target, n);
}

bool
SIDBridge::getAudioFilter()
{
    if (useReSID) {
        return resid->getAudioFilter();
    } else {
        return fastsid->getAudioFilter();
    }
}

void 
SIDBridge::setAudioFilter(bool value)
{
    resid->setAudioFilter(value);
    fastsid->setAudioFilter(value);
}

SamplingMethod
SIDBridge::getSamplingMethod()
{
    // This option is ReSID only
    return resid->getSamplingMethod();
}

void
SIDBridge::setSamplingMethod(SamplingMethod value)
{
    // This option is ReSID only
    resid->setSamplingMethod(value);
}

SIDChipModel
SIDBridge::getChipModel()
{
    if (useReSID) {
        return resid->getChipModel();
    } else {
        return fastsid->getChipModel();
    }
}

void 
SIDBridge::setChipModel(SIDChipModel model)
{
    if (model != MOS_6581 && model != MOS_8580) {
        warn("Unknown chip model (%d). Using  MOS8580\n", model);
        model = MOS_8580;
    }
    
    resid->setChipModel(model);
    fastsid->setChipModel(model);
}

uint32_t
SIDBridge::getSampleRate()
{
    if (useReSID) {
        return resid->getSampleRate();
    } else {
        return fastsid->getSampleRate();
    }
}

void 
SIDBridge::setSampleRate(uint32_t rate)
{
    resid->setSampleRate(rate);
    fastsid->setSampleRate(rate);
}

uint32_t
SIDBridge::getClockFrequency()
{
    if (useReSID) {
        return resid->getClockFrequency();
    } else {
        return fastsid->getClockFrequency();
    }
}

void 
SIDBridge::setClockFrequency(uint32_t frequency)
{
    resid->setClockFrequency(frequency);
    fastsid->setClockFrequency(frequency);
}
