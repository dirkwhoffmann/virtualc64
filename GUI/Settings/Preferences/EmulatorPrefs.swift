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
                
        // VC1541
        emuWarpLoad.state = c64.warpLoad() ? .on : .off
        emuDriveSounds.state = myAppDelegate.pref.driveSounds ? .on : .off
        
        // Screenshots
        emuScreenshotSourcePopup.selectItem(withTag: pref.screenshotSource)
        emuScreenshotTargetPopup.selectItem(withTag: pref.screenshotTargetIntValue)
        
        // Documents
        emuCloseWithoutAskingButton.state = pref.closeWithoutAsking ? .on : .off
        emuEjectWithoutAskingButton.state = pref.driveEjectUnasked ? .on : .off
        
        // Miscellaneous
        emuPauseInBackground.state = pref.pauseInBackground ? .on : .off
        emuAutoSnapshots.state = c64.takeAutoSnapshots() ? .on : .off
        emuSnapshotInterval.integerValue = c64.snapshotInterval()
        emuSnapshotInterval.isEnabled = c64.takeAutoSnapshots()
    }

    //
    // Action methods (VC1541)
    //
    
    @IBAction func emuWarpLoadAction(_ sender: NSButton!) {
        
        proxy?.setWarpLoad(sender.state == .on)
        refresh()
    }
    
    @IBAction func emuDriveSoundsAction(_ sender: NSButton!) {
        
        myAppDelegate.pref.driveSounds = sender.state == .on
        refresh()
    }

    //
    // Action methods (Screenshots)
    //
    
    @IBAction func emuScreenshotSourceAction(_ sender: NSPopUpButton!) {
        
        pref.screenshotSource = sender.selectedTag()
        refresh()
    }
    
    @IBAction func emuScreenshotTargetAction(_ sender: NSPopUpButton!) {
        
        pref.screenshotTargetIntValue = sender.selectedTag()
        refresh()
    }

    //
    // Action methods (User Dialogs)
    //
    
    @IBAction func emuCloseWithoutAskingAction(_ sender: NSButton!) {
        
        pref.closeWithoutAsking = (sender.state == .on)
        refresh()
    }
    
    @IBAction func emuEjectWithoutAskingAction(_ sender: NSButton!) {
        
        pref.driveEjectUnasked = (sender.state == .on)
        refresh()
    }

    //
    // Action methods (Miscellaneous)
    //
    
    @IBAction func emuPauseInBackgroundAction(_ sender: NSButton!) {
        
        pref.pauseInBackground = (sender.state == .on)
        refresh()
    }
    
    @IBAction func emuAutoSnapshotAction(_ sender: NSButton!) {
        
        proxy?.setTakeAutoSnapshots(sender.state == .on)
        refresh()
    }
    
    @IBAction func emuSnapshotIntervalAction(_ sender: NSTextField!) {
        
        track("\(sender.integerValue)")
        if sender.integerValue > 0 {
            proxy?.setSnapshotInterval(sender.integerValue)
        } else {
            track("IGNORING")
        }
        refresh()
    }

    //
    // Action methods (Media files)
    //
    
    private func mediaFileType(_ tag: Int) -> String? {
        switch tag {
        case 0: return "D64"
        case 1: return "PRG"
        case 2: return "T64"
        case 3: return "TAP"
        case 4: return "CRT"
        default: return nil
        }
    }
    
    @IBAction func emuAutoMountAction(_ sender: NSPopUpButton!) {
        
        if let fileType = mediaFileType(sender.tag) {
            let action = AutoMountAction(rawValue: sender.selectedTag())
            pref.autoMountAction[fileType] = action
            refresh()
        }
    }
    
    @IBAction func emuAutoTypeAction(_ sender: NSButton!) {
        
        if let fileType = mediaFileType(sender.tag) {
            pref.autoType[fileType] = (sender.intValue == 0) ? false : true
            refresh()
        }
    }

    @IBAction func emuAutoTypeTextAction(_ sender: NSTextField!) {
        
        if let fileType = mediaFileType(sender.tag) {
            pref.autoTypeText[fileType] = sender.stringValue
            refresh()
        }
    }
    
    @IBAction func emuPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        assert(sender.selectedTag() == 0)
        
        UserDefaults.resetEmulatorUserDefaults()
        refresh()
     }
    
    @IBAction func emuDefaultsAction(_ sender: NSButton!) {
        
        track()
    }
}
