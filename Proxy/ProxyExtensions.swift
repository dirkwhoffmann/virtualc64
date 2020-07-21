// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

public extension DriveProxy {
    
    static let ledGray = NSImage.init(named: "LEDgray")!
    static let ledGreen = NSImage.init(named: "LEDgreen")!
    static let ledRed = NSImage.init(named: "LEDred")!
    static let diskSaved = NSImage.init(named: "diskTemplate")!
    static let diskUnsaved = NSImage.init(named: "diskUnsavedTemplate")!
        
    var greenLedImage: NSImage {
        return isConnected() ? DriveProxy.ledGreen : DriveProxy.ledGray
    }
    
    var redLedImage: NSImage {
        return redLED() ? DriveProxy.ledRed : DriveProxy.ledGray
    }

    var icon: NSImage {
        return hasModifiedDisk() ? DriveProxy.diskUnsaved : DriveProxy.diskSaved
    }
}
