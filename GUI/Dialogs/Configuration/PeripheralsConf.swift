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

        let enable8 = config.drive8Connected && !config.drive8AutoConf
        let enable9 = config.drive9Connected && !config.drive9AutoConf
            
        // Drive
        perDrive8Connect.state = config.drive8Connected ? .on : .off
        perDrive8Config.selectItem(withTag: config.drive8AutoConf ? 0 : 1)
        perDrive8Type.selectItem(withTag: config.drive8Type)
        perDrive8Ram.selectItem(withTag: config.drive8Ram)
        perDrive8Cable.selectItem(withTag: config.drive8ParCable)
        perDrive8Config.isEnabled = config.drive8Connected
        perDrive8Type.isEnabled = enable8
        perDrive8Ram.isEnabled = enable8
        perDrive8Cable.isEnabled = enable8

        perDrive9Connect.state = config.drive9Connected ? .on : .off
        perDrive9Config.selectItem(withTag: config.drive9AutoConf ? 0 : 1)
        perDrive9Type.selectItem(withTag: config.drive9Type)
        perDrive9Ram.selectItem(withTag: config.drive9Ram)
        perDrive9Cable.selectItem(withTag: config.drive9ParCable)
        perDrive9Config.isEnabled = config.drive9Connected
        perDrive9Type.isEnabled = enable9
        perDrive9Ram.isEnabled = enable9
        perDrive9Cable.isEnabled = enable9

        // Ports
        parent.gamePadManager.refresh(popup: perControlPort1, hide: true)
        parent.gamePadManager.refresh(popup: perControlPort2, hide: true)
        perControlPort1.selectItem(withTag: config.gameDevice1)
        perControlPort2.selectItem(withTag: config.gameDevice2)

        // Mouse
        perMouseModel.selectItem(withTag: config.mouseModel)

        // Power button
        perPowerButton.isHidden = !bootable
    }
    
    @IBAction func perDriveConnectAction(_ sender: NSButton!) {
        
        switch sender.tag {
        case 8: config.drive8Connected = sender.state == .on
        case 9: config.drive9Connected = sender.state == .on
        default: fatalError()
        }
        refresh()
    }

    @IBAction func perDriveConfigAction(_ sender: NSPopUpButton!) {
        
        switch sender.tag {
        case 8: config.drive8AutoConf = sender.selectedTag() == 0
        case 9: config.drive9AutoConf = sender.selectedTag() == 0
        default: fatalError()
        }
        refresh()
    }

    @IBAction func perDriveTypeAction(_ sender: NSPopUpButton!) {
        
        switch sender.tag {
        case 8: config.drive8Type = sender.selectedTag()
        case 9: config.drive9Type = sender.selectedTag()
        default: fatalError()
        }
        refresh()
    }

    @IBAction func perDriveRamAction(_ sender: NSPopUpButton!) {
        
        switch sender.tag {
        case 8: config.drive8Ram = sender.selectedTag()
        case 9: config.drive9Ram = sender.selectedTag()
        default: fatalError()
        }
        refresh()
    }

    @IBAction func perDriveCableAction(_ sender: NSPopUpButton!) {
        
        switch sender.tag {
        case 8: config.drive8ParCable = sender.selectedTag()
        case 9: config.drive9ParCable = sender.selectedTag()
        default: fatalError()
        }
        refresh()
    }

    @IBAction func perControlPortAction(_ sender: NSPopUpButton!) {
                
        switch sender.tag {
        case 1: config.gameDevice1 = sender.selectedTag()
        case 2: config.gameDevice2 = sender.selectedTag()
        default: fatalError()
        }
        refresh()
    }
    
    @IBAction func perMouseModelAction(_ sender: NSPopUpButton!) {
        
        config.mouseModel = sender.selectedTag()
        refresh()
    }
    
    @IBAction func perPresetAction(_ sender: NSPopUpButton!) {

        c64.suspend()

        // Revert to standard settings
        C64Proxy.defaults.removePeripheralsUserDefaults()

        // Update the configuration
        config.applyPeripheralsUserDefaults()

        c64.resume()
        refresh()
    }

    @IBAction func perDefaultsAction(_ sender: NSButton!) {
        
        track()
        config.savePeripheralsUserDefaults()
    }
    
}
