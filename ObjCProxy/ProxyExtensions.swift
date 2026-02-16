// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

@MainActor
extension C64Proxy {

    func loadSnapshot(_ proxy: MediaFileProxy) throws {

        let exc = ExceptionWrapper()
        loadSnapshot(proxy, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }

    func loadSnapshot(url: URL) throws {

        let exc = ExceptionWrapper()
        loadSnapshot(from: url, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }

    func saveSnapshot(url: URL, compressor: Compressor) throws {

        let exc = ExceptionWrapper()
        saveSnapshot(to: url, compressor: compressor, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }

    func loadWorkspace(url: URL) throws {
        
        let exc = ExceptionWrapper()
        loadWorkspace(url, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }
    
    func saveWorkspace(url: URL) throws {

        let exc = ExceptionWrapper()
        saveWorkspace(url, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }
}

//
// Factory extensions
//

@MainActor
extension MediaFileProxy {

    static func makeWith(buffer: UnsafeRawPointer, length: Int, type: vc64.FileType) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withBuffer: buffer, length: length, type: type, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
        return obj!
    }

    static func make(with data: Data, type: vc64.FileType) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(with: data, type: type, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
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
        if let _ = exc.fault { throw AppError(exc) }
        return obj!
    }

    static func make(with url: URL, type: vc64.FileType) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withFile: url.path, type: type, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
        return obj!
    }

    static func make(with drive: DriveProxy, type: vc64.FileType) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withDrive: drive, type: type, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
        return obj!
    }

    static func make(with fs: FileSystemProxy, type: vc64.FileType) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withFileSystem: fs, type: type, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
        return obj!
    }
}

@MainActor
extension FileSystemProxy {

    static func make(with file: MediaFileProxy) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withMediaFile: file, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
        return obj!
    }

    static func make(with drive: DriveProxy) throws -> Self {

        let exc = ExceptionWrapper()
        let obj = make(withDrive: drive, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
        return obj!
    }
}

//
// Exception passing
//

@MainActor
extension EmulatorProxy {

    func launch() throws {
        
        let exc = ExceptionWrapper()
        launch(exc)
        if let _ = exc.fault { throw AppError(exc) }
    }
    
    func launch(_ listener: UnsafeRawPointer, _ callback: @escaping @convention(c) (UnsafeRawPointer?, Message) -> Void) throws
    {
        let exc = ExceptionWrapper()
        launch(listener, function: callback, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }
    
    func isReady() throws {
        
        let exc = ExceptionWrapper()
        isReady(exc)
        if let _ = exc.fault { throw AppError(exc) }
    }

    func powerOn() throws {
        
        let exc = ExceptionWrapper()
        power(on: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }

    func run() throws {
        
        let exc = ExceptionWrapper()
        run(exc)
        if let _ = exc.fault { throw AppError(exc) }
    }

    func exportConfig(url: URL) throws {

        let exc = ExceptionWrapper()
        exportConfig(url, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }

    func saveRom(_ type: vc64.RomType, url: URL) throws {

        let exc = ExceptionWrapper()
        save(type, url: url, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }
     
    func flash(_ proxy: MediaFileProxy) throws {

        let exc = ExceptionWrapper()
        flash(proxy, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }
    
    func flash(_ proxy: FileSystemProxy, item: Int) throws {

        let exc = ExceptionWrapper()
        flash(proxy, item: item, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }
}

@MainActor
extension ExpansionPortProxy {
    
    func attachCartridge(_ proxy: MediaFileProxy, reset: Bool) throws {
        
        let exc = ExceptionWrapper()
        attachCartridge(proxy, reset: reset, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }
}

@MainActor
extension MediaFileProxy {
    
    func writeToFile(url: URL) throws {
        
        let exc = ExceptionWrapper()
        write(toFile: url.path, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }
}

@MainActor
extension FileSystemProxy {
        
    func export(url: URL) throws {
            
        let exc = ExceptionWrapper()
        export(url.path, exception: exc)
        if let _ = exc.fault { throw AppError(exc) }
    }
}

@MainActor
extension RemoteManagerProxy {

    var icon: NSImage? {

        var numActive = 0
        var numConnected = 0

        func count(_ info: RemoteServerInfo) {

            if info.state != .OFF { numActive += 1 }
            if info.state == .CONNECTED { numConnected += 1 }
        }

        let info = info
        count(info.rshInfo)
        count(info.rpcInfo)
        count(info.dapInfo)
        count(info.promInfo)

        if numConnected > 0 { return SFSymbol.get(.serverConnected) }
        if numActive > 0 { return SFSymbol.get(.serverListening) }

        return nil
    }
}

//
// Keyboard
//

@MainActor
extension KeyboardProxy {

    func isPressed(_ key: C64Key) -> Bool {

        return isPressed(key.nr)
    }
}

//
// Other extensions
//

@MainActor
extension EmulatorProxy {
    
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

@MainActor
extension DriveProxy {
    
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

@MainActor
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

@MainActor
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
    
