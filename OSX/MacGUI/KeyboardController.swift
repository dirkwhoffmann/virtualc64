/*
 * (C) 2017 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// TODO:
// Port Properties Dialog to Swift as it used Fingerprints stuff heavily
// Remove temporary keyboard API in MyController
// Update Joystick emulation key code. Make Shift etc. keys to work again
// Get rid of Macfingerprint, C64Fingerprint
// Use Unicode symbols for keys (a lot of keys have "" assigned right now)
// Think about layout of KeyboardDialog

import Foundation
import Carbon.HIToolbox

/*! @brief   Remembers currently pressed keys and there related C64KeyFingerprints
 *  @details Make this a simple variable once the whole class has been ported to Swift
 *  @deprecated
 */
class PressedKeys: NSObject {
    
    var keycodes : [UInt16:C64KeyFingerprint] = [:]
}

//!@ brief Keyboard event handler
class KeyboardController: NSObject {
    
    var controller : MyController!
    
    /// Determines whether the joystick emulation keys should be uncoupled from the keyboard.
    var disconnectEmulationKeys: Bool = true
    
    /**
     Key mapping mode
     
     The user can choose between a symbolic and a positional assignment of the keys. The symbolic assignment tries to assign the keys according to their meaning while the positional assignment establishes a one-to-one mapping between Mac keys and C64 keys.
    */
    var mapKeysByPosition: Bool = false
    
    /// Used key map if keys are mapped by position
 
    var keyMap: [MacKey:C64Key] = KeyboardController.standardKeyMap
    
    // Delete when Objective-C code is gone
    @objc func getDisconnectEmulationKeys() -> Bool { return disconnectEmulationKeys }
    @objc func setDisconnectEmulationKeys(_ b: Bool) { disconnectEmulationKeys = b }
    @objc func getMapKeysByPosition() -> Bool { return mapKeysByPosition }
    @objc func setMapKeysByPosition(_ b: Bool) { mapKeysByPosition = b }

    /// Remembers the currently pressed key modifiers
    var shift: Bool = false
    var control: Bool = false
    var option: Bool = false
    
    /**
     Remembers the currently pressed keys and their assigned C64 key list

     This variable is only used when keys are mapped symbolically. It's written in keyDown and picked up in keyUp.
     */
    var pressedKeys: [MacKey:[C64Key]] = [:]
    
    /**
     Checks if the internal values are consistent with the provides modifier flags.
     
     There should never be an insonsistency. But if there is, we release the suspicous key. Otherwise, we risk to block the C64's keyboard matrix forever.
     */
    func checkConsistency(withFlags flags: NSEvent.ModifierFlags) {
        
        if (shift != flags.contains(NSEvent.ModifierFlags.shift)) {
            keyUp(with: MacKey.shift)
            Swift.print("*** SHIFT inconsistency detected *** \(shift)")
        }
        if (control != flags.contains(NSEvent.ModifierFlags.control)) {
            keyUp(with: MacKey.control)
            Swift.print("*** SHIFT inconsistency *** \(control)")
        }
        if (option != flags.contains(NSEvent.ModifierFlags.option)) {
            keyUp(with: MacKey.option)
            Swift.print("*** SHIFT inconsistency *** \(option)")
        }
    }
    
    init(controller c: MyController) {
        
        super.init()
        self.controller = c
    }
    
    func keyDown(with event: NSEvent) {
        
        // track()
        
        // Ignore repeating keys
        if (event.isARepeat) {
            return
        }
        
        // Exit fullscreen mode if escape key is pressed
        if (event.keyCode == MacKeys.ESC && controller.fullscreen()) {
            controller.window!.toggleFullScreen(nil)
        }
        
        let keyCode = event.keyCode
        let flags = event.modifierFlags
        let characters = event.charactersIgnoringModifiers
        
        // Ignore keys that are pressed in combination with the command key
        if (flags.contains(NSEvent.ModifierFlags.command)) {
            track("Ignoring the command key")
            return
        }
        
        // Create and press MacKey
        let macKey = MacKey.init(keyCode: keyCode, characters: characters)
        checkConsistency(withFlags: flags)
        keyDown(with: macKey)
    }
    
