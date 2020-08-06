// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
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

        cpuPc.integerValue = Int(cpuInfo.pc)
        cpuSp.integerValue = Int(cpuInfo.sp)
        cpuA.integerValue = Int(cpuInfo.a)
        cpuX.integerValue = Int(cpuInfo.x)
        cpuY.integerValue = Int(cpuInfo.y)
        
        cpuN.state = cpuInfo.nFlag ? .on : .off
        cpuV.state = cpuInfo.vFlag ? .on : .off
        cpuB.state = cpuInfo.bFlag ? .on : .off
        cpuD.state = cpuInfo.dFlag ? .on : .off
        cpuI.state = cpuInfo.iFlag ? .on : .off
        cpuZ.state = cpuInfo.zFlag ? .on : .off
        cpuC.state = cpuInfo.cFlag ? .on : .off

        cpuIrq.state = cpuInfo.irq ? .on : .off
        cpuNmi.state = cpuInfo.nmi ? .on : .off
        cpuRdy.state = cpuInfo.rdy ? .on : .off
        
        cpuPortReg.integerValue = Int(cpuInfo.processorPort)
        cpuPortDir.integerValue = Int(cpuInfo.processorPortDir)

        cpuInstrView.refresh(count: count, full: full, addr: Int(cpuInfo.pc))
        cpuTraceView.refresh(count: count, full: full)
        cpuBreakView.refresh(count: count, full: full)
        cpuWatchView.refresh(count: count, full: full)
    }
}
