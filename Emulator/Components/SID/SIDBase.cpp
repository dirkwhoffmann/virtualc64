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

#include "config.h"
#include "SID.h"
#include "Emulator.h"

namespace vc64 {

void
SID::_dump(Category category, std::ostream& os) const
{
    using namespace util;

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

void
SID::cacheInfo(SIDInfo &info) const
{
    {   SYNCHRONIZED

        reSID::SID::State state = resid.sid->read_state();
        u8 *reg = (u8 *)state.sid_register;

        info.volume = reg[0x18] & 0xF;
        info.filterModeBits = reg[0x18] & 0xF0;
        info.filterType = reg[0x18] & 0x70;
        info.filterCutoff = u16(reg[0x16] << 3 | (reg[0x15] & 0x07));
        info.filterResonance = reg[0x17] >> 4;
        info.filterEnableBits = reg[0x17] & 0x0F;

        info.potX = port1.mouse.readPotX() & port2.mouse.readPotX();
        info.potY = port1.mouse.readPotY() & port2.mouse.readPotY();
        
        for (isize i = 0; i < 3; i++, reg += 7) {

            for (isize j = 0; j < 7; j++) info.voice[i].reg[j] = reg[j];
            info.voice[i].frequency = HI_LO(reg[0x1], reg[0x0]);
            info.voice[i].pulseWidth = u16((reg[0x3] & 0xF) << 8 | reg[0x02]);
            info.voice[i].waveform = reg[0x4] & 0xF0;
            info.voice[i].ringMod = (reg[0x4] & 0x4) != 0;
            info.voice[i].hardSync = (reg[0x4] & 0x2) != 0;
            info.voice[i].gateBit = (reg[0x4] & 0x1) != 0;
            info.voice[i].testBit = (reg[0x4] & 0x8) != 0;
            info.voice[i].attackRate = reg[0x5] >> 4;
            info.voice[i].decayRate = reg[0x5] & 0xF;
            info.voice[i].sustainRate = reg[0x6] >> 4;
            info.voice[i].releaseRate = reg[0x6] & 0xF;
        }
    }
}

i64
SID::getOption(Option option) const
{

    switch (option) {

        case OPT_SID_ENABLE:        return config.enabled;
        case OPT_SID_ADDRESS:       return config.address;
        case OPT_SID_REVISION:      return config.revision;
        case OPT_SID_FILTER:        return config.filter;
        case OPT_SID_ENGINE:        return config.engine;
        case OPT_SID_SAMPLING:      return config.sampling;
        case OPT_SID_POWER_SAVE:    return config.powerSave;

        default:
            fatalError;
    }
}

i64
SID::getFallback(Option opt) const
{
    return emulator.defaults.getFallback(opt, objid);
}

void
SID::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_SID_ENABLE:

            if (objid == 0 && value == false) {
                throw Error(ERROR_OPT_INV_ARG, "SID 0 can't be disabled");
            }
            return;

        case OPT_SID_ADDRESS:

            if (objid == 0 && value != 0xD400) {
                throw Error(ERROR_OPT_INV_ARG, "SID 0 can't be remapped");
            }
            if (value < 0xD400 || value > 0xD7E0 || (value & 0x1F)) {
                throw Error(ERROR_OPT_INV_ARG, "D400, D420 ... D7E0");
            }
            return;

        case OPT_SID_REVISION:

            if (!SIDRevisionEnum::isValid(value)) {
                throw Error(ERROR_OPT_INV_ARG, SIDRevisionEnum::keyList());
            }
            return;

        case OPT_SID_FILTER:

            return;

        case OPT_SID_ENGINE:

            if (!SIDEngineEnum::isValid(value)) {
                throw Error(ERROR_OPT_INV_ARG, SIDEngineEnum::keyList());
            }
            return;

        case OPT_SID_SAMPLING:

            if (!SamplingMethodEnum::isValid(value)) {
                throw Error(ERROR_OPT_INV_ARG, SamplingMethodEnum::keyList());
            }
            return;

        case OPT_SID_POWER_SAVE:

            return;

        default:
            throw Error(ERROR_OPT_UNSUPPORTED);
    }
}

void
SID::setOption(Option opt, i64 value)
{    
    checkOption(opt, value);

    switch (opt) {

        case OPT_SID_ENABLE:

            if (config.enabled != bool(value)) {

                config.enabled = bool(value);
                stream.clear(0);
                c64.sidBridge.hardReset();
            }
            return;

        case OPT_SID_ADDRESS:

            if (config.address != value) {

                config.address = (u16)value;
                stream.clear(0);
            }
            return;

        case OPT_SID_REVISION:

            if (config.revision != value) {

                config.revision = SIDRevision(value);
                setRevision(SIDRevision(value));
            }
            return;

        case OPT_SID_FILTER:

            if (config.filter != bool(value)) {

                config.filter = bool(value);
                setAudioFilter(bool(value));
            }
            return;

        case OPT_SID_ENGINE:

            if (config.engine != value) {

                config.engine = SIDEngine(value);
            }
            return;

        case OPT_SID_SAMPLING:

            if (config.sampling != value) {

                config.sampling = SamplingMethod(value);
                setSamplingMethod(SamplingMethod(value));
            }
            return;

        case OPT_SID_POWER_SAVE:

            config.powerSave = bool(value);
            return;

        default:
            fatalError;
    }
}

}
