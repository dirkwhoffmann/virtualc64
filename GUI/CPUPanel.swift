// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController {
    
    private var cpuInfo: CPUInfo { return c64.cpu.getInfo() }
        
    // Registers
    
    func _pcAction(_ value: UInt16) {
        
        let oldValue = cpuInfo.pc
        
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._pcAction(oldValue)
            }
            undoManager?.setActionName("Set Program Counter")
            
            c64.cpu.setPC(value)
            refreshCPU()
        }
    }
    
    @IBAction func pcAction(_ sender: NSTextField!) {
        
        _pcAction(UInt16(sender.intValue))
    }
    
    func _aAction(_ value: UInt8) {
        
        let oldValue = cpuInfo.a
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._aAction(oldValue)
            }
            undoManager?.setActionName("Set Accumulator")
            
            c64.cpu.setA(value)
            refreshCPU()
        }
    }
    
    @IBAction func aAction(_ sender: NSTextField!) {
        
        _aAction(UInt8(sender.intValue))
    }
    
    func _xAction(_ value: UInt8) {
        
        let oldValue = cpuInfo.x
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._xAction(oldValue)
            }
            undoManager?.setActionName("Set X Register")
        
            c64.cpu.setX(value)
            refreshCPU()
        }
    }
    
    @IBAction func xAction(_ sender: NSTextField!) {
        
        _xAction(UInt8(sender.intValue))
    }
    
    func _yAction(_ value: UInt8) {
        
        let oldValue = cpuInfo.y
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._yAction(oldValue)
            }
            undoManager?.setActionName("Set Y Register")
            
            c64.cpu.setY(value)
            refreshCPU()
        }
    }
    
    @IBAction func yAction(_ sender: NSTextField!) {
        
        _yAction(UInt8(sender.intValue))
    }
    
    func _spAction(_ value: UInt8) {
        
        let oldValue = cpuInfo.sp
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._spAction(oldValue)
            }
            undoManager?.setActionName("Set Stack Pointer")
            
            c64.cpu.setSP(value)
            refreshCPU()
        }
    }
    
    @IBAction func spAction(_ sender: NSTextField!) {
        
        _spAction(UInt8(sender.intValue))
    }
    
    // Processor flags
    
    func _nAction(_ value: Bool) {
        
        let oldValue = cpuInfo.nFlag
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._nAction(oldValue)
            }
            undoManager?.setActionName("Toggle Negative Flag")
            
            c64.cpu.setNflag(value)
            refreshCPU()
        }
    }
    
    @IBAction func NAction(_ sender: NSButton!) {
        
        _nAction(sender.intValue != 0)
    }
    
    func _zAction(_ value: Bool) {
        
        let oldValue = cpuInfo.zFlag
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._zAction(oldValue)
            }
            undoManager?.setActionName("Toggle Zero Flag")
            
            c64.cpu.setZflag(value)
            refreshCPU()
        }
    }
    
    @IBAction func ZAction(_ sender: NSButton!) {
        
        _zAction(sender.intValue != 0)
    }
    
    func _cAction(_ value: Bool) {
        
        let oldValue = cpuInfo.cFlag
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._cAction(oldValue)
            }
            undoManager?.setActionName("Toggle Carry Flag")
            
            c64.cpu.setCflag(value)
            refreshCPU()
        }
    }
    
    @IBAction func CAction(_ sender: NSButton!) {
        
        _cAction(sender.intValue != 0)
    }
    
    func _iAction(_ value: Bool) {
        
        let oldValue = cpuInfo.iFlag
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._iAction(oldValue)
            }
            undoManager?.setActionName("Toggle Interrupt Flag")
            
            c64.cpu.setIflag(value)
            refreshCPU()
        }
    }
    
    @IBAction func IAction(_ sender: NSButton!) {
        
        _iAction(sender.intValue != 0)
    }
    
    func _bAction(_ value: Bool) {
        
        let oldValue = cpuInfo.bFlag
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._bAction(oldValue)
            }
            undoManager?.setActionName("Toggle Break Flag")
            
            c64.cpu.setBflag(value)
            refreshCPU()
        }
    }
    
    @IBAction func BAction(_ sender: NSButton!) {
        
        _bAction(sender.intValue != 0)
    }
    
    func _dAction(_ value: Bool) {
        
        let oldValue = cpuInfo.dFlag
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._dAction(oldValue)
            }
            undoManager?.setActionName("Toggle Decimal Flag")
            
            c64.cpu.setDflag(value)
            refreshCPU()
        }
    }
    
    @IBAction func DAction(_ sender: NSButton!) {
        
        _dAction(sender.intValue != 0)
    }
    
    func _vAction(_ value: Bool) {
        
        let oldValue = cpuInfo.vFlag
        if value != oldValue {
            undoManager?.registerUndo(withTarget: self) { me in
                me._vAction(oldValue)
            }
            undoManager?.setActionName("Toggle Overflow Flag")
            
            c64.cpu.setVflag(value)
            refreshCPU()
        }
    }
    
    @IBAction func VAction(_ sender: NSButton!) {
        
        _vAction(sender.intValue != 0)
    }
    
    func _setBreakpointAction(_ value: UInt16) {
        
        undoManager?.registerUndo(withTarget: self) { me in
            me._deleteBreakpointAction(value)
        }
        undoManager?.setActionName("Set Breakpoint")
        
        c64.cpu.setBreakpoint(value)
        cpuTableView.reloadData()
    }
    
    func _deleteBreakpointAction(_ value: UInt16) {
        
        undoManager?.registerUndo(withTarget: self) { me in
            me._setBreakpointAction(value)
        }
        undoManager?.setActionName("Set Breakpoint")
        
        c64.cpu.deleteBreakpoint(value)
        cpuTableView.reloadData()
    }
    
    @IBAction func breakpointAction(_ sender: NSTextField!) {
        
        let value = UInt16(sender.intValue)
        if !c64.cpu.breakpoint(value) {
            _setBreakpointAction(value)
        }
    }
    
    func refreshCPU() {
        
        let info = c64.cpu.getInfo()
        
        pc.intValue = Int32(info.pc)
        a.intValue = Int32(info.a)
        x.intValue = Int32(info.x)
        y.intValue = Int32(info.y)
        sp.intValue = Int32(info.sp)
        nflag.intValue = info.nFlag ? 1 : 0
        vflag.intValue = info.vFlag ? 1 : 0
        bflag.intValue = info.bFlag ? 1 : 0
        dflag.intValue = info.dFlag ? 1 : 0
        iflag.intValue = info.iFlag ? 1 : 0
        zflag.intValue = info.zFlag ? 1 : 0
        cflag.intValue = info.cFlag ? 1 : 0
        
        cpuTableView.refresh()
        cpuTraceView.refresh()
    }
}
