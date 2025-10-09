// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* This class implements a data provider for Game pads connected via USB or
 * Bluetooth. It is build around the gamecontrollerdb.txt file hosted at:
 *
 *   https://github.com/mdqinc/SDL_GameControllerDB
 *
 * The file contains SDL compatible controller mappings such as the following:
 *
 *   030000000b0400003365000000010000,Competitio Pro,
 *     a:b0,b:b1,back:b2,leftx:a0,lefty:a1,start:b3,platform:Linux,
 *
 * Each entry starts with a GUID of the following format (from SDL_joystick.c):
 *
 *   16-bit bus
 *   16-bit CRC16 of the joystick name (can be zero)
 *   16-bit vendor ID
 *   16-bit zero
 *   16-bit product ID
 *   16-bit zero
 *   16-bit version
 *   8-bit driver identifier ('h' for HIDAPI, 'x' for XInput, etc.)
 *   8-bit driver-dependent type info
 *
 * When a game pad is attached to the emulator, the database is searched for
 * a matching vendor ID, product ID, and version. All other fields are ignored.
 * If no perfect match is found, the search is repeated by only comparing the
 * vendor ID and product ID.
 */

enum HIDEvent {
    
    case AXIS
    case BUTTON
    case DPAD_UP
    case DPAD_DOWN
    case DPAD_RIGHT
    case DPAD_LEFT
    case HATSWITCH
}

// Mapping scheme: HIDEvent -> Item -> Value -> [Actions]
typealias HIDMapping = [ HIDEvent: [ Int: [ Int: [GamePadAction] ] ] ]

func printHIDMapping(_ mapping: HIDMapping) {
    
    for (event, keys) in mapping {
        print("\(event):")
        for (key, value) in keys {
            print("  \(key): \(value)")
        }
    }
}

class DeviceDatabase {
    
    // Known devices
    private var devices: [GUID: String] = [:]
    
    //
    // Initializing
    //
    
    init() {
        
        devices = [:]
        
        // Load database from the user defaults storage
        if let encoded = UserDefaults.standard.data(forKey: Keys.Dev.schemes),
           let decoded = try? JSONDecoder().decode([GUID: String].self, from: encoded) {
            devices = decoded
        }
        
        // If the database still empty, register all known devices
        if devices.isEmpty { reset() }
    }
    
    func reset() {
        
        // Start from scratch
        devices = [:]
        
        // Register all known devices
        parse(file: "gamecontrollerdb", withExtension: "txt")
    }
    
    //
    // Creating the database
    //
    
    private func parse(file: String, withExtension ext: String) {
        
        if let url = Bundle.main.url(forResource: file, withExtension: ext) {
            
            do {
                
                let fileContents = try String(contentsOf: url, encoding: .utf8)
                for line in fileContents.split(separator: "\n") {
                    self.parse(line: String(line))
                }
                
            } catch { print("Error reading file: \(error)") }
        }
    }
    
    private func parse(line: String) {
        
        // Eliminate newline characters (if any)
        var descriptor = line.replacingOccurrences(of: "\n", with: "")
        
        // Eliminate unneeded commas at both ends
        descriptor = descriptor.trimmingCharacters(in: CharacterSet(charactersIn: ","))
        
        // Extract the GUID and create a mapping
        if let guid = GUID(string: descriptor) { devices[guid] = descriptor }
    }
    
    func update(line: String) {
        
        parse(line: line)
    }
    
    //
    // Querying descriptors
    //
    
    func isKnown(guid: GUID, exact: Bool = false) -> Bool {
        
        return seek(guid: guid, exact: exact) != nil
    }
    
    func seek(guid: GUID, withDelimiter del: String) -> String {
        
        let result = seek(guid: guid).trimmingCharacters(in: CharacterSet(charactersIn: ","))
        return result.replacingOccurrences(of: ",", with: del)
    }
    
    func seek(guid: GUID) -> String {
        
        let macOS = "Mac OS X"
        
        // Search for a perfect match
        if let result = seek(guid: guid, exact: true, platform: macOS) { return result }
        
        // Search again, ignoring the version number
        if let result = seek(guid: guid, exact: false, platform: macOS) { return result }
        
        // Search again, ignoring the platform
        if let result = seek(guid: guid, exact: true) { return result }
        
        // Search again, ignoring the version number and the platform
        if let result = seek(guid: guid, exact: false) { return result }
        
        // Return a fallback descriptor
        return "Generic,a:b0,b:b1,leftx:a0,lefty:a1"
    }
    
