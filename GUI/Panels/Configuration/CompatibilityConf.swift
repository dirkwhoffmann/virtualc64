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

        // Frame rate
        let fps = c64.refreshRate
        comSyncMode.selectItem(withTag: config.syncMode)
        comAutoFps.state = config.autoFps ? .on : .off
        comFpsSlider.integerValue = fps
        comFpsInfo.stringValue = "\(fps) frames per second"
        comSliceSlider.integerValue = config.timeSlices
        comSliceInfo.stringValue = "\(config.timeSlices) time slices per frame"
        comAutoFps.isEnabled = config.syncMode != 1
        comFpsSlider.isEnabled = config.syncMode != 1 && !config.autoFps
        comFpsInfo.isEnabled = config.syncMode != 1

        // Power button
        comPowerButton.isHidden = !bootable
    }

    //
    // Action methods (power saving)
    //

    @IBAction func comDrivePowerSaveAction(_ sender: NSButton!) {

        config.drive8PowerSave = sender.state == .on
        config.drive9PowerSave = sender.state == .on
        refresh()
    }

    @IBAction func comViciiPowerSaveAction(_ sender: NSButton!) {
        
        config.viciiPowerSave = sender.state == .on
        refresh()
    }

    @IBAction func comSidPowerSaveAction(_ sender: NSButton!) {
        
        config.sidPowerSave = sender.state == .on
        refresh()
    }
    
    //
    // Action methods (collision detection)
    //

    @IBAction func comSsCollisionsAction(_ sender: NSButton!) {
        
        config.ssCollisions = sender.state == .on
        refresh()
    }

    @IBAction func comSbCollisionsAction(_ sender: NSButton!) {
        
        config.sbCollisions = sender.state == .on
        refresh()
    }

    //
    // Action methods (warp)
    //

    @IBAction func comWarpModeAction(_ sender: NSPopUpButton!) {

        config.warpMode = sender.selectedTag()
        refresh()
    }

    @IBAction func comWarpBootAction(_ sender: NSTextField!) {

        config.warpBoot = sender.integerValue
        refresh()
    }

    //
    // Action methods (threading)
    //

    @IBAction func comSyncModeAction(_ sender: NSPopUpButton!) {

        config.syncMode = sender.selectedTag()
        refresh()
    }

    @IBAction func comTimeSlicesAction(_ sender: NSSlider!) {

        config.timeSlices = sender.integerValue
        refresh()
    }

    @IBAction func comAutoFpsAction(_ sender: NSButton!) {

        config.autoFps = sender.state == .on
        refresh()
    }

    @IBAction func comFpsAction(_ sender: NSSlider!) {

        config.proposedFps = sender.integerValue
        refresh()
    }

    @IBAction func comPresetAction(_ sender: NSPopUpButton!) {
        
        c64.suspend()

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

        c64.resume()
        refresh()
    }
    
    @IBAction func comDefaultsAction(_ sender: NSButton!) {
        
        config.savePerformanceUserDefaults()
    }
}
