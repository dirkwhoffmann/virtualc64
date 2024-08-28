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
        static let flash       = Option(rawValue: 1 << 5)
    }

    // References to other objects
    var document: MyDocument!
    var emu: EmulatorProxy? { return document.emu }
    var controller: MyController { return document.parent }
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

    init(with document: MyDocument) {

        debug(.lifetime, "Creating media manager")
        self.document = document
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

    func createFileProxy(from url: URL, allowedTypes: [vc64.FileType]) throws -> MediaFileProxy {

        debug(.media, "Reading file \(url.lastPathComponent)")

        // If the provided URL points to compressed file, decompress it first
        let newUrl = url.unpacked(maxSize: 2048 * 1024)

        // Iterate through all allowed file types
        for type in allowedTypes {

            do {
                switch type {

                case .SNAPSHOT:
                    return try MediaFileProxy.make(with: newUrl, type: .SNAPSHOT)

                case .SCRIPT:
                    return try MediaFileProxy.make(with: newUrl, type: .SCRIPT)

                case .CRT:
                    return try MediaFileProxy.make(with: newUrl, type: .CRT)

                case .D64:
                    return try MediaFileProxy.make(with: newUrl, type: .D64)

                case .T64:
                    return try MediaFileProxy.make(with: newUrl, type: .T64)

                case .PRG:
                    return try MediaFileProxy.make(with: newUrl, type: .PRG)

                case .P00:
                    return try MediaFileProxy.make(with: newUrl, type: .P00)

                case .G64:
                    return try MediaFileProxy.make(with: newUrl, type: .G64)

                case .TAP:
                    return try MediaFileProxy.make(with: newUrl, type: .TAP)

                case .FOLDER:
                    return try MediaFileProxy.make(with: newUrl, type: .FOLDER)

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
    // Importing media files
    //

    func addMedia(url: URL,
                  allowedTypes types: [vc64.FileType] = vc64.FileType.all,
                  drive id: Int = DRIVE8,
                  options: [Option] = [.remember]) throws {

        debug(.media, "url = \(url) types = \(types)")

        if let emu = emu {

            // Read file
            let file = try createFileProxy(from: url, allowedTypes: types)

            // Remember the URL if requested
            if options.contains(.remember) {

                switch file.type {

                case .SNAPSHOT:
                    document.snapshots.append(file, size: file.size)

                case .CRT:
                    MediaManager.noteNewRecentlyAtachedCartridgeURL(url)

                case .TAP:
                    MediaManager.noteNewRecentlyInsertedTapeURL(url)

                case .T64, .P00, .PRG, .D64, .G64:
                    MediaManager.noteNewRecentlyInsertedDiskURL(url)

                default:
                    break
                }
            }

            // Process file
            if options.contains(.flash) {
                try flashMedia(proxy: file, options: options)
            } else {
                try addMedia(proxy: file, drive: emu.drive(id), options: options)
            }
        }
    }

    func addMedia(proxy: MediaFileProxy,
                  drive: DriveProxy? = nil,
                  options: [Option] = []) throws {

        var proceedUnsaved: Bool {
            return options.contains(.force) || proceedWithUnsavedFloppyDisk(drive: drive!)
        }

        if let emu = emu {

            switch proxy.type {

            case .SNAPSHOT:

                debug(.media, "Snapshot")
                try emu.flash(proxy)
                debug(.media, "Snapshot flashed")

            case .CRT:

                debug(.media, "CRT")
                try emu.expansionport.attachCartridge(proxy, reset: true)

            case .TAP:

                debug(.media, "TAP")
                emu.datasette.insertTape(proxy)

                if options.contains(.autostart) {
                    controller.keyboard.type("LOAD\n")
                    emu.datasette.pressPlay()
                }

            case .T64, .PRG, .P00, .D64, .G64, .FOLDER:

                debug(.media, "T64, PRG, P00, D64, G64, FOLDER")
                if proceedUnsaved {
                    drive!.insertMedia(proxy, protected: options.contains(.protect))
                }

            case .SCRIPT:

                debug(.media, "Script")
                console.runScript(script: proxy)

            default:
                break
            }
        }
    }

    func flashMedia(proxy: MediaFileProxy, options: [Option] = []) throws {

        if let emu = emu {

            switch proxy.type {

            case .SNAPSHOT:

                debug(.media, "Snapshot")
                try emu.flash(proxy)

            case .CRT:

                debug(.media, "CRT")
                try emu.expansionport.attachCartridge(proxy, reset: true)

            case .TAP:

                debug(.media, "TAP")
                emu.datasette.insertTape(proxy)

                if options.contains(.autostart) {
                    controller.keyboard.type("load\n")
                    emu.datasette.pressPlay()
                }

            case .PRG, .P00, .T64:

                debug(.media, "PRG, P00, T64")
                if let volume = try? FileSystemProxy.make(with: proxy) {

                    try? emu.flash(volume, item: 0)
                    controller.keyboard.type("run\n")
                    controller.renderer.rotateLeft()
                }

            case .SCRIPT:

                debug(.media, "Script")
                console.runScript(script: proxy)

            default:
                break
            }
        }
    }

    //
    // Exporting disks
    //

    func export(drive id: Int, to url: URL) throws {

        debug(.media, "drive: \(id) to: \(url)")

        if let emu = emu {
            
            let drive = emu.drive(id)
            try export(disk: drive, to: url)

            emu.put(.DSK_MODIFIED, value: id)
        }
    }

    func export(disk: DriveProxy, to url: URL) throws {

        debug(.media, "disk: \(disk) to: \(url)")

        if url.c64FileType == .G64 {

            let g64 = try MediaFileProxy.make(with: disk, type: .G64)
            try export(file: g64, to: url)

        } else {

            let fs = try FileSystemProxy.make(with: disk)
            try export(fs: fs, to: url)
        }
    }

    func export(fs: FileSystemProxy, to url: URL) throws {

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
            throw VC64Error(vc64.ErrorCode.FILE_TYPE_MISMATCH)
        }

        try export(file: file!, to: url)
    }

    func export(file: MediaFileProxy, to url: URL) throws {

        try file.writeToFile(url: url)
    }
}
