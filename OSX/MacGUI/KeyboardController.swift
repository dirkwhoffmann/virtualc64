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

/*
extension CChar {
    var char: Character {
        return Character(UnicodeScalar(UInt8(bitPattern: Int8(self))))
    }
}
*/

/*
@objc public enum JoyDir : Int {
    case UP
    case DOWN
    case LEFT
    case RIGHT
    case FIRE
    case RELEASED
}
*/

//! @brief Mapping from keyboard keys to joystick movements
public class KeyMap: NSObject {

    //! @brief fingerprint of the assign key
    var fingerprint : [GamePadDirection:MacKeyFingerprint] = [:]

    //! @brief readable character for the assigned key
    var character : [GamePadDirection:String] = [:]
    
    @objc public func getFingerprint(forDirection d: GamePadDirection) -> MacKeyFingerprint {
        return fingerprint[d]!
    }
    
    @objc public func setFingerprint(_ f: MacKeyFingerprint, forDirection d: GamePadDirection) {
        fingerprint[d] = f
    }

    @objc public func getCharacter(forDirection d: GamePadDirection) -> String {
        return character[d]!
    }
    
    @objc public func setCharacter(_ c: String, forDirection d: GamePadDirection) {
        character[d] = c
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
                
        keymap1.fingerprint[GamePadDirection.LEFT] = 123
        keymap1.fingerprint[GamePadDirection.RIGHT] = 124
        keymap1.fingerprint[GamePadDirection.UP] = 126
        keymap1.fingerprint[GamePadDirection.DOWN] = 125
        keymap1.fingerprint[GamePadDirection.FIRE] = 49
            
        keymap1.character[GamePadDirection.LEFT] = " "
        keymap1.character[GamePadDirection.RIGHT] = " "
        keymap1.character[GamePadDirection.UP] = " "
        keymap1.character[GamePadDirection.DOWN] = " "
        keymap1.character[GamePadDirection.FIRE] = " "
            
        keymap2.fingerprint[GamePadDirection.LEFT] = 0
        keymap2.fingerprint[GamePadDirection.RIGHT] = 1
        keymap2.fingerprint[GamePadDirection.UP] = 6
        keymap2.fingerprint[GamePadDirection.DOWN] = 13
        keymap2.fingerprint[GamePadDirection.FIRE] = 7
            
        keymap2.character[GamePadDirection.LEFT] = "a"
        keymap2.character[GamePadDirection.RIGHT] = "s"
        keymap2.character[GamePadDirection.UP] = "y"
        keymap2.character[GamePadDirection.DOWN] = "w"
        keymap2.character[GamePadDirection.FIRE] = "x"
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
        
        keymap.fingerprint[GamePadDirection.LEFT] =
            MacKeyFingerprint(defaults.integer(forKey: "VC64Left" + s + "keycodeKey"))
        keymap.fingerprint[GamePadDirection.RIGHT] =
            MacKeyFingerprint(defaults.integer(forKey: "VC64Right" + s + "keycodeKey"))
        keymap.fingerprint[GamePadDirection.UP] =
            MacKeyFingerprint(defaults.integer(forKey: "VC64Up" + s + "keycodeKey"))
        keymap.fingerprint[GamePadDirection.DOWN] =
            MacKeyFingerprint(defaults.integer(forKey: "VC64Down" + s + "keycodeKey"))
        keymap.fingerprint[GamePadDirection.FIRE] =
            MacKeyFingerprint(defaults.integer(forKey: "VC64Fire" + s + "keycodeKey"))
        
        keymap.character[GamePadDirection.LEFT] =
            defaults.string(forKey: "VC64Left" + s + "charKey")
        keymap.character[GamePadDirection.RIGHT] =
            defaults.string(forKey: "VC64Right" + s + "charKey")
        keymap.character[GamePadDirection.UP] =
            defaults.string(forKey: "VC64Up" + s + "charKey")
        keymap.character[GamePadDirection.DOWN] =
            defaults.string(forKey: "VC64Down" + s + "charKey")
        keymap.character[GamePadDirection.FIRE] =
            defaults.string(forKey: "VC64Fire" + s + "charKey")
    }
    
    @objc func saveUserDefaults() {
        
        NSLog("\(#function)")
        saveUserDefaults(forKeymap: keymap1, s: "1")
        saveUserDefaults(forKeymap: keymap2, s: "2")
    }
    
