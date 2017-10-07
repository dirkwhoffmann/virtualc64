//
//  GamePad.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 07.10.17.
//

import Foundation
import IOKit.hid

// ---------------------------------------------------------------------------------------------
//                                             GamePad
// ---------------------------------------------------------------------------------------------

class GamePad
{
    // private var proxy: C64Proxy?
    
    //! @brief    Indicates if this object represents a plugged in USB joystick device
    var pluggedIn: Bool;
    
    //! @brief    Location ID of the represented USB joystick
    var locationID: String;
    
    //! @brief    Mapping to one of the two virtual joysticks of the emulator
    /*! @details  Initially, this pointer is NULL, meaning that the USB joystick has not yet been selected
     *            as input device. It can be selected as input device via bindJoystick(). In that case, it
     *            will point to one of the two static Joystick objects hold by the emulator.
     */
    var joystick: JoystickProxy?;
    
    init() {
        pluggedIn = false;
        locationID = "";
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
            
            /*
             switch(usage) {
             
             case kHIDUsage_Button_1, kHIDUsage_Button_2,
             kHIDUsage_Button_3, kHIDUsage_Button_4:
             joystick?.setButton(intValue)
             
             default: ()
             }
             */

            joystick?.setButton(intValue)
            
        }
        
        if (usagePage == kHIDPage_GenericDesktop) {
            
            let v = mapAnalogAxis(value: value, element: element)
            
            switch(usage) {
                
            case kHIDUsage_GD_X, kHIDUsage_GD_Rz:
                print("X: ", v)
                joystick?.setXAxis(v)
                break
                
            case kHIDUsage_GD_Y, kHIDUsage_GD_Z:
                print("Y: ", v)
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
    
    func pullJoystick(dir: GamePadDirection) {
        joystick?.pullJoystick(dir)
        
    }
    
    func releaseJoystick(dir: GamePadDirection) {
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
