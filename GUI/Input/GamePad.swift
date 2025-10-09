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
    
    // References
    var manager: GamePadManager
    var prefs: Preferences { return manager.controller.pref }
    var config: Configuration { return manager.controller.config }
    var db: DeviceDatabase { return myAppDelegate.database }
    
    // The control port this device is connected to (0, 1, or nil)
    var port: Int?
    
    // HID mapping
    var mapping: HIDMapping?
    
    // Reference to the HID device
    var device: IOHIDDevice?
    var vendorID: String { return device?.vendorID ?? "" }
    var productID: String { return device?.productID ?? "" }
    var locationID: String { return device?.locationID ?? "" }
    var version: String { return device?.versionNumberKey ?? "" }
    var guid: GUID { return device?.guid ?? GUID() }
    
    // Type of the managed device (joystick or mouse)
    var type: ControlPortDevice
    var isMouse: Bool { return type == .MOUSE }
    var isJoystick: Bool { return type == .JOYSTICK }
    
    // Name of the managed device
    var name = ""
    
    // Icon of this device
    var icon: NSImage?
    
    // Indicates if this device is officially supported
    var isKnown: Bool { return db.isKnown(guid: guid) }
    
    // Keymap of the managed device (only set for keyboard emulated devices)
    var keyMap: Int?
    
    // Indicates if a joystick emulation key is currently pressed
    var keyUp = false, keyDown = false, keyLeft = false, keyRight = false
    
    // Indicates if other components should be notified when the device is used
    var notify = false
    
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
        
        name = device?.name ?? "HID device"
        // icon = NSImage(named: isMouse ? "devMouseTemplate" : "devGamepad1Template")!
        icon = isMouse ? SFSymbol.get(.mouse) : SFSymbol.get(.gamecontroller)
        
        updateMapping()
    }
    
    func updateMapping() {
        
        if device != nil {
            
            mapping = db.query(guid: device!.guid)
        }
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
        print("")
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
        
        guard let n = keyMap, let direction = prefs.keyMaps[n][macKey] else { return [] }
        
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
        
        debug(.events, "keyUpEvents \(direction)")
        
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
        
        let min = IOHIDElementGetLogicalMin(element)
        let max = IOHIDElementGetLogicalMax(element)
        let val = IOHIDValueGetIntegerValue(value)
        
        var v = (Double) (val - min) / (Double) (max - min)
        v = v * 2.0 - 1.0
        
        if v < 0 {
            if v < -0.45 { return -1 }
            if v > -0.35 { return 0 }
        } else {
            if v > 0.45 { return 1 }
            if v < 0.35 { return 0 }
        }
        
        return nil // Dead zone
    }
    
    func mapHatSwitch(value: IOHIDValue, element: IOHIDElement) -> Int? {
        
        
        let val = IOHIDValueGetIntegerValue(value)
        let min = IOHIDElementGetLogicalMin(element)
        
        // let max = IOHIDElementGetLogicalMax(element)
        // print("min = \(min) max = \(max) val = \(val)")
        
        switch (val - min) {
        case 0: return 1
        case 1: return 1 | 2
        case 2: return 2
        case 3: return 2 | 4
        case 4: return 4
        case 5: return 4 | 8
        case 6: return 8
        case 7: return 8 | 1
        default: return 0
        }
    }
    
    func hidInputValueAction(context: UnsafeMutableRawPointer?,
                             result: IOReturn,
                             sender: UnsafeMutableRawPointer?,
                             value: IOHIDValue) {
        
        var hidEvent: (HIDEvent, Int, Int)?
        
        let element   = IOHIDValueGetElement(value)
        let intValue  = Int(IOHIDValueGetIntegerValue(value))
        let usagePage = Int(IOHIDElementGetUsagePage(element))
        let usage     = Int(IOHIDElementGetUsage(element))
        
        // debug(.hid, "usagePage = \(usagePage) usage = \(usage) value = \(intValue)")
        
        if usagePage == kHIDPage_Button {
            
            // usage - 1 yields the button number with 0 = first button
            hidEvent = (.BUTTON, usage - 1, intValue)
        }
        
        if usagePage == kHIDPage_GenericDesktop {
            
            switch usage {
                
            case kHIDUsage_GD_X: // A0
                if let value = mapAnalogAxis(value: value, element: element) {
                    hidEvent = (.AXIS, 0, value)
                }
                
            case kHIDUsage_GD_Y: // A1
                if let value = mapAnalogAxis(value: value, element: element) {
                    hidEvent = (.AXIS, 1, value)
                }
                
            case kHIDUsage_GD_Z: // A2
                if let value = mapAnalogAxis(value: value, element: element) {
                    hidEvent = (.AXIS, 2, value)
                }
                
            case kHIDUsage_GD_Rx: // A3
                if let value = mapAnalogAxis(value: value, element: element) {
                    hidEvent = (.AXIS, 3, value)
                }
                
            case kHIDUsage_GD_Ry: // A4
                if let value = mapAnalogAxis(value: value, element: element) {
                    hidEvent = (.AXIS, 4, value)
                }
                
            case kHIDUsage_GD_Rz: // A5
                if let value = mapAnalogAxis(value: value, element: element) {
                    hidEvent = (.AXIS, 5, value)
                }
                
            case kHIDUsage_GD_Hatswitch:
                if let value = mapHatSwitch(value: value, element: element) {
                    hidEvent = (.HATSWITCH, 0, value)
                }
                
            case kHIDUsage_GD_DPadUp:
                hidEvent = (.DPAD_UP, 0, intValue == 0 ? 0 : 1)
                
            case kHIDUsage_GD_DPadDown:
                hidEvent = (.DPAD_DOWN, 0, intValue == 0 ? 0 : 1)
                
            case kHIDUsage_GD_DPadRight:
                hidEvent = (.DPAD_RIGHT, 0, intValue == 0 ? 0 : 1)
                
            case kHIDUsage_GD_DPadLeft:
                hidEvent = (.DPAD_LEFT, 0, intValue == 0 ? 0 : 1)
                
            default:
                debug(.hid, "Unknown HID usage: \(usage)")
            }
        }
        
        if let hid = hidEvent {
            
            // Notify the GUI
            if let controller = myAppDelegate.settingsController, controller.isVisible {
                controller.devicesVC?.refreshDeviceEvent(event: hid.0, nr: hid.1, value: hid.2)
            }
            
            // Map the HID event to an action list
            if let events = mapping?[hid.0]?[hid.1]?[hid.2] {
                
                // Only proceed if the event is different than the previous one
                if oldEvents[usage] != events {
                    
                    // Trigger events
                    processJoystickEvents(events: events)
                    oldEvents[usage] = events
                }
            }
        }
    }
    
    //
    // Emulate events on the C64 side
    //
    
    @discardableResult
    func processJoystickEvents(events: [GamePadAction]) -> Bool {
        
        let emu = manager.controller.emu!

        if port == 1 { for e in events { emu.port1.joystick.trigger(e) } }
        if port == 2 { for e in events { emu.port2.joystick.trigger(e) } }

        // Notify the GUI
        if let controller = myAppDelegate.settingsController, controller.isVisible {
            controller.devicesVC?.refreshDeviceActions(actions: events)
        }
        
        // if notify { myAppDelegate.devicePulled(events: events) }
        
        return events != []
    }
    
    @discardableResult
    func processMouseEvents(events: [GamePadAction]) -> Bool {
        
        let emu = manager.controller.emu!

        if port == 1 { for e in events { emu.port1.mouse.trigger(e) } }
        if port == 2 { for e in events { emu.port2.mouse.trigger(e) } }

        return events != []
    }
    
    func processMouseEvents(delta: NSPoint) {
        
        let emu = manager.controller.emu!

        // Check for a shaking mouse
        emu.port1.mouse.detectShakeRel(delta)

        if port == 1 { emu.port1.mouse.setDxDy(delta) }
        if port == 2 { emu.port2.mouse.setDxDy(delta) }
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
    
    func processKeyboardEvent(events: [GamePadAction]) {
        
        let emu = manager.controller.emu!

        if isMouse {
            if port == 1 { for e in events { emu.port1.mouse.trigger(e) } }
            if port == 2 { for e in events { emu.port2.mouse.trigger(e) } }
        } else {
            if port == 1 { for e in events { emu.port1.joystick.trigger(e) } }
            if port == 2 { for e in events { emu.port2.joystick.trigger(e) } }
        }
    }
}
