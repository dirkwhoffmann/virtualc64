// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
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

    // Mapping schemes
    enum Schemes {

        // Left stick
        static let A0A1 = 0
        static let A0A1r = 1
        
        // Right stick
        static let A2A5 = 0
        static let A2A3 = 1
        static let A3A4 = 2
        static let A2A5r = 3

        // Hat switch
        static let H0H7 = 0
        static let H1H8 = 1
        static let B4B7 = 2
        static let B11B14 = 3
        static let U90U93 = 4
    }
             
    // References to other objects
    var manager: GamePadManager
    var prefs: Preferences { return manager.parent.pref }
    var config: Configuration { return manager.parent.config }
    var db: DeviceDatabase { return myAppDelegate.database }
    
    // The control port this device is connected to (0, 1, or nil)
    var port: Int?

    // Reference to the HID device
    var device: IOHIDDevice?
    var vendorID: String { return device?.vendorID ?? "" }
    var productID: String { return device?.productID ?? "" }
    var locationID: String { return device?.locationID ?? "" }
    
    // Type of the managed device (joystick or mouse)
    var type: vc64.ControlPortDevice
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
        
    // Controller specific mapping schemes for the two sticks and the hat switch
    var lScheme = 0
    var rScheme = 0
    var hScheme = 0

    /* Rescued information from the latest invocation of the action function.
     * This information is utilized to determine whether a joystick event has
     * to be triggered.
     */
    var oldEvents: [Int: [vc64.GamePadAction]] = [:]

    // Receivers for HID events
    let inputValueCallback: IOHIDValueCallback = {
        inContext, inResult, inSender, value in
        let this: GamePad = unsafeBitCast(inContext, to: GamePad.self)
        this.hidInputValueAction(context: inContext,
                                 result: inResult,
                                 sender: inSender,
                                 value: value)
    }
    
    init(manager: GamePadManager, device: IOHIDDevice? = nil, type: vc64.ControlPortDevice) {

        self.manager = manager
        self.device = device
        self.type = type
        
        name = db.name(vendorID: vendorID, productID: productID) ?? device?.name ?? ""
        icon = db.icon(vendorID: vendorID, productID: productID)

        if icon == nil && isMouse {
            icon = NSImage(named: "devMouseTemplate")
        }
        
        updateMappingScheme()
    }
    
    func updateMappingScheme() {
        
        lScheme = db.left(vendorID: vendorID, productID: productID)
        rScheme = db.right(vendorID: vendorID, productID: productID)
        hScheme = db.hatSwitch(vendorID: vendorID, productID: productID)
    }
        
    func setIcon(name: String) {
        
        icon = NSImage(named: name)
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
        print(port != nil ? "[\(port!)]" : "[-] ", terminator: "")
        print(vendorID == "" ? "" : "v: \(vendorID) ", terminator: "")
        print(productID == "" ? "" : "v: \(productID) ", terminator: "")
        print(locationID == "" ? "" : "v: \(locationID) ")
    }
    
    //
    // Responding to keyboard events
    //

    // Binds a key to a gamepad action
    func bind(key: MacKey, action: vc64.GamePadAction) {

        guard let n = keyMap else { return }
        
        // Avoid double mappings
        unbind(action: action)

        prefs.keyMaps[n][key] = action.rawValue
    }

    // Removes a key binding to the specified gampad action (if any)
    func unbind(action: vc64.GamePadAction) {

        guard let n = keyMap else { return }
        
        for (k, dir) in prefs.keyMaps[n] where dir == action.rawValue {
            prefs.keyMaps[n][k] = nil
        }
     }

    // Translates a key press event to a list of gamepad actions
    func keyDownEvents(_ macKey: MacKey) -> [vc64.GamePadAction] {

        var macKey2 = macKey
        macKey2.carbonFlags = 0
        guard let n = keyMap, let direction = prefs.keyMaps[n][macKey2] else { return [] }
                    
        switch vc64.GamePadAction(rawValue: direction) {

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
    func keyUpEvents(_ macKey: MacKey) -> [vc64.GamePadAction] {

        var macKey2 = macKey
        macKey2.carbonFlags = 0
        guard let n = keyMap, let direction = prefs.keyMaps[n][macKey2] else { return [] }
                
        debug(.events, "keyUpEvents \(direction)")
        
        switch vc64.GamePadAction(rawValue: direction) {

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

    static func analogAxis(value: IOHIDValue, element: IOHIDElement) -> Double {

        let min = IOHIDElementGetLogicalMin(element)
        let max = IOHIDElementGetLogicalMax(element)
        let val = IOHIDValueGetIntegerValue(value)

        let v = (Double) (val - min) / (Double) (max - min)
        return v * 2.0 - 1.0
    }

    static func analogAxisRev(value: IOHIDValue, element: IOHIDElement) -> Double {

        return -analogAxis(value: value, element: element)
    }

    static func mapAnalogAxis(value: IOHIDValue, element: IOHIDElement) -> Int? {

        let min = IOHIDElementGetLogicalMin(element)
        let max = IOHIDElementGetLogicalMax(element)
        let val = IOHIDValueGetIntegerValue(value)
        
        var v = (Double) (val - min) / (Double) (max - min)
        v = v * 2.0 - 1.0

        if v < 0 {
            if v < -0.45 { return -2 }
            if v > -0.35 { return 0 }
        } else {
            if v > 0.45 { return 2 }
            if v < 0.35 { return 0 }
        }
        
        return nil // Dead zone
    }
    
    static func mapHAxis(value: IOHIDValue, element: IOHIDElement) -> [vc64.GamePadAction]? {

        if let v = mapAnalogAxis(value: value, element: element) {
            return v == 2 ? [.PULL_RIGHT] : v == -2 ? [.PULL_LEFT] : [.RELEASE_X]
        } else {
            return nil
        }
    }
    
    static func mapVAxis(value: IOHIDValue, element: IOHIDElement) -> [vc64.GamePadAction]? {

        if let v = mapAnalogAxis(value: value, element: element) {
            return v == 2 ? [.PULL_DOWN] : v == -2 ? [.PULL_UP] : [.RELEASE_Y]
        } else {
            return nil
        }
    }

    static func mapVAxisRev(value: IOHIDValue, element: IOHIDElement) -> [vc64.GamePadAction]? {

        if let v = mapAnalogAxis(value: value, element: element) {
            return v == 2 ? [.PULL_UP] : v == -2 ? [.PULL_DOWN] : [.RELEASE_Y]
        } else {
            return nil
        }
    }

    func hidInputValueAction(context: UnsafeMutableRawPointer?,
                             result: IOReturn,
                             sender: UnsafeMutableRawPointer?,
                             value: IOHIDValue) {
        
        let element   = IOHIDValueGetElement(value)
        let intValue  = Int(IOHIDValueGetIntegerValue(value))
        let usagePage = Int(IOHIDElementGetUsagePage(element))
        let usage     = Int(IOHIDElementGetUsage(element))
                
        // debug(.hid, "usagePage = \(usagePage) usage = \(usage) value = \(intValue)")

        var events: [vc64.GamePadAction]?
        
        if usagePage == kHIDPage_Button {

            switch hScheme {
            
            case Schemes.B4B7:
                
                switch usage {
                case 5: events = intValue != 0 ? [.PULL_UP] : [.RELEASE_Y]
                case 6: events = intValue != 0 ? [.PULL_RIGHT] : [.RELEASE_X]
                case 7: events = intValue != 0 ? [.PULL_DOWN] : [.RELEASE_Y]
                case 8: events = intValue != 0 ? [.PULL_LEFT] : [.RELEASE_X]
                default: events = intValue != 0 ? [.PRESS_FIRE] : [.RELEASE_FIRE]
                }
            
            case Schemes.B11B14:
                
                switch usage {
                case 12: events = intValue != 0 ? [.PULL_UP] : [.RELEASE_Y]
                case 13: events = intValue != 0 ? [.PULL_DOWN] : [.RELEASE_Y]
                case 14: events = intValue != 0 ? [.PULL_LEFT] : [.RELEASE_X]
                case 15: events = intValue != 0 ? [.PULL_RIGHT] : [.RELEASE_X]
                default: events = intValue != 0 ? [.PRESS_FIRE] : [.RELEASE_FIRE]
                }
            
            default:
                events = intValue != 0 ? [.PRESS_FIRE] : [.RELEASE_FIRE]
            }
        }
        
        /* Experimental code for Paddle support
        var pos: Double?
        if usagePage == kHIDPage_GenericDesktop {

            switch usage {

            case kHIDUsage_GD_X where lScheme == Schemes.A0A1:   // A0
                pos = GamePad.analogAxis(value: value, element: element)

            case kHIDUsage_GD_X where lScheme == Schemes.A0A1r:  // A0
                pos = GamePad.analogAxis(value: value, element: element)

            case kHIDUsage_GD_Y where lScheme == Schemes.A0A1:   // A1
                break

            case kHIDUsage_GD_Y where lScheme == Schemes.A0A1r:  // A1
                break

            case kHIDUsage_GD_Z where rScheme == Schemes.A2A5:   // A2
                pos = GamePad.analogAxis(value: value, element: element)

            case kHIDUsage_GD_Z where rScheme == Schemes.A2A5r:  // A2
                pos = GamePad.analogAxis(value: value, element: element)

            case kHIDUsage_GD_Z where rScheme == Schemes.A2A3:   // A2
                pos = GamePad.analogAxis(value: value, element: element)

            case kHIDUsage_GD_Rx where lScheme == Schemes.A3A4:  // A3
                pos = GamePad.analogAxis(value: value, element: element)

            case kHIDUsage_GD_Rx where lScheme == Schemes.A2A3:  // A3
                break

            case kHIDUsage_GD_Ry where lScheme == Schemes.A3A4:  // A4
                break

            case kHIDUsage_GD_Rz where rScheme == Schemes.A2A5:  // A5
                break

            case kHIDUsage_GD_Rz where rScheme == Schemes.A2A5r: // A5
                break

            default:
                debug(.hid, "Unknown HID usage: \(usage)")
            }
        }

        if pos != nil {
            print("New paddle pos: \(pos!)")
            processPaddleEvents(pos: pos!)
        }
        */
        if usagePage == kHIDPage_GenericDesktop {
            
            switch usage {
            
            case kHIDUsage_GD_X where lScheme == Schemes.A0A1:   // A0
                events = GamePad.mapHAxis(value: value, element: element)

            case kHIDUsage_GD_X where lScheme == Schemes.A0A1r:  // A0
                events = GamePad.mapHAxis(value: value, element: element)

            case kHIDUsage_GD_Y where lScheme == Schemes.A0A1:   // A1
                events = GamePad.mapVAxis(value: value, element: element)

            case kHIDUsage_GD_Y where lScheme == Schemes.A0A1r:  // A1
                events = GamePad.mapVAxisRev(value: value, element: element)

            case kHIDUsage_GD_Z where rScheme == Schemes.A2A5:   // A2
                events = GamePad.mapHAxis(value: value, element: element)

            case kHIDUsage_GD_Z where rScheme == Schemes.A2A5r:  // A2
                events = GamePad.mapHAxis(value: value, element: element)

            case kHIDUsage_GD_Z where rScheme == Schemes.A2A3:   // A2
                events = GamePad.mapHAxis(value: value, element: element)

            case kHIDUsage_GD_Rx where lScheme == Schemes.A3A4:  // A3
                events = GamePad.mapHAxis(value: value, element: element)

            case kHIDUsage_GD_Rx where lScheme == Schemes.A2A3:  // A3
                events = GamePad.mapVAxisRev(value: value, element: element)

            case kHIDUsage_GD_Ry where lScheme == Schemes.A3A4:  // A4
                events = GamePad.mapVAxis(value: value, element: element)

            case kHIDUsage_GD_Rz where rScheme == Schemes.A2A5:  // A5
                events = GamePad.mapVAxis(value: value, element: element)

            case kHIDUsage_GD_Rz where rScheme == Schemes.A2A5r: // A5
                events = GamePad.mapVAxisRev(value: value, element: element)

            case 0x90 where hScheme == Schemes.U90U93:
                events = intValue != 0 ? [.PULL_UP] : [.RELEASE_Y]

            case 0x91 where hScheme == Schemes.U90U93:
                events = intValue != 0 ? [.PULL_DOWN] : [.RELEASE_Y]

            case 0x92 where hScheme == Schemes.U90U93:
                events = intValue != 0 ? [.PULL_RIGHT] : [.RELEASE_X]

            case 0x93 where hScheme == Schemes.U90U93:
                events = intValue != 0 ? [.PULL_LEFT] : [.RELEASE_X]

            case kHIDUsage_GD_Hatswitch:
                
                let shift = hScheme == Schemes.H0H7 ? 0 : 1
                    
                switch intValue - shift {
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
                debug(.hid, "Unknown HID usage: \(usage)")
            }
        }

        // Only proceed if the event is different than the previous one
        if events == nil || oldEvents[usage] == events { return }
        oldEvents[usage] = events!
        
        // Trigger events
        processJoystickEvents(events: events!)
    }
    
    //
    // Emulate events on the C64 side
    //
    
    @discardableResult
    func processJoystickEvents(events: [vc64.GamePadAction]) -> Bool {

        let c64 = manager.parent.emu!

        if let id = port {

            for event in events {
                c64.put(.JOY_EVENT, action: vc64.GamePadCmd(port: id, action: event))
            }
        }

        // Notify other components (if requested)
        if notify { myAppDelegate.devicePulled(events: events) }

        return events != []
    }

    @discardableResult
    func processMouseEvents(events: [vc64.GamePadAction]) -> Bool {

        let c64 = manager.parent.emu!

        if let id = port {

            for event in events {
                c64.put(.MOUSE_EVENT, action: vc64.GamePadCmd(port: id, action: event))
            }
        }

        return events != []
    }
    
    func processMouseEvents(delta: NSPoint) {
        
        let c64 = manager.parent.emu!

        // Check for a shaking mouse
        c64.port1.mouse.detectShakeRel(delta)

        if let id = port {

            c64.put(.MOUSE_MOVE_REL, coord: vc64.CoordCmd(port: id, x: delta.x, y: delta.y))
        }
    }

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

    func processKeyboardEvent(events: [vc64.GamePadAction]) {

        let c64 = manager.parent.emu!

        if let id = port {

            let type  = isMouse ? vc64.CmdType.MOUSE_EVENT : vc64.CmdType.JOY_EVENT

            for e in events {
                c64.put(type, action: vc64.GamePadCmd(port: id, action: e))
            }
        }
    }
}
