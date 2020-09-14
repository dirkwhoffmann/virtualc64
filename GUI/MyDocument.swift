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
            track("\(url) is a directory")
            return .FILETYPE_GENERIC_ARCHIVE
        }
        
        switch url.pathExtension.uppercased() {
            
        case "VC64": return .FILETYPE_V64
        case "CRT":  return .FILETYPE_CRT
        case "D64":  return .FILETYPE_D64
        case "T64":  return .FILETYPE_T64
        case "PRG":  return .FILETYPE_PRG
        case "P00":  return .FILETYPE_P00
        case "G64":  return .FILETYPE_G64
        case "TAP":  return .FILETYPE_TAP
        default:     return .FILETYPE_UNKNOWN
        }
    }
    
    func createAttachment(from url: URL) throws {
        
        let types = [ FileType.FILETYPE_V64,
                      FileType.FILETYPE_CRT,
                      FileType.FILETYPE_T64,
                      FileType.FILETYPE_PRG,
                      FileType.FILETYPE_P00,
                      FileType.FILETYPE_D64,
                      FileType.FILETYPE_G64,
                      FileType.FILETYPE_TAP,
                      FileType.FILETYPE_GENERIC_ARCHIVE ]
        
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
        
        // Only proceed if the file type is an allowed type
        let type = fileType(url: newUrl)
        track("type = \(type.rawValue)")
        if !allowedTypes.contains(type) {
            throw NSError.unsupportedFormatError(filename: url.lastPathComponent)
        }
        
        // If url points to a directory, convert it to a generic archive
        if url.hasDirectoryPath {
            return try createFileProxyFromDirectory(url: url)
        }
        
        // Get the file wrapper and create the proxy with it
        let wrapper = try FileWrapper.init(url: newUrl)
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
            
        case .FILETYPE_V64:
            if SnapshotProxy.isUnsupportedSnapshot(buffer, length: length) {
                throw NSError.snapshotVersionError(filename: name)
            }
            result = SnapshotProxy.make(withBuffer: buffer, length: length)
            
        case .FILETYPE_CRT:
            if CRTFileProxy.isUnsupportedCRTBuffer(buffer, length: length) {
                let type = CRTFileProxy.typeName(ofCRTBuffer: buffer, length: length)!
                throw NSError.unsupportedCartridgeError(filename: name, type: type)
            }
            result = CRTFileProxy.make(withBuffer: buffer, length: length)
            
        case .FILETYPE_D64:
            result = D64FileProxy.make(withBuffer: buffer, length: length)
            
        case .FILETYPE_T64:
            result = T64FileProxy.make(withBuffer: buffer, length: length)
            
        case .FILETYPE_PRG:
            result = PRGFileProxy.make(withBuffer: buffer, length: length)
            
        case .FILETYPE_P00:
            result = P00FileProxy.make(withBuffer: buffer, length: length)
            
        case .FILETYPE_G64:
            result = G64FileProxy.make(withBuffer: buffer, length: length)
            
        case .FILETYPE_TAP:
            result = TAPFileProxy.make(withBuffer: buffer, length: length)
            
        default:
            fatalError()
        }
        
        if result == nil {
            throw NSError.corruptedFileError(filename: name)
        }
        result!.setPath(name)
        return result
    }
        
    fileprivate
    func createFileProxyFromDirectory(url: URL) throws -> AnyFileProxy? {

        var result: GenericArchiveProxy?
        
        track("Creating GenericArchive proxy from directory \(url)")
        
        let files = try url.contents(allowedTypes: ["PRG"])
        track("files = \(files)")

        result = GenericArchiveProxy.make()
        for file in files {
            
            let suffix = file.pathExtension
            let name = file.deletingPathExtension().lastPathComponent

            let wrapper = try FileWrapper.init(url: file)
            
            guard let data = wrapper.regularFileContents else {
                throw NSError.fileAccessError(filename: name)
            }
            let buffer = (data as NSData).bytes
            let length = data.count
                
            track("File \(name).\(suffix): \(length) bytes")
                
            result?.addItem(name, buffer: buffer, size: length)
        }
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

        // Determine the action to perform and the text to type
        let key = attachment!.typeAsString()!
        let action = parent.pref.mountAction[key] ?? AutoMountAction.openBrowser

        // If the action is to open the media dialog, open it and return
        if action == .openBrowser { runMountDialog(); return true }

        // Determine if a text should be typed
        let type = parent.pref.autoType[key] ?? false
        let text = type ? parent.pref.autoText[key] : nil
        
        /* Determine when the action should be performed. Background: If the
         * emulator has just been startet, we have to wait until the startup
         * procedure has been executed. Otherwise, the Kernal would ignore
         * everything we do here.
         */
        let delay = (c64.cpu.cycle() < 3000000) ? 2.0 : 0.0

        // Execute the action asynchronously
        DispatchQueue.main.asyncAfter(deadline: .now() + delay, execute: {
            self.mountAttachment(action: action, text: text)
        })
        
        return true
    }
    
    func mountAttachment(action: AutoMountAction, text: String?) {
        
        // Perform action
        track("Action = \(action)")
        switch action {
        case .openBrowser: runMountDialog()
        case .insertIntoDrive8: mountAttachmentAsDisk(drive: DRIVE8)
        case .insertIntoDrive9: mountAttachmentAsDisk(drive: DRIVE9)
        case .flashFirstFile: flashAttachmentIntoMemory()
        case .insertIntoDatasette: mountAttachmentAsTape()
        case .attachToExpansionPort: mountAttachmentAsCartridge()
        }
        
        // Type text
        if text != nil {
            track("Auto typing: \(text!)")
            myController?.keyboard.type(text! + "\n")
        }
    }
    
    func runMountDialog() {
        
        let name = NSNib.Name("MediaDialog")
        let controller = MediaDialogController.make(parent: parent, nibName: name)
        controller?.showSheet()
    }
    
    @discardableResult
    func mountAttachmentAsDisk(drive: DriveID) -> Bool {
        
        if let archive = attachment as? AnyArchiveProxy {
            
            if proceedWithUnexportedDisk(drive: drive) {
                
                c64.drive(drive)?.insertDisk(archive)
                return true
            }
        }
        return false
    }
    
    @discardableResult
    func mountAttachmentAsTape() -> Bool {
        
        if let tape = attachment as? TAPFileProxy {
            
            return parent.c64.datasette.insertTape(tape)
        }
        return false
    }
    
    @discardableResult
    func flashAttachmentIntoMemory() -> Bool {
        
        if let archive = attachment as? AnyArchiveProxy {
            
            return parent.c64.flash(archive, item: 0)
        }
        return false
    }
    
    @discardableResult
    func mountAttachmentAsCartridge() -> Bool {
        
        if let cartridge = attachment as? CRTFileProxy {
            
            parent.c64.expansionport.attachCartridgeAndReset(cartridge)
            return true
        }
        return false
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
    
    func export(drive: DriveID, to url: URL, ofType typeName: String) -> Bool {
        
        track("url = \(url) typeName = \(typeName)")
        precondition(["D64", "T64", "PRG", "P00", "G64"].contains(typeName))
        
        let proxy = c64.drive(drive)!
        
        // Convert disk to a D64 archive
        guard let d64archive = D64FileProxy.make(withDisk: proxy.disk) else {
            showCannotDecodeDiskAlert()
            return false
        }
        
        // Convert the D64 archive into the target format
        var archive: AnyArchiveProxy?
        switch typeName.uppercased() {
        case "D64":
            track("Exporting to D64 format")
            archive = d64archive
        
        case "G64":
            track("Exporting to G64 format")
            archive = G64FileProxy.make(withDisk: proxy.disk)
            
        case "T64":
            track("Exporting to T64 format")
            archive = T64FileProxy.make(withAnyArchive: d64archive)
            
        case "PRG":
            track("Exporting to PRG format")
            if d64archive.numberOfItems() > 1 {
                showDiskHasMultipleFilesAlert(format: "PRG")
            }
            archive = PRGFileProxy.make(withAnyArchive: d64archive)
            
        case "P00":
            track("Exporting to P00 format")
            if d64archive.numberOfItems() > 1 {
                showDiskHasMultipleFilesAlert(format: "P00")
            }
            archive = P00FileProxy.make(withAnyArchive: d64archive)
            
        default:
            fatalError()
        }
        
        // Serialize archive
        let data = NSMutableData.init(length: archive!.sizeOnDisk())!
        archive!.write(toBuffer: data.mutableBytes)
        
        // Write to file
        if !data.write(to: url, atomically: true) {
            showExportErrorAlert(url: url)
            return false
        }
        
        // Mark disk as "not modified"
        proxy.setModifiedDisk(false)
        
        // Remember export URL
        myAppDelegate.noteNewRecentlyExportedDiskURL(url, drive: drive)
        return true
    }
    
    @discardableResult
    func export(drive: DriveID, to url: URL?) -> Bool {
        
        if let suffix = url?.pathExtension {
            return export(drive: drive, to: url!, ofType: suffix.uppercased())
        } else {
            return false
        }
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
