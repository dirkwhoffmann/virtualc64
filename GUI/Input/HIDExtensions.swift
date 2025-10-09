// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

struct GUID: Hashable, Codable {
    
    var guid = String(repeating: "0", count: 32)
    
    init?(string: String) {
        
        if string.count < 32 { return nil }
        
        guid = String(string.prefix(32))
        
        let hexSet = CharacterSet(charactersIn: "0123456789abcdefABCDEF")
        for char in guid.unicodeScalars {
            if !hexSet.contains(char) { return nil }
        }
    }
    
    init(vendorID: Int = 0, productID: Int = 0, version: Int = 0) {
        
        func littleEndian (_ value: Int) -> String {
            
            let lo = String(format: "%02x", value & 0xFF)
            let hi = String(format: "%02x", (value >> 8) & 0xFF)
            
            return lo + hi
        }
        
        let _vendor = littleEndian(vendorID)
        let _product = littleEndian(productID)
        let _version = littleEndian(version)
        
        guid = "00000000" + _vendor + "0000" + _product + "0000" + _version + "0000000000"
        
        assert(self.vendorID == vendorID)
        assert(self.productID == productID)
        assert(self.version == version)
    }
    
    var vendorID: Int {
        
        let i1 = guid.index(guid.startIndex, offsetBy: 8)
        let i2 = guid.index(guid.startIndex, offsetBy: 10)
        let i3 = guid.index(guid.startIndex, offsetBy: 12)
        
        return Int(guid[i2..<i3] + guid[i1..<i2], radix: 16) ?? 0
    }
    
    var productID: Int {
        
        let i1 = guid.index(guid.startIndex, offsetBy: 16)
        let i2 = guid.index(guid.startIndex, offsetBy: 18)
        let i3 = guid.index(guid.startIndex, offsetBy: 20)
        
        return Int(guid[i2..<i3] + guid[i1..<i2], radix: 16) ?? 0
    }
    
    var version: Int {
        
        let i1 = guid.index(guid.startIndex, offsetBy: 24)
        let i2 = guid.index(guid.startIndex, offsetBy: 26)
        let i3 = guid.index(guid.startIndex, offsetBy: 28)
        
        return Int(guid[i2..<i3] + guid[i1..<i2], radix: 16) ?? 0
    }
    
    func match(vendorID: Int, productID: Int, version: Int? = nil) -> Bool {
        
        if self.vendorID != vendorID { return false }
        if self.productID != productID { return false }
        if version != nil && self.version != version { return false }
        
        return true
    }
    
    func match(guid other: GUID, offset: Int, length: Int) -> Bool {
        
        let a1 = guid.index(guid.startIndex, offsetBy: offset)
        let a2 = guid.index(guid.startIndex, offsetBy: offset + length)
        let b1 = other.guid.index(other.guid.startIndex, offsetBy: offset)
        let b2 = other.guid.index(other.guid.startIndex, offsetBy: offset + length)
        
        return guid[a1..<a2] == other.guid[b1..<b2]
    }
}

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
    var versionNumberKey: String { return property(key: kIOHIDVersionNumberKey) ?? "" }
    var manufacturerKey: String { return property(key: kIOHIDManufacturerKey) ?? "" }
    var usageKey: String { return property(key: kIOHIDPrimaryUsageKey) ?? "" }
    var builtInKey: String { return property(key: kIOHIDBuiltInKey) ?? "" }
    var transportKey: String { return property(key: kIOHIDTransportKey) ?? "" }
    
    var isMouse: Bool { return usageKey == "\(kHIDUsage_GD_Mouse)" }
    
    var guid: GUID {
        
        return GUID(vendorID: Int(vendorID) ?? 0,
                    productID: Int(productID) ?? 0,
                    version: Int(versionNumberKey) ?? 0)
    }
    
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
        
        let apple = manufacturerKey.hasPrefix("Apple")
        let bluetooth = transportKey.hasPrefix("Bluetooth")
        let spi = transportKey.hasPrefix("SPI")
        let builtIn = builtInKey == "1"
        
        // Classify all SPI connected devices as internal
        if spi { return true }
        
        // For mice, evaluate the builtIn key and the manufacturer key
        if isMouse { return builtIn || apple }
        
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
