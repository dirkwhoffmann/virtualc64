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

class GamePad
{
    //! @brief    Keymap of the managed device
    /*! @details  Only used for keyboard emulated devices
     */
    var keyMap: [MacKey:UInt32]?
    
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

    //! @brief    Rescued information from the last invocation of the action function
    /*! @details  Used to determine if a joystick event needs to be triggered.
     */
    var oldUsage: [Int : Int] = [:]
    
    /// Reference to the GamePadManager
    var manager: GamePadManager
    
    init(manager: GamePadManager,
         vendorID: String?, productID: String?, locationID: String?) {
        
        self.manager = manager
        self.vendorID = vendorID
        self.productID = productID
        self.locationID = locationID
    }
    
    convenience init(manager: GamePadManager) {
        self.init(manager: manager, vendorID: nil, productID: nil, locationID: nil)
    }
    
    /// Assigns a keyboard emulation key
    func assign(key: MacKey, direction: JoystickDirection) {
        
        precondition(keyMap != nil)
        
        // Avoid double mappings
        for (k, dir) in keyMap! {
            if dir == direction.rawValue {
                keyMap![k] = nil
            }
        }
        keyMap![key] = direction.rawValue
    }
    
    //! @brief   Handles a keyboard down event
    /*! @details Checks if the provided keycode matches a joystick emulation key
     *           and triggeres an event if a match has been found.
     *  @result  Returns true if a joystick event has been triggered.
     */
    func keyDown(_ macKey: MacKey) -> Bool {
        
        if let direction = keyMap?[macKey] {

            var event: JoystickEvent
            
            switch (JoystickDirection(direction)) {
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
                assert(JoystickDirection(direction) == JOYSTICK_FIRE)
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
    func keyUp(_ macKey: MacKey) -> Bool
    {
        if let direction = keyMap?[macKey] {
            
            var event: JoystickEvent
            
            switch (JoystickDirection(direction)) {
            case JOYSTICK_UP, JOYSTICK_DOWN:
                event = RELEASE_Y
                break
            case JOYSTICK_LEFT, JOYSTICK_RIGHT:
                event = RELEASE_X
                break
            default:
                assert(JoystickDirection(direction) == JOYSTICK_FIRE)
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
        
        if v < -0.6 { return -2 };
        if v < -0.1 { return -1 };
        if v <= 0.1 { return 0 };
        if v <= 0.6 { return 1 };
        return 2;
    }
    
    
    func hidDeviceAction(context: Optional<UnsafeMutableRawPointer>,
                         result: IOReturn,
                         sender: Optional<UnsafeMutableRawPointer>,
                         value: IOHIDValue) {
    
        let element   = IOHIDValueGetElement(value)
        let intValue  = Int(IOHIDValueGetIntegerValue(value))
        let usagePage = Int(IOHIDElementGetUsagePage(element))
        let usage     = Int(IOHIDElementGetUsage(element))
        // let analog    = Float(IOHIDValueGetScaledValue(value, IOHIDValueScaleType(kIOHIDValueScaleTypeCalibrated)))
        
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
            
            
            if (v == -1 || v == 1) {
                return;
            }
            
            if oldUsage[usage] == v {
                return;
            } else {
                oldUsage[usage] = v
            }
            
            
            switch(usage) {
            case kHIDUsage_GD_X:
                track("kHIDUsage_GD_X \(intValue) \(v)")
                break
            case kHIDUsage_GD_Y:
                track("kHIDUsage_GD_Y \(intValue) \(v)")
                break
            case kHIDUsage_GD_Z:
                // track("kHIDUsage_GD_Z \(intValue) \(v)")
                break
            case kHIDUsage_GD_Rx:
                // track("kHIDUsage_GD_Rx \(intValue) \(v)")
                break
            case kHIDUsage_GD_Ry:
                // track("kHIDUsage_GD_Ry \(intValue) \(v)")
                break
            case kHIDUsage_GD_Rz:
                // track("kHIDUsage_GD_Rz \(intValue) \(v)")
                break
            case kHIDUsage_GD_Hatswitch:
                // track("kHIDUsage_GD_Hatswitch \(intValue) \(v)")
                break
            default:
                break
            }
            
            switch(usage) {
                
            case kHIDUsage_GD_X, kHIDUsage_GD_Rz:
                let event = (v == 2 ? PULL_RIGHT : v == -2 ? PULL_LEFT : RELEASE_X)
                manager.joystickEvent(self, event: event)
                return
                
            case kHIDUsage_GD_Y, kHIDUsage_GD_Z:
                let event = (v == 2 ? PULL_DOWN : v == -2 ? PULL_UP : RELEASE_Y)
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

