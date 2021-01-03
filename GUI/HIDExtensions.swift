// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension IOHIDDevice {
        
    @discardableResult
    func open() -> Bool {
        
        let optionBits = IOOptionBits(kIOHIDOptionsTypeNone)
        if IOHIDDeviceOpen(self, optionBits) != kIOReturnSuccess {
            
            track("WARNING: Cannot open HID device")
            return false
        }

        track("HID device opened")
        return true
    }
    
    @discardableResult
    func close() -> Bool {
                
        let optionBits = IOOptionBits(kIOHIDOptionsTypeNone)
        if IOHIDDeviceClose(self, optionBits) != kIOReturnSuccess {

            track("WARNING: Cannot close HID device")
            return false
        }
        
        track("HID device closed")
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

    var primaryUsage: String { return property(key: kIOHIDPrimaryUsageKey) ?? "" }
    
    // var isBuiltIn: Bool { return property(key: kIOHIDBuiltInKey) == "1" }

    var isMouse: Bool { return primaryUsage == "\(kHIDUsage_GD_Mouse)" }
            
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
    
    var isBuiltIn: Bool {
        
        /* The purpose of this function is to distinguish internal from external
         * HID devices. Note: The old implementation only checked the BuiltIn
         * key of the device, which turned out to be insufficient. Some external
         * devices such as the Sony DualShock 4 controller set the BuildIn key
         * to 1. To get a more accurate result, the new implementation first
         * evaluates the Transport property and classifies each wireless device
         * as external.
         */
        if property(key: kIOHIDTransportKey)?.hasPrefix("Bluetooth") == true {
            return false
        }
        
        return property(key: kIOHIDBuiltInKey) == "1"
    }
}
