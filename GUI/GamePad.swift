// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import IOKit.hid

/* An object of this class represents an input device connected to the Game
 * Port. The object can either represent a connected HID device or a keyboard
 * emulated device. In the first case, the object serves as a callback handler
 * for HID events. In the latter case, it translates keyboard events to
 * GamePadAction events by utilizing a key map.
 */
class GamePad {

    // References to other objects
    var manager: GamePadManager
    var prefs: Preferences { return manager.parent.pref }
    var db: DeviceDatabase { return manager.parent.myAppDelegate.database }
    
    // The control port this device is connected to (1, 2, or nil)
    var port: Int?

    // Reference to the device object
    var device: IOHIDDevice?
    var vendorID: String { return device?.vendorID ?? "" }
    var productID: String { return device?.productID ?? "" }
    var locationID: String { return device?.locationID ?? "" }
    
    // Type of the managed device (joystick or mouse)
    var type: ControlPortDevice
    var isMouse: Bool { return type == .MOUSE }
    var isJoystick: Bool { return type == .JOYSTICK }
    
    // Name of the managed device
    var name = ""
    
    // Icon of this device
    var icon: NSImage?
    
    // Indicates if this device is officially supported
    var isKnown: Bool { return db.isKnown(vendorID: vendorID, productID: productID) }
    
    // Keymap of the managed device (only set for keyboard emulated devices)
    var keyMap: Int?
    
    // Indicates if a joystick emulation key is currently pressed
    var keyUp = false, keyDown = false, keyLeft = false, keyRight = false
    
    // Indicates if other components should be notified when the device is used
    var notify = false
    
    // Minimum and maximum value of analog axis event
    var min: Int?, max: Int?
    
    // Cotroller specific usage IDs (set in updateMappingScheme())
    var lxAxis = kHIDUsage_GD_X
    var lyAxis = kHIDUsage_GD_Y
    var rxAxis = kHIDUsage_GD_Z
    var ryAxis = kHIDUsage_GD_Rz
    var hShift = 0
    
    /* Rescued information from the latest invocation of the action function.
     * This information is utilized to determine whether a joystick event has
     * to be triggered.
     */
    var oldEvents: [Int: [GamePadAction]] = [:]
    
    // Receivers for HID events
    let inputValueCallback: IOHIDValueCallback = {
        inContext, inResult, inSender, value in
        let this: GamePad = unsafeBitCast(inContext, to: GamePad.self)
        this.hidInputValueAction(context: inContext,
                                 result: inResult,
                                 sender: inSender,
                                 value: value)
    }
    
    init(manager: GamePadManager, device: IOHIDDevice? = nil, type: ControlPortDevice) {
                
        self.manager = manager
        self.device = device
        self.type = type
                
        name = db.name(vendorID: vendorID, productID: productID) ?? device?.name ?? ""
        icon = db.icon(vendorID: vendorID, productID: productID)

        if icon == nil && isMouse {
            icon = NSImage.init(named: "devMouseTemplate")
        }
        
        updateMappingScheme()
    }

    func updateMappingScheme() {
        
        let lScheme = db.left(vendorID: vendorID, productID: productID)
        let rScheme = db.right(vendorID: vendorID, productID: productID)
        let hScheme = db.hatSwitch(vendorID: vendorID, productID: productID)
                
        switch lScheme { // Left stick
        case 1:
            lxAxis = kHIDUsage_GD_Y
            lyAxis = kHIDUsage_GD_X
        default:
            lxAxis = kHIDUsage_GD_X
            lyAxis = kHIDUsage_GD_Y
        }
        
        switch rScheme { // Right stick
        case 1:
            rxAxis = kHIDUsage_GD_Z
            ryAxis = kHIDUsage_GD_Rz
        default:
            rxAxis = kHIDUsage_GD_Rz
            ryAxis = kHIDUsage_GD_Z
        }

        switch hScheme { // Hat switch
        case 1:
            hShift = 1
        default:
            hShift = 0
        }
    }
    
    func setIcon(name: String) {
        
        icon = NSImage.init(named: name)
    }
    
    func property(key: String) -> String? {
            
        if device != nil {
            if let prop = IOHIDDeviceGetProperty(device!, key as CFString) {
                return "\(prop)"
            }
        }
        return nil
    }

    func dump() {
        
        print(name != "" ? "\(name) " : "Placeholder device ", terminator: "")
        print(isMouse ? "(Mouse) " : "", terminator: "")
        print(port != nil ? "[\(port!)] " : "[-] ", terminator: "")
        if vendorID != "" { print("v: \(vendorID) ", terminator: "") }
        if productID != "" { print("p: \(productID) ", terminator: "") }
        if locationID != "" { print("l: \(locationID) ", terminator: "") }
        print("\(lxAxis) \(lyAxis) ", terminator: "")
        print("\(rxAxis) \(ryAxis) ", terminator: "")
        print("\(hShift)")
    }

