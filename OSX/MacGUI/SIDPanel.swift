//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension MyController {
    
    func refreshSID() {
        
        let info = c64.sid.getInfo()
        let vinfo = c64.sid.getVoiceInfo(selectedVoice)
        
        // Volume and potentiometers
        volume.intValue = Int32(info.volume)
        potX.intValue = Int32(info.potX)
        potY.intValue = Int32(info.potY)
        
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
        pulseWidth.isHidden = (vinfo.waveform & 0x40 == 0)
        pulseWidthText.isHidden = (vinfo.waveform & 0x40 == 0)
        
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
        
        if info.filterType & 0x10 != 0 { filterType.selectItem(at: 1) }
        else if info.filterType & 0x20 != 0 { filterType.selectItem(at: 2) }
        else if info.filterType & 0x40 != 0 { filterType.selectItem(at: 3) }
        else { filterType.selectItem(at: 0) }
        filterType.item(at: 0)?.state = (info.filterType == 0) ? .on : .off
        filterType.item(at: 1)?.state = (info.filterType & 0x10 != 0) ? .on : .off
        filterType.item(at: 2)?.state = (info.filterType & 0x20 != 0) ? .on : .off
        filterType.item(at: 3)?.state = (info.filterType & 0x40 != 0) ? .on : .off
        
        filterResonance.intValue = Int32(info.filterResonance)
        filterCutoff.intValue = Int32(info.filterCutoff)
        filter1.intValue = (info.filterEnableBits & 0x01) != 0 ? 1 : 0
        filter2.intValue = (info.filterEnableBits & 0x02) != 0 ? 1 : 0
        filter3.intValue = (info.filterEnableBits & 0x04) != 0 ? 1 : 0
        
        let fillLevel = Int32(c64.sid.fillLevel() * 100)
        audioBufferLevel.intValue = fillLevel
        audioBufferLevelText.stringValue = "\(fillLevel) %"
        bufferUnderflows.intValue = Int32(c64.sid.bufferUnderflows())
        bufferOverflows.intValue = Int32(c64.sid.bufferOverflows())
        
        waveformView.update()
    }
    
    private var selectedVoice: Int {
        get { return voiceSelector.indexOfSelectedItem }
    }
    
    func pokeSidReg(_ register:(UInt16), _ value:(UInt8)) {
        c64.mem.pokeIO(0xD400 + register, value: value)
    }
    
    func pokeSidReg(_ register:(UInt16), _ value:(UInt8), voice:(Int)) {
        pokeSidReg(register + UInt16(7*voice), value)
    }
    
    //
    // Voice section
    //
    
    @IBAction func selectVoiceAction(_ sender: Any!) {
     
        refreshSID()
    }
    
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
    
    @IBAction func waveformAction(_ sender: NSPopUpButton!) {
        
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
    
    @IBAction func frequencyAction(_ sender: NSTextField!) {
        
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
    
    @IBAction func pulseWidthAction(_ sender: NSTextField!) {
        
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
    
    @IBAction func attackRateAction(_ sender: NSTextField!) {
        
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
    
    @IBAction func decayRateAction(_ sender: NSTextField!) {
        
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
    
    @IBAction func sustainRateAction(_ sender: NSTextField!) {
        
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
    
    @IBAction func releaseRateAction(_ sender: NSTextField!) {
        
        _releaseRateAction((selectedVoice, UInt8(sender.intValue)))
    }
    
    func _gateBitAction(_ value: (Int,Bool)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.gateBit
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._gateBitAction((voice, oldValue))
            }
            undoManager?.setActionName("Toogle Gate Bit")
            pokeSidReg(4, (info.reg.4 & 0xFE) | (value.1 ? 0x01 : 0x00), voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func gateBitAction(_ sender: NSButton!) {
        
        _gateBitAction((selectedVoice, sender.intValue != 0))
    }
    
    func _hardSyncAction(_ value: (Int,Bool)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.hardSync
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._hardSyncAction((voice, oldValue))
            }
            undoManager?.setActionName("Toogle Hard Sync Bit")
            pokeSidReg(4, (info.reg.4 & 0xFD) | (value.1 ? 0x02 : 0x00), voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func hardSyncAction(_ sender: NSButton!) {
        
        _hardSyncAction((selectedVoice, sender.intValue != 0))
    }
    
    func _ringModAction(_ value: (Int,Bool)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.ringMod
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._ringModAction((voice, oldValue))
            }
            undoManager?.setActionName("Toogle Ring Modulation Bit")
            pokeSidReg(4, (info.reg.4 & 0xFB) | (value.1 ? 0x04 : 0x00), voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func ringModAction(_ sender: NSButton!) {
        
        _ringModAction((selectedVoice, sender.intValue != 0))
    }
    
    func _testBitAction(_ value: (Int,Bool)) {
        
        let voice = value.0
        let info = c64.sid.getVoiceInfo(voice)
        let oldValue = info.testBit
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._testBitAction((voice, oldValue))
            }
            undoManager?.setActionName("Toogle Test Bit")
            pokeSidReg(4, (info.reg.4 & 0xF7) | (value.1 ? 0x08 : 0x00), voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func testBitAction(_ sender: NSButton!) {
        
        _testBitAction((selectedVoice, sender.intValue != 0))
    }
    
    //
    // Filter section
    //
    
    func _filterAction(_ value: (Int,UInt8)) {
        
        let voice = value.0
        let info = c64.sid.getInfo()
        let oldValue = info.filterType
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._filterAction((voice, oldValue))
            }
            undoManager?.setActionName("Set Filter Type")
            let otherBits = (info.filterModeBits & 0x8F) | info.volume
            pokeSidReg(0x18, value.1 | otherBits)
            refreshSID()
        }
    }
    
    @IBAction func filterAction(_ sender: NSPopUpButton!) {
        
        let value = UInt8(sender.selectedTag())
        _filterAction((selectedVoice, value))
    }
    
    func _filterCutoffAction(_ value: (Int,UInt16)) {
        
        let voice = value.0
        let info = c64.sid.getInfo()
        let oldValue = info.filterCutoff
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._filterCutoffAction((voice, oldValue))
            }
            undoManager?.setActionName("Set Filter Cutoff")
            let hi = UInt8((value.1 >> 3) & 0xFF)
            let lo = UInt8(value.1 & 0x7)
            pokeSidReg(0x15, lo, voice: voice)
            pokeSidReg(0x16, hi, voice: voice)
            refreshSID()
        }
    }
    
    @IBAction func filterCutoffAction(_ sender: NSTextField!) {
        
        _filterCutoffAction((selectedVoice, UInt16(sender.intValue)))
    }
    
    func _filterResonanceAction(_ value: (Int,UInt8)) {
        
        let voice = value.0
        let info = c64.sid.getInfo()
        let oldValue = info.filterResonance
        let newValue = value.1 & 0x0F
        
        if (newValue != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._filterResonanceAction((voice, oldValue))
            }
            undoManager?.setActionName("Set Filter Resonance")
            // track("\(newValue << 4) \((newValue << 4) | info.filterEnableBits)")
            pokeSidReg(0x17, (newValue << 4) | info.filterEnableBits)
            refreshSID()
        }
    }
    
    @IBAction func filterResonanceAction(_ sender: NSTextField!) {
        
        _filterResonanceAction((selectedVoice, UInt8(sender.intValue)))
    }
    
    func _filterEnableAction(_ value: (Int,Bool)) {
        
        let voice = value.0
        let mask = UInt8(1 << value.0)
        let info = c64.sid.getInfo()
        let oldValue = (info.filterEnableBits & mask) != 0
        let newValue = value.1
        
        if (newValue != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._filterEnableAction((voice, oldValue))
            }
            undoManager?.setActionName("Toggle Filter Bit")
            let oldBits = (info.filterResonance << 4) | info.filterEnableBits
            pokeSidReg(0x17, (oldBits & ~mask) | (newValue ? mask : 0x00))
            refreshSID()
        }
    }
    
    @IBAction func filterEnableAction(_ sender: NSButton!) {
        
        _filterEnableAction((sender.tag, sender.intValue != 0))
    }
    
    //
    // Volume and potentiometers
    //
    
    func _volumeAction(_ value: UInt8) {
        
        let info = c64.sid.getInfo()
        let oldValue = info.volume
        let newValue = value & 0x0F
        
        if (newValue != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._volumeAction(oldValue)
            }
            undoManager?.setActionName("Set Volume")
            pokeSidReg(0x18, (info.filterEnableBits & 0xF0) | newValue)
            refreshSID()
        }
    }
    
    @IBAction func volumeAction(_ sender: NSTextField!) {
        
        _volumeAction(UInt8(sender.intValue))
    }
}

