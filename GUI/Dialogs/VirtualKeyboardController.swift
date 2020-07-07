// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class VirtualKeyboardController: DialogController, NSWindowDelegate {

    // Array holding a reference to the view of each key
    var keyView = Array(repeating: nil as NSButton?, count: 66)

    // Array holding a reference to the image of each key
    var keyImage = Array(repeating: nil as NSImage?, count: 66)

    // Remembers the state of some keys (true = currently pressed)
    var lshift = false
    var rshift = false
    var shiftLock = false
    var control = false
    var commodore = false
    
    // Indicates if the lower case character set is currently in use
    var lowercase = false
    
    // Indicates if the window should close itself when a key has been pressed.
    // If the virtual keyboard is opened as a sheet, this variable is set to
    // true. If it is opened as a seperate window, it is set to false.
    var autoClose = true
    
    var keyboard: KeyboardProxy { return c64.keyboard }
    
    static func make(parent: MyController) -> VirtualKeyboardController? {
        
        let name = "VirtualKeyboard"
        let keyboard = VirtualKeyboardController.init(windowNibName: name)
        keyboard.parent = parent
        keyboard.c64 = parent.c64
        
        return keyboard
    }
    
    func showSheet(autoClose: Bool) {

         self.autoClose = autoClose
         showSheet()
     }

     func showWindow(autoClose: Bool) {
         
         self.autoClose = autoClose
         showWindow(self)
     }
    
    override func windowDidLoad() {
        
        track()
        
        // Setup key references
        for tag in 0 ... 65 {
            keyView[tag] = window!.contentView!.viewWithTag(tag) as? NSButton
        }

        updateImages()
    }
    
    func windowWillClose(_ notification: Notification) {
    
        track()
        releaseSpecialKeys()
    }
    
    func windowDidBecomeMain(_ notification: Notification) {
        
        track()
        refresh()
    }
    
    func refresh() {
        
        if let win = window, win.isVisible {
            
            var needsUpdate = false
            
            if lshift != keyboard.leftShiftIsPressed() {
                lshift = keyboard.leftShiftIsPressed()
                needsUpdate = true
            }
            if rshift != keyboard.rightShiftIsPressed() {
                rshift = keyboard.rightShiftIsPressed()
                needsUpdate = true
            }
            if shiftLock != keyboard.shiftLockIsHoldDown() {
                shiftLock = keyboard.shiftLockIsHoldDown()
                needsUpdate = true
            }
            if control != keyboard.controlIsPressed() {
                control = keyboard.controlIsPressed()
                needsUpdate = true
            }
            if commodore != keyboard.commodoreIsPressed() {
                commodore = keyboard.commodoreIsPressed()
                needsUpdate = true
            }
            if lowercase != !keyboard.inUpperCaseMode() {
                lowercase = !keyboard.inUpperCaseMode()
                needsUpdate = true
            }
            
            if needsUpdate {
                updateImages()
            }
        }
    }
    
    func updateImages() {
                
        for nr in 0 ... 65 {
            
            let shiftLock = keyboard.shiftLockIsHoldDown()
            
            let pressed =
                (nr == 17 && control) ||
                (nr == 34 && shiftLock) ||
                (nr == 49 && commodore) ||
                (nr == 50 && lshift) ||
                (nr == 61 && rshift)
            let shift = lshift || rshift || shiftLock
        
            keyView[nr]!.image = C64Key(nr).image(pressed: pressed,
                                                  shift: shift,
                                                  control: control,
                                                  commodore: commodore,
                                                  lowercase: lowercase)
        }
    }
    
    func releaseSpecialKeys() {
                
        keyboard.releaseKey(atRow: C64Key.control.row, col: C64Key.control.col)
        keyboard.releaseKey(atRow: C64Key.commodore.row, col: C64Key.commodore.col)
        keyboard.releaseKey(atRow: C64Key.shift.row, col: C64Key.shift.col)
        keyboard.releaseKey(atRow: C64Key.rightShift.row, col: C64Key.rightShift.col)
    }
    
    @IBAction func pressVirtualC64Key(_ sender: NSButton!) {
        
        let tag = sender.tag
        let key = C64Key(tag)
        
        func press() {
            if key.nr == 31 {
                keyboard.pressRestoreKey()
            } else {
                keyboard.pressKey(atRow: key.row, col: key.col)
            }
        }
        func release() {
            if key.nr == 31 {
                keyboard.releaseRestoreKey()
            } else {
                keyboard.releaseKey(atRow: key.row, col: key.col)
            }
        }
        
        switch key.nr {
            
        case 34: // Shift Lock
            shiftLock ? keyboard.unlockShift() : keyboard.lockShift()

        case 17: // Control
            control ? release() : press()

        case 49: // Commodore
            commodore ? release() : press()
            
        case 50: // Left Shift
            lshift ? release() : press()
            
        case 61: // Right Shift
            rshift ? release() : press()
            
        default:
            
            DispatchQueue.global().async {
                
                press()
                usleep(useconds_t(20000))
                release()
                if self.autoClose {
                    self.releaseSpecialKeys()
                }
            }
            
            if autoClose {
                cancelAction(self)
            }
        }
    }
    
    override func mouseDown(with event: NSEvent) {
        
        track()
        if autoClose {
            cancelAction(self)
        }
    }
}
