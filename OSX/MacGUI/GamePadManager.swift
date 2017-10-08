//
//  GamePadManager.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 06.10.17.
//


//! @brief   Holds and manages an array of GamePad objects
/*! @details Up to four devices are managed. The first two are always present and represent
 *           keyboard emulates joysticks. The other two slots are dynamically added when, e.g.,
 *           a USB joystick or game pad is plugged in.
 */
@objc class GamePadManager: NSObject {
    
    // private let inputLock = NSLock()
    // Such a thing is used here: TODO: Check if we need this
    // https://github.com/joekarl/swift_handmade_hero/blob/master/Handmade%20Hero%20OSX/Handmade%20Hero%20OSX/InputManager.swift
    
    //! @brief   Reference to the the C64 proxy
    private var proxy: C64Proxy?
    
    //! @brief   Reference to the HID manager
    private var hidManager: IOHIDManager

    //! @brief   References to all references game pads
    /*! @details Each device ist referenced by a slot number
     */
    var gamePads: [Int:GamePad] = [:]

    
    //! @brief   Returns the lowest free slot number
    func findFreeSlot() -> Int {
        var slotNr = 0
        while (gamePads[slotNr] != nil) { slotNr += 1 }
        return slotNr
    }
    
    //! @brief   Returns true iff the specified game pad slot is assigned a game pad
    @objc public func gamePadSlotIsEmpty(_ nr: Int) -> Bool {
        return gamePads[nr] != nil;
    }
 
    //! @brief   Plugs a game pad into the specified control port
    /*! @details If another device is connected, it is disconnected automatically
     */
    @objc public func attachGamePad(_ nr: Int, toPort port: JoystickProxy) {
        
        NSLog("\(#function)")
        
        // Remove existing device (if any)
        detachGamePadFromPort(port)
        
        // Connect new device
        gamePads[nr]?.joystick = port
    }
    
    //! @brief   Remove game pads from the specified control port (in any)
    @objc public func detachGamePadFromPort(_ port: JoystickProxy) {
        
        NSLog("\(#function)")
        
        for (_, device) in gamePads {
            if (device.joystick == port) {
                device.joystick = nil
            }
        }
    }
    
    //! @brief   Returns slot number of device connected to the specified control port
    /*! @details Returns -1 if no device is connected
     */
    @objc func slotOfGamePadAttachedToPort(_ port: JoystickProxy) -> Int {
        
        NSLog("\(#function)")
        
        for (slotNr, device) in gamePads {
            if (device.joystick == port) {
                NSLog("Device nr %d is attached to control port %@", slotNr, port)
                return slotNr
            }
        }
        return -1
    }
    
    //! @brief   Initialization
    override init()
    {
        hidManager = IOHIDManagerCreate(kCFAllocatorDefault, IOOptionBits(kIOHIDOptionsTypeNone));
        super.init()
    }
    
    //! @brief   Convenience initialization
    @objc public convenience init?(withC64: C64Proxy) {

        NSLog("\(#function)")
        
        self.init()
        proxy = withC64

        //
        // Add two generic devices (keyboard emulated joysticks)
        //
        
        gamePads[0] = GamePad()
        gamePads[1] = GamePad()
        
        //
        // Prepare for accepting HID devices
        //
        
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
        
        // Declare bridging closures (needed to bridge between Swift methods and C callbacks)
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
    
    //! @brief   Device matching callback
    /*! @details Method is invoked when a matching HID device is plugged in
     */
    func hidDeviceAdded(context: Optional<UnsafeMutableRawPointer>,
                        result: IOReturn,
                        sender: Optional<UnsafeMutableRawPointer>,
                        device: IOHIDDevice) {
    
        NSLog("\(#function)")
        
        let vendorIDKey = kIOHIDVendorIDKey as CFString
        let productIDKey = kIOHIDProductIDKey as CFString
        let locationIDKey = kIOHIDLocationIDKey as CFString

        let vendorID = String(describing: IOHIDDeviceGetProperty(device, vendorIDKey))
        let productID = String(describing: IOHIDDeviceGetProperty(device, productIDKey))
        let locationID = String(describing: IOHIDDeviceGetProperty(device, locationIDKey))

        // Find a free slot for the new device
        let slotNr = findFreeSlot()
        if (slotNr > 4) {
            NSLog("Maximum number of devices reached. Ignoring device\n")
            return
        }
        
        // Create GamePad object
        gamePads[slotNr] = GamePad()
        gamePads[slotNr]?.vendorID = vendorID
        gamePads[slotNr]?.productID = productID
        gamePads[slotNr]?.locationID = locationID

        // Open HID device
        let status = IOHIDDeviceOpen(device, IOOptionBits(kIOHIDOptionsTypeSeizeDevice))
        if (status != 0) {
            NSLog("WARNING: Cannot open HID device")
            return
        }
    
        // Register input value callback
        let hidContext = unsafeBitCast(gamePads[slotNr], to: UnsafeMutableRawPointer.self)
        IOHIDDeviceRegisterInputValueCallback(device, gamePads[slotNr]!.actionCallback, hidContext)

        // Inform emulator
        proxy?.putMessage(Int32(MSG_JOYSTICK_ATTACHED.rawValue))
        NSLog("Successfully opened device with location ID %@", locationID)

        listDevices()
    }
    
    func hidDeviceRemoved(context: Optional<UnsafeMutableRawPointer>,
                          result: IOReturn,
                          sender: Optional<UnsafeMutableRawPointer>,
                          device: IOHIDDevice) {
        
        NSLog("\(#function)")
        
        let locationIDKey = kIOHIDLocationIDKey as CFString
        let locationID = String(describing: IOHIDDeviceGetProperty(device, locationIDKey))
        
        // Search for a matching locationID and remove device
        for (slotNr, device) in gamePads {
            if (device.locationID == locationID) {
                gamePads[slotNr] = nil
                NSLog("Clearing slot %d", slotNr)
            }
        }
        
        // Close device
        let status = IOHIDDeviceClose(device, IOOptionBits(kIOHIDOptionsTypeSeizeDevice))
        if (status != 0) {
            NSLog("WARNING: Cannot close HID device")
            return
        }
        
        // Inform emulator
        proxy?.putMessage(Int32(MSG_JOYSTICK_REMOVED.rawValue))
        NSLog("Closed device with location ID %@", locationID)
        
        listDevices()
    }
    
    func listDevices() {
        
        for (slotNr, device) in gamePads {
            if (device.locationID == "") {
                NSLog("Game pad slot %d: Keyboard emulated device", slotNr)
            } else {
                NSLog("Game pad slot %d: HID USB joystick", slotNr)
                NSLog("  Vendor ID:   %@", device.vendorID!);
                NSLog("  Product ID:  %@", device.productID!);
                NSLog("  Location ID: %@", device.locationID!);
            }
            if (device.joystick != nil) {
                NSLog("  Connected to control port %@", device.joystick!)
            }
        }
    }
    
}
