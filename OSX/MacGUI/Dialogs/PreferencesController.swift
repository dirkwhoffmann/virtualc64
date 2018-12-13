//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class PreferencesController : UserDialogController {

    @IBOutlet weak var prefTabView: NSTabView!
    
    var opendOnAppLaunch = false
    
    //
    // Rom preferences
    //
    
    @IBOutlet weak var romHeaderImage: NSImageView!
    @IBOutlet weak var romHeaderText: NSTextField!
    @IBOutlet weak var romHeaderSubText: NSTextField!
    
    @IBOutlet weak var romBasicImage: RomDropView!
    @IBOutlet weak var romBasicDragImage: NSImageView!
    @IBOutlet weak var romBasicHashText: NSTextField!
    @IBOutlet weak var romBasicPathText: NSTextField!
    @IBOutlet weak var romBasicDescription: NSTextField!
    @IBOutlet weak var romBasicButton: NSButton!
    
    @IBOutlet weak var romKernalImage: RomDropView!
    @IBOutlet weak var romKernalDragImage: NSImageView!
    @IBOutlet weak var romKernalHashText: NSTextField!
    @IBOutlet weak var romKernalPathText: NSTextField!
    @IBOutlet weak var romKernalDescription: NSTextField!
    @IBOutlet weak var romKernelButton: NSButton!
    
    @IBOutlet weak var romCharImage: RomDropView!
    @IBOutlet weak var romCharDragImage: NSImageView!
    @IBOutlet weak var romCharHashText: NSTextField!
    @IBOutlet weak var romCharPathText: NSTextField!
    @IBOutlet weak var romCharDescription: NSTextField!
    @IBOutlet weak var romCharButton: NSButton!
    
    @IBOutlet weak var romVc1541Image: RomDropView!
    @IBOutlet weak var romVc1541DragImage: NSImageView!
    @IBOutlet weak var romVc1541HashText: NSTextField!
    @IBOutlet weak var romVc1541PathText: NSTextField!
    @IBOutlet weak var romVc1541Description: NSTextField!
    @IBOutlet weak var romVc1541Button: NSButton!
    
    @IBOutlet weak var romOkButton: NSButton!
    @IBOutlet weak var romCancelButton: NSButton!

    override func awakeFromNib() {
        
        // Determine if the dialog was opened on start up. This happens when
        // the emulator can't run because of missing Roms.
        opendOnAppLaunch = !c64.isRunnable()
        
        // If the dialog was opend on start up, we don't want to present the
        // Cancel button to the user.
        romCancelButton.isHidden = opendOnAppLaunch
        
        // Connect outlets of drop views
        romBasicImage.dragImage = romBasicDragImage
        romCharImage.dragImage = romCharDragImage
        romKernalImage.dragImage = romKernalDragImage
        romVc1541Image.dragImage = romVc1541DragImage

        refresh()
    }
    
    override func refresh() {
        
        if let id = prefTabView.selectedTabViewItem?.identifier as? String {
            
            track("\(id)")
            
            switch id {
            case "Roms": refreshRomTab()
            case "Hardware": refreshHardwareTab()
            case "Video": refreshVideoTab()
            case "Emulator": refreshEmulatorTab()
            case "Devices": refreshDevicesTab()
            case "Keyboard": refreshKeyboardTab()
            default: break
            }
        }
    }
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        track()
        
        hideSheet()
        parent.loadRomUserDefaults()
        
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        track()
        
        hideSheet()
        
        if !c64.isRunnable() {
            NSApp.terminate(self)
        }
        
        parent.saveRomUserDefaults()
        
    }
}
