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
#include "CPU.h"
#include "Emulator.h"

namespace vc64 {

void
CPU::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    auto print = [&](const string &name, const Guards &guards) {

        for (int i = 0; i < guards.elements(); i++) {

            auto guard = guards.guardNr(i);

            os << util::tab(name + " " + std::to_string(i));
            os << util::hex(guard->addr);

            if (!guard->enabled) os << " (Disabled)";
            else if (guard->ignore) os << " (Disabled for " << util::dec(guard->ignore) << " hits)";
            os << std::endl;
        }
    };

    if (category == Category::Registers) {

        os << tab("Instruction Address") << hex(reg.pc0) << std::endl;
        os << tab("Program Counter") << hex(reg.pc) << std::endl;
        os << tab("Accumulator") << hex(reg.a) << std::endl;
        os << tab("X Register") << hex(reg.x) << std::endl;
        os << tab("Y Register") << hex(reg.y) << std::endl;
        os << tab("Stack Pointer") << hex(reg.sp) << std::endl;
        os << tab("Flags");
        os << (reg.sr.n ? "N" : "n");
        os << (reg.sr.v ? "V" : "v");
        os << (reg.sr.b ? "B" : "b");
        os << (reg.sr.d ? "D" : "d");
        os << (reg.sr.i ? "I" : "i");
        os << (reg.sr.z ? "Z" : "z");
        os << (reg.sr.c ? "C" : "c");
        os << std::endl;
    }

    if (category == Category::State) {

        auto append = [&](const string &s1, const string &s2) {
            return s1.empty() ? s2 : s1 + ", " + s2;
        };

        string str = "";
        if (flags & CPU_LOG_INSTRUCTION) str = append(str, "LOG_INSTRUCTION");
        if (flags & CPU_CHECK_BP) str = append(str, "CHECK_BP");
        if (flags & CPU_CHECK_WP) str = append(str, "CHECK_WP");

        os << tab("Clock");
        os << dec(clock) << std::endl;
        os << tab("Flags");
        os << (str.empty() ? "-" : str) << std::endl;
        os << tab("Next microinstruction");
        os << dec(next) << std::endl;
        os << tab("Nmi Line");
        os << (nmiLine ? "low" : "high") << " (" << hex(nmiLine) << ")" << std::endl;
        os << tab("Edge detector");
        os << hex(edgeDetector.current()) << std::endl;
        os << tab("doNmi");
        os << bol(doNmi) << std::endl;
        os << tab("Irq Line");
        os << (irqLine ? "low" : "high") << " (" << hex(irqLine) << ")" << std::endl;
        os << tab("Edge Detector");
        os << hex(levelDetector.current()) << std::endl;
        os << tab("doIrq");
        os << bol(doIrq) << std::endl;
        os << tab("IRQ Routine");
        os << hex(HI_W_LO_W(readDasm(0xFFFF), readDasm(0xFFFE))) << std::endl;
        os << tab("NMI Routine");
        os << hex(HI_W_LO_W(readDasm(0xFFFB), readDasm(0xFFFA))) << std::endl;
        os << tab("Rdy Line");
        os << (rdyLine ? "low" : "high") << " (" << hex(rdyLine) << ")" << std::endl;


        if (hasProcessorPort()) {

            os << tab("Processor port");
            os << hex(reg.pport.data) << std::endl;
            os << tab("Direction bits");
            os << hex(reg.pport.direction) << std::endl;
            os << tab("Bit 3 discharge cycle");
            os << dec(dischargeCycleBit3) << std::endl;
            os << tab("Bit 6 discharge cycle");
            os << dec(dischargeCycleBit6) << std::endl;
            os << tab("Bit 7 discharge cycle");
            os << dec(dischargeCycleBit7) << std::endl;
        }
    }

    if (category == Category::Breakpoints) {

        if (debugger.breakpoints.elements()) {
            print("Breakpoint", debugger.breakpoints);
        } else {
            os << "No breakpoints set" << std::endl;
        }
    }

    if (category == Category::Watchpoints) {

        if (debugger.watchpoints.elements()) {
            print("Watchpoint", debugger.watchpoints);
        } else {
            os << "No watchpoints set" << std::endl;
        }
    }
}

void
CPU::cacheInfo(CPUInfo &result) const
{
    {   SYNCHRONIZED
        
        result.cycle = clock;

        result.pc0 = reg.pc0;
        result.pc = reg.pc;
        result.sp = reg.sp;
        result.a = reg.a;
        result.x = reg.x;
        result.y = reg.y;
        result.sr = getP();

        result.irq = irqLine;
        result.nmi = nmiLine;
        result.rdy = rdyLine;

        result.next = next;
        
        result.processorPort = readPort();
        result.processorPortDir = readPortDir();
    }
}

void
CPU::_didReset(bool hard)
{
    if (!hard) return;

    Peddle::reset();

    // Enable or disable CPU debugging
    c64.emulator.isTracking() ? debugger.enableLogging() : debugger.disableLogging();

    assert(levelDetector.isClear());
    assert(edgeDetector.isClear());
}

void
CPU::_trackOn()
{
    // We only allow the C64 CPU to enter track mode
    if (!isC64CPU()) return;

    debugger.enableLogging();
}

void
CPU::_trackOff()
{
    debugger.disableLogging();
}

}