    func keyUp(with event: NSEvent)
    {
        let keyCode = event.keyCode
        let flags = event.modifierFlags
        let characters = event.charactersIgnoringModifiers

        // Create and release macKey
        let macKey = MacKey.init(keyCode: keyCode, characters: characters)
        checkConsistency(withFlags: flags)
        keyUp(with: macKey)
    }
    
    func flagsChanged(with event: NSEvent) {
        
        let mod = event.modifierFlags
        
        if mod.contains(.shift) && !shift {
            shift = true
            keyDown(with: MacKey.shift)
        }
        if !mod.contains(.shift) && shift {
            shift = false
            keyUp(with: MacKey.shift)
        }
        
        if mod.contains(.control) && !control {
            control = true
            keyDown(with: MacKey.control)
        }
        if !mod.contains(.control) && control {
            control = false
            keyUp(with: MacKey.control)
        }
        
        if mod.contains(.option) && !option {
            option = true
            keyDown(with: MacKey.option)
        }
        if !mod.contains(.option) && option {
            option = false
            keyUp(with: MacKey.option)
        }
    }
    
    func keyDown(with macKey: MacKey) {
        
        // Check if this key is used for joystick emulation
        if controller.gamePadManager.keyDown(with: macKey) && disconnectEmulationKeys {
            return
        }
        
        if mapKeysByPosition {
            keyDown(with: macKey, keyMap: keyMap)
            return
        }

        // Translate MacKey to a list of C64Keys
        let c64Keys = translate(macKey: macKey)
        
        if c64Keys != [] {

            // Store key combination for later use in keyUp
            pressedKeys[macKey] = c64Keys
        
            // Press all required keys
            for key in c64Keys {
                controller.c64.keyboard.pressKey(atRow: key.row, col: key.col)
            }
        }
    }
    
    func keyDown(with macKey: MacKey, keyMap: [MacKey:C64Key]) {
        
        if let key = keyMap[macKey] {
            controller.c64.keyboard.pressKey(atRow: key.row, col: key.col)
        }
    }
        
    func keyUp(with macKey: MacKey) {
        
        // Check if this key is used for joystick emulation
        if controller.gamePadManager.keyUp(with: macKey) && disconnectEmulationKeys {
            return
        }
        
        if mapKeysByPosition {
            keyUp(with: macKey, keyMap: keyMap)
            return
        }
        
        // Lookup keys to be released
        if let c64Keys = pressedKeys[macKey] {
            for key in c64Keys {
                controller.c64.keyboard.releaseKey(atRow: key.row, col: key.col)
            }
        }
    }
    