    private func seek(guid: GUID, exact: Bool, platform: String? = nil) -> String? {
        
        for (otherguid, descriptor) in devices {
            
            // Compare the vendor ID
            if !guid.match(guid: otherguid, offset: 8, length: 4) { continue }
            
            // Compare the product ID
            if !guid.match(guid: otherguid, offset: 16, length: 4) { continue }
            
            // Compare the version if requested
            if exact && !guid.match(guid: otherguid, offset: 24, length: 4) { continue }
            
            // Compare the platform if requested
            if platform != nil && platform != extract(from: descriptor, key: "platform:") { continue }
            
            return descriptor;
        }
        
        return nil
    }
    
    func extract(from descriptor: String, key: String) -> String? {
        
        guard let range = descriptor.range(of: key) else { return nil }
        
        let start = descriptor.index(range.upperBound, offsetBy: 0)
        let end = descriptor[start...].firstIndex(of: ",") ?? descriptor.endIndex
        
        return String(descriptor[start..<end])
    }
    
    //
    // Compute mappings
    //
    
    func query(guid: GUID) -> HIDMapping {
        
        return query(descriptor: seek(guid: guid))
    }
    
    func query(descriptor: String) -> HIDMapping {
        
        var result: HIDMapping = [
            
            .AXIS: [:],
            .BUTTON: [:],
            .DPAD_UP: [0:[:]],
            .DPAD_DOWN: [0:[:]],
            .DPAD_RIGHT: [0:[:]],
            .DPAD_LEFT: [0:[:]],
            .HATSWITCH: [0:[:]]
        ]
        
        // Add default values for the directional pad
        result[.DPAD_UP]![0] = [0:[.RELEASE_Y], 1:[.PULL_UP]]
        result[.DPAD_DOWN]![0] = [0:[.RELEASE_Y], 1:[.PULL_DOWN]]
        result[.DPAD_RIGHT]![0] = [0:[.RELEASE_X,], 1:[.PULL_RIGHT]]
        result[.DPAD_LEFT]![0] = [0:[.RELEASE_X,], 1:[.PULL_LEFT]]
        
        // Add default values for the hat switch
        result[.HATSWITCH]![0]![0] = [.RELEASE_XY]
        result[.HATSWITCH]![0]![1] = [.PULL_UP]
        result[.HATSWITCH]![0]![3] = [.PULL_UP,.PULL_RIGHT]
        result[.HATSWITCH]![0]![2] = [.PULL_RIGHT]
        result[.HATSWITCH]![0]![6] = [.PULL_DOWN,.PULL_RIGHT]
        result[.HATSWITCH]![0]![4] = [.PULL_DOWN]
        result[.HATSWITCH]![0]![12] = [.PULL_DOWN,.PULL_LEFT]
        result[.HATSWITCH]![0]![8] = [.PULL_LEFT]
        result[.HATSWITCH]![0]![9] = [.PULL_UP,.PULL_LEFT]
        
        // Iterate through all key value pairs
        for assignment in descriptor.split(separator: ",") {
            
            let pair = assignment.split(separator: ":").map { String($0) }
            if pair.count != 2 { continue }
            
            switch (pair[1]) {
                
            case "a0":  result[.AXIS]![0] = mapAxis(key: pair[0])
            case "a1":  result[.AXIS]![1] = mapAxis(key: pair[0])
            case "a2":  result[.AXIS]![2] = mapAxis(key: pair[0])
            case "a3":  result[.AXIS]![3] = mapAxis(key: pair[0])
            case "a4":  result[.AXIS]![4] = mapAxis(key: pair[0])
            case "a5":  result[.AXIS]![5] = mapAxis(key: pair[0])
            case "+a0":  result[.AXIS]![0] = mapAxis(key: pair[0])
            case "+a1":  result[.AXIS]![1] = mapAxis(key: pair[0])
            case "+a2":  result[.AXIS]![2] = mapAxis(key: pair[0])
            case "+a3":  result[.AXIS]![3] = mapAxis(key: pair[0])
            case "+a4":  result[.AXIS]![4] = mapAxis(key: pair[0])
            case "+a5":  result[.AXIS]![5] = mapAxis(key: pair[0])
            case "-a0":  result[.AXIS]![0] = mapAxis(key: pair[0], rev: true)
            case "-a1":  result[.AXIS]![1] = mapAxis(key: pair[0], rev: true)
            case "-a2":  result[.AXIS]![2] = mapAxis(key: pair[0], rev: true)
            case "-a3":  result[.AXIS]![3] = mapAxis(key: pair[0], rev: true)
            case "-a4":  result[.AXIS]![4] = mapAxis(key: pair[0], rev: true)
            case "-a5":  result[.AXIS]![5] = mapAxis(key: pair[0], rev: true)
            case "a0~": result[.AXIS]![0] = mapAxis(key: pair[0], rev: true)
            case "a1~": result[.AXIS]![1] = mapAxis(key: pair[0], rev: true)
            case "a2~": result[.AXIS]![2] = mapAxis(key: pair[0], rev: true)
            case "a3~": result[.AXIS]![3] = mapAxis(key: pair[0], rev: true)
            case "a4~": result[.AXIS]![4] = mapAxis(key: pair[0], rev: true)
            case "a5~": result[.AXIS]![5] = mapAxis(key: pair[0], rev: true)
            case "b0":  result[.BUTTON]![0] = mapButton(key: pair[0])
            case "b1":  result[.BUTTON]![1] = mapButton(key: pair[0])
            case "b2":  result[.BUTTON]![2] = mapButton(key: pair[0])
            case "b3":  result[.BUTTON]![3] = mapButton(key: pair[0])
            case "b4":  result[.BUTTON]![4] = mapButton(key: pair[0])
            case "b5":  result[.BUTTON]![5] = mapButton(key: pair[0])
            case "b6":  result[.BUTTON]![6] = mapButton(key: pair[0])
            case "b7":  result[.BUTTON]![7] = mapButton(key: pair[0])
            case "b8":  result[.BUTTON]![8] = mapButton(key: pair[0])
            case "b9":  result[.BUTTON]![9] = mapButton(key: pair[0])
            case "b10": result[.BUTTON]![10] = mapButton(key: pair[0])
            case "b11": result[.BUTTON]![11] = mapButton(key: pair[0])
            case "b12": result[.BUTTON]![12] = mapButton(key: pair[0])
            case "b13": result[.BUTTON]![13] = mapButton(key: pair[0])
            case "b14": result[.BUTTON]![14] = mapButton(key: pair[0])
            case "b15": result[.BUTTON]![15] = mapButton(key: pair[0])
            case "b16": result[.BUTTON]![16] = mapButton(key: pair[0])
                
            default:
                break
            }
        }
        
        // printHIDMapping(result)
        return result
    }
    
