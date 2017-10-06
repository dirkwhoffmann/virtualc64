//
//  GamePadManager.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 06.10.17.
//

import Foundation
import IOKit.hid

@objc class GamePadManager : NSObject {
    
    // private let inputLock = NSLock()
    // https://github.com/joekarl/swift_handmade_hero/blob/master/Handmade%20Hero%20OSX/Handmade%20Hero%20OSX/InputManager.swift
    
    private var hidManager: IOHIDManager // Unmanaged<IOHIDManager>

    @objc override init() {
        
        hidManager = IOHIDManagerCreate(kCFAllocatorDefault, IOOptionBits(kIOHIDOptionsTypeNone));
        super.init()

        let hidContext = unsafeBitCast(self, to: UnsafeMutableRawPointer.self)
        // let hidManagerVal = hidManager.takeUnretainedValue();
        // device types we want to monitor
        
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
        
        // Bridging closures (Used to bridge between Swift and plain C callbacks)
        let matchingCallback : IOHIDDeviceCallback = { inContext, inResult, inSender, device in
            let this : GamePadManager = unsafeBitCast(inContext, to: GamePadManager.self)
            this.hidDeviceAdded(context: inContext, result: inResult, sender: inSender, device: device)
        }
        
        let removalCallback : IOHIDDeviceCallback = { inContext, inResult, inSender, device in
            let this : GamePadManager = unsafeBitCast(inContext, to: GamePadManager.self)
            this.hidDeviceRemoved(context: inContext, result: inResult, sender: inSender, device: device)
        }

        let actionCallback : IOHIDValueCallback = { inContext, inResult, inSender, value in
            let this : GamePadManager = unsafeBitCast(inContext, to: GamePadManager.self)
            this.hidDeviceAction(context: inContext, result: inResult, sender: inSender, value: value)
        }

        // Configure HID manager
        IOHIDManagerSetDeviceMatchingMultiple(hidManager, deviceCriteria as CFArray)
        IOHIDManagerRegisterDeviceMatchingCallback(hidManager, matchingCallback, hidContext);
        IOHIDManagerRegisterDeviceRemovalCallback(hidManager, removalCallback, hidContext);
        IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), CFRunLoopMode.defaultMode.rawValue)
        IOHIDManagerOpen(hidManager, IOOptionBits(kIOHIDOptionsTypeNone))
        IOHIDManagerRegisterInputValueCallback(hidManager, actionCallback, hidContext);
    }
    
    func hidDeviceAdded(context: Optional<UnsafeMutableRawPointer>,
                        result: IOReturn,
                        sender: Optional<UnsafeMutableRawPointer>,
                        device: IOHIDDevice) {
    
        NSLog("Device added")
    }
    
    func hidDeviceRemoved(context: Optional<UnsafeMutableRawPointer>,
                          result: IOReturn,
                          sender: Optional<UnsafeMutableRawPointer>,
                          device: IOHIDDevice) {
        
        NSLog("Device removed")
    }
    
    func hidDeviceAction(context: Optional<UnsafeMutableRawPointer>,
                         result: IOReturn,
                         sender: Optional<UnsafeMutableRawPointer>,
                         value: IOHIDValue) {
        NSLog("Device Action")
    }
}
