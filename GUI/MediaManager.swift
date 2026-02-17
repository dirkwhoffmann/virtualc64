// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

@MainActor
class MediaManager {
    
    struct Option: OptionSet {
        
        let rawValue: Int
        
        static let force       = Option(rawValue: 1 << 0)
        static let remember    = Option(rawValue: 1 << 1)
        static let protect     = Option(rawValue: 1 << 2)
        static let reset       = Option(rawValue: 1 << 3)
        static let autostart   = Option(rawValue: 1 << 4)
        static let flash       = Option(rawValue: 1 << 5)
    }
    
    // References to other objects
    var mydocument: MyDocument!
    var emu: EmulatorProxy? { return mydocument.emu }
    var mycontroller: MyController { return mydocument.controller }
    var console: Console { return mycontroller.renderer.console }
    
    // References to menu outlets
    var drive8InsertRecent: NSMenuItem! { return myAppDelegate.drive8InsertRecent }
    var drive9InsertRecent: NSMenuItem! { return myAppDelegate.drive9InsertRecent }
    var drive8ExportRecent: NSMenuItem! { return myAppDelegate.drive8ExportRecent }
    var drive9ExportRecent: NSMenuItem! { return myAppDelegate.drive9ExportRecent }
    
    // Shared list of recently inserted tapes
    static var insertedTapes: [URL] = []
    
    // Shared list of recently attached cartridges
    static var attachedCartridges: [URL] = []
    
    // Shared list of recently inserted floppy disks
    static var insertedFloppyDisks: [URL] = []
    
    // Unshared list of recently exported floppy disks
    var exportedFloppyDisks: [[URL]] = [[URL]](repeating: [URL](), count: 2)
    
    //
    // Initializing
    //
    
    init(with document: MyDocument) {
        
        debug(.lifetime, "Creating media manager")
        self.mydocument = document
        
        initUrlMenus([drive8InsertRecent, drive9InsertRecent], count: 10,
                     action: #selector(MyController.insertRecentDiskAction(_:)))
        initUrlMenus([drive8ExportRecent, drive9ExportRecent], count: 1,
                     action: #selector(MyController.exportRecentDiskAction(_:)))
    }
    
    func initUrlMenus(_ menus: [NSMenuItem], count: Int,
                      action: Selector?, clearAction: Selector? = nil) {
        
        for (index, menu) in menus.enumerated() {
            
            initUrlMenu(menu, count: count, tag: index, action: action, clearAction: clearAction)
        }
    }
    
    func initUrlMenu(_ menuItem: NSMenuItem, count: Int, tag: Int,
                     action: Selector?, clearAction: Selector? = nil) {
        
        let menu = menuItem.submenu!
        menu.removeAllItems()
        
        for index in 0..<count {
            
            let item = NSMenuItem(title: "\(index)", action: action, keyEquivalent: "")
            item.tag = tag << 16 | index
            menu.addItem(item)
        }
        
        if let clearAction = clearAction {
            
            menu.addItem(NSMenuItem.separator())
            menu.addItem(NSMenuItem(title: "Clear Menu", action: clearAction, keyEquivalent: ""))
        }
    }
    
    //
    // Handling lists of recently used URLs
    //
    
    static func noteRecentlyUsedURL(_ url: URL, to list: inout [URL], size: Int) {
        
        if !list.contains(url) {
            
            // Shorten the list if it is too large
            if list.count == size { list.remove(at: size - 1) }
            
            // Add new item at the beginning
            list.insert(url, at: 0)
        }
    }
    
    static func getRecentlyUsedURL(_ pos: Int, from list: [URL]) -> URL? {
        return (pos < list.count) ? list[pos] : nil
    }
    
    // Import URLs (shared among all emulator instances and drives)
    
    static func noteNewRecentlyInsertedTapeURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &insertedTapes, size: 10)
    }
    
    static func getRecentlyInsertedTapeURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: insertedTapes)
    }
    
    static func noteNewRecentlyAtachedCartridgeURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &attachedCartridges, size: 10)
    }
    
    static func getRecentlyAtachedCartridgeURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: attachedCartridges)
    }
    
    static func noteNewRecentlyInsertedDiskURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &insertedFloppyDisks, size: 10)
    }
    
    static func getRecentlyInsertedDiskURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: insertedFloppyDisks)
    }
    
    static func noteNewRecentlyUsedURL(_ url: URL) {
        
        switch url.pathExtension.uppercased() {
            
        case "D64", "T64", "G64", "PRG", "P00":
            noteNewRecentlyInsertedDiskURL(url)
            
        case "TAP":
            noteNewRecentlyInsertedTapeURL(url)
            
        case "CRT":
            noteNewRecentlyAtachedCartridgeURL(url)
            
        default:
            break
        }
    }
    
    // Export URLs (not shared)
    
    func noteNewRecentlyExportedDiskURL(_ url: URL, drive: Int) {
        
        precondition(drive == DRIVE8 || drive == DRIVE9)
        let n = drive == DRIVE8 ? 0 : 1
        MediaManager.noteRecentlyUsedURL(url, to: &exportedFloppyDisks[n], size: 1)
    }
    
    func getRecentlyExportedDiskURL(_ pos: Int, drive: Int) -> URL? {
        
        precondition(drive == DRIVE8 || drive == DRIVE9)
        let n = drive == DRIVE8 ? 0 : 1
        return MediaManager.getRecentlyUsedURL(pos, from: exportedFloppyDisks[n])
    }
    
    func clearRecentlyExportedDiskURLs(drive: Int) {
        
        precondition(drive == DRIVE8 || drive == DRIVE9)
        let n = drive == DRIVE8 ? 0 : 1
        exportedFloppyDisks[n] = [URL]()
    }

    func noteNewRecentlyOpenedURL(_ url: URL, type: FileType) {

        switch type {

        case .CRT:
            MediaManager.noteNewRecentlyAtachedCartridgeURL(url)
        case .T64, .P00, .PRG, .D64, .G64:
            MediaManager.noteNewRecentlyInsertedDiskURL(url)
        case .TAP:
            MediaManager.noteNewRecentlyInsertedTapeURL(url)
        default:
            break
        }
    }

    func noteNewRecentlyExportedURL(_ url: URL, nr: Int, type: FileType) {

        switch type {

        case .T64, .P00, .PRG, .D64, .G64:
            noteNewRecentlyExportedDiskURL(url, drive: nr)
        case .TAP:
            break
        default:
            break
        }
    }

    //
    // Creating media files from URLs
    //
    
    @available(*, deprecated, message: "MediaFileProxy will go away")
    static func createFileProxy(from url: URL, type: FileType) throws -> MediaFileProxy {
        
        return try createFileProxy(from: url, allowedTypes: [type])
    }
    
    @available(*, deprecated, message: "MediaFileProxy will go away")
    static func createFileProxy(from url: URL, allowedTypes: [FileType]) throws -> MediaFileProxy {
        
        debug(.media, "Reading file \(url.lastPathComponent)")
        
        // If the provided URL points to compressed file, decompress it first
        let newUrl = url.unpacked(maxSize: 2048 * 1024)
        
        // Iterate through all allowed file types
        for type in allowedTypes {
            
            do {
                return try MediaFileProxy.make(with: newUrl, type: type)
            } catch let error as AppError {
                if error.key != "FILE_TYPE_MISMATCH" { throw error }
            }
        }
        
        // None of the allowed types matched the file
        throw AppError(what: "The type of this file is not known to the emulator.")
    }

    //
    // Workspaces, Snapshots, Scripts
    //

    func loadWorkspace(url: URL, options: [Option] = []) throws {
        
        debug(.media, "url = \(url)")
        try emu?.c64.loadWorkspace(url: url)
    }
    
    func loadSnapshot(url: URL, options: [Option] = []) throws {
        
        debug(.media, "url = \(url)")
        try emu?.c64.loadSnapshot(url: url)
    }

    func runScript(url: URL, options: [Option] = []) throws {
        
        debug(.media, "url = \(url)")
        try emu?.retroShell.executeScript(url)
    }
    
        
    //
    // Flash items
    //
    
    func flashFile(url: URL, options: [Option] = []) throws {
        
        debug(.media, "url = \(url)")
        
        guard let emu = emu else { return }
        try emu.flash(url: url)
        
        mycontroller.keyboard.type("run\n")
        mycontroller.renderer.rotateLeft()
    }
    
    //
    // Floppy disks
    //

    func insertDisk(url: URL, drive n: Int, options: [Option] = [.remember]) throws {
        
        debug(.media, "url = \(url) n = \(n)")
        
        guard let emu = emu else { return }
        
        try emu.drive(n).insert(url: url, protected: options.contains(.protect))

        if options.contains(.remember) {
            MediaManager.noteNewRecentlyInsertedDiskURL(url)
        }
    }
    
    
    //
    // Tapes
    //

    func insertTape(url: URL, options: [Option] = [.remember]) throws {

        guard let emu = emu else { return }
        
        try emu.datasette.insertTape(url)
     
        if options.contains(.remember) {
            MediaManager.noteNewRecentlyInsertedTapeURL(url)
        }
                
        if options.contains(.autostart) {
            mycontroller.keyboard.type("LOAD\n")
            emu.datasette.pressPlay()
        }
    }
    
    
    //
    // Cartridges
    //
    
    func attachCartridge(url: URL, options: [Option] = [.remember, .reset]) throws {

        guard let emu = emu else { return }
        
        try emu.expansionport.attachCartridge(url, reset: options.contains(.reset))
     
        if options.contains(.remember) {
            MediaManager.noteNewRecentlyAtachedCartridgeURL(url)
        }
    }

    
    //
    // Wrapper for unspecified URLs
    //
    
    func process(url: URL) {
    
        do { try insertDisk(url: url, drive: DRIVE8); return } catch { }
        do { try attachCartridge(url: url); return } catch { }
        do { try insertTape(url: url); return } catch { }
        do { try loadWorkspace(url: url); return } catch { }
        do { try loadSnapshot(url: url); return } catch { }
        do { try runScript(url: url); return } catch { }
    }
        
    //
    // Exporting disks
    //
    
    func export(drive id: Int, to url: URL) throws {
        
        debug(.media, "drive: \(id) to: \(url)")
        guard let drive = emu?.drive(id) else { return }
                        
        if url.c64FileType == .G64 {
            
            let g64 = try MediaFileProxy.make(with: drive, type: .G64)
            try export(file: g64, to: url)
            
        } else {
            
            let fs = try OldFileSystemProxy.make(with: drive)
            try export(fs: fs, to: url)
        }
        
        emu?.put(.DSK_MODIFIED, value: id)
    }
    
    func export(fs: OldFileSystemProxy, to url: URL) throws {
        
        func showAlert(format: String) {
            
            let msg1 = "Only the first file will be exported."
            let msg2 = "The \(format) format is designed to store a single file."
            
            showMultipleFilesAlert(msg1: msg1, msg2: msg2)
        }
        
        debug(.media, "fs: \(fs) to: \(url)")
        
        var file: MediaFileProxy?
        
        switch url.c64FileType {
            
        case .D64:
            file = try MediaFileProxy.make(with: fs, type: .D64)
            
        case .T64:
            file = try MediaFileProxy.make(with: fs, type: .T64)
            
        case .PRG:
            if fs.numFiles > 1 { showAlert(format: "PRG") }
            file = try MediaFileProxy.make(with: fs, type: .PRG)
            
        case .P00:
            if fs.numFiles > 1 { showAlert(format: "P00") }
            file = try MediaFileProxy.make(with: fs, type: .P00)
            
        default:
            throw AppError(what: "File type mismatch.")
        }
        
        try export(file: file!, to: url)
    }
    
    func export(file: MediaFileProxy, to url: URL) throws {
        
        try file.writeToFile(url: url)
    }
}
