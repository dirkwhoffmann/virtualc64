//
//  GamePadManager.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 06.10.17.
//


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
    
    @objc public convenience init?(withC64 proxy: C64Proxy) {

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
        print("CLosing status:", status)
        
        removeJoystickProxy(locationID: locationID);
        _proxy?.putMessage(Int32(MSG_JOYSTICK_REMOVED.rawValue))
        NSLog("Closed device with location ID %@", locationID)
    }
    
    @objc public func joystickIsPluggedIn(nr: Int) -> Bool {
    
        assert (nr >= 1 && nr <= 2);
        return usbjoy[nr - 1].pluggedIn;
    }
    
    @objc public func bindJoystickToPortA(nr: Int) {
        
        assert (nr >= 1 && nr <= 2)
        usbjoy[nr - 1].bindJoystickToPortA(c64: _proxy!)
    }
    
    @objc public func bindJoystickToPortB(nr: Int) {
        
        assert (nr >= 1 && nr <= 2)
        usbjoy[nr - 1].bindJoystickToPortB(c64: _proxy!)
    }

    @objc public func unbindJoysticksFromPortA() {
        if (_proxy != nil && usbjoy[0].joystick == _proxy!.joystickA) {
            usbjoy[0].unbindJoystick();
        }
        if (_proxy != nil && usbjoy[1].joystick == _proxy!.joystickA) {
            usbjoy[1].unbindJoystick();
        }
    }
    
    @objc public func unbindJoysticksFromPortB() {
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
    
    func listDevices() {
        NSLog("USB joystick slot 1:");
        
        if (usbjoy[0].locationID != nil) {
            NSLog("HID device with location ID %@", usbjoy[0].locationID!)
        }
    }

    
}
