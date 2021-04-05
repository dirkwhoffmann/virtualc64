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

        cpuInfo = c64.cpu.getInfo()
    }

    func refreshCPU(count: Int = 0, full: Bool = false) {
        
        cacheCPU()
        
        if full {
            cpuPc.assignFormatter(fmt16)
            cpuSp.assignFormatter(fmt8)
            cpuA.assignFormatter(fmt8)
            cpuX.assignFormatter(fmt8)
            cpuY.assignFormatter(fmt8)
            cpuPortReg.assignFormatter(fmt8)
            cpuPortDir.assignFormatter(fmt8)
        }

        cpuPc.integerValue = Int(cpuInfo.reg.pc0)
        cpuSp.integerValue = Int(cpuInfo.reg.sp)
        cpuA.integerValue = Int(cpuInfo.reg.a)
        cpuX.integerValue = Int(cpuInfo.reg.x)
        cpuY.integerValue = Int(cpuInfo.reg.y)
        
        cpuN.state = cpuInfo.reg.sr.n ? .on : .off
        cpuV.state = cpuInfo.reg.sr.v ? .on : .off
        cpuB.state = cpuInfo.reg.sr.b ? .on : .off
        cpuD.state = cpuInfo.reg.sr.d ? .on : .off
        cpuI.state = cpuInfo.reg.sr.i ? .on : .off
        cpuZ.state = cpuInfo.reg.sr.z ? .on : .off
        cpuC.state = cpuInfo.reg.sr.c ? .on : .off

        cpuIrq.state = (cpuInfo.irq != 0) ? .on : .off
        cpuIrqCIA.state = (cpuInfo.irq & IntSource.CIA.rawValue) != 0 ? .on : .off
        cpuIrqVIC.state = (cpuInfo.irq & IntSource.VIC.rawValue) != 0 ? .on : .off
        cpuIrqEXP.state = (cpuInfo.irq & IntSource.EXP.rawValue) != 0 ? .on : .off
        cpuNmi.state = (cpuInfo.nmi != 0) ? .on : .off
        cpuNmiCIA.state = (cpuInfo.irq & IntSource.CIA.rawValue) != 0 ? .on : .off
        cpuNmiKBD.state = (cpuInfo.irq & IntSource.KBD.rawValue) != 0 ? .on : .off
        cpuNmiEXP.state = (cpuInfo.irq & IntSource.EXP.rawValue) != 0 ? .on : .off
        cpuRdy.state = cpuInfo.rdy ? .on : .off
        
        cpuPortReg.integerValue = Int(cpuInfo.processorPort)
        cpuPortDir.integerValue = Int(cpuInfo.processorPortDir)

        cpuInstrView.refresh(count: count, full: full, addr: Int(cpuInfo.reg.pc0))
        cpuTraceView.refresh(count: count, full: full)
        cpuBreakView.refresh(count: count, full: full)
        cpuWatchView.refresh(count: count, full: full)
    }
    
    @IBAction func cpuClearTraceBufferAction(_ sender: NSButton!) {

        c64.cpu.clearLog()
        refreshCPU(full: true)
    }
    
    @IBAction func cpuGotoAction(_ sender: NSSearchField!) {

        if sender.stringValue == "" {
            cpuInstrView.jumpTo(addr: Int(cpuInfo.reg.pc0))
        } else if let addr = Int(sender.stringValue, radix: 16) {
            cpuInstrView.jumpTo(addr: addr)
        } else {
            sender.stringValue = ""
        }
    }
}
