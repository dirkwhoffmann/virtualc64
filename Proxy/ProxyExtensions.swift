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

extension MediaFileProxy {

    static func makeWith(buffer: UnsafeRawPointer, length: Int, type: vc64.FileType) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withBuffer: buffer, length: length, type: type, exception: exc)
        if exc.errorCode != .OK { throw VC64Error(exc) }
        return obj!
    }

    static func make(with data: Data, type: vc64.FileType) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(with: data, type: type, exception: exc)
        if exc.errorCode != .OK { throw VC64Error(exc) }
        return obj!
    }

    private static func make(with data: Data, type: vc64.FileType, exception: ExceptionWrapper) -> Self? {

        return data.withUnsafeBytes { uwbp -> Self? in

            return make(withBuffer: uwbp.baseAddress!, length: uwbp.count, type: type, exception: exception)
        }
    }

    static func make(with url: URL) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withFile: url.path, exception: exc)
        if exc.errorCode != .OK { throw VC64Error(exc) }
        return obj!
    }

    static func make(with url: URL, type: vc64.FileType) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withFile: url.path, type: type, exception: exc)
        if exc.errorCode != .OK { throw VC64Error(exc) }
        return obj!
    }

    static func make(with drive: DriveProxy, type: vc64.FileType) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withDrive: drive, type: type, exception: exc)
        if exc.errorCode != .OK { throw VC64Error(exc) }
        return obj!
    }

    static func make(with fs: FileSystemProxy, type: vc64.FileType) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withFileSystem: fs, type: type, exception: exc)
        if exc.errorCode != .OK { throw VC64Error(exc) }
        return obj!
    }
}

extension FileSystemProxy {

    static func make(with file: MediaFileProxy) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withMediaFile: file, exception: exc)
        if exc.errorCode != .OK { throw VC64Error(exc) }
        return obj!
    }

    static func make(with drive: DriveProxy) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withDrive: drive, exception: exc)
        if exc.errorCode != .OK { throw VC64Error(exc) }
        return obj!
    }
}

//
// Exception passing
//

extension EmulatorProxy {

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

    func exportConfig(url: URL) throws {

        let exception = ExceptionWrapper()
        exportConfig(url, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }

    func saveRom(_ type: vc64.RomType, url: URL) throws {

        let exception = ExceptionWrapper()
        saveRom(type, url: url, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }
     
    func flash(_ proxy: MediaFileProxy) throws {

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
    
    func attachCartridge(_ proxy: MediaFileProxy, reset: Bool) throws {
        
        let exception = ExceptionWrapper()
        attachCartridge(proxy, reset: reset, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }
}

extension MediaFileProxy {
    
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

    func startRecording(rect: NSRect) throws {

        let exception = ExceptionWrapper()
        startRecording(rect, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }
}

extension RemoteManagerProxy {

    var icon: NSImage? {

        let info = info

        if info.numConnected > 0 {
            return NSImage(named: "srvConnectTemplate")!
        }
        if info.numListening > 0 {
            return NSImage(named: "srvListenTemplate")!
        }
        if info.numLaunching > 0 {
            return NSImage(named: "srvLaunchTemplate")!
        }
        if info.numErroneous > 0 {
            return NSImage(named: "srvErrorTemplate")!
        }

        return nil
    }
}

//
// Keyboard
//

public extension KeyboardProxy {

    func isPressed(_ key: C64Key) -> Bool {

        return isPressed(key.nr)
    }
}

//
// Other extensions
//

public extension EmulatorProxy {
    
    func drive(_ nr: NSInteger) -> DriveProxy {
        
        switch nr {
            
        case DRIVE8: return drive8
        case DRIVE9: return drive9
        
        default:
            fatalError()
        }
    }
    
    func drive(_ item: NSButton!) -> DriveProxy {
        
        return drive(item.tag)
    }
    
    func drive(_ item: NSMenuItem!) -> DriveProxy {
        
        return drive(item.tag)
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
    static let diskProt = NSImage(named: "diskTemplate")!
    static let diskProtGray = NSImage(named: "diskGrayTemplate")!
    static let diskUnprot = NSImage(named: "diskWritableTemplate")!
    static let diskUnprotGray = NSImage(named: "diskWritableGrayTemplate")!

    var greenLedImage: NSImage {        
        return config.switchedOn ? DriveProxy.ledGreen : DriveProxy.ledGray
    }
    
    var redLedImage: NSImage {
        return info.redLED ? DriveProxy.ledRed : DriveProxy.ledGray
    }

    var icon: NSImage {
        if info.hasProtectedDisk {
            return info.hasModifiedDisk ? DriveProxy.diskProtGray : DriveProxy.diskProt
        } else {
            return info.hasModifiedDisk ? DriveProxy.diskUnprotGray : DriveProxy.diskUnprot
        }
    }
}

extension MediaFileProxy {

    func icon(protected: Bool = false) -> NSImage {
     
        switch type {
     
        case .CRT:
            return NSImage(named: "cartridge")!
        
        case .TAP:
            return NSImage(named: "tape")!
            
        case .FOLDER:
            return NSImage(named: "NSFolder")!

        case .D64, .G64, .T64, .PRG, .P00:
            return MediaFileProxy.diskIcon(protected: protected)

        default:
            fatalError()
        }
    }
    
    static func diskIcon(protected: Bool = false) -> NSImage {

        let name = "disk2" + (protected ? "_protected" : "")
        return NSImage(named: name)!
    }
}

extension FileSystemProxy {

    func icon(protected: Bool) -> NSImage {
                        
        let name = "disk2" + (protected ? "_protected" : "")
        return NSImage(named: name)!
    }

    var layoutInfo: String {

        return "Single sided, single density disk with \(numTracks) tracks"
    }

    var dosInfo: String {

        return dos.description
    }

    var usageInfo: String {

        return "\(numBlocks) blocks, \(usedBlocks) blocks used"
    }

    var filesInfo: String {
        
        let num = numFiles
        let files = num == 1 ? "file" : "files"
        return "\(num) \(files)"
    }
}
    
