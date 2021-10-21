// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {

    func refreshCompatibilityTab() {
                                
        // Power saving
        comDrivePowerSave.state = config.drivePowerSave ? .on : .off
        comViciiPowerSave.state = config.viciiPowerSave ? .on : .off
        comSidPowerSave.state = config.sidPowerSave ? .on : .off
        
        // Collision detection
        comSsCollisions.state = config.ssCollisions ? .on : .off
        comSbCollisions.state = config.sbCollisions ? .on : .off
        
        // Power button
        comPowerButton.isHidden = !bootable
    }

    @IBAction func comDrivePowerSaveAction(_ sender: NSButton!) {
        
        track()
        config.drivePowerSave = sender.state == .on
        refresh()
    }

    @IBAction func comViciiPowerSaveAction(_ sender: NSButton!) {
        
        track()
        config.viciiPowerSave = sender.state == .on
        refresh()
    }

    @IBAction func comSidPowerSaveAction(_ sender: NSButton!) {
        
        track()
        config.sidPowerSave = sender.state == .on
        refresh()
    }
    
    @IBAction func comSsCollisionsAction(_ sender: NSButton!) {
        
        track()
        config.ssCollisions = sender.state == .on
        refresh()
    }

    @IBAction func comSbCollisionsAction(_ sender: NSButton!) {
        
        track()
        config.sbCollisions = sender.state == .on
        refresh()
    }

    @IBAction func comPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        
        switch sender.selectedTag() {
        case 0: config.loadCompatibilityDefaults(CompatibilityDefaults.std)
        case 1: config.loadCompatibilityDefaults(CompatibilityDefaults.accurate)
        case 2: config.loadCompatibilityDefaults(CompatibilityDefaults.accelerated)
        default: fatalError()
        }
        refresh()
    }
    
    @IBAction func comDefaultsAction(_ sender: NSButton!) {
        
        track()
        config.saveCompatibilityUserDefaults()
    }
}
