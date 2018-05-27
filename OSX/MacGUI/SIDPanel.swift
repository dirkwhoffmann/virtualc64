//
//  SIDPanel.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 24.05.18.
//

import Foundation

extension MyController {
    
    @objc func refreshSID() {
        
        let info = c64.sid.getInfo()
        
        // track("Waveform 1 = \(info.voice1.waveform)");
        
        // Volume and potentiometers
        volume.intValue = Int32(info.volume)
        potX.intValue = Int32(info.potX)
        potY.intValue = Int32(info.potY)
        
        // Voice selector
        voiceSelector.selectedSegment = selectedVoice
        
        // Voice items
        let i = voiceSelector.indexOfSelectedItem
        let vinfo = (i == 0) ? info.voice1 : (i == 1) ? info.voice2 : info.voice3
        /*
        switch(vinfo.waveform) {
        case 0x10, 0x20, 0x40, 0x80:
            waveform.selectItem(withTag: Int(vinfo.waveform))
            break
        default:
            waveform.selectItem(withTag: 0)
            break
        }
        */
        if vinfo.waveform & 0x10 != 0 { waveform.selectItem(at: 1) }
        else if vinfo.waveform & 0x20 != 0 { waveform.selectItem(at: 2) }
        else if vinfo.waveform & 0x40 != 0 { waveform.selectItem(at: 3) }
        else if vinfo.waveform & 0x80 != 0 { waveform.selectItem(at: 4) }
        else { waveform.selectItem(at: 0) }

        waveform.item(at: 0)?.state = (vinfo.waveform == 0) ? .on : .off
        waveform.item(at: 1)?.state = (vinfo.waveform & 0x10 != 0) ? .on : .off
        waveform.item(at: 2)?.state = (vinfo.waveform & 0x20 != 0) ? .on : .off
        waveform.item(at: 3)?.state = (vinfo.waveform & 0x40 != 0) ? .on : .off
        waveform.item(at: 4)?.state = (vinfo.waveform & 0x80 != 0) ? .on : .off

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
    
    func pokeSidReg(_ register:(UInt16), _ value:(UInt8)) {
        c64.mem.pokeIO(0xD400 + register, value: value)
    }
    
    func pokeSidReg(_ register:(UInt16), _ value:(UInt8), voice:(Int)) {
        pokeSidReg(register + UInt16(7*voice), value)
    }
    
    // Voice selector
    @IBAction func selectVoiceAction(_ sender: Any!) {
     
        let sender = sender as! NSSegmentedControl
        selectedVoice = sender.indexOfSelectedItem
        track("selectedVoice = \(selectedVoice)")
        refreshSID()
    }
    
    // Voice items
    
    func _waveformAction(_ value: (Int,UInt8)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.waveform
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._waveformAction((voice, oldValue))
            }
            undoManager?.setActionName("Set Voice Waveform")
            pokeSidReg(4, value.1 | (info.reg.4 & 0x0F), voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func waveformAction(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        let value = UInt8(sender.selectedTag())
        _waveformAction((selectedVoice, value))
    }
    
    func _frequencyAction(_ value: (Int,UInt16)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.frequency
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._frequencyAction((voice, oldValue))
            }
            undoManager?.setActionName("Set Voice Frequency")
            let hi = UInt8(value.1 >> 8)
            let lo = UInt8(value.1 & 0x00FF)
            pokeSidReg(0, lo, voice: voice)
            pokeSidReg(1, hi, voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func frequencyAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _frequencyAction((selectedVoice, UInt16(sender.intValue)))
    }
    
    func _pulseWidthAction(_ value: (Int,UInt16)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.pulseWidth
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._pulseWidthAction((voice, oldValue))
            }
            undoManager?.setActionName("Set Pulse Width")
            let hi = UInt8(value.1 >> 8) & 0x0F
            let lo = UInt8(value.1 & 0x00FF)
            pokeSidReg(2, lo, voice: voice)
            pokeSidReg(3, hi, voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func pulseWidthAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _pulseWidthAction((selectedVoice, UInt16(sender.intValue)))
    }
    
    func _attackRateAction(_ value: (Int,UInt8)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.attackRate
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._attackRateAction((voice, oldValue))
            }
            undoManager?.setActionName("Set Attack Rate")
            track("\(value.1) \(value.1 << 4) \(info.reg.5 & 0x0F) \((value.1 << 4) | (info.reg.5 & 0x0F))")
            pokeSidReg(5, (value.1 << 4) | (info.reg.5 & 0x0F), voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func attackRateAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _attackRateAction((selectedVoice, UInt8(sender.intValue)))
    }
    
    func _decayRateAction(_ value: (Int,UInt8)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.decayRate
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._decayRateAction((voice, oldValue))
            }
            undoManager?.setActionName("Set Decay Rate")
            pokeSidReg(5, (info.reg.5 & 0xF0) | value.1, voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func decayRateAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _decayRateAction((selectedVoice, UInt8(sender.intValue)))
    }
    
    func _sustainRateAction(_ value: (Int,UInt8)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.sustainRate
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._sustainRateAction((voice, oldValue))
            }
            undoManager?.setActionName("Set Sustain Rate")
            pokeSidReg(6, (value.1 << 4) | (info.reg.6 & 0x0F), voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func sustainRateAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _sustainRateAction((selectedVoice, UInt8(sender.intValue)))
    }
    
    func _releaseRateAction(_ value: (Int,UInt8)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.releaseRate
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._releaseRateAction((voice, oldValue))
            }
            undoManager?.setActionName("Set Release Rate")
            pokeSidReg(6, (info.reg.6 & 0xF0) | value.1, voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func releaseRateAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _releaseRateAction((selectedVoice, UInt8(sender.intValue)))
    }
}

