// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {
   
    func refreshPeripheralsTab() {

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
        perDrive8Connect.state = config.drive8Connected ? .on : .off
        perDrive8Config.selectItem(withTag: config.drive8AutoConf ? 0 : 1)
        perDrive8Type.selectItem(withTag: config.drive8Type)
        perDrive8Ram.selectItem(withTag: config.drive8Ram)
        perDrive8Cable.selectItem(withTag: config.drive8ParCable)
        perDrive8Config.isEnabled = config.drive8Connected
        perDrive8Type.isEnabled = enable8
        perDrive8Ram.isEnabled = enable8
        perDrive8Cable.isEnabled = enable8

        // Second drive
        perDrive9Connect.state = config.drive9Connected ? .on : .off
        perDrive9Config.selectItem(withTag: config.drive9AutoConf ? 0 : 1)
        perDrive9Type.selectItem(withTag: config.drive9Type)
        perDrive9Ram.selectItem(withTag: config.drive9Ram)
        perDrive9Cable.selectItem(withTag: config.drive9ParCable)
        perDrive9Config.isEnabled = config.drive9Connected
        perDrive9Type.isEnabled = enable9
        perDrive9Ram.isEnabled = enable9
        perDrive9Cable.isEnabled = enable9

        // Datasette
        perDatasetteModel.selectItem(withTag: config.datasetteModel)
        perDatasetteConnect.state = config.datasetteConnected ? .on : .off
        perDrive9Connect.state = config.drive9Connected ? .on : .off
        perDrive9Config.selectItem(withTag: config.drive9AutoConf ? 0 : 1)
        perDrive9Type.selectItem(withTag: config.drive9Type)
        perDatasetteModel.isEnabled = config.datasetteConnected

        // Ports
        parent.gamePadManager.refresh(popup: perControlPort1, hide: true)
        parent.gamePadManager.refresh(popup: perControlPort2, hide: true)
        perControlPort1.selectItem(withTag: config.gameDevice1)
        perControlPort2.selectItem(withTag: config.gameDevice2)

        // Mouse
        let paddle = config.mouseModel >= 3
        perMouseModel.selectItem(withTag: config.mouseModel)
        perPaddleOrientation.selectItem(withTag: config.paddleOrientation)
        update(perPaddleOrientation, enable: paddle)
        update(perPaddleOrientationText, enable: paddle)

        // Joysticks
        let enable = config.autofire
        perAutofire.state = enable ? .on : .off
        perAutofireCease.state = config.autofireBursts ? .on : .off
        perAutofireBullets.integerValue = config.autofireBullets
        perAutofireFrequency.integerValue = config.autofireFrequency
        update(perAutofireFrequency, enable: enable)
        update(perAutofireFrequencyText1, enable: enable)
        update(perAutofireFrequencyText2, enable: enable)
        update(perAutofireCease, enable: enable)
        update(perAutofireCeaseText, enable: enable)
        update(perAutofireBullets, enable: enable && perAutofireCease.state == .on)
        update(perAutofireBulletsText, enable: enable && perAutofireCease.state == .on)

        // Power button
        perPowerButton.isHidden = !bootable
    }
    
    @IBAction func perDriveConnectAction(_ sender: NSButton!) {
        
        switch sender.tag {
        case 8: config.drive8Connected = sender.state == .on
        case 9: config.drive9Connected = sender.state == .on
        default: fatalError()
        }
    }

    @IBAction func perDriveConfigAction(_ sender: NSPopUpButton!) {
        
        switch sender.tag {
        case 8: config.drive8AutoConf = sender.selectedTag() == 0
        case 9: config.drive9AutoConf = sender.selectedTag() == 0
        default: fatalError()
        }
    }

    @IBAction func perDriveTypeAction(_ sender: NSPopUpButton!) {
        
        switch sender.tag {
        case 8: config.drive8Type = sender.selectedTag()
        case 9: config.drive9Type = sender.selectedTag()
        default: fatalError()
        }
    }

    @IBAction func perDriveRamAction(_ sender: NSPopUpButton!) {
        
        switch sender.tag {
        case 8: config.drive8Ram = sender.selectedTag()
        case 9: config.drive9Ram = sender.selectedTag()
        default: fatalError()
        }
    }

    @IBAction func perDriveCableAction(_ sender: NSPopUpButton!) {
        
        switch sender.tag {
        case 8: config.drive8ParCable = sender.selectedTag()
        case 9: config.drive9ParCable = sender.selectedTag()
        default: fatalError()
        }
    }

    @IBAction func perDatasetteConnectAction(_ sender: NSButton!) {

        config.datasetteConnected = sender.state == .on
    }

    @IBAction func perDatasetteModelAction(_ sender: NSPopUpButton!) {

        config.datasetteModel = sender.selectedTag()
    }

    @IBAction func perControlPortAction(_ sender: NSPopUpButton!) {
                
        switch sender.tag {
        case 1: config.gameDevice1 = sender.selectedTag()
        case 2: config.gameDevice2 = sender.selectedTag()
        default: fatalError()
        }
    }
    
    @IBAction func perMouseModelAction(_ sender: NSPopUpButton!) {
        
        config.mouseModel = sender.selectedTag()
    }

    @IBAction func perPaddleOrientationAction(_ sender: NSPopUpButton!) {

        config.paddleOrientation = sender.selectedTag()
    }

    @IBAction func perAutofireAction(_ sender: NSButton!) {

        config.autofire = (sender.state == .on)
    }

    @IBAction func perAutofireCeaseAction(_ sender: NSButton!) {

        config.autofireBursts = (sender.state == .on)
    }

    @IBAction func perAutofireBulletsAction(_ sender: NSTextField!) {

        config.autofireBullets = sender.integerValue
    }

    @IBAction func perAutofireFrequencyAction(_ sender: NSSlider!) {

        config.autofireFrequency = sender.integerValue
    }

    @IBAction func perPresetAction(_ sender: NSPopUpButton!) {

        if let emu = emu {
            
            emu.suspend()

            // Revert to standard settings
            EmulatorProxy.defaults.removePeripheralsUserDefaults()

            // Update the configuration
            config.applyPeripheralsUserDefaults()

            emu.resume()
        }
    }

    @IBAction func perDefaultsAction(_ sender: NSButton!) {
        
        config.savePeripheralsUserDefaults()
    }
    
}
