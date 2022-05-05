// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* This class stores all emulator settings that belong to the application level.
 * There is a single object of this class stored in the application delegate.
 * The object is shared among all emulator instances.
 *
 * See class "Configuration" for instance specific settings.
 */

class Preferences {
        
    //
    // General
    //

    // Snapshots
    var autoSnapshots = false {
        didSet { for c in myAppDelegate.controllers { c.validateSnapshotTimer() } }
    }
    var snapshotInterval = 0 {
        didSet { for c in myAppDelegate.controllers { c.validateSnapshotTimer() } }
    }

    // Screenshots
    var screenshotSource = 0
    var screenshotTarget = NSBitmapImageRep.FileType.png
    var screenshotTargetIntValue: Int {
        get { return Int(screenshotTarget.rawValue) }
        set { screenshotTarget = NSBitmapImageRep.FileType(rawValue: UInt(newValue))! }
    }
    
    // Screen captures
    var ffmpegPath = "" {
        didSet {
            for amiga in myAppDelegate.proxies {
                amiga.recorder.path = ffmpegPath
            }
        }
    }
    var captureSource = 0
    var bitRate = 512 {
        didSet {
            if bitRate < 64 { bitRate = 64 }
            if bitRate > 16384 { bitRate = 16384 }
        }
    }
    var aspectX = 768 {
        didSet {
            if aspectX < 1 { aspectX = 1 }
            if aspectX > 999 { aspectX = 999 }
        }
    }
    var aspectY = 702 {
        didSet {
            if aspectY < 1 { aspectY = 1 }
            if aspectY > 999 { aspectY = 999 }
        }
    }
    
    // Fullscreen
    var keepAspectRatio = false
    var exitOnEsc = false

    // Warp mode
    var warpMode = WarpMode.off {
        didSet { for c in myAppDelegate.controllers { c.updateWarp() } }
    }
    var warpModeIntValue: Int {
        get { return Int(warpMode.rawValue) }
        set { warpMode = WarpMode(rawValue: newValue)! }
    }
    
    // Misc
    var ejectWithoutAsking = false
    var closeWithoutAsking = false
    var pauseInBackground = false

    //
    // Controls
    //
    
    // Emulation keys
    var keyMaps: [[MacKey: Int]] = [ [:], [:], [:] ]

    // Joystick
    var disconnectJoyKeys = false
    var autofire = false {
        didSet {
            for c64 in myAppDelegate.proxies {
                c64.configure(.AUTOFIRE, enable: autofire)
            }
        }
    }
    var autofireBullets = 0 {
        didSet {
            for c64 in myAppDelegate.proxies {
                c64.configure(.AUTOFIRE_BULLETS, value: autofireBullets)
            }
        }
    }
    var autofireFrequency = 1.0 {
        didSet {
            let autofireDelay = Int(50.0 / autofireFrequency)
            for c64 in myAppDelegate.proxies {
                c64.configure(.AUTOFIRE_DELAY, value: autofireDelay)
            }
        }
    }
    
    // Mouse
    var retainMouseKeyComb = 0
    var retainMouseWithKeys = false
    var retainMouseByClick = false
    var retainMouseByEntering = false
    var releaseMouseKeyComb = 0
    var releaseMouseWithKeys = false
    var releaseMouseByShaking = false

    //
    // Keyboard
    //
    
    // Mapping
    var mapKeysByPosition = false
    var keyMap: [MacKey: C64Key] = [:]
}
