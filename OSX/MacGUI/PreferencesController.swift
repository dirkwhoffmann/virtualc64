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
    
    let romImage = NSImage.init(named: "rom")
    let romImageLight = NSImage.init(named: "rom_light")
    
    @IBOutlet weak var romHeaderImage: NSImageView!
    @IBOutlet weak var romHeaderText: NSTextField!
    @IBOutlet weak var romHeaderSubText: NSTextField!
    
    @IBOutlet weak var romBasicImage: NSImageView!
    @IBOutlet weak var romBasicDragText: NSTextField!
    @IBOutlet weak var romBasicHashText: NSTextField!
    @IBOutlet weak var romBasicPathText: NSTextField!
    @IBOutlet weak var romBasicDescription: NSTextField!
    @IBOutlet weak var romBasicButton: NSButton!
    
    @IBOutlet weak var romKernalImage: NSImageView!
    @IBOutlet weak var romKernalDragText: NSTextField!
    @IBOutlet weak var romKernalHashText: NSTextField!
    @IBOutlet weak var romKernalPathText: NSTextField!
    @IBOutlet weak var romKernalDescription: NSTextField!
    @IBOutlet weak var romKernelButton: NSButton!
    
    @IBOutlet weak var romCharImage: NSImageView!
    @IBOutlet weak var romCharDragText: NSTextField!
    @IBOutlet weak var romCharHashText: NSTextField!
    @IBOutlet weak var romCharPathText: NSTextField!
    @IBOutlet weak var romCharDescription: NSTextField!
    @IBOutlet weak var romCharButton: NSButton!
    
    @IBOutlet weak var romVc1541Image: NSImageView!
    @IBOutlet weak var romVc1541DragText: NSTextField!
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
        
        refresh()
    }
    
    override func refresh() {
        
        if let id = prefTabView.selectedTabViewItem?.identifier as? String {
            
            track("\(id)")
            
            switch id {
            case "Roms": refreshRomTab()
            case "Hardware": track()
            case "Video": track()
            case "Emulator": track()
            case "Devices": track()
            case "Keyboard": track()
            default: break
            }
        }
    }
    
    func refreshAll() {
        
        refreshRomTab()
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
