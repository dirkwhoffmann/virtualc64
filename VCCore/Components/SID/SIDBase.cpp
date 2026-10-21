// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "vcconfig.h"
#include "SID.h"
#include "Emulator.h"

namespace vc64 {

void
SID::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("Chip");
        os << dec(objid) << std::endl;
        os << tab("Clock");
        os << dec(clock) << std::endl;

        resid.dump(category, os);
    }
}

SIDInfo
SID::cacheInfo() const
{
    SIDInfo result = { };

    {   SYNCHRONIZED

        reSID::SID::State state = resid.sid->read_state();
        u8 *reg = (u8 *)state.sid_register;

        result.volume = reg[0x18] & 0xF;
        result.filterModeBits = reg[0x18] & 0xF0;
        result.filterType = reg[0x18] & 0x70;
        result.filterCutoff = u16(reg[0x16] << 3 | (reg[0x15] & 0x07));
        result.filterResonance = reg[0x17] >> 4;
        result.filterEnableBits = reg[0x17] & 0x0F;

        result.potX = port1.mouse.readPotX() & port2.mouse.readPotX();
        result.potY = port1.mouse.readPotY() & port2.mouse.readPotY();
        
        for (isize i = 0; i < 3; i++, reg += 7) {

            for (isize j = 0; j < 7; j++) result.voice[i].reg[j] = reg[j];
            result.voice[i].frequency = HI_LO(reg[0x1], reg[0x0]);
            result.voice[i].pulseWidth = u16((reg[0x3] & 0xF) << 8 | reg[0x02]);
            result.voice[i].waveform = reg[0x4] & 0xF0;
            result.voice[i].ringMod = (reg[0x4] & 0x4) != 0;
            result.voice[i].hardSync = (reg[0x4] & 0x2) != 0;
            result.voice[i].gateBit = (reg[0x4] & 0x1) != 0;
            result.voice[i].testBit = (reg[0x4] & 0x8) != 0;
            result.voice[i].attackRate = reg[0x5] >> 4;
            result.voice[i].decayRate = reg[0x5] & 0xF;
            result.voice[i].sustainRate = reg[0x6] >> 4;
            result.voice[i].releaseRate = reg[0x6] & 0xF;
        }
    }

    return result;
}

i64
SID::getOption(Opt option) const
{

    switch (option) {

        case Opt::SID_ENABLE:        return (i64)config.enabled;
        case Opt::SID_ADDRESS:       return (i64)config.address;
        case Opt::SID_REV:           return (i64)config.revision;
        case Opt::SID_FILTER:        return (i64)config.filter;
        case Opt::SID_ENGINE:        return (i64)config.engine;
        case Opt::SID_SAMPLING:      return (i64)config.sampling;
        case Opt::SID_POWER_SAVE:    return (i64)config.powerSave;

        default:
            fatalError;
    }
}

i64
SID::getFallback(Opt opt) const
{
    return emulator.defaults.getFallback(opt, objid);
}

void
SID::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::SID_ENABLE:

            if (objid == 0 && value == false) {
                throw CoreError(CoreError::OPT_INV_ARG, "SID 0 can't be disabled");
            }
            return;

        case Opt::SID_ADDRESS:

            if (objid == 0 && value != 0xD400) {
                throw CoreError(CoreError::OPT_INV_ARG, "SID 0 can't be remapped");
            }
            if (value < 0xD400 || value > 0xD7E0 || (value & 0x1F)) {
                throw CoreError(CoreError::OPT_INV_ARG, "D400, D420 ... D7E0");
            }
            return;

        case Opt::SID_REV:

            if (!SIDRevisionEnum::isValid(value)) {
                throw CoreError(CoreError::OPT_INV_ARG, SIDRevisionEnum::keyList());
            }
            return;

        case Opt::SID_FILTER:

            return;

        case Opt::SID_ENGINE:

            if (!SIDEngineEnum::isValid(value)) {
                throw CoreError(CoreError::OPT_INV_ARG, SIDEngineEnum::keyList());
            }
            return;

        case Opt::SID_SAMPLING:

            if (!SamplingMethodEnum::isValid(value)) {
                throw CoreError(CoreError::OPT_INV_ARG, SamplingMethodEnum::keyList());
            }
            return;

        case Opt::SID_POWER_SAVE:

            return;

        default:
            throw CoreError(CoreError::OPT_UNSUPPORTED);
    }
}

void
SID::setOption(Opt opt, i64 value)
{    
    checkOption(opt, value);

    switch (opt) {
            
        case Opt::SID_ENABLE:
            
            if (config.enabled != bool(value)) {
                
                config.enabled = bool(value);
                stream.clear(0);
                c64.sidBridge.hardReset();
            }
            return;
            
        case Opt::SID_ADDRESS:
            
            if (config.address != value) {
                
                config.address = (u16)value;
                stream.clear(0);
            }
            return;
            
        case Opt::SID_REV:
            
        if (config.revision != SIDRevision(value)) {

                config.revision = SIDRevision(value);
                setRevision(SIDRevision(value));
            }
            return;

        case Opt::SID_FILTER:

            if (config.filter != bool(value)) {

                config.filter = bool(value);
                setAudioFilter(bool(value));
            }
            return;

        case Opt::SID_ENGINE:

            if (config.engine != SIDEngine(value)) {

                config.engine = SIDEngine(value);
            }
            return;

        case Opt::SID_SAMPLING:

            if (config.sampling != SamplingMethod(value)) {

                config.sampling = SamplingMethod(value);
                setSamplingMethod(SamplingMethod(value));
            }
            return;

        case Opt::SID_POWER_SAVE:

            config.powerSave = bool(value);
            return;

        default:
            fatalError;
    }
}

}
