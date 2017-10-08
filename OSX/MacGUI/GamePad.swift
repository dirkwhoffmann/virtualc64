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
    
    //! @brief Maps key fingerprints to joystick events
    var mapping : [MacKeyFingerprint:JoystickDirection] = [:]
    
    //! @brief Stores a readable representation for each simulation key
    var character : [JoystickDirection:String] = [:]
    
    @objc public
    func fingerprint(for d: JoystickDirection) -> MacKeyFingerprint {
        
        for (fingerprint, direction) in mapping {
            if (direction == d) {
                return fingerprint
            }
        }
        return 0
    }
    
    @objc public
    func setFingerprint(_ f: MacKeyFingerprint, for d: JoystickDirection) {
        mapping[f] = d
    }
    
    @objc public
    func getCharacter(for d: JoystickDirection) -> String {
        return character[d] ?? ""
    }
    
    @objc public
    func setCharacter(_ c: String?, for d: JoystickDirection) {
        if (c != nil) {
            character[d] = c
        }
    }
}

class GamePad
{
    //! @brief    Indicates if this object represents a plugged in USB joystick device
    var pluggedIn: Bool
 
    //! @brief    Vendor ID of the managed device
    /*! @details  Value is only used for HID devices
     */
    var keymap: KeyMap?
    
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
    
    //! @brief   Handles a keyboard event
    /*! @details Checks if the provided keycode matches a joystick emulation key
     *           and triggeres an event if a match has been found.
     */
    @discardableResult
    func keyDown(key: MacKeyFingerprint) -> Bool
    {
        if let direction = keymap?.mapping[key] {
            joystick?.pullJoystick(direction)
            return true
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
                print("UNKNOWN HID-DEVICE USAGE:", usage)
            }
        }
    }
    
    //! @brief Connects the USB device to port A of the emulator
    func bindJoystickToPortA(c64: C64Proxy) {
        if (pluggedIn) {
            joystick = c64.joystickA
        }
    }
    
    //! @brief Connects the USB device to port B of the emulator
    func bindJoystickToPortB(c64: C64Proxy) {
        if (pluggedIn) {
            joystick = c64.joystickB
        }
    }
    
    //! @brief Unconnect USB device
    func unbindJoystick() {
        joystick = nil
    }
    
    func pullJoystick(dir: JoystickDirection) {
        joystick?.pullJoystick(dir)
        
    }
    
    func releaseJoystick(dir: JoystickDirection) {
        joystick?.releaseJoystick(dir)
    }
    
    func releaseXAxis() {
        joystick?.releaseXAxis()
    }
    
    func releaseYAxis() {
        joystick?.releaseYAxis()
    }
}



// ---------------------------------------------------------------------------------------------
//                                             IOHIDDeviceInfo
// ---------------------------------------------------------------------------------------------

// DEPRECATED, DELETE!
/*
class IOHIDDeviceInfo
{
    private var vendor = "";
    private var product = "";
    private var productID = "";
    private var locationID = "";
    
    let vendorKey = kIOHIDVendorIDKey as CFString
    let productKey = kIOHIDProductKey as CFString
    let productIDKey = kIOHIDProductIDKey as CFString
    let locationIDKey = kIOHIDLocationIDKey as CFString
    
    convenience init (device: IOHIDDevice) {
        
        NSLog("\(#function)")
        self.init()
        
        vendor     = String(describing: IOHIDDeviceGetProperty(device, vendorKey))
        product    = String(describing: IOHIDDeviceGetProperty(device, productKey))
        productID  = String(describing: IOHIDDeviceGetProperty(device, productIDKey))
        locationID = String(describing: IOHIDDeviceGetProperty(device, locationIDKey))
        
        NSLog(vendor)
        NSLog(product)
        NSLog(productID)
        NSLog(locationID)
    }
}
*/
