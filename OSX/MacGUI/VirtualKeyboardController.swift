//
//  VirtualKeyboardController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 30.07.18.
//

import Foundation

class VirtualKeyboardController : UserDialogController
{
    /// Array holding a reference to the view of each key
    var keyView = Array(repeating: nil as NSButton?, count: 66)

    /// Array holding a reference to the image of each key
    var keyImage = Array(repeating: nil as NSImage?, count: 66)

    /// Indicates if the left Shift key is pressed
    var lshift = false

    /// Indicates if the right Shift key is pressed
    var rshift = false
    
    /// Indicates if the Commodore key is pressed
    var commodore = false
    
    /// Indicates if the window should close itself when a key has been pressed.
    /// If the virtual keyboard is opened as a sheet, this variable is set to
    /// true. If it is opened as a seperate window, it is set to false.
    var autoClose = true
    
    func showWindow(withParent controller: MyController) {
        
        track()
        
        parent = controller
        parentWindow = parent.window
        c64 = parent.mydocument.c64
        autoClose = false
        
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
    
    func updateImages() {
    
        for nr in 0 ... 65 {
            
            let shiftLock = c64.keyboard.shiftLockIsPressed()
            let pressed =
                (nr == 34 && shiftLock) ||
                (nr == 49 && commodore) ||
                (nr == 50 && lshift) ||
                (nr == 61 && rshift)
            let shift = lshift || rshift || shiftLock
            
            keyView[nr]!.image = C64Key(nr).image(pressed: pressed,
                                                  shift: shift,
                                                  commodore: commodore)
        }
    }
    
    @IBAction func pressVirtualC64Key(_ sender: Any!) {
        
        track()
        
        let tag = (sender as! NSButton).tag
        let key = C64Key(tag)
        
        func press() {
            if key.nr == 31 {
                parent.c64.keyboard.pressRestoreKey()
            } else {
                parent.c64.keyboard.pressKey(atRow: key.row, col: key.col)
            }
        }
        func release() {
            if key.nr == 31 {
                parent.c64.keyboard.releaseRestoreKey()
            } else {
                parent.c64.keyboard.releaseKey(atRow: key.row, col: key.col)
            }
        }
        
        switch (key.nr) {
            
        case 34: // Shift Lock
            
            if c64.keyboard.shiftLockIsPressed() {
                c64.keyboard.unlockShift()
            } else {
                c64.keyboard.lockShift()
            }
            updateImages()

        case 49: // Commodore
            
            commodore = !commodore
            commodore ? press() : release()
            updateImages()
            
        case 50: // Left Shift
            
            lshift = !lshift
            lshift ? press() : release()
            updateImages()
            
        case 61: // Right Shift
            
            rshift = !rshift
            rshift ? press() : release()
            updateImages()
            
        default:
            
            DispatchQueue.global().async {
                
                press()
                usleep(useconds_t(20000))
                release()
                self.parent.c64.keyboard.releaseKey(atRow: C64Key.commodore.row,
                                               col: C64Key.commodore.col)
                self.parent.c64.keyboard.releaseKey(atRow: C64Key.shift.row,
                                               col: C64Key.shift.col)
                self.parent.c64.keyboard.releaseKey(atRow: C64Key.rightShift.row,
                                               col: C64Key.rightShift.col)
            }
            
            if (autoClose) {
                cancelAction(self)
            }
        }
    }
    
}
