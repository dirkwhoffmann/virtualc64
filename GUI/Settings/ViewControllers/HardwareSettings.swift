// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class HardwareSettingsViewController: SettingsViewController {

    // VIC
    @IBOutlet weak var vicModelPopup: NSPopUpButton!
    @IBOutlet weak var vicIcon: NSImageView!
    @IBOutlet weak var vicGrayDotBug: NSButton!

    // CIA
    @IBOutlet weak var ciaModelPopup: NSPopUpButton!
    @IBOutlet weak var ciaTimerBBug: NSButton!

    // SID
    @IBOutlet weak var sidModelPopup: NSPopUpButton!
    @IBOutlet weak var sidEnable1: NSButton!
    @IBOutlet weak var sidEnable2: NSButton!
    @IBOutlet weak var sidEnable3: NSButton!
    @IBOutlet weak var sidAddress1: NSPopUpButton!
    @IBOutlet weak var sidAddress2: NSPopUpButton!
    @IBOutlet weak var sidAddress3: NSPopUpButton!

    // Board logic and power supply
    @IBOutlet weak var glueLogicPopup: NSPopUpButton!
    @IBOutlet weak var powerGridPopup: NSPopUpButton!

    // Startup
    @IBOutlet weak var ramPatternPopup: NSPopUpButton!

    override var showLock: Bool { true }

    override func viewDidLoad() {

        log(.lifetime)
        for addr in stride(from: 0xD400, through: 0xD7E0, by: 0x20) {

            let label = String(format: "$%04X", addr)
            sidAddress1.addItem(withTitle: label)
            sidAddress2.addItem(withTitle: label)
            sidAddress3.addItem(withTitle: label)
            sidAddress1.lastItem!.tag = addr
            sidAddress2.lastItem!.tag = addr
            sidAddress3.lastItem!.tag = addr
        }
    }

    override func refresh() {

        super.refresh()
        
        guard let config = config else { return }

        // VIC
        vicModelPopup.selectItem(withTag: config.vicRevision)

        switch vc64.VICIIRev(rawValue: config.vicRevision) {

        case .PAL_6569_R1, .PAL_6569_R3:

            vicIcon.image = NSImage(named: "pref_vicii_pal")
            vicGrayDotBug.isEnabled = false

        case .PAL_8565:

            vicIcon.image = NSImage(named: "pref_vicii_pal")
            vicGrayDotBug.isEnabled = true

        case .NTSC_6567_R56A, .NTSC_6567:

            vicIcon.image = NSImage(named: "pref_vicii_ntsc")
            vicGrayDotBug.isEnabled = false

        case .NTSC_8562:

            vicIcon.image = NSImage(named: "pref_vicii_ntsc")
            vicGrayDotBug.isEnabled = true

        default:
            assert(false)
        }
        vicGrayDotBug.state = config.vicGrayDotBug ? .on : .off

        // CIA
        ciaModelPopup.selectItem(withTag: config.ciaRevision)
        ciaTimerBBug.state = config.ciaTimerBBug ? .on : .off

        // Audio
        sidModelPopup.selectItem(withTag: config.sidRevision)
        sidEnable1.state = config.sidEnable1 ? .on : .off
        sidEnable2.state = config.sidEnable2 ? .on : .off
        sidEnable3.state = config.sidEnable3 ? .on : .off
        sidAddress1.selectItem(withTag: config.sidAddress1)
        sidAddress2.selectItem(withTag: config.sidAddress2)
        sidAddress3.selectItem(withTag: config.sidAddress3)

        // Logic board and power supply
        glueLogicPopup.selectItem(withTag: config.glueLogic)
        powerGridPopup.selectItem(withTag: config.powerGrid)

        // Startup
        ramPatternPopup.selectItem(withTag: config.ramPattern)
    }

    @IBAction func vicRevAction(_ sender: NSPopUpButton!) {

        config?.vicRevision = sender.selectedTag()
    }

    @IBAction func vicGrayDotBugAction(_ sender: NSButton!) {

        config?.vicGrayDotBug = sender.state == .on
    }

    @IBAction func ciaRevAction(_ sender: NSPopUpButton!) {

        config?.ciaRevision = sender.selectedTag()
    }

    @IBAction func ciaTimerBBugAction(_ sender: NSButton!) {

        config?.ciaTimerBBug = sender.state == .on
    }

    @IBAction func sidRevAction(_ sender: NSPopUpButton!) {

        config?.sidRevision = sender.selectedTag()
    }

    @IBAction func sidEnable(_ sender: NSButton!) {

        switch sender.tag {
        case 1: config?.sidEnable1 = sender.state == .on
        case 2: config?.sidEnable2 = sender.state == .on
        case 3: config?.sidEnable3 = sender.state == .on
        default: fatalError()
        }
    }

    @IBAction func sidAddressAction(_ sender: NSPopUpButton!) {

        switch sender.tag {
        case 1: config?.sidAddress1 = sender.selectedTag()
        case 2: config?.sidAddress2 = sender.selectedTag()
        case 3: config?.sidAddress3 = sender.selectedTag()
        default: fatalError()
        }
    }

    @IBAction func glueLogicAction(_ sender: NSPopUpButton!) {

        config?.glueLogic = sender.selectedTag()
    }

    @IBAction func powerGridAction(_ sender: NSPopUpButton!) {

        config?.powerGrid = sender.selectedTag()
    }

    @IBAction func ramPatternAction(_ sender: NSPopUpButton!) {

        config?.ramPattern = sender.selectedTag()
    }

    //
    // Presets and Saving
    //
    
    override func preset(tag: Int) {

        guard let emu = emu else { return }

        emu.suspend()

        // Revert to standard settings
        EmulatorProxy.defaults.removePeripheralsUserDefaults()

        // Update the configuration
        config?.applyPeripheralsUserDefaults()

        // Override some options
        switch tag {

        case 0: // C64_PAL
            config?.vicRevision = vc64.VICIIRev.PAL_6569_R3.rawValue
            config?.vicGrayDotBug = false
            config?.ciaRevision = vc64.CIARev.MOS_6526.rawValue
            config?.ciaTimerBBug = true
            config?.sidRevision = vc64.SIDRevision.MOS_6581.rawValue
            config?.glueLogic = vc64.GlueLogic.DISCRETE.rawValue
            config?.powerGrid = vc64.PowerGrid.STABLE_50HZ.rawValue

        case 1: // C64_II_PAL
            config?.vicRevision = vc64.VICIIRev.PAL_8565.rawValue
            config?.vicGrayDotBug = true
            config?.ciaRevision = vc64.CIARev.MOS_8521.rawValue
            config?.ciaTimerBBug = false
            config?.sidRevision = vc64.SIDRevision.MOS_8580.rawValue
            config?.glueLogic = vc64.GlueLogic.IC.rawValue
            config?.powerGrid = vc64.PowerGrid.STABLE_50HZ.rawValue

        case 2: // C64_OLD_PAL
            config?.vicRevision = vc64.VICIIRev.PAL_6569_R1.rawValue
            config?.vicGrayDotBug = false
            config?.ciaRevision = vc64.CIARev.MOS_6526.rawValue
            config?.ciaTimerBBug = true
            config?.sidRevision = vc64.SIDRevision.MOS_6581.rawValue
            config?.glueLogic = vc64.GlueLogic.DISCRETE.rawValue
            config?.powerGrid = vc64.PowerGrid.STABLE_50HZ.rawValue

        case 3: // C64_NTSC
            config?.vicRevision = vc64.VICIIRev.NTSC_6567.rawValue
            config?.vicGrayDotBug = false
            config?.ciaRevision = vc64.CIARev.MOS_6526.rawValue
            config?.ciaTimerBBug = false
            config?.sidRevision = vc64.SIDRevision.MOS_6581.rawValue
            config?.glueLogic = vc64.GlueLogic.DISCRETE.rawValue
            config?.powerGrid = vc64.PowerGrid.STABLE_60HZ.rawValue

        case 4: // C64_II_NTSC
            config?.vicRevision = vc64.VICIIRev.NTSC_8562.rawValue
            config?.vicGrayDotBug = true
            config?.ciaRevision = vc64.CIARev.MOS_8521.rawValue
            config?.ciaTimerBBug = true
            config?.sidRevision = vc64.SIDRevision.MOS_8580.rawValue
            config?.glueLogic = vc64.GlueLogic.IC.rawValue
            config?.powerGrid = vc64.PowerGrid.STABLE_60HZ.rawValue

        case 5: // C64_OLD_NTSC
            config?.vicRevision = vc64.VICIIRev.NTSC_6567_R56A.rawValue
            config?.vicGrayDotBug = false
            config?.ciaRevision = vc64.CIARev.MOS_6526.rawValue
            config?.ciaTimerBBug = false
            config?.sidRevision = vc64.SIDRevision.MOS_6581.rawValue
            config?.glueLogic = vc64.GlueLogic.DISCRETE.rawValue
            config?.powerGrid = vc64.PowerGrid.STABLE_60HZ.rawValue

        default:
            fatalError()
        }

        emu.resume()
    }

    override func save() {

        config?.saveHardwareUserDefaults()
    }
}
