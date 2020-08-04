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
            let elements = [ cpuPc: fmt16,
                             cpuSp: fmt8,
                             cpuA: fmt8,
                             cpuX: fmt8,
                             cpuY: fmt8,
                             cpuPortReg: fmt8,
                             cpuPortDir: fmt8 ]
            
            for (c, f) in elements { assignFormatter(f, c!) }
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
        // cpuBreakView.refresh(count: count, full: full)
    }
}
