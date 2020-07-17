// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Foundation

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
    
    var autoMountAction: [String: AutoMountAction] = Defaults.autoMountAction
    var autoType: [String: Bool] = Defaults.autoType
    var autoTypeText: [String: String] = Defaults.autoTypeText
    
    //
    // General
    //
    
    func loadGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        // Floppy
        driveBlankDiskFormatIntValue = defaults.integer(forKey: Keys.driveBlankDiskFormat)
        driveEjectUnasked = defaults.bool(forKey: Keys.driveEjectUnasked)
        driveSounds = defaults.bool(forKey: Keys.driveSounds)
        driveSoundPan = defaults.double(forKey: Keys.driveSoundPan)
        driveInsertSound = defaults.bool(forKey: Keys.driveInsertSound)
        driveEjectSound = defaults.bool(forKey: Keys.driveEjectSound)
        driveHeadSound = defaults.bool(forKey: Keys.driveHeadSound)
        driveConnectSound = defaults.bool(forKey: Keys.driveConnectSound)
        
        // Fullscreen
        keepAspectRatio = defaults.bool(forKey: Keys.keepAspectRatio)
        exitOnEsc = defaults.bool(forKey: Keys.exitOnEsc)
        
        // Snapshots and screenshots
        autoSnapshots = defaults.bool(forKey: Keys.autoSnapshots)
        snapshotInterval = defaults.integer(forKey: Keys.autoSnapshotInterval)
        autoScreenshots = defaults.bool(forKey: Keys.autoScreenshots)
        screenshotSource = defaults.integer(forKey: Keys.screenshotSource)
        screenshotTargetIntValue = defaults.integer(forKey: Keys.screenshotTarget)
        
        // Warp mode
        warpModeIntValue = defaults.integer(forKey: Keys.warpMode)
        
        // Misc
        pauseInBackground = defaults.bool(forKey: Keys.pauseInBackground)
        closeWithoutAsking = defaults.bool(forKey: Keys.closeWithoutAsking)
    }
    
    func saveGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        // Floppy
        defaults.set(screenshotTargetIntValue, forKey: Keys.screenshotTarget)
        defaults.set(driveEjectUnasked, forKey: Keys.driveEjectUnasked)
        defaults.set(driveSounds, forKey: Keys.driveSounds)
        defaults.set(driveSoundPan, forKey: Keys.driveSoundPan)
        defaults.set(driveInsertSound, forKey: Keys.driveInsertSound)
        defaults.set(driveEjectSound, forKey: Keys.driveEjectSound)
        defaults.set(driveHeadSound, forKey: Keys.driveHeadSound)
        defaults.set(driveConnectSound, forKey: Keys.driveConnectSound)
        defaults.set(driveBlankDiskFormatIntValue, forKey: Keys.driveBlankDiskFormat)
        
        // Fullscreen
        defaults.set(keepAspectRatio, forKey: Keys.keepAspectRatio)
        defaults.set(exitOnEsc, forKey: Keys.exitOnEsc)
        
        // Snapshots and screenshots
        defaults.set(autoSnapshots, forKey: Keys.autoSnapshots)
        defaults.set(snapshotInterval, forKey: Keys.autoSnapshotInterval)
        defaults.set(autoScreenshots, forKey: Keys.autoScreenshots)
        defaults.set(screenshotSource, forKey: Keys.screenshotSource)
        
        // Warp mode
        defaults.set(warpModeIntValue, forKey: Keys.warpMode)
        
        // Misc
        defaults.set(pauseInBackground, forKey: Keys.pauseInBackground)
        defaults.set(closeWithoutAsking, forKey: Keys.closeWithoutAsking)
    }
    
    //
    // Media
    //
    
    func loadMediaUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.decode(&autoMountAction, forKey: Keys.autoMountAction)
        defaults.decode(&autoType, forKey: Keys.autoType)
        defaults.decode(&autoTypeText, forKey: Keys.autoTypeText)
    }
    
    func saveMediaUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.encode(autoMountAction, forKey: Keys.autoMountAction)
        defaults.encode(autoType, forKey: Keys.autoType)
        defaults.encode(autoTypeText, forKey: Keys.autoTypeText)
    }
}
