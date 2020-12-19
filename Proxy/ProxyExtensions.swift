// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

public extension C64Proxy {
    
    func image(data: UnsafeMutablePointer<UInt8>?, size: NSSize) -> NSImage {
        
        var bitmap = data
        let width = Int(size.width)
        let height = Int(size.height)
        let imageRep = NSBitmapImageRep(bitmapDataPlanes: &bitmap,
                                        pixelsWide: width,
                                        pixelsHigh: height,
                                        bitsPerSample: 8,
                                        samplesPerPixel: 4,
                                        hasAlpha: true,
                                        isPlanar: false,
                                        colorSpaceName: NSColorSpaceName.calibratedRGB,
                                        bytesPerRow: 4*width,
                                        bitsPerPixel: 32)
        let image = NSImage(size: (imageRep?.size)!)
        image.addRepresentation(imageRep!)
        image.makeGlossy()
        
        return image
    }
}

public extension DriveProxy {
    
    static let ledGray = NSImage.init(named: "LEDgray")!
    static let ledGreen = NSImage.init(named: "LEDgreen")!
    static let ledRed = NSImage.init(named: "LEDred")!
    static let diskSaved = NSImage.init(named: "diskTemplate")!
    static let diskUnsaved = NSImage.init(named: "diskUnsavedTemplate")!
        
    var greenLedImage: NSImage {        
        return isSwitchedOn() ? DriveProxy.ledGreen : DriveProxy.ledGray
    }
    
    var redLedImage: NSImage {
        return redLED() ? DriveProxy.ledRed : DriveProxy.ledGray
    }

    var icon: NSImage {
        return hasModifiedDisk() ? DriveProxy.diskUnsaved : DriveProxy.diskSaved
    }
}

extension D64FileProxy {
       
    func icon(protected: Bool) -> NSImage {
                        
        var name = "disk"
        if protected { name += "_protected" }
        
        return NSImage.init(named: name)!
    }
    
    var layoutInfo: String {

        let numTracks = numberOfTracks()
        return "Single sided, single density, \(numTracks) tracks"
    }
}
