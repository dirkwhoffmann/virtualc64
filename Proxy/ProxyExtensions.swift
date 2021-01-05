// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

//
// Factory extensions
//

func create<T: Makeable>(buffer: UnsafeRawPointer, length: Int) throws -> T {
    
    track()
    
    var err = ErrorCode.OK
    let obj = T.make(withBuffer: buffer, length: length, error: &err)
    track("err = \(err.rawValue)")
    if err != ErrorCode.OK { throw MyError(err) }
    if obj == nil { fatalError() }
    return obj!
}

func create<T: Makeable>(url: URL) throws -> T {
    
    var err = ErrorCode.OK
    let obj = T.make(withFile: url.path, error: &err)
    if err != ErrorCode.OK { throw MyError(err) }
    return obj!
}

//
// Exception passing
//

extension C64Proxy {
    
    func loadRom(type: RomType, url: URL) throws {

        var err = ErrorCode.OK
        loadRom(type, url: url, error: &err)
        if err != .OK { throw MyError(err) }
    }

    func loadRom(type: RomType, data: Data?) throws {

        var err = ErrorCode.OK
        loadRom(type, data: data, error: &err)
        if err != .OK { throw MyError(err) }
    }
}

extension AnyFileProxy {
    
    @discardableResult
    func writeToFile(url: URL) throws -> Int {
        
        var err = ErrorCode.OK
        let result = write(toFile: url.path, error: &err)
        if err != .OK { throw MyError(err) }
        
        return result
    }
}

//
// Informational extensions
//

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
    
