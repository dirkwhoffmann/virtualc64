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
                                
        // Drive
        perDrive8Connect.state = config.drive8Connected ? .on : .off
        perDrive8AutoConf.state = config.drive8AutoConf ? .on : .off
        perDrive8Type.selectItem(withTag: config.drive8Type)
        perDrive8Ram.selectItem(withTag: config.drive8Ram)
        perDrive8Cable.selectItem(withTag: config.drive8ParCable)
        perDrive8AutoConf.isEnabled = config.drive8Connected
        perDrive8Type.isEnabled = config.drive8Connected
        perDrive8Ram.isEnabled = config.drive8Connected
        perDrive8Cable.isEnabled = config.drive8Connected

        perDrive9Connect.state = config.drive9Connected ? .on : .off
        perDrive9AutoConf.state = config.drive9AutoConf ? .on : .off
        perDrive9Type.selectItem(withTag: config.drive9Type)
        perDrive9Ram.selectItem(withTag: config.drive9Ram)
        perDrive9Cable.selectItem(withTag: config.drive9ParCable)
        perDrive9AutoConf.isEnabled = config.drive9Connected
        perDrive9Type.isEnabled = config.drive9Connected
        perDrive9Ram.isEnabled = config.drive9Connected
        perDrive9Cable.isEnabled = config.drive9Connected

        // Disk
        perBlankDiskFormat.selectItem(withTag: config.blankDiskFormatIntValue)

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

    @IBAction func perDriveAutoConfAction(_ sender: NSButton!) {
        
        switch sender.tag {
        case 8: config.drive8AutoConf = sender.state == .on
        case 9: config.drive9AutoConf = sender.state == .on
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

    @IBAction func perBlankDiskFormatAction(_ sender: NSPopUpButton!) {
        
        let tag = sender.selectedTag()
        config.blankDiskFormatIntValue = tag
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
    
        config.loadPeripheralsDefaults(PeripheralsDefaults.std)
        refresh()
    }
    
    @IBAction func perDefaultsAction(_ sender: NSButton!) {
        
        track()
        config.savePeripheralsUserDefaults()
    }
    
}
