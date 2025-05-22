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
        didSet {
            for emu in myAppDelegate.proxies {
                emu?.set(.C64_SNAP_AUTO, enable: autoSnapshots)
            }
        }
    }
    var snapshotInterval = 0 {
        didSet {
            for emu in myAppDelegate.proxies {
                emu?.set(.C64_SNAP_DELAY, value: snapshotInterval)
            }
        }
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
            for emu in myAppDelegate.proxies {
                emu?.recorder.path = ffmpegPath
            }
        }
    }
    var captureSource = 0

    var bitRate = 512 {
        didSet {
            for emu in myAppDelegate.proxies {
                emu?.set(.REC_BIT_RATE, value: bitRate)
            }
        }
    }
    var aspectX = 768 {
        didSet {
            for emu in myAppDelegate.proxies {
                emu?.set(.REC_ASPECT_X, value: aspectX)
            }
        }
    }
    var aspectY = 702 {
        didSet {
            for emu in myAppDelegate.proxies {
                emu?.set(.REC_ASPECT_Y, value: aspectY)
            }
        }
    }
    
    // Fullscreen
    var keepAspectRatio = false
    var exitOnEsc = false
    
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
