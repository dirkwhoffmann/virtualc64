// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension IOHIDDevice {
        
    @discardableResult
    func open() -> Bool {
        
        let optionBits = IOOptionBits(kIOHIDOptionsTypeNone)
        if IOHIDDeviceOpen(self, optionBits) != kIOReturnSuccess {
            
            warn("Cannot open HID device")
            return false
        }

        debug(.hid, "HID device opened")
        return true
    }
    
    @discardableResult
    func close() -> Bool {
                
        let optionBits = IOOptionBits(kIOHIDOptionsTypeNone)
        if IOHIDDeviceClose(self, optionBits) != kIOReturnSuccess {

            warn("Cannot close HID device")
            return false
        }
        
        debug(.hid, "HID device closed")
        return true
    }
    
    func property(key: String) -> String? {
            
        if let prop = IOHIDDeviceGetProperty(self, key as CFString) {
            return "\(prop)"
        }
        return nil
    }
    
    var name: String { return property(key: kIOHIDProductKey) ?? "" }
    var vendorID: String { return property(key: kIOHIDVendorIDKey) ?? "" }
    var productID: String { return property(key: kIOHIDProductIDKey) ?? "" }
    var locationID: String { return property(key: kIOHIDLocationIDKey) ?? "" }
    var usageKey: String { return property(key: kIOHIDPrimaryUsageKey) ?? "" }
    var builtInKey: String { return property(key: kIOHIDBuiltInKey) ?? "" }
    var transportKey: String { return property(key: kIOHIDTransportKey) ?? "" }

    var isMouse: Bool { return usageKey == "\(kHIDUsage_GD_Mouse)" }
            
    func listProperties() {
        
        let keys = [
            
            kIOHIDTransportKey, kIOHIDVendorIDKey, kIOHIDVendorIDSourceKey,
            kIOHIDProductIDKey, kIOHIDVersionNumberKey, kIOHIDManufacturerKey,
            kIOHIDProductKey, kIOHIDSerialNumberKey, kIOHIDCountryCodeKey,
            kIOHIDStandardTypeKey, kIOHIDLocationIDKey, kIOHIDDeviceUsageKey,
            kIOHIDDeviceUsagePageKey, kIOHIDDeviceUsagePairsKey,
            kIOHIDPrimaryUsageKey, kIOHIDPrimaryUsagePageKey,
            kIOHIDMaxInputReportSizeKey, kIOHIDMaxOutputReportSizeKey,
            kIOHIDMaxFeatureReportSizeKey, kIOHIDReportIntervalKey,
            kIOHIDSampleIntervalKey, kIOHIDBatchIntervalKey,
            kIOHIDRequestTimeoutKey, kIOHIDReportDescriptorKey,
            kIOHIDResetKey, kIOHIDKeyboardLanguageKey, kIOHIDAltHandlerIdKey,
            kIOHIDBuiltInKey, kIOHIDDisplayIntegratedKey, kIOHIDProductIDMaskKey,
            kIOHIDProductIDArrayKey, kIOHIDPowerOnDelayNSKey, kIOHIDCategoryKey,
            kIOHIDMaxResponseLatencyKey, kIOHIDUniqueIDKey,
            kIOHIDPhysicalDeviceUniqueIDKey
        ]
        
        for key in keys {
            if let prop = property(key: key) {
                print("\t" + key + ": \(prop)")
            }
        }
    }
    
    var isInternalDevice: Bool {
        
        /* The purpose of this function is to distinguish internal from external
         * HID devices. Note: The old implementation only checked the BuiltIn
         * key of the device, which turned out to be insufficient. Some external
         * devices such as the Sony DualShock 4 controller set the BuildIn key
         * to 1. To get a more accurate result, the new implementation first
         * evaluates the Transport property and classifies each wireless device
         * as external.
         */
        
        let bluetooth = transportKey.hasPrefix("Bluetooth")
        let spi = transportKey.hasPrefix("SPI")
        let builtIn = builtInKey == "1"

        // Classify all SPI connected devices as internal
        if spi { return true }

        // For mice, evaluate the BuiltIn key
        if isMouse { return builtIn }

        // For other device types, consider each Bluetooth device as external
        if bluetooth { return false }
        
        // If it wasn't a Bluetooth device, evaluate the BuitIn key
        return builtIn
    }
    
    var usageDescription: String? {
        
        if let usage = Int(usageKey) {
            
            switch usage {
            case kHIDUsage_GD_Mouse:               return "Mouse"
            case kHIDUsage_GD_Joystick:            return "Joystick"
            case kHIDUsage_GD_GamePad:             return "GamePad"
            case kHIDUsage_GD_Keyboard:            return "Keyboard"
            case kHIDUsage_GD_Keypad:              return "Keypad"
            case kHIDUsage_GD_MultiAxisController: return "Multi axis controller"
            default: break
            }
        }
        return nil
    }
}
