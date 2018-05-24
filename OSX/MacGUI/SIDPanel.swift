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
    
    // Voice items
    
    func _waveFormAction(_ waveform : UInt8, forVoice: Int) {
        
        let info = c64.sid.getVoiceInfo(0)
        let oldWaveform = info.waveform
        
        if waveform != oldWaveform {
            let target = undoManager?.prepare(withInvocationTarget: self)
            _ = (target as! MyController)._waveFormAction(oldWaveform, forVoice: forVoice)
            undoManager?.setActionName("Set Waveform")
            // TODO
            // Use c64.pokeIO(..., waveform)
            refreshSID()
        }
    }
    
    @IBAction func waveformAction(_ sender: Any!) {
        
        /*
        let sender = sender as! NSTextField
        let info = c64.sid.getVoiceInfo(forVoice)
        let undoValue = info.wavefor
        let value = UInt16(sender.intValue)
        if (value != c64.cpu.pc()) {
            _pcAction(value)
        }
         */
    }
}

