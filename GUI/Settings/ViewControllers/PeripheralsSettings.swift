/// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class PeripheralsSettingsViewController: SettingsViewController {

    // Drive
    @IBOutlet weak var drive8Connect: NSButton!
    @IBOutlet weak var drive8Config: NSPopUpButton!
    @IBOutlet weak var drive8Type: NSPopUpButton!
    @IBOutlet weak var drive8Ram: NSPopUpButton!
    @IBOutlet weak var drive8Cable: NSPopUpButton!
    @IBOutlet weak var drive9Connect: NSButton!
    @IBOutlet weak var drive9Config: NSPopUpButton!
    @IBOutlet weak var drive9Type: NSPopUpButton!
    @IBOutlet weak var drive9Ram: NSPopUpButton!
    @IBOutlet weak var drive9Cable: NSPopUpButton!

    // Parallel cable
    @IBOutlet weak var parCableType: NSPopUpButton!

    // Datasette
    @IBOutlet weak var datasetteConnect: NSButton!
    @IBOutlet weak var datasetteModel: NSPopUpButton!

    // Ports
    @IBOutlet weak var controlPort1: NSPopUpButton!
    @IBOutlet weak var controlPort2: NSPopUpButton!

    // Mouse
    @IBOutlet weak var mouseModel: NSPopUpButton!
    @IBOutlet weak var paddleOrientation: NSPopUpButton!
    @IBOutlet weak var paddleOrientationText: NSTextField!

    // Joystick
    @IBOutlet weak var autofire: NSButton!
    @IBOutlet weak var autofireText: NSTextField!
    @IBOutlet weak var autofireFrequency: NSSlider!
    @IBOutlet weak var autofireFrequencyText1: NSTextField!
    @IBOutlet weak var autofireFrequencyText2: NSTextField!
    @IBOutlet weak var autofireCease: NSButton!
    @IBOutlet weak var autofireCeaseText: NSTextField!
    @IBOutlet weak var autofireBullets: NSTextField!
    @IBOutlet weak var autofireBulletsText: NSTextField!

    override var showLock: Bool { true }

    override func viewDidLoad() {

        log(.lifetime)
    }

    override func refresh() {

        super.refresh()
        
        guard let config = config else { return }

        func update(_ component: NSTextField, enable: Bool) {
            component.textColor = enable ? .controlTextColor : .disabledControlTextColor
            component.isEnabled = enable
        }
        func update(_ component: NSControl, enable: Bool) {
            component.isEnabled = enable
        }

        let enable8 = config.drive8Connected && !config.drive8AutoConf
        let enable9 = config.drive9Connected && !config.drive9AutoConf

        // First drive
        drive8Connect.state = config.drive8Connected ? .on : .off
        drive8Config.selectItem(withTag: config.drive8AutoConf ? 0 : 1)
        drive8Type.selectItem(withTag: config.drive8Type)
        drive8Ram.selectItem(withTag: config.drive8Ram)
        drive8Cable.selectItem(withTag: config.drive8ParCable)
        drive8Config.isEnabled = config.drive8Connected
        drive8Type.isEnabled = enable8
        drive8Ram.isEnabled = enable8
        drive8Cable.isEnabled = enable8

        // Second drive
        drive9Connect.state = config.drive9Connected ? .on : .off
        drive9Config.selectItem(withTag: config.drive9AutoConf ? 0 : 1)
        drive9Type.selectItem(withTag: config.drive9Type)
        drive9Ram.selectItem(withTag: config.drive9Ram)
        drive9Cable.selectItem(withTag: config.drive9ParCable)
        drive9Config.isEnabled = config.drive9Connected
        drive9Type.isEnabled = enable9
        drive9Ram.isEnabled = enable9
        drive9Cable.isEnabled = enable9

        // Datasette
        datasetteModel.selectItem(withTag: config.datasetteModel)
        datasetteConnect.state = config.datasetteConnected ? .on : .off
        drive9Connect.state = config.drive9Connected ? .on : .off
        drive9Config.selectItem(withTag: config.drive9AutoConf ? 0 : 1)
        drive9Type.selectItem(withTag: config.drive9Type)
        datasetteModel.isEnabled = config.datasetteConnected

        // Ports
        gamePadManager?.refresh(popup: controlPort1, hide: true)
        gamePadManager?.refresh(popup: controlPort2, hide: true)
        controlPort1.selectItem(withTag: config.gameDevice1)
        controlPort2.selectItem(withTag: config.gameDevice2)

        // Mouse
        let paddle = config.mouseModel >= 3
        mouseModel.selectItem(withTag: config.mouseModel)
        paddleOrientation.selectItem(withTag: config.paddleOrientation)
        update(paddleOrientation, enable: paddle)
        update(paddleOrientationText, enable: paddle)

        // Joysticks
        let enable = config.autofire
        autofire.state = enable ? .on : .off
        autofireCease.state = config.autofireBursts ? .on : .off
        autofireBullets.integerValue = config.autofireBullets
        autofireFrequency.integerValue = config.autofireFrequency
        update(autofireFrequency, enable: enable)
        update(autofireFrequencyText1, enable: enable)
        update(autofireFrequencyText2, enable: enable)
        update(autofireCease, enable: enable)
        update(autofireCeaseText, enable: enable)
        update(autofireBullets, enable: enable && autofireCease.state == .on)
        update(autofireBulletsText, enable: enable && autofireCease.state == .on)
    }

    @IBAction func driveConnectAction(_ sender: NSButton!) {

        switch sender.tag {
        case 8: config?.drive8Connected = sender.state == .on
        case 9: config?.drive9Connected = sender.state == .on
        default: fatalError()
        }
    }

    @IBAction func driveConfigAction(_ sender: NSPopUpButton!) {

        switch sender.tag {
        case 8: config?.drive8AutoConf = sender.selectedTag() == 0
        case 9: config?.drive9AutoConf = sender.selectedTag() == 0
        default: fatalError()
        }
    }

    @IBAction func driveTypeAction(_ sender: NSPopUpButton!) {

        switch sender.tag {
        case 8: config?.drive8Type = sender.selectedTag()
        case 9: config?.drive9Type = sender.selectedTag()
        default: fatalError()
        }
    }

    @IBAction func driveRamAction(_ sender: NSPopUpButton!) {

        switch sender.tag {
        case 8: config?.drive8Ram = sender.selectedTag()
        case 9: config?.drive9Ram = sender.selectedTag()
        default: fatalError()
        }
    }

    @IBAction func driveCableAction(_ sender: NSPopUpButton!) {

        switch sender.tag {
        case 8: config?.drive8ParCable = sender.selectedTag()
        case 9: config?.drive9ParCable = sender.selectedTag()
        default: fatalError()
        }
    }

    @IBAction func datasetteConnectAction(_ sender: NSButton!) {

        config?.datasetteConnected = sender.state == .on
    }

    @IBAction func datasetteModelAction(_ sender: NSPopUpButton!) {

        config?.datasetteModel = sender.selectedTag()
    }

    @IBAction func controlPortAction(_ sender: NSPopUpButton!) {

        switch sender.tag {
        case 1: config?.gameDevice1 = sender.selectedTag()
        case 2: config?.gameDevice2 = sender.selectedTag()
        default: fatalError()
        }
    }

    @IBAction func mouseModelAction(_ sender: NSPopUpButton!) {

        config?.mouseModel = sender.selectedTag()
    }

    @IBAction func paddleOrientationAction(_ sender: NSPopUpButton!) {

        config?.paddleOrientation = sender.selectedTag()
    }

    @IBAction func autofireAction(_ sender: NSButton!) {

        config?.autofire = (sender.state == .on)
    }

    @IBAction func autofireCeaseAction(_ sender: NSButton!) {

        config?.autofireBursts = (sender.state == .on)
    }

    @IBAction func autofireBulletsAction(_ sender: NSTextField!) {

        config?.autofireBullets = sender.integerValue
    }

    @IBAction func autofireFrequencyAction(_ sender: NSSlider!) {

        config?.autofireFrequency = sender.integerValue
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

        emu.resume()
    }

    override func save() {

        config?.savePeripheralsUserDefaults()
    }
    
}
