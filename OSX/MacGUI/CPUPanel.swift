//
//  CPUPanel.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 24.05.18.
//

import Foundation

extension MyController {
    
    // Registers
    
    func _pcAction(_ value: UInt16) {
        
        let undoValue = c64.cpu.pc()
        undoManager?.registerUndo(withTarget: self) {
            me in me._pcAction(undoValue)
        }
        undoManager?.setActionName("Set Program Counter")
        
        c64.cpu.setPC(value)
        refresh()
    }
    
    @IBAction func pcAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt16(sender.intValue)
        if (value != c64.cpu.pc()) {
            _pcAction(value)
        }
    }
    
    func _aAction(_ value: UInt8) {
        
        let undoValue = c64.cpu.a()
        undoManager?.registerUndo(withTarget: self) {
            me in me._aAction(undoValue)
        }
        undoManager?.setActionName("Set Accumulator")
        
        c64.cpu.setA(value)
        refresh()
    }
    
    @IBAction func aAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt8(sender.intValue)
        if (value != c64.cpu.a()) {
            _aAction(value)
        }
    }
    
    func _xAction(_ value: UInt8) {
        
        let undoValue = c64.cpu.x()
        undoManager?.registerUndo(withTarget: self) {
            me in me._xAction(undoValue)
        }
        undoManager?.setActionName("Set X Register")
        
        c64.cpu.setX(value)
        refresh()
    }
    
    @IBAction func xAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt8(sender.intValue)
        if (value != c64.cpu.x()) {
            _xAction(value)
        }
    }
    
    func _yAction(_ value: UInt8) {
        
        let undoValue = c64.cpu.y()
        undoManager?.registerUndo(withTarget: self) {
            me in me._yAction(undoValue)
        }
        undoManager?.setActionName("Set Y Register")
        
        c64.cpu.setY(value)
        refresh()
    }
    
    @IBAction func yAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt8(sender.intValue)
        if (value != c64.cpu.y()) {
            _yAction(value)
        }
    }
    
    func _spAction(_ value: UInt8) {
        
        let undoValue = c64.cpu.sp()
        undoManager?.registerUndo(withTarget: self) {
            me in me._spAction(undoValue)
        }
        undoManager?.setActionName("Set Stack Pointer")
        
        c64.cpu.setSP(value)
        refresh()
    }
    
    @IBAction func spAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt8(sender.intValue)
        if (value != c64.cpu.sp()) {
            _spAction(value)
        }
    }
    
    // Processor flags
    
    func _nAction(_ value: Bool) {
        
        let undoValue = c64.cpu.nflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._nAction(undoValue)
        }
        undoManager?.setActionName("Toggle Negative Flag")
        
        c64.cpu.setNflag(value)
        refresh()
    }
    
    @IBAction func NAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _nAction(sender.intValue != 0)
    }
    
    func _zAction(_ value: Bool) {
        
        let undoValue = c64.cpu.zflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._zAction(undoValue)
        }
        undoManager?.setActionName("Toggle Zero Flag")
        
        c64.cpu.setZflag(value)
        refresh()
    }
    
    @IBAction func ZAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _zAction(sender.intValue != 0)
    }
    
    func _cAction(_ value: Bool) {
        
        let undoValue = c64.cpu.cflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._cAction(undoValue)
        }
        undoManager?.setActionName("Toggle Carry Flag")
        
        c64.cpu.setCflag(value)
        refresh()
    }
    
    @IBAction func CAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _cAction(sender.intValue != 0)
    }
    
    func _iAction(_ value: Bool) {
        
        let undoValue = c64.cpu.iflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._iAction(undoValue)
        }
        undoManager?.setActionName("Toggle Interrupt Flag")
        
        c64.cpu.setIflag(value)
        refresh()
    }
    
    @IBAction func IAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _iAction(sender.intValue != 0)
    }
    
    func _bAction(_ value: Bool) {
        
        let undoValue = c64.cpu.bflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._bAction(undoValue)
        }
        undoManager?.setActionName("Toggle Break Flag")
        
        c64.cpu.setBflag(value)
        refresh()
    }
    
    @IBAction func BAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _bAction(sender.intValue != 0)
    }
    
    func _dAction(_ value: Bool) {
        
        let undoValue = c64.cpu.dflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._dAction(undoValue)
        }
        undoManager?.setActionName("Toggle Decimal Flag")
        
        c64.cpu.setDflag(value)
        refresh()
    }
    
    @IBAction func DAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _dAction(sender.intValue != 0)
    }
    
    func _vAction(_ value: Bool) {
        
        let undoValue = c64.cpu.vflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._vAction(undoValue)
        }
        undoManager?.setActionName("Toggle Overflow Flag")
        
        c64.cpu.setVflag(value)
        refresh()
    }
    
    @IBAction func VAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _vAction(sender.intValue != 0)
    }
    
    func _setBreakpointAction(_ value: UInt16) {
        
        undoManager?.registerUndo(withTarget: self) {
            me in me._deleteBreakpointAction(value)
        }
        undoManager?.setActionName("Set Breakpoint")
        
        c64.cpu.setHardBreakpoint(value)
        cpuTableView.reloadData()
    }
    
    func _deleteBreakpointAction(_ value: UInt16) {
        
        undoManager?.registerUndo(withTarget: self) {
            me in me._setBreakpointAction(value)
        }
        undoManager?.setActionName("Set Breakpoint")
        
        c64.cpu.deleteHardBreakpoint(value)
        cpuTableView.reloadData()
    }
    
    @IBAction func breakpointAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt16(sender.intValue)
        if !c64.cpu.hardBreakpoint(value) {
            _setBreakpointAction(value)
        }
    }
    
    @objc func refreshCPU() {
        
        pc.intValue = Int32(c64.cpu.pc())
        a.intValue = Int32(c64.cpu.a())
        x.intValue = Int32(c64.cpu.x())
        y.intValue = Int32(c64.cpu.y())
        sp.intValue = Int32(c64.cpu.sp())
        nflag.intValue = c64.cpu.nflag() ? 1 : 0
        vflag.intValue = c64.cpu.vflag() ? 1 : 0
        bflag.intValue = c64.cpu.bflag() ? 1 : 0
        dflag.intValue = c64.cpu.dflag() ? 1 : 0
        iflag.intValue = c64.cpu.iflag() ? 1 : 0
        zflag.intValue = c64.cpu.zflag() ? 1 : 0
        cflag.intValue = c64.cpu.cflag() ? 1 : 0
    }
}
