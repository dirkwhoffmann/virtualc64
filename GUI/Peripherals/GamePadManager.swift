// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

struct InputDevice {
    
    static let none = -1
    static let keyset1 = 0
    static let keyset2 = 1
    static let mouse = 2
    static let joystick1 = 3
    static let joystick2 = 4
}

/* An object of this class holds and manages an array of GamePad objects.
 * Up to five gamepads are managed. The first three gamepads are initialized
 * by default and represent a mouse and two keyboard emulated joysticks.
 * All remaining gamepads are added dynamically when HID devices are connected.
 */
class GamePadManager {

    // Reference to the main controller
    var parent: MyController!
    
    // Reference to the HID manager
    var hidManager: IOHIDManager
        
    // Gamepad storage
    var gamePads: [Int: GamePad] = [:]
    
    // Lock for synchronizing asynchroneous calls
    var lock = NSLock()
    
    //
    // Initializing
    //
    
    init(parent: MyController) {
        
        self.parent = parent
        
        hidManager = IOHIDManagerCreate(kCFAllocatorDefault,
                                        IOOptionBits(kIOHIDOptionsTypeNone))
        
        // Add default devices
        gamePads[0] = GamePad(manager: self, type: .MOUSE)
        gamePads[0]!.name = "Mouse"
        gamePads[0]!.setIcon(name: "devMouseTemplate")
        gamePads[0]!.keyMap = 0

        gamePads[1] = GamePad(manager: self, type: .JOYSTICK)
        gamePads[1]!.name = "Joystick Keyset 1"
        gamePads[1]!.setIcon(name: "devKeyset1Template")
        gamePads[1]!.keyMap = 1
        
        gamePads[2] = GamePad(manager: self, type: .JOYSTICK)
        gamePads[2]!.name = "Joystick Keyset 2"
        gamePads[2]!.setIcon(name: "devKeyset2Template")
        gamePads[2]!.keyMap = 2
                
        // Tell the mouse event receiver where the mouse resides
        parent.metal.mouse1 = gamePads[0]!

        // Prepare to accept HID devices
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
            ],
            [
                kIOHIDDeviceUsagePageKey: kHIDPage_GenericDesktop,
                kIOHIDDeviceUsageKey: kHIDUsage_GD_Mouse
            ]
        ]
        
        // Declare bridging closures (bridge between Swift methods and C callbacks)
        let matchingCallback: IOHIDDeviceCallback = { inContext, inResult, inSender, device in
            let this: GamePadManager = unsafeBitCast(inContext, to: GamePadManager.self)
            this.hidDeviceAdded(context: inContext, result: inResult, sender: inSender, device: device)
        }
        
        let removalCallback: IOHIDDeviceCallback = { inContext, inResult, inSender, device in
            let this: GamePadManager = unsafeBitCast(inContext, to: GamePadManager.self)
            this.hidDeviceRemoved(context: inContext, result: inResult, sender: inSender, device: device)
        }
        
        // Configure the HID manager
        let hidContext = unsafeBitCast(self, to: UnsafeMutableRawPointer.self)
        IOHIDManagerSetDeviceMatchingMultiple(hidManager, deviceCriteria as CFArray)
        IOHIDManagerRegisterDeviceMatchingCallback(hidManager, matchingCallback, hidContext)
        IOHIDManagerRegisterDeviceRemovalCallback(hidManager, removalCallback, hidContext)
        IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), CFRunLoopMode.defaultMode.rawValue)
        IOHIDManagerOpen(hidManager, IOOptionBits(kIOHIDOptionsTypeNone))
    }
    
    func shutDown() {
        
        debug(.shutdown)
        
        // Terminate communication with all connected HID devices
        for (_, pad) in gamePads { pad.device?.close() }
        
        // Close the HID manager
        IOHIDManagerClose(hidManager, IOOptionBits(kIOHIDOptionsTypeNone))
        
        // Free all slots
        gamePads = [:]
    }
    
    deinit {
        
        debug(.shutdown)
    }
    
    //
    // Managing slots
    //
    
    // Returns true iff the specified game pad slot is used or free
    func isUsed(slot: Int) -> Bool { return gamePads[slot] != nil }
    func isEmpty(slot: Int) -> Bool { return gamePads[slot] == nil }
    
    // Returns the lowest free slot number or nil if all slots are occupied
    func findFreeSlot() -> Int? {
        
        var nr = 0
        while !isEmpty(slot: nr) { nr += 1 }
        
        // We support up to 5 devices
        if nr < 5 { return nr }
        
        warn("Maximum number of devices reached")
        return nil
    }
    
    func connect(slot: Int, port: Int) {
        
        // Remove any existing binding to this port
        for (_, pad) in gamePads where pad.port == port { pad.port = 0 }
        
        // Bind the new device
        gamePads[slot]?.port = port
    }

    func name(slot: Int) -> String {
        return gamePads[slot]?.name ?? "External device"
    }

    func icon(slot: Int) -> NSImage {
        return gamePads[slot]?.icon ?? NSImage(named: "devGamepad1Template")!
    }

    //
    // HID support
    //
    
    // Matching callback (invoked when a matching HID device is plugged in)
    func hidDeviceAdded(context: UnsafeMutableRawPointer?,
                        result: IOReturn,
                        sender: UnsafeMutableRawPointer?,
                        device: IOHIDDevice) {
    
        lock.lock(); defer { lock.unlock() }

        debug(.hid)
        // device.listProperties()

        // Ignore internal devices
        if device.isInternalDevice { return }
        
        // Find a free slot for the new device
        guard let slot = findFreeSlot() else { return }
        
        // Add device
        addDevice(slot: slot, device: device)
                
        // Reconnect devices (assignments trigger side effects)
        parent.config.gameDevice1 = parent.config.gameDevice1
        parent.config.gameDevice2 = parent.config.gameDevice2
        
        // Inform about the changed configuration
        parent.toolbar.validateVisibleItems()
        myAppDelegate.deviceAdded()
        
        listDevices()
    }
    
    func addDevice(slot: Int, device: IOHIDDevice) {
        
        if device.isMouse {
            
            // Create a GamePad object
            gamePads[slot] = GamePad(manager: self, device: device, type: .MOUSE)
            
            // Inform the mouse event receiver about the new mouse
            parent.metal.mouse2 = gamePads[slot]
            
        } else {
        
        // Open device
        if !device.open() { return }
        
        // Create a GamePad object
        gamePads[slot] = GamePad(manager: self, device: device, type: .JOYSTICK)

        // Register input value callback
        let hidContext = unsafeBitCast(gamePads[slot], to: UnsafeMutableRawPointer.self)
        IOHIDDeviceRegisterInputValueCallback(device,
                                              gamePads[slot]!.inputValueCallback,
                                              hidContext)
        }
    }
    
    func hidDeviceRemoved(context: UnsafeMutableRawPointer?,
                          result: IOReturn,
                          sender: UnsafeMutableRawPointer?,
                          device: IOHIDDevice) {
        
        lock.lock(); defer { lock.unlock() }

        debug(.hid)

        // Search for a matching locationID and remove device
        for (slot, pad) in gamePads where pad.locationID == device.locationID {
            gamePads[slot] = nil
        }

        // Inform about the changed configuration
        parent.toolbar.validateVisibleItems()
        myAppDelegate.deviceAdded()

        listDevices()
    }
    
    func listDevices() {
        
        print("Input devices:")
        for i in 0 ... Int.max {
            
            guard let dev = gamePads[i] else { break }
            dev.dump()
        }
    }
    
    func refresh(popup: NSPopUpButton, hide: Bool = false) {
        
        let slots = [
            InputDevice.mouse,
            InputDevice.keyset1,
            InputDevice.keyset2,
            InputDevice.joystick1,
            InputDevice.joystick2
        ]
        
        for s in slots {
            if let item = popup.menu?.item(withTag: s) {
                item.title = name(slot: s)
                item.image = icon(slot: s)
                item.isEnabled = isUsed(slot: s)
                item.isHidden = isEmpty(slot: s) && hide
            }
        }
    }
}
