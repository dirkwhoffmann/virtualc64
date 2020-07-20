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
    
    /*
     Emulator proxy object. This object is an Objective-C bridge between
     the GUI (written in Swift) an the core emulator (written in C++).
     */
    var c64: C64Proxy!
    
    /*
     An otional media object attached to this document.
     This variable is checked in mountAttachment() which is called in
     windowDidLoad(). If an attachment is present, e.g., a D64 archive, it
     is automatically attached to the emulator.
     */
    var attachment: AnyC64FileProxy?
    
    //
    // Initialization
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
    
    /// Creates an attachment from a URL
    func createAttachment(from url: URL) throws {
    
        track("Creating attachment from URL \(url.lastPathComponent).")

        // Try to create the attachment
        let fileWrapper = try FileWrapper.init(url: url)
        let pathExtension = url.pathExtension.uppercased()
        try createAttachment(from: fileWrapper, ofType: pathExtension)

        // Put URL in recently used URL lists
        myAppDelegate.noteNewRecentlyUsedURL(url)
    }
    
    /// Creates an attachment from a file wrapper
    fileprivate func createAttachment(from fileWrapper: FileWrapper,
                                      ofType typeName: String) throws {
        
        guard let filename = fileWrapper.filename else {
            throw NSError(domain: "VirtualC64", code: 0, userInfo: nil)
        }
        guard let data = fileWrapper.regularFileContents else {
            throw NSError(domain: "VirtualC64", code: 0, userInfo: nil)
        }
        
        let buffer = (data as NSData).bytes
        let length = data.count
        var openAsUntitled = true
        
        track("Read \(length) bytes from file \(filename).")
        
        switch typeName {
            
        case "VC64":
            // Check for outdated snapshot formats
            if SnapshotProxy.isUnsupportedSnapshot(buffer, length: length) {
                throw NSError.snapshotVersionError(filename: filename)
            }
            attachment = SnapshotProxy.make(withBuffer: buffer, length: length)
            openAsUntitled = false

        case "CRT":
            // Check for unsupported cartridge types
            if CRTFileProxy.isUnsupportedCRTBuffer(buffer, length: length) {
                let type = CRTFileProxy.typeName(ofCRTBuffer: buffer, length: length)!
                throw NSError.unsupportedCartridgeError(filename: filename, type: type)
            }
            attachment = CRTFileProxy.make(withBuffer: buffer, length: length)
            
        case "TAP":
            attachment = TAPFileProxy.make(withBuffer: buffer, length: length)
            
        case "T64":
            attachment = T64FileProxy.make(withBuffer: buffer, length: length)
            
        case "PRG":
            attachment = PRGFileProxy.make(withBuffer: buffer, length: length)
            
        case "D64":
            attachment = D64FileProxy.make(withBuffer: buffer, length: length)
            
        case "P00":
            attachment = P00FileProxy.make(withBuffer: buffer, length: length)
            
        case "G64":
            attachment = G64FileProxy.make(withBuffer: buffer, length: length)
            
        default:
            throw NSError.unsupportedFormatError(filename: filename)
        }
        
        if attachment == nil {
            throw NSError.corruptedFileError(filename: filename)
        }
        if openAsUntitled {
            fileURL = nil
        }
        attachment!.setPath(filename)
    }

    //
    // Processing attachments
    //
    
    @discardableResult
    func mountAttachment() -> Bool {
        
        guard let controller = myController else { return false }
        let pref = controller.pref
        
        // Determine action to perform and text to type
        var mountAction = AutoMountAction.openBrowser
        var autoText: String?

        func getAction(_ type: String) {
            mountAction = pref.mountAction[type] ?? mountAction
            if mountAction != .openBrowser && (pref.autoType[type] ?? false) {
                autoText = pref.autoText[type]
            }
        }

        switch attachment {
        case _ as SnapshotProxy: c64.flash(attachment); return true
        case _ as D64FileProxy, _ as G64FileProxy: getAction("D64")
        case _ as PRGFileProxy, _ as P00FileProxy: getAction("PRG")
        case _ as T64FileProxy: getAction("T64")
        case _ as TAPFileProxy: getAction("TAP")
        case _ as CRTFileProxy: getAction("CRT")
        default: return false
        }
    
        // Check if the emulator has just been startet. In that case, we have
        // to wait until the Kernal boot routine has been executed. Otherwise,
        // the C64 would ignore everything we are doing here.
        let delay = (c64.cpu.cycle() < 3000000) ? 2.0 : 0.0

        // Execute asynchronously ...
        DispatchQueue.main.asyncAfter(deadline: .now() + delay, execute: {
            self.mountAttachment(action: mountAction, text: autoText)
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
        
        switch attachment {
            
        case _ as CRTFileProxy:
            runCartridgeMountDialog()
            
        case _ as TAPFileProxy:
            runTapeMountDialog()
            
        case _ as T64FileProxy, _ as D64FileProxy,
             _ as PRGFileProxy, _ as P00FileProxy:
            runArchiveMountDialog()
            
        case _ as G64FileProxy:
            runDiskMountDialog()
            
        default:
            break
        }
    }
    
    func runArchiveMountDialog() {
        let nibName = NSNib.Name("ArchiveMountDialog")
        let controller = ArchiveMountController.init(windowNibName: nibName)
        controller.showSheet()
    }
    
    func runDiskMountDialog() {
        let nibName = NSNib.Name("DiskMountDialog")
        let controller = DiskMountController.init(windowNibName: nibName)
        controller.showSheet()
    }
    
    func runTapeMountDialog() {
        let nibName = NSNib.Name("TapeMountDialog")
        let controller = TapeMountController.init(windowNibName: nibName)
        controller.showSheet()
    }
    
    func runCartridgeMountDialog() {
        let nibName = NSNib.Name("CartridgeMountDialog")
        let controller = CartridgeMountController.init(windowNibName: nibName)
        controller.showSheet()
    }
    
    @discardableResult
    func mountAttachmentAsDisk(drive: DriveID) -> Bool {
        
        if let archive = attachment as? AnyArchiveProxy {
            
            if proceedWithUnexportedDisk(drive: drive) {
                
                parent.changeDisk(archive, drive: drive)
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
        
        try createAttachment(from: url)
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
    // Shutting down
    //
    
    open override func removeWindowController(_ windowController: NSWindowController) {
        
        track()

        // Shut down the emulator.
        // Note that all GUI elements have to be inactive when the proxy is set
        // to nil. Hence, the emulator should be shut down as late as possible.
        
        /*
        let controller = myController!
        
        controller.proxyLock.lock()
        c64.kill()
        c64 = nil
        controller.c64 = nil
        controller.proxyLock.unlock()
        */
        
        super.removeWindowController(windowController)
    }
}
