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

// TODO: USE C64 STRUCT, BECAUSE IT'S ALREADY NEEDED THERE!!!!
@objc enum JoyDir : Int {
    case UP
    case DOWN
    case LEFT
    case RIGHT
    case FIRE
    case RELEASED
}

//! @brief Mapping from keyboard keys to joystick movements
public class KeyMap: NSObject {

    //! @brief fingerprint of assign key
    var fingerprint : [JoyDir:MacKeyFingerprint] = [:]

    //! @brief readable charackter for assigned key
    var character : [JoyDir:Int8] = [:]
    
    func fingerprintForJoyDir(_ d: JoyDir) -> MacKeyFingerprint
    {
        return fingerprint[d]!
    }
    
    func setFingerprint(_ f: MacKeyFingerprint, forJoyDir d: JoyDir)
    {
        fingerprint[d] = f
    }
    
    func characterForJoyDir(_ d: JoyDir) -> Int8
    {
        return character[d]!
    }
    
    func setCharacter(_ c: Int8, forJoyDir d: JoyDir)
    {
        character[d] = c
    }
}

//!@ brief Keyboard event handler
extension MyController
{
    
    //
    // Keyboard events
    //
    
    override open func keyDown(with event: NSEvent)
    {
        // Exit fullscreen mode if ESC is pressed
        if (event.keyCode == MAC_ESC && metalScreen.fullscreen) {
            window!.toggleFullScreen(nil)
        }
        
        // Pass all keyboard events to the metal view
        metalScreen.keyDown(with: event)
    }
    
    override open func keyUp(with event: NSEvent)
    {
        metalScreen.keyUp(with: event)
    }

    override open func flagsChanged(with event: NSEvent) {
        metalScreen.flagsChanged(with: event)
    }
    
    //
    // Joystick simulation
    //

    func loadUserDefaultsKeyboard(keymap: KeyMap, nr: Int) {
        
        NSLog("\(#function)")
        assert (nr == 0 || nr == 1)
        let s = (nr == 1 ? "0" : "1")
        
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
            Int8(defaults.integer(forKey: "VC64Left" + s + "charKey"))
        keymap.character[JoyDir.RIGHT] =
            Int8(defaults.integer(forKey: "VC64Right" + s + "charKey"))
        keymap.character[JoyDir.UP] =
            Int8(defaults.integer(forKey: "VC64Up" + s + "charKey"))
        keymap.character[JoyDir.DOWN] =
            Int8(defaults.integer(forKey: "VC64Down" + s + "charKey"))
        keymap.character[JoyDir.FIRE] =
            Int8(defaults.integer(forKey: "VC64Fire" + s + "charKey"))
    }

    func saveUserDefaultsKeyboard(keymap1: KeyMap, nr: Int) {
        
        NSLog("\(#function)")
        assert (nr == 0 || nr == 1)
        let s = (nr == 1 ? "0" : "1")
        
        let defaults = UserDefaults.standard
        
        defaults.set(keymap1.fingerprint[JoyDir.LEFT],  forKey :"VC64Left" + s + "keycodeKey")
        defaults.set(keymap1.fingerprint[JoyDir.RIGHT], forKey :"VC64Right" + s + "keycodeKey")
        defaults.set(keymap1.fingerprint[JoyDir.UP],    forKey :"VC64Up" + s + "keycodeKey")
        defaults.set(keymap1.fingerprint[JoyDir.DOWN],  forKey :"VC64Down" + s + "keycodeKey")
        defaults.set(keymap1.fingerprint[JoyDir.FIRE],  forKey :"VC64Fire" + s + "keycodeKey")
        
        defaults.set(keymap1.character[JoyDir.LEFT],    forKey :"VC64Left" + s + "charKey")
        defaults.set(keymap1.character[JoyDir.RIGHT],   forKey :"VC64Right" + s + "charKey")
        defaults.set(keymap1.character[JoyDir.UP],      forKey :"VC64Up" + s + "charKey")
        defaults.set(keymap1.character[JoyDir.DOWN],    forKey :"VC64Down" + s + "charKey")
        defaults.set(keymap1.character[JoyDir.FIRE],    forKey :"VC64Fire" + s + "charKey")
    }
    
    /*! @brief  Computes unique fingerprint for a certain key combination pressed
     *          on the pyhsical Mac keyboard
     */
    func fingerprint(forKey keycode: Int32, withModifierFlags flags: UInt) -> MacKeyFingerprint
    {
        let result = Int(keycode)
        
        // For most keys, the recorded fingerprint simply consists of the keycode.
        // In case of number keys (0 - 9), the fingerprint might also contains the
        // NSNumericPadKeyMask flag to distinguish keys from the numeric keypad.
        
        /* THIS CODES NEEDS TESTING
        switch (keycode) {
        case Int32(kVK_ANSI_Keypad0),
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

#if false
    //! @brief  Returns fingerprint from keymap
    func joyFingerprint(forKeymap nr: Int32, direction d: JoystickDirection) -> MacKeyFingerprint
    {
        assert(d.rawValue >= 0 && d.rawValue <= 4)
        
        
        switch (nr) {
        //        case 1: return self.joyKeymap[0][d]
        //        case 2: return joyFingerprint[1][d]
        default: assert(false); return 0
        }
    }
    
    //! @brief  Stores fingerprint in keymap
    func setJoyFingerprint(_ fingerprint: MacKeyFingerprint, forKeymap map: Int32, direction d: JoystickDirection)
    {
        
    }
    
    //! @brief  Returns keymap entry as a printable charakter
    func joyChar(forKeymap nr: Int32, direction d: JoystickDirection) -> Int8
    {
        
        return 0
    }
    
    //! @brief  Stores a printable charakter in keymap
    func setJoyChar(_ c: Int8, forKeymap map: Int32, direction d: JoystickDirection)
    {
        
    }
#endif
    
    /*! @brief  Pulls joystick if key matches some value stored in keymap
     */
    func pullJoystick(_ joynr: Int32, ifKeyMatches key: MacKeyFingerprint, inKeymap map: KeyMap) -> Bool
    {
        assert (joynr == 1 || joynr == 2);
        
        let j = (joynr == 1 ? c64.joystickA : c64.joystickB)!
        
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
    func releaseJoystick(_ joynr: Int32, ifKeyMatches key: MacKeyFingerprint, inKeymap map: KeyMap) -> Bool
    {
        assert (joynr == 1 || joynr == 2);
        
        let j = (joynr == 1 ? c64.joystickA : c64.joystickB)!
        
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
    func translateKey(_ key: Int8, plainkey: Character, keycode: UInt16, flags: NSEventModifierFlags) -> C64KeyFingerprint
    {
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
            return C64KeyFingerprint(Int("^")!) // TODO: NEEDS TESTING IN PLAYGROUND
        
        case MAC_TILDE_US:
            if (plainkey != "<" && plainkey != ">") {
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
        return C64KeyFingerprint(key);
    }

}
