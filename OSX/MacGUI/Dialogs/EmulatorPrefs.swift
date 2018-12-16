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
        emuCloseWithoutAskingButton.state = parent.closeWithoutAsking ? .on : .off
        emuEjectWithoutAskingButton.state = parent.ejectWithoutAsking ? .on : .off
        
        // Miscellaneous
        emuPauseInBackground.state = parent.pauseInBackground ? .on : .off
        emuAutoSnapshots.state = (c64.snapshotInterval() > 0) ? .on : .off
        emuSnapshotInterval.integerValue = Int(c64.snapshotInterval().magnitude)
        emuSnapshotInterval.isEnabled = (c64.snapshotInterval() > 0)
        
        // Media files
        let autoD64Action = parent.autoMountAction["D64"]?.rawValue ?? 0
        emuD64Popup.selectItem(withTag: autoD64Action)
        let autoPrgAction = parent.autoMountAction["PRG"]?.rawValue ?? 0
        emuPrgPopup.selectItem(withTag: autoPrgAction)
        let autoT64Action = parent.autoMountAction["T64"]?.rawValue ?? 0
        emuT64Popup.selectItem(withTag: autoT64Action)
        let autoTapAction = parent.autoMountAction["TAP"]?.rawValue ?? 0
        emuTapPopup.selectItem(withTag: autoTapAction)
        let autoCrtAction = parent.autoMountAction["CRT"]?.rawValue ?? 0
        emuCrtPopup.selectItem(withTag: autoCrtAction)
        
        var autoType : Bool
        autoType = parent.autoType["D64"] ?? false
        emuD64AutoTypeButton.isEnabled = (autoD64Action != 0)
        emuD64AutoTypeButton.intValue = autoType ? 1 : 0
        autoType = parent.autoType["PRG"] ?? false
        emuPrgAutoTypeButton.isEnabled = (autoPrgAction != 0)
        emuPrgAutoTypeButton.intValue = autoType ? 1 : 0
        autoType = parent.autoType["T64"] ?? false
        emuT64AutoTypeButton.isEnabled = (autoT64Action != 0)
        emuT64AutoTypeButton.intValue = autoType ? 1 : 0
        autoType = parent.autoType["TAP"] ?? false
        emuTapAutoTypeButton.isEnabled = (autoTapAction != 0)
        emuTapAutoTypeButton.intValue = autoType ? 1 : 0
        autoType = parent.autoType["CRT"] ?? false
        emuCrtAutoTypeButton.isEnabled = (autoCrtAction != 0)
        emuCrtAutoTypeButton.intValue = autoType ? 1 : 0

        var autoTypeText : String
        autoTypeText = parent.autoTypeText["D64"] ?? ""
        emuD64AutoTypeText.isEnabled = (autoD64Action != 0)
        emuD64AutoTypeText.stringValue = autoTypeText
        autoTypeText = parent.autoTypeText["PRG"] ?? ""
        emuPrgAutoTypeText.isEnabled = (autoPrgAction != 0)
        emuPrgAutoTypeText.stringValue = autoTypeText
        autoTypeText = parent.autoTypeText["T64"] ?? ""
        emuT64AutoTypeText.isEnabled = (autoT64Action != 0)
        emuT64AutoTypeText.stringValue = autoTypeText
        autoTypeText = parent.autoTypeText["TAP"] ?? ""
        emuTapAutoTypeText.isEnabled = (autoTapAction != 0)
        emuTapAutoTypeText.stringValue = autoTypeText
        autoTypeText = parent.autoTypeText["CRT"] ?? ""
        emuCrtAutoTypeText.isEnabled = (autoCrtAction != 0)
        emuCrtAutoTypeText.stringValue = autoTypeText

        // OK button
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
        parent.closeWithoutAsking = Defaults.closeWithoutAsking
        parent.ejectWithoutAsking = Defaults.ejectWithoutAsking
        
        // Misc
        parent.pauseInBackground = Defaults.pauseInBackground
        c64.setSnapshotInterval(Defaults.snapshotInterval)
        
        // Media files
        parent.autoMountAction = Defaults.autoMountAction
        parent.autoType = Defaults.autoType
        parent.autoTypeText = Defaults.autoTypeText
        
        refresh()
    }
    
    //
    // Action methods (Media files)
    //
    
    private func mediaFileType(_ tag: Int) -> String? {
        switch(tag) {
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
            parent.autoMountAction[fileType] = action
            refresh()
        }
        track("\(parent.autoMountAction)")
    }
    
    @IBAction func emuAutoTypeAction(_ sender: NSButton!) {
        
        if let fileType = mediaFileType(sender.tag) {
            parent.autoType[fileType] = (sender.intValue == 0) ? false : true
            refresh()
        }
        track("\(parent.autoType)")
    }

    @IBAction func emuAutoTypeTextAction(_ sender: NSTextField!) {
        
        if let fileType = mediaFileType(sender.tag) {
            parent.autoTypeText[fileType] = sender.stringValue
            // refresh()
        }
    }
}

extension PreferencesController : NSTextFieldDelegate {

    func controlTextDidChange(_ obj: Notification) {

        // Make sure the new text gets processed
        emuAutoTypeTextAction(emuD64AutoTypeText)
        emuAutoTypeTextAction(emuPrgAutoTypeText)
        emuAutoTypeTextAction(emuT64AutoTypeText)
        emuAutoTypeTextAction(emuTapAutoTypeText)
        emuAutoTypeTextAction(emuCrtAutoTypeText)
    }
}
