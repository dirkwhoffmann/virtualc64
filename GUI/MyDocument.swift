// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
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
    private(set) var snapshots = ManagedArray<SnapshotProxy>(capacity: 32)
    
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
     
    override open func makeWindowControllers() {
        
        track()
        
        let nibName = NSNib.Name("MyDocument")
        let controller = MyController(windowNibName: nibName)
        controller.c64 = c64
        self.addWindowController(controller)
    }

    //
    // Creating attachments
    //
    
    /*
    func fileType(url: URL) -> FileType {
                
        if url.hasDirectoryPath {
            return .FOLDER
        }
        
        switch url.pathExtension.uppercased() {
            
        case "VC64": return .V64
        case "INI":  return .SCRIPT
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
    */
    
    func createAttachment(from url: URL) throws {
        
        let types: [FileType] =
            [ .V64, .SCRIPT, .CRT, .T64, .P00, .PRG, .FOLDER, .D64, .G64, .TAP ]
        
        try createAttachment(from: url, allowedTypes: types)
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
        let type = AnyFileProxy.type(of: newUrl)

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
        let wrapper = try FileWrapper(url: fileUrl)
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
            try result = Proxy.make(buffer: buffer, length: length) as SnapshotProxy
            
        case .SCRIPT:
            try? result = Proxy.make(buffer: buffer, length: length) as ScriptProxy

        case .CRT:
            try result = Proxy.make(buffer: buffer, length: length) as CRTFileProxy
            
        case .D64:
            try result = Proxy.make(buffer: buffer, length: length) as D64FileProxy
            
        case .T64:
            try result = Proxy.make(buffer: buffer, length: length) as T64FileProxy
            
        case .PRG:
            try result = Proxy.make(buffer: buffer, length: length) as PRGFileProxy
            
        case .P00:
            try result = Proxy.make(buffer: buffer, length: length) as P00FileProxy
            
        case .G64:
            try result = Proxy.make(buffer: buffer, length: length) as G64FileProxy
            
        case .TAP:
            try result = Proxy.make(buffer: buffer, length: length) as TAPFileProxy
            
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
    func mountAttachment(destination: DriveProxy? = nil) -> Bool {

        // Only proceed if an attachment is present
        if attachment == nil { return false }

        // If the attachment is a snapshot, flash it and return
        if let proxy = attachment as? SnapshotProxy {
            c64.flash(proxy)
            return true
        }

        // If the attachment is a script, execute it
        if let proxy = attachment as? ScriptProxy {
            parent.renderer.console.runScript(script: proxy)
            return true
        }

        // Try to insert the attachment as a disk
        if let id = destination?.id {
            if mountAttachmentAsDisk(drive: id) {
                return true
            }
        }
        
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

        let drive = c64.drive(id)!
        
        if let file = attachment as? D64FileProxy {

            if proceedWithUnexportedDisk(drive: id) {
                if let fs = try? Proxy.make(d64: file) as FSDeviceProxy {
                    drive.insertFileSystem(fs, protected: false)
                    return true
                }
            }
        }
        if let file = attachment as? G64FileProxy {
            
            if proceedWithUnexportedDisk(drive: id) {
                drive.insertG64(file, protected: false)
                return true
            }
        }
        if let file = attachment as? AnyCollectionProxy {
            
            if proceedWithUnexportedDisk(drive: id) {
                if let fs = try? Proxy.make(collection: file) as FSDeviceProxy {
                    drive.insertFileSystem(fs, protected: false)
                    return true
                }
            }
        }
        return false
    }
    
    @discardableResult
    func mountAttachmentAsTape() -> Bool {
        
        track()
        
        if let tape = attachment as? TAPFileProxy {
            parent.c64.datasette.insertTape(tape)
        }
        return true
    }
        
    @discardableResult
    func mountAttachmentAsCartridge() -> Bool {
        
        guard let cartridge = attachment as? CRTFileProxy else { return false }
        return c64.expansionport.attachCartridge(cartridge, reset: true)
    }

    //
    // Loading
    //
    
    override open func read(from url: URL, ofType typeName: String) throws {
        
        track()
        
        do {
            try createAttachment(from: url)
        } catch let error as VC64Error {
            error.cantOpen(url: url)
        }
    }
    
    override open func revert(toContentsOf url: URL, ofType typeName: String) throws {
        
        track()
        
        do {
            try createAttachment(from: url)
            mountAttachment()
        } catch let error as VC64Error {
            error.cantOpen(url: url)
        }
    }
    
    //
    // Saving
    //
    
    override func write(to url: URL, ofType typeName: String) throws {
            
        track()
        
        if typeName == "VC64" {
            
            // Take snapshot
            if let snapshot = SnapshotProxy.make(withC64: c64) {

                // Write to data buffer
                do {
                    _ = try snapshot.writeToFile(url: url)
                } catch {
                    throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
                }
            }
        }
    }

    //
    // Exporting disks
    //
        
    func export(drive id: DriveID, to url: URL) throws {
        
        track("drive: \(id.rawValue) to: \(url)")
        
        let drive = c64.drive(id)
        let disk = c64.drive(id).disk
        try export(disk: disk!, to: url)
        
        drive?.setModifiedDisk(false)
    }
 
    func export(disk: DiskProxy, to url: URL) throws {

        track("disk: \(disk) to: \(url)")
        
        if url.c64FileType == .G64 {
         
            let g64 = try Proxy.make(disk: disk) as G64FileProxy
            try export(file: g64, to: url)

        } else {
            
            let fs = try Proxy.make(disk: disk) as FSDeviceProxy
            try export(fs: fs, to: url)
        }
    }
    
    func export(fs: FSDeviceProxy, to url: URL) throws {

        func showMultipleFilesAlert(format: String) {

            let msg1 = "Only the first file will be exported."
            let msg2 = "The \(format) format is designed to store a single file."
            
            VC64Error.informational(msg1, msg2)
        }
        
        track("fs: \(fs) to: \(url)")

        var file: AnyFileProxy?
                
        switch url.c64FileType {
        
        case .D64:
            file = try Proxy.make(fs: fs) as D64FileProxy
            
        case .T64:
            file = try Proxy.make(fs: fs) as T64FileProxy
            
        case .PRG:
            if fs.numFiles > 1 { showMultipleFilesAlert(format: "PRG") }
            file = try Proxy.make(fs: fs) as PRGFileProxy
            
        case .P00:
            if fs.numFiles > 1 { showMultipleFilesAlert(format: "P00") }
            file = try Proxy.make(fs: fs) as P00FileProxy

        default:
            throw VC64Error(ErrorCode.FILE_TYPE_MISMATCH)
        }
        
        try export(file: file!, to: url)
    }
    
    func export(file: AnyFileProxy, to url: URL) throws {
        
        try file.writeToFile(url: url)
    }

    //
    // Screenshots
    //
    
    func deleteBootDiskID() {
        
        bootDiskID = 0
    }
    
    @discardableResult
    func setBootDiskID(_ id: UInt64) -> Bool {
                
        if bootDiskID == 0 {
            
            bootDiskID = id
            return true
        }
        return false
    }
}
