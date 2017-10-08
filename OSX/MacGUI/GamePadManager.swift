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
    
    //
    // Slot handling
    //
    
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
    
    //
    // Keyboard stuff
    //
    
    //! @brief   Handles a keyboard down event
    func keyDown(_ key: MacKeyFingerprint) {
        for (_, device) in gamePads {
            device.keyDown(key)
        }
    }

    //! @brief   Handles a keyboard up event
    func keyUp(_ key: MacKeyFingerprint) {
        for (_, device) in gamePads {
            device.keyUp(key)
        }
    }
    
    //! @brief   Assign a joystick emulation key
    /*
    func assignKey(_ key: MacKeyFingerprint, slotNr: Int, direction: JoystickDirection) {
        if let keymap = gamePads[slotNr]?.keymap {
            keymap[key] = direction
        }
    }
    */
    @objc public func keysetOfDevice(_ slotNr: Int) -> KeyMap? {
        return gamePads[slotNr]?.keymap
    }
    
    //
    // HID stuff
    //
    
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
    
    //
    // User default storage
    //
    
    @objc func restoreFactorySettings()
    {
        NSLog("\(#function)")
        
        let keymap1 = gamePads[0]!.keymap
        let keymap2 = gamePads[1]!.keymap
        
        keymap1.setFingerprint(123, for: JoystickDirection.LEFT)
        keymap1.setFingerprint(124, for: JoystickDirection.RIGHT)
        keymap1.setFingerprint(126, for: JoystickDirection.UP)
        keymap1.setFingerprint(125, for: JoystickDirection.DOWN)
        keymap1.setFingerprint(49,  for: JoystickDirection.FIRE)
        
        keymap1.setCharacter(" ", for: JoystickDirection.LEFT)
        keymap1.setCharacter(" ", for: JoystickDirection.RIGHT)
        keymap1.setCharacter(" ", for: JoystickDirection.UP)
        keymap1.setCharacter(" ", for: JoystickDirection.DOWN)
        keymap1.setCharacter(" ", for: JoystickDirection.FIRE)
        
        keymap2.setFingerprint(0,  for: JoystickDirection.LEFT)
        keymap2.setFingerprint(1,  for: JoystickDirection.RIGHT)
        keymap2.setFingerprint(13,  for: JoystickDirection.UP)
        keymap2.setFingerprint(6, for: JoystickDirection.DOWN)
        keymap2.setFingerprint(7,  for: JoystickDirection.FIRE)
        
        keymap2.setCharacter("a", for: JoystickDirection.LEFT)
        keymap2.setCharacter("s", for: JoystickDirection.RIGHT)
        keymap2.setCharacter("w", for: JoystickDirection.UP)
        keymap2.setCharacter("y", for: JoystickDirection.DOWN)
        keymap2.setCharacter("x", for: JoystickDirection.FIRE)
    }
    
    @objc class func registerStandardUserDefaults() {
        
        let dictionary : [String:Any] = [
            "VC64Left1keycodeKey":123,
            "VC64Right1keycodeKey":124,
            "VC64Up1keycodeKey":126,
            "VC64Down1keycodeKey":125,
            "VC64Fire1keycodeKey":49,
            
            "VC64Left1charKey":" ",
            "VC64Right1charKey":" ",
            "VC64Up1charKey":" ",
            "VC64Down1charKey":" ",
            "VC64Fire1charKey":" ",
            
            "VC64Left2keycodeKey":0,
            "VC64Right2keycodeKey":1,
            "VC64Up2keycodeKey":13,
            "VC64Down2keycodeKey":6,
            "VC64Fire2keycodeKey":7,
            
            "VC64Left2charKey":"a",
            "VC64Right2charKey":"s",
            "VC64Up2charKey":"w",
            "VC64Down2charKey":"y",
            "VC64Fire2charKey":"x"]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    @objc func loadUserDefaults() {
        
        NSLog("\(#function)")

        let keymap1 = gamePads[0]!.keymap
        let keymap2 = gamePads[1]!.keymap

        loadUserDefaults(forKeymap: keymap1, s: "1")
        loadUserDefaults(forKeymap: keymap2, s: "2")
    }
    
    func loadUserDefaults(forKeymap keymap: KeyMap, s : String) {
        
        let defaults = UserDefaults.standard
        
        func loadFingerprint(for d: JoystickDirection, usingKey key: String) {
            keymap.setFingerprint(
                MacKeyFingerprint(defaults.integer(forKey: key + s + "keycodeKey")), for: d)
        }
        
        loadFingerprint(for:JoystickDirection.LEFT, usingKey: "VC64Left")
        loadFingerprint(for:JoystickDirection.RIGHT, usingKey: "VC64Right")
        loadFingerprint(for:JoystickDirection.UP, usingKey: "VC64Up")
        loadFingerprint(for:JoystickDirection.DOWN, usingKey: "VC64Down")
        loadFingerprint(for:JoystickDirection.FIRE, usingKey: "VC64Fire")
        
        func loadCharacter(for d: JoystickDirection, usingKey key: String) {
            keymap.setCharacter(defaults.string(forKey: key + s + "charKey"), for: d)
        }
        
        loadCharacter(for:JoystickDirection.LEFT, usingKey: "VC64Left")
        loadCharacter(for:JoystickDirection.RIGHT, usingKey: "VC64Right")
        loadCharacter(for:JoystickDirection.UP, usingKey: "VC64Up")
        loadCharacter(for:JoystickDirection.DOWN, usingKey: "VC64Down")
        loadCharacter(for:JoystickDirection.FIRE, usingKey: "VC64Fire")
    }
    
    @objc func saveUserDefaults() {
        
        NSLog("\(#function)")

        let keymap1 = gamePads[0]!.keymap
        let keymap2 = gamePads[1]!.keymap
        
        saveUserDefaults(forKeymap: keymap1, s: "1")
        saveUserDefaults(forKeymap: keymap2, s: "2")
    }
    
    func saveUserDefaults(forKeymap keymap: KeyMap, s : String) {
        
        let defaults = UserDefaults.standard
        
        func saveFingerprint(for d: JoystickDirection, usingKey key: String) {
            defaults.set(keymap.fingerprint(for: d), forKey: key + s + "keycodeKey")
        }
        
        saveFingerprint(for:JoystickDirection.LEFT, usingKey: "VC64Left")
        saveFingerprint(for:JoystickDirection.RIGHT, usingKey: "VC64Right")
        saveFingerprint(for:JoystickDirection.UP, usingKey: "VC64Up")
        saveFingerprint(for:JoystickDirection.DOWN, usingKey: "VC64Down")
        saveFingerprint(for:JoystickDirection.FIRE, usingKey: "VC64Fire")
        
        func saveCharacter(for d: JoystickDirection, usingKey key: String) {
            defaults.set(keymap.getCharacter(for: d), forKey: key + s + "charKey")
        }
        
        saveCharacter(for:JoystickDirection.LEFT, usingKey: "VC64Left")
        saveCharacter(for:JoystickDirection.RIGHT, usingKey: "VC64Right")
        saveCharacter(for:JoystickDirection.UP, usingKey: "VC64Up")
        saveCharacter(for:JoystickDirection.DOWN, usingKey: "VC64Down")
        saveCharacter(for:JoystickDirection.FIRE, usingKey: "VC64Fire")
    }
    
    
}
