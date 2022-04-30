// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MyDocument: NSDocument {

    var pref: Preferences { return myAppDelegate.pref }

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

    //
    // Initializing
    //
    
    override init() {

        log()

        super.init()

        // Check for Metal support
        if MTLCreateSystemDefaultDevice() == nil {

            showAlert(.noMetalSupport)
            NSApp.terminate(self)
            return
        }

        // Register all GUI related user defaults
        C64Proxy.defaults.registerUserDefaults()

        // Load the user default settings
        C64Proxy.defaults.load()

        // Create an emulator instance
        c64 = C64Proxy()
    }

    override open func makeWindowControllers() {
        
        log()
        
        let controller = MyController(windowNibName: "MyDocument")
        controller.c64 = c64
        self.addWindowController(controller)
    }

    //
    // Creating file proxys
    //

    func createFileProxy(from url: URL, allowedTypes: [FileType]) throws -> AnyFileProxy? {

        log("Reading file \(url.lastPathComponent)")

        // If the provided URL points to compressed file, decompress it first
        let newUrl = url.unpacked(maxSize: 2048 * 1024)

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

        // None of the allowed types matched the file
        throw VC64Error(.FILE_TYPE_MISMATCH,
                      "The type of this file is not known to the emulator.")
    }

    //
    // Loading
    //

    override open func read(from url: URL, ofType typeName: String) throws {

        log()

        let types: [FileType] =
        [ .SNAPSHOT, .SCRIPT, .D64, .T64, .PRG, .P00, .G64, .TAP, .FOLDER ]

        do {

            try addMedia(url: url, allowedTypes: types)

        } catch let error as VC64Error {

            throw NSError(error: error)
        }
    }

    override open func revert(toContentsOf url: URL, ofType typeName: String) throws {

        log()

        do {
            let proxy = try createFileProxy(from: url, allowedTypes: [.SNAPSHOT])
            if let snapshot = proxy as? SnapshotProxy {
                try processSnapshotFile(snapshot)
            }

        } catch let error as VC64Error {

            throw NSError(error: error)
        }
    }

    //
    // Saving
    //

    override func write(to url: URL, ofType typeName: String) throws {

        track()

        if typeName == "VC64" {

            if let snapshot = SnapshotProxy.make(withC64: c64) {

                do {
                    try snapshot.writeToFile(url: url)

                } catch let error as VC64Error {

                    throw NSError(error: error)
                }
            }
        }
    }

    //
    // Handling media files
    //

    func addMedia(url: URL,
                  allowedTypes types: [FileType],
                  drive id: DriveID = .DRIVE8,
                  force: Bool = false,
                  remember: Bool = true) throws {

        log("url = \(url) types = \(types)")

        let proxy = try createFileProxy(from: url, allowedTypes: types)

        if let proxy = proxy as? SnapshotProxy {

            log("Snapshot")
            try processSnapshotFile(proxy)
        }
        if let proxy = proxy as? ScriptProxy {

            log("Script")
            parent.renderer.console.runScript(script: proxy)
        }
        if let proxy = proxy as? CRTFileProxy {

            log("CRT")
            try c64.expansionport.attachCartridge(proxy, reset: true)
        }
        if let proxy = proxy as? TAPFileProxy {

            log()
            c64.datasette.insertTape(proxy)
        }
        if let proxy = proxy as? D64FileProxy {

            log("D64")
            if proceedWithUnexportedDisk(drive: id) {

                c64.drive(id).insertD64(proxy, protected: false)
                if remember { myAppDelegate.noteNewRecentlyInsertedDiskURL(url) }
            }
        }
        if let proxy = proxy as? G64FileProxy {

            log("G64")
            if proceedWithUnexportedDisk(drive: id) {

                c64.drive(id).insertG64(proxy, protected: false)
                if remember { myAppDelegate.noteNewRecentlyInsertedDiskURL(url) }
            }
        }
        if let proxy = proxy as? AnyCollectionProxy {

            log("T64, PRG, P00")
            if proceedWithUnexportedDisk(drive: id) {

                c64.drive(id).insertCollection(proxy, protected: false)
                if remember { myAppDelegate.noteNewRecentlyInsertedDiskURL(url) }
            }
        }
    }

    func processSnapshotFile(_ proxy: SnapshotProxy, force: Bool = false) throws {

        try c64.flash(proxy)
        snapshots.append(proxy)
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
        let newUrl = url.unpacked(maxSize: 2048 * 1024)

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

        // None of the allowed types matched the file
        throw VC64Error(.FILE_TYPE_MISMATCH,
                        "The type of this file is not known to the emulator.")
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
            try c64.expansionport.attachCartridge(proxy, reset: true)
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
                
                drive.insertD64(proxy, protected: false)
                return
            }
        }
        if let proxy = attachment as? G64FileProxy {
            
            if proceedWithUnexportedDisk(drive: id) {
                
                drive.insertG64(proxy, protected: false)
                return
            }
        }
        if let proxy = attachment as? AnyCollectionProxy {
            
            if proceedWithUnexportedDisk(drive: id) {
                
                drive.insertCollection(proxy, protected: false)
                return
            }
        }
    }

    func runImportDialog() {
        
        let name = NSNib.Name("ImportDialog")
        let controller = ImportDialog(with: parent, nibName: name)
        controller?.showSheet()
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
            
            let fs = try Proxy.make(disk: disk) as FileSystemProxy
            try export(fs: fs, to: url)
        }
    }
    
    func export(fs: FileSystemProxy, to url: URL) throws {

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
}
