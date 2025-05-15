// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class VirtualKeyboardController: DialogController {

    var keyboard: KeyboardProxy? { return emu?.keyboard }

    @IBOutlet weak var caseSelector: NSSegmentedControl!

    // Array holding a reference to the view of each key
    var keyView = Array(repeating: nil as NSButton?, count: 66)

    // Array holding a reference to the image of each key
    var keyImage = Array(repeating: nil as NSImage?, count: 66)

    // Image cache for keys that are currently pressed
    var pressedKeyImage = Array(repeating: nil as NSImage?, count: 66)

    // Currently set key modifiers
    var modifiers: Modifier = []
        
    // Indicates if lower case or upper case characters should be displayed
    var lowercase: Bool { return caseSelector.selectedSegment == 1 }
    
    /* Indicates if the window should be closed when a key is pressed. If the
     * virtual keyboard is opened as a sheet, this variable is set to true. If
     * it is opened as a seperate window, it is set to false.
     */
    var autoClose = true

    func showSheet() {
        
        autoClose = true
        super.showAsSheet()
    }
    
    func showWindow() {
        
        autoClose = false
        super.showAsWindow()
    }
    
    override func windowDidLoad() {
        
        updateImageCache()
        refresh()
    }

    override func dialogWillShow() {

        // Collect references to all buttons
        for tag in 0 ... 65 {
            keyView[tag] = window!.contentView!.viewWithTag(tag) as? NSButton
        }
    }

    override func dialogDidShow() {

        refresh()
    }

    func windowDidBecomeMain(_ notification: Notification) {
        
        refresh()
    }

    func refresh() {
                
        if let keyboard = keyboard {

            // Only proceed if the keyboard is visible
            if window == nil || !window!.isVisible { return }

            var newModifiers: Modifier = []

            if keyboard.isPressed(.shift) { newModifiers.insert(.shift) }
            if keyboard.isPressed(.rightShift) { newModifiers.insert(.shift) }
            if keyboard.isPressed(.shiftLock) { newModifiers.insert(.shift) }
            if keyboard.isPressed(.control) { newModifiers.insert(.control) }
            if keyboard.isPressed(.commodore) { newModifiers.insert(.commodore) }
            if lowercase { newModifiers.insert(.lowercase) }

            // Update images if the modifier flags have changed
            if modifiers != newModifiers {
                modifiers = newModifiers
                updateImageCache()
            }

            for nr in 0 ... 65 {

                if keyboard.isPressed(nr) {
                    keyView[nr]!.image = pressedKeyImage[nr]
                } else {
                    keyView[nr]!.image = keyImage[nr]
                }
            }
        }
    }
    
    func updateImageCache() {
        
        for nr in 0 ... 65 {
            
            let keycap = C64Key.lookupKeycap(for: nr, modifier: modifiers)!
            keyImage[nr] = keycap.image
            pressedKeyImage[nr] = keycap.image?.copy() as? NSImage
            pressedKeyImage[nr]?.pressed()
        }
    }
        
    func pressKey(nr: Int) {

        if let keyboard = keyboard {

            if autoClose {

                if nr == 34 /* SHIFT LOCK */ {
                    keyboard.toggleKey(nr)
                } else {
                    keyboard.pressKey(nr)
                    keyboard.releaseAll(withDelay: 0.25)
                }
                cancelAction(self)

            } else {

                if nr == 34 /* SHIFT LOCK */ {
                    keyboard.toggleKey(nr)
                } else {
                    keyboard.pressKey(nr)
                    keyboard.releaseKey(nr, delay: 0.25)
                }
                refresh()
            }
        }
    }
    
    func holdKey(nr: Int) {
        
        if let keyboard = keyboard {

            keyboard.toggleKey(nr)
            refresh()
        }
    }
    
    override func mouseDown(with event: NSEvent) {

        // If opened as a sheet, close if the user clicked inside unsued area
        if autoClose { cancelAction(self) }
    }

    @IBAction func pressVirtualKey(_ sender: NSButton!) {
        
        // Not used at the moment
    }

    @IBAction func caseSelectorAction(_ sender: NSSegmentedControl!) {
        
        refresh()
    }
}

// Subclass of NSButton for the keys in the virtual keyboard
class KeycapButton: NSButton {
    
    override func mouseDown(with event: NSEvent) {
        
        if let controller = window?.delegate as? VirtualKeyboardController {
            
            controller.pressKey(nr: self.tag)
        }
    }

    override func mouseUp(with event: NSEvent) {
        
        if let controller = window?.delegate as? VirtualKeyboardController {
            
            controller.refresh()
        }
    }

    override func rightMouseDown(with event: NSEvent) {
    
        if let controller = window?.delegate as? VirtualKeyboardController {
            
            controller.holdKey(nr: self.tag)
        }
    }
}
