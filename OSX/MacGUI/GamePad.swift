//
//  GamePad.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 07.10.17.
//

import Foundation
import IOKit.hid

//! @brief   Mapping from keycodes to joystick movements
/*! @details Each GamePad can be assigned a KeyMap which can be used
 *           to trigger events by using the keyboard.
 */
public class KeyMap: NSObject {
    
    //! @brief Maps joystick events to key fingerprints
    var mapping : [JoystickDirection:MacKeyFingerprint] = [:]
    
    //! @brief Maps joystick events to readable representations of the fingerprint
    var character : [JoystickDirection:String] = [:]
    
    /*
    func printMapping() {
        print("UP: ", mapping[JoystickDirection.UP] ?? "",
              " (", character[JoystickDirection.UP] ?? "", ")",
              getCharacter(for: JoystickDirection.UP))
        print("DOWN: ", mapping[JoystickDirection.DOWN] ?? "",
              " (", character[JoystickDirection.DOWN] ?? "", ")")
        print("LEFT: ", mapping[JoystickDirection.LEFT] ?? "",
              " (", character[JoystickDirection.LEFT] ?? "", ")")
        print("RIGHT: ", mapping[JoystickDirection.RIGHT] ?? "",
              " (", character[JoystickDirection.RIGHT] ?? "", ")")
        print("FIRE: ", mapping[JoystickDirection.FIRE] ?? "",
              " (", character[JoystickDirection.FIRE] ?? "", ")")
    }
    */
    
    @objc public
    func fingerprint(for d: JoystickDirection) -> MacKeyFingerprint {
        return mapping[d] ?? 0
    }
    
    @objc public
    func setFingerprint(_ f: MacKeyFingerprint, for d: JoystickDirection) {
        
        // Avoid double mappings
        for (direction, fingerprint) in mapping {
            if (fingerprint == f) {
                mapping[direction] = nil
                character[direction] = ""
            }
        }
        
        mapping[d] = f
    }
    
    @objc public
    func getCharacter(for d: JoystickDirection) -> String {
        return character[d] ?? ""
    }
    
    @objc public
    func setCharacter(_ c: String?, for d: JoystickDirection) {
        character[d] = c!
    }
}

class GamePad
{
    //! @brief    Indicates if this object represents a plugged in USB joystick device
    var pluggedIn: Bool
 
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
    
    init() {
        pluggedIn = false
        locationID = ""
    }
    
    //! @brief   Handles a keyboard down event
    /*! @details Checks if the provided keycode matches a joystick emulation key
     *           and triggeres an event if a match has been found.
     */
    func keyDown(_ key: MacKeyFingerprint)
    {
        if (keymap.mapping[JoystickDirection.UP] == key) {
            joystick?.pullJoystick(JoystickDirection.UP)
        } else if (keymap.mapping[JoystickDirection.DOWN] == key) {
            joystick?.pullJoystick(JoystickDirection.DOWN)
        } else if (keymap.mapping[JoystickDirection.LEFT] == key) {
            joystick?.pullJoystick(JoystickDirection.LEFT)
        } else if (keymap.mapping[JoystickDirection.RIGHT] == key) {
            joystick?.pullJoystick(JoystickDirection.RIGHT)
        } else if (keymap.mapping[JoystickDirection.FIRE] == key) {
            joystick?.pullJoystick(JoystickDirection.FIRE)
        }
    }
    
    //! @brief   Handles a keyboard up event
    /*! @details Checks if the provided keycode matches a joystick emulation key
     *           and triggeres an event if a match has been found.
     */
    func keyUp(_ key: MacKeyFingerprint)
    {
        if (keymap.mapping[JoystickDirection.UP] == key) {
            joystick?.releaseJoystick(JoystickDirection.UP)
        } else if (keymap.mapping[JoystickDirection.DOWN] == key) {
            joystick?.releaseJoystick(JoystickDirection.DOWN)
        } else if (keymap.mapping[JoystickDirection.LEFT] == key) {
            joystick?.releaseJoystick(JoystickDirection.LEFT)
        } else if (keymap.mapping[JoystickDirection.RIGHT] == key) {
            joystick?.releaseJoystick(JoystickDirection.RIGHT)
        } else if (keymap.mapping[JoystickDirection.FIRE] == key) {
            joystick?.releaseJoystick(JoystickDirection.FIRE)
        }
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
        if (v < -0.1) { return -1 };
        if (v > 0.1) { return 1 };
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
        
        if (usagePage == kHIDPage_Button) {
            
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
                
                if (intValue == 8 || intValue == 1 || intValue == 2) {
                    joystick?.pullUp()
                }
                if (intValue == 2 || intValue == 3 || intValue == 4) {
                    joystick?.pullRight()
                }
                if (intValue == 4 || intValue == 5 || intValue == 6) {
                    joystick?.pullDown()
                }
                if (intValue == 6 || intValue == 7 || intValue == 8) {
                    joystick?.pullLeft()
                }
                if (intValue == 0) {
                    joystick?.releaseAxes()
                }
                break
                
            default:
                print("USB device: Unknown HID usage", usage)
            }
        }
    }

}