    func keyUp(with macKey: MacKey, keyMap: [MacKey:C64Key]) {
        
        if let key = keyMap[macKey] {
            // track("Releasing row: \(key.row) col: \(key.col)\n")
            controller.c64.keyboard.releaseKey(atRow: key.row, col: key.col)
        }
    }

    
    /// Standard physical key mapping
    /// Keys are matched based on their position on the keyboard
    static let standardKeyMap: [MacKey:C64Key] = [
        
        // First row of C64 keyboard
        MacKey.ansi.grave: C64Key.leftArrow,
        MacKey.iso.hat: C64Key.leftArrow,
        MacKey.ansi.digit0: C64Key.digit0,
        MacKey.ansi.digit1: C64Key.digit1,
        MacKey.ansi.digit2: C64Key.digit2,
        MacKey.ansi.digit3: C64Key.digit3,
        MacKey.ansi.digit4: C64Key.digit4,
        MacKey.ansi.digit5: C64Key.digit5,
        MacKey.ansi.digit6: C64Key.digit6,
        MacKey.ansi.digit7: C64Key.digit7,
        MacKey.ansi.digit8: C64Key.digit8,
        MacKey.ansi.digit9: C64Key.digit9,
        MacKey.ansi.minus: C64Key.minus,
        MacKey.ansi.equal: C64Key.plus,
        MacKey.delete: C64Key.delete,
        MacKey.F1 : C64Key.F1F2,
        
        // Second row of C64 keyboard
        MacKey.tab: C64Key.control,
        MacKey.ansi.Q: C64Key.Q,
        MacKey.ansi.W: C64Key.W,
        MacKey.ansi.E: C64Key.E,
        MacKey.ansi.R: C64Key.R,
        MacKey.ansi.T: C64Key.T,
        MacKey.ansi.Y: C64Key.Y,
        MacKey.ansi.U: C64Key.U,
        MacKey.ansi.I: C64Key.I,
        MacKey.ansi.O: C64Key.O,
        MacKey.ansi.P: C64Key.P,
        MacKey.ansi.leftBracket: C64Key.at,
        MacKey.ansi.rightBracket: C64Key.asterisk,
        MacKey.F3 : C64Key.F3F4,
        
        // Third row of C64 keyboard
        MacKey.control: C64Key.runStop,
        MacKey.ansi.A: C64Key.A,
        MacKey.ansi.S: C64Key.S,
        MacKey.ansi.D: C64Key.D,
        MacKey.ansi.F: C64Key.F,
        MacKey.ansi.G: C64Key.G,
        MacKey.ansi.H: C64Key.H,
        MacKey.ansi.J: C64Key.J,
        MacKey.ansi.K: C64Key.K,
        MacKey.ansi.L: C64Key.L,
        MacKey.ansi.semicolon: C64Key.semicolon,
        MacKey.ansi.quote: C64Key.colon,
        MacKey.ansi.backSlash: C64Key.equal,
        MacKey.ret: C64Key.ret,
        MacKey.F5 : C64Key.F5F6,
        
        // Fourth row of C64 keyboard
        MacKey.option: C64Key.commodore,
        MacKey.shift: C64Key.shift,
        MacKey.ansi.Z: C64Key.Z,
        MacKey.ansi.X: C64Key.X,
        MacKey.ansi.C: C64Key.C,
        MacKey.ansi.V: C64Key.V,
        MacKey.ansi.B: C64Key.B,
        MacKey.ansi.N: C64Key.N,
        MacKey.ansi.M: C64Key.M,
        MacKey.ansi.comma: C64Key.comma,
        MacKey.ansi.period: C64Key.period,
        MacKey.ansi.slash: C64Key.slash,
        MacKey.curRight : C64Key.curLeftRight,
        MacKey.curLeft : C64Key.curLeftRight,
        MacKey.curDown : C64Key.curUpDown,
        MacKey.curUp : C64Key.curUpDown,
        MacKey.F7 : C64Key.F7F8,

        // Fifth row of C64 keyboard
        MacKey.space : C64Key.space
    ]
    
