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

#include "VirtualC64Config.h"
#include "CIA.h"
#include "CPU.h"
#include "IOUtils.h"

namespace vc64 {

void
CIA::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::Registers) {

        os << std::endl;
        os << tab("Counter A") << hex(counterA) << std::endl;
        os << tab("Latch A") << hex(latchA) << std::endl;
        os << tab("Data register A") << hex(PRA) << std::endl;
        os << tab("Data port direction A") << hex(DDRA) << std::endl;
        os << tab("Data port A") << hex(PA) << std::endl;
        os << tab("Control register A") << hex(CRA) << std::endl;
        os << std::endl;
        os << tab("Counter B") << hex(counterB) << std::endl;
        os << tab("Latch B") << hex(latchB) << std::endl;
        os << tab("Data register B") << hex(PRB) << std::endl;
        os << tab("Data port direction B") << hex(DDRB) << std::endl;
        os << tab("Data port B") << hex(PB) << std::endl;
        os << tab("Control register B") << hex(CRB) << std::endl;
        os << std::endl;
        os << tab("Interrupt control reg") << hex(icr) << std::endl;
        os << tab("Interrupt mask reg") << hex(imr) << std::endl;
        os << std::endl;
        os << tab("SDR") << hex(sdr) << std::endl;
        os << tab("serCounter") << dec(serCounter) << std::endl;
        os << std::endl;
    }

    if (category == Category::State) {

        os << tab("Sleeping") << bol(sleeping) << std::endl;
        os << tab("Tiredness") << dec(tiredness) << std::endl;
        os << tab("Sleep cycle") << dec(sleepCycle) << std::endl;
        os << tab("Wakeup cycle") << dec(wakeUpCycle) << std::endl;
        os << tab("CNT") << dec(CNT) << std::endl;
        os << tab("INT") << dec(INT) << std::endl;
    }

    if (category == Category::Tod) {

        tod.dump(Category::State, os);
    }
}

void
CIA::cacheInfo(CIAInfo &result) const
{
    {   SYNCHRONIZED

        result.portA.port = computePA();
        result.portA.reg = PRA;
        result.portA.dir = DDRA;

        result.portB.port = computePB();
        result.portB.reg = PRB;
        result.portB.dir = DDRB;

        result.timerA.count = LO_HI(spypeek(0x04), spypeek(0x05));
        result.timerA.latch = latchA;
        result.timerA.running = (delay & CIACountA3);
        result.timerA.toggle = CRA & 0x04;
        result.timerA.pbout = CRA & 0x02;
        result.timerA.oneShot = CRA & 0x08;

        result.timerB.count = LO_HI(spypeek(0x06), spypeek(0x07));
        result.timerB.latch = latchB;
        result.timerB.running = (delay & CIACountB3);
        result.timerB.toggle = CRB & 0x04;
        result.timerB.pbout = CRB & 0x02;
        result.timerB.oneShot = CRB & 0x08;

        result.sdr = sdr;
        result.ssr = sdr;  // ssr not yet implemented
        result.icr = icr;
        result.imr = imr;
        result.intLine = INT;

        result.tod = tod.info;
        result.todIntEnable= imr & 0x04;
    }
}
void
CIA::cacheStats(CIAStats &result) const
{
    {   SYNCHRONIZED
        
        auto total = cpu.clock;
        auto idle = idleTotal() + idleSince();
        
        auto totalDiff = total - result.totalCycles;
        auto idleDiff = idle - result.idleCycles;
        
        result.totalCycles = total;
        result.idleCycles = idle;
        result.idleSince = idleSince();

        // debug(true, "totalDiff: %lld idleDiff: %lld\n", totalDiff, idleDiff);
        result.idlePercentage =  totalDiff ? double(idleDiff) / double(totalDiff) : 1.0;
    }
}

i64
CIA::getOption(Opt option) const
{
    switch (option) {

        case Opt::CIA_REVISION:      return (i64)config.revision;
        case Opt::CIA_TIMER_B_BUG:   return (i64)config.timerBBug;
        case Opt::CIA_IDLE_SLEEP:    return (i64)config.idleSleep;


        default:
            fatalError;
    }
}

void
CIA::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::CIA_REVISION:

            if (!CIARevisionEnum::isValid(value)) {
                throw AppError(Fault::OPT_INV_ARG, CIARevisionEnum::keyList());
            }
            return;

        case Opt::CIA_TIMER_B_BUG:
        case Opt::CIA_IDLE_SLEEP:

            return;

        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
CIA::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case Opt::CIA_REVISION:

            config.revision = (CIARev)value;
            return;

        case Opt::CIA_TIMER_B_BUG:

            config.timerBBug = value;
            return;

        case Opt::CIA_IDLE_SLEEP:

            config.idleSleep = value;
            return;

        default:
            fatalError;
    }
}

}
