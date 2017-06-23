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

@objc public enum JoyDir : Int {
    case UP
    case DOWN
    case LEFT
    case RIGHT
    case FIRE
    case RELEASED
}

//! @brief Mapping from keyboard keys to joystick movements
public class KeyMap: NSObject {

    //! @brief fingerprint of the assign key
    var fingerprint : [JoyDir:MacKeyFingerprint] = [:]

    //! @brief readable character for the assigned key
    var character : [JoyDir:String] = [:]
    
    public func getFingerprint(forDirection d: JoyDir) -> MacKeyFingerprint {
        return fingerprint[d]!
    }
    
    public func setFingerprint(_ f: MacKeyFingerprint, forDirection d: JoyDir) {
        fingerprint[d] = f
    }

    public func getCharacter(forDirection d: JoyDir) -> String {
        return character[d]!
    }
    
    public func setCharacter(_ c: String, forDirection d: JoyDir) {
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
    
    var keymap1 = KeyMap()
    var keymap2 = KeyMap()
    var pressedKeys: [UInt16:C64KeyFingerprint] = [:]
    
    //
    // Initialization
    //

    override init()
    {
        super.init()
        restoreFactorySettings()
    }
 
    convenience init(withController c: MyController)
    {
        self.init()
        self.controller = c
    }

    func restoreFactorySettings()
    {
        NSLog("\(#function)")
                
        keymap1.fingerprint[JoyDir.LEFT] = 123
        keymap1.fingerprint[JoyDir.RIGHT] = 124
        keymap1.fingerprint[JoyDir.UP] = 126
        keymap1.fingerprint[JoyDir.DOWN] = 125
        keymap1.fingerprint[JoyDir.FIRE] = 49
            
        keymap1.character[JoyDir.LEFT] = " "
        keymap1.character[JoyDir.RIGHT] = " "
        keymap1.character[JoyDir.UP] = " "
        keymap1.character[JoyDir.DOWN] = " "
        keymap1.character[JoyDir.FIRE] = " "
            
        keymap2.fingerprint[JoyDir.LEFT] = 0
        keymap2.fingerprint[JoyDir.RIGHT] = 1
        keymap2.fingerprint[JoyDir.UP] = 6
        keymap2.fingerprint[JoyDir.DOWN] = 13
        keymap2.fingerprint[JoyDir.FIRE] = 7
            
        keymap2.character[JoyDir.LEFT] = "a"
        keymap2.character[JoyDir.RIGHT] = "s"
        keymap2.character[JoyDir.UP] = "y"
        keymap2.character[JoyDir.DOWN] = "w"
        keymap2.character[JoyDir.FIRE] = "x"
    }
    
    
    //
    // User default storage
    //
    
    class func registerStandardUserDefaults() {
        
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

    func loadUserDefaults() {
        
        NSLog("\(#function)")
        loadUserDefaults(forKeymap: keymap1, s: "1")
        loadUserDefaults(forKeymap: keymap2, s: "2")
    }
    
    func loadUserDefaults(forKeymap keymap: KeyMap, s : String) {
        
        let defaults = UserDefaults.standard
        
        keymap.fingerprint[JoyDir.LEFT] =
            MacKeyFingerprint(defaults.integer(forKey: "VC64Left" + s + "keycodeKey"))
        keymap.fingerprint[JoyDir.RIGHT] =
            MacKeyFingerprint(defaults.integer(forKey: "VC64Right" + s + "keycodeKey"))
        keymap.fingerprint[JoyDir.UP] =
            MacKeyFingerprint(defaults.integer(forKey: "VC64Up" + s + "keycodeKey"))
        keymap.fingerprint[JoyDir.DOWN] =
            MacKeyFingerprint(defaults.integer(forKey: "VC64Down" + s + "keycodeKey"))
        keymap.fingerprint[JoyDir.FIRE] =
            MacKeyFingerprint(defaults.integer(forKey: "VC64Fire" + s + "keycodeKey"))
        
        keymap.character[JoyDir.LEFT] =
            defaults.string(forKey: "VC64Left" + s + "charKey")
        keymap.character[JoyDir.RIGHT] =
            defaults.string(forKey: "VC64Right" + s + "charKey")
        keymap.character[JoyDir.UP] =
            defaults.string(forKey: "VC64Up" + s + "charKey")
        keymap.character[JoyDir.DOWN] =
            defaults.string(forKey: "VC64Down" + s + "charKey")
        keymap.character[JoyDir.FIRE] =
            defaults.string(forKey: "VC64Fire" + s + "charKey")
    }
    
    func saveUserDefaults() {
        
        NSLog("\(#function)")
        saveUserDefaults(forKeymap: keymap1, s: "1")
        saveUserDefaults(forKeymap: keymap2, s: "2")
    }
    
    func saveUserDefaults(forKeymap keymap: KeyMap, s : String) {
        
        let defaults = UserDefaults.standard
        
        defaults.set(keymap.fingerprint[JoyDir.LEFT],  forKey :"VC64Left" + s + "keycodeKey")
        defaults.set(keymap.fingerprint[JoyDir.RIGHT], forKey :"VC64Right" + s + "keycodeKey")
        defaults.set(keymap.fingerprint[JoyDir.UP],    forKey :"VC64Up" + s + "keycodeKey")
        defaults.set(keymap.fingerprint[JoyDir.DOWN],  forKey :"VC64Down" + s + "keycodeKey")
        defaults.set(keymap.fingerprint[JoyDir.FIRE],  forKey :"VC64Fire" + s + "keycodeKey")
        
        defaults.set(keymap.character[JoyDir.LEFT],    forKey :"VC64Left" + s + "charKey")
        defaults.set(keymap.character[JoyDir.RIGHT],   forKey :"VC64Right" + s + "charKey")
        defaults.set(keymap.character[JoyDir.UP],      forKey :"VC64Up" + s + "charKey")
        defaults.set(keymap.character[JoyDir.DOWN],    forKey :"VC64Down" + s + "charKey")
        defaults.set(keymap.character[JoyDir.FIRE],    forKey :"VC64Fire" + s + "charKey")
    }
    
    //
    // Keyboard events
    //
    
    public func keyDown(with event: NSEvent)
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
        if (flags.contains(NSCommandKeyMask)) {
            return
        }
        
        // Pull joysticks if a key matches
        let f = fingerprint(forKey:keycode, withModifierFlags:flags)
        pullJoystick(ifKeyMatches: f);

        // Remove alternate key modifier if present
        if (flags.contains(NSAlternateKeyMask)) {
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
    
    public func keyUp(with event: NSEvent)
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

    public func flagsChanged(with event: NSEvent) {
        
        let flags = event.modifierFlags // NSEventModifierFlags
        var key: MacKeyFingerprint
        
        // Check if special keys are used for joystick emulation
        if (flags.contains(NSAlternateKeyMask)) {
            key = NSAlternateKeyMask.rawValue;
        } else if (flags.contains(NSShiftKeyMask)) {
            key = NSShiftKeyMask.rawValue;
        } else if (flags.contains(NSCommandKeyMask)) {
            key = NSCommandKeyMask.rawValue;
        } else if (flags.contains(NSControlKeyMask)) {
            key = NSControlKeyMask.rawValue;
        } else {
            // Release joytick
            releaseJoystick(ifKeyMatches: NSAlternateKeyMask.rawValue)
            releaseJoystick(ifKeyMatches: NSShiftKeyMask.rawValue)
            releaseJoystick(ifKeyMatches: NSCommandKeyMask.rawValue)
            releaseJoystick(ifKeyMatches: NSControlKeyMask.rawValue)
            releaseJoystick(ifKeyMatches: NSAlternateKeyMask.rawValue)
            return;
        }
        
        // Pull joysticks
        pullJoystick(ifKeyMatches: key)
    }
    

    
    /*! @brief  Computes unique fingerprint for a certain key combination pressed
     *          on the pyhsical Mac keyboard
     */
    func fingerprint(forKey keycode: UInt16, withModifierFlags flags: NSEventModifierFlags) -> MacKeyFingerprint
    {
        let result = Int(keycode)
        
        // For most keys, the recorded fingerprint simply consists of the keycode.
        // In case of number keys (0 - 9), the fingerprint might also contains the
        // NSNumericPadKeyMask flag to distinguish keys from the numeric keypad.
        
        /* THIS CODES NEEDS TESTING
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
        if (key == map.fingerprint[JoyDir.LEFT])       { j.setAxisX(JOYSTICK_LEFT) }
        else if (key == map.fingerprint[JoyDir.RIGHT]) { j.setAxisX(JOYSTICK_RIGHT) }
        else if (key == map.fingerprint[JoyDir.UP])    { j.setAxisY(JOYSTICK_UP) }
        else if (key == map.fingerprint[JoyDir.DOWN])  { j.setAxisY(JOYSTICK_DOWN) }
        else if (key == map.fingerprint[JoyDir.FIRE])  { j.setButtonPressed(true) }
        else { return false }
        
        return true
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
        if (key == map.fingerprint[JoyDir.LEFT])       { j.setAxisX(JOYSTICK_RELEASED) }
        else if (key == map.fingerprint[JoyDir.RIGHT]) { j.setAxisX(JOYSTICK_RELEASED) }
        else if (key == map.fingerprint[JoyDir.UP])    { j.setAxisY(JOYSTICK_RELEASED) }
        else if (key == map.fingerprint[JoyDir.DOWN])  { j.setAxisY(JOYSTICK_RELEASED) }
        else if (key == map.fingerprint[JoyDir.FIRE])  { j.setButtonPressed(false) }
        else { return false }
        
        return true
    }
    
    /*! @brief  Translates a pressed key on the Mac keyboard to a C64 key fingerprint
     *  @note   The returned value can be used as argument for the emulators pressKey() function
     */
    func translateKey(_ key: UInt8, plainkey: UInt8, keycode: UInt16, flags: NSEventModifierFlags) -> C64KeyFingerprint
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
            return (flags.contains(NSShiftKeyMask)) ?
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
            if (flags.contains(NSAlternateKeyMask)) {
                // Commodore key (ALT) is pressed
                return C64KeyFingerprint(Int(String(plainkey))! | C64KEY_COMMODORE);
            } else if (flags.contains(NSControlKeyMask)) {
                // CTRL key is pressed
                return C64KeyFingerprint(Int(String(plainkey))! | C64KEY_CTRL);
            }
        }
        
        // No special translation needed
        
        return C64KeyFingerprint(UInt8(key));
    }

}
