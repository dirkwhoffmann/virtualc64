// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension PreferencesController {
    
    func refreshEmulatorTab() {
        
        track()
        
        // Drive
        emuDriveBlankDiskFormat.selectItem(withTag: pref.driveBlankDiskFormatIntValue)
        emuEjectUnasked.state = pref.driveEjectUnasked ? .on : .off
        emuDriveSounds.state = pref.driveSounds ? .on : .off
        emuDriveSoundPan.selectItem(withTag: Int(pref.driveSoundPan))
        emuDriveInsertSound.state = pref.driveInsertSound ? .on : .off
        emuDriveEjectSound.state = pref.driveEjectSound ? .on : .off
        emuDriveHeadSound.state = pref.driveHeadSound ? .on : .off
        emuDriveConnectSound.state = pref.driveConnectSound ? .on : .off
        emuDriveSoundPan.isEnabled = pref.driveSounds
        emuDriveInsertSound.isEnabled = pref.driveSounds
        emuDriveEjectSound.isEnabled = pref.driveSounds
        emuDriveHeadSound.isEnabled = pref.driveSounds
        emuDriveConnectSound.isEnabled = pref.driveSounds
        
        // Fullscreen
        emuAspectRatioButton.state = pref.keepAspectRatio ? .on : .off
        emuExitOnEscButton.state = pref.exitOnEsc ? .on : .off
        
        // Snapshots and Screenshots
        emuAutoSnapshots.state = pref.autoSnapshots ? .on : .off
        emuSnapshotInterval.integerValue = pref.snapshotInterval
        emuSnapshotInterval.isEnabled = pref.autoSnapshots
        emuScreenshotSourcePopup.selectItem(withTag: pref.screenshotSource)
        emuScreenshotTargetPopup.selectItem(withTag: pref.screenshotTargetIntValue)
        
        // Drive
        emuWarpMode.selectItem(withTag: pref.warpModeIntValue)
        
        // Miscellaneous
        emuPauseInBackground.state = pref.pauseInBackground ? .on : .off
        emuCloseWithoutAskingButton.state = pref.closeWithoutAsking ? .on : .off
    }
    
    func selectEmulatorTab() {

        track()
        refreshEmulatorTab()
    }

    //
    // Action methods (Drive)
    //
    
    @IBAction func emuDriveSoundsAction(_ sender: NSButton!) {
        
        pref.driveSounds = sender.state == .on
        refresh()
    }
    
    @IBAction func emuDriveSoundPanAction(_ sender: NSPopUpButton!) {
        
        pref.driveSoundPan = Double(sender.selectedTag())
        refresh()
    }
    
    @IBAction func emuDriveInsertSoundAction(_ sender: NSButton!) {
        
        pref.driveInsertSound = sender.state == .on
        refresh()
    }
    
    @IBAction func emuDriveEjectSoundAction(_ sender: NSButton!) {
        
        pref.driveEjectSound = sender.state == .on
        refresh()
    }
    
    @IBAction func emuDriveHeadSoundAction(_ sender: NSButton!) {
        
        pref.driveHeadSound = sender.state == .on
        refresh()
    }
    
    @IBAction func emuDriveConnectSoundAction(_ sender: NSButton!) {
        
        track()
        pref.driveConnectSound = sender.state == .on
        refresh()
    }
    
    @IBAction func emuBlankDiskFormatAction(_ sender: NSPopUpButton!) {
        
        let tag = sender.selectedTag()
        pref.driveBlankDiskFormatIntValue = tag
        refresh()
    }
    
    //
    // Action methods (Fullscreen)
    //
    
    @IBAction func emuAspectRatioAction(_ sender: NSButton!) {
        
        pref.keepAspectRatio = (sender.state == .on)
        refresh()
    }
    
    @IBAction func emuExitOnEscAction(_ sender: NSButton!) {
        
        pref.exitOnEsc = (sender.state == .on)
        refresh()
    }
    
    //
    // Action methods (Snapshots and screenshots)
    //
    
    @IBAction func emuAutoSnapshotAction(_ sender: NSButton!) {
        
        pref.autoSnapshots = sender.state == .on
        refresh()
    }
    
    @IBAction func emuSnapshotIntervalAction(_ sender: NSTextField!) {
        
        if sender.integerValue > 0 {
            pref.snapshotInterval = sender.integerValue
        }
        refresh()
    }
    
    @IBAction func emuScreenshotSourceAction(_ sender: NSPopUpButton!) {
        
        pref.screenshotSource = sender.selectedTag()
        refresh()
    }
    
    @IBAction func emuScreenshotTargetAction(_ sender: NSPopUpButton!) {
        
        pref.screenshotTargetIntValue = sender.selectedTag()
        refresh()
    }
    
    //
    // Action methods (Warp mode)
    //
    
    @IBAction func emuWarpModeAction(_ sender: NSPopUpButton!) {
        
        pref.warpMode = WarpMode(rawValue: sender.selectedTag())!
        refresh()
    }
    
    //
    // Action methods (Miscellaneous)
    //
    
    @IBAction func emuPauseInBackgroundAction(_ sender: NSButton!) {
        
        pref.pauseInBackground = (sender.state == .on)
        refresh()
    }
    
    @IBAction func emuCloseWithoutAskingAction(_ sender: NSButton!) {
        
        pref.closeWithoutAsking = (sender.state == .on)
        for c in myAppDelegate.controllers {
            c.needsSaving = c.c64.running
        }
        refresh()
    }
    
    @IBAction func emuEjectWithoutAskingAction(_ sender: NSButton!) {
        
        pref.driveEjectUnasked = (sender.state == .on)
        refresh()
    }
    
    //
    // Action methods (Misc)
    //
    
    @IBAction func emuPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        assert(sender.selectedTag() == 0)
        
        UserDefaults.resetGeneralUserDefaults()
        pref.loadGeneralUserDefaults()
        refresh()
    }
}
