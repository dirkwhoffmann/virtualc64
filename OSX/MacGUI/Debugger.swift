//
//  Debugger.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 19.05.18.
//

import Foundation

extension MyController {
    
    func setupDebugger() {
        
        // Start with hexadecimal number format
        setHexadecimalAction(self)
        
        // Create and assign binary number formatter
        let bF = MyFormatter.init(inFormat: "[0-1]{0,7}", outFormat: "", radix: 2)
        ciaPAbinary.formatter = bF
        ciaPRA.formatter = bF
        ciaDDRA.formatter = bF
        ciaPBbinary.formatter = bF
        ciaPRB.formatter = bF
        ciaDDRB.formatter = bF
        ciaIcrBinary.formatter = bF
        ciaImrBinary.formatter = bF
    }
}

//
// Panel independent controls
//

extension MyController {
    
    @IBAction func stepIntoAction(_ sender: Any!) {
        
        document?.updateChangeCount(.changeDone)
        c64.step()
        refresh()
    }
  
    @IBAction func stepOverAction(_ sender: Any!) {

        document?.updateChangeCount(.changeDone)
        
        // If the next instruction is a JSR instruction, ...
        if (c64.cpu.readPC() == 0x20) {

            // we set soft breakpoint at next command
            c64.cpu.setSoftBreakpoint(c64.cpu.addressOfNextInstruction())
            c64.run()

        } else {
            
            // Same as step
            stepIntoAction(self)
        }
    }
    
    @IBAction func stopAndGoAction(_ sender: Any!) {
    
        document?.updateChangeCount(.changeDone)
        if c64.isHalted() {
            c64.run()
        } else {
            c64.halt()
            debugPanel.open()
        }
        refresh()
    }
    
    @IBAction func pauseAction(_ sender: Any!) {
        
        if c64.isRunning() {
            c64.halt()
            debugPanel.open()
        }
        refresh()
    }
    
    @IBAction func continueAction(_ sender: Any!) {
        
        document?.updateChangeCount(.changeDone)
        if c64.isHalted() {
            c64.run()
        }
        refresh()
    }
    
    @IBAction func setDecimalAction(_ sender: Any!) {
  
        hex = false
        cpuTableView.setHex(false)

        let bF = MyFormatter.init(inFormat: "[0-9]{0,3}", outFormat: "%03d", radix: 10)
        let sF = MyFormatter.init(inFormat: "[0-9]{0,3}", outFormat: "%03d", radix: 10)
        let wF = MyFormatter.init(inFormat: "[0-9]{0,5}", outFormat: "%05d", radix: 10)
        refresh(bF, word: wF, threedigit: sF)
    }
    
    @IBAction func setHexadecimalAction(_ sender: Any!) {
        
        hex = true
        cpuTableView.setHex(true)

        let bF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,2}", outFormat: "%02X", radix: 16)
        let sF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,3}", outFormat: "%03X", radix: 16)
        let wF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,4}", outFormat: "%04X", radix: 16)
        refresh(bF, word: wF, threedigit: sF)
    }
}

//
// CPU debug panel
//

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

//
// Memory debug panel
//

extension MyController {

    @IBAction func setMemSource(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        memTableView.setMemView(sender.selectedTag())
    }
    
    @IBAction func setHighlighting(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        memTableView.setHighlighting(sender.selectedTag())
    }
}

//
// CIA debug panel
//

extension MyController {
    
