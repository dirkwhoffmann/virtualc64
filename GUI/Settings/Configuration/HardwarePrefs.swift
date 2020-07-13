// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {
    
    func refreshHardwareTab() {
                
        let c64Model = c64.model()
        let vicRevision = config.vicRevision
        
        track()
        
        // VIC
        hwVicModelPopup.selectItem(withTag: vicRevision)
        
        switch vicRevision {
            
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
        hwVicGrayDotBug.state = config.grayDotBug ? .on : .off
        
        // CIA
        hwCiaModelPopup.selectItem(withTag: config.ciaRevision)
        hwCiaTimerBBug.state = config.timerBBug ? .on : .off
        
        // Audio
        hwSidModelPopup.selectItem(withTag: config.sidRevision)
        hwSidFilter.state = c64.sid.audioFilter() ? .on : .off
        hwSidEnginePopup.selectItem(withTag: (c64.sid.reSID() ? 1 : 0))
        hwSidSamplingPopup.isEnabled = c64.sid.reSID()
        hwSidSamplingPopup.selectItem(withTag: c64.sid.samplingMethod())
        
        // Logic board
        hwGlueLogicPopup.selectItem(withTag: config.glueLogic)
        hwRamInitPatternPopup.selectItem(withTag: c64.mem.ramInitPattern())
        
        // Configuration info text
        var descr: String
        switch c64Model {
        case C64_PAL:
            descr = "matches a C64 with brown casing (breadbox) and PAL video ouput"
            
        case C64_II_PAL:
            descr = "matches a C64 II (white casing) with PAL video ouput"
            
        case C64_OLD_PAL:
            descr = "matches an early C64 with brown casing (breadbox) and PAL video output"
            
        case C64_NTSC:
            descr = "matches a C64 with brown casing (breadbox) and NTSC video ouput"
            
        case C64_II_NTSC:
            descr = "matches a C64 II (white casing) with NTSC video ouput"
            
        case C64_OLD_NTSC:
            descr = "matches an early C64 with brown casing (breadbox) and NTSC video output"
            
        default:
            descr = "is a custom configuration. It matches no known C64 model"
        }
        hwInfoText.stringValue = "This configuration \(descr)."

        hwOkButton.title = c64.isReady() ? "OK" : "Quit"
    }
    
    @IBAction func hwVicModelAction(_ sender: NSPopUpButton!) {
        
        config.vicRevision = sender.selectedTag()
        refresh()
    }
    
    @IBAction func hwVicGrayDotBugAction(_ sender: NSButton!) {
        
        config.grayDotBug = sender.state == .on
        refresh()
    }
    
    @IBAction func hwCiaModelAction(_ sender: NSPopUpButton!) {
        
        config.ciaRevision = sender.selectedTag()
        refresh()
    }
    
    @IBAction func hwCiaTimerBBugAction(_ sender: NSButton!) {

        config.timerBBug = sender.state == .on
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
        
        config.sidRevision = sender.selectedTag()
        refresh()
    }
    
    @IBAction func hwGlueLogicAction(_ sender: NSPopUpButton!) {
        
        config.glueLogic = sender.selectedTag()
        refresh()
    }
    
    @IBAction func hwRamInitPatternAction(_ sender: NSPopUpButton!) {
        
        proxy?.mem.setRamInitPattern(sender.selectedTag())
        refresh()
    }

    @IBAction func hwPresetAction(_ sender: NSPopUpButton!) {
        
        if sender.selectedTag() != C64_CUSTOM.rawValue {
            proxy?.setModel((C64Model)(sender.selectedTag()))
        }
        refresh()
    }
    
    @IBAction func hwDefaultsAction(_ sender: NSButton!) {
        
        track()
        // config.saveHardwareUserDefaults()
    }
}
