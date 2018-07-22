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
     When the GUI receives the READY_TO_RUN message from the emulator, it
     checks this variable. If an attachment is present, e.g., a T64 archive,
     is displays a user dialog. The user can then choose to mount the archive
     as a disk or to flash a single file into memory. If the attachment is a
     snapshot, it is read into the emulator without asking the user.
     */
    var attachment: ContainerProxy? = nil
    
    /**
     If the attachment is a file archive, this variable determines how it is
     handled. If set to true, the first item is flushed directly into memory.
     If set to false, the archive is mounted as disk. For other attachment
     types (snapshots, cartridges, etc.), this variable has no effect.
     */
    var flushAttachment = false
    
    /// The list of recently inserted disk URLs.
    var recentlyInsertedDiskURLs: [URL] = []

    /// The list of recently inserted disk URLs.
    var recentlyExportedDiskURLs: [URL] = []

    /// The maximum number of items stored in the recentlyXXX lists
    var maximumRecentDiskCount = 10

    
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
        // or writes a MISSING_ROM message into the message queue.
        c64.run()
    }
 
    override open func makeWindowControllers() {
        
        track()
        
        let nibName = NSNib.Name(rawValue: "MyDocument")
        let controller = MyController.init(windowNibName: nibName)
        controller.c64 = c64
        self.addWindowController(controller)
    }
    
    //
    // Handling the lists of recently used files
    //
    
    func noteNewRecentlyInsertedDiskURL(url: URL) {
        
        if !recentlyInsertedDiskURLs.contains(url) {
            if recentlyInsertedDiskURLs.count == maximumRecentDiskCount {
                recentlyInsertedDiskURLs.remove(at: maximumRecentDiskCount - 1)
            }
            recentlyInsertedDiskURLs.insert(url, at: 0)
        }
    }
    
    func noteNewRecentlyExportedDiskURL(url: URL) {
        
        if !recentlyExportedDiskURLs.contains(url) {
            if recentlyExportedDiskURLs.count == maximumRecentDiskCount {
                recentlyExportedDiskURLs.remove(at: maximumRecentDiskCount - 1)
            }
            recentlyExportedDiskURLs.insert(url, at: 0)
        }
    }
    
    
    //
    // Creating attachments
    //
    
    /// Creates an attachment from a URL
    func createAttachment(from url: URL) throws {
    
        track("Trying to create attachment for file \(url.lastPathComponent).")

        // Try to create the attachment
        let fileWrapper = try FileWrapper.init(url: url)
        try createAttachment(from: fileWrapper, ofType: url.pathExtension)
        
        // Add URL to list of recently used files
        switch (url.pathExtension.uppercased()) {
        case "T64", "PRG", "D64", "P00", "G64", "NIB":
            noteNewRecentlyInsertedDiskURL(url: url)
        default:
            break
        }
    }
    
    /// Creates an attachment from a file wrapper
    func createAttachment(from fileWrapper: FileWrapper, ofType typeName: String) throws {
        
        guard let filename = fileWrapper.filename else {
            throw NSError(domain: "VirtualC64", code: 0, userInfo: nil)
        }
        guard let data = fileWrapper.regularFileContents else {
            throw NSError(domain: "VirtualC64", code: 0, userInfo: nil)
        }
        
        let buffer = (data as NSData).bytes
        let length = data.count
        var openAsUntitled = true
        
        track("Reading \(length) bytes for file \(filename) at \(buffer).")
        
        switch (typeName.uppercased()) {
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
        case "NIB":
            attachment = NIBProxy.make(withBuffer: buffer, length: length)
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
    }
    
    
    //
    // Processing attachments
    //
    
    func runCartridgeMountDialog(_ parent: MyController) {
        let nibName = NSNib.Name(rawValue: "CartridgeMountDialog")
        let controller = CartridgeMountController.init(windowNibName: nibName)
        controller.showSheet(withParent: parent)
    }
    
    func runTapeMountDialog(_ parent: MyController) {
        let nibName = NSNib.Name(rawValue: "TapeMountDialog")
        let controller = TapeMountController.init(windowNibName: nibName)
        controller.showSheet(withParent: parent)
    }
    
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
    
    
    /**
     This method is called when the user selects "Open..." or "Open recent"
     and a new document has been created. It analyzes the attachment type and
     performs several actions. When mount dialogs are enabled, it opens the
     corresponding dialog and let's the user decide what to do. Otherwise, a
     default action is performed.
     */
    func processAttachmentAfterOpen() {
        
        let parent = windowForSheet!.windowController as! MyController
        
        if attachment == nil {
            return
        }
        if attachment!.type() == V64_CONTAINER {
            c64.flash(attachment!)
            return
        }
        if parent.autoMount {
            c64.mount(attachment!)
            return
        }
        
        let type = attachment!.type()
        switch type {
    
        case CRT_CONTAINER:
            runCartridgeMountDialog(parent)
            return
                
        case TAP_CONTAINER:
            runTapeMountDialog(parent)
            return
                
        case T64_CONTAINER, D64_CONTAINER,
             PRG_CONTAINER, P00_CONTAINER:
            runArchiveMountDialog(parent)
            return
                
        case G64_CONTAINER, NIB_CONTAINER:
            runDiskMountDialog(parent)
            return
            
        default:
            track("Unknown attachment type \(type).")
            fatalError()
        }
    }
    
    /**
     This method is called when the user selects "Insert Disk..." or "Insert
     Recent" from the Drive menu. In contrast to processAttachmentAfterOpen(),
     no user dialogs show up.
     */
    @discardableResult
    func processAttachmentAfterInsert() -> Bool {
        
        var result = true
        // let parent = windowForSheet!.windowController as! MyController
        
        if attachment == nil {
            return false
        }
        
        let type = attachment!.type()
        switch type {
            
        case V64_CONTAINER:
            result = c64.flash(attachment!)
            break
            
        case PRG_CONTAINER, P00_CONTAINER,
             CRT_CONTAINER, TAP_CONTAINER,
             T64_CONTAINER, D64_CONTAINER,
             G64_CONTAINER, NIB_CONTAINER:
            result = c64.mount(attachment!)
            break
            
        default:
            track("Attachments of type \(type) cannot be mounted as a disk.")
            fatalError()
        }
        
        return result
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
        case V64_CONTAINER,
             T64_CONTAINER, D64_CONTAINER,
             G64_CONTAINER, NIB_CONTAINER:
            if (!proceedWithUnsavedDisk()) { return false }
        default:
            break;
        }
        
        if attachment!.type() == V64_CONTAINER {
            return c64.flash(attachment!)
        }
        
        // Perform default behavior if mount dialogs are disabled
        if parent.autoMount {
            if type == PRG_CONTAINER || type == P00_CONTAINER {
                flashAttachment(archive: attachment as! ArchiveProxy)
                return true
            } else {
                return c64.mount(attachment!)
            }
        }
        
        // Show mount dialog
        switch type {
            
        case CRT_CONTAINER:
            runCartridgeMountDialog(parent)
            return true
            
        case TAP_CONTAINER:
            runTapeMountDialog(parent)
            return true
            
        case T64_CONTAINER, D64_CONTAINER,
             PRG_CONTAINER, P00_CONTAINER:
            runArchiveMountDialog(parent)
            return true
            
        case G64_CONTAINER, NIB_CONTAINER:
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
            parent.keyboardcontroller.type("RUN")
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
            // track("Basic ROM:  \(url!)")
            defaults.set(url, forKey: VC64Keys.basicRom)
            return true
        }
        if c64.loadCharRom(url!) {
            // track("Char ROM:   \(url!)")
            defaults.set(url, forKey: VC64Keys.charRom)
            return true
        }
        if c64.loadKernalRom(url!) {
            // track("Kernal ROM: \(url!)")
            defaults.set(url, forKey: VC64Keys.kernalRom)
            return true
        }
        if c64.loadVC1541Rom(url!) {
            // track("VC1541 ROM: \(url!)")
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
    
    open override func removeWindowController(_ windowController: NSWindowController) {
        
        NSLog("MyDocument:\(#function)")

        super.removeWindowController(windowController)
        
        // Shut down the emulator.
        // Note that all GUI elements need to be inactive when we set the proxy
        // to nil. Hence, the emulator should be shut down as late as possible.
        c64.kill()
    }
}

