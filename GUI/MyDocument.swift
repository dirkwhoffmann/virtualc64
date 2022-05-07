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

    // The media manager for this document
    var mm: MediaManager { return parent.mm }

    // Gateway to the core emulator
    var c64: C64Proxy!

    // Snapshot storage
    private(set) var snapshots = ManagedArray<SnapshotProxy>(capacity: 32)

    //
    // Initializing
    //
    
    override init() {

        debug(.lifetime)

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
        
        debug(.lifetime)

        // Create the window controller
        let controller = MyController(windowNibName: "MyDocument")
        controller.c64 = c64
        self.addWindowController(controller)
    }

    //
    // Loading
    //

    override open func read(from url: URL, ofType typeName: String) throws {

        debug(.media)

        do {

            try mm.addMedia(url: url, allowedTypes: FileType.draggable)

        } catch let error as VC64Error {

            throw NSError(error: error)
        }
    }

    override open func revert(toContentsOf url: URL, ofType typeName: String) throws {

        debug(.media)

        do {
            try mm.addMedia(url: url, allowedTypes: [.SNAPSHOT])

        } catch let error as VC64Error {

            throw NSError(error: error)
        }
    }

    //
    // Saving
    //

    override func write(to url: URL, ofType typeName: String) throws {

        debug(.media)

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
    // Exporting disks
    //

    func export(drive id: Int, to url: URL) throws {
        
        debug(.media, "drive: \(id) to: \(url)")
        
        let drive = c64.drive(id)
        try export(disk: drive.disk, to: url)
        
        drive.markDiskAsUnmodified()
    }

    func export(disk: DiskProxy, to url: URL) throws {

        debug(.media, "disk: \(disk) to: \(url)")
        
        if url.c64FileType == .G64 {

            let g64 = try G64FileProxy.make(with: disk)
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

        var file: AnyFileProxy?

        switch url.c64FileType {

        case .D64:
            file = try D64FileProxy.make(with: fs)
            
        case .T64:
            file = try T64FileProxy.make(with: fs)
            
        case .PRG:
            if fs.numFiles > 1 { showAlert(format: "PRG") }
            file = try PRGFileProxy.make(with: fs)
            
        case .P00:
            if fs.numFiles > 1 { showAlert(format: "P00") }
            file = try P00FileProxy.make(with: fs)

        default:
            throw VC64Error(ErrorCode.FILE_TYPE_MISMATCH)
        }
        
        try export(file: file!, to: url)
    }
    
    func export(file: AnyFileProxy, to url: URL) throws {
        
        try file.writeToFile(url: url)
    }
}
