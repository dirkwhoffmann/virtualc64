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
#include "CPUInspector.h"
#include "CPU.h"
#include "IOUtils.h"

namespace vc64 {

void
CPUInspector::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Registers) {

        os << tab("Instruction Address") << hex(cpu.reg.pc0) << std::endl;
        os << tab("Program Counter") << hex(cpu.reg.pc) << std::endl;
        os << tab("Accumulator") << hex(cpu.reg.a) << std::endl;
        os << tab("X Register") << hex(cpu.reg.x) << std::endl;
        os << tab("Y Register") << hex(cpu.reg.y) << std::endl;
        os << tab("Stack Pointer") << hex(cpu.reg.sp) << std::endl;
        os << tab("Flags");
        os << (cpu.reg.sr.n ? "N" : "n");
        os << (cpu.reg.sr.v ? "V" : "v");
        os << (cpu.reg.sr.b ? "B" : "b");
        os << (cpu.reg.sr.d ? "D" : "d");
        os << (cpu.reg.sr.i ? "I" : "i");
        os << (cpu.reg.sr.z ? "Z" : "z");
        os << (cpu.reg.sr.c ? "C" : "c");
        os << std::endl;
    }

    if (category == Category::State) {

        auto append = [&](const string &s1, const string &s2) {
            return s1.empty() ? s2 : s1 + ", " + s2;
        };

        string str = "";
        if (cpu.flags & CPU_LOG_INSTRUCTION) str = append(str, "LOG_INSTRUCTION");
        if (cpu.flags & CPU_CHECK_BP) str = append(str, "CHECK_BP");
        if (cpu.flags & CPU_CHECK_WP) str = append(str, "CHECK_WP");

        os << tab("Clock");
        os << dec(cpu.clock) << std::endl;
        os << tab("Flags");
        os << (str.empty() ? "-" : str) << std::endl;
        os << tab("Next microinstruction");
        os << dec(cpu.next) << std::endl;
        os << tab("Rdy Line");
        os << bol(cpu.rdyLine, "high", "low") << std::endl;
        os << tab("Nmi Line");
        os << hex(cpu.nmiLine) << std::endl;
        os << tab("Edge detector");
        os << hex(cpu.edgeDetector.current()) << std::endl;
        os << tab("doNmi");
        os << bol(cpu.doNmi) << std::endl;
        os << tab("Irq Line");
        os << hex(cpu.irqLine) << std::endl;
        os << tab("Edge Detector");
        os << hex(cpu.levelDetector.current()) << std::endl;
        os << tab("doIrq");
        os << bol(cpu.doIrq) << std::endl;
        os << tab("IRQ Routine");
        os << hex(HI_W_LO_W(cpu.readDasm(0xFFFF), cpu.readDasm(0xFFFE))) << std::endl;
        os << tab("NMI Routine");
        os << hex(HI_W_LO_W(cpu.readDasm(0xFFFB), cpu.readDasm(0xFFFA))) << std::endl;

        if (cpu.hasProcessorPort()) {

            os << tab("Processor port");
            os << hex(cpu.reg.pport.data) << std::endl;
            os << tab("Direction bits");
            os << hex(cpu.reg.pport.direction) << std::endl;
            os << tab("Bit 3 discharge cycle");
            os << dec(cpu.dischargeCycleBit3) << std::endl;
            os << tab("Bit 6 discharge cycle");
            os << dec(cpu.dischargeCycleBit6) << std::endl;
            os << tab("Bit 7 discharge cycle");
            os << dec(cpu.dischargeCycleBit7) << std::endl;
        }
    }
}

}
