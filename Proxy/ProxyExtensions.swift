// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

//
// Factory extensions
//

extension Proxy {
    
    static func make<T: MakeWithBuffer>(buffer: UnsafeRawPointer, length: Int) throws -> T {
        
        track()
        
        let exception = ExceptionWrapper()
        let obj = T.make(withBuffer: buffer, length: length, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
        return obj!
    }

    static func make<T: MakeWithFile>(url: URL) throws -> T {
        
        let exception = ExceptionWrapper()
        let obj = T.make(withFile: url.path, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
        return obj!
    }
    
    static func make<T: MakeWithDisk>(disk: DiskProxy) throws -> T {
        
        let exception = ExceptionWrapper()
        let obj = T.make(withDisk: disk, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
        return obj!
    }
    
    static func make<T: MakeWithFileSystem>(fs: FileSystemProxy) throws -> T {
        
        let exception = ExceptionWrapper()
        let obj = T.make(withFileSystem: fs, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
        return obj!
    }
    
    static func make<T: MakeWithCollection>(collection: AnyCollectionProxy) throws -> T {
        
        let exception = ExceptionWrapper()
        let obj = T.make(withCollection: collection, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
        return obj!
    }
    
    static func make<T: MakeWithD64>(d64: D64FileProxy) throws -> T {

        let exception = ExceptionWrapper()
        let obj = T.make(withD64: d64, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
        return obj!
    }
    
    static func make<T: MakeWithFolder>(folder: URL) throws -> T {
        
        let exception = ExceptionWrapper()
        let obj = T.make(withFolder: folder.path, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
        return obj!
    }
}

//
// Exception passing
//

extension C64Proxy {

    func isReady() throws {
        
        let exception = ExceptionWrapper()
        isReady(exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }

    func powerOn() throws {
        
        let exception = ExceptionWrapper()
        power(on: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }

    func run() throws {
        
        let exception = ExceptionWrapper()
        run(exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }

    func saveRom(_ type: RomType, url: URL) throws {

        let exception = ExceptionWrapper()
        saveRom(type, url: url, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }
     
    func flash(_ proxy: AnyFileProxy) throws {

        let exception = ExceptionWrapper()
        flash(proxy, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }
    
    func flash(_ proxy: FileSystemProxy, item: Int) throws {

        let exception = ExceptionWrapper()
        flash(proxy, item: item, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }
}
 
extension ExpansionPortProxy {
    
    func attachCartridge(_ proxy: CRTFileProxy, reset: Bool) throws {
        
        let exception = ExceptionWrapper()
        attachCartridge(proxy, reset: reset, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }
}

extension AnyFileProxy {
    
    func writeToFile(url: URL) throws {
        
        let exception = ExceptionWrapper()
        write(toFile: url.path, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }
}

extension FileSystemProxy {
        
    func export(url: URL) throws {
            
        let exception = ExceptionWrapper()
        export(url.path, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }
}

extension RecorderProxy {

    func startRecording(rect: NSRect, rate: Int, ax: Int, ay: Int) throws {

        let exception = ExceptionWrapper()
        startRecording(rect, bitRate: rate, aspectX: ax, aspectY: ay, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }
}

//
// Other extensions
//

public extension C64Proxy {
    
    func drive(_ nr: DriveID) -> DriveProxy {
        
        switch nr {
            
        case .DRIVE8: return drive8
        case .DRIVE9: return drive9
        
        default:
            fatalError()
        }
    }
    
    func drive(_ item: NSButton!) -> DriveProxy {
        
        return drive(DriveID(rawValue: item.tag)!)
    }
    
    func drive(_ item: NSMenuItem!) -> DriveProxy {
        
        return drive(DriveID(rawValue: item.tag)!)
    }
    
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
                                        bytesPerRow: 4 * width,
                                        bitsPerPixel: 32)
        
        let image = NSImage(size: (imageRep?.size)!)
        image.addRepresentation(imageRep!)
        image.makeGlossy()
        
        return image
    }
}

public extension DriveProxy {
    
    static let ledGray = NSImage(named: "LEDgray")!
    static let ledGreen = NSImage(named: "LEDgreen")!
    static let ledRed = NSImage(named: "LEDred")!
    static let diskSaved = NSImage(named: "diskTemplate")!
    static let diskUnsaved = NSImage(named: "diskUnsavedTemplate")!
        
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
     
        switch type {
     
        case .CRT:
            return NSImage(named: "cartridge")!
        
        case .TAP:
            return NSImage(named: "tape")!
            
        case .FOLDER:
            return NSImage(named: "NSFolder")!

        case .D64, .G64, .T64, .PRG, .P00:
            return AnyFileProxy.diskIcon(protected: protected)
            
        default:
            fatalError()
        }
    }
    
    static func diskIcon(protected: Bool = false) -> NSImage {

        let name = "disk2" + (protected ? "_protected" : "")
        return NSImage(named: name)!
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

extension FileSystemProxy {

    static func make(withDisk disk: DiskProxy) throws -> FileSystemProxy {

        let exception = ExceptionWrapper()
        let result = FileSystemProxy.make(withDisk: disk, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }

        return result!
    }

    func icon(protected: Bool) -> NSImage {
                        
        let name = "disk2" + (protected ? "_protected" : "")
        return NSImage(named: name)!
    }

    var layoutInfo: String {

        return "Single sided, single density disk with \(numTracks) tracks"
    }
    
    var filesInfo: String {
        
        let num = numFiles
        let files = num == 1 ? "file" : "files"
        
        return "\(num) \(files), \(usedBlocks) blocks used"
    }
}
    
