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

//! @brief   Mapping from keycodes to joystick movements
/*! @details Each GamePad can be assigned a KeyMap which can be used
 *           to trigger events by using the keyboard.
 */
public class KeyMap: NSObject {
    
    //! @brief Mapping of fingerprints to joystick events
    var mapping : [MacKeyFingerprint:JoystickDirection] = [:]

    //! @brief Mapping of joystick events to readable representations of related fingerprints
    var character : [Int:String] = [:]
    
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
        
        return character[Int(d.rawValue)] ?? ""
    }
    
    @objc public
    func setCharacter(_ c: String?, for d: JoystickDirection) {
        
        character[Int(d.rawValue)] = c
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

    //! @brief    Reference to the GamePadManager
    var manager: GamePadManager
    
    //! @brief    Constructor
    init(manager: GamePadManager,
         vendorID: String?, productID: String?, locationID: String?) {
        
        self.manager = manager
        self.vendorID = vendorID
        self.productID = productID
        self.locationID = locationID
    }
    
    //! @brief    Convenience constructor
    convenience init(manager: GamePadManager) {
        self.init(manager: manager, vendorID: nil, productID: nil, locationID: nil)
    }
    
    //! @brief   Handles a keyboard down event
    /*! @details Checks if the provided keycode matches a joystick emulation key
     *           and triggeres an event if a match has been found.
     *  @result  Returns true if a joystick event has been triggered.
     */
    func keyDown(_ key: MacKeyFingerprint) -> Bool {
        
        if let direction = keymap.mapping[key] {

            var event: JoystickEvent
            
            switch (direction) {
            case JOYSTICK_UP:
                event = PULL_UP
                break
            case  JOYSTICK_DOWN:
                event = PULL_DOWN
                break
            case JOYSTICK_LEFT:
                event = PULL_LEFT
                break
            case JOYSTICK_RIGHT:
                event = PULL_RIGHT
                break
            default:
                assert(direction == JOYSTICK_FIRE)
                event = PRESS_FIRE
            }
            
            return manager.joystickEvent(self, event: event)
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
        if let direction = keymap.mapping[key] {
            
            var event: JoystickEvent
            
            switch (direction) {
            case JOYSTICK_UP, JOYSTICK_DOWN:
                event = RELEASE_Y
                break
            case JOYSTICK_LEFT, JOYSTICK_RIGHT:
                event = RELEASE_X
                break
            default:
                assert(direction == JOYSTICK_FIRE)
                event = RELEASE_FIRE
            }
            
            return manager.joystickEvent(self, event: event)
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
    
        let element   = IOHIDValueGetElement(value)
        let intValue  = Int(IOHIDValueGetIntegerValue(value))
        let usagePage = Int(IOHIDElementGetUsagePage(element))
        let usage     = Int(IOHIDElementGetUsage(element))
        
        // Check button
        if usagePage == kHIDPage_Button {
            manager.joystickEvent(self, event: (intValue != 0) ? PRESS_FIRE : RELEASE_FIRE)
            return
            // event = (intValue != 0) ? PRESS_FIRE : RELEASE_FIRE
        }
        
        // Check movement
        if (usagePage == kHIDPage_GenericDesktop) {
            
            // var event: JoystickEvent
            let v = mapAnalogAxis(value: value, element: element)
            
            switch(usage) {
                
            case kHIDUsage_GD_X, kHIDUsage_GD_Rz:
                let event = (v == 1 ? PULL_RIGHT : (v == -1 ? PULL_LEFT : RELEASE_X))
                manager.joystickEvent(self, event: event)
                return
                
            case kHIDUsage_GD_Y, kHIDUsage_GD_Z:
                let event = (v == 1 ? PULL_DOWN : (v == -1 ? PULL_UP : RELEASE_Y))
                manager.joystickEvent(self, event: event)
                return
                
            case kHIDUsage_GD_Hatswitch:
                // The following values are based on Saitek's
                // "Impact Dual Analog Rumble Pad".
                // Not sure if this works for other controllers
                if intValue == 8 || intValue == 1 || intValue == 2 {
                    manager.joystickEvent(self, event: PULL_UP)
                    return
                }
                if intValue == 2 || intValue == 3 || intValue == 4 {
                    manager.joystickEvent(self, event: PULL_RIGHT)
                    return
                }
                if intValue == 4 || intValue == 5 || intValue == 6 {
                    manager.joystickEvent(self, event: PULL_DOWN)
                    return
                }
                if intValue == 6 || intValue == 7 || intValue == 8 {
                    manager.joystickEvent(self, event: PULL_LEFT)
                    return
                }
                if intValue == 0 {
                    manager.joystickEvent(self, event: RELEASE_XY)
                    return
                }
                break
                
            default:
                print("USB device: Unknown HID usage: \(usage)")
            }
        }
    }
    
}

