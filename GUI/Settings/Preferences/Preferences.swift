// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* Preferences
 *
 * This class stores all emulator settings that belong to the application level.
 * There is a single object of this class stored in the application delegate.
 * The object is shared among all emulator instances.
 *
 * See class "Configuration" for instance specific settings.
 */

class Preferences {
    
    var myAppDelegate: MyAppDelegate { return NSApp.delegate as! MyAppDelegate }
    
    //
    // General
    //
        
    // Fullscreen
    var keepAspectRatio = GeneralDefaults.std.keepAspectRatio
    var exitOnEsc = GeneralDefaults.std.exitOnEsc
    
    // Snapshots and screenshots
    var autoSnapshots = GeneralDefaults.std.autoSnapshots
    var snapshotInterval = 0 {
        didSet { for c in myAppDelegate.controllers { c.startSnapshotTimer() } }
    }
    var screenshotSource = GeneralDefaults.std.screenshotSource
    var screenshotTarget = GeneralDefaults.std.screenshotTarget
    var screenshotTargetIntValue: Int {
        get { return Int(screenshotTarget.rawValue) }
        set { screenshotTarget = NSBitmapImageRep.FileType(rawValue: UInt(newValue))! }
    }
    
    // Warp mode
    var warpMode = GeneralDefaults.std.warpMode {
        didSet { for c in myAppDelegate.controllers { c.updateWarp() } }
    }
    var warpModeIntValue: Int {
        get { return Int(warpMode.rawValue) }
        set { warpMode = WarpMode.init(rawValue: newValue)! }
    }
    
    // Misc
    var driveEjectUnasked = GeneralDefaults.std.driveEjectUnasked
    var closeWithoutAsking = GeneralDefaults.std.closeWithoutAsking
    var pauseInBackground = GeneralDefaults.std.pauseInBackground
    
    //
    // Controls
    //
    
    // Emulation keys
    var keyMaps = [ ControlsDefaults.std.mouseKeyMap,
                    ControlsDefaults.std.joyKeyMap1,
                    ControlsDefaults.std.joyKeyMap2 ]
    
    // Joystick
    var disconnectJoyKeys = ControlsDefaults.std.disconnectJoyKeys
    var autofire = ControlsDefaults.std.autofire {
        didSet {
            for c64 in myAppDelegate.proxies {
                c64.configure(.AUTOFIRE, enable: autofire)
            }
        }
    }
    var autofireBullets = ControlsDefaults.std.autofireBullets {
        didSet {
            for c64 in myAppDelegate.proxies {
                c64.configure(.AUTOFIRE_BULLETS, value: autofireBullets)
            }
        }
    }
    var autofireFrequency = ControlsDefaults.std.autofireFrequency {
        didSet {
            let autofireDelay = Int(50.0 / autofireFrequency)
            for c64 in myAppDelegate.proxies {
                c64.configure(.AUTOFIRE_DELAY, value: autofireDelay)
            }
        }
    }
    
    // Mouse
    var mouseModel = ControlsDefaults.std.mouseModel {
        didSet {
            for c64 in myAppDelegate.proxies {
                c64.configure(.MOUSE_MODEL, value: mouseModel.rawValue)
            }
        }
    }
    var mouseModelIntValue: Int {
        get { return Int(mouseModel.rawValue) }
        set { mouseModel = MouseModel(rawValue: newValue) ?? .C1350 }
    }

    var retainMouseKeyComb = ControlsDefaults.std.retainMouseKeyComb
    var retainMouseWithKeys = ControlsDefaults.std.retainMouseWithKeys
    var retainMouseByClick = ControlsDefaults.std.retainMouseByClick
    var retainMouseByEntering = ControlsDefaults.std.retainMouseByEntering
    var releaseMouseKeyComb = ControlsDefaults.std.retainMouseKeyComb
    var releaseMouseWithKeys = ControlsDefaults.std.releaseMouseWithKeys
    var releaseMouseByShaking = ControlsDefaults.std.releaseMouseByShaking

    //
    // Keyboard
    //
    
    // Mapping
    var mapKeysByPosition = false
    var keyMap: [MacKey: C64Key] = [:]
    
    //
    // General
    //
    
    func loadGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard
                
        // Fullscreen
        keepAspectRatio = defaults.bool(forKey: Keys.Gen.keepAspectRatio)
        exitOnEsc = defaults.bool(forKey: Keys.Gen.exitOnEsc)
        
        // Snapshots and screenshots
        autoSnapshots = defaults.bool(forKey: Keys.Gen.autoSnapshots)
        snapshotInterval = defaults.integer(forKey: Keys.Gen.autoSnapshotInterval)
        screenshotSource = defaults.integer(forKey: Keys.Gen.screenshotSource)
        screenshotTargetIntValue = defaults.integer(forKey: Keys.Gen.screenshotTarget)
        
        // Warp mode
        warpModeIntValue = defaults.integer(forKey: Keys.Gen.warpMode)
        
