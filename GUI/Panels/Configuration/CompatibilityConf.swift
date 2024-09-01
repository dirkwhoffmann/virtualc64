// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {

    func refreshPerformanceTab() {
                                
        // Power saving
        comDrivePowerSave.state = config.drive8PowerSave ? .on : .off
        comViciiPowerSave.state = config.viciiPowerSave ? .on : .off
        comSidPowerSave.state = config.sidPowerSave ? .on : .off
        
        // Collision detection
        comSsCollisions.state = config.ssCollisions ? .on : .off
        comSbCollisions.state = config.sbCollisions ? .on : .off

        // Warp
        comWarpMode.selectItem(withTag: config.warpMode)
        comWarpBoot.integerValue = config.warpBoot

        // Threading
        let speedBoost = config.speedBoost
        let runAhead = config.runAhead
        prfVSync.state = config.vsync ? .on : .off
        prfSpeedBoost.integerValue = speedBoost
        prfSpeedBoostInfo.stringValue = "\(speedBoost) %"
        prfRunAhead.integerValue = runAhead
        prfRunAheadInfo.stringValue = "\(runAhead) frame" + (runAhead == 1 ? "" : "s")
        prfSpeedBoost.isEnabled = !config.vsync
        prfSpeedBoostInfo.textColor = config.vsync ? .tertiaryLabelColor : .labelColor

        // Power button
        comPowerButton.isHidden = !bootable
    }

    //
    // Action methods (power saving)
    //

    @IBAction func comDrivePowerSaveAction(_ sender: NSButton!) {

        config.drive8PowerSave = sender.state == .on
        config.drive9PowerSave = sender.state == .on
    }

    @IBAction func comViciiPowerSaveAction(_ sender: NSButton!) {
        
        config.viciiPowerSave = sender.state == .on
    }

    @IBAction func comSidPowerSaveAction(_ sender: NSButton!) {
        
        config.sidPowerSave = sender.state == .on
    }
    
    //
    // Action methods (collision detection)
    //

    @IBAction func comSsCollisionsAction(_ sender: NSButton!) {
        
        config.ssCollisions = sender.state == .on
    }

    @IBAction func comSbCollisionsAction(_ sender: NSButton!) {
        
        config.sbCollisions = sender.state == .on
    }

    //
    // Action methods (warp)
    //

    @IBAction func comWarpModeAction(_ sender: NSPopUpButton!) {

        config.warpMode = sender.selectedTag()
    }

    @IBAction func comWarpBootAction(_ sender: NSTextField!) {

        config.warpBoot = sender.integerValue
    }

    //
    // Action methods (threading)
    //

    @IBAction func prfVSyncAction(_ sender: NSButton!) {

        config.vsync = sender.state == .on
    }

    @IBAction func prfSpeedBoostAction(_ sender: NSSlider!) {

        config.speedBoost = sender.integerValue
    }

    @IBAction func prfRunAheadAction(_ sender: NSSlider!) {

        config.runAhead = sender.integerValue
    }

    @IBAction func comPresetAction(_ sender: NSPopUpButton!) {
        
        if let emu = emu {
            
            emu.suspend()

            // Revert to standard settings
            EmulatorProxy.defaults.removePerformanceUserDefaults()

            // Update the configuration
            config.applyPerformanceUserDefaults()

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

            emu.resume()
        }
    }
    
    @IBAction func comDefaultsAction(_ sender: NSButton!) {
        
        config.savePerformanceUserDefaults()
    }
}
