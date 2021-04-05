// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension PreferencesController {
    
    func refreshGeneralTab() {
        
        track()
        
        // Drive
        genDriveBlankDiskFormat.selectItem(withTag: pref.driveBlankDiskFormatIntValue)
        genEjectUnasked.state = pref.driveEjectUnasked ? .on : .off
        genDriveSounds.state = pref.driveSounds ? .on : .off
        genDriveSoundPan.selectItem(withTag: Int(pref.driveSoundPan))
        genDriveInsertSound.state = pref.driveInsertSound ? .on : .off
        genDriveEjectSound.state = pref.driveEjectSound ? .on : .off
        genDriveHeadSound.state = pref.driveHeadSound ? .on : .off
        genDriveConnectSound.state = pref.driveConnectSound ? .on : .off
        genDriveSoundPan.isEnabled = pref.driveSounds
        genDriveInsertSound.isEnabled = pref.driveSounds
        genDriveEjectSound.isEnabled = pref.driveSounds
        genDriveHeadSound.isEnabled = pref.driveSounds
        genDriveConnectSound.isEnabled = pref.driveSounds
        
        // Fullscreen
        genAspectRatioButton.state = pref.keepAspectRatio ? .on : .off
        genExitOnEscButton.state = pref.exitOnEsc ? .on : .off
        
        // Snapshots and Screenshots
        genAutoSnapshots.state = pref.autoSnapshots ? .on : .off
        genSnapshotInterval.integerValue = pref.snapshotInterval
        genSnapshotInterval.isEnabled = pref.autoSnapshots
        genScreenshotSourcePopup.selectItem(withTag: pref.screenshotSource)
        genScreenshotTargetPopup.selectItem(withTag: pref.screenshotTargetIntValue)
        
        // Drive
        genWarpMode.selectItem(withTag: pref.warpModeIntValue)
        
        // Miscellaneous
        genPauseInBackground.state = pref.pauseInBackground ? .on : .off
        genCloseWithoutAskingButton.state = pref.closeWithoutAsking ? .on : .off
    }
    
    func selectGeneralTab() {

        track()
        refreshGeneralTab()
    }

    //
    // Action methods (Drive)
    //
    
    @IBAction func genDriveSoundsAction(_ sender: NSButton!) {
        
        pref.driveSounds = sender.state == .on
        refresh()
    }
    
    @IBAction func genDriveSoundPanAction(_ sender: NSPopUpButton!) {
        
        pref.driveSoundPan = Double(sender.selectedTag())
        refresh()
    }
    
    @IBAction func genDriveInsertSoundAction(_ sender: NSButton!) {
        
        pref.driveInsertSound = sender.state == .on
        refresh()
    }
    
    @IBAction func genDriveEjectSoundAction(_ sender: NSButton!) {
        
        pref.driveEjectSound = sender.state == .on
        refresh()
    }
    
    @IBAction func genDriveHeadSoundAction(_ sender: NSButton!) {
        
        pref.driveHeadSound = sender.state == .on
        refresh()
    }
    
    @IBAction func genDriveConnectSoundAction(_ sender: NSButton!) {
        
        track()
        pref.driveConnectSound = sender.state == .on
        refresh()
    }
    
    @IBAction func genBlankDiskFormatAction(_ sender: NSPopUpButton!) {
        
        let tag = sender.selectedTag()
        pref.driveBlankDiskFormatIntValue = tag
        refresh()
    }
    
    //
    // Action methods (Fullscreen)
    //
    
    @IBAction func genAspectRatioAction(_ sender: NSButton!) {
        
        pref.keepAspectRatio = (sender.state == .on)
        refresh()
    }
    
    @IBAction func genExitOnEscAction(_ sender: NSButton!) {
        
        pref.exitOnEsc = (sender.state == .on)
        refresh()
    }
    
    //
    // Action methods (Snapshots and screenshots)
    //
    
    @IBAction func genAutoSnapshotAction(_ sender: NSButton!) {
        
        pref.autoSnapshots = sender.state == .on
        refresh()
    }
    
    @IBAction func genSnapshotIntervalAction(_ sender: NSTextField!) {
        
        if sender.integerValue > 0 {
            pref.snapshotInterval = sender.integerValue
        }
        refresh()
    }
    
    @IBAction func genScreenshotSourceAction(_ sender: NSPopUpButton!) {
        
        pref.screenshotSource = sender.selectedTag()
        refresh()
    }
    
    @IBAction func genScreenshotTargetAction(_ sender: NSPopUpButton!) {
        
        pref.screenshotTargetIntValue = sender.selectedTag()
        refresh()
    }
    
    //
    // Action methods (Warp mode)
    //
    
    @IBAction func genWarpModeAction(_ sender: NSPopUpButton!) {
        
        pref.warpMode = WarpMode(rawValue: sender.selectedTag())!
        refresh()
    }
    
    //
    // Action methods (Miscellaneous)
    //
    
    @IBAction func genPauseInBackgroundAction(_ sender: NSButton!) {
        
        pref.pauseInBackground = (sender.state == .on)
        refresh()
    }
    
    @IBAction func genCloseWithoutAskingAction(_ sender: NSButton!) {
        
        pref.closeWithoutAsking = (sender.state == .on)
        for c in myAppDelegate.controllers {
            c.needsSaving = c.c64.running
        }
        refresh()
    }
    
    @IBAction func genEjectWithoutAskingAction(_ sender: NSButton!) {
        
        pref.driveEjectUnasked = (sender.state == .on)
        refresh()
    }
    
    //
    // Action methods (Misc)
    //
    
    @IBAction func genPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        assert(sender.selectedTag() == 0)
        
        UserDefaults.resetGeneralUserDefaults()
        pref.loadGeneralUserDefaults()
        refresh()
    }
}
