// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

struct InputDevice {
    static let none = -1
    static let keyset1 = 0
    static let keyset2 = 1
    static let mouse = 2
    static let joystick1 = 3
    static let joystick2 = 4
}

extension MyController {
         
    @IBAction func port1Action(_ sender: NSPopUpButton) {
        
        setPort1(sender.selectedTag())
    }
 
    func setPort1(_ value: Int) {
        
        track("setPort1: \(value)")
        
        // Remember selection
        config.gameDevice1 = value
        
        // Avoid double mappings
        config.gameDevice2 = (config.gameDevice1 == config.gameDevice2) ? InputDevice.none : config.gameDevice2
        
        // Connect or disconnect analog mouse
        if c64.mouse.port != 1 && config.gameDevice1 == InputDevice.mouse {
            c64.mouse.connect(1)
        }
        if c64.mouse.port == 1 && config.gameDevice1 != InputDevice.mouse {
            c64.mouse.disconnect()
        }
                
        toolbar.validateVisibleItems()
    }
    
    @IBAction func port2Action(_ sender: NSPopUpButton) {
        
        setPort2(sender.selectedTag())
    }
    
    func setPort2(_ value: Int) {
        
        track("setPort2: \(value)")
        
        // Remember selection
        config.gameDevice2 = value
        
        // Avoid double mappings
        config.gameDevice1 = (config.gameDevice1 == config.gameDevice2) ? InputDevice.none : config.gameDevice1
        
        // Connect or disconnect analog mouse
        if c64.mouse.port != 2 && config.gameDevice2 == InputDevice.mouse {
            c64.mouse.connect(2)
        }
        if c64.mouse.port == 2 && config.gameDevice2 != InputDevice.mouse {
            c64.mouse.disconnect()
        }
        
        toolbar.validateVisibleItems()
    }
            
    @IBAction func inspectAction(_ sender: NSSegmentedControl) {
        
        switch sender.selectedSegment {
            
        case 0: inspectorAction(sender)
        case 1: monitorAction(sender)
            
        default: assert(false)
        }
    }
    
     @IBAction func snapshotAction(_ sender: NSSegmentedControl) {
        
        switch sender.selectedSegment {
            
        case 0: takeSnapshotAction(self)
        case 1: restoreSnapshotAction(self)
        case 2: browseSnapshotsAction(self)
            
        default:
            assert(false)
        }
    }
    
    @IBAction func screenshotAction(_ sender: NSSegmentedControl) {
        
        track()
        
        switch sender.selectedSegment {
            
        case 0: takeScreenshotAction(self)
        case 1: browseScreenshotsAction(self)
            
        default:
            assert(false)
        }
    }
    
    @IBAction func keyboardAction(_ sender: Any!) {
        
        track()

        if virtualKeyboard == nil {
            let name = NSNib.Name("VirtualKeyboard")
            virtualKeyboard = VirtualKeyboardController.make(parent: self, nibName: name)
        }
        if virtualKeyboard?.window?.isVisible == true {
            track("Virtual keyboard already open")
        } else {
            track("Opeining virtual keyboard as a sheet")
            virtualKeyboard?.showSheet()
        }
    }
}