    /// Logical key mapping
    /// Keys are mapped based on their meaning or the characters they represent
    func translate(macKey: MacKey) -> [C64Key] {
        
        // Translate key that do not have a printable representation
        switch (macKey) {
        
        // First row of C64 keyboard
        case MacKey.delete: return [C64Key.delete]

        // Second row of C64 keyboard
        case MacKey.tab: return [C64Key.control]

        // Third row of C64 keyboard
        case MacKey.control: return [C64Key.runStop]
        case MacKey.ret: return [C64Key.ret]

        // Fourth row of C64 keyboard
        case MacKey.option: return [C64Key.commodore]
        case MacKey.curLeft: return [C64Key.curLeftRight, C64Key.shift]
        case MacKey.curRight: return [C64Key.curLeftRight]
        case MacKey.curUp: return [C64Key.curUpDown, C64Key.shift]
        case MacKey.curDown: return [C64Key.curUpDown]
           
        // Fifth row of C64 keyboard
        case MacKey.space: return [C64Key.space]
            
        // Function keys
        case MacKey.F1: return [C64Key.F1F2]
        case MacKey.F2: return [C64Key.F1F2, C64Key.shift]
        case MacKey.F3: return [C64Key.F3F4]
        case MacKey.F4: return [C64Key.F3F4, C64Key.shift]
        case MacKey.F5: return [C64Key.F5F6]
        case MacKey.F6: return [C64Key.F5F6, C64Key.shift]
        case MacKey.F7: return [C64Key.F7F8]
        case MacKey.F8: return [C64Key.F7F8, C64Key.shift]
            
        default: if macKey.description == nil { return [] }
        }
        
        // Translate keys having a printable representation
        
        switch (macKey.description!) {
        
        // First row of C64 keyboard
        case "ü": return [C64Key.leftArrow]
        case "1": return [C64Key.digit1]
        case "!": return [C64Key.digit1, C64Key.shift]
        case "2": return [C64Key.digit2]
        case "\"": return [C64Key.digit2, C64Key.shift]
        case "3": return [C64Key.digit3]
        case "#": return [C64Key.digit3, C64Key.shift]
        case "4": return [C64Key.digit4]
        case "$": return [C64Key.digit4, C64Key.shift]
        case "5": return [C64Key.digit5]
        case "%": return [C64Key.digit5, C64Key.shift]
        case "6": return [C64Key.digit6]
        case "&": return [C64Key.digit6, C64Key.shift]
        case "7": return [C64Key.digit7]
        case "'": return [C64Key.digit7, C64Key.shift]
        case "8": return [C64Key.digit8]
        case "(": return [C64Key.digit8, C64Key.shift]
        case "9": return [C64Key.digit9]
        case ")": return [C64Key.digit9, C64Key.shift]
        case "0": return [C64Key.digit0]
        case "+": return [C64Key.plus]
        case "-": return [C64Key.minus]
        case "ü": return [C64Key.pound]
        case "§": return [C64Key.pound]
            
        // Second row of C64 keyboard
        case "q": return [C64Key.Q]
        case "Q": return [C64Key.Q, C64Key.shift]
        case "w": return [C64Key.W]
        case "W": return [C64Key.W, C64Key.shift]
        case "e": return [C64Key.E]
        case "E": return [C64Key.E, C64Key.shift]
        case "r": return [C64Key.R]
        case "R": return [C64Key.R, C64Key.shift]
        case "t": return [C64Key.T]
        case "T": return [C64Key.T, C64Key.shift]
        case "y": return [C64Key.Y]
        case "Y": return [C64Key.Y, C64Key.shift]
        case "u": return [C64Key.U]
        case "U": return [C64Key.U, C64Key.shift]
        case "i": return [C64Key.I]
        case "I": return [C64Key.I, C64Key.shift]
        case "o": return [C64Key.O]
        case "O": return [C64Key.O, C64Key.shift]
        case "p": return [C64Key.P]
        case "P": return [C64Key.P, C64Key.shift]
        case "@": return [C64Key.at]
        case "ö": return [C64Key.at]
        case "*": return [C64Key.asterisk]
        case "ä": return [C64Key.upArrow]
            
        // Third row of C64 keyboard
        case "a": return [C64Key.A]
        case "A": return [C64Key.A, C64Key.shift]
        case "s": return [C64Key.S]
        case "S": return [C64Key.S, C64Key.shift]
        case "d": return [C64Key.D]
        case "D": return [C64Key.D, C64Key.shift]
        case "f": return [C64Key.F]
        case "F": return [C64Key.F, C64Key.shift]
        case "g": return [C64Key.G]
        case "G": return [C64Key.G, C64Key.shift]
        case "h": return [C64Key.H]
        case "H": return [C64Key.H, C64Key.shift]
        case "j": return [C64Key.J]
        case "J": return [C64Key.J, C64Key.shift]
        case "k": return [C64Key.K]
        case "K": return [C64Key.K, C64Key.shift]
        case "l": return [C64Key.L]
        case "L": return [C64Key.L, C64Key.shift]
        case ":": return [C64Key.colon]
        case "[": return [C64Key.colon, C64Key.shift]
        case ";": return [C64Key.semicolon]
        case "]": return [C64Key.semicolon, C64Key.shift]
        case "=": return [C64Key.equal]
            
        // Fourth row of C64 keyboard
        case "z": return [C64Key.Z]
        case "Z": return [C64Key.Z, C64Key.shift]
        case "x": return [C64Key.X]
        case "X": return [C64Key.X, C64Key.shift]
        case "c": return [C64Key.C]
        case "C": return [C64Key.C, C64Key.shift]
        case "v": return [C64Key.V]
        case "V": return [C64Key.V, C64Key.shift]
        case "b": return [C64Key.B]
        case "B": return [C64Key.B, C64Key.shift]
        case "n": return [C64Key.N]
        case "N": return [C64Key.N, C64Key.shift]
        case "m": return [C64Key.M]
        case "M": return [C64Key.M, C64Key.shift]
        case ",": return [C64Key.comma]
        case "<": return [C64Key.comma, C64Key.shift]
        case ".": return [C64Key.period]
        case ">": return [C64Key.period, C64Key.shift]
        case "/": return [C64Key.slash]
        case "?": return [C64Key.slash, C64Key.shift]
            
        default: return []
        }
}
    
    
    

    



//
// OLD CONTROLLER (TO BE REMOVED)
//

