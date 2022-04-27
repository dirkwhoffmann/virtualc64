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
        comDrivePowerSave.state = config.drive8PowerSave ? .on : .off
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
        config.drive8PowerSave = sender.state == .on
        config.drive9PowerSave = sender.state == .on
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
        
        c64.suspend()

        // Revert to standard settings
        C64Proxy.defaults.removeCompatibilityUserDefaults()

        // Update the configuration
        config.applyCompatibilityUserDefaults()

        // Override some options
        switch sender.selectedTag() {

        case 1: // Accurate

            config.drive8PowerSave = false
            config.drive9PowerSave = false
            config.viciiPowerSave = false
            config.sidPowerSave = false
            config.ssCollisions = true
            config.sbCollisions = true

        case 2: // Accelerated

            config.drive8PowerSave = true
            config.drive9PowerSave = true
            config.viciiPowerSave = true
            config.sidPowerSave = true
            config.ssCollisions = false
            config.sbCollisions = false

        default:
            break
        }

        c64.resume()
        refresh()
    }
    
    @IBAction func comDefaultsAction(_ sender: NSButton!) {
        
        track()
        config.saveCompatibilityUserDefaults()
    }
}
