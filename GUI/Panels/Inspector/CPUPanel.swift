// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Inspector {
    
    private func cacheCPU() {

        if let emu = emu {

            cpuInfo = emu.paused ? emu.cpu.info : emu.cpu.cachedInfo
        }
    }

    func refreshCPU(count: Int = 0, full: Bool = false) {
        
        if let emu = emu {

            cacheCPU()

            if full {

                if hex { emu.cpu.setHex() } else { emu.cpu.setDec() }
                cpuPc.assignFormatter(fmt16)
                cpuSp.assignFormatter(fmt8)
                cpuA.assignFormatter(fmt8)
                cpuX.assignFormatter(fmt8)
                cpuY.assignFormatter(fmt8)
                cpuPortReg.assignFormatter(fmt8)
                cpuPortDir.assignFormatter(fmt8)
            }

            cpuPcWarning.isHidden = cpuInfo.next == 0 || emu.running
            cpuPc.integerValue = Int(cpuInfo.pc)
            cpuSp.integerValue = Int(cpuInfo.sp)
            cpuA.integerValue = Int(cpuInfo.a)
            cpuX.integerValue = Int(cpuInfo.x)
            cpuY.integerValue = Int(cpuInfo.y)

            cpuN.state = (cpuInfo.sr & 0x80) != 0 ? .on : .off
            cpuV.state = (cpuInfo.sr & 0x40) != 0 ? .on : .off
            cpuB.state = (cpuInfo.sr & 0x10) != 0 ? .on : .off
            cpuD.state = (cpuInfo.sr & 0x08) != 0 ? .on : .off
            cpuI.state = (cpuInfo.sr & 0x04) != 0 ? .on : .off
            cpuZ.state = (cpuInfo.sr & 0x02) != 0 ? .on : .off
            cpuC.state = (cpuInfo.sr & 0x01) != 0 ? .on : .off

            cpuIrq.state = (cpuInfo.irq != 0) ? .on : .off
            cpuIrqCIA.state = (cpuInfo.irq & UInt8(INTSRC_CIA)) != 0 ? .on : .off
            cpuIrqVIC.state = (cpuInfo.irq & UInt8(INTSRC_VIC)) != 0 ? .on : .off
            cpuIrqEXP.state = (cpuInfo.irq & UInt8(INTSRC_EXP)) != 0 ? .on : .off
            cpuNmi.state = (cpuInfo.nmi != 0) ? .on : .off
            cpuNmiCIA.state = (cpuInfo.nmi & UInt8(INTSRC_CIA)) != 0 ? .on : .off
            cpuNmiKBD.state = (cpuInfo.nmi & UInt8(INTSRC_KBD)) != 0 ? .on : .off
            cpuNmiEXP.state = (cpuInfo.nmi & UInt8(INTSRC_EXP)) != 0 ? .on : .off
            cpuRdy.state = cpuInfo.rdy ? .on : .off

            cpuPortReg.integerValue = Int(cpuInfo.processorPort)
            cpuPortDir.integerValue = Int(cpuInfo.processorPortDir)

            cpuInstrView.refresh(count: count, full: full, addr: Int(cpuInfo.pc0))
            cpuTraceView.refresh(count: count, full: full)
            cpuBreakView.refresh(count: count, full: full)
            cpuWatchView.refresh(count: count, full: full)
        }
    }
    
    @IBAction func cpuClearTraceBufferAction(_ sender: NSButton!) {

        emu?.cpu.clearLog()
        refreshCPU(full: true)
    }
    
    @IBAction func cpuGotoAction(_ sender: NSSearchField!) {

        if sender.stringValue == "" {
            cpuInstrView.jumpTo(addr: Int(cpuInfo.pc0))
        } else if let addr = Int(sender.stringValue, radix: 16) {
            cpuInstrView.jumpTo(addr: addr)
        } else {
            sender.stringValue = ""
        }
    }
}
