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
        let document = self.document as! MyDocument
        
        // NSLog("Validating \(tag)...")
    
        if c64.isRunning() {
            document.updateChangeCount(.changeDone)
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
   
    func validateJoystickToolbarItem(_ popup: NSPopUpButton, port: JoystickProxy!) {
        
        let menu =  popup.menu
        let item1 = menu?.item(withTag: InputDevice.joystick1)
        let item2 = menu?.item(withTag: InputDevice.joystick2)
        item1?.isEnabled = !gamePadManager.slotIsEmpty(InputDevice.joystick1)
        item2?.isEnabled = !gamePadManager.slotIsEmpty(InputDevice.joystick2)
        
        // Mark game pad connected to port
        let slot = gamePadManager.lookupGamePad(port: port)
        popup.selectItem(withTag: slot)
    }
    
    @objc func validateJoystickToolbarItems() {
    
        validateJoystickToolbarItem(joystickPortA, port: c64.joystickA)
        validateJoystickToolbarItem(joystickPortB, port: c64.joystickB)
    }
    
    @objc func setupToolbarIcons() {

        // Joystick selectors
        joystickPortA.selectItem(at: 0)
        joystickPortB.selectItem(at: 0)
        validateJoystickToolbarItems()
    }
    
    
    func portAction(_ sender: NSPopUpButton, port: JoystickProxy) {

        gamePadManager.attachGamePad(sender.selectedTag(), toPort: port)
        validateJoystickToolbarItems()
    }

    @IBAction func portAAction(_ sender: NSPopUpButton) {
        
        portAction(sender, port: c64.joystickA)
    }
    
    @IBAction func portBAction(_ sender: NSPopUpButton) {
        
        portAction(sender, port: c64.joystickB)
    }
    
    @IBAction func propertiesAction(_ sender: Any!) {
    
        showPropertiesDialog()
    }
    
    @IBAction func hardwareAction(_ sender: Any!) {
    
        showHardwareDialog()
    }
    
    @IBAction func mediaAction(_ sender: Any!) {
    
        showMediaDialog()
    }
    
    @objc public func debugOpenAction(_ sender: Any!) {
    
        cheatboxCloseAction(self)
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
    
    @objc public func cheatboxOpenAction(_ sender: Any!) {
    
        debugCloseAction(self)
        let state = cheatboxPanel.state
        if state == NSDrawerState.closed || state == NSDrawerState.closing {
         
            c64.suspend()
            cheatboxImageBrowserView.refresh()
            cheatboxPanel.open()
        }
    }
    
    @objc public func cheatboxCloseAction(_ sender: Any!) {
        
        let state = cheatboxPanel.state
        if state == NSDrawerState.open || state == NSDrawerState.opening {
            
            c64.resume()
            cheatboxPanel.close()
        }
    }
    
    @IBAction func cheatboxAction(_ sender: Any!) {
        
        let state = cheatboxPanel.state
        if state == NSDrawerState.closed || state == NSDrawerState.closing {
            cheatboxOpenAction(self)
        } else {
            cheatboxCloseAction(self)
        }
        
    }

    @IBAction func printDocument(_ sender: Any!) {
    
        NSLog("\(#function)")
    
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
        let image = screenshot()
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
