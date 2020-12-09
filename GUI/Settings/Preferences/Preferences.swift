// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
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
    
    // Floppy
     var driveBlankDiskFormat = GeneralDefaults.std.driveBlankDiskFormat
     var driveBlankDiskFormatIntValue: Int {
         get { return Int(driveBlankDiskFormat.rawValue) }
         set { driveBlankDiskFormat = FileSystemType.init(newValue) }
     }
     var driveEjectUnasked = GeneralDefaults.std.driveEjectUnasked
     var driveSounds = GeneralDefaults.std.driveSounds
     var driveSoundPan = GeneralDefaults.std.driveSoundPan
     var driveInsertSound = GeneralDefaults.std.driveInsertSound
     var driveEjectSound = GeneralDefaults.std.driveEjectSound
     var driveHeadSound = GeneralDefaults.std.driveHeadSound
     var driveConnectSound = GeneralDefaults.std.driveConnectSound
     
     // Fullscreen
     var keepAspectRatio = GeneralDefaults.std.keepAspectRatio
     var exitOnEsc = GeneralDefaults.std.exitOnEsc
         
     // Snapshots and screenshots
     var autoSnapshots = GeneralDefaults.std.autoSnapshots
     var snapshotInterval = 0 {
         didSet { for c in myAppDelegate.controllers { c.startSnapshotTimer() } }
     }
     var autoScreenshots = GeneralDefaults.std.autoScreenshots

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
     var closeWithoutAsking = GeneralDefaults.std.closeWithoutAsking
     var pauseInBackground = GeneralDefaults.std.pauseInBackground
    
    //
    // Media
    //
    
    var mountAction: [String: AutoMountAction] = MediaDefaults.std.mountAction
    var autoType: [String: Bool] = MediaDefaults.std.autoType
    var autoText: [String: String] = MediaDefaults.std.autoText
    
    //
    // Devices
    //
    
    // Emulation keys
    var keyMaps = [ DevicesDefaults.std.joyKeyMap1,
                    DevicesDefaults.std.joyKeyMap2 ]
    
    // Joystick
    var disconnectJoyKeys = DevicesDefaults.std.disconnectJoyKeys
    var autofire = DevicesDefaults.std.autofire {
        didSet {
            for c64 in myAppDelegate.proxies {
                c64.port1.setAutofire(autofire)
                c64.port2.setAutofire(autofire)
            }
        }
    }
    var autofireBullets = DevicesDefaults.std.autofireBullets {
        didSet {
            for c64 in myAppDelegate.proxies {
                c64.port1.setAutofireBullets(autofireBullets)
                c64.port2.setAutofireBullets(autofireBullets)
            }
        }
    }
    var autofireFrequency = DevicesDefaults.std.autofireFrequency {
        didSet {
            for c64 in myAppDelegate.proxies {
                c64.port1.setAutofireFrequency(autofireFrequency)
                c64.port2.setAutofireFrequency(autofireFrequency)
            }
        }
    }
    
    // Mouse
    var mouseModel = MouseModel.MOUSE1350
    
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
        
        // Floppy
        driveBlankDiskFormatIntValue = defaults.integer(forKey: Keys.Gen.driveBlankDiskFormat)
        driveEjectUnasked = defaults.bool(forKey: Keys.Gen.driveEjectUnasked)
        driveSounds = defaults.bool(forKey: Keys.Gen.driveSounds)
        driveSoundPan = defaults.double(forKey: Keys.Gen.driveSoundPan)
        driveInsertSound = defaults.bool(forKey: Keys.Gen.driveInsertSound)
        driveEjectSound = defaults.bool(forKey: Keys.Gen.driveEjectSound)
        driveHeadSound = defaults.bool(forKey: Keys.Gen.driveHeadSound)
        driveConnectSound = defaults.bool(forKey: Keys.Gen.driveConnectSound)
        
        // Fullscreen
        keepAspectRatio = defaults.bool(forKey: Keys.Gen.keepAspectRatio)
        exitOnEsc = defaults.bool(forKey: Keys.Gen.exitOnEsc)
        
        // Snapshots and screenshots
        autoSnapshots = defaults.bool(forKey: Keys.Gen.autoSnapshots)
        snapshotInterval = defaults.integer(forKey: Keys.Gen.autoSnapshotInterval)
        autoScreenshots = defaults.bool(forKey: Keys.Gen.autoScreenshots)
        screenshotSource = defaults.integer(forKey: Keys.Gen.screenshotSource)
        screenshotTargetIntValue = defaults.integer(forKey: Keys.Gen.screenshotTarget)
        
        // Warp mode
        warpModeIntValue = defaults.integer(forKey: Keys.Gen.warpMode)
        
        // Misc
        pauseInBackground = defaults.bool(forKey: Keys.Gen.pauseInBackground)
        closeWithoutAsking = defaults.bool(forKey: Keys.Gen.closeWithoutAsking)
    }
    
    func saveGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        // Floppy
        defaults.set(screenshotTargetIntValue, forKey: Keys.Gen.screenshotTarget)
        defaults.set(driveEjectUnasked, forKey: Keys.Gen.driveEjectUnasked)
        defaults.set(driveSounds, forKey: Keys.Gen.driveSounds)
        defaults.set(driveSoundPan, forKey: Keys.Gen.driveSoundPan)
        defaults.set(driveInsertSound, forKey: Keys.Gen.driveInsertSound)
        defaults.set(driveEjectSound, forKey: Keys.Gen.driveEjectSound)
        defaults.set(driveHeadSound, forKey: Keys.Gen.driveHeadSound)
        defaults.set(driveConnectSound, forKey: Keys.Gen.driveConnectSound)
        defaults.set(driveBlankDiskFormatIntValue, forKey: Keys.Gen.driveBlankDiskFormat)
        
        // Fullscreen
        defaults.set(keepAspectRatio, forKey: Keys.Gen.keepAspectRatio)
        defaults.set(exitOnEsc, forKey: Keys.Gen.exitOnEsc)
        
        // Snapshots and screenshots
        defaults.set(autoSnapshots, forKey: Keys.Gen.autoSnapshots)
        defaults.set(snapshotInterval, forKey: Keys.Gen.autoSnapshotInterval)
        defaults.set(autoScreenshots, forKey: Keys.Gen.autoScreenshots)
        defaults.set(screenshotSource, forKey: Keys.Gen.screenshotSource)
        
        // Warp mode
        defaults.set(warpModeIntValue, forKey: Keys.Gen.warpMode)
        
        // Misc
        defaults.set(pauseInBackground, forKey: Keys.Gen.pauseInBackground)
        defaults.set(closeWithoutAsking, forKey: Keys.Gen.closeWithoutAsking)
    }
    
    //
    // Devices
    //
        
    func loadDevicesUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        // Emulation keys
        defaults.decode(&keyMaps[0], forKey: Keys.Dev.joyKeyMap1)
        defaults.decode(&keyMaps[1], forKey: Keys.Dev.joyKeyMap2)
        disconnectJoyKeys = defaults.bool(forKey: Keys.Dev.disconnectJoyKeys)
        
        // Joysticks
        autofire = defaults.bool(forKey: Keys.Dev.autofire)
        autofireBullets = defaults.integer(forKey: Keys.Dev.autofireBullets)
        autofireFrequency = defaults.float(forKey: Keys.Dev.autofireFrequency)
    }
    
    func saveDevicesUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        // Emulation keys
        defaults.encode(keyMaps[0], forKey: Keys.Dev.joyKeyMap1)
        defaults.encode(keyMaps[1], forKey: Keys.Dev.joyKeyMap2)
        defaults.set(disconnectJoyKeys, forKey: Keys.Dev.disconnectJoyKeys)
        
        // Joysticks
        defaults.set(autofire, forKey: Keys.Dev.autofire)
        defaults.set(autofireBullets, forKey: Keys.Dev.autofireBullets)
        defaults.set(autofireFrequency, forKey: Keys.Dev.autofireFrequency)
    }
    
    //
    // Keyboard
    //

    func loadKeyboardUserDefaults() {
        
        let defaults = UserDefaults.standard
        mapKeysByPosition = defaults.bool(forKey: Keys.Kbd.mapKeysByPosition)
        defaults.decode(&keyMap, forKey: Keys.Kbd.keyMap)
    }
    
    func saveKeyboardUserDefaults() {
        
        let defaults = UserDefaults.standard
        defaults.encode(keyMap, forKey: Keys.Kbd.keyMap)
        defaults.set(mapKeysByPosition, forKey: Keys.Kbd.mapKeysByPosition)
    }
    
    //
    // Media
    //
    
    func loadMediaUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.decode(&mountAction, forKey: Keys.Med.mountAction)
        defaults.decode(&autoType, forKey: Keys.Med.autoType)
        defaults.decode(&autoText, forKey: Keys.Med.autoText)
    }
    
    func saveMediaUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.encode(mountAction, forKey: Keys.Med.mountAction)
        defaults.encode(autoType, forKey: Keys.Med.autoType)
        defaults.encode(autoText, forKey: Keys.Med.autoText)
    }
}
