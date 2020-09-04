// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class VirtualKeyboardController: DialogController, NSWindowDelegate {

    var keyboard: KeyboardProxy { return c64.keyboard }

    // Array holding a reference to the view of each key
    var keyView = Array(repeating: nil as NSButton?, count: 66)

    // Array holding a reference to the image of each key
    var keyImage = Array(repeating: nil as NSImage?, count: 66)

    // Image cache for keys that are currently pressed
    var pressedKeyImage = Array(repeating: nil as NSImage?, count: 128)

    // Currently set key modifiers
    var modifiers: Modifier = []
    
    // Remembers the state of some keys (true = currently pressed)
    var lshift = false
    var rshift = false
    var shiftLock = false
    var control = false
    var commodore = false
    
    // Indicates if the lower case character set is currently in use
    var lowercase = false
    
    /* Indicates if the window should close when a key is pressed. If the
     * virtual keyboard is opened as a sheet, this variable is set to true. If
     * it is opened as a seperate window, it is set to false.
     */
    var autoClose = true
            
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

        // Compute key caps
        updateImageCache()
        refresh()
    }
    
    func windowWillClose(_ notification: Notification) {
    
        track()
    }
    
    func windowDidBecomeMain(_ notification: Notification) {
        
        track()
        refresh()
    }
    
    func refresh() {
        
        track()
        
        var newModifiers: Modifier = []
        
        if keyboard.leftShiftIsPressed() { newModifiers.insert(.shift) }
        if keyboard.rightShiftIsPressed() { newModifiers.insert(.shift) }
        if keyboard.shiftLockIsHoldDown() { newModifiers.insert(.shift) }
        if keyboard.controlIsPressed() { newModifiers.insert(.control) }
        if keyboard.commodoreIsPressed() { newModifiers.insert(.commodore) }
        if lowercase { newModifiers.insert(.lowercase) }
        
        if modifiers != newModifiers {
            updateImageCache()
        }
        
        for nr in 0 ... 65 {
                        
            if c64.keyboard.keyIsPressed(nr) {
                keyView[nr]!.image = pressedKeyImage[nr]
            } else {
                keyView[nr]!.image = keyImage[nr]
            }
        }
    }
    
    func updateImageCache() {
        
        track()
        
        var modifier: Modifier = []
        if lshift || rshift || shiftLock { modifier.insert(.shift) }
        if commodore { modifier.insert(.commodore) }
        if lowercase { modifier.insert(.lowercase) }

        for nr in 0 ... 65 {
            
            let keycap = C64Key.lookupKeycap(for: nr, modifier: modifier)!
            keyImage[nr] = keycap.image
            pressedKeyImage[nr] = keycap.image?.copy() as? NSImage
            pressedKeyImage[nr]?.pressed()
        }
    }
        
    func pressKey(nr: Int) {
        
        track()
        c64.keyboard.pressKey(nr)
        // refresh()
        
        DispatchQueue.main.async {

            track()

            // usleep(useconds_t(5000))
            self.c64.keyboard.releaseAll()
            self.c64.keyboard.releaseRestoreKey()
            self.refresh()
        }
        
        if autoClose {
            cancelAction(self)
        }
    }
    
    func holdKey(nr: Int) {
        
        c64.keyboard.pressKey(nr)
        refresh()
    }
    
    @IBAction func pressVirtualKey(_ sender: NSButton!) {
        
        // Not used at the moment
    }
    
    override func mouseDown(with event: NSEvent) {
        
        track()
        if autoClose {
            cancelAction(self)
        }
    }
}

// Subclass of NSButton for the keys in the virtual keyboard.
class KeycapButton: NSButton {
    
    override func mouseDown(with event: NSEvent) {
        
        if let controller = window?.delegate as? VirtualKeyboardController {
            
            controller.pressKey(nr: self.tag)
        }
    }
    
    override func rightMouseDown(with event: NSEvent) {
    
        if let controller = window?.delegate as? VirtualKeyboardController {
            
            controller.holdKey(nr: self.tag)
        }
    }
}
