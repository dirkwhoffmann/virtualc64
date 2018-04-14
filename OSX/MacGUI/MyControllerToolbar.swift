//
//  MyControllerToolbar.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 19.01.18.
//

import Foundation

extension MyController {
 
    struct InputDevice {
        static let none = -1
        static let keyset1 = 0
        static let keyset2 = 1
        static let joystick1 = 2
        static let joystick2 = 3
    }
    
    // NSDrawerState is deprected an not available natively in Swift
    struct NSDrawerState {
        static let closed = 0
        static let opening = 1
        static let open = 2
        static let closing = 3
    }

    open override func validateToolbarItem(_ item: NSToolbarItem) -> Bool {
        
        let tag = item.tag
        
        // NSLog("Validating \(tag)...")
    
        if c64 != nil && c64.isRunning() {
            
            let document = self.document as! MyDocument?
            document?.updateChangeCount(.changeDone)
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
                item.label = "Run"
            }
            return true
        }
    
        // Step into, Step out, Step over
        if (tag >= 2 && tag <= 4) {
            return !c64.isRunning() && c64.isRunnable()
        }
        
        // All other items
        return true
    }
   
    func validateJoystickToolbarItem(_ popup: NSPopUpButton, selectedSlot: Int, port: JoystickProxy!) {
        
        
        let menu =  popup.menu
        let item0 = menu?.item(withTag: InputDevice.keyset1)
        let item1 = menu?.item(withTag: InputDevice.keyset2)
        let item2 = menu?.item(withTag: InputDevice.joystick1)
        let item3 = menu?.item(withTag: InputDevice.joystick2)
        
        // Set images and titles
        let defaultImage = NSImage(named: NSImage.Name(rawValue: "joystick32_generic"))
        item0?.image = (gamePadManager.gamePads[0]?.image)!
        item1?.image = (gamePadManager.gamePads[1]?.image)!
        item2?.image = gamePadManager.gamePads[2]?.image ?? defaultImage
        item3?.image = gamePadManager.gamePads[3]?.image ?? defaultImage

        item2?.title = gamePadManager.gamePads[2]?.name ?? "USB Device 1"
        item3?.title = gamePadManager.gamePads[3]?.name ?? "USB Device 2"
        
        item2?.isEnabled = !gamePadManager.slotIsEmpty(InputDevice.joystick1)
        item3?.isEnabled = !gamePadManager.slotIsEmpty(InputDevice.joystick2)
        
        // Mark game pad connected to port
        popup.selectItem(withTag: selectedSlot)
    }
    
    @objc func validateJoystickToolbarItems() {
    
        validateJoystickToolbarItem(joystickPortA, selectedSlot: gamepadSlotA, port: c64.joystickA)
        validateJoystickToolbarItem(joystickPortB, selectedSlot: gamepadSlotB, port: c64.joystickB)
    }
        
    @IBAction func portAAction(_ sender: NSPopUpButton) {
        
        // Remember selection
        gamepadSlotA = sender.selectedTag();

        // Avoid double mappings
        gamepadSlotB = (gamepadSlotA == gamepadSlotB) ? InputDevice.none : gamepadSlotB
        
        validateJoystickToolbarItems();
    }
    
    @IBAction func portBAction(_ sender: NSPopUpButton) {
        
        // Remember selection
        gamepadSlotB = sender.selectedTag();
        
        // Avoid double mappings
        gamepadSlotA = (gamepadSlotA == gamepadSlotB) ? InputDevice.none : gamepadSlotA
        
        validateJoystickToolbarItems();
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
    
    @IBAction func mediaAction(_ sender: Any!) {
    
        let nibName = NSNib.Name(rawValue: "DiskInspector")
        let dialogController = DiskInspectorController.init(windowNibName: nibName)
        dialogController.showSheet(withParent: self)
    }
    
    @objc public func debugOpenAction(_ sender: Any!) {
    
        let state = debugPanel.state
        if state == NSDrawerState.closed || state == NSDrawerState.closing {
            debugPanel.open()
        }
    }
    
    @objc public func debugCloseAction(_ sender: Any!) {
    
        let state = debugPanel.state
        if state == NSDrawerState.open || state == NSDrawerState.opening {
            debugPanel.close()
        }
    }
    
    @IBAction func debugAction(_ sender: Any!) {
    
        let state = debugPanel.state
        if state == NSDrawerState.closed || state == NSDrawerState.closing {
            debugOpenAction(self)
        } else {
            debugCloseAction(self)
        }
   
        refresh()
    }
    
    @IBAction func snapshotsAction(_ sender: Any!) {
        
        let nibName = NSNib.Name(rawValue: "SnapshotDialog")
        let controller = SnapshotDialog.init(windowNibName: nibName)
        controller.showSheet(withParent: self)
    }

    @IBAction func restoreLatestAutoSnapshotAction(_ sender: Any!) {
        
        if (c64.restoreLatestAutoSnapshot()) {
            metalScreen.snapToFront()
        }
    }

    @IBAction func restoreLatestUserSnapshotAction(_ sender: Any!) {
        
        if (c64.restoreLatestUserSnapshot()) {
            metalScreen.snapToFront()
        }
    }
    
    @IBAction func printDocument(_ sender: Any!) {
    
        let document = self.document as! MyDocument
        let window = document.windowForSheet!

        // Printing properties
        let printInfo = document.printInfo
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
