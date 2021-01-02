// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MyError: Error {
    
    var errorCode: ErrorCode
    
    init(_ errorCode: ErrorCode) { self.errorCode = errorCode }
}

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

extension AnyFileProxy {
        
    func icon(protected: Bool = false) -> NSImage {
     
        switch type() {
     
        case .CRT:
            return NSImage.init(named: "cartridge")!
        
        case .TAP:
            return NSImage.init(named: "tape")!
            
        case .FOLDER:
            return NSImage.init(named: "NSFolder")!

        case .D64, .G64, .T64, .PRG, .P00:
            
            let name = "disk2" + (protected ? "_protected" : "")
            return NSImage.init(named: name)!
            
        default:
            fatalError()
        }
    }
}

extension D64FileProxy {
       
    var layoutInfo: String {

        return "Single sided, single density disk with \(numTracks) tracks"
    }
}

extension CRTFileProxy {
    
    var packageInfo: String {
        
        let cnt = chipCount
        let type = cartridgeType
        let packages = cnt == 1 ? "package" : "packages"

        if type == .NORMAL {
            return "Standard cartridge with \(cnt) chip \(packages)"
        } else {
            return "\(type.description)"
        }
    }
    
    var lineInfo: String {
                
        let exrom = initialExromLine
        let game = initialGameLine
        
        var result = ""
        
        switch (exrom, game) {
        case (0, 0): result += "16K Cartridge Mode"
        case (0, 1): result += "8K Cartridge Mode"
        case (1, 0): result += "Ultimax Cartridge Mode"
        case (1, 1): result += "Disabled Cartridge"
        default: fatalError()
        }

        result += " (Exrom: \(exrom), " + "Game: \(game))"
        return result
    }
}

extension PRGFileProxy {

    static func make(withBuffer buffer: UnsafeRawPointer, length: Int) throws -> PRGFileProxy? {
        
        var err = ErrorCode.OK
        let prg = PRGFileProxy.make(withBuffer: buffer, length: length, error: &err)

        if err != ErrorCode.OK { throw MyError(err) }        
        return prg
    }
}

extension FSDeviceProxy {
    
    func icon(protected: Bool) -> NSImage {
                        
        let name = "disk2" + (protected ? "_protected" : "")
        return NSImage.init(named: name)!
    }

    var layoutInfo: String {

        return "Single sided, single density disk with \(numTracks) tracks"
    }
    
    var filesInfo: String {
        
        let num = numFiles
        let files = num == 1 ? "file" : "files"
        
        return "\(num) \(files), \(numUsedBlocks) blocks used"
    }
}
    
