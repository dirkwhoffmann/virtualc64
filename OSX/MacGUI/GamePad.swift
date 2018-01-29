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
import IOKit.hid

/*
struct JoystickDirection {
    static let up = 0
    static let down = 1
    static let left = 2
    static let right = 3
    static let fire = 5
    static let released = 6
}
*/

//! @brief   Mapping from keycodes to joystick movements
/*! @details Each GamePad can be assigned a KeyMap which can be used
 *           to trigger events by using the keyboard.
 */
public class KeyMap: NSObject {
    
    //! @brief Mapping of fingerprints to joystick events
    var mapping : [MacKeyFingerprint:JoystickDirection] = [:]

    //! @brief Mapping of joystick events to readable representations of related fingerprints
    var character : [JoystickDirection:String] = [:]
    
    @objc public
    func fingerprint(for d: JoystickDirection) -> MacKeyFingerprint {
        
        for (fingerprint, direction) in mapping {
            if direction == d {
                return fingerprint;
            }
        }
        return 0;
    }
    
    @objc public
    func setFingerprint(_ f: MacKeyFingerprint, for d: JoystickDirection) {
        
        // Avoid double mappings
        for (fingerprint, direction) in mapping {
            if direction == d {
                mapping[fingerprint] = nil
            }
        }
        mapping[f] = d
    }
    
    @objc public
    func getCharacter(for d: JoystickDirection) -> String {
        return character[d] ?? ""
    }
    
    @objc public
    func setCharacter(_ c: String?, for d: JoystickDirection) {
        character[d] = c
    }
}

class GamePad
{
    //! @brief    Keymap of the managed device
    /*! @details  Only used for keyboard emulated devices
     */
    var keymap: KeyMap = KeyMap()
    
    //! @brief    Vendor ID of the managed device
    /*! @details  Value is only used for HID devices
     */
    var vendorID: String?

    //! @brief    Product ID of the managed device
    /*! @details  Value is only used for HID devices
     */
    var productID: String?

    //! @brief    Location ID of the managed device
    /*! @details  Value is only used for HID devices
     */
    var locationID: String?

    //! @brief    Reference to the C64 game port
    /*! @details  Each triggered event will be passed to this object.
     *            Hence, this object is the entry point to the core emulator.
     *            nil means that the device is unconnected.
     */
    var joystick: JoystickProxy?
    
    convenience init(vendorID: String?, productID: String?, locationID: String?) {
        
        self.init()
        self.vendorID = vendorID
        self.productID = productID
        self.locationID = locationID
    }
    
    //! @brief   Handles a keyboard down event
    /*! @details Checks if the provided keycode matches a joystick emulation key
     *           and triggeres an event if a match has been found.
     *  @result  Returns true if a joystick event has been triggered.
     */
    func keyDown(_ key: MacKeyFingerprint) -> Bool
    {
        let map = [JoystickDirection.UP: joystick?.pullUp,
                   JoystickDirection.DOWN: joystick?.pullDown,
                   JoystickDirection.LEFT: joystick?.pullLeft,
                   JoystickDirection.RIGHT: joystick?.pullRight,
                   JoystickDirection.FIRE: joystick?.pressButton]
        
        if joystick != nil {
            if let dir = keymap.mapping[key] {
                if let f = map[dir] {
                    f?()
                    return true
                }
            }
        }
        
        return false
    }
    
    //! @brief   Handles a keyboard up event
    /*! @details Checks if the provided keycode matches a joystick emulation key
     *           and triggeres an event if a match has been found.
     *  @result  Returns true if a joystick event has been triggered.
     */
    func keyUp(_ key: MacKeyFingerprint) -> Bool
    {
        let map = [JoystickDirection.UP: joystick?.releaseYAxis,
                   JoystickDirection.DOWN: joystick?.releaseYAxis,
                   JoystickDirection.LEFT: joystick?.releaseXAxis,
                   JoystickDirection.RIGHT: joystick?.releaseXAxis,
                   JoystickDirection.FIRE: joystick?.releaseButton]
        
        if joystick != nil {
            if let dir = keymap.mapping[key] {
                if let f = map[dir] {
                    f?()
                    return true
                }
            }
        }
        
        return false
    }
    
    let actionCallback : IOHIDValueCallback = { inContext, inResult, inSender, value in
        let this : GamePad = unsafeBitCast(inContext, to: GamePad.self)
        this.hidDeviceAction(context: inContext, result: inResult, sender: inSender, value: value)
    }
    
    // Based on http://docs.ros.org/hydro/api/oculus_sdk/html/OSX__Gamepad_8cpp_source.html#l00170
    func mapAnalogAxis(value: IOHIDValue, element: IOHIDElement) -> Int {
        
        let val = IOHIDValueGetIntegerValue(value);
        let min = IOHIDElementGetLogicalMin(element);
        let max = IOHIDElementGetLogicalMax(element);
        
        var v = (Double) (val - min) / (Double) (max - min);
        v = v * 2.0 - 1.0;
        
        // Considering [ -0.1 ; 0.1 ] as "released state"
        if v < -0.1 { return -1 };
        if v > 0.1 { return 1 };
        return 0;
    }
    
    
    func hidDeviceAction(context: Optional<UnsafeMutableRawPointer>,
                         result: IOReturn,
                         sender: Optional<UnsafeMutableRawPointer>,
                         value: IOHIDValue) {
        
        print("\(#function) joystick = \(joystick) self = \(self)")
        
        let element   = IOHIDValueGetElement(value)
        let intValue  = Int(IOHIDValueGetIntegerValue(value))
        let usagePage = Int(IOHIDElementGetUsagePage(element))
        let usage     = Int(IOHIDElementGetUsage(element))
        
        if usagePage == kHIDPage_Button {
            
            joystick?.setButton(intValue)
        }
        
        if (usagePage == kHIDPage_GenericDesktop) {
            
            let v = mapAnalogAxis(value: value, element: element)
            
            switch(usage) {
                
            case kHIDUsage_GD_X, kHIDUsage_GD_Rz:
                joystick?.setXAxis(v)
                break
                
            case kHIDUsage_GD_Y, kHIDUsage_GD_Z:
                joystick?.setYAxis(v)
                break
                
            case kHIDUsage_GD_Hatswitch:
                
                // The following values are based on Saitek's
                // "Impact Dual Analog Rumble Pad".
                // Not sure if this works for other controllers
                
                if intValue == 8 || intValue == 1 || intValue == 2 {
                    joystick?.pullUp()
                }
                if intValue == 2 || intValue == 3 || intValue == 4 {
                    joystick?.pullRight()
                }
                if intValue == 4 || intValue == 5 || intValue == 6 {
                    joystick?.pullDown()
                }
                if intValue == 6 || intValue == 7 || intValue == 8 {
                    joystick?.pullLeft()
                }
                if intValue == 0 {
                    joystick?.releaseAxes()
                }
                break
                
            default:
                print("USB device: Unknown HID usage", usage)
            }
        }
    }

}

