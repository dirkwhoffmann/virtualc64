// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MyToolbar: NSToolbar {
    
    var emu: EmulatorProxy? { parent.emu }

    @IBOutlet weak var parent: MyController!

    // References to toolbar items
    @IBOutlet weak var controlPort1Item: NSToolbarItem!
    @IBOutlet weak var controlPort2Item: NSToolbarItem!
    @IBOutlet weak var keyboardItem: NSToolbarItem!
    @IBOutlet weak var preferencesItem: NSToolbarItem!
    @IBOutlet weak var controlsItem: NSToolbarItem!

    // Reference to toolbar item objects
    @IBOutlet weak var controlPort1: NSPopUpButton!
    @IBOutlet weak var controlPort2: NSPopUpButton!
    @IBOutlet weak var snapshotSegCtrl: NSSegmentedControl!
    @IBOutlet weak var screenshotSegCtrl: NSSegmentedControl!
    @IBOutlet weak var controlsSegCtrl: NSSegmentedControl!
    
    override func validateVisibleItems() {
                           
        // Disable the keyboard button if the virtual keyboard is open
        let visible = parent.virtualKeyboard?.window?.isVisible ?? false
        let view = keyboardItem.view as? NSButton
        view?.isEnabled = !visible
        
        // Update input devices
        parent.gamePadManager.refresh(popup: controlPort1)
        parent.gamePadManager.refresh(popup: controlPort2)
        controlPort1.selectItem(withTag: parent.config.gameDevice1)
        controlPort2.selectItem(withTag: parent.config.gameDevice2)

        controlPort1Item.menuFormRepresentation = nil
        controlPort2Item.menuFormRepresentation = nil
        keyboardItem.menuFormRepresentation = nil
        preferencesItem.menuFormRepresentation = nil
        controlsItem.menuFormRepresentation = nil
    }
    
    func updateToolbar() {
        
        if emu != nil {

            if emu!.poweredOn {
                controlsSegCtrl.setEnabled(true, forSegment: 0) // Pause
                controlsSegCtrl.setEnabled(true, forSegment: 1) // Reset
                controlsSegCtrl.setToolTip("Power off", forSegment: 2) // Power
            } else {
                controlsSegCtrl.setEnabled(false, forSegment: 0) // Pause
                controlsSegCtrl.setEnabled(false, forSegment: 1) // Reset
                controlsSegCtrl.setToolTip("Power on", forSegment: 2) // Power
            }
            if emu!.running {
                controlsSegCtrl.setToolTip("Pause", forSegment: 0)
                controlsSegCtrl.setImage(NSImage(named: "pauseTemplate"), forSegment: 0)
            } else {
                controlsSegCtrl.setToolTip("Run", forSegment: 0)
                controlsSegCtrl.setImage(NSImage(named: "runTemplate"), forSegment: 0)
            }
        }
    }
    
    //
    // Action methods
    //
    
    @IBAction func inspectAction(_ sender: NSSegmentedControl) {
        
        switch sender.selectedSegment {
            
        case 0: parent.inspectorAction(sender)
        case 1: parent.monitorAction(sender)
        case 2: parent.consoleAction(sender)
            
        default: assert(false)
        }
    }
    
    @IBAction func snapshotAction(_ sender: NSSegmentedControl) {
        
        switch sender.selectedSegment {
        
        case 0: parent.takeSnapshotAction(self)
        case 1: parent.restoreSnapshotAction(self)
        case 2: parent.browseSnapshotsAction(self)
            
        default: assert(false)
        }
    }
    
    @IBAction func screenshotAction(_ sender: NSSegmentedControl) {
                
        switch sender.selectedSegment {
            
        case 0: parent.takeScreenshotAction(self)
        case 1: parent.browseScreenshotsAction(self)
            
        default: assert(false)
        }
    }
    
    @IBAction func port1Action(_ sender: Any) {

        if let obj = sender as? NSPopUpButton {
            parent.config.gameDevice1 = obj.selectedTag()
        }
    }
 
    @IBAction func port2Action(_ sender: Any) {

        if let obj = sender as? NSPopUpButton {
            parent.config.gameDevice2 = obj.selectedTag()
        }
    }

    @IBAction func keyboardAction(_ sender: Any!) {
        
        if parent.virtualKeyboard == nil {
            parent.virtualKeyboard =
            VirtualKeyboardController(with: parent, nibName: "VirtualKeyboard")
        }
        if parent.virtualKeyboard?.window?.isVisible == false {
            parent.virtualKeyboard?.showSheet()
        }
    }
    
    @IBAction func preferencesAction(_ sender: Any) {

        if let obj = sender as? NSSegmentedControl {

            switch obj.selectedSegment {
            case 0: parent.preferencesAction(obj)
            case 1: parent.configureAction(obj)
            default: assert(false)
            }
        }
    }
    
    @IBAction func controlsAction(_ sender: Any) {

        if let obj = sender as? NSSegmentedControl {

            switch obj.selectedSegment {
            case 0: parent.stopAndGoAction(self)
            case 1: parent.resetAction(self)
            case 2: parent.powerAction(self)
            default: assert(false)
            }
        }
    }
}