        // Misc
        driveEjectUnasked = defaults.bool(forKey: Keys.Gen.driveEjectUnasked)
        pauseInBackground = defaults.bool(forKey: Keys.Gen.pauseInBackground)
        closeWithoutAsking = defaults.bool(forKey: Keys.Gen.closeWithoutAsking)
    }
    
    func saveGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        // Fullscreen
        defaults.set(keepAspectRatio, forKey: Keys.Gen.keepAspectRatio)
        defaults.set(exitOnEsc, forKey: Keys.Gen.exitOnEsc)
        
        // Snapshots and screenshots
        defaults.set(autoSnapshots, forKey: Keys.Gen.autoSnapshots)
        defaults.set(snapshotInterval, forKey: Keys.Gen.autoSnapshotInterval)
        defaults.set(screenshotSource, forKey: Keys.Gen.screenshotSource)
        defaults.set(screenshotTargetIntValue, forKey: Keys.Gen.screenshotTarget)

        // Warp mode
        defaults.set(warpModeIntValue, forKey: Keys.Gen.warpMode)
        
        // Misc
        defaults.set(driveEjectUnasked, forKey: Keys.Gen.driveEjectUnasked)
        defaults.set(pauseInBackground, forKey: Keys.Gen.pauseInBackground)
        defaults.set(closeWithoutAsking, forKey: Keys.Gen.closeWithoutAsking)
    }
    
    //
    // Controls
    //
    
    func loadControlsUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        // Emulation keys
        defaults.decode(&keyMaps[0], forKey: Keys.Con.mouseKeyMap)
        defaults.decode(&keyMaps[1], forKey: Keys.Con.joyKeyMap1)
        defaults.decode(&keyMaps[2], forKey: Keys.Con.joyKeyMap2)
        disconnectJoyKeys = defaults.bool(forKey: Keys.Con.disconnectJoyKeys)
        
        // Joysticks
        autofire = defaults.bool(forKey: Keys.Con.autofire)
        autofireBullets = defaults.integer(forKey: Keys.Con.autofireBullets)
        autofireFrequency = defaults.float(forKey: Keys.Con.autofireFrequency)
        
        // Mouse
        mouseModelIntValue = defaults.integer(forKey: Keys.Con.mouseModel)
        retainMouseKeyComb = defaults.integer(forKey: Keys.Con.retainMouseKeyComb)
        retainMouseWithKeys = defaults.bool(forKey: Keys.Con.retainMouseWithKeys)
        retainMouseByClick = defaults.bool(forKey: Keys.Con.retainMouseByClick)
        retainMouseByEntering = defaults.bool(forKey: Keys.Con.retainMouseByEntering)
        releaseMouseKeyComb = defaults.integer(forKey: Keys.Con.releaseMouseKeyComb)
        releaseMouseWithKeys = defaults.bool(forKey: Keys.Con.releaseMouseWithKeys)
        releaseMouseByShaking = defaults.bool(forKey: Keys.Con.releaseMouseByShaking)
    }
    
    func saveControlsUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        // Emulation keys
        defaults.encode(keyMaps[0], forKey: Keys.Con.mouseKeyMap)
        defaults.encode(keyMaps[1], forKey: Keys.Con.joyKeyMap1)
        defaults.encode(keyMaps[2], forKey: Keys.Con.joyKeyMap2)
        defaults.set(disconnectJoyKeys, forKey: Keys.Con.disconnectJoyKeys)
        
        // Joysticks
        defaults.set(autofire, forKey: Keys.Con.autofire)
        defaults.set(autofireBullets, forKey: Keys.Con.autofireBullets)
        defaults.set(autofireFrequency, forKey: Keys.Con.autofireFrequency)
        
        // Mouse
        defaults.set(mouseModelIntValue, forKey: Keys.Con.mouseModel)
        defaults.set(retainMouseKeyComb, forKey: Keys.Con.retainMouseKeyComb)
        defaults.set(retainMouseWithKeys, forKey: Keys.Con.retainMouseWithKeys)
        defaults.set(retainMouseByClick, forKey: Keys.Con.retainMouseByClick)
        defaults.set(retainMouseByEntering, forKey: Keys.Con.retainMouseByEntering)
        defaults.set(releaseMouseKeyComb, forKey: Keys.Con.releaseMouseKeyComb)
        defaults.set(releaseMouseWithKeys, forKey: Keys.Con.releaseMouseWithKeys)
        defaults.set(releaseMouseByShaking, forKey: Keys.Con.releaseMouseByShaking)

        track("mouseModel = \(mouseModelIntValue)")
    }
    
    //
    // Keyboard
    //
    
    func loadKeyboardUserDefaults() {
        
        let defaults = UserDefaults.standard
        mapKeysByPosition = defaults.bool(forKey: Keys.Kbd.mapKeysByPosition)
        defaults.decode(&keyMap, forKey: Keys.Kbd.keyMap)
        
        // Remove double mappings (if any)
        var values: [Int] = []
        for (k, v) in keyMap {
            if values.contains(v.nr) {
                track("Removing conflicting key map assignment \(k.keyCode) -> \(v.nr)")
                keyMap[k] = nil
            }
            values.append(v.nr)
        }
    }
    
    func saveKeyboardUserDefaults() {
        
        let defaults = UserDefaults.standard
        defaults.encode(keyMap, forKey: Keys.Kbd.keyMap)
        defaults.set(mapKeysByPosition, forKey: Keys.Kbd.mapKeysByPosition)
    }
}