    @objc func refreshCIA() {
        
        var info: CIAInfo
        
        if ciaSelector.indexOfSelectedItem == 0 {
                info = c64.cia1.getInfo()
                ciaIntLineLow.title = "IRQ line active"
        } else {
                info = c64.cia2.getInfo()
                ciaIntLineLow.title = "NMI line active"
        }

        ciaPA.intValue = Int32(info.portA.port)
        ciaPAbinary.intValue = Int32(info.portA.port)
        ciaPRA.intValue = Int32(info.portA.reg)
        ciaDDRA.intValue = Int32(info.portA.dir)

        ciaPB.intValue = Int32(info.portB.port)
        ciaPBbinary.intValue = Int32(info.portB.port)
        ciaPRB.intValue = Int32(info.portB.reg)
        ciaDDRB.intValue = Int32(info.portB.dir)

        ciaTimerA.intValue = Int32(info.timerA.count)
        ciaLatchA.intValue = Int32(info.timerA.latch)
        ciaRunningA.state = info.timerA.running ? .on : .off
        ciaToggleA.state = info.timerA.toggle ? .on : .off
        ciaPBoutA.state = info.timerA.pbout ? .on : .off
        ciaOneShotA.state = info.timerA.oneShot ? .on : .off
        
        ciaTimerB.intValue = Int32(info.timerB.count)
        ciaLatchB.intValue = Int32(info.timerB.latch)
        ciaRunningB.state = info.timerB.running ? .on : .off
        ciaToggleB.state = info.timerB.toggle ? .on : .off
        ciaPBoutB.state = info.timerB.pbout ? .on : .off
        ciaOneShotB.state = info.timerB.oneShot ? .on : .off
        
        todHours.intValue = Int32(info.tod.time.hours)
        todMinutes.intValue = Int32(info.tod.time.minutes)
        todSeconds.intValue = Int32(info.tod.time.seconds)
        todTenth.intValue = Int32(info.tod.time.tenth)
        todIntEnable.state = info.todIntEnable ? .on : .off
        alarmHours.intValue = Int32(info.tod.alarm.hours)
        alarmMinutes.intValue = Int32(info.tod.alarm.minutes)
        alarmSeconds.intValue = Int32(info.tod.alarm.seconds)
        alarmTenth.intValue = Int32(info.tod.alarm.tenth)
        
        ciaIcr.intValue = Int32(info.icr)
        ciaIcrBinary.intValue = Int32(info.icr)
        ciaImr.intValue = Int32(info.imr)
        ciaImrBinary.intValue = Int32(info.imr)
        ciaIntLineLow.state = info.intLine ? .off : .on
    }
}

//
// SID debug panel
//

extension MyController {

    @objc func refreshSID() {
        
        let info = c64.sid.getInfo()

        // Volume and potentiometers
        volume.intValue = Int32(info.volume)
        potX.intValue = Int32(info.potX)
        potY.intValue = Int32(info.potY)
        
        // Voice
        let i = voiceSelector.indexOfSelectedItem
        let vinfo = (i == 0) ? info.voice1 : (i == 1) ? info.voice2 : info.voice3
        switch(vinfo.waveform) {
        case 0x10, 0x20, 0x40, 0x80:
            waveform.selectItem(withTag: Int(vinfo.waveform))
            break
        default:
            waveform.selectItem(withTag: 0)
            break
        }
        frequency.intValue = Int32(vinfo.frequency)
        pulseWidth.intValue = Int32(vinfo.pulseWidth)
        pulseWidth.intValue = Int32(vinfo.pulseWidth)
        attackRate.intValue = Int32(vinfo.attackRate)
        decayRate.intValue = Int32(vinfo.decayRate)
        sustainRate.intValue = Int32(vinfo.sustainRate)
        releaseRate.intValue = Int32(vinfo.releaseRate)
        gateBit.intValue = vinfo.gateBit ? 1 : 0
        testBit.intValue = vinfo.testBit ? 1 : 0
        syncBit.intValue = vinfo.hardSync ? 1 : 0
        ringBit.intValue = vinfo.ringMod ? 1 : 0

        filterType.selectItem(withTag: Int(info.filterType))
        filterResonance.intValue = Int32(info.filterResonance)
        filterCutoff.intValue = Int32(info.filterCutoff)
        filter1.intValue = info.voice1.filterOn ? 1 : 0
        filter2.intValue = info.voice2.filterOn ? 1 : 0
        filter3.intValue = info.voice3.filterOn ? 1 : 0

        let fillLevel = Int32(c64.sid.fillLevel() * 100)
        audioBufferLevel.intValue = fillLevel
        audioBufferLevelText.stringValue = "\(fillLevel) %"
        bufferUnderflows.intValue = Int32(c64.sid.bufferUnderflows())
        bufferOverflows.intValue = Int32(c64.sid.bufferOverflows())

        waveformView.update()
    }
}