    //
    // Responding to keyboard events
    //

    // Binds a key to a gamepad action
    func bind(key: MacKey, action: GamePadAction) {

        guard let n = keyMap else { return }
        
        // Avoid double mappings
        unbind(action: action)

        prefs.keyMaps[n][key] = action.rawValue
    }

    // Removes a key binding to the specified gampad action (if any)
    func unbind(action: GamePadAction) {
        
        guard let n = keyMap else { return }
        
        for (k, dir) in prefs.keyMaps[n] where dir == action.rawValue {
            prefs.keyMaps[n][k] = nil
        }
     }

    // Translates a key press event to a list of gamepad actions
    func keyDownEvents(_ macKey: MacKey) -> [GamePadAction] {
        
        var macKey2 = macKey
        macKey2.carbonFlags = 0
        guard let n = keyMap, let direction = prefs.keyMaps[n][macKey2] else { return [] }
                    
        // track("keyDownEvents \(direction)")

        switch GamePadAction(rawValue: direction) {
            
        case .PULL_UP:
            keyUp = true
            return [.PULL_UP]
            
        case .PULL_DOWN:
            keyDown = true
            return [.PULL_DOWN]
            
        case .PULL_LEFT:
            keyLeft = true
            return [.PULL_LEFT]
            
        case .PULL_RIGHT:
            keyRight = true
            return [.PULL_RIGHT]
            
        case .PRESS_FIRE:
            return [.PRESS_FIRE]
            
        case .PRESS_LEFT:
            return [.PRESS_LEFT]
            
        case .PRESS_RIGHT:
            return [.PRESS_RIGHT]
            
        default:
            fatalError()
        }
    }
        
    // Handles a key release event
    func keyUpEvents(_ macKey: MacKey) -> [GamePadAction] {
        
        var macKey2 = macKey
        macKey2.carbonFlags = 0
        guard let n = keyMap, let direction = prefs.keyMaps[n][macKey2] else { return [] }
                
        // track("keyUpEvents \(direction)")
        
        switch GamePadAction(rawValue: direction) {
            
        case .PULL_UP:
            keyUp = false
            return keyDown ? [.PULL_DOWN] : [.RELEASE_Y]
            
        case .PULL_DOWN:
            keyDown = false
            return keyUp ? [.PULL_UP] : [.RELEASE_Y]
            
        case .PULL_LEFT:
            keyLeft = false
            return keyRight ? [.PULL_RIGHT] : [.RELEASE_X]
            
        case .PULL_RIGHT:
            keyRight = false
            return keyLeft ? [.PULL_LEFT] : [.RELEASE_X]
            
        case .PRESS_FIRE:
            return [.RELEASE_FIRE]
            
        case .PRESS_LEFT:
            return [.RELEASE_LEFT]
            
        case .PRESS_RIGHT:
            return [.RELEASE_RIGHT]
            
        default:
            fatalError()
        }
    }
    
    //
    // Responding to HID events
    //

    // Based on
    // http://docs.ros.org/hydro/api/oculus_sdk/html/OSX__Gamepad_8cpp_source.html#l00170
    
    func mapAnalogAxis(value: IOHIDValue, element: IOHIDElement) -> Int? {
        
        if min == nil {
            min = IOHIDElementGetLogicalMin(element)
            // track("Minumum axis value = \(min!)")
        }
        if max == nil {
            max = IOHIDElementGetLogicalMax(element)
            // track("Maximum axis value = \(max!)")
        }
        let val = IOHIDValueGetIntegerValue(value)
        
        var v = (Double) (val - min!) / (Double) (max! - min!)
        v = v * 2.0 - 1.0
        if v < -0.45 { return -2 }
        if v < -0.1 { return nil }  // dead zone
        if v <= 0.1 { return 0 }
        if v <= 0.45 { return nil } // dead zone
        return 2
    }
    
