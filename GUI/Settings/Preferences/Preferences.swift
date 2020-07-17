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
    
    // Floppy
     var driveBlankDiskFormat = GeneralDefaults.std.driveBlankDiskFormat
     var driveBlankDiskFormatIntValue: Int {
         get { return Int(driveBlankDiskFormat.rawValue) }
         set { driveBlankDiskFormat = FileSystemType.init(newValue) }
     }
     var ejectWithoutAsking = GeneralDefaults.std.driveEjectUnasked
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
}
