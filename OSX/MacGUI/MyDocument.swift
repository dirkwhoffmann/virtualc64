/*
 * (C) 2018 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

import Foundation

class MyDocument : NSDocument {
    
    /**
     Emulator proxy object. This object is an Objective-C bridge between
     the GUI (written in Swift) an the core emulator (written in C++).
     */
    var c64: C64Proxy!
    
    /**
     An otional media object attached to this document.
     This variable is checked by the GUI, e.g., when the READY_TO_RUN message
     is received. If an attachment is present, e.g., a T64 archive,
     is displays a user dialog. The user can then choose to mount the archive
     as a disk or to flash a single file into memory. If the attachment is a
     snapshot, it is read into the emulator without asking the user.
     This variable is also used when the user selects the "Insert Disk",
     "Insert Tape" or "Attach Cartridge" menu items. In that case, the selected
     URL is translated into an attachment and then processed. The actual
     post-processing depends on the attachment type and user options. E.g.,
     snapshots are flashed while T64 archives are converted to a disk and
     inserted into the disk drive.
     */
    var attachment: ContainerProxy? = nil
    
    /// The list of recently inserted disk URLs.
    var recentlyInsertedDiskURLs: [URL] = []

    /// The list of recently exported disk URLs for drive 1.
    var recentlyExportedDisk1URLs: [URL] = []

    /// The list of recently exported disk URLs for drive 2.
    var recentlyExportedDisk2URLs: [URL] = []

    /// The list of recently inserted tape URLs.
    var recentlyInsertedTapeURLs: [URL] = []

    /// The list of recently atached cartridge URLs.
    var recentlyAttachedCartridgeURLs: [URL] = []

    
    override init() {
        
        track()
        super.init()
        
        // Register standard user defaults
        MyController.registerUserDefaults()
        
        // Create emulator instance
        c64 = C64Proxy()
        
        // Try to load ROMs
        let defaults = UserDefaults.standard
        loadRom(defaults.url(forKey: VC64Keys.basicRom))
        loadRom(defaults.url(forKey: VC64Keys.charRom))
        loadRom(defaults.url(forKey: VC64Keys.kernalRom))
        loadRom(defaults.url(forKey: VC64Keys.vc1541Rom))
        
        // Try to run. The emulator will either run (if all ROMs were found)
        // or write a MISSING_ROM message into the message queue.
        // c64.run()
    }
 
    override open func makeWindowControllers() {
        
        track()
        
        let nibName = NSNib.Name(rawValue: "MyDocument")
        let controller = MyController.init(windowNibName: nibName)
        controller.c64 = c64
        self.addWindowController(controller)
    }
    
    
    //
    // Handling the lists of recently used URLs
    //
    
    func noteRecentlyUsedURL(_ url: URL, to list: inout [URL], size: Int) {
        if !list.contains(url) {
            if list.count == size {
                list.remove(at: size - 1)
            }
            list.insert(url, at: 0)
        }
    }
    
    func getRecentlyUsedURL(_ pos: Int, from list: [URL]) -> URL? {
        return (pos < list.count) ? list[pos] : nil
    }
    
    func noteNewRecentlyInsertedDiskURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &recentlyInsertedDiskURLs, size: 10)
    }
 
    func getRecentlyInsertedDiskURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: recentlyInsertedDiskURLs)
    }
    
    func noteNewRecentlyExportedDiskURL(_ url: URL, drive nr: Int) {

        precondition(nr == 1 || nr == 2)
        
        if (nr == 1) {
            noteRecentlyUsedURL(url, to: &recentlyExportedDisk1URLs, size: 1)
        } else {
            noteRecentlyUsedURL(url, to: &recentlyExportedDisk2URLs, size: 1)
        }
    }

    func getRecentlyExportedDiskURL(_ pos: Int, drive nr: Int) -> URL? {
        
        precondition(nr == 1 || nr == 2)
        
        if (nr == 1) {
            return getRecentlyUsedURL(pos, from: recentlyExportedDisk1URLs)
        } else {
            return getRecentlyUsedURL(pos, from: recentlyExportedDisk2URLs)
        }
    }
   
    func clearRecentlyExportedDiskURLs(drive nr: Int) {
        
        precondition(nr == 1 || nr == 2)
        
        if (nr == 1) {
            recentlyExportedDisk1URLs = []
        } else {
            recentlyExportedDisk2URLs = []
        }
    }
    
    func noteNewRecentlyInsertedTapeURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &recentlyInsertedTapeURLs, size: 10)
    }

    func getRecentlyInsertedTapeURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: recentlyInsertedTapeURLs)
    }
    
    func noteNewRecentlyAtachedCartridgeURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &recentlyAttachedCartridgeURLs, size: 10)
    }
  
    func getRecentlyAtachedCartridgeURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: recentlyAttachedCartridgeURLs)
    }
    
    func noteNewRecentlyUsedURL(_ url: URL) {
        
        switch (url.pathExtension.uppercased()) {
            
        case "D64", "T64", "G64", "NIB", "PRG", "P00":
            noteNewRecentlyInsertedDiskURL(url)

        case "TAP":
            noteNewRecentlyInsertedTapeURL(url)

        case "CRT":
            noteNewRecentlyAtachedCartridgeURL(url)
            
        default:
            break
        }
    }
    
    
    //
    // Creating attachments
    //
    
    /// Creates an attachment from a URL
    func createAttachment(from url: URL) throws {
    
        track("Creating attachment for file \(url.lastPathComponent).")

        // Try to create the attachment
        let fileWrapper = try FileWrapper.init(url: url)
        let pathExtension = url.pathExtension.uppercased()
        try createAttachment(from: fileWrapper, ofType: pathExtension)

        // Put URL in recently used URL lists
        noteNewRecentlyUsedURL(url)
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
        
        switch (typeName) {
        case "VC64":
            if SnapshotProxy.isUnsupportedSnapshot(buffer, length: length) {
                throw NSError.snapshotVersionError(filename: filename)
            }
            attachment = SnapshotProxy.make(withBuffer: buffer, length: length)
            openAsUntitled = false
            break
        case "CRT":
            if CRTProxy.isUnsupportedCRTBuffer(buffer, length: length) {
                let type = CRTProxy.typeName(ofCRTBuffer: buffer, length: length)!
                throw NSError.unsupportedCartridgeError(filename: filename, type: type)
            }
            attachment = CRTProxy.make(withBuffer: buffer, length: length)
            break
        case "TAP":
            attachment = TAPProxy.make(withBuffer: buffer, length: length)
            break
        case "T64":
            attachment = T64Proxy.make(withBuffer: buffer, length: length)
            break
        case "PRG":
            attachment = PRGProxy.make(withBuffer: buffer, length: length)
            break
        case "D64":
            attachment = D64Proxy.make(withBuffer: buffer, length: length)
            break
        case "P00":
            attachment = P00Proxy.make(withBuffer: buffer, length: length)
            break
        case "G64":
            attachment = G64Proxy.make(withBuffer: buffer, length: length)
            break
        default:
            throw NSError.unsupportedFormatError(filename: filename)
        }
        
        if openAsUntitled {
            fileURL = nil
        }
        if attachment == nil {
            throw NSError.corruptedFileError(filename: filename)
        }
        track("filename = \(filename)")
        attachment!.setPath(filename)
    }
    
    
    //
    // Processing attachments
    //
    
    func runArchiveMountDialog(_ parent: MyController) {
        let nibName = NSNib.Name(rawValue: "ArchiveMountDialog")
        let controller = ArchiveMountController.init(windowNibName: nibName)
        controller.showSheet(withParent: parent)
    }
    
    func runDiskMountDialog(_ parent: MyController) {
        let nibName = NSNib.Name(rawValue: "DiskMountDialog")
        let controller = DiskMountController.init(windowNibName: nibName)
        controller.showSheet(withParent: parent)
    }
    
    func runTapeMountDialog(_ parent: MyController) {
        let nibName = NSNib.Name(rawValue: "TapeMountDialog")
        let controller = TapeMountController.init(windowNibName: nibName)
        controller.showSheet(withParent: parent)
    }
    
    func runCartridgeMountDialog(_ parent: MyController) {
        let nibName = NSNib.Name(rawValue: "CartridgeMountDialog")
        let controller = CartridgeMountController.init(windowNibName: nibName)
        controller.showSheet(withParent: parent)
    }
    
    
    /**
     This method is called when a new document is created. It analyzes the
     attachment type and performs several actions. When mount dialogs are
     enabled, it opens the corresponding dialog and let's the user decide what
     to do. Otherwise, a default action is performed.
     */
    func openAttachmentWithDocument() {
        
        let parent = windowForSheet!.windowController as! MyController
        
        if attachment == nil {
            return
        }
        if attachment!.type() == V64_FILE {
            c64.flash(attachment!)
            return
        }
        if parent.autoMount {
            parent.mount(attachment)
            return
        }
        
        let type = attachment!.type()
        switch type {
    
        case CRT_FILE:
            runCartridgeMountDialog(parent)
            return
                
        case TAP_FILE:
            runTapeMountDialog(parent)
            return
                
        case T64_FILE, D64_FILE,
             PRG_FILE, P00_FILE:
            runArchiveMountDialog(parent)
            return
                
        case G64_FILE:
            runDiskMountDialog(parent)
            return
            
        default:
            track("Unknown attachment type \(type).")
            fatalError()
        }
    }
    
    /**
     This method is called when the user selects "Insert Disk..." or "Insert
     Recent" from the Drive menu. In contrast to openAttachmentWithDocument(),
     no user dialogs show up.
     */
    @discardableResult
    func insertAttachmentAsDisk(drive nr: Int) -> Bool {
        
        if let archive = attachment as? ArchiveProxy {
           
            if proceedWithUnexportedDisk(drive: nr) {
             
                let parent = windowForSheet!.windowController as! MyController
                parent.changeDisk(archive, drive: nr)
                return true;
            }
        }
        return false
    }
    
    /**
     This method is called when the user selects "Insert Tape..." or "Insert
     Recent" from the Tape menu. In contrast to openAttachmentWithDocument(),
     no user dialogs show up.
     */
    @discardableResult
    func insertAttachmentAsTape() -> Bool {
        
        guard let type = attachment?.type() else {
            return false
        }
        
        switch type {
            
        case TAP_FILE:
            let parent = windowForSheet!.windowController as! MyController
            return parent.mount(attachment!)
            
        default:
            track("Attachments of type \(type) cannot be inserted as tape.")
            fatalError()
        }
    }
    
    /**
     This method is called when the user selects "Attach Cartridge..." or
     "Attach Recent" from the Cartridge menu. In contrast to
     openAttachmentWithDocument(), no user dialogs show up.
     */
    @discardableResult
    func attachAttachmentAsCartridge() -> Bool {
        
        guard let type = attachment?.type() else {
            return false
        }
        
        switch type {
        
        case CRT_FILE:
            let parent = windowForSheet!.windowController as! MyController
            return parent.mount(attachment!)
            
        default:
            track("Attachments of type \(type) cannot be attached as cartridge.")
            fatalError()
        }
    }
    
    /**
     This method is called after a media file has been dragged and dropped
     into the emulator.
     */
    @discardableResult
    func processAttachmentAfterDragAndDrop() -> Bool {
        
        let parent = windowForSheet!.windowController as! MyController
        
        if attachment == nil {
            return false
        }
        
        // Check if disk data would be lost, if we continue
        let type = attachment!.type()
        switch type {
        case V64_FILE, T64_FILE, D64_FILE, G64_FILE:
            if (!proceedWithUnexportedDisk(drive: 1)) { return false }
        default:
            break;
        }
        
        if attachment!.type() == V64_FILE {
            return c64.flash(attachment!)
        }
        
        // Perform default behavior if mount dialogs are disabled
        if parent.autoMount {
            if type == PRG_FILE || type == P00_FILE {
                flashAttachment(archive: attachment as! ArchiveProxy)
                return true
            } else {
                return parent.mount(attachment)
            }
        }
        
        // Show mount dialog
        switch type {
            
        case CRT_FILE:
            runCartridgeMountDialog(parent)
            return true
            
        case TAP_FILE:
            runTapeMountDialog(parent)
            return true
            
        case T64_FILE, D64_FILE,
             PRG_FILE, P00_FILE:
            runArchiveMountDialog(parent)
            return true
            
        case G64_FILE:
            runDiskMountDialog(parent)
            return true
            
        default:
            track("Unknown attachment type \(type).")
            fatalError()
        }
    }
    
    func flashAttachment(archive: ArchiveProxy, item: Int = 0) {
        
        let parent = windowForSheet!.windowController as! MyController
        
        // Get load address of the flashed item
        let loadAddr = (attachment as! ArchiveProxy).destinationAddr(ofItem: item)

        // Flash program at it's designated load address
        c64.flash(archive, item: item)
    
        // Type RUN or SYS
        if loadAddr == 0x801 {
            parent.keyboardcontroller.type("RUN\n")
        } else {
            parent.keyboardcontroller.type("SYS \(loadAddr)")
        }
    }
    
    
    //
    // Loading
    //
    
    override open func read(from url: URL, ofType typeName: String) throws {
        
        try createAttachment(from: url)
    }
    
    /// Loads a ROM image file into the emulator and stores the URL in the
    /// the user defaults.
    @discardableResult
    func loadRom(_ url: URL?) -> Bool {
        
        if (url == nil) {
            return false
        }
        
        let defaults = UserDefaults.standard
        
        if c64.loadBasicRom(url!) {
            defaults.set(url, forKey: VC64Keys.basicRom)
            return true
        }
        if c64.loadCharRom(url!) {
            defaults.set(url, forKey: VC64Keys.charRom)
            return true
        }
        if c64.loadKernalRom(url!) {
            defaults.set(url, forKey: VC64Keys.kernalRom)
            return true
        }
        if c64.loadVC1541Rom(url!) {
            defaults.set(url, forKey: VC64Keys.vc1541Rom)
            return true
        }
        
        track("ROM file \(url!) not found")
        return false
    }
    
    
    //
    // Saving
    //
    
    override open func data(ofType typeName: String) throws -> Data {
        
        track("Trying to write \(typeName) file.")
        
        if typeName == "VC64" {

            NSLog("Type is VC64")
            
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
    
    func export(drive nr: Int, to url: URL, ofType typeName: String) -> Bool {
        
        track("url = \(url) typeName = \(typeName)")
        precondition(["D64", "T64", "PRG", "P00", "G64"].contains(typeName))
        
        let drive = c64.drive(nr)!
        
        // Convert disk to a D64 archive
        guard let d64archive = D64Proxy.make(withDrive: drive) else {
            return false
        }
        
        // Convert the D64 archive into the target format
        var archive: ArchiveProxy?
        switch typeName.uppercased() {
        case "D64":
            track("Exporting to D64 format")
            archive = d64archive
        
        case "G64":
            track("Exporting to G64 format")
            archive = G64Proxy.make(withDisk: drive.disk)
            
        case "T64":
            track("Exporting to T64 format")
            archive = T64Proxy.make(withAnyArchive: d64archive)
            
        case "PRG":
            track("Exporting to PRG format")
            if d64archive.numberOfItems() > 1  {
                showDiskHasMultipleFilesAlert(format: "PRG")
            }
            archive = PRGProxy.make(withAnyArchive: d64archive)
            
        case "P00":
            track("Exporting to P00 format")
            if d64archive.numberOfItems() > 1  {
                showDiskHasMultipleFilesAlert(format: "P00")
            }
            archive = P00Proxy.make(withAnyArchive: d64archive)
            
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
        drive.setModifiedDisk(false)
        
        // Remember export URL
        noteNewRecentlyExportedDiskURL(url, drive: nr)
        return true
    }
    
    @discardableResult
    func export(drive nr: Int, to url: URL?) -> Bool {
        
        if let suffix = url?.pathExtension {
            return export(drive: nr, to: url!, ofType: suffix.uppercased())
        } else {
            return false
        }
    }
    
    
    //
    // Shutting down
    //
    
    open override func removeWindowController(_ windowController: NSWindowController) {
        
        NSLog("MyDocument:\(#function)")

        super.removeWindowController(windowController)
        
        // Shut down the emulator.
        // Note that all GUI elements need to be inactive when we set the proxy
        // to nil. Hence, the emulator should be shut down as late as possible.
        c64.kill()
    }
}

