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

            let label = String(format: "$%04X", addr)
            hwSidAddress1.addItem(withTitle: label)
            hwSidAddress2.addItem(withTitle: label)
            hwSidAddress3.addItem(withTitle: label)
            hwSidAddress1.lastItem!.tag = addr
            hwSidAddress2.lastItem!.tag = addr
            hwSidAddress3.lastItem!.tag = addr
        }
    }
    
    func refreshHardwareTab() {
                                
        // VIC
        hwVicModelPopup.selectItem(withTag: config.vicRevision)
        
        switch VICIIRevision(rawValue: config.vicRevision) {
            
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
        hwVicSpeedPopup.selectItem(withTag: config.vicSpeed)
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
        
        // Logic board and power supply
        hwGlueLogicPopup.selectItem(withTag: config.glueLogic)
        hwPowerGridPopup.selectItem(withTag: config.powerGrid)

        // Startup
        hwRamPatternPopup.selectItem(withTag: config.ramPattern)

        // Power button
        hwPowerButton.isHidden = !bootable
    }
    
    @IBAction func hwVicRevAction(_ sender: NSPopUpButton!) {
        
        config.vicRevision = sender.selectedTag()
        refresh()
    }

    @IBAction func hwVicSpeedAction(_ sender: NSPopUpButton!) {
        
        config.vicSpeed = sender.selectedTag()
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

        switch sender.tag {
        case 1: config.sidEnable1 = sender.state == .on
        case 2: config.sidEnable2 = sender.state == .on
        case 3: config.sidEnable3 = sender.state == .on
        default: fatalError()
        }
        refresh()
    }

    @IBAction func hwSidAddressAction(_ sender: NSPopUpButton!) {

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

    @IBAction func hwPowerGridAction(_ sender: NSPopUpButton!) {
        
        config.powerGrid = sender.selectedTag()
        refresh()
    }

    @IBAction func hwRamPatternAction(_ sender: NSPopUpButton!) {
        
        config.ramPattern = sender.selectedTag()
        refresh()
    }

    @IBAction func hwPresetAction(_ sender: NSPopUpButton!) {

        c64.suspend()

        // Revert to standard settings
        C64Proxy.defaults.removePeripheralsUserDefaults()

        // Update the configuration
        config.applyPeripheralsUserDefaults()

        // Override some options
        switch sender.selectedTag() {

        case 0: // C64_PAL
            config.vicRevision = VICIIRevision.PAL_6569_R3.rawValue
            config.vicGrayDotBug = false
            config.ciaRevision = CIARevision.MOS_6526.rawValue
            config.ciaTimerBBug = true
            config.sidRevision = SIDRevision.MOS_6581.rawValue
            config.glueLogic = GlueLogic.DISCRETE.rawValue
            config.powerGrid = PowerGrid.STABLE_50HZ.rawValue

        case 1: // C64_II_PAL
            config.vicRevision = VICIIRevision.PAL_8565.rawValue
            config.vicGrayDotBug = true
            config.ciaRevision = CIARevision.MOS_8521.rawValue
            config.ciaTimerBBug = false
            config.sidRevision = SIDRevision.MOS_8580.rawValue
            config.glueLogic = GlueLogic.IC.rawValue
            config.powerGrid = PowerGrid.STABLE_50HZ.rawValue

        case 2: // C64_OLD_PAL
            config.vicRevision = VICIIRevision.PAL_6569_R1.rawValue
            config.vicGrayDotBug = false
            config.ciaRevision = CIARevision.MOS_6526.rawValue
            config.ciaTimerBBug = true
            config.sidRevision = SIDRevision.MOS_6581.rawValue
            config.glueLogic = GlueLogic.DISCRETE.rawValue
            config.powerGrid = PowerGrid.STABLE_50HZ.rawValue

        case 3: // C64_NTSC
            config.vicRevision = VICIIRevision.NTSC_6567.rawValue
            config.vicGrayDotBug = false
            config.ciaRevision = CIARevision.MOS_6526.rawValue
            config.ciaTimerBBug = false
            config.sidRevision = SIDRevision.MOS_6581.rawValue
            config.glueLogic = GlueLogic.DISCRETE.rawValue
            config.powerGrid = PowerGrid.STABLE_60HZ.rawValue

        case 4: // C64_II_NTSC
            config.vicRevision = VICIIRevision.NTSC_8562.rawValue
            config.vicGrayDotBug = true
            config.ciaRevision = CIARevision.MOS_8521.rawValue
            config.ciaTimerBBug = true
            config.sidRevision = SIDRevision.MOS_8580.rawValue
            config.glueLogic = GlueLogic.IC.rawValue
            config.powerGrid = PowerGrid.STABLE_60HZ.rawValue

        case 5: // C64_OLD_NTSC
            config.vicRevision = VICIIRevision.NTSC_6567_R56A.rawValue
            config.vicGrayDotBug = false
            config.ciaRevision = CIARevision.MOS_6526.rawValue
            config.ciaTimerBBug = false
            config.sidRevision = SIDRevision.MOS_6581.rawValue
            config.glueLogic = GlueLogic.DISCRETE.rawValue
            config.powerGrid = PowerGrid.STABLE_60HZ.rawValue

        default:
            fatalError()
        }

        c64.resume()
        refresh()
    }

    @IBAction func hwDefaultsAction(_ sender: NSButton!) {
        
        config.saveHardwareUserDefaults()
    }
}