    private func mapAxis(key: String, rev: Bool = false) -> [Int: [GamePadAction]] {
        
        var result: [Int: [GamePadAction]] = [:]
        
        switch (key) {
            
        case "leftx", "rightx":
            result = [-1: [.PULL_LEFT], 0: [.RELEASE_X], 1: [.PULL_RIGHT]]
        case "lefty", "righty":
            result = [-1: [.PULL_UP], 0: [.RELEASE_Y], 1: [.PULL_DOWN]]
        case "dpup":
            result = [-1: [.PULL_DOWN], 0: [.RELEASE_Y], 1: [.PULL_UP]]
        case "dpdown":
            result = [-1: [.PULL_UP], 0: [.RELEASE_Y], 1: [.PULL_DOWN]]
        case "dpright":
            result = [-1: [.PULL_LEFT], 0: [.RELEASE_X], 1: [.PULL_RIGHT]]
        case "dpleft":
            result = [-1: [.PULL_RIGHT], 0: [.RELEASE_X], 1: [.PULL_LEFT]]
        default:
            break
        }
        
        if rev, let left = result[-1], let right = result[1] {
            
            result[-1] = right
            result[1]  = left
        }
        
        return result
    }
    
    private func mapButton(key: String) -> [Int: [GamePadAction]] {
        
        switch (key) {
            
        case "a", "b", "x", "y", "leftshoulder", "rightshoulder":
            return [0: [.RELEASE_FIRE], 1: [.PRESS_FIRE]]
        case "dpdown":
            return [0: [.RELEASE_Y], 1: [.PULL_DOWN]]
        case "dpup":
            return [0: [.RELEASE_Y], 1: [.PULL_UP]]
        case "dpleft":
            return [0: [.RELEASE_X], 1: [.PULL_LEFT]]
        case "dpright":
            return [0: [.RELEASE_X], 1: [.PULL_RIGHT]]
        default:
            return [:]
        }
    }
    
    //
    // Misc
    //
    
    func save() {
        
        debug(.hid)
        
        if let encoded = try? JSONEncoder().encode(devices) {
            UserDefaults.standard.set(encoded, forKey: Keys.Dev.schemes)
        }
    }
}
