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
    
    var driveSounds = GeneralDefaults.std.driveSounds
    var driveConnectSound = GeneralDefaults.std.driveConnectSound
    var driveInsertSound = GeneralDefaults.std.driveInsertSound
    var driveEjectSound = GeneralDefaults.std.driveEjectSound
    var driveHeadSound = GeneralDefaults.std.driveHeadSound
}
