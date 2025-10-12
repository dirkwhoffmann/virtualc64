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

    // Fullscreen
    var keepAspectRatio = false
    var exitOnEsc = false
    
    // Misc
    var ejectWithoutAsking = false
    var closeWithoutAsking = false
    var pauseInBackground = false

    //
    // Captures
    //

    // Snapshots
    var snapshotCompressor = Compressor.NONE
    var snapshotCompressorIntValue: Int {
        get { return Int(snapshotCompressor.rawValue) }
        set { snapshotCompressor = Compressor(rawValue: newValue) ?? snapshotCompressor }
    }
    var snapshotAutoDelete: Bool = true

    // Screenshots
    var screenshotFormat = NSBitmapImageRep.FileType.png
    var screenshotFormatIntValue: Int {
        get { return Int(screenshotFormat.rawValue) }
        set { screenshotFormat = NSBitmapImageRep.FileType(rawValue: UInt(newValue)) ?? screenshotFormat }
    }

    var screenshotSource = ScreenshotSource.emulator
    var screenshotSourceIntValue: Int {
        get { return Int(screenshotSource.rawValue) }
        set { screenshotSource = ScreenshotSource(rawValue: newValue) ?? screenshotSource }
    }
    var screenshotCutout = ScreenshotCutout.visible
    var screenshotCutoutIntValue: Int {
        get { return Int(screenshotCutout.rawValue) }
        set { screenshotCutout = ScreenshotCutout(rawValue: newValue) ?? screenshotCutout }
    }
    var screenshotWidth = 1200 {
        didSet {
            screenshotWidth = max(screenshotWidth, 0)
            screenshotWidth = min(screenshotWidth, TextureSize.original.width)
        }
    }
    var screenshotHeight = 900 {
        didSet {
            screenshotHeight = max(screenshotHeight, 0)
            screenshotHeight = min(screenshotHeight, TextureSize.original.height)
        }
    }
    
    //
    // Controls
    //
    
    // Emulation keys
    var keyMaps: [[MacKey: Int]] = [ [:], [:], [:] ]

    // Joystick
    var disconnectJoyKeys = false

    // Mouse
    var retainMouseByClick = false
    var retainMouseByEntering = false
    var releaseMouseByShaking = false

    //
    // Keyboard
    //
    
    // Mapping
    var mapKeysByPosition = false
    var keyMap: [MacKey: C64Key] = [:]
}
