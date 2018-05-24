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
    
    // Voice selector
    @IBAction func selectVoiceAction(_ sender: Any!) {
     
        let sender = sender as! NSSegmentedControl
        selectedVoice = sender.indexOfSelectedItem
        track("selectedVoice = \(selectedVoice)")
        refreshSID()
    }
    
    // Voice items
    
    func _waveform1Action(_ value: UInt8) {
        
        let info = c64.sid.getVoiceInfo(0)
        undoManager?.registerUndo(withTarget: self) {
            me in me._waveform1Action(info.waveform)
        }
        undoManager?.setActionName("Set Voice 1 Waveform")
        c64.mem.pokeIO(UInt16(0xD404), value: value | (info.reg.4 & 0x0F))
        refreshSID()
    }
    func _waveform2Action(_ value: UInt8) {
        
        let info = c64.sid.getVoiceInfo(1)
        undoManager?.registerUndo(withTarget: self) {
            me in me._waveform1Action(info.waveform)
        }
        undoManager?.setActionName("Set Voice 2 Waveform")
        c64.mem.pokeIO(UInt16(0xD40B), value: value | (info.reg.4 & 0x0F))
        refreshSID()
    }
    func _waveform3Action(_ value: UInt8) {
        
        let info = c64.sid.getVoiceInfo(2)
        undoManager?.registerUndo(withTarget: self) {
            me in me._waveform1Action(info.waveform)
        }
        undoManager?.setActionName("Set Voice 3 Waveform")
        c64.mem.pokeIO(UInt16(0xD412), value: value | (info.reg.4 & 0x0F))
        refreshSID()
    }
    func _waveformAction(_ value: (UInt8,Int)) {
        
        let info = c64.sid.getVoiceInfo(value.1)
        undoManager?.registerUndo(withTarget: self) {
            me in me._waveform1Action(info.waveform)
        }
        undoManager?.setActionName("Set Voice Waveform")
        let addr = UInt16(0xD404 + 7*value.1)
        let val = value.0 | (info.reg.4 & 0x0F)
        c64.mem.pokeIO(addr, value: val)
        refreshSID()
    }
    
    @IBAction func waveformAction(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        let value = UInt8(sender.selectedTag())
        _waveformAction((value,selectedVoice))
    }
}

