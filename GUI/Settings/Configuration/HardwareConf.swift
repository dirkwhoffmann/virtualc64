// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {
    
    func awakeHardwarePrefsFromNib() {
        
        for addr in stride(from: 0xD400, through: 0xD7E0, by: 0x20) {

            let label = String.init(format: "$%04X", addr)
            hwSidAddress1.addItem(withTitle: label)
            hwSidAddress2.addItem(withTitle: label)
            hwSidAddress3.addItem(withTitle: label)
            hwSidAddress1.lastItem!.tag = addr
            hwSidAddress2.lastItem!.tag = addr
            hwSidAddress3.lastItem!.tag = addr
        }
    }
    
    func refreshHardwareTab() {
                        
        track()
        
        // VIC
        hwVicModelPopup.selectItem(withTag: config.vicRevision)
        
        switch VICIIRevision.init(rawValue: config.vicRevision) {
            
        case .PAL_6569_R1,
             .PAL_6569_R3,
             .PAL_8565:
            
            hwVicIcon.image = NSImage(named: "pref_vicii_pal")
            
        case .NTSC_6567_R56A:
            
            hwVicIcon.image = NSImage(named: "pref_vicii_ntsc")
            
        case .NTSC_6567,
             .NTSC_8562:
            
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
        hwSidEnable1.state = config.sidEnable1 ? .on : .off
        hwSidEnable2.state = config.sidEnable2 ? .on : .off
        hwSidEnable3.state = config.sidEnable3 ? .on : .off
        hwSidAddress1.selectItem(withTag: config.sidAddress1)
        hwSidAddress2.selectItem(withTag: config.sidAddress2)
        hwSidAddress3.selectItem(withTag: config.sidAddress3)
        
        // Logic board
        hwGlueLogicPopup.selectItem(withTag: config.glueLogic)

        // Startup
        hwRamPatternPopup.selectItem(withTag: config.ramPattern)

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

    @IBAction func hwSidEnable(_ sender: NSButton!) {
        
        track()
        
        switch sender.tag {
        case 1: config.sidEnable1 = sender.state == .on
        case 2: config.sidEnable2 = sender.state == .on
        case 3: config.sidEnable3 = sender.state == .on
        default: fatalError()
        }
        refresh()
    }

    @IBAction func hwSidAddressAction(_ sender: NSPopUpButton!) {
        
        track()
        
        switch sender.tag {
        case 1: config.sidAddress1 = sender.selectedTag()
        case 2: config.sidAddress2 = sender.selectedTag()
        case 3: config.sidAddress3 = sender.selectedTag()
        default: fatalError()
        }
        refresh()
    }
            
    @IBAction func hwGlueLogicAction(_ sender: NSPopUpButton!) {
        
        config.glueLogic = sender.selectedTag()
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
