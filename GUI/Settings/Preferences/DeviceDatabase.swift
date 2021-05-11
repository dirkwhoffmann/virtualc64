// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* This class implements a data provider for external devices such as GamePads
 * or Joysticks that can be connected via USB or Bluetooth.
 *
 * An object of this class is used inside the PreferencesController
 */

class DeviceDatabase {
 
    // Mapping scheme ( VendorID -> (ProductID -> Dictionary) )
    typealias DeviceDescription = [ String: [ String: [ String: String ] ] ]
    
    // Known devices
    var known: DeviceDescription = [:]
    
    // Devices configured by the user
    var custom: DeviceDescription = [:]
        
    init() {
    
        // Setup the lookup table for all known devices
        known =
            ["4":
                ["1": [:]],                     // aJoy Retro Adapter
             "13":
                ["0": ["L": "1", "R": "3", "H": "4"]], // Nimbus+
             "121":
                ["17": ["R": "1"],              // iNNEXT Retro (SNES)
                 "6354": [:]],                  // Mayflash Magic-NS 1.2
             "1035":
                ["25907": [:]],                 // Competition Pro SL-6602
             "1118":
                ["2835": ["H": "1"],            // XBox Carbon Black
                 "746": ["H": "3"]],            // XBox One Wired Controller
             "1133":
                ["49250": [:]],                 // Logitech Mouse
             "1155":
                ["36869": [:]],                 // RetroFun! Joystick Adapter
             "1356":
                ["616": [:],                    // Sony DualShock 3
                 "1476": [:],                   // Sony DualShock 4
                 "2508": [:]],                  // Sony Dualshock 4 (2nd Gen)
             "1848":
                ["8727": [:]],                  // Competition Pro SL-650212
             "3853":
                ["193": ["R": "1"]],            // HORIPAD for Nintendo Switch
             "7257":
                ["36": [:]]                     // The C64 Joystick
            ]
        
        // Load the lookup table for all custom devices
        let defaults = UserDefaults.standard
        if let obj = defaults.object(forKey: Keys.Dev.schemes) as? DeviceDescription {
            custom = obj
        }
        
        // track("\(custom)")
    }
    
    func save() {
        
        track()
        
        let defaults = UserDefaults.standard
        defaults.set(custom, forKey: Keys.Dev.schemes)
        
        track("\(custom)")
    }
    
    //
    // Querying the database
    //
    
    func isKnown(vendorID: String, productID: String) -> Bool {
        
        return known[vendorID]?[productID] != nil
    }

    func query(_ v: String, _ p: String, _ key: String) -> String? {
        
        if let value = custom[v]?[p]?[key] {
            return value
        }
        if let value = known[v]?[p]?[key] {
            return value
        }
        return nil
    }

    func name(vendorID: String, productID: String) -> String? {
        
        if let value = query(vendorID, productID, "Name") {
            return value
        }
        return nil
    }

    func icon(vendorID: String, productID: String) -> NSImage? {
    
        if let value = query(vendorID, productID, "Image") {
            return NSImage(named: value)
        }
        return nil
    }

    func left(vendorID: String, productID: String) -> Int {
    
        if let value = query(vendorID, productID, "L") {
            return Int.init(value) ?? 0
        }
        return 0
    }
        
    func right(vendorID: String, productID: String) -> Int {
    
        if let value = query(vendorID, productID, "R") {
            return Int.init(value) ?? 0
        }
        return 0
    }

    func hatSwitch(vendorID: String, productID: String) -> Int {
    
        if let value = query(vendorID, productID, "H") {
            return Int.init(value) ?? 0
        }
        return 0
    }

    //
    // Updating the database
    //
    
    func replace(_ v: String, _ p: String, _ key: String, _ value: String?) {
        
        // Replace key / value pair if it already exists
        if custom[v]?[p] != nil { custom[v]![p]![key] = value; return }
        
        // Only proceed if there is something to set
        if value == nil { return }
        
        // Add a new key / value pair
        if custom[v] != nil {
            custom[v]![p] = [key: value!]
        } else {
            custom[v] = [p: [key: value!]]
        }
    }

    func setLeft(vendorID: String, productID: String, _ value: String?) {
        
        replace(vendorID, productID, "L", value)
        // track("\(custom)")
    }

    func setRight(vendorID: String, productID: String, _ value: String?) {
        
        replace(vendorID, productID, "R", value)
        // track("\(custom)")
    }

    func setHatSwitch(vendorID: String, productID: String, _ value: String?) {
        
        replace(vendorID, productID, "H", value)
        // track("\(custom)")
    }
}
