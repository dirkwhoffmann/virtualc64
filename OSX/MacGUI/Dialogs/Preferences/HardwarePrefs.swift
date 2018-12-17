//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension PreferencesController {
    
    func refreshHardwareTab() {
        
        track()
        
        guard let c64 = proxy else { return }
        
        // VIC
        let model = c64.vic.model()
        hwVicModelPopup.selectItem(withTag: model)
        
        switch (UInt32(model)) {
            
        case PAL_6569_R1.rawValue,
             PAL_6569_R3.rawValue,
             PAL_8565.rawValue:
            
            hwVicIcon.image = NSImage(named: "pref_vicii_pal")
            hwVicDescription.stringValue = "PAL 0.985 MHz 65 cycles"
            
        case NTSC_6567_R56A.rawValue:
            
            hwVicIcon.image = NSImage(named: "pref_vicii_ntsc")
            hwVicDescription.stringValue = "NTSC 1.023 MHz 64 cycles"
            
        case NTSC_6567.rawValue,
             NTSC_8562.rawValue:
            
            hwVicIcon.image = NSImage(named: "pref_vicii_ntsc")
            hwVicDescription.stringValue = "NTSC 1.023 MHz 63 cycles"
            
        default:
            assert(false)
        }
        hwVicGrayDotBug.state = c64.vic.emulateGrayDotBug() ? .on : .off
        
        // CIA
        assert(c64.cia1.model() == c64.cia2.model());
        assert(c64.cia1.emulateTimerBBug() == c64.cia2.emulateTimerBBug());
        hwCiaModelPopup.selectItem(withTag: c64.cia1.model())
        hwCiaTimerBBug.state = c64.cia1.emulateTimerBBug() ? .on : .off
        
        // Audio
        let sidModel = c64.sid.model()
        hwSidModelPopup.selectItem(withTag: sidModel)
        hwSidFilter.state = c64.sid.audioFilter() ? .on : .off
        hwSidEnginePopup.selectItem(withTag: (c64.sid.reSID() ? 1 : 0))
        hwSidSamplingPopup.isEnabled = c64.sid.reSID()
        hwSidSamplingPopup.selectItem(withTag: c64.sid.samplingMethod())
        
        // Logic board
        hwGlueLogicPopup.selectItem(withTag: c64.vic.glueLogic())
        hwRamInitPatternPopup.selectItem(withTag: c64.mem.ramInitPattern())
        
        // Configuration info text
        var descr = "???"
        switch UInt32(c64.model()) {
        case C64_PAL.rawValue:
            descr = "matches a C64 with brown casing (breadbox) and PAL video ouput"
            
        case C64_II_PAL.rawValue:
            descr = "matches a C64 II (white casing) with PAL video ouput"
            
        case C64_OLD_PAL.rawValue:
            descr = "matches an early C64 with brown casing (breadbox) and PAL video output"
            
        case C64_NTSC.rawValue:
            descr = "matches a C64 with brown casing (breadbox) and NTSC video ouput"
            
        case C64_II_NTSC.rawValue:
            descr = "matches a C64 II (white casing) with NTSC video ouput"
            
        case C64_OLD_NTSC.rawValue:
            descr = "matches an early C64 with brown casing (breadbox) and NTSC video output"
            
        default:
            descr = "is a custom configuration. It matches no known C64 model"
        }
        hwInfoText.stringValue = "This configuration \(descr)."

        hwOkButton.title = parent.c64.isRunnable() ? "OK" : "Quit"
    }
    
    @IBAction func hwVicModelAction(_ sender: NSPopUpButton!) {
        
        proxy?.vic.setModel(sender.selectedTag())
        refresh()
    }
    
    @IBAction func hwVicGrayDotBugAction(_ sender: NSButton!) {
        
        proxy?.vic.setEmulateGrayDotBug(sender.state == .on)
        refresh()
    }
    
    @IBAction func hwCiaModelAction(_ sender: NSPopUpButton!) {
        
        proxy?.cia1.setModel(sender.selectedTag())
        proxy?.cia2.setModel(sender.selectedTag())
        refresh()
    }
    
    @IBAction func hwCiaTimerBBugAction(_ sender: NSButton!) {
        
        proxy?.cia1.setEmulateTimerBBug(sender.state == .on)
        proxy?.cia2.setEmulateTimerBBug(sender.state == .on)
        refresh()
    }
    
    @IBAction func hwSidFilterAction(_ sender: NSButton!) {
        
        proxy?.sid.setAudioFilter(sender.state == .on)
        refresh()
    }
    
    @IBAction func hwSidEngineAction(_ sender: NSPopUpButton!) {
        
        proxy?.sid.setReSID(sender.selectedTag() == 1)
        refresh()
    }
    
    @IBAction func hwSidSamplingAction(_ sender: NSPopUpButton!) {
        
        proxy?.sid.setSamplingMethod(sender.selectedTag())
        refresh()
    }
    
    @IBAction func hwSidModelAction(_ sender: NSPopUpButton!) {
        
        guard let c64 = proxy else { return }
        
        let model = UInt32(sender.selectedTag())
        let method = UInt32(c64.sid.samplingMethod())
        
        track("Model = \(model) method = \(method)")
        if (model == MOS_8580.rawValue && method == SID_SAMPLE_FAST.rawValue) {
            parent.showResidSamplingMethodAlert()
            c64.sid.setSamplingMethod(Int(SID_SAMPLE_INTERPOLATE.rawValue))
        }
        
        c64.sid.setModel(sender.selectedTag())
        refresh()
    }
    
    @IBAction func hwGlueLogicAction(_ sender: NSPopUpButton!) {
        
        proxy?.vic.setGlueLogic(sender.selectedTag())
        refresh()
    }
    
    @IBAction func hwRamInitPatternAction(_ sender: NSPopUpButton!) {
        
        proxy?.mem.setRamInitPattern(sender.selectedTag())
        refresh()
    }

    @IBAction func hwFactorySettingsAction(_ sender: NSPopUpButton!) {
        
        if sender.selectedTag() != C64_CUSTOM.rawValue {
            proxy?.setModel(sender.selectedTag())
        }
        refresh()
    }
}
