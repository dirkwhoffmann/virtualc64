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

import Foundation
import Carbon.HIToolbox

// Some convenience stuff
extension UInt8 {
    var char: Character {
        return Character(UnicodeScalar(self))
    }
}

extension CChar {
    var uint8: UInt8 {
        return UInt8(bitPattern: Int8(self))
    }
}


/*! @brief   Remembers currently pressed keys and there related C64KeyFingerprints
 *  @details Make this a simple variable once the whole class has been ported to Swift
 */
public class PressedKeys: NSObject {
    
    var keycodes : [UInt16:C64KeyFingerprint] = [:]
}

//!@ brief Keyboard event handler
class KeyboardController: NSObject {
    
    var controller : MyController!
    
    // If set to true, a key that is used for joystick emulation will not
    // be passed to the keyboard.
    var disconnectEmulationKeys: Bool = true
    
    var pressedKeys: [UInt16:C64KeyFingerprint] = [:]
    
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

    //
    // Initialization
    //

    override init()
    {
        disconnectEmulationKeys = true
        super.init()
        // restoreFactorySettings()
    }
 
    @objc convenience init(withController c: MyController)
    {
        self.init()
        self.controller = c
    }

    @objc func getDisconnectEmulationKeys() -> Bool { return disconnectEmulationKeys }
    @objc func setDisconnectEmulationKeys(_ b: Bool) { disconnectEmulationKeys = b }
    
    //
    // Keyboard events
    //
    
    @objc public func keyDown(with event: NSEvent)
    {
        // Exit fullscreen mode if ESC is pressed
        if (event.keyCode == MacKeys.ESC && controller.fullscreen()) {
            controller.window!.toggleFullScreen(nil)
        }
        
        var c       = event.characters!.utf8CString[0].uint8
        let c_unmod = event.charactersIgnoringModifiers!.utf8CString[0].uint8
        let keycode = event.keyCode
        let flags   = event.modifierFlags
        
        // print("keyDown: '\(c.char)' keycode: \(keycode) flags: \(String(format:"%08X", flags.rawValue))")
            
        // Ignore keys that are already pressed
        if (pressedKeys[keycode] != nil) {
            return
        }
        
        // Ignore command key
        if (flags.contains(NSEvent.ModifierFlags.command)) {
            return
        }
        
        // Inform GamePadManager about pressed key
        let f = KeyboardController.fingerprint(forKey:keycode, withModifierFlags:flags)
        if controller.gamePadManager.keyDown(f) && disconnectEmulationKeys {
            return
        }
        
        // Remove alternate key modifier if present
        if (flags.contains(NSEvent.ModifierFlags.option)) {
            c = c_unmod
        }
        
        // Translate key
        let c64key = KeyboardController.translateKey(c, plainkey: c_unmod, keycode: keycode, flags: flags)
        if (c64key == 0) {
            return
        }
        
        // Press key
        pressedKeys[keycode] = c64key
        controller.c64.keyboard.pressKey(c64key)
    }
    
    @objc public func keyUp(with event: NSEvent)
    {
        let keycode = event.keyCode       // UInt16
        let flags   = event.modifierFlags // NSEventModifierFlags
        
        // print("keyUp: keycode: \(keycode) flags: \(String(format:"%08X", flags.rawValue))")

        // Inform GamePadManager about released key
        let f = KeyboardController.fingerprint(forKey:keycode, withModifierFlags:flags)
        if controller.gamePadManager.keyUp(f) && disconnectEmulationKeys {
            return
        }
        
        // Release key
        if let key = pressedKeys[keycode] {
            controller.c64.keyboard.releaseKey(key)
            pressedKeys[keycode] = nil
        }
    }

    @objc public func flagsChanged(with event: NSEvent) {
        
        let flags = event.modifierFlags
        var key: MacKeyFingerprint
        
        // Check if special keys are used for joystick emulation
        if (flags.contains(NSEvent.ModifierFlags.option)) {
            key = NSEvent.ModifierFlags.option.rawValue;
        } else if (flags.contains(NSEvent.ModifierFlags.shift)) {
            key = NSEvent.ModifierFlags.shift.rawValue;
        } else if (flags.contains(NSEvent.ModifierFlags.command)) {
            key = NSEvent.ModifierFlags.command.rawValue;
        } else if (flags.contains(NSEvent.ModifierFlags.control)) {
            key = NSEvent.ModifierFlags.control.rawValue;
        } else {
            // Release joystick
            controller.gamePadManager.keyUp(NSEvent.ModifierFlags.option.rawValue)
            controller.gamePadManager.keyUp(NSEvent.ModifierFlags.shift.rawValue)
            controller.gamePadManager.keyUp(NSEvent.ModifierFlags.command.rawValue)
            controller.gamePadManager.keyUp(NSEvent.ModifierFlags.control.rawValue)
            
            return;
        }
        
        controller.gamePadManager.keyDown(key)
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
    
    @objc func simulateUserTypingText(_ text: String, initialDelay: Int = 0) {

        let truncated = (text.count < 256) ? text : text.prefix(256) + "..."

        DispatchQueue.global().async {
        
            usleep(useconds_t(initialDelay));
            for c in truncated.lowercased().utf8 {
                self.controller.c64.keyboard.pressKey(C64KeyFingerprint(c))
                usleep(useconds_t(27500))
                self.controller.c64.keyboard.releaseKey(C64KeyFingerprint(c))
                usleep(useconds_t(27500))
            }
        }
    }
}
