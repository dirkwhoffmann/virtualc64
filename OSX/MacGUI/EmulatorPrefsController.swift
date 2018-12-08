//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class EmulatorPrefsController : UserDialogController {
    
    // VC1541
    @IBOutlet weak var warpLoad: NSButton!
    @IBOutlet weak var driveNoise: NSButton!
    
    // Screenshots
    @IBOutlet weak var screenshotResolutionPopup: NSPopUpButton!
    @IBOutlet weak var screenshotFormatPopup: NSPopUpButton!

    // User Dialogs
    @IBOutlet weak var autoMountButton: NSButton!
    @IBOutlet weak var closeWithoutAskingButton: NSButton!
    @IBOutlet weak var ejectWithoutAskingButton: NSButton!

    // Misc
    @IBOutlet weak var pauseInBackground: NSButton!
    @IBOutlet weak var autoSnapshots: NSButton!
    @IBOutlet weak var snapshotInterval: NSTextField!
    
    override func awakeFromNib() {
        
        update()
    }
    
    func update() {

        // VC1541
        warpLoad.state = c64.warpLoad() ? .on : .off
        driveNoise.state = c64.drive1.sendSoundMessages() ? .on : .off
        
        // Screenshots
        screenshotResolutionPopup.selectItem(withTag: parent.screenshotResolution)
        screenshotFormatPopup.selectItem(withTag: parent.screenshotFormatIntValue)

        // Documents
        autoMountButton.state = parent.autoMount ? .on : .off
        closeWithoutAskingButton.state = parent.closeWithoutAsking ? .on : .off
        ejectWithoutAskingButton.state = parent.ejectWithoutAsking ? .on : .off
        
        // Miscellaneous
        pauseInBackground.state = parent.pauseInBackground ? .on : .off
        autoSnapshots.state = (c64.snapshotInterval() > 0) ? .on : .off
        snapshotInterval.integerValue = Int(c64.snapshotInterval().magnitude)
        snapshotInterval.isEnabled = (c64.snapshotInterval() > 0)
    }
    
 
    //
    // Action methods (VC1541)
    //
    
    @IBAction func warpLoadAction(_ sender: NSButton!) {
        
        c64.setWarpLoad(sender.state == .on)
        update()
    }
    
    @IBAction func driveNoiseAction(_ sender: NSButton!) {
        
        c64.drive1.setSendSoundMessages(sender.state == .on)
        c64.drive2.setSendSoundMessages(sender.state == .on)
        update()
    }
    

    //
    // Action methods (Screenshots)
    //
    
    @IBAction func screenshotResolutionAction(_ sender: NSMenuItem!) {
        
        parent.screenshotResolution = sender.tag
        update()
    }

    @IBAction func screenshotFormatAction(_ sender: NSMenuItem!) {
        
        parent.screenshotFormatIntValue = sender.tag
        update()
    }

    
    //
    // Action methods (User Dialogs)
    //

    @IBAction func autoMountAction(_ sender: NSButton!) {
        
        parent.autoMount = (sender.state == .on)
        update()
    }

    @IBAction func closeWithoutAskingAction(_ sender: NSButton!) {
        
        parent.closeWithoutAsking = (sender.state == .on)
        update()
    }
    
    @IBAction func ejectWithoutAskingAction(_ sender: NSButton!) {
        
        parent.ejectWithoutAsking = (sender.state == .on)
        update()
    }
    
    
    //
    // Action methods (Miscellaneous)
    //

    @IBAction func pauseInBackgroundAction(_ sender: NSButton!) {
        
        parent.pauseInBackground =  (sender.state == .on)
        update()
    }

    @IBAction func autoSnapshotAction(_ sender: NSButton!) {
        
        if sender.state == .on {
            c64.enableAutoSnapshots()
        } else {
            c64.disableAutoSnapshots()
        }
        update()
    }

    @IBAction func snapshotIntervalAction(_ sender: NSTextField!) {
        
        c64.setSnapshotInterval(sender.integerValue)
        update()
    }
    
    
    //
    // Action methods (Misc)
    //
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        parent.loadEmulatorUserDefaults()
        hideSheet()
    }
    
    @IBAction func factorySettingsAction(_ sender: Any!) {
        
        // VC1541
        c64.setWarpLoad(Defaults.warpLoad)
        c64.drive1.setSendSoundMessages(Defaults.driveNoise)
        c64.drive2.setSendSoundMessages(Defaults.driveNoise)
        
        // Screenshots
        parent.screenshotResolution = Defaults.screenshotResolution
        parent.screenshotFormat = Defaults.screenshotFormat
        
        // User Dialogs
        parent.autoMount = Defaults.autoMount
        parent.closeWithoutAsking = Defaults.closeWithoutAsking
        parent.ejectWithoutAsking = Defaults.ejectWithoutAsking

        // Misc
        parent.pauseInBackground = Defaults.pauseInBackground
        c64.setSnapshotInterval(Defaults.snapshotInterval)

        update()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        parent.saveEmulatorUserDefaults()
        
        if parent.closeWithoutAsking {
            parent.needsSaving = false
        } else if c64.isRunning() {
            parent.needsSaving = true
        }
        
        hideSheet()
    }
}

