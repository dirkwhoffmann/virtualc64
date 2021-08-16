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

    // Gateway to the core emulator
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
        
        super.init()
        
        // Check for Metal support
        if MTLCreateSystemDefaultDevice() == nil {
            showNoMetalSupportAlert()
            NSApp.terminate(self)
            return
        }
        
        // Register standard user defaults
        UserDefaults.registerUserDefaults()
        
        // Create an emulator instance
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
        
    func createAttachment(from url: URL) throws {
        
        let types: [FileType] =
            [ .SNAPSHOT, .SCRIPT, .CRT, .T64, .P00, .PRG, .FOLDER, .D64, .G64, .TAP ]
        
        try createAttachment(from: url, allowedTypes: types)
    }
    
    func createAttachment(from url: URL, allowedTypes: [FileType]) throws {
                
        attachment = try createFileProxy(url: url, allowedTypes: allowedTypes)
        myAppDelegate.noteNewRecentlyUsedURL(url)
        
        track("Attachment created successfully")
    }
        
    fileprivate
    func createFileProxy(url: URL, allowedTypes: [FileType]) throws -> AnyFileProxy? {
        
        track("Creating proxy object from URL: \(url.lastPathComponent)")

        // If the provided URL points to compressed file, decompress it first
        let newUrl = url.unpacked

        // Iterate through all allowed file types
        for type in allowedTypes {
                        
            do {
                switch type {
                
                case .SNAPSHOT:
                    return try Proxy.make(url: newUrl) as SnapshotProxy
                    
                case .SCRIPT:
                    return try Proxy.make(url: newUrl) as ScriptProxy
                    
                case .CRT:
                    return try Proxy.make(url: newUrl) as CRTFileProxy
                    
                case .D64:
                    return try Proxy.make(url: newUrl) as D64FileProxy
                    
                case .T64:
                    return try Proxy.make(url: newUrl) as T64FileProxy
                    
                case .PRG:
                    return try Proxy.make(url: newUrl) as PRGFileProxy
                    
                case .P00:
                    return try Proxy.make(url: newUrl) as P00FileProxy
                    
                case .G64:
                    return try Proxy.make(url: newUrl) as G64FileProxy
                    
                case .TAP:
                    return try Proxy.make(url: newUrl) as TAPFileProxy
                    
                case .FOLDER:
                    return try Proxy.make(folder: newUrl) as FolderProxy
                    
                default:
                    fatalError()
                }
                
            } catch let error as VC64Error {
                if error.errorCode != .FILE_TYPE_MISMATCH {
                    throw error
                }
            }
        }

        // None of the allowed typed matched the file
        throw VC64Error(.FILE_TYPE_MISMATCH, url.lastPathComponent)
    }

    func mountAttachment() throws {

        // Only proceed if an attachment is present
        if attachment == nil { return }
        
        if let proxy = attachment as? SnapshotProxy {
            try c64.flash(proxy)
            snapshots.append(proxy)
            return
        }
        if let proxy = attachment as? ScriptProxy {
            parent.renderer.console.runScript(script: proxy)
            return
        }
        if let proxy = attachment as? CRTFileProxy {
            c64.expansionport.attachCartridge(proxy, reset: true)
            return
        }
        if let proxy = attachment as? TAPFileProxy {
            c64.datasette.insertTape(proxy)
            return
        }
        
        // Try to insert the attachment as a disk in drive 8
        try mountAttachment(drive: .DRIVE8)
    }
        
    func mountAttachment(drive id: DriveID) throws {

        let drive = c64.drive(id)
        
        if let proxy = attachment as? D64FileProxy {

            if proceedWithUnexportedDisk(drive: id) {
                
                if let fs = try? Proxy.make(d64: proxy) as FSDeviceProxy {
                    drive.insertFileSystem(fs, protected: false)
                    return
                }
            }
        }
        if let proxy = attachment as? G64FileProxy {
            
            if proceedWithUnexportedDisk(drive: id) {
                
                drive.insertG64(proxy, protected: false)
                return
            }
        }
        if let file = attachment as? AnyCollectionProxy {
            
            if proceedWithUnexportedDisk(drive: id) {
                if let fs = try? Proxy.make(collection: file) as FSDeviceProxy {
                    drive.insertFileSystem(fs, protected: false)
                }
            }
        }
    }
            
    func runImportDialog() {
        
        let name = NSNib.Name("ImportDialog")
        let controller = ImportDialog.make(parent: parent, nibName: name)
        controller?.showSheet()
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
            try mountAttachment()
            
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
        try export(disk: drive.disk, to: url)
        
        drive.setModifiedDisk(false)
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