    func saveUserDefaults(forKeymap keymap: KeyMap, s : String) {
        
        let defaults = UserDefaults.standard
        
        defaults.set(keymap.fingerprint[GamePadDirection.LEFT],  forKey :"VC64Left" + s + "keycodeKey")
        defaults.set(keymap.fingerprint[GamePadDirection.RIGHT], forKey :"VC64Right" + s + "keycodeKey")
        defaults.set(keymap.fingerprint[GamePadDirection.UP],    forKey :"VC64Up" + s + "keycodeKey")
        defaults.set(keymap.fingerprint[GamePadDirection.DOWN],  forKey :"VC64Down" + s + "keycodeKey")
        defaults.set(keymap.fingerprint[GamePadDirection.FIRE],  forKey :"VC64Fire" + s + "keycodeKey")
        
        defaults.set(keymap.character[GamePadDirection.LEFT],    forKey :"VC64Left" + s + "charKey")
        defaults.set(keymap.character[GamePadDirection.RIGHT],   forKey :"VC64Right" + s + "charKey")
        defaults.set(keymap.character[GamePadDirection.UP],      forKey :"VC64Up" + s + "charKey")
        defaults.set(keymap.character[GamePadDirection.DOWN],    forKey :"VC64Down" + s + "charKey")
        defaults.set(keymap.character[GamePadDirection.FIRE],    forKey :"VC64Fire" + s + "charKey")
    }
    
    //
    // Keyboard events
    //
    
    @objc public func keyDown(with event: NSEvent)
    {
        // Exit fullscreen mode if ESC is pressed
        if (event.keyCode == MAC_ESC && controller.metalScreen.fullscreen) {
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

    @discardableResult
    func pullJoystick(_ j: JoystickProxy, ifKeyMatches key: MacKeyFingerprint, inKeymap map: KeyMap) -> Bool
    {
        for (offset: _, element: (key: direction, value: value)) in map.fingerprint.enumerated() {
            
            if key == value {
                j.pullJoystick(direction)
                return true
            }
        }
        
        /*
        if (key == map.fingerprint[GamePadDirection.LEFT])       { j.pullJoystick(GamePadDirection.LEFT) }
        else if (key == map.fingerprint[GamePadDirection.RIGHT]) { j.pullJoystick(GamePadDirection.RIGHT) }
        else if (key == map.fingerprint[GamePadDirection.UP])    { j.pullJoystick(GamePadDirection.UP) }
        else if (key == map.fingerprint[GamePadDirection.DOWN])  { j.pullJoystick(GamePadDirection.DOWN) }
        else if (key == map.fingerprint[GamePadDirection.FIRE])  { j.pullJoystick(GamePadDirection.FIRE) }
        else { return false }
        */
        
        return false
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
    
    @discardableResult
    func releaseJoystick(_ j: JoystickProxy, ifKeyMatches key: MacKeyFingerprint, inKeymap map: KeyMap) -> Bool
    {
        for (offset: _, element: (key: direction, value: value)) in map.fingerprint.enumerated() {
            
            if key == value {
                j.releaseJoystick(direction)
                return true;
            }
        }
        
        return false

        /*
        if (key == map.fingerprint[GamePadDirection.LEFT])       { j.setAxisX(JOYSTICK_RELEASED) }
        else if (key == map.fingerprint[GamePadDirection.RIGHT]) { j.setAxisX(JOYSTICK_RELEASED) }
        else if (key == map.fingerprint[GamePadDirection.UP])    { j.setAxisY(JOYSTICK_RELEASED) }
        else if (key == map.fingerprint[GamePadDirection.DOWN])  { j.setAxisY(JOYSTICK_RELEASED) }
        else if (key == map.fingerprint[GamePadDirection.FIRE])  { j.setButtonPressed(false) }
        else { return false }
        
        return true
        */
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
            
        case MAC_F1: return C64KeyFingerprint(C64KEY_F1)
        case MAC_F2: return C64KeyFingerprint(C64KEY_F2)
        case MAC_F3: return C64KeyFingerprint(C64KEY_F3)
        case MAC_F4: return C64KeyFingerprint(C64KEY_F4)
        case MAC_F5: return C64KeyFingerprint(C64KEY_F5)
        case MAC_F6: return C64KeyFingerprint(C64KEY_F6)
        case MAC_F7: return C64KeyFingerprint(C64KEY_F7)
        case MAC_F8: return C64KeyFingerprint(C64KEY_F8)
        case MAC_RET: return C64KeyFingerprint(C64KEY_RET)
        case MAC_CL: return C64KeyFingerprint(C64KEY_CL)
        case MAC_CR: return C64KeyFingerprint(C64KEY_CR)
        case MAC_CU: return C64KeyFingerprint(C64KEY_CU)
        case MAC_CD: return C64KeyFingerprint(C64KEY_CD)
        case MAC_ESC: return C64KeyFingerprint(C64KEY_RUNSTOP)
        case MAC_TAB: return C64KeyFingerprint(C64KEY_RESTORE)
            
        case MAC_DEL:
            return (flags.contains(NSEvent.ModifierFlags.shift)) ?
                C64KeyFingerprint(C64KEY_INS) : C64KeyFingerprint(C64KEY_DEL)
        
        case MAC_HAT:
            return C64KeyFingerprint(HAT_KEY)
        
        case MAC_TILDE_US:
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
