// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MyDocument: NSDocument {
    
    // The window controller for this document
    var parent: MyController { return windowControllers.first as! MyController }

    // The application delegate
    var myAppDelegate: MyAppDelegate { return NSApp.delegate as! MyAppDelegate }
    
    /* Emulator proxy. This object is an Objective-C bridge between the Swift
     * GUI an the core emulator which is written in C++.
     */
    var c64: C64Proxy!
    
    /* An otional media object attached to this document. This variable is
     * checked in mountAttachment() which is called in windowDidLoad(). If an
     * attachment is present, e.g., a D64 archive, it is automatically attached
     * to the emulator.
     */
    var attachment: AnyFileProxy?
    
    // Snapshots
    private(set) var snapshots = ManagedArray<SnapshotProxy>.init(capacity: 32)
    
    // Fingerprint of the first media file used after reset
    var bootDiskID = UInt64(0)
    
    //
    // Initializing
    //
    
    override init() {
        
        track()
        super.init()
        
        // Check for Metal support
        if MTLCreateSystemDefaultDevice() == nil {
            showNoMetalSupportAlert()
            NSApp.terminate(self)
            return
        }
        
        // Register standard user defaults
        UserDefaults.registerUserDefaults()
        
        // Create emulator instance
        c64 = C64Proxy()
    }
 
    deinit {
        
        track()
        c64.kill()
    }
    
    override open func makeWindowControllers() {
        
        track()
        
        let nibName = NSNib.Name("MyDocument")
        let controller = MyController.init(windowNibName: nibName)
        controller.c64 = c64
        self.addWindowController(controller)
    }

    //
    // Creating attachments
    //
    
    func fileType(url: URL) -> FileType {
                
        if url.hasDirectoryPath {
            return .FOLDER
        }
        
        switch url.pathExtension.uppercased() {
            
        case "VC64": return .V64
        case "CRT":  return .CRT
        case "D64":  return .D64
        case "T64":  return .T64
        case "P00":  return .P00
        case "PRG":  return .PRG
        case "G64":  return .G64
        case "TAP":  return .TAP
        default:     return .UNKNOWN
        }
    }
    
    func createAttachment(from url: URL) throws {
        
        let types = [ FileType.V64,
                      FileType.CRT,
                      FileType.T64,
                      FileType.P00,
                      FileType.PRG,
                      FileType.FOLDER,
                      FileType.D64,
                      FileType.G64,
                      FileType.TAP ]
        
        do {
            try createAttachment(from: url, allowedTypes: types)
        } catch let error as MyError {
            let code = error.errorCode.rawValue
            track("CATCHED ERROR \(code)")
            throw error
        }
    }
    
    func createAttachment(from url: URL, allowedTypes: [FileType]) throws {
        
        track("Creating attachment from URL: \(url.lastPathComponent)")
        
        attachment = try createFileProxy(url: url, allowedTypes: allowedTypes)
        myAppDelegate.noteNewRecentlyUsedURL(url)

        track("Attachment created successfully")
    }
        
    fileprivate
    func createFileProxy(url: URL, allowedTypes: [FileType]) throws -> AnyFileProxy? {
        
        track("Creating proxy object from URL: \(url.lastPathComponent)")
        
        // If the provided URL points to compressed file, decompress it first
        let newUrl = url.unpacked

        // Get the file type
        let type = fileType(url: newUrl)

        // Only proceed if the file type is accepted
        if !allowedTypes.contains(type) {
            throw NSError.unsupportedFormatError(filename: url.lastPathComponent)
        }
        
        if url.hasDirectoryPath {
            return try createFileProxy(folderUrl: newUrl, type: type)
        } else {
            return try createFileProxy(fileUrl: newUrl, type: type)
        }
    }
                
    func createFileProxy(fileUrl: URL, type: FileType) throws -> AnyFileProxy? {
            
        track()
        
        // Get the file wrapper and create the proxy with it
        let wrapper = try FileWrapper.init(url: fileUrl)
        return try createFileProxy(wrapper: wrapper, type: type)
    }
    
    fileprivate
    func createFileProxy(wrapper: FileWrapper, type: FileType) throws -> AnyFileProxy? {
                
        guard let name = wrapper.filename else {
            throw NSError.fileAccessError()
        }
        guard let data = wrapper.regularFileContents else {
            throw NSError.fileAccessError(filename: name)
        }
        
        var result: AnyFileProxy?
        let buffer = (data as NSData).bytes
        let length = data.count
        
        track("Read \(length) bytes from file \(name) [\(type.rawValue)].")
        
        switch type {
            
        case .V64:
            if SnapshotProxy.isUnsupportedSnapshot(buffer, length: length) {
                throw NSError.snapshotVersionError(filename: name)
            }
            try result = create(buffer: buffer, length: length) as SnapshotProxy
            
        case .CRT:
            try result = create(buffer: buffer, length: length) as CRTFileProxy
            
        case .D64:
            try result = create(buffer: buffer, length: length) as D64FileProxy
            
        case .T64:
            try result = create(buffer: buffer, length: length) as T64FileProxy
            
        case .PRG:
            try result = create(buffer: buffer, length: length) as PRGFileProxy
            
        case .P00:
            try result = create(buffer: buffer, length: length) as P00FileProxy
            
        case .G64:
            try result = create(buffer: buffer, length: length) as G64FileProxy
            
        case .TAP:
            try result = create(buffer: buffer, length: length) as TAPFileProxy
            
        default:
            fatalError()
        }
        
        result!.setPath(name)
        return result
    }

    fileprivate
    func createFileProxy(folderUrl: URL, type: FileType) throws -> AnyFileProxy? {

        var result: AnyFileProxy?
        
        let name = folderUrl.path
        track("Creating proxy from directory \(name)")
        
        switch type {
            
        case .FOLDER:
            var err = ErrorCode.OK
            result = FolderProxy.make(withFolder: name, error: &err)
            
        default:
            fatalError()
        }
        
        if result == nil {
            throw NSError.corruptedFileError(filename: name)
        }
        result!.setPath(name)
        return result
    }
                
    //
    // Processing attachments
    //
    
    @discardableResult
    func mountAttachment() -> Bool {

        // Only proceed if an attachment is present
        if attachment == nil { return false }

        // If the attachment is a snapshot, flash it and return
        if let s = attachment as? SnapshotProxy { c64.flash(s); return true }

        runMountDialog()
        return true
    }
    
    func runMountDialog() {
        
        let name = NSNib.Name("ImportDialog")
        let controller = ImportDialog.make(parent: parent, nibName: name)
        controller?.showSheet()
    }
    
    @discardableResult
    func mountAttachmentAsDisk(drive id: DriveID) -> Bool {

        if let file = attachment as? D64FileProxy {

            if proceedWithUnexportedDisk(drive: id) {
                c64.drive(id)?.insertD64(file)
                return true
            }
        }
        if let file = attachment as? AnyCollectionProxy {
            
            if proceedWithUnexportedDisk(drive: id) {
                c64.drive(id)?.insertCollection(file)
                return true
            }
        }
        return false
    }
    
    @discardableResult
    func mountAttachmentAsTape() -> Bool {
        
        track()
        if let tape = attachment as? TAPFileProxy {
            
            return parent.c64.datasette.insertTape(tape)
        }
        return false
    }
    
    @discardableResult
    func flashAttachmentIntoMemory() -> Bool {
        
        if let archive = attachment as? AnyCollectionProxy {
            
            return parent.c64.flash(archive, item: 0)
        }
        return false
    }
    
    @discardableResult
    func mountAttachmentAsCartridge() throws -> Bool {
        
        guard let cartridge = attachment as? CRTFileProxy else { return false }
            
            /*
            if cartridge.isSupported {

                parent.c64.expansionport.attachCartridgeAndReset(cartridge)
                return true
            }

             let name = cartridge.name()!
             let type = cartridge.cartridgeType.description
             throw NSError.unsupportedCartridgeError(filename: name, type: type)

             */

        return c64.expansionport.attachCartridgeAndReset(cartridge)
    }

    //
    // Loading
    //
    
    override open func read(from url: URL, ofType typeName: String) throws {
        
        track()
        try createAttachment(from: url)
    }
    
    override open func revert(toContentsOf url: URL, ofType typeName: String) throws {
        
            track()
            try createAttachment(from: url)
            mountAttachment()
    }
    
    //
    // Saving
    //
    
    override open func data(ofType typeName: String) throws -> Data {
        
        track("Trying to write \(typeName) file.")
        
        if typeName == "VC64" {
            
            // Take snapshot
            if let snapshot = SnapshotProxy.make(withC64: c64) {

                // Write to data buffer
                if let data = NSMutableData.init(length: snapshot.sizeOnDisk()) {
                    snapshot.write(toBuffer: data.mutableBytes)
                    return data as Data
                }
            }
        }
        
        throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
    }
    
    //
    // Exporting disks
    //
    
    enum ExportError: Error {
        case invalidFormat(format: FileType)
        case fileSystemError(error: FSError)
        case undecodableDisk
        case other
    }
    
    func export(drive id: DriveID, to url: URL) throws {
        
        track("drive: \(id) to: \(url)")
        
        let disk = c64.drive(id)?.disk
        try export(disk: disk!, to: url)
    }
 
    func export(disk: DiskProxy, to url: URL) throws {

        track("disk: \(disk) to: \(url)")
        
        if url.c64FileType == .G64 {
         
            if let g64 = G64FileProxy.make(withDisk: disk) {
                try export(file: g64, to: url)
            }

        } else {
            
            if let fs = FSDeviceProxy.make(withDisk: disk) {
                try export(fs: fs, to: url)
            } else {
                throw ExportError.undecodableDisk
            }
        }
    }
    
    func export(fs: FSDeviceProxy, to url: URL) throws {
        
        track("fs: \(fs) to: \(url)")

        var err = FSError.OK
                
        switch url.c64FileType {
        
        case .D64:

            if let d64 = D64FileProxy.make(withVolume: fs, error: &err) {
                try export(file: d64, to: url)
            } else {
                throw ExportError.fileSystemError(error: err)
            }
            
        case .T64:
            
            if let t64 = T64FileProxy.make(withFileSystem: fs) {
                try export(file: t64, to: url)
            } else {
                throw ExportError.fileSystemError(error: err)
            }
            
        case .PRG:
            
            if fs.numFiles > 1 {
                showDiskHasMultipleFilesAlert(format: "PRG")
            }
            
            if let prg = PRGFileProxy.make(withFileSystem: fs) {
                try export(file: prg, to: url)
            } else {
                throw ExportError.fileSystemError(error: err)
            }

        case .P00:
            
            if fs.numFiles > 1 {
                showDiskHasMultipleFilesAlert(format: "P00")
            }
            
            if let p00 = P00FileProxy.make(withFileSystem: fs) {
                try export(file: p00, to: url)
            } else {
                throw ExportError.fileSystemError(error: err)
            }

        default:
            throw ExportError.invalidFormat(format: url.c64FileType)
        }
    }
    
    func export(file: AnyFileProxy, to url: URL) throws {
        
        track("file: \(file) to: \(url)")
        
        // Serialize archive
        if let data = NSMutableData.init(length: file.sizeOnDisk()) {
            
            file.write(toBuffer: data.mutableBytes)
            if data.write(to: url, atomically: true) { return }
        }
        
        throw ExportError.other
    }

    //
    // Screenshots
    //
    
    func deleteBootDiskID() {
        
        bootDiskID = 0
    }
    
    @discardableResult
    func setBootDiskID(_ id: UInt64) -> Bool {
        
        track("setBootDiskID(\(id))")
        
        if bootDiskID == 0 {
            track("Assigning new ID")
            bootDiskID = id
            return true
        }
        return false
    }
}