    func hidInputValueAction(context: UnsafeMutableRawPointer?,
                             result: IOReturn,
                             sender: UnsafeMutableRawPointer?,
                             value: IOHIDValue) {
        
        let element   = IOHIDValueGetElement(value)
        let intValue  = Int(IOHIDValueGetIntegerValue(value))
        let usagePage = Int(IOHIDElementGetUsagePage(element))
        let usage     = Int(IOHIDElementGetUsage(element))
        
        // Buttons
        if usagePage == kHIDPage_Button {

            let events: [GamePadAction] = (intValue != 0) ? [.PRESS_FIRE] : [.RELEASE_FIRE]
            processJoystickEvents(events: events)
            return
        }
        
        // Stick
        if usagePage == kHIDPage_GenericDesktop {
            
            var events: [GamePadAction]?
            
            /*
            switch usage {
            case lThumbXUsageID: track("lThumbXUsageID \(value)")
            case rThumbXUsageID: track("rThumbXUsageID \(value)")
            case lThumbYUsageID: track("lThumbYUsageID \(value)")
            case rThumbYUsageID: track("rThumbYUsageID \(value)")
            case kHIDUsage_GD_Hatswitch: track("kHIDUsage_GD_Hatswitch \(value)")
            default: break
            }
            */
            
            switch usage {
                
            case lxAxis, rxAxis:
                
                if let v = mapAnalogAxis(value: value, element: element) {
                    events =
                        (v == 2) ? [.PULL_RIGHT] :
                        (v == -2) ? [.PULL_LEFT] : [.RELEASE_X]
                }
                
            case lyAxis, ryAxis:
                
                if let v = mapAnalogAxis(value: value, element: element) {
                    events =
                        (v == 2) ? [.PULL_DOWN] :
                        (v == -2) ? [.PULL_UP] : [.RELEASE_Y]
                }
                
            case kHIDUsage_GD_Hatswitch:
                
                switch intValue {
                case 0: events = [.PULL_UP, .RELEASE_X]
                case 1: events = [.PULL_UP, .PULL_RIGHT]
                case 2: events = [.PULL_RIGHT, .RELEASE_Y]
                case 3: events = [.PULL_RIGHT, .PULL_DOWN]
                case 4: events = [.PULL_DOWN, .RELEASE_X]
                case 5: events = [.PULL_DOWN, .PULL_LEFT]
                case 6: events = [.PULL_LEFT, .RELEASE_Y]
                case 7: events = [.PULL_LEFT, .PULL_UP]
                default: events = [.RELEASE_XY]
                }
                
            default:
                // track("Unknown HID usage: \(usage)")")
                break
            }
            
            // Only proceed if the event is different than the previous one
            if events == nil || oldEvents[usage] == events { return }
            oldEvents[usage] = events!
            
            // Trigger events
            processJoystickEvents(events: events!)
        }
    }
    
    //
    // Emulate events on the C64 side
    //
    
    @discardableResult
    func processJoystickEvents(events: [GamePadAction]) -> Bool {
        
        let c64 = manager.parent.c64!
        
        if port == 1 { for event in events { c64.port1.joystick.trigger(event) } }
        if port == 2 { for event in events { c64.port2.joystick.trigger(event) } }
        
        // Notify other components (if requested)
        if notify { manager.parent.myAppDelegate.devicePulled(events: events) }
        
        return events != []
    }
    
    @discardableResult
    func processMouseEvents(events: [GamePadAction]) -> Bool {
        
        let c64 = manager.parent.c64!
        
        if port == 1 { for event in events { c64.port1.mouse.trigger(event) } }
        if port == 2 { for event in events { c64.port2.mouse.trigger(event) } }
        
        return events != []
    }
    
    func processMouseEvents(xy: NSPoint) {

        let c64 = manager.parent.c64!
        
        if port == 1 { c64.port1.mouse.setXY(xy) }
        if port == 2 { c64.port2.mouse.setXY(xy) }
    }
    
    /*
    func processMouseEvents(delta: NSPoint) {
        
        let c64 = manager.parent.c64!
        
        if port == 1 { c64.port1.mouse.setDeltaXY(delta) }
        if port == 2 { c64.port2.mouse.setDeltaXY(delta) }
    }
    */
    
    func processKeyDownEvent(macKey: MacKey) -> Bool {
                
        // Only proceed if a keymap is present
        if keyMap == nil { return false }
                
        // Only proceed if this key is used for emulation
        let events = keyDownEvents(macKey)
        if events.isEmpty { return false }
                
        // Process the events
        processKeyboardEvent(events: events)
        return true
    }

    func processKeyUpEvent(macKey: MacKey) -> Bool {
        
        // Only proceed if a keymap is present
        if keyMap == nil { return false }
        
        // Only proceed if this key is used for emulation
        let events = keyUpEvents(macKey)
        if events.isEmpty { return false }
        
        // Process the events
        processKeyboardEvent(events: events)
        return true
    }

    func processKeyboardEvent(events: [GamePadAction]) {

        let c64 = manager.parent.c64!
        
        if isMouse {
            if port == 1 { for e in events { c64.port1.mouse.trigger(e) } }
            if port == 2 { for e in events { c64.port2.mouse.trigger(e) } }
        } else {
            if port == 1 { for e in events { c64.port1.joystick.trigger(e) } }
            if port == 2 { for e in events { c64.port2.joystick.trigger(e) } }
        }
    }
}
