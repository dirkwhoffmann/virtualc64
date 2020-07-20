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
                
        let vicRevision = config.vicRevision
        let resid = config.sidEngine == ENGINE_RESID.rawValue
        
        track()
        
        // VIC
        hwVicModelPopup.selectItem(withTag: vicRevision)
        
        switch vicRevision {
            
        case PAL_6569_R1.rawValue,
             PAL_6569_R3.rawValue,
             PAL_8565.rawValue:
            
            hwVicIcon.image = NSImage(named: "pref_vicii_pal")
            
        case NTSC_6567_R56A.rawValue:
            
            hwVicIcon.image = NSImage(named: "pref_vicii_ntsc")
            
        case NTSC_6567.rawValue,
             NTSC_8562.rawValue:
            
            hwVicIcon.image = NSImage(named: "pref_vicii_ntsc")
            
        default:
            assert(false)
        }
        hwVicGrayDotBug.state = config.vicGrayDotBug ? .on : .off
        
        // CIA
        hwCiaModelPopup.selectItem(withTag: config.ciaRevision)
        hwCiaTimerBBug.state = config.ciaTimerBBug ? .on : .off
        
        // Audio
        hwSidModelPopup.selectItem(withTag: config.sidRevision)
        hwSidFilter.state = config.sidFilter ? .on : .off
        hwSidEnginePopup.selectItem(withTag: config.sidEngine)
        hwSidSamplingPopup.isEnabled = resid
        hwSidSamplingPopup.selectItem(withTag: config.sidSampling)
        
        // Logic board
        hwGlueLogicPopup.selectItem(withTag: config.glueLogic)

        // Startup
        hwRamPatternPopup.selectItem(withTag: config.ramPattern)
        
        // Drive
         hwDrive8Connect.state = config.drive8Connected ? .on : .off
         hwDrive9Connect.state = config.drive9Connected ? .on : .off
         hwDrive8Type.selectItem(withTag: config.drive8Type)
         hwDrive9Type.selectItem(withTag: config.drive9Type)
        
        // Ports
        parent.gamePadManager.refresh(popup: hwGameDevice1, hide: true)
        parent.gamePadManager.refresh(popup: hwGameDevice2, hide: true)
        hwGameDevice1.selectItem(withTag: config.gameDevice1)
        hwGameDevice2.selectItem(withTag: config.gameDevice2)
        
        // Configuration info text
        /*
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
        */
        
        // Power button
        hwPowerButton.isHidden = !bootable
    }
    
    @IBAction func hwVicRevAction(_ sender: NSPopUpButton!) {
        
        config.vicRevision = sender.selectedTag()
        refresh()
    }
    
    @IBAction func hwVicGrayDotBugAction(_ sender: NSButton!) {
        
        config.vicGrayDotBug = sender.state == .on
        refresh()
    }
    
    @IBAction func hwCiaRevAction(_ sender: NSPopUpButton!) {
        
        config.ciaRevision = sender.selectedTag()
        refresh()
    }
    
    @IBAction func hwCiaTimerBBugAction(_ sender: NSButton!) {

        config.ciaTimerBBug = sender.state == .on
        refresh()
    }
    
    @IBAction func hwSidRevAction(_ sender: NSPopUpButton!) {
        
        config.sidRevision = sender.selectedTag()
        refresh()
    }

    @IBAction func hwSidFilterAction(_ sender: NSButton!) {
        
        config.sidFilter = sender.state == .on
        refresh()
    }
    
    @IBAction func hwSidEngineAction(_ sender: NSPopUpButton!) {
        
        config.sidEngine = sender.selectedTag()
        refresh()
    }
    
    @IBAction func hwSidSamplingAction(_ sender: NSPopUpButton!) {
        
        config.sidSampling = sender.selectedTag()
        refresh()
    }
        
    @IBAction func hwGlueLogicAction(_ sender: NSPopUpButton!) {
        
        config.glueLogic = sender.selectedTag()
        refresh()
    }
        
    @IBAction func hwDriveConnectAction(_ sender: NSButton!) {
        
        switch sender.tag {
        case 8: config.drive8Connected = sender.state == .on
        case 9: config.drive9Connected = sender.state == .on
        default: fatalError()
        }
        refresh()
    }
    
    @IBAction func hwDriveTypeAction(_ sender: NSPopUpButton!) {
        
        switch sender.tag {
        case 8: config.drive8Type = sender.tag
        case 9: config.drive9Type = sender.tag
        default: fatalError()
        }
        refresh()
    }
    
    @IBAction func hwGameDeviceAction(_ sender: NSPopUpButton!) {
        
        track("port: \(sender.tag) device: \(sender.selectedTag())")
        
        switch sender.tag {
        case 1: config.gameDevice1 = sender.selectedTag()
        case 2: config.gameDevice2 = sender.selectedTag()
        default: fatalError()
        }
        refresh()
    }

    @IBAction func hwRamPatternAction(_ sender: NSPopUpButton!) {
        
        track()
        config.ramPattern = sender.selectedTag()
        refresh()
    }

    @IBAction func hwPresetAction(_ sender: NSPopUpButton!) {
        
        switch sender.selectedTag() {
        case 0: config.loadHardwareDefaults(HardwareDefaults.C64_PAL)
        case 1: config.loadHardwareDefaults(HardwareDefaults.C64_II_PAL)
        case 2: config.loadHardwareDefaults(HardwareDefaults.C64_OLD_PAL)
        case 3: config.loadHardwareDefaults(HardwareDefaults.C64_NTSC)
        case 4: config.loadHardwareDefaults(HardwareDefaults.C64_II_NTSC)
        case 5: config.loadHardwareDefaults(HardwareDefaults.C64_OLD_NTSC)
        default: fatalError()
        }
        refresh()
    }
    
    @IBAction func hwDefaultsAction(_ sender: NSButton!) {
        
        track()
        config.saveHardwareUserDefaults()
    }
}
