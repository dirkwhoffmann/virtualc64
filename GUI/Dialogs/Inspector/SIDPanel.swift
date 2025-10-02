// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Inspector {
    
    var selectedSID: Int { return sidSelector.indexOfSelectedItem }

    func refreshSID(count: Int = 0, full: Bool = false) {
        
        if let emu = emu {
            
            let info = emu.sid.getInfo(selectedSID)

            if full {

                sidFrequency1.assignFormatter(fmt16)
                sidPulseWidth1.assignFormatter(fmt12)
                sidAttackRate1.assignFormatter(fmt4)
                sidDecayRate1.assignFormatter(fmt4)
                sidSustainRate1.assignFormatter(fmt4)
                sidReleaseRate1.assignFormatter(fmt4)

                sidFrequency2.assignFormatter(fmt16)
                sidPulseWidth2.assignFormatter(fmt12)
                sidAttackRate2.assignFormatter(fmt4)
                sidDecayRate2.assignFormatter(fmt4)
                sidSustainRate2.assignFormatter(fmt4)
                sidReleaseRate2.assignFormatter(fmt4)

                sidFrequency3.assignFormatter(fmt16)
                sidPulseWidth3.assignFormatter(fmt12)
                sidAttackRate3.assignFormatter(fmt4)
                sidDecayRate3.assignFormatter(fmt4)
                sidSustainRate3.assignFormatter(fmt4)
                sidReleaseRate3.assignFormatter(fmt4)

                sidVolume.assignFormatter(fmt8)
                sidPotX.assignFormatter(fmt8)
                sidPotY.assignFormatter(fmt8)
                sidFilterCutoff.assignFormatter(fmt12)
                sidFilterResonance.assignFormatter(fmt8)
            }

            // First voice
            var vinfo = info.voice.0
            refresh(waveform: vinfo.waveform, waveformPopup: sidWaveform1)
            sidPulseWidth1.isHidden = (vinfo.waveform & 0x40 == 0)
            sidPulseWidthText1.isHidden = (vinfo.waveform & 0x40 == 0)
            sidFrequency1.intValue = Int32(vinfo.frequency)
            sidPulseWidth1.intValue = Int32(vinfo.pulseWidth)
            sidPulseWidth1.intValue = Int32(vinfo.pulseWidth)
            sidAttackRate1.intValue = Int32(vinfo.attackRate)
            sidDecayRate1.intValue = Int32(vinfo.decayRate)
            sidSustainRate1.intValue = Int32(vinfo.sustainRate)
            sidReleaseRate1.intValue = Int32(vinfo.releaseRate)
            sidGateBit1.intValue = vinfo.gateBit ? 1 : 0
            sidTestBit1.intValue = vinfo.testBit ? 1 : 0
            sidSyncBit1.intValue = vinfo.hardSync ? 1 : 0
            sidRingBit1.intValue = vinfo.ringMod ? 1 : 0

            // Second voice
            vinfo = info.voice.1
            refresh(waveform: vinfo.waveform, waveformPopup: sidWaveform2)
            sidPulseWidth2.isHidden = (vinfo.waveform & 0x40 == 0)
            sidPulseWidthText2.isHidden = (vinfo.waveform & 0x40 == 0)
            sidFrequency2.intValue = Int32(vinfo.frequency)
            sidPulseWidth2.intValue = Int32(vinfo.pulseWidth)
            sidPulseWidth2.intValue = Int32(vinfo.pulseWidth)
            sidAttackRate2.intValue = Int32(vinfo.attackRate)
            sidDecayRate2.intValue = Int32(vinfo.decayRate)
            sidSustainRate2.intValue = Int32(vinfo.sustainRate)
            sidReleaseRate2.intValue = Int32(vinfo.releaseRate)
            sidGateBit2.intValue = vinfo.gateBit ? 1 : 0
            sidTestBit2.intValue = vinfo.testBit ? 1 : 0
            sidSyncBit2.intValue = vinfo.hardSync ? 1 : 0
            sidRingBit2.intValue = vinfo.ringMod ? 1 : 0

            // Third voice
            vinfo = info.voice.2
            refresh(waveform: vinfo.waveform, waveformPopup: sidWaveform3)
            sidPulseWidth3.isHidden = (vinfo.waveform & 0x40 == 0)
            sidPulseWidthText3.isHidden = (vinfo.waveform & 0x40 == 0)
            sidFrequency3.intValue = Int32(vinfo.frequency)
            sidPulseWidth3.intValue = Int32(vinfo.pulseWidth)
            sidPulseWidth3.intValue = Int32(vinfo.pulseWidth)
            sidAttackRate3.intValue = Int32(vinfo.attackRate)
            sidDecayRate3.intValue = Int32(vinfo.decayRate)
            sidSustainRate3.intValue = Int32(vinfo.sustainRate)
            sidReleaseRate3.intValue = Int32(vinfo.releaseRate)
            sidGateBit3.intValue = vinfo.gateBit ? 1 : 0
            sidTestBit3.intValue = vinfo.testBit ? 1 : 0
            sidSyncBit3.intValue = vinfo.hardSync ? 1 : 0
            sidRingBit3.intValue = vinfo.ringMod ? 1 : 0

            // Volume and potentiometers
            sidVolume.intValue = Int32(info.volume)
            sidPotX.intValue = Int32(info.potX)
            sidPotY.intValue = Int32(info.potY)

            // Filter
            sidFilterType.item(at: 0)?.state = (info.filterType == 0) ? .on : .off
            sidFilterType.item(at: 1)?.state = (info.filterType & 0x10 != 0) ? .on : .off
            sidFilterType.item(at: 2)?.state = (info.filterType & 0x20 != 0) ? .on : .off
            sidFilterType.item(at: 3)?.state = (info.filterType & 0x40 != 0) ? .on : .off

            sidFilterResonance.intValue = Int32(info.filterResonance)
            sidFilterCutoff.intValue = Int32(info.filterCutoff)
            sidFilter1.intValue = (info.filterEnableBits & 0x01) != 0 ? 1 : 0
            sidFilter2.intValue = (info.filterEnableBits & 0x02) != 0 ? 1 : 0
            sidFilter3.intValue = (info.filterEnableBits & 0x04) != 0 ? 1 : 0

            // Audio buffer
            let stats = emu.audioPort.stats
            let fillLevel = Int32(stats.fillLevel * 100)
            sidAudioBufferLevel.intValue = fillLevel
            sidAudioBufferLevelText.stringValue = "\(fillLevel) %"
            sidBufferUnderflows.intValue = Int32(stats.bufferUnderflows)
            sidBufferOverflows.intValue = Int32(stats.bufferOverflows)

            sidWaveformView.update()
        }
    }
    
    func refresh(waveform: UInt8, waveformPopup popup: NSPopUpButton) {

        popup.item(at: 0)?.state = (waveform == 0) ? .on : .off
        popup.item(at: 1)?.state = (waveform & 0x10 != 0) ? .on : .off
        popup.item(at: 2)?.state = (waveform & 0x20 != 0) ? .on : .off
        popup.item(at: 3)?.state = (waveform & 0x40 != 0) ? .on : .off
        popup.item(at: 4)?.state = (waveform & 0x80 != 0) ? .on : .off
    }

    @IBAction func selectSIDAction(_ sender: Any!) {

        fullRefresh()
    }
}