    // var pressedKeys: [UInt16:C64KeyFingerprint] = [:]
    
    struct MacKeys {
        static let F1 = UInt16(122)
        static let F2 = UInt16(120)
        static let F3 = UInt16(99)
        static let F4 = UInt16(118)
        static let F5 = UInt16(96)
        static let F6 = UInt16(97)
        static let F7 = UInt16(98)
        static let F8 = UInt16(100)
        static let APO = UInt16(39)
        static let DEL = UInt16(51)
        static let RET = UInt16(36)
        static let CL = UInt16(123)
        static let CR = UInt16(124)
        static let CU = UInt16(126)
        static let CD = UInt16(125)
        static let TAB = UInt16(48)
        static let SPC = UInt16(49)
        static let ESC = UInt16(53)
        static let HAT = UInt16(10)
        static let TILDE_US = UInt16(50)
    }

    
    /*! @brief  Computes unique fingerprint for a certain key combination pressed
     *          on the pyhsical Mac keyboard
     */
    @objc class func fingerprint(forKey keycode: UInt16, withModifierFlags flags: NSEvent.ModifierFlags) -> MacKeyFingerprint
    {
        let result = Int(keycode)
        
        // For most keys, the recorded fingerprint simply consists of the keycode.
        // In case of number keys (0 - 9), the fingerprint might also contains the
        // NSNumericPadKeyMask flag to distinguish keys from the numeric keypad.
        
        /* NOT NEEDED, KEYCODE IS DIFFERENT FOR NUMERIC KEYPAD
        switch (keycode) {
        case UInt16(kVK_ANSI_Keypad0),
             Int32(kVK_ANSI_Keypad1),
             Int32(kVK_ANSI_Keypad2),
             Int32(kVK_ANSI_Keypad3),
             Int32(kVK_ANSI_Keypad4),
             Int32(kVK_ANSI_Keypad5),
             Int32(kVK_ANSI_Keypad6),
             Int32(kVK_ANSI_Keypad7),
             Int32(kVK_ANSI_Keypad8),
             Int32(kVK_ANSI_Keypad9):
            result |= Int(NSNumericPadKeyMask.rawValue);
            break;
        default: break
        }
        */
        
        return MacKeyFingerprint(result);
    }
    
    /*! @brief  Translates a pressed key on the Mac keyboard to a C64 key fingerprint
     *  @note   The returned value can be used as argument for the emulators pressKey() function
     */
    class func translateKey(_ key: UInt8, plainkey: UInt8, keycode: UInt16, flags: NSEvent.ModifierFlags) -> C64KeyFingerprint
    {
    
        // print("key: \(key) keycode: \(keycode) flags: \(String(format:"%08X", flags.rawValue))")
        
        // let HAT_KEY     = UInt8(UnicodeScalar("^")!.value)
        let IHAT_KEY    = UInt8(UnicodeScalar("ì")!.value)
        let LESS_KEY    = UInt8(UnicodeScalar("<")!.value)
        let GREATER_KEY = UInt8(UnicodeScalar(">")!.value)
        
        switch (key) {
            
        case IHAT_KEY: return C64KeyFingerprint(C64KEY_UPARROW) // for Italien keyboards
        default: break
        }
        
