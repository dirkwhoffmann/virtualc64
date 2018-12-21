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
        
        guard let c64 = proxy, let controller = myController else { return }
        
        // VC1541
        emuWarpLoad.state = c64.warpLoad() ? .on : .off
        emuDriveSounds.state = c64.drive1.sendSoundMessages() ? .on : .off
        
        // Screenshots
        emuScreenshotSourcePopup.selectItem(withTag: controller.screenshotSource)
        emuScreenshotTargetPopup.selectItem(withTag: controller.screenshotTargetIntValue)
        
        // Documents
        emuCloseWithoutAskingButton.state = controller.closeWithoutAsking ? .on : .off
        emuEjectWithoutAskingButton.state = controller.ejectWithoutAsking ? .on : .off
        
        // Miscellaneous
        emuPauseInBackground.state = controller.pauseInBackground ? .on : .off
        emuAutoSnapshots.state = (c64.snapshotInterval() > 0) ? .on : .off
        emuSnapshotInterval.integerValue = Int(c64.snapshotInterval().magnitude)
        emuSnapshotInterval.isEnabled = (c64.snapshotInterval() > 0)
        
        // Media files
        let autoD64Action = controller.autoMountAction["D64"]?.rawValue ?? 0
        emuD64Popup.selectItem(withTag: autoD64Action)
        let autoPrgAction = controller.autoMountAction["PRG"]?.rawValue ?? 0
        emuPrgPopup.selectItem(withTag: autoPrgAction)
        let autoT64Action = controller.autoMountAction["T64"]?.rawValue ?? 0
        emuT64Popup.selectItem(withTag: autoT64Action)
        let autoTapAction = controller.autoMountAction["TAP"]?.rawValue ?? 0
        emuTapPopup.selectItem(withTag: autoTapAction)
        let autoCrtAction = controller.autoMountAction["CRT"]?.rawValue ?? 0
        emuCrtPopup.selectItem(withTag: autoCrtAction)
        
        var autoType : Bool
        autoType = controller.autoType["D64"] ?? false
        emuD64AutoTypeButton.isEnabled = (autoD64Action != 0)
        emuD64AutoTypeButton.intValue = autoType ? 1 : 0
        autoType = controller.autoType["PRG"] ?? false
        emuPrgAutoTypeButton.isEnabled = (autoPrgAction != 0)
        emuPrgAutoTypeButton.intValue = autoType ? 1 : 0
        autoType = controller.autoType["T64"] ?? false
        emuT64AutoTypeButton.isEnabled = (autoT64Action != 0)
        emuT64AutoTypeButton.intValue = autoType ? 1 : 0
        autoType = controller.autoType["TAP"] ?? false
        emuTapAutoTypeButton.isEnabled = (autoTapAction != 0)
        emuTapAutoTypeButton.intValue = autoType ? 1 : 0
        autoType = controller.autoType["CRT"] ?? false
        emuCrtAutoTypeButton.isEnabled = (autoCrtAction != 0)
        emuCrtAutoTypeButton.intValue = autoType ? 1 : 0

        var autoTypeText : String
        autoTypeText = controller.autoTypeText["D64"] ?? ""
        emuD64AutoTypeText.isEnabled = (autoD64Action != 0)
        emuD64AutoTypeText.stringValue = autoTypeText
        autoTypeText = controller.autoTypeText["PRG"] ?? ""
        emuPrgAutoTypeText.isEnabled = (autoPrgAction != 0)
        emuPrgAutoTypeText.stringValue = autoTypeText
        autoTypeText = controller.autoTypeText["T64"] ?? ""
        emuT64AutoTypeText.isEnabled = (autoT64Action != 0)
        emuT64AutoTypeText.stringValue = autoTypeText
        autoTypeText = controller.autoTypeText["TAP"] ?? ""
        emuTapAutoTypeText.isEnabled = (autoTapAction != 0)
        emuTapAutoTypeText.stringValue = autoTypeText
        autoTypeText = controller.autoTypeText["CRT"] ?? ""
        emuCrtAutoTypeText.isEnabled = (autoCrtAction != 0)
        emuCrtAutoTypeText.stringValue = autoTypeText

        // OK button
        emuOkButton.title = controller.c64.isRunnable() ? "OK" : "Quit"
    }

    //
    // Action methods (VC1541)
    //
    
    @IBAction func emuWarpLoadAction(_ sender: NSButton!) {
        
        proxy?.setWarpLoad(sender.state == .on)
        refresh()
    }
    
    @IBAction func emuDriveSoundsAction(_ sender: NSButton!) {
        
        proxy?.drive1.setSendSoundMessages(sender.state == .on)
        proxy?.drive2.setSendSoundMessages(sender.state == .on)
        refresh()
    }
    
    
    //
    // Action methods (Screenshots)
    //
    
    @IBAction func emuScreenshotSourceAction(_ sender: NSPopUpButton!) {
        
        myController?.screenshotSource = sender.selectedTag()
        refresh()
    }
    
    @IBAction func emuScreenshotTargetAction(_ sender: NSPopUpButton!) {
        
        myController?.screenshotTargetIntValue = sender.selectedTag()
        refresh()
    }
    
    
    //
    // Action methods (User Dialogs)
    //
    
    @IBAction func emuCloseWithoutAskingAction(_ sender: NSButton!) {
        
        myController?.closeWithoutAsking = (sender.state == .on)
        refresh()
    }
    
    @IBAction func emuEjectWithoutAskingAction(_ sender: NSButton!) {
        
        myController?.ejectWithoutAsking = (sender.state == .on)
        refresh()
    }
    
    
    //
    // Action methods (Miscellaneous)
    //
    
    @IBAction func emuPauseInBackgroundAction(_ sender: NSButton!) {
        
        myController?.pauseInBackground = (sender.state == .on)
        refresh()
    }
    
    @IBAction func emuAutoSnapshotAction(_ sender: NSButton!) {
        
        if sender.state == .on {
            proxy?.enableAutoSnapshots()
        } else {
            proxy?.disableAutoSnapshots()
        }
        refresh()
    }
    
    @IBAction func emuSnapshotIntervalAction(_ sender: NSTextField!) {
        
        proxy?.setSnapshotInterval(sender.integerValue)
        refresh()
    }
    
    
    //
    // Action methods (Misc)
    //
    
    @IBAction func emuFactorySettingsAction(_ sender: Any!) {
        
        myController?.resetEmulatorUserDefaults()
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
            myController?.autoMountAction[fileType] = action
            refresh()
        }
    }
    
    @IBAction func emuAutoTypeAction(_ sender: NSButton!) {
        
        if let fileType = mediaFileType(sender.tag) {
            myController?.autoType[fileType] = (sender.intValue == 0) ? false : true
            refresh()
        }
    }

    @IBAction func emuAutoTypeTextAction(_ sender: NSTextField!) {
        
        if let fileType = mediaFileType(sender.tag) {
            myController?.autoTypeText[fileType] = sender.stringValue
            refresh()
        }
    }
}

extension PreferencesController : NSTextFieldDelegate {

    func controlTextDidChange(_ obj: Notification) {
        
        if let view = obj.object as? NSTextField {
            
            let formatter = view.formatter as? NumberFormatter
            
            switch view {
                
            case emuSnapshotInterval:
                
                if let _ = formatter?.number(from: view.stringValue) {
                    emuSnapshotIntervalAction(view)
                }
                
            case emuD64AutoTypeText, emuPrgAutoTypeText, emuT64AutoTypeText,
                 emuTapAutoTypeText, emuCrtAutoTypeText:
                
                emuAutoTypeTextAction(emuD64AutoTypeText)
                
            case devAutofireBullets:
                
                if let _ = formatter?.number(from: view.stringValue) {
                    devAutofireBulletsAction(view)
                }
                
            default:
                break
            }
        }
    }
}
