//
//  CIAPanel.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 24.05.18.
//

import Foundation

extension MyController {
    
    func refreshCIA() {
        
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
    
    private var selectedCia: Int {
        get { return ciaSelector.indexOfSelectedItem == 0 ? 1 : 2 }
    }
    
    @IBAction func selectCIAAction(_ sender: Any!) {
        
        let sender = sender as! NSSegmentedControl
        selectedVoice = sender.indexOfSelectedItem
        refreshCIA()
    }
    
    func _praAction(_ value: (Int,UInt8)) {
        
        let cia = c64.cia(value.0)
        let info = cia!.getInfo()
        let oldValue = info.portA.reg
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._praAction((value.0, oldValue))
            }
            undoManager?.setActionName("Set Port Register A")
            cia?.poke(0x0, value: value.1)
            refreshCIA()
        }
    }
    
    @IBAction func praAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _praAction((selectedCia, UInt8(sender.intValue)))
    }

    func _prbAction(_ value: (Int,UInt8)) {
        
        let cia = c64.cia(value.0)
        let info = cia!.getInfo()
        let oldValue = info.portB.reg
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._prbAction((value.0, oldValue))
            }
            undoManager?.setActionName("Set Port Register B")
            cia?.poke(0x1, value: value.1)
            refreshCIA()
        }
    }
    
    @IBAction func prbAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _prbAction((selectedCia, UInt8(sender.intValue)))
    }
 
    func _ddraAction(_ value: (Int,UInt8)) {
        
        let cia = c64.cia(value.0)
        let info = cia!.getInfo()
        let oldValue = info.portA.dir
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._ddraAction((value.0, oldValue))
            }
            undoManager?.setActionName("Set Port Direction A")
            cia?.poke(0x2, value: value.1)
            refreshCIA()
        }
    }
    
    @IBAction func ddraAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _ddraAction((selectedCia, UInt8(sender.intValue)))
    }
    
    func _ddrbAction(_ value: (Int,UInt8)) {
        
        let cia = c64.cia(value.0)
        let info = cia!.getInfo()
        let oldValue = info.portB.dir
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._ddrbAction((value.0, oldValue))
            }
            undoManager?.setActionName("Set Port Direction B")
            cia?.poke(0x3, value: value.1)
            refreshCIA()
        }
    }
    
    @IBAction func ddrbAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _ddrbAction((selectedCia, UInt8(sender.intValue)))
    }
    
    func _timerLatchAAction(_ value: (Int,UInt16)) {
        
        let cia = c64.cia(value.0)
        let info = cia!.getInfo()
        let oldValue = info.timerA.latch
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._timerLatchAAction((value.0, oldValue))
            }
            undoManager?.setActionName("Set Timer Latch A")
            cia!.poke(0x4, value: UInt8(value.1 & 0xFF))
            cia!.poke(0x5, value: UInt8(value.1 >> 8))
            refreshCIA()
        }
    }
    
    @IBAction func timerLatchAAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _timerLatchAAction((selectedCia, UInt16(sender.intValue)))
    }
    
    func _timerLatchBAction(_ value: (Int,UInt16)) {
        
        let cia = c64.cia(value.0)
        let info = cia!.getInfo()
        let oldValue = info.timerB.latch
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._timerLatchBAction((value.0, oldValue))
            }
            undoManager?.setActionName("Set Timer Latch B")
            cia!.poke(0x6, value: UInt8(value.1 & 0xFF))
            cia!.poke(0x7, value: UInt8(value.1 >> 8))
            refreshCIA()
        }
    }
    
    @IBAction func timerLatchBAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _timerLatchBAction((selectedCia, UInt16(sender.intValue)))
    }
}




