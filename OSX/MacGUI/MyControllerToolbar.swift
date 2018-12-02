//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension MyController {
 
    struct InputDevice {
        static let none = -1
        static let keyset1 = 0
        static let keyset2 = 1
        static let mouse = 2
        static let joystick1 = 3
        static let joystick2 = 4
    }
    
    // NSDrawerState is deprected an not available natively in Swift
    struct NSDrawerState {
        static let closed = 0
        static let opening = 1
        static let open = 2
        static let closing = 3
    }

    /*
    open override func validateToolbarItem(_ item: NSToolbarItem) -> Bool {
        
        let tag = item.tag
        
        track("Validating \(tag)...")
        assert(false)
        
        if c64 != nil && c64.isRunning() {
            mydocument.updateChangeCount(.changeDone)
        }
    
        // Snapshot handling
        if tag == 5 {
            // return c64.numAutoSnapshots() != 0
            return true
        }
        if tag == 6 {
            return c64.numUserSnapshots() != 0
        }

        // Pause/Continue
        if tag == 1 {
            if c64.isRunning() {
                item.image = NSImage.init(named: NSImage.Name(rawValue: "pause32"))
                item.label = "Pause"
            } else {
                item.image = NSImage.init(named: NSImage.Name(rawValue: "play32"))
                item.label = "Continue"
            }
            return true
        }
    
        // All other items
        return true
    }
    */
    
    func validateToolbarItems() {
        let button = pauseTbItem.view as! NSButton
        if c64.isRunning() {
            button.image = NSImage.init(named: NSImage.Name(rawValue: "pauseTemplate"))
            pauseTbItem.label = "Pause"
        } else {
            button.image = NSImage.init(named: NSImage.Name(rawValue: "continueTemplate"))
            pauseTbItem.label = "Run"
        }
        // snapshotSegCtrl.setEnabled(c64.numUserSnapshots() != 0, forSegment: 2)
        
    }
    
    func validateJoystickToolbarItem(_ popup: NSPopUpButton, selectedSlot: Int, port: ControlPortProxy!) {
        
        let menu =  popup.menu
        // let item2 = menu?.item(withTag: InputDevice.mouse)
        let item3 = menu?.item(withTag: InputDevice.joystick1)
        let item4 = menu?.item(withTag: InputDevice.joystick2)
        
        // USB joysticks
        item3?.title = gamePadManager.gamePads[3]?.name ?? "USB Device 1"
        item4?.title = gamePadManager.gamePads[4]?.name ?? "USB Device 2"
        item3?.isEnabled = !gamePadManager.slotIsEmpty(InputDevice.joystick1)
        item4?.isEnabled = !gamePadManager.slotIsEmpty(InputDevice.joystick2)
        
        // Mark game pad connected to port
        popup.selectItem(withTag: selectedSlot)
    }
    
    func validateJoystickToolbarItems() {
    
        validateJoystickToolbarItem(controlPort1, selectedSlot: gamepadSlot1, port: c64.port1)
        validateJoystickToolbarItem(controlPort2, selectedSlot: gamepadSlot2, port: c64.port2)
    }
        
    @IBAction func port1Action(_ sender: NSPopUpButton) {
        
        track()
        
        // Remember selection
        gamepadSlot1 = sender.selectedTag();

        // Avoid double mappings
        gamepadSlot2 = (gamepadSlot1 == gamepadSlot2) ? InputDevice.none : gamepadSlot2
        
        // Connect or disconnect analog mouse
        c64.mouse.connect(gamepadSlot1 == InputDevice.mouse ? 1 : 0);

        validateJoystickToolbarItems();
    }
    
    @IBAction func port2Action(_ sender: NSPopUpButton) {
        
        track()
        
        // Remember selection
        gamepadSlot2 = sender.selectedTag();
        
        // Avoid double mappings
        gamepadSlot1 = (gamepadSlot1 == gamepadSlot2) ? InputDevice.none : gamepadSlot1
        
        // Connect or disconnect analog mouse
        c64.mouse.connect(gamepadSlot2 == InputDevice.mouse ? 2 : 0);
        
        validateJoystickToolbarItems();
    }
    
    /*
    @IBAction func mediaAction(_ sender: Any!) {
        
        let nibName = NSNib.Name(rawValue: "DiskInspector")
        let controller = DiskInspectorController.init(windowNibName: nibName)
        controller.showSheet(withParent: self)
    }
    */
    @IBAction func diskInspectorAction(_ sender: Any!) {
 
        let nibName = NSNib.Name(rawValue: "DiskInspector")
        let controller = DiskInspectorController.init(windowNibName: nibName)
        controller.showSheet(withParent: self)
    }

    public func debugOpenAction(_ sender: Any!) {
        
        let state = debugger.state
        if state == NSDrawerState.closed || state == NSDrawerState.closing {
            c64.cpu.setTracing(true)
            debugger.open()
        }
    }
    
    public func debugCloseAction(_ sender: Any!) {
        
        let state = debugger.state
        if state == NSDrawerState.open || state == NSDrawerState.opening {
            c64.cpu.setTracing(false)
            debugger.close()
        }
    }
    
    @IBAction func debugAction(_ sender: Any!) {
        
        let state = debugger.state
        if state == NSDrawerState.closed || state == NSDrawerState.closing {
            debugOpenAction(self)
        } else {
            debugCloseAction(self)
        }
        
        refresh()
    }
    
    @IBAction func inspectAction(_ sender: NSSegmentedControl) {
        
        switch(sender.selectedSegment) {
            
        case 0: // Debugger
            
            track("Debugger")
            debugAction(sender)
            
        case 1: // Disk Inspector

            track("Disk Inspector")
            diskInspectorAction(sender)
            
        default:
            assert(false)
        }
    }
 
    @IBAction func devicesAction(_ sender: Any!) {
        
        let nibName = NSNib.Name(rawValue: "DevicesPrefs")
        let controller = DevicesPrefsController.init(windowNibName: nibName)
        controller.showSheet(withParent: self)
    }
    
    @IBAction func propertiesAction(_ sender: Any!) {
        
        let nibName = NSNib.Name(rawValue: "EmulatorPrefs")
        let controller = EmulatorPrefsController.init(windowNibName: nibName)
        controller.showSheet(withParent: self)
    }
    
    @IBAction func hardwareAction(_ sender: Any!) {
        
        let nibName = NSNib.Name(rawValue: "HardwarePrefs")
        let controller = HardwarePrefsController.init(windowNibName: nibName)
        controller.showSheet(withParent: self)
    }
    
    @IBAction func preferencesAction(_ sender: NSSegmentedControl) {
        
        switch(sender.selectedSegment) {
            
        case 0: // Devices
            
            devicesAction(sender)
            
        case 1: // Emulator
            
            propertiesAction(sender)
            
        case 2: // Hardware

            hardwareAction(sender)
            
        default:
            assert(false)
        }
    }
 
    @IBAction func snapshotAction(_ sender: NSSegmentedControl) {
        
        switch(sender.selectedSegment) {
        
        case 0: // Rewind

            track("Rewind")
            if (c64.restoreLatestAutoSnapshot()) {
                metalScreen.snapToFront()
            }
        
        case 1: // Take

            track("Snap")
            c64.takeUserSnapshot()
            
        case 2: // Restore
            
            track("Restore")
            if (c64.restoreLatestUserSnapshot()) {
                metalScreen.snapToFront()
            } else {
                NSSound.beep()
            }

        case 3: // Browse
            
            track("Browse")
            let nibName = NSNib.Name(rawValue: "SnapshotDialog")
            let controller = SnapshotDialog.init(windowNibName: nibName)
            controller.showSheet(withParent: self)

        default:
            assert(false)
        }
    }
    
    @IBAction func keyboardAction(_ sender: Any!) {
        
        // Open the virtual keyboard as a sheet
        let nibName = NSNib.Name(rawValue: "VirtualKeyboard")
        virtualKeyboardSheet = VirtualKeyboardController.init(windowNibName: nibName)
        virtualKeyboardSheet?.showSheet(withParent: self)
    }

    @IBAction func snapshotsAction(_ sender: Any!) {
        
        let nibName = NSNib.Name(rawValue: "SnapshotDialog")
        let controller = SnapshotDialog.init(windowNibName: nibName)
        controller.showSheet(withParent: self)
    }

    @IBAction func restoreLatestAutoSnapshotAction(_ sender: Any!) {
        
        if (c64.restoreLatestAutoSnapshot()) {
            c64.deleteAutoSnapshot(0)
            metalScreen.snapToFront()
        }
    }

    @IBAction func restoreLatestUserSnapshotAction(_ sender: Any!) {
        
        if (c64.restoreLatestUserSnapshot()) {
            metalScreen.snapToFront()
        }
    }
    
    @IBAction func printDocument(_ sender: Any!) {
    
        let window = mydocument.windowForSheet!

        // Printing properties
        let printInfo = mydocument.printInfo
        printInfo.horizontalPagination = .fitPagination
        printInfo.isHorizontallyCentered = true
        printInfo.verticalPagination = .fitPagination
        printInfo.isVerticallyCentered = true
        printInfo.orientation = .landscape
        printInfo.leftMargin = 32.0
        printInfo.rightMargin = 32.0
        printInfo.topMargin = 32.0
        printInfo.bottomMargin = 32.0

        // Image view
        let paperSize = printInfo.paperSize
        let image = metalScreen.screenshot()
        let printRect = NSMakeRect(0.0, 0.0, paperSize.width, paperSize.height)
        let imageView = NSImageView.init(frame: printRect)
        imageView.image = image
        imageView.imageScaling = .scaleAxesIndependently
    
        // Print image
        let printOperation = NSPrintOperation.init(view: imageView, printInfo: printInfo)
        printOperation.runModal(for: window,
                                delegate: nil,
                                didRun: nil,
                                contextInfo: nil)
    }
    
}
