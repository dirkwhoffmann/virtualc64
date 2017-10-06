//
//  GamePadManager.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 06.10.17.
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
    
    /*
    @objc convenience init?(withC64 p: C64Proxy) {
        
        NSLog("\(#function)")
        
        self.init()
        proxy = p
    }
    */
    
    let actionCallback : IOHIDValueCallback = { inContext, inResult, inSender, value in
        let this : GamePad = unsafeBitCast(inContext, to: GamePad.self)
        this.hidDeviceAction(context: inContext, result: inResult, sender: inSender, value: value)
    }
    
    // http://docs.ros.org/hydro/api/oculus_sdk/html/OSX__Gamepad_8cpp_source.html#l00170
    func mapAnalogAxis(value: IOHIDValue, element: IOHIDElement) -> Double {
    
        let val = IOHIDValueGetIntegerValue(value);
        let min = IOHIDElementGetLogicalMin(element);
        let max = IOHIDElementGetLogicalMax(element);
    
        var v = (Double) (val - min) / (Double) (max - min);
        v = v * 2.0 - 1.0;
    
        // Map dead zone to 0
        if (v > -0.1 && v < 0.1) {
             v = 0.0
         }
    
         return v
     }
    
    
    func hidDeviceAction(context: Optional<UnsafeMutableRawPointer>,
                         result: IOReturn,
                         sender: Optional<UnsafeMutableRawPointer>,
                         value: IOHIDValue) {
        
        // NSLog("\(#function) (location ID = %@)", locationID)
        
        let element = IOHIDValueGetElement(value)
        let elementType = IOHIDElementGetType(element)
        let elementValue = IOHIDValueGetIntegerValue(value)
        let elementValuee = IOHIDValueGetScaledValue(value, IOHIDValueScaleType(kIOHIDValueScaleTypePhysical))
        let usagePage = Int(IOHIDElementGetUsagePage(element))
        let usage = Int(IOHIDElementGetUsage(element))
        let min = IOHIDElementGetLogicalMin(element)
        let max = IOHIDElementGetLogicalMax(element)

        if (elementType == kIOHIDElementTypeInput_Button) {
            
            print("BUTTON")
            if (elementValue == 1) {
                joystick?.pullJoystick(GamePadDirection.FIRE);
            } else {
                joystick?.releaseJoystick(GamePadDirection.FIRE);
            }
        }
        
        if (elementType == kIOHIDElementTypeInput_Misc) {
            
            let v = mapAnalogAxis(value: value, element: element)
            let axis = (v == 0) ? 0 : ((v > 0) ? 1 : -1)
            
            switch(usage) {
                
            case kHIDUsage_GD_X:
            
                switch (axis) {
                case -1: print("LEFT"); joystick?.pullJoystick(GamePadDirection.LEFT); return;
                case  1: print("RIGHT"); joystick?.pullJoystick(GamePadDirection.RIGHT); return;
                default: joystick?.releaseXAxis(); return;
                }
                
            case kHIDUsage_GD_Y:
                
                switch (axis) {
                case -1: print("UP"); joystick?.pullJoystick(GamePadDirection.UP); return;
                case  1: print("DOWN"); joystick?.pullJoystick(GamePadDirection.DOWN); return;
                default: joystick?.releaseYAxis(); return;
                }
                
            default:
                break
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

// ---------------------------------------------------------------------------------------------
//                                             GamePadManager
// ---------------------------------------------------------------------------------------------

@objc class GamePadManager: NSObject {
    
    // private let inputLock = NSLock()
    // https://github.com/joekarl/swift_handmade_hero/blob/master/Handmade%20Hero%20OSX/Handmade%20Hero%20OSX/InputManager.swift
    
    private var _proxy: C64Proxy?
    private var hidManager: IOHIDManager
    // private var hidContext: UnsafeMutableRawPointer
    private var usbjoy: [GamePad] = [GamePad(), GamePad()] // Max is two Joysticks right now
    
    override init()
    {
        hidManager = IOHIDManagerCreate(kCFAllocatorDefault, IOOptionBits(kIOHIDOptionsTypeNone));
        super.init()
    }
    
    @objc convenience init?(withC64 proxy: C64Proxy) {

        NSLog("\(#function)")
        
        self.init()
        _proxy = proxy

        // Matching criteria for game pad device
        let deviceCriteria = [
            [
                kIOHIDDeviceUsagePageKey: kHIDPage_GenericDesktop,
                kIOHIDDeviceUsageKey: kHIDUsage_GD_Joystick
            ],
            [
                kIOHIDDeviceUsagePageKey: kHIDPage_GenericDesktop,
                kIOHIDDeviceUsageKey: kHIDUsage_GD_GamePad
            ],
            [
                kIOHIDDeviceUsagePageKey: kHIDPage_GenericDesktop,
                kIOHIDDeviceUsageKey: kHIDUsage_GD_MultiAxisController
            ]
        ]
        
        // Declare bridging closures
        // (Used to bridge between Swift and plain C callbacks)
        let matchingCallback : IOHIDDeviceCallback = { inContext, inResult, inSender, device in
            let this : GamePadManager = unsafeBitCast(inContext, to: GamePadManager.self)
            this.hidDeviceAdded(context: inContext, result: inResult, sender: inSender, device: device)
        }
        
        let removalCallback : IOHIDDeviceCallback = { inContext, inResult, inSender, device in
            let this : GamePadManager = unsafeBitCast(inContext, to: GamePadManager.self)
            this.hidDeviceRemoved(context: inContext, result: inResult, sender: inSender, device: device)
        }
        
        // Configure HID manager
        let hidContext = unsafeBitCast(self, to: UnsafeMutableRawPointer.self)
        IOHIDManagerSetDeviceMatchingMultiple(hidManager, deviceCriteria as CFArray)
        IOHIDManagerRegisterDeviceMatchingCallback(hidManager, matchingCallback, hidContext)
        IOHIDManagerRegisterDeviceRemovalCallback(hidManager, removalCallback, hidContext)
        IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), CFRunLoopMode.defaultMode.rawValue)
        IOHIDManagerOpen(hidManager, IOOptionBits(kIOHIDOptionsTypeNone))
    }
    
    deinit {
        NSLog("\(#function)")
        IOHIDManagerClose(hidManager, IOOptionBits(kIOHIDOptionsTypeNone));
    }
    
    func hidDeviceAdded(context: Optional<UnsafeMutableRawPointer>,
                        result: IOReturn,
                        sender: Optional<UnsafeMutableRawPointer>,
                        device: IOHIDDevice) {
    
        NSLog("\(#function)")
        NSLog("hidDeviceAdded")
        
        let locationIDKey = kIOHIDLocationIDKey as CFString
        let locationID = String(describing: IOHIDDeviceGetProperty(device, locationIDKey))
        
        if (getJoystickProxy(locationID: locationID) != nil) {
            NSLog("Device with location ID %@ already opend.\n", locationID)
            return;
        }
        
        if(usbjoy[0].pluggedIn && usbjoy[1].pluggedIn) {
            NSLog("Ignoring game pad: Maximum number of devices reached.\n")
            return;
        }
        
        let status = IOHIDDeviceOpen(device, IOOptionBits(kIOHIDOptionsTypeSeizeDevice))
        print (status)
        
        // NSLog(@"Failed to open device with location ID %s\n", locationID);
        
        // Add proxy object to list of connected USB joysticks
        if (addJoystickProxy(locationID: locationID)) {
            _proxy?.putMessage(Int32(MSG_JOYSTICK_ATTACHED.rawValue))
            NSLog("Successfully opened device with location ID %@", locationID)
        
            let newProxy = getJoystickProxy(locationID: locationID)!;
            
            // Register input value callback
            let hidContext = unsafeBitCast(newProxy, to: UnsafeMutableRawPointer.self)
            IOHIDDeviceRegisterInputValueCallback(device, newProxy.actionCallback, hidContext)
        }
    }
    
    func hidDeviceRemoved(context: Optional<UnsafeMutableRawPointer>,
                          result: IOReturn,
                          sender: Optional<UnsafeMutableRawPointer>,
                          device: IOHIDDevice) {
        
        NSLog("\(#function)")
        
        let locationIDKey = kIOHIDLocationIDKey as CFString
        let locationID = String(describing: IOHIDDeviceGetProperty(device, locationIDKey))
        
        let proxy = getJoystickProxy(locationID: locationID)
        
        if(proxy == nil) {
            NSLog("Device with location ID %@ is not among opend devices\n")
            return;
        }
        
        let status = IOHIDDeviceClose(device, IOOptionBits(kIOHIDOptionsTypeSeizeDevice))
        
        removeJoystickProxy(locationID: locationID);
        _proxy?.putMessage(Int32(MSG_JOYSTICK_REMOVED.rawValue))
        NSLog("Closed device with location ID %@", locationID)
    }
    
    func joystickIsPluggedIn(nr: Int) -> Bool {
    
        assert (nr >= 1 && nr <= 2);
        return usbjoy[nr - 1].pluggedIn;
    }
    
    func unbindJoysticksFromPortA() {
        if (_proxy != nil && usbjoy[0].joystick == _proxy!.joystickA) {
            usbjoy[0].unbindJoystick();
        }
        if (_proxy != nil && usbjoy[1].joystick == _proxy!.joystickA) {
            usbjoy[1].unbindJoystick();
        }
    }
    
    func unbindJoysticksFromPortB() {
        if (_proxy != nil && usbjoy[0].joystick == _proxy!.joystickB) {
            usbjoy[0].unbindJoystick();
        }
        if (_proxy != nil && usbjoy[1].joystick == _proxy!.joystickB) {
            usbjoy[1].unbindJoystick();
        }
    }

    func addJoystickProxy(locationID: String) -> Bool {
    
        if (!usbjoy[0].pluggedIn) {
            usbjoy[0].pluggedIn = true;
            usbjoy[0].locationID = locationID;
            NSLog("Joystick with ID %@ added to slot 0", locationID);
            return true;
        }
    
        if (!usbjoy[1].pluggedIn) {
            usbjoy[1].pluggedIn = true;
            usbjoy[1].locationID = locationID;
            NSLog("Joystick with ID %@ added to slot 1", locationID);
            return true;
        }
    
        return false;
    }
    
    func getJoystickProxy(locationID: String) -> GamePad? {
    
        if (usbjoy[0].pluggedIn && usbjoy[0].locationID == locationID) {
            return usbjoy[0];
        }
        if (usbjoy[1].pluggedIn && usbjoy[1].locationID == locationID) {
            return usbjoy[1];
        }
        return nil;
    }
    
    func removeJoystickProxy(locationID: String) {
    
        if (usbjoy[0].pluggedIn && usbjoy[0].locationID == locationID) {
            usbjoy[0].pluggedIn = false;
            usbjoy[0].locationID = "";
            NSLog("Joystick with ID %@ removed from slot 0", locationID);
        }
    
        if (usbjoy[1].pluggedIn && usbjoy[1].locationID == locationID) {
            usbjoy[1].pluggedIn = false;
            usbjoy[1].locationID = "";
            NSLog("Joystick with ID %@ removed from slot 1", locationID);
        }
    }
    
    func listJoystickManagers() {
    
        NSLog("USB joystick slot 1: (ID %@)", usbjoy[0].locationID);
        NSLog("USB joystick slot 2: (ID %@)", usbjoy[1].locationID);
    }

    
}
