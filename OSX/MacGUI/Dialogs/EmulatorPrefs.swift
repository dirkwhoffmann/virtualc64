//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension PreferencesController {
    
    func refreshEmulatorTab() {
        
        track()
        
        // VC1541
        emuWarpLoad.state = c64.warpLoad() ? .on : .off
        emuDriveSounds.state = c64.drive1.sendSoundMessages() ? .on : .off
        
        // Screenshots
        emuScreenshotSourcePopup.selectItem(withTag: parent.screenshotSource)
        emuScreenshotTargetPopup.selectItem(withTag: parent.screenshotTargetIntValue)
        
        // Documents
        emuAutoMountButton.state = parent.autoMount ? .on : .off
        emuCloseWithoutAskingButton.state = parent.closeWithoutAsking ? .on : .off
        emuEjectWithoutAskingButton.state = parent.ejectWithoutAsking ? .on : .off
        
        // Miscellaneous
        emuPauseInBackground.state = parent.pauseInBackground ? .on : .off
        emuAutoSnapshots.state = (c64.snapshotInterval() > 0) ? .on : .off
        emuSnapshotInterval.integerValue = Int(c64.snapshotInterval().magnitude)
        emuSnapshotInterval.isEnabled = (c64.snapshotInterval() > 0)
        
        emuOkButton.title = parent.c64.isRunnable() ? "OK" : "Quit"
    }

    //
    // Action methods (VC1541)
    //
    
    @IBAction func emuWarpLoadAction(_ sender: NSButton!) {
        
        c64.setWarpLoad(sender.state == .on)
        refresh()
    }
    
    @IBAction func emuDriveSoundsAction(_ sender: NSButton!) {
        
        c64.drive1.setSendSoundMessages(sender.state == .on)
        c64.drive2.setSendSoundMessages(sender.state == .on)
        refresh()
    }
    
    
    //
    // Action methods (Screenshots)
    //
    
    @IBAction func emuScreenshotSourceAction(_ sender: NSPopUpButton!) {
        
        parent.screenshotSource = sender.selectedTag()
        refresh()
    }
    
    @IBAction func emuScreenshotTargetAction(_ sender: NSPopUpButton!) {
        
        parent.screenshotTargetIntValue = sender.selectedTag()
        refresh()
    }
    
    
    //
    // Action methods (User Dialogs)
    //
    
    @IBAction func emuAutoMountAction(_ sender: NSButton!) {
        
        parent.autoMount = (sender.state == .on)
        refresh()
    }
    
    @IBAction func emuCloseWithoutAskingAction(_ sender: NSButton!) {
        
        parent.closeWithoutAsking = (sender.state == .on)
        refresh()
    }
    
    @IBAction func emuEjectWithoutAskingAction(_ sender: NSButton!) {
        
        parent.ejectWithoutAsking = (sender.state == .on)
        refresh()
    }
    
    
    //
    // Action methods (Miscellaneous)
    //
    
    @IBAction func emuPauseInBackgroundAction(_ sender: NSButton!) {
        
        parent.pauseInBackground =  (sender.state == .on)
        refresh()
    }
    
    @IBAction func emuAutoSnapshotAction(_ sender: NSButton!) {
        
        if sender.state == .on {
            c64.enableAutoSnapshots()
        } else {
            c64.disableAutoSnapshots()
        }
        refresh()
    }
    
    @IBAction func emuSnapshotIntervalAction(_ sender: NSTextField!) {
        
        c64.setSnapshotInterval(sender.integerValue)
        refresh()
    }
    
    
    //
    // Action methods (Misc)
    //
    
    @IBAction func emuFactorySettingsAction(_ sender: Any!) {
        
        // VC1541
        c64.setWarpLoad(Defaults.warpLoad)
        c64.drive1.setSendSoundMessages(Defaults.driveNoise)
        c64.drive2.setSendSoundMessages(Defaults.driveNoise)
        
        // Screenshots
        parent.screenshotSource = Defaults.screenshotSource
        parent.screenshotTarget = Defaults.screenshotTarget
        
        // User Dialogs
        parent.autoMount = Defaults.autoMount
        parent.closeWithoutAsking = Defaults.closeWithoutAsking
        parent.ejectWithoutAsking = Defaults.ejectWithoutAsking
        
        // Misc
        parent.pauseInBackground = Defaults.pauseInBackground
        c64.setSnapshotInterval(Defaults.snapshotInterval)
        
        emuOkButton.title = parent.c64.isRunnable() ? "OK" : "Quit"
        
        refresh()
    }
}
