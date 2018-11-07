//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation
import IOKit.hid

//
// A GamePad object is created for each connected USB device
// Creation and destruction is done by the GamePadManager
//

class GamePad
{
    //! @brief    Keymap of the managed device
    /*! @details  Only used for keyboard emulated devices
     */
    var keyMap: [MacKey:UInt32]?
    
    //! @brief    Name of the connected controller
    var name: String?
    
    //! @brief    Image of the connected controller
    // var image: NSImage?
    
    //! @brief    Vendor ID of the managed device
    /*! @details  Value is only used for HID devices
     */
    var vendorID: Int

    //! @brief    Product ID of the managed device
    /*! @details  Value is only used for HID devices
     */
    var productID: Int

    //! @brief    Location ID of the managed device
    /*! @details  Value is only used for HID devices
     */
    var locationID: Int

    //! @brief    Minimum value of analog axis event
    var min : Int?
    
    //! @brief    Maximum value of analog axis event
    var max : Int?
    
    //! @brief    Rescued information from the last invocation of the action function
    /*! @details  Used to determine if a joystick event needs to be triggered.
     */
    var oldEvents: [Int : [JoystickEvent]] = [:]
    
    //! @brief    Cotroller dependent usage IDs for left and right gamepad joysticks
    var lThumbXUsageID = kHIDUsage_GD_X;
    var lThumbYUsageID = kHIDUsage_GD_Y;
    var rThumbXUsageID = kHIDUsage_GD_Rz;
    var rThumbYUsageID = kHIDUsage_GD_Z;

    /// Reference to the GamePadManager
    var manager: GamePadManager
    
    init(manager: GamePadManager,
         vendorID: Int, productID: Int, locationID: Int) {
        
        track();
        
        self.manager = manager
        self.vendorID = vendorID
        self.productID = productID
        self.locationID = locationID
    
        // Check for known devices
        if (vendorID == 0x40B && productID == 0x6533) {
            
            name = "Competition Pro SL-6602"
        
        } else if (vendorID == 0x54C && productID == 0x268) {

            name = "Sony DualShock 3"
            rThumbXUsageID = kHIDUsage_GD_Z;
            rThumbYUsageID = kHIDUsage_GD_Rz;
        
        } else if (vendorID == 0x54C && productID == 0x5C4) {
            
            name = "Sony DualShock 4"
            rThumbXUsageID = kHIDUsage_GD_Z;
            rThumbYUsageID = kHIDUsage_GD_Rz;

        } else if (vendorID == 0x54C && productID == 0x9CC) {
            
            name = "Sony Dualshock 4 (2nd Gen)"
            rThumbXUsageID = kHIDUsage_GD_Z;
            rThumbYUsageID = kHIDUsage_GD_Rz;
        
        } else if (vendorID == 0x483 && productID == 0x9005) {
            
            name = "RetroFun! Joystick Adapter"

        } else if (vendorID == 0x004 && productID == 0x0001) {
            
            name = "aJoy Retro Adapter"
            
        } else {
        
            // name = "Generic Gamepad"
        }
    }
    
    convenience init(manager: GamePadManager) {
        self.init(manager: manager, vendorID: 0, productID: 0, locationID: 0)
    }
    
    let actionCallback : IOHIDValueCallback = { inContext, inResult, inSender, value in
        let this : GamePad = unsafeBitCast(inContext, to: GamePad.self)
        this.hidDeviceAction(context: inContext, result: inResult, sender: inSender, value: value)
    }
}

//
// Keyboard emulation
//

extension GamePad {
    
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

            var events: [JoystickEvent]
            
            switch (JoystickDirection(direction)) {
                
            case JOYSTICK_UP:
                events = [PULL_UP]
                
            case  JOYSTICK_DOWN:
                events = [PULL_DOWN]
                
            case JOYSTICK_LEFT:
                events = [PULL_LEFT]
                
            case JOYSTICK_RIGHT:
                events = [PULL_RIGHT]
                
            case JOYSTICK_FIRE:
                events = [PRESS_FIRE]

            default:
                fatalError()
            }
            
