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
     
    // NSDrawerState is deprected an not available natively in Swift
    struct NSDrawerState {
        static let closed = 0
        static let opening = 1
        static let open = 2
        static let closing = 3
    }
    
    @IBAction func port1Action(_ sender: NSPopUpButton) {
        
        setPort1(sender.selectedTag())
    }
 
    func setPort1(_ value: Int) {
        
        track("setPort1: \(value)")
        
        // Remember selection
        inputDevice1 = value
        
        // Avoid double mappings
        inputDevice2 = (inputDevice1 == inputDevice2) ? InputDevice.none : inputDevice2
        
        // Connect or disconnect analog mouse
        if c64.mouse.port() != 1 && inputDevice1 == InputDevice.mouse {
            c64.mouse.connect(1)
        }
        if c64.mouse.port() == 1 && inputDevice1 != InputDevice.mouse {
            c64.mouse.disconnect()
        }
        
        UserDefaults.standard.set(inputDevice1, forKey: VC64Keys.inputDevice1)
        UserDefaults.standard.set(inputDevice2, forKey: VC64Keys.inputDevice2)
        
        toolbar.validateVisibleItems()
    }
    
    @IBAction func port2Action(_ sender: NSPopUpButton) {
        
        setPort2(sender.selectedTag())
    }
    
    func setPort2(_ value: Int) {
        
        // Remember selection
        inputDevice2 = value
        
        // Avoid double mappings
        inputDevice1 = (inputDevice1 == inputDevice2) ? InputDevice.none : inputDevice1
        
        // Connect or disconnect analog mouse
        if c64.mouse.port() != 2 && inputDevice2 == InputDevice.mouse {
            c64.mouse.connect(2)
        }
        if c64.mouse.port() == 2 && inputDevice2 != InputDevice.mouse {
            c64.mouse.disconnect()
        }
        
        UserDefaults.standard.set(inputDevice1, forKey: VC64Keys.inputDevice1)
        UserDefaults.standard.set(inputDevice2, forKey: VC64Keys.inputDevice2)

        toolbar.validateVisibleItems()
    }
        
    @IBAction func diskInspectorAction(_ sender: Any!) {
 
        let nibName = NSNib.Name("DiskInspector")
        let controller = DiskInspectorController.init(windowNibName: nibName)
        controller.showSheet()
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
        
        switch sender.selectedSegment {
            
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

    @IBAction func preferencesAction(_ sender: Any!) {
        
        openPreferences()
    }
    
    @IBAction func snapshotAction(_ sender: NSSegmentedControl) {
        
        switch sender.selectedSegment {
        
        case 0: // Rewind

            track("Rewind")
            if c64.restoreLatestAutoSnapshot() {
                metalScreen.snapToFront()
            }
        
        case 1: // Take

            track("Snap")
            c64.takeUserSnapshot()
            
        case 2: // Restore
            
            track("Restore")
            if c64.restoreLatestUserSnapshot() {
                metalScreen.snapToFront()
            } else {
                NSSound.beep()
            }

        case 3: // Browse
            
            track("Browse")
            let nibName = NSNib.Name("SnapshotDialog")
            let controller = SnapshotDialog.init(windowNibName: nibName)
            controller.showSheet()

        default:
            assert(false)
        }
    }
    
    @IBAction func keyboardAction(_ sender: Any!) {
        
        // Open the virtual keyboard as a sheet
        if virtualKeyboard == nil {
            virtualKeyboard = VirtualKeyboardController.make(parent: self)
        }
        
        virtualKeyboard?.showSheet(autoClose: true)        
    }

    @IBAction func snapshotsAction(_ sender: Any!) {
        
        let nibName = NSNib.Name("SnapshotDialog")
        let controller = SnapshotDialog.init(windowNibName: nibName)
        controller.showSheet()
    }

    @IBAction func restoreLatestAutoSnapshotAction(_ sender: Any!) {
        
        if c64.restoreLatestAutoSnapshot() {
            c64.deleteAutoSnapshot(0)
            metalScreen.snapToFront()
        }
    }

    @IBAction func restoreLatestUserSnapshotAction(_ sender: Any!) {
        
        if c64.restoreLatestUserSnapshot() {
            metalScreen.snapToFront()
        }
    }
    
    @IBAction func printDocument(_ sender: Any!) {
    
        guard let window = mydocument?.windowForSheet else { return }

        // Printing properties
        let printInfo = mydocument!.printInfo
        printInfo.horizontalPagination = .fit
        printInfo.isHorizontallyCentered = true
        printInfo.verticalPagination = .fit
        printInfo.isVerticallyCentered = true
        printInfo.orientation = .landscape
        printInfo.leftMargin = 32.0
        printInfo.rightMargin = 32.0
        printInfo.topMargin = 32.0
        printInfo.bottomMargin = 32.0

        // Image view
        let size = printInfo.paperSize
        let image = metalScreen.screenshot()
        let printRect = NSRect.init(x: 0, y: 0, width: size.width, height: size.height)
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
