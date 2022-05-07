// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MediaManager {

    struct Option: OptionSet {

        let rawValue: Int

        static let force       = Option(rawValue: 1 << 0)
        static let remember    = Option(rawValue: 1 << 1)
        static let protect     = Option(rawValue: 1 << 2)
        static let reset       = Option(rawValue: 1 << 3)
        static let autostart   = Option(rawValue: 1 << 4)

    }

    // References to other objects
    var controller: MyController!
    var document: MyDocument!
    var c64: C64Proxy!

    // Computed references
    var console: Console { return controller.renderer.console }

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

    init(with controller: MyController) {

        debug(.lifetime, "Creating media manager")
        
        self.controller = controller
        self.document = controller.mydocument
        self.c64 = controller.c64
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

    //
    // Loading media files
    //

    func createFileProxy(from url: URL, allowedTypes: [FileType]) throws -> AnyFileProxy {

        debug(.media, "Reading file \(url.lastPathComponent)")

        // If the provided URL points to compressed file, decompress it first
        let newUrl = url.unpacked(maxSize: 2048 * 1024)

        // Iterate through all allowed file types
        for type in allowedTypes {

            do {
                switch type {

                case .SNAPSHOT:
                    return try SnapshotProxy.make(with: newUrl)

                case .SCRIPT:
                    return try ScriptProxy.make(with: newUrl)

                case .CRT:
                    return try CRTFileProxy.make(with: newUrl)

                case .D64:
                    return try D64FileProxy.make(with: newUrl)

                case .T64:
                    return try T64FileProxy.make(with: newUrl)

                case .PRG:
                    return try PRGFileProxy.make(with: newUrl)

                case .P00:
                    return try P00FileProxy.make(with: newUrl)

                case .G64:
                    return try G64FileProxy.make(with: newUrl)

                case .TAP:
                    return try TAPFileProxy.make(with: newUrl)

                case .FOLDER:
                    return try FolderProxy.make(with: newUrl)

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
    // Processing media files
    //

    func addMedia(url: URL,
                  allowedTypes types: [FileType],
                  drive id: Int = DRIVE8,
                  options: [Option] = [.remember]) throws {

        debug(.media, "url = \(url) types = \(types)")

        // Read file
        let file = try createFileProxy(from: url, allowedTypes: types)

        // Remember the URL if requested
        if options.contains(.remember) {

            switch file {

            case let file as SnapshotProxy:
                document.snapshots.append(file)

            case is CRTFileProxy:
                MediaManager.noteNewRecentlyAtachedCartridgeURL(url)

            case is TAPFileProxy:
                MediaManager.noteNewRecentlyInsertedTapeURL(url)

            case is D64FileProxy, is G64FileProxy, is AnyCollectionProxy:
                MediaManager.noteNewRecentlyInsertedDiskURL(url)

            default:
                break
            }
        }

        // Process file
        try addMedia(proxy: file, drive: c64.drive(id), options: options)
    }

    func addMedia(proxy: AnyFileProxy,
                  drive: DriveProxy? = nil,
                  options: [Option] = []) throws {

        var proceedUnsaved: Bool {
            return options.contains(.force) || proceedWithUnsavedFloppyDisk(drive: drive!)
        }

        switch proxy {

        case let proxy as SnapshotProxy:

            debug(.media, "Snapshot")
            try c64.flash(proxy)

        case let proxy as ScriptProxy:

            debug(.media, "Script")
            console.runScript(script: proxy)

        case let proxy as CRTFileProxy:

            debug(.media, "CRT")
            try c64.expansionport.attachCartridge(proxy, reset: true)

        case let proxy as TAPFileProxy:

            debug(.media, "TAP")
            c64.datasette.insertTape(proxy)

            if options.contains(.autostart) {
                controller.keyboard.type("LOAD\n")
                c64.datasette.pressPlay()
            }

        case let proxy as D64FileProxy:

            debug(.media, "D64")
            if proceedUnsaved {
                drive!.insertD64(proxy, protected: options.contains(.protect))
            }

        case let proxy as G64FileProxy:

            debug(.media, "G64")
            if proceedUnsaved {
                drive!.insertG64(proxy, protected: options.contains(.protect))
            }

        case let proxy as AnyCollectionProxy:

            debug(.media, "T64, PRG, P00")
            if proceedUnsaved {
                drive!.insertCollection(proxy, protected: options.contains(.protect))
            }

        default:
            fatalError()
        }

        /*
         func processSnapshotFile(_ proxy: SnapshotProxy, force: Bool = false) throws {

         try c64.flash(proxy)
         document.snapshots.append(proxy)
         }
         */
    }
}