            return manager.joystickEvent(self, events: events)
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
            
            var events: [JoystickEvent]
            
            switch (JoystickDirection(direction)) {
            case JOYSTICK_UP, JOYSTICK_DOWN:
                events = [RELEASE_Y]
                
            case JOYSTICK_LEFT, JOYSTICK_RIGHT:
                events = [RELEASE_X]
                
            case JOYSTICK_FIRE:
                events = [RELEASE_FIRE]
                
            default:
                fatalError()
            }
            
            return manager.joystickEvent(self, events: events)
        }
    
        return false
    }
}

//
// Event handling
//

extension GamePad {
    
 
    
    // Based on http://docs.ros.org/hydro/api/oculus_sdk/html/OSX__Gamepad_8cpp_source.html#l00170
    func mapAnalogAxis(value: IOHIDValue, element: IOHIDElement) -> Int? {
        
        if min == nil {
            min = IOHIDElementGetLogicalMin(element)
            track("Minumum axis value = \(min!)")
        }
        if max == nil {
            max = IOHIDElementGetLogicalMax(element)
            track("Maximum axis value = \(max!)")
        }
        let val = IOHIDValueGetIntegerValue(value)
        
        var v = (Double) (val - min!) / (Double) (max! - min!);
        v = v * 2.0 - 1.0;
        if v < -0.45 { return -2 };
        if v < -0.1 { return nil };  // dead zone
        if v <= 0.1 { return 0 };
        if v <= 0.45 { return nil }; // dead zone
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
        
        // Buttons
        if usagePage == kHIDPage_Button {
            // track("BUTTON")
            manager.joystickEvent(self, events: (intValue != 0) ? [PRESS_FIRE] : [RELEASE_FIRE])
            return
        }
        
        // Stick
        if (usagePage == kHIDPage_GenericDesktop) {
            
            var events: [JoystickEvent]?
            
            switch(usage) {
                
            case lThumbXUsageID, rThumbXUsageID:
                
                // track("lThumbXUsageID, rThumbXUsageID: \(usage) \(intValue)")
                if let v = mapAnalogAxis(value: value, element: element) {
                    events = (v == 2) ? [PULL_RIGHT] : (v == -2) ? [PULL_LEFT] : [RELEASE_X]
                }
                break
   
            case lThumbYUsageID, rThumbYUsageID:
                
                // track("lThumbYUsageID, rThumbYUsageID: \(intValue)")
                if let v = mapAnalogAxis(value: value, element: element) {
                    events = (v == 2) ? [PULL_DOWN] : (v == -2) ? [PULL_UP] : [RELEASE_Y]
                }
                break
                
            case kHIDUsage_GD_Hatswitch:
                
                // track("kHIDUsage_GD_Hatswitch \(intValue)")
                switch intValue {
                case 0: events = [PULL_UP, RELEASE_X]; break
                case 1: events = [PULL_UP, PULL_RIGHT]; break
                case 2: events = [PULL_RIGHT, RELEASE_Y]; break
                case 3: events = [PULL_RIGHT, PULL_DOWN]; break
                case 4: events = [PULL_DOWN, RELEASE_X]; break
                case 5: events = [PULL_DOWN, PULL_LEFT]; break
                case 6: events = [PULL_LEFT, RELEASE_Y]; break
                case 7: events = [PULL_LEFT, PULL_UP]; break
                case 8: events = [RELEASE_XY]; break
                default: break
                }
                
            default:
                // track("Unknown HID usage: \(usage)")")
                break
            }
            
            // Only proceed if the event is different than the previous one
            if events == nil || oldEvents[usage] == events {
                return
            } else {
                oldEvents[usage] = events!
            }
            
            // Trigger event
            manager.joystickEvent(self, events: events!)
        }
    }
}

