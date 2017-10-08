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
    
    @objc var keymap1 = KeyMap()
    @objc var keymap2 = KeyMap()
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
        super.init()
        restoreFactorySettings()
    }
 
    @objc convenience init(withController c: MyController)
    {
        self.init()
        self.controller = c
    }

    @objc func restoreFactorySettings()
    {
        NSLog("\(#function)")
                
        keymap1.setFingerprint(123, for: JoystickDirection.LEFT)
        keymap1.setFingerprint(124, for: JoystickDirection.RIGHT)
        keymap1.setFingerprint(126, for: JoystickDirection.UP)
        keymap1.setFingerprint(125, for: JoystickDirection.DOWN)
        keymap1.setFingerprint(49,  for: JoystickDirection.FIRE)
            
        keymap1.setCharacter(" ", for: JoystickDirection.LEFT)
        keymap1.setCharacter(" ", for: JoystickDirection.RIGHT)
        keymap1.setCharacter(" ", for: JoystickDirection.UP)
        keymap1.setCharacter(" ", for: JoystickDirection.DOWN)
        keymap1.setCharacter(" ", for: JoystickDirection.FIRE)
            
        keymap2.setFingerprint(0,  for: JoystickDirection.LEFT)
        keymap2.setFingerprint(1,  for: JoystickDirection.RIGHT)
        keymap2.setFingerprint(6,  for: JoystickDirection.UP)
        keymap2.setFingerprint(13, for: JoystickDirection.DOWN)
        keymap2.setFingerprint(7,  for: JoystickDirection.FIRE)
            
        keymap2.setCharacter("a", for: JoystickDirection.LEFT)
        keymap2.setCharacter("s", for: JoystickDirection.RIGHT)
        keymap2.setCharacter("y", for: JoystickDirection.UP)
        keymap2.setCharacter("w", for: JoystickDirection.DOWN)
        keymap2.setCharacter("x", for: JoystickDirection.FIRE)
    }
    
    
    //
    // User default storage
    //
    
    @objc class func registerStandardUserDefaults() {
        
        let dictionary : [String:Any] = [
            "VC64Left1keycodeKey":123,
            "VC64Right1keycodeKey":124,
            "VC64Up1keycodeKey":126,
            "VC64Down1keycodeKey":125,
            "VC64Fire1keycodeKey":49,
            
            "VC64Left1charKey":" ",
            "VC64Right1charKey":" ",
            "VC64Up1charKey":" ",
            "VC64Down1charKey":" ",
            "VC64Fire1charKey":" ",

            "VC64Left2keycodeKey":0,
            "VC64Right2keycodeKey":1,
            "VC64Up2keycodeKey":6,
            "VC64Down2keycodeKey":13,
            "VC64Fire2keycodeKey":7,
            
            "VC64Left2charKey":"a",
            "VC64Right2charKey":"s",
            "VC64Up2charKey":"y",
            "VC64Down2charKey":"w",
            "VC64Fire2charKey":"x"]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
        // [[NSUserDefaults standardUserDefaults] registerDefaults:defaultValues];
    }

    @objc func loadUserDefaults() {
        
        NSLog("\(#function)")
        loadUserDefaults(forKeymap: keymap1, s: "1")
        loadUserDefaults(forKeymap: keymap2, s: "2")
    }
    
    func loadUserDefaults(forKeymap keymap: KeyMap, s : String) {
        
        let defaults = UserDefaults.standard
        
        func loadFingerprint(for d: JoystickDirection, usingKey key: String) {
            keymap.setFingerprint(
                MacKeyFingerprint(defaults.integer(forKey: key + s + "keycodeKey")), for: d)
        }
        
        loadFingerprint(for:JoystickDirection.LEFT, usingKey: "VC64Left")
        loadFingerprint(for:JoystickDirection.RIGHT, usingKey: "VC64Right")
        loadFingerprint(for:JoystickDirection.UP, usingKey: "VC64Up")
        loadFingerprint(for:JoystickDirection.DOWN, usingKey: "VC64Down")
        loadFingerprint(for:JoystickDirection.FIRE, usingKey: "VC64Fire")
        
        func loadCharacter(for d: JoystickDirection, usingKey key: String) {
            keymap.setCharacter(defaults.string(forKey: key + s + "keycodeKey"), for: d)
        }
        
        loadCharacter(for:JoystickDirection.LEFT, usingKey: "VC64Left")
        loadCharacter(for:JoystickDirection.RIGHT, usingKey: "VC64Right")
        loadCharacter(for:JoystickDirection.UP, usingKey: "VC64Up")
        loadCharacter(for:JoystickDirection.DOWN, usingKey: "VC64Down")
        loadCharacter(for:JoystickDirection.FIRE, usingKey: "VC64Fire")
    }
    
    @objc func saveUserDefaults() {
        
        NSLog("\(#function)")
        saveUserDefaults(forKeymap: keymap1, s: "1")
        saveUserDefaults(forKeymap: keymap2, s: "2")
    }
    
    func saveUserDefaults(forKeymap keymap: KeyMap, s : String) {
        
        let defaults = UserDefaults.standard
        
        func saveFingerprint(for d: JoystickDirection, usingKey key: String) {
            defaults.set(keymap.fingerprint(for: d), forKey: key + s + "keycodeKey")
        }
        
        saveFingerprint(for:JoystickDirection.LEFT, usingKey: "VC64Left")
        saveFingerprint(for:JoystickDirection.RIGHT, usingKey: "VC64Right")
        saveFingerprint(for:JoystickDirection.UP, usingKey: "VC64Up")
        saveFingerprint(for:JoystickDirection.DOWN, usingKey: "VC64Down")
        saveFingerprint(for:JoystickDirection.FIRE, usingKey: "VC64Fire")
        
        func saveCharacter(for d: JoystickDirection, usingKey key: String) {
            defaults.set(keymap.getCharacter(for: d), forKey: key + s + "charKey")
        }
        
        saveCharacter(for:JoystickDirection.LEFT, usingKey: "VC64Left")
        saveCharacter(for:JoystickDirection.RIGHT, usingKey: "VC64Right")
        saveCharacter(for:JoystickDirection.UP, usingKey: "VC64Up")
        saveCharacter(for:JoystickDirection.DOWN, usingKey: "VC64Down")
        saveCharacter(for:JoystickDirection.FIRE, usingKey: "VC64Fire")
    }
    
    //
    // Keyboard events
    //
    
    @objc public func keyDown(with event: NSEvent)
    {
        // Exit fullscreen mode if ESC is pressed
        if (event.keyCode == MacKeys.ESC && controller.metalScreen.fullscreen) {
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
        
        // Pull joysticks if a key matches
        let f = fingerprint(forKey:keycode, withModifierFlags:flags)
        pullJoystick(ifKeyMatches: f);

        // Remove alternate key modifier if present
        if (flags.contains(NSEvent.ModifierFlags.option)) {
            c = c_unmod
        }
        
        // Translate key
        let c64key = translateKey(c, plainkey: c_unmod, keycode: keycode, flags: flags)
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

        // Release joysticks if a key matches
        let f = fingerprint(forKey:keycode, withModifierFlags:flags)
        releaseJoystick(ifKeyMatches: f);

        // Release key
        if let key = pressedKeys[keycode] {
            controller.c64.keyboard.releaseKey(key)
            pressedKeys[keycode] = nil
        }
    }

    @objc public func flagsChanged(with event: NSEvent) {
        
        let flags = event.modifierFlags // NSEventModifierFlags
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
            // Release joytick
            releaseJoystick(ifKeyMatches: NSEvent.ModifierFlags.option.rawValue)
            releaseJoystick(ifKeyMatches: NSEvent.ModifierFlags.shift.rawValue)
            releaseJoystick(ifKeyMatches: NSEvent.ModifierFlags.command.rawValue)
            releaseJoystick(ifKeyMatches: NSEvent.ModifierFlags.control.rawValue)
            return;
        }
        
        // Pull joysticks
        pullJoystick(ifKeyMatches: key)
    }
    

    
    /*! @brief  Computes unique fingerprint for a certain key combination pressed
     *          on the pyhsical Mac keyboard
     */
    @objc func fingerprint(forKey keycode: UInt16, withModifierFlags flags: NSEvent.ModifierFlags) -> MacKeyFingerprint
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
    
    /*! @brief  Pulls joystick if key matches some value stored in keymap
     */
    func pullJoystick(ifKeyMatches key: MacKeyFingerprint)
    {
        let joyA = controller.c64.joystickA!
        let joyB = controller.c64.joystickB!
        
        if (controller.inputDeviceA == INPUT_DEVICES.IPD_KEYSET_1) {
            pullJoystick(joyA, ifKeyMatches: key, inKeymap: keymap1)
        }
        if (controller.inputDeviceA == INPUT_DEVICES.IPD_KEYSET_2) {
            pullJoystick(joyA, ifKeyMatches: key, inKeymap: keymap2)
        }
        if (controller.inputDeviceB == INPUT_DEVICES.IPD_KEYSET_1) {
            pullJoystick(joyB, ifKeyMatches: key, inKeymap: keymap1)
        }
        if (controller.inputDeviceB == INPUT_DEVICES.IPD_KEYSET_2) {
            pullJoystick(joyB, ifKeyMatches: key, inKeymap: keymap2)
        }
        
    }

    func pullJoystick(_ j: JoystickProxy, ifKeyMatches key: MacKeyFingerprint, inKeymap map: KeyMap)
    {
        if let direction = map.mapping[key] {
            j.pullJoystick(direction)
        }
    }
    
    /*! @brief  Releases joystick if key matches some value stored in keymap
     */
    func releaseJoystick(ifKeyMatches key: MacKeyFingerprint)
    {
        let joyA = controller.c64.joystickA!
        let joyB = controller.c64.joystickB!
        
        if (controller.inputDeviceA == INPUT_DEVICES.IPD_KEYSET_1) {
            releaseJoystick(joyA, ifKeyMatches: key, inKeymap: keymap1)
        }
        if (controller.inputDeviceA == INPUT_DEVICES.IPD_KEYSET_2) {
            releaseJoystick(joyA, ifKeyMatches: key, inKeymap: keymap2)
        }
        if (controller.inputDeviceB == INPUT_DEVICES.IPD_KEYSET_1) {
            releaseJoystick(joyB, ifKeyMatches: key, inKeymap: keymap1)
        }
        if (controller.inputDeviceB == INPUT_DEVICES.IPD_KEYSET_2) {
            releaseJoystick(joyB, ifKeyMatches: key, inKeymap: keymap2)
        }
    }
    
    func releaseJoystick(_ j: JoystickProxy, ifKeyMatches key: MacKeyFingerprint, inKeymap map: KeyMap)
    {
        if let direction = map.mapping[key] {
            j.releaseJoystick(direction)
        }
    }
    
    /*! @brief  Translates a pressed key on the Mac keyboard to a C64 key fingerprint
     *  @note   The returned value can be used as argument for the emulators pressKey() function
     */
    func translateKey(_ key: UInt8, plainkey: UInt8, keycode: UInt16, flags: NSEvent.ModifierFlags) -> C64KeyFingerprint
    {
    
        let HAT_KEY     = UInt8(UnicodeScalar("^")!.value)
        let LESS_KEY    = UInt8(UnicodeScalar("<")!.value)
        let GREATER_KEY = UInt8(UnicodeScalar(">")!.value)
        
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
                C64KeyFingerprint(C64KEY_INS) : C64KeyFingerprint(C64KEY_DEL)
        
        case MacKeys.HAT:
            return C64KeyFingerprint(HAT_KEY)
        
        case MacKeys.TILDE_US:
            if (plainkey != LESS_KEY && plainkey != GREATER_KEY) {
                return C64KeyFingerprint(C64KEY_ARROW);
            } else {
                break;
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

}