        switch (keycode) {
            
        case MacKeys.F1: return C64KeyFingerprint(C64KEY_F1)
        case MacKeys.F2: return C64KeyFingerprint(C64KEY_F2)
        case MacKeys.F3: return C64KeyFingerprint(C64KEY_F3)
        case MacKeys.F4: return C64KeyFingerprint(C64KEY_F4)
        case MacKeys.F5: return C64KeyFingerprint(C64KEY_F5)
        case MacKeys.F6: return C64KeyFingerprint(C64KEY_F6)
        case MacKeys.F7: return C64KeyFingerprint(C64KEY_F7)
        case MacKeys.F8: return C64KeyFingerprint(C64KEY_F8)
        case MacKeys.RET: return C64KeyFingerprint(C64KEY_RET)
        case MacKeys.CL: return C64KeyFingerprint(C64KEY_CL)
        case MacKeys.CR: return C64KeyFingerprint(C64KEY_CR)
        case MacKeys.CU: return C64KeyFingerprint(C64KEY_CU)
        case MacKeys.CD: return C64KeyFingerprint(C64KEY_CD)
        case MacKeys.ESC: return C64KeyFingerprint(C64KEY_RUNSTOP)
        case MacKeys.TAB: return C64KeyFingerprint(C64KEY_RESTORE)
            
        case MacKeys.DEL:
            return (flags.contains(NSEvent.ModifierFlags.shift)) ?
                C64KeyFingerprint(C64KEY_INST) : C64KeyFingerprint(C64KEY_DEL)
        
        case MacKeys.HAT:
            return (flags.contains(NSEvent.ModifierFlags.shift)) ?
                C64KeyFingerprint(C64KEY_UPARROW) : C64KeyFingerprint(C64KEY_LEFTARROW)
        
        case MacKeys.TILDE_US:
            if (plainkey != LESS_KEY && plainkey != GREATER_KEY) {
                return C64KeyFingerprint(C64KEY_LEFTARROW)
            } else {
                break
            }
            
        default:
            if (flags.contains(NSEvent.ModifierFlags.option)) {
                // Commodore key (ALT) is pressed
                return C64KeyFingerprint(Int(String(plainkey))! | C64KEY_COMMODORE);
            } else if (flags.contains(NSEvent.ModifierFlags.control)) {
                // CTRL key is pressed
                return C64KeyFingerprint(Int(String(plainkey))! | C64KEY_CTRL);
            }
        }
        
        // No special translation needed
        
        return C64KeyFingerprint(UInt8(key));
    }
    
    func simulatePress(_ key: C64KeyFingerprint) {
        
        self.controller.c64.keyboard.pressKey(key)
        usleep(useconds_t(50000))
        self.controller.c64.keyboard.releaseKey(key)
    }
    
    @objc func simulateUserPressingKey(_ key: C64KeyFingerprint) {
        
        DispatchQueue.global().async {
            self.simulatePress(key)
        }
    }
 
    @objc func simulateUserPressingKeyWithShift(_ key: C64KeyFingerprint) {
        
        DispatchQueue.global().async {
            self.controller.c64.keyboard.pressShiftKey()
            self.simulatePress(key)
            self.controller.c64.keyboard.releaseShiftKey()
        }
    }
    
    @objc func simulateUserPressingKeyWithRunstop(_ key: C64KeyFingerprint) {
            
        DispatchQueue.global().async {
            self.controller.c64.keyboard.pressRunstopKey()
            self.simulatePress(key)
            self.controller.c64.keyboard.releaseRunstopKey()
        }
    }
    
    @objc func simulateUserTyping(text: String,
                                  initialDelay: useconds_t = 0,
                                  completion: (() -> Void)?) {

        let truncated = (text.count < 256) ? text : text.prefix(256) + "..."

        DispatchQueue.global().async {
        
            usleep(initialDelay);
            for c in truncated.lowercased().utf8 {
                self.controller.c64.keyboard.pressKey(C64KeyFingerprint(c))
                usleep(useconds_t(27500))
                self.controller.c64.keyboard.releaseKey(C64KeyFingerprint(c))
                usleep(useconds_t(27500))
            }
            completion?()
        }
    }
    
    @objc func simulateUserTypingAndPressPlay(text: String) {
        simulateUserTyping(text: text, completion: controller.c64.datasette.pressPlay)
    }
}
